// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "M5AirQuality.h"
#include "tools.h"
#include "images.h"
#include "font.h"
#include "functions.h"
#include "menu.h"
#include "tasks.h"

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
  alert      = preferences.getUInt("alert", ALERT);
  unit       = preferences.getUInt("unit", CELCIUS);
  port       = preferences.getUInt("port", I2C_PORT_A);
  beep       = preferences.getUInt("beep", BEEP);

  // Init display
  display = hdmi;
  M5.setPrimaryDisplay(display);
  M5.Displays(display).setBrightness(map(brightness, 1, 100, 1, 254));

  // For models with EPD : refresh control
  M5.Displays(display).setEpdMode(epd_mode_t::epd_fastest);  // fastest but very-low quality.

  offsetX = (M5.Displays(display).width() - 320) / 2;
  offsetY = (M5.Displays(display).height() - 240) / 2;

  // Clean settings
  settings = cleanSettings();

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

  // Sprite
  measureSprite.setColorDepth(24);
  measureSprite.createSprite(26, 40);
  tempSprite.setColorDepth(24);
  tempSprite.createSprite(90, 40);

  // view UI
  viewGUI();

  // view battery
  viewBattery();

  // Init Led
  initLed();

  // Get temperature offset
  // temperatureOffset = getTemperatureOffset();

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
  static float co2Old         = 0;
  static float temperatureOld = 0;
  static float humidityOld    = 0;
  static int16_t co2Last[20]  = {1000};
  int16_t co2Max              = 0;
  int16_t co2Min              = 10000;
  char strTempInt[8]          = "25";
  char strTempFloat[8]        = "25.0";
  char strHumidityFloat[8]    = "50";
  uint8_t shift               = 0;

  uint8_t data[12], counter;

  size_t n = sizeof(co2Last) / sizeof(co2Last[0]);

  Serial.println(n);

  // view battery
  viewBattery();

  if (settingsMode == false) {
    settingLock = true;
    if (resetSensor == true) {
      // Reinit Sensor
      initSensor();
      resetSensor = false;
    }

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

    // temperature = random(-10, 90);

    if (unit == FAHRENHEIT) {
      temperature = (temperature * 1.8) + 32;
    }

    if (temperature < -9 || temperature > 99) {
      sprintf(strTempInt, "%3d", (int)temperature);
    } else {
      sprintf(strTempInt, "%2d", (int)temperature);
      shift = 10;
    }

    sprintf(strTempFloat, "%.2f", temperature);

    Serial.println(strTempFloat);
    Serial.println(strTempInt);

    // Convert RH in %
    humidity = 100 * (float)((uint16_t)data[6] << 8 | data[7]) / 65535;

    sprintf(strHumidityFloat, "%.2f", humidity);

    Serial.printf("co2 %02f, temperature %02f, temperature offset %02f, humidity %02f\n", co2, temperature,
                  temperatureOffset, humidity);

    if (temperature > -50) {
      // View result

      // co2 = random(600, 1200);

      for (uint8_t i = 0; i < (n - 1); i++) {
        co2Last[i] = co2Last[i + 1];
      }
      co2Last[(n - 1)] = int(co2);

      // View co2
      M5.Displays(display).setFont(&digital_7__mono_24pt7b);
      M5.Displays(display).setTextDatum(CL_DATUM);

      if (int(co2) < 1000) {
        M5.Displays(display).setTextPadding(120);
      } else {
        M5.Displays(display).setTextPadding(150);
      }

      M5.Displays(display).setTextColor(TFT_PINK, TFT_SCREEN_BG);
      M5.Displays(display).drawString(String(int(co2)), 90, 46);

      // View + or - and legend
      measureSprite.clear();
      measureSprite.fillRect(0, 0, 26, 40, TFT_SCREEN_BG);
      measureSprite.setFont(&digital_7__mono_24pt7b);
      measureSprite.setTextColor(TFT_PINK);

      if (co2Old < co2) {
        measureSprite.drawString("+", 2, 4);
      } else if (co2Old > co2) {
        measureSprite.drawString("-", 2, 4);
      } else {
        measureSprite.drawString("=", 2, 4);
      }
      co2Old = co2;

      measureSprite.setFont(&arial6pt7b);
      measureSprite.setTextColor(TFT_PINK);
      measureSprite.drawString("ppm", 0, 0);

      if (co2 < 1000) {
        measureSprite.pushSprite(165, 30, TFT_TRANSPARENT);
      } else {
        measureSprite.pushSprite(185, 30, TFT_TRANSPARENT);
      }

      for (uint8_t i = 0; i < n; i++) {
        if (co2Last[i] != 0) {
          co2Max = max(co2Last[i], co2Max);
          co2Min = min(co2Last[i], co2Min);
        }
      }

      for (uint8_t i = 0; i < n; i++) {
        uint8_t j = map(co2Last[i], co2Min, co2Max, 1, 40);

        if (j > 40) {
          j = 40;
        }

        // Serial.printf("%d %d %d %d\n", co2Min, co2Max, co2Last[i], j);
        M5.Displays(display).drawFastVLine(220 + (i * 4), 62 - 40, 40, TFT_SCREEN_BG);
        M5.Displays(display).drawFastVLine(220 + (i * 4) + 1, 62 - 40, 40, TFT_SCREEN_BG);

        if (co2Last[i] != 0) {
          M5.Displays(display).drawFastVLine(220 + (i * 4), 62 - j, j,
                                             M5.Displays(display).color565(255, 128 - (i * 4), 128 - (i * 4)));
          M5.Displays(display).drawFastVLine(220 + (i * 4) + 1, 62 - j, j,
                                             M5.Displays(display).color565(255, 128 - (i * 4), 128 - (i * 4)));
        }
      }

      M5.Displays(display).fillRect(220, 63, 78, 1, M5.Displays(display).color565(255, 128, 128));

      // View temperature
      M5.Displays(display).setFont(&arial6pt7b);
      M5.Displays(display).setTextPadding(60);
      M5.Displays(display).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
      M5.Displays(display).drawString(strTempFloat, 90, 218);

      tempSprite.clear();
      tempSprite.fillRect(0, 0, 90, 40, TFT_SCREEN_BG);
      tempSprite.setFont(&digital_7__mono_24pt7b);
      tempSprite.setTextPadding(60);
      tempSprite.setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
      tempSprite.drawString(strTempInt, 0 + shift, 0);
      tempSprite.pushSprite(80, 170, TFT_TRANSPARENT);

      // View + or - and legend
      measureSprite.clear();
      measureSprite.fillRect(0, 0, 26, 40, TFT_SCREEN_BG);
      measureSprite.setFont(&digital_7__mono_24pt7b);
      measureSprite.setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);

      if (temperatureOld < temperature) {
        measureSprite.drawString("+", 2, 4);
      } else if (temperatureOld > temperature) {
        measureSprite.drawString("-", 2, 4);
      } else {
        measureSprite.drawString("=", 2, 4);
      }
      temperatureOld = temperature;

      measureSprite.setFont(&arial6pt7b);
      measureSprite.setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
      measureSprite.drawString("o", 0, 0);
      if (unit == FAHRENHEIT) {
        measureSprite.drawString("F", 8, 5);
        measureSprite.pushSprite(150 - shift, 174, TFT_TRANSPARENT);
      } else {
        measureSprite.drawString("C", 8, 5);
        measureSprite.pushSprite(150 - shift, 174, TFT_TRANSPARENT);
      }

      // View humidity
      M5.Displays(display).setFont(&arial6pt7b);
      M5.Displays(display).setTextPadding(44);
      M5.Displays(display).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
      M5.Displays(display).drawString(strHumidityFloat, 250, 218);

      M5.Displays(display).setFont(&digital_7__mono_24pt7b);
      M5.Displays(display).drawString(String(int(humidity)), 250, 190);

      // View + or - and legend
      measureSprite.clear();
      measureSprite.fillRect(0, 0, 26, 40, TFT_SCREEN_BG);
      measureSprite.setFont(&digital_7__mono_24pt7b);
      measureSprite.setTextColor(TFT_ORANGE, TFT_SCREEN_BG);

      if (humidityOld < humidity) {
        measureSprite.drawString("+", 2, 4);
      } else if (humidityOld > humidity) {
        measureSprite.drawString("-", 2, 4);
      } else {
        measureSprite.drawString("=", 2, 4);
      }
      humidityOld = humidity;

      measureSprite.setFont(&arial6pt7b);
      measureSprite.setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
      measureSprite.drawString("%", 0, 2);

      measureSprite.pushSprite(300, 174, TFT_TRANSPARENT);

      // Bar
      M5.Displays(display).fillRect(0, 100, 320, 2, TFT_SCREEN_BG);

      if (co2 < 1000) {
        M5.Displays(display).fillRect(16 + 64 * 0 + 8 * 0, 100, 64, 2, TFT_WHITE);
      } else if (co2 < 2000) {
        M5.Displays(display).fillRect(16 + 64 * 1 + 8 * 1, 100, 64, 2, TFT_WHITE);
      } else if (co2 < 3000) {
        M5.Displays(display).fillRect(16 + 64 * 2 + 8 * 2, 100, 64, 2, TFT_WHITE);
      } else {
        M5.Displays(display).fillRect(16 + 64 * 3 + 8 * 3, 100, 64, 2, TFT_WHITE);
      }

      if (alert == 0) {
        if (co2 < 1000) {
          m5goColor = CRGB::Green;
        } else if (co2 < 2000) {
          m5goColor = CRGB::Yellow;
        } else if (co2 < 3000) {
          m5goColor = CRGB::Orange;
        } else {
          m5goColor = CRGB::Red;
        }
      } else {
        if (co2 >= alert) {
          m5goColor = CRGB::Red;
        } else {
          m5goColor = CRGB::Black;
        }
      }
    }
    settingLock = false;
    // Wait for next measurement
    delay(5000);
  }
}
