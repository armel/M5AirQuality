// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// View UI
void viewUI() {
  M5.Displays(0).setBrightness(brightness);

  M5.Displays(0).fillScreen(TFT_SCREEN_BG);
  M5.Displays(0).drawPng(co2, sizeof(co2), 16, 16, 64, 64);
  M5.Displays(0).drawPng(temperature, sizeof(temperature), 16, 160, 64, 64);
  M5.Displays(0).drawPng(humidity, sizeof(humidity), 176, 160, 64, 64);

  M5.Displays(0).fillRect(16 + 64 * 0 + 8 * 0, 110, 64, 10, TFT_GREEN);
  M5.Displays(0).fillRect(16 + 64 * 1 + 8 * 1, 110, 64, 10, TFT_YELLOW);
  M5.Displays(0).fillRect(16 + 64 * 2 + 8 * 2, 110, 64, 10, TFT_ORANGE);
  M5.Displays(0).fillRect(16 + 64 * 3 + 8 * 3, 110, 64, 10, TFT_RED);

  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CC_DATUM);
  M5.Displays(0).setTextPadding(20);

  M5.Displays(0).drawString("GOOD", 50, 130);
  M5.Displays(0).drawString("OK", 155, 130);
  M5.Displays(0).drawString("BAD", 265, 130);

  M5.Displays(0).setTextDatum(CL_DATUM);
  M5.Displays(0).setTextColor(TFT_PINK, TFT_SCREEN_BG);
  M5.Displays(0).drawString("CO2 Air Quality", 90, 21);
  M5.Displays(0).setTextColor(TFT_SKYBLUE, TFT_SCREEN_BG);
  M5.Displays(0).drawString("Temperature", 90, 165);
  M5.Displays(0).setTextColor(TFT_ORANGE, TFT_SCREEN_BG);
  M5.Displays(0).drawString("Humidity", 250, 165);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).drawString("V" + String(VERSION) + " by " + String(AUTHOR), 222, 232);
}

// View battery
void viewBattery() {
  char buf[8];

  uint8_t batteryLevel;
  boolean batteryCharging;

  // On left, view battery level
  batteryLevel    = map(getBatteryLevel(), 0, 100, 0, 17);
  batteryCharging = isCharging();

  // Serial.printf(">>> %d %d %d %d\n", batteryLevel, batteryLevelOld,
  // batteryCharging, batteryCharginglOld);

  if (batteryLevel != batteryLevelOld || batteryCharging != batteryCharginglOld) {
    // M5.Displays(display).drawFastHLine(0, 20, 320, TFT_BLACK);

    batteryLevelOld     = batteryLevel;
    batteryCharginglOld = batteryCharging;

    M5.Displays(0).drawRect(293, 4, 21, 12, TFT_WHITE);
    M5.Displays(0).drawRect(313, 7, 4, 6, TFT_WHITE);

    for (uint8_t i = 0; i < batteryLevel; i++) {
      M5.Displays(0).drawGradientVLine(295 + i, 6, 8, TFT_GREEN, TFT_DARKGREEN);
    }

    M5.Displays(0).drawFastVLine(300, 5, 10, TFT_SCREEN_BG);
    M5.Displays(0).drawFastVLine(306, 5, 10, TFT_SCREEN_BG);

    if (batteryCharging) {
      M5.Displays(0).setTextColor(TFT_WHITE);
      M5.Displays(0).setFont(&arial6pt7b);
      M5.Displays(0).setTextDatum(CC_DATUM);
      M5.Displays(0).setTextPadding(0);
      snprintf(buf, 8, "%s", "+");
    } else {
      M5.Displays(0).setTextColor(TFT_WHITE);
      M5.Displays(0).setFont(0);
      M5.Displays(0).setTextDatum(CR_DATUM);
      M5.Displays(0).setTextPadding(0);
      snprintf(buf, 8, "%d%s", getBatteryLevel(), "%");
    }
    M5.Displays(0).drawString(buf, 290, 11);
  }
}

// Init Led
void initLed() {
  for (uint8_t j = 0; j < NUM_LEDS; j++) {
    leds[j] = CRGB::Black;
  }
  FastLED.setBrightness(16);
  FastLED.show();
}

// Init sensor SCD4x
void initSensor() {
  Serial.begin(115200);
  while (!Serial)
    ;

  // Init I2C
  Wire.begin();        // Port A
  //Wire.begin(14, 13);  // Port C available on M5GO2 for Core2

  // Wait until sensors are ready, > 1000 ms according to datasheet
  delay(1000);

  // Start SCD4x measurement in periodic mode, will update every 5s
  Wire.beginTransmission(SCD_ADDRESS);
  Wire.write(0x21);
  Wire.write(0xb1);
  Wire.endTransmission();

  // Wait for first measurement to be finished
  M5.Displays(0).setFont(&arial6pt7b);
  M5.Displays(0).setTextColor(TFT_WHITE, TFT_SCREEN_BG);
  M5.Displays(0).setTextDatum(CC_DATUM);
  M5.Displays(0).setTextPadding(180);

#if BOARD != CORES3
  for (uint8_t i = 0; i < 5; i++) {
    leds[4 - i] = CRGB::DarkBlue;
    leds[5 + i] = CRGB::DarkBlue;

    FastLED.setBrightness(16);
    FastLED.show();

    M5.Displays(0).drawString("Please wait - Init CO2 Sensor", 160, 90);
    delay(500);
    M5.Displays(0).drawString("", 160, 90);
    delay(500);
  }
#else
  for (uint8_t i = 0; i < 5; i++) {
    M5.Displays(0).drawString("Please wait - Init CO2 Sensor", 160, 90);
    delay(500);
    M5.Displays(0).drawString("", 160, 90);
    delay(500);
  }
#endif
}

