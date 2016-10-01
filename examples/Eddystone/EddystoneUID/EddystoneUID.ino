// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (EddystoneBeacon depends on SPI)
#include <SPI.h>
#include <EddystoneBeacon.h>

// define pins (varies per shield/board)
//
//   Adafruit Bluefruit LE   10, 2, 9
//   Blend                    9, 8, UNUSED
//   Blend Micro              6, 7, 4
//   RBL BLE Shield           9, 8, UNUSED

#define EDDYSTONE_BEACON_REQ   6
#define EDDYSTONE_BEACON_RDY   7
#define EDDYSTONE_BEACON_RST   4

EddystoneBeacon eddystoneBeacon = EddystoneBeacon(EDDYSTONE_BEACON_REQ, EDDYSTONE_BEACON_RDY, EDDYSTONE_BEACON_RST);
BLEUuid         uid             = BLEUuid("01020304050607080910-AABBCCDDEEFF"); // <namespace id>-<instance id>

void setup() {
  Serial.begin(9600);

  eddystoneBeacon.begin(-18, uid); // power, UID

  Serial.println(F("Eddystone UID Beacon"));
}

void loop() {
  eddystoneBeacon.loop();
}
