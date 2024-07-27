// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "M5AirQuality.h"
#include "tools.h"
#include "images.h"
#include "font.h"
#include "functions.h"

// Setup
void setup() {
  // Init M5
  auto cfg = M5.config();

  cfg.clear_display = true;   // default=true. clear the screen when begin.
  cfg.internal_imu  = true;   // default=true. use internal IMU.
  cfg.internal_rtc  = true;   // default=true. use internal RTC.
  cfg.internal_spk  = true;   // default=true. use internal speaker.
  cfg.internal_mic  = true;   // default=true. use internal microphone.
  cfg.external_imu  = false;  // default=false. use Unit Accel & Gyro.
  cfg.external_rtc  = false;  // default=false. use Unit RTC.

  cfg.external_display.module_display = true;   // default=true. use ModuleDisplay
  cfg.external_display.atom_display   = true;   // default=true. use AtomDisplay
  cfg.external_display.unit_oled      = false;  // default=true. use UnitOLED
  cfg.external_display.unit_lcd       = false;  // default=true. use UnitLCD
  cfg.external_display.unit_rca       = false;  // default=false. use UnitRCA VideoOutput
  cfg.external_display.module_rca     = false;  // default=false. use ModuleRCA VideoOutput

  M5.begin(cfg);

  // Init Preferences
  preferences.begin(NAME);
  brightness = preferences.getUInt("brightness", BRIGHTNESS);
  Serial.printf("Brightness = %d\n", brightness);

  // Init Leds
#if BOARD != CORES3
  if (M5.getBoard() == m5::board_t::board_M5Stack) {
    FastLED.addLeds<NEOPIXEL, 15>(leds,
                                  NUM_LEDS);  // GRB ordering is assumed
  } else if (M5.getBoard() == m5::board_t::board_M5StackCore2) {
    FastLED.addLeds<NEOPIXEL, 25>(leds,
                                  NUM_LEDS);  // GRB ordering is assumed
  }
  FastLED.setBrightness(16);
#else
  FastLED.addLeds<NEOPIXEL, 5>(leds,
                                  NUM_LEDS);  // GRB ordering is assumed
  FastLED.setBrightness(32);
#endif

  // view UI
  viewUI();

  // view battery
  viewBattery();

  // Init Led
  initLed();

  // Get temperature offset
  // temperatureOffset = getTemperatureOffset();

  // Init Sensor
  initSensor();

  // Multitasking task for retreive button
  xTaskCreatePinnedToCore(button,    // Function to implement the task
                          "button",  // Name of the task
                          8192,      // Stack size in words
                          NULL,      // Task input parameter
                          4,         // Priority of the task
                          NULL,      // Task handle
                          1);        // Core where the task should run

  xTaskCreatePinnedToCore(led,    // Function to implement the task
                          "led",  // Name of the task
                          1024,   // Stack size in words
                          NULL,   // Task input parameter
                          4,      // Priority of the task
                          NULL,   // Task handle
                          1);     // Core where the task should run
}

// Main loop
void loop() {
  float co2, temperature, humidity;
  uint8_t data[12], counter;

  // view battery
  viewBattery();

  // Send read data command
  Wire.beginTransmission(SCD_ADDRESS);
  Wire.write(0xec);
  Wire.write(0x05);
  Wire.endTransmission();

  Wire.requestFrom(SCD_ADDRESS, 12);
  counter = 0;
  while (Wire.available()) {
    data[counter++] = Wire.read();
  }

  // Floating point conversion according to datasheet
  co2 = (float)((uint16_t)data[0] << 8 | data[1]);
  // Convert T in deg C
  temperature = -45 + 175 * (float)((uint16_t)data[3] << 8 | data[4]) / 65535 - temperatureOffset;
  // Convert RH in %
  humidity = 100 * (float)((uint16_t)data[6] << 8 | data[7]) / 65535;

  Serial.printf("co2 %02f, temperature %02f, temperature offset %02f, humidity %02f\n", co2, temperature,
                temperatureOffset, humidity);

  if (temperature > -10) {
    // View result
    M5.Displays(0).setFont(&digital_7__mono_24pt7b);
    M5.Displays(0).setTextDatum(CL_DATUM);

    M5.Displays(0).setTextPadding(160);
    M5.Displays(0).setTextColor(TFT_PINK, TFT_SCREEN_BG);
    M5.Displays(0).drawString(String(int(co2)), 90, 46);

    M5.Displays(0).setTextPadding(40);
    M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);

    if(int(temperature) < 100) // Prevent display bug
    {
      M5.Displays(0).drawString(String(int(temperature)), 90, 190);
    }

    M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
    M5.Displays(0).drawString(String(int(humidity)), 250, 190);

    M5.Displays(0).setFont(&arial6pt7b);
    M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
    M5.Displays(0).setTextDatum(CL_DATUM);
    M5.Displays(0).setTextPadding(20);

    M5.Displays(0).setTextColor(TFT_PINK, TFT_SCREEN_BG);
    if (co2 < 1000) {
      M5.Displays(0).drawString("ppm", 165, 36);
    } else {
      M5.Displays(0).drawString("ppm", 185, 36);
    }

    M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
    M5.Displays(0).drawString("o", 140, 180);
    M5.Displays(0).drawString("C", 148, 185);

    M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
    M5.Displays(0).drawString("%", 300, 182);

    M5.Displays(0).fillRect(0, 100, 320, 2, TFT_SCREEN_BG);

    if (co2 < 1000) {
      M5.Displays(0).fillRect(16 + 64 * 0 + 8 * 0, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Green;
    } else if (co2 < 2000) {
      M5.Displays(0).fillRect(16 + 64 * 1 + 8 * 1, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Yellow;
    } else if (co2 < 3000) {
      M5.Displays(0).fillRect(16 + 64 * 2 + 8 * 2, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Orange;
    } else {
      M5.Displays(0).fillRect(16 + 64 * 3 + 8 * 3, 100, 64, 2, TFT_WHITE);
      m5goColor = CRGB::Blue;
    }
  }

  // Wait for next measurement
  delay(5000);
}
