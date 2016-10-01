// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <BLEPeripheral.h>
#include <iBeacon.h>

#if !defined(NRF51) && !defined(NRF52) && !defined(__RFduino__)
#error "This example only works with nRF51 boards"
#endif

iBeacon beacon;

void setup() {
  char* uuid                   = "a196c876-de8c-4c47-ab5a-d7afd5ae7127";
  unsigned short major         = 0;
  unsigned short minor         = 0;
  unsigned short measuredPower = -55;

  beacon.begin(uuid, major, minor, measuredPower);
}

void loop() {
  beacon.loop();
}
