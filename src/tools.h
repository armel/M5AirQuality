// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

uint8_t btnA, btnB, btnC, btnESC;

struct Button {
  String name;     // name
  int x;           // x
  int y;           // y
  int w;           // width
  int h;           // height
  int d;           // distance
  boolean active;  // active, if true, check this button, else bypass
  boolean read;    // read, if true, button is push, else false
};

Button myBtn[] = {
  {"myBtnA", 0, 240, 100, 80, 1000, true, false},
  {"myBtnB", 110, 240, 100, 80, 1000, true, false},
  {"myBtnC", 220, 240, 100, 80, 1000, true, false},
};

uint8_t limit = sizeof myBtn / sizeof myBtn[0];

// Get Battery level
int8_t getBatteryLevel() {
  return M5.Power.getBatteryLevel();
}

// Control if charging
bool isCharging() {
  return M5.Power.isCharging();
}

// Shutdown
void shutdown() {
  M5.Power.powerOff();
}

// Reboot
void reboot() {
  ESP.restart();
}

// Get button
void getButton() {
  // Redefine if CoreS3
  if (M5.getBoard() == m5::board_t::board_M5StackCoreS3) {
    Button myBtn[] = {
      {"myBtnA", 0, 160, 100, 80, 1000, true, false},
      {"myBtnB", 110, 160, 100, 80, 1000, true, false},
      {"myBtnC", 220, 160, 100, 80, 1000, true, false},
    };
  }

  // M5Stack Core
  if (M5.getBoard() == m5::board_t::board_M5Stack) {
    M5.update();

    if (ESC) {
      btnESC = M5.BtnB.pressedFor(500);

      btnA = M5.BtnA.isPressed();
      btnB = M5.BtnB.wasReleased();
      btnC = M5.BtnC.isPressed();

      if (btnESC == 1) btnB = 0;
    } else {
      btnA   = M5.BtnA.isPressed();
      btnB   = M5.BtnB.isPressed();
      btnC   = M5.BtnC.isPressed();
      btnESC = 0;
    }
    // Serial.printf("btnA %d btnB %d btnC %d btnESC %d\n", btnA, btnB, btnC, btnESC);
  }
  // M5Stack Core2 and CoreS3
  else if (M5.getBoard() == m5::board_t::board_M5StackCore2 || M5.getBoard() == m5::board_t::board_M5StackCoreS3 ||
           M5.getBoard() == m5::board_t::board_M5StackCoreS3SE) {
    M5.update();

    if (ESC) {
      btnESC = M5.BtnPWR.wasClicked();
    } else {
      btnESC = 0;
    }

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
          distanceCurrent =
            (int)(sqrt(pow(t.x - (myBtn[i].x + (myBtn[i].w / 2)), 2) + pow(t.y - (myBtn[i].y + (myBtn[i].h / 2)), 2)));
          myBtn[i].d = distanceCurrent;
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

    if (ESC && btnESC == 1) btnB = 0;
    // Serial.printf("btnA %d btnB %d btnC %d btnESC %d\n", btnA, btnB, btnC, btnESC);
  }

  M5.Speaker.setVolume(beep);

  // Play sound
  if (btnA || btnC) {
    M5.Speaker.tone(1000, 50);
    vTaskDelay(pdMS_TO_TICKS(20));
  } else if (btnB) {
    M5.Speaker.tone(2000, 50);
    vTaskDelay(pdMS_TO_TICKS(100));
  } else if (btnESC) {
    M5.Speaker.tone(3000, 50);
    vTaskDelay(pdMS_TO_TICKS(100));
  } else {
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}