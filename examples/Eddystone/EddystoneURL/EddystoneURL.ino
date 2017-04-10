// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (EddystoneBeacon depends on SPI)
#include <SPI.h>
#include <EddystoneBeacon.h>

//custom boards may override default pin definitions with EddystoneBeacon(PIN_REQ, PIN_RDY, PIN_RST)
EddystoneBeacon eddystoneBeacon = EddystoneBeacon();

void setup() {
  Serial.begin(9600);

  delay(1000);

  eddystoneBeacon.begin(-18, "http://www.example.com"); // power, URI

  Serial.println(F("Eddystone URL Beacon"));
}

void loop() {
  eddystoneBeacon.loop();
}
