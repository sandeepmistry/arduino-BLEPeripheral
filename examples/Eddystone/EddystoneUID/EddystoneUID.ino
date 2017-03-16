// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (EddystoneBeacon depends on SPI)
#include <SPI.h>
#include <EddystoneBeacon.h>

//custom boards may override default pin definitions with EddystoneBeacon(PIN_REQ, PIN_RDY, PIN_RST)
EddystoneBeacon eddystoneBeacon = EddystoneBeacon();
BLEUuid         uid             = BLEUuid("01020304050607080910-AABBCCDDEEFF"); // <namespace id>-<instance id>

void setup() {
  Serial.begin(9600);

  eddystoneBeacon.begin(-18, uid); // power, UID

  Serial.println(F("Eddystone UID Beacon"));
}

void loop() {
  eddystoneBeacon.loop();
}
