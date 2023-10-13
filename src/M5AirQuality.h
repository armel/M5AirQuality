// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Version
#define VERSION "1.0.1"
#define AUTHOR  "F4HWN"
#define NAME    "M5AirQuality"

#define RANDOM 1  // Set to 1 for random view
#define CORE   1
#define CORE2  2
#define CORES3 3

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

// Variables
uint16_t brightness  = BRIGHTNESS;
uint16_t batteryLevelOld = 1024;
uint16_t colorOffset = 16 + 64 * 0 + 8 * 0 + 32;

boolean batteryCharginglOld = true;
float temperatureOffset;