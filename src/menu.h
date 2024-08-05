// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

void viewMenu(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  M5.Displays(display).fillRoundRect(x, y, w, h, 8, TFT_MENU_BACK);
  M5.Displays(display).drawRoundRect(x, y, w, h, 8, TFT_MENU_BORDER);

  M5.Displays(display).setTextDatum(CC_DATUM);
  M5.Displays(display).setFont(&YELLOWCRE8pt7b);
  M5.Displays(display).setTextPadding(w - 2);
  M5.Displays(display).setTextColor(TFT_MENU_SELECT);
  M5.Displays(display).drawString("SETTINGS", 160 + offsetX, 14 + y);

  M5.Displays(display).setTextDatum(CC_DATUM);
  M5.Displays(display).setFont(0);
  M5.Displays(display).setTextPadding(w - 2);
  M5.Displays(display).setTextColor(TFT_MENU_SELECT);
  M5.Displays(display).drawString(String(NAME) + " V" + String(VERSION) + " by " + String(AUTHOR), 160 + offsetX,
                                  28 + y);

  M5.Displays(display).drawFastHLine(120 + offsetX, 3 + offsetY, 80, TFT_BLACK);
  M5.Displays(display).drawFastHLine(x + 1, 36 + y, w - 2, TFT_MENU_SELECT);
  M5.Displays(display).drawFastHLine(x + 1, (y + h) - 24, w - 2, TFT_MENU_SELECT);
}

void viewOption(int8_t settingsChoice, boolean settingsSelect, uint16_t x, uint16_t y, uint16_t w) {
  uint8_t start = 0;
  uint8_t stop  = 0;
  uint8_t i, j;

  M5.Displays(display).setTextDatum(CC_DATUM);
  M5.Displays(display).setFont(&tahoma8pt7b);
  M5.Displays(display).setTextPadding(w - 2);
  M5.Displays(display).setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);

  if (settingsChoice > 6) {
    start = settingsChoice - 6;
  } else {
    start = 0;
  }

  if (settingsLength < 7)  // If remove certains option from cleanSettings()
    stop = start + settingsLength;
  else
    stop = start + 7;

  j = 0;
  for (i = start; i < stop; i++) {
    // Serial.printf("%d %d %d %d %d\n", i, j, start, stop, settingsChoice);
    if (settingsSelect == false) {
      if (settingsChoice == i) {
        M5.Displays(display).setTextColor(TFT_BLACK, TFT_MENU_SELECT);
        M5.Displays(display).drawString(settings[i], 160 + offsetX, 45 + y + (j * 18));
      } else {
        M5.Displays(display).setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);
        M5.Displays(display).drawString(settings[i], 160 + offsetX, 45 + y + (j * 18));
      }
    } else {
      if (settingsChoice == i) {
        M5.Displays(display).setTextColor(TFT_BLACK, TFT_MENU_SELECT);
        M5.Displays(display).drawString(settings[i], 160 + offsetX, 45 + y + (j * 18));
      } else {
        M5.Displays(display).setTextColor(TFT_SILVER, TFT_MENU_BACK);
        M5.Displays(display).drawString(settings[i], 160 + offsetX, 45 + y + (j * 18));
      }
    }
    j++;
    if (j > 7) {
      j = 7;
    }
  }
  // Serial.println("-----");
}