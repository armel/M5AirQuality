// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Get Battery level
int8_t getBatteryLevel() {
  return M5.Power.getBatteryLevel();
}

// Control if charging
bool isCharging() {
  return M5.Power.isCharging();
}