// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

// Get Button
void button(void *pvParameters) {
  uint8_t brightnessOld         = 64;
  int8_t beepOld                = 0;
  static int8_t settingsChoice  = 0;
  static boolean settingsSelect = false;

  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;

  static uint32_t timer = 0;

  for (;;) {
    // Position
    x = 44 + offsetX;
    y = 4 + offsetY;
    w = M5.Displays(display).width() - (x * 2);
    h = 185;

    // Get button
    getButton();

    if (btnA || btnB || btnC) {
      timer         = millis();
      brightnessOld = preferences.getUInt("brightness");
      alertOld      = preferences.getUInt("alert");
      portOld       = preferences.getUInt("port");
    }

    if (settingsMode == false) {
      // Enter settings
      if (btnB) {
        settingsMode = true;
        while (settingLock == true) {
          vTaskDelay(10);
        }
        viewMenu(x, y, w, h);
        viewOption(settingsChoice, settingsSelect, x, y, w);
        vTaskDelay(300);
      }
      vTaskDelay(100);
    }
    // Select settings
    else if (settingsMode == true) {
      // Sortie automatique si aucune action
      if ((millis() - timer) > TIMEOUT_MENU) {
        clearData();
        viewGUI();
        settingsSelect = false;
        settingsMode   = false;
        vTaskDelay(pdMS_TO_TICKS(150));
      }

      if (settingsSelect == false) {
        if (btnA || btnC) {
          if (btnA) {
            settingsChoice--;
          } else if (btnC) {
            settingsChoice++;
          }

          size_t stop = settingsLength - 1;

          settingsChoice = (settingsChoice < 0) ? stop : settingsChoice;
          settingsChoice = (settingsChoice > stop) ? 0 : settingsChoice;

          viewOption(settingsChoice, settingsSelect, x, y, w);
        } else if (btnB) {
          settingsSelect = true;
          viewOption(settingsChoice, settingsSelect, x, y, w);

          String settingsString = String(settings[settingsChoice]);
          if (settingsString == "Shutdown") {
            shutdown();
          } else if (settingsString == "Reboot") {
            reboot();
          } else if (settingsString == "Exit") {
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
      }
      // Manage settings
      else if (settingsSelect == true) {
        String settingsString = String(settings[settingsChoice]);

        M5.Displays(display).setTextDatum(CC_DATUM);
        M5.Displays(display).setFont(&YELLOWCRE8pt7b);
        M5.Displays(display).setTextPadding(w - 2);
        M5.Displays(display).setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);

        // Temperature Unit Values
        if (settingsString == "Temperature Unit") {
          M5.Displays(display).drawString(String(choiceUnit[unit]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA == 1) {
              unit -= 1;
              if (unit < 0) {
                unit = 1;
              }
            } else if (btnC == 1) {
              unit += 1;
              if (unit > 1) {
                unit = 0;
              }
            }
          } else if (btnB == 1) {
            if (unitOld != unit) preferences.putUInt("unit", unit);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(100));
        }

        // Led Alter
        if (settingsString == "Led Alert") {
          M5.Displays(display).drawString(String(choiceLedAlert[0]) + " " + String(alert) + " PPM", 160 + offsetX,
                                          h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA == 1) {
              alert -= 10;
              if (alert < 0) {
                alert = 0;
              }
            } else if (btnC == 1) {
              alert += 10;
              if (alert > 5000) {
                alert = 5000;
              }
            }
          } else if (btnB == 1) {
            if (alertOld != alert) preferences.putUInt("alert", alert);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(75));
        }

        // Brightness
        if (settingsString == "Brightness") {
          M5.Displays(display).drawString(String(choiceBrightness[0]) + " " + String(brightness) + "%", 160 + offsetX,
                                          h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA == 1) {
              brightness -= 1;
              if (brightness < 1) {
                brightness = 1;
              }
            } else if (btnC == 1) {
              brightness += 1;
              if (brightness > 100) {
                brightness = 100;
              }
            }
          } else if (btnB == 1) {
            if (brightnessOld != brightness) preferences.putUInt("brightness", brightness);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          M5.Lcd.setBrightness(map(brightness, 1, 100, 1, 254));
          vTaskDelay(pdMS_TO_TICKS(75));
        }

        // Sensor Port Values
        if (settingsString == "Sensor Port") {
          M5.Displays(display).drawString(String(choicePort[port]), 160 + offsetX, h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA == 1) {
              port -= 1;
              if (port < 0) {
                port = 2;
              }
            } else if (btnC == 1) {
              port += 1;
              if (port > 2) {
                port = 0;
              }
            }
          } else if (btnB == 1) {
            if (portOld != port) preferences.putUInt("port", port);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          resetSensor = true;
          vTaskDelay(pdMS_TO_TICKS(100));
        }

        // Beep
        else if (settingsString == "Beep") {
          M5.Displays(display).drawString(String(choiceBeep[0]) + " " + String(beep) + "%", 160 + offsetX,
                                          h - 6 + offsetY);

          if (btnA || btnC) {
            if (btnA == 1) {
              beep -= 1;
              if (beep < 0) {
                beep = 0;
              }
            } else if (btnC == 1) {
              beep += 1;
              if (beep > 100) {
                beep = 100;
              }
            }
          } else if (btnB == 1) {
            if (beepOld != beep) preferences.putUInt("beep", beep);
            clearData();
            viewGUI();
            settingsSelect = false;
            settingsMode   = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(75));
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}