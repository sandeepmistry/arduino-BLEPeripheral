// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ     10
#define BLE_RDY     2
#define BLE_RST     9

// LED and button pin
#define LED_PIN     3
#define BUTTON_PIN  4

// create peripheral instance, see pinouts above
BLEPeripheral            blePeripheral        = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create service
BLEService               ledService           = BLEService("19b10010e8f2537e4f6cd104768a1214");

// create switch and button characteristic
BLECharCharacteristic    switchCharacteristic = BLECharCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);
BLECharCharacteristic    buttonCharacteristic = BLECharCharacteristic("19b10012e8f2537e4f6cd104768a1214", BLERead | BLENotify);

void setup() {
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  // set LED pin to output mode, button pin to input mode
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  // set advertised local name and service UUID
  blePeripheral.setLocalName("LED Switch");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(buttonCharacteristic);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE LED Switch Peripheral"));
}

void loop() {
  // poll peripheral
  blePeripheral.poll();

  // read the current button pin state
  char buttonValue = digitalRead(BUTTON_PIN);

  // has the value changed since the last read
  bool buttonChanged = (buttonCharacteristic.value() != buttonValue);

  if (buttonChanged) {
    // button state changed, update characteristics
    switchCharacteristic.setValue(buttonValue);
    buttonCharacteristic.setValue(buttonValue);
  }

  if (switchCharacteristic.written() || buttonChanged) {
    // update LED, either central has written to characteristic or button state has changed
    if (switchCharacteristic.value()) {
      Serial.println(F("LED on"));
      digitalWrite(LED_PIN, HIGH);
    } else {
      Serial.println(F("LED off"));
      digitalWrite(LED_PIN, LOW);
    }
  }
}