// Get temperature offset
float getTemperatureOffset() {
  uint8_t data[12], counter;
  counter = 0;

  Wire.beginTransmission(SCD_ADDRESS);
  Wire.write(0x23);
  Wire.write(0x16);
  Wire.endTransmission();

  Wire.requestFrom(SCD_ADDRESS, 12);

  while (Wire.available()) {
    data[counter++] = Wire.read();
  }

  // Wait for first measurement to be finished
  delay(1000);

  // Return result
  return 175 * (float)((uint16_t)data[0] << 8 | data[1]) / 65536;
}

// Fade all led
void fadeall() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(100);
  }
}

// Led task
void led(void *pvParameters) {
  for (;;) {
    // First slide the led in one direction
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[i] = m5goColor;
      ;
      // Show the leds
      FastLED.show();
      fadeall();
      vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Now go in the other direction
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      leds[(NUM_LEDS)-1 - i] = m5goColor;
      ;
      FastLED.show();
      fadeall();
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

// Check button
void button(void *pvParameters) {
  uint8_t step = 2;
  uint8_t min  = 4;
  uint8_t max  = 255;

  uint8_t btnA = 0;
  uint8_t btnB = 0;
  uint8_t btnC = 0;

  uint16_t brightnessOld = 0;

  struct Button {
    char name[16];   // name
    int x;           // x
    int y;           // y
    int w;           // width
    int h;           // height
    int d;           // distance
    boolean active;  // active, if true, check this button, else bypass
    boolean read;    // read, if true, button is push, else false
  };

#if BOARD == CORES3
  Button myBtn[] = {
    {"myBtnA", 0, 160, 100, 80, 1000, true, false},
    {"myBtnB", 110, 160, 100, 80, 1000, true, false},
    {"myBtnC", 220, 160, 100, 80, 1000, true, false},
  };
#else
  Button myBtn[] = {
    {"myBtnA", 0, 240, 100, 80, 1000, true, false},
    {"myBtnB", 110, 240, 100, 80, 1000, true, false},
    {"myBtnC", 220, 240, 100, 80, 1000, true, false},
  };
#endif

  for (;;) {
    M5.update();

    if (M5.getBoard() == m5::board_t::board_M5Stack || M5.getBoard() == m5::board_t::board_M5StackCore2) {
      step = 4;
      min  = 4;
      max  = 255;

      btnA = M5.BtnA.isPressed();
      btnB = M5.BtnB.isPressed();
      btnC = M5.BtnC.isPressed();
    } else if (M5.getBoard() == m5::board_t::board_M5StackCoreS3) {
      step = 16;
      min  = 64;
      max  = 255;

      auto t = M5.Touch.getDetail();
      auto c = M5.Touch.getCount();

      uint8_t limit = sizeof myBtn / sizeof myBtn[0];

      int distanceBtn     = 0;
      int distanceMin     = 1000;
      int distanceCurrent = 1000;

      if (c == 1) {
        for (uint8_t i = 0; i < limit; i++) {
          myBtn[i].read = false;
          if (myBtn[i].active == true) {
            distanceCurrent = (int)(sqrt(pow(t.x - (myBtn[i].x + (myBtn[i].w / 2)), 2) +
                                         pow(t.y - (myBtn[i].y + (myBtn[i].h / 2)), 2)));
            myBtn[i].d      = distanceCurrent;
            if (distanceCurrent < distanceMin) {
              distanceMin = distanceCurrent;
              distanceBtn = i;
            }
          }
        }

        if (t.state == 1 || t.state == 3 || t.state == 5) {
          myBtn[distanceBtn].read = true;
        }
      }

      btnA = myBtn[0].read;
      btnB = myBtn[1].read;
      btnC = myBtn[2].read;
    }

    if (btnA || btnC) {
      if (M5.getBoard() == m5::board_t::board_M5StackCoreS3) {
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      brightnessOld = brightness;

      if (btnA) {
        brightnessOld -= step;
        brightnessOld = (brightnessOld <= min) ? min : brightnessOld;
      } else if (btnC) {
        brightnessOld += step;
        brightnessOld = (brightnessOld >= max) ? max : brightnessOld;
      }

      if (brightnessOld != brightness) {
        brightness = brightnessOld;
        M5.Displays(0).setBrightness(brightness);
        preferences.putUInt("brightness", brightness);
        Serial.println(brightness);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}