// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEHIDPeripheral.h>
#include <BLEKeyboard.h>

// From: http://playground.arduino.cc/Code/Keypad
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = { A0, A1, A2, A3 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 7, 6, 5 }; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// define pins (varies per shield/board)
#define BLE_REQ   9
#define BLE_RDY   8
#define BLE_RST   4

// create peripheral instance, see pinouts above
BLEHIDPeripheral bleHIDPeripheral = BLEHIDPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEKeyboard bleKeyboard;

void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  while(!Serial);
#endif
  char c = keypad.getKey();

  if (c == '#') {
    Serial.println(F("BLE HID Peripheral - clearing bond data"));

    // clear bond store data
    bleHIDPeripheral.clearBondStoreData();
  }

  bleHIDPeripheral.setLocalName("HID Keypad");
  bleHIDPeripheral.addHID(bleKeyboard);

  bleHIDPeripheral.begin();

  Serial.println(F("BLE HID Keypad"));
}

void loop() {
  BLECentral central = bleHIDPeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    while (central.connected()) {
      char c = keypad.getKey();

      if (c) {
        Serial.println(c);
        bleKeyboard.print(c);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
