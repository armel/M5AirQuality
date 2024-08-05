// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Version
#define VERSION "1.3.0"
#define AUTHOR  "F4HWN"
#define NAME    "M5AirQuality"

#define RANDOM 1  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

#if not defined(BOARD)
#define BOARD 2
#endif

#if not defined(WIDTH)
#define WIDTH 320
#endif

#if not defined(HEIGHT)
#define HEIGHT 240
#endif

#define TIMEOUT_MENU 10 * 1000  // 10 sec

#define M5MODULEDISPLAY_LOGICAL_WIDTH  WIDTH   // Width
#define M5MODULEDISPLAY_LOGICAL_HEIGHT HEIGHT  // Height
#define M5MODULEDISPLAY_REFRESH_RATE   60      // Refresh rate
#define M5MODULEDISPLAY_OUTPUT_WIDTH   960
#define M5MODULEDISPLAY_OUTPUT_HEIGHT  480

#define TFT_MENU_BORDER M5.Displays(display).color565(88, 24, 69)
#define TFT_MENU_BACK   M5.Displays(display).color565(130, 0, 65)
#define TFT_MENU_SELECT M5.Displays(display).color565(255, 255, 255)
#define TFT_SCREEN_BG   M5.Displays(display).color565(32, 32, 32)

// Config
#define ALERT            0
#define CELCIUS          0
#define FAHRENHEIT       1
#define I2C_PORT_A       0
#define I2C_PORT_C_M5GO2 1
#define I2C_PORT_C_M5GO3 2
#define BEEP             20
#define BRIGHTNESS       48

// Dependencies
#include <Preferences.h>
#include <LittleFS.h>
#include <SD.h>
#include <Wire.h>
#include <FastLED.h>
#include <M5ModuleDisplay.h>
#include <M5Unified.h>

// Preferences
Preferences preferences;

// LED
#define FASTLED_INTERNAL  // To disable pragma messages on compile
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];
CRGB m5goColor = CRGB::Blue;

// Sensor SCD4x
const int16_t SCD_ADDRESS = 0x62;

// Display
uint16_t offsetX = 0;
uint16_t offsetY = 0;

// Sprite
LGFX_Sprite measureSprite(&M5.Display);
LGFX_Sprite tempSprite(&M5.Display);

// Variables
float temperatureOffset;

uint16_t brightness      = 0;
uint16_t batteryLevelOld = 1024;

int8_t beep      = 50;
int8_t display   = 0;
int8_t hdmi      = 0;
int8_t unit      = 0;
int8_t unitOld   = 255;
int8_t port      = 0;
int8_t portOld   = 255;
int16_t alert    = 0;
int16_t alertOld = 0;

boolean resetSensor         = true;
boolean batteryCharginglOld = true;
boolean settingsMode        = false;
boolean settingLock         = false;
boolean ESC                 = false;

// Menu
char *settingsStandard[] = {(char *)"Temperature Unit", (char *)"Led Alert", (char *)"Brightness",
                            (char *)"Sensor Port",      (char *)"Beep",      (char *)"Shutdown",
                            (char *)"Reboot",           (char *)"HDMI",      (char *)"Exit"};

int settingsLength;
char **settings;

const char *choiceUnit[]       = {"CELCIUS", "FAHRENHEIT"};
const char *choiceLedAlert[]   = {"CO2"};
const char *choiceBrightness[] = {"BRIGHTNESS"};
const char *choicePort[]       = {"PORT A", "PORT C / M5GO2", "PORT C / M5GO3"};
const char *choiceBeep[]       = {"BEEP LEVEL"};
const char *choiceHDMI[]       = {"OFF", "ON"};
