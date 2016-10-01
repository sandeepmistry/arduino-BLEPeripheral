// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEHIDPeripheral.h>
#include <BLEKeyboard.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

//#define ANDROID_CENTRAL

// create peripheral instance, see pinouts above
BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEKeyboard bleKeyboard;

void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  while(!Serial);
#endif

  // clear bond store data
  bleHIDPeripheral.clearBondStoreData();

#ifdef ANDROID_CENTRAL
  bleHIDPeripheral.setReportIdOffset(1);
#endif

  bleHIDPeripheral.setLocalName("HID Keyboard");
  bleHIDPeripheral.addHID(bleKeyboard);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Keyboard"));
}

void loop() {
  BLECentral central = bleHIDPeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      if (Serial.available() > 0) {
        // read in character
        char c = Serial.read();

        Serial.print(F("c = "));
        Serial.println(c);

        bleKeyboard.print(c);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
