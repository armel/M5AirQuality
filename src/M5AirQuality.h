// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Version
#define VERSION "1.2.0"
#define AUTHOR  "F4HWN"
#define NAME    "M5AirQuality"

#define RANDOM 1  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

// Config
#define CELCIUS          1
#define FAHRENHEIT       2
#define I2C_PORT_A       0
#define I2C_PORT_C_M5GO2 1
#define I2C_PORT_C_M5GO3 2

// Your config here
#define I2C  I2C_PORT_A  // Set I2C Port (I2C_PORT_A, I2C_PORT_C_M5GO2 or I2C_PORT_C_M5GO3)
#define UNIT CELCIUS     // Set UNIT to CELCIUS or FAHRENHEIT
#define LED  1200        // Set to 0 (LEDs are on) or to a certain level (otherwise the LEDs are off)

#define TFT_SCREEN_BG M5.Displays(0).color565(32, 32, 32)

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

// Sensor SCD4x
const int16_t SCD_ADDRESS = 0x62;

// Sprite
LGFX_Sprite measureSprite(&M5.Display);

// Variables
uint16_t brightness      = BRIGHTNESS;
uint16_t batteryLevelOld = 1024;

boolean batteryCharginglOld = true;

float temperatureOffset;

CRGB m5goColor = CRGB::Blue;