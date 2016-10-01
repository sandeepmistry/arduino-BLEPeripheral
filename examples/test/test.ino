// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//#define SHOW_FREE_MEMORY

#ifdef SHOW_FREE_MEMORY
#include <MemoryFree.h>
#endif

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ   10
#define BLE_RDY   2
#define BLE_RST   9

// create peripheral instance, see pinouts above
BLEPeripheral                    blePeripheral       = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create service
BLEService                       testService         = BLEService("fff0");
// create counter characteristic
BLEUnsignedShortCharacteristic   testCharacteristic  = BLEUnsignedShortCharacteristic("fff1", BLERead | BLEWrite | BLEWriteWithoutResponse | BLENotify /*| BLEIndicate*/);
// create user description descriptor for characteristic
BLEDescriptor                    testDescriptor      = BLEDescriptor("2901", "counter");

// last counter update time
unsigned long long               lastSent            = 0;

void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  blePeripheral.setLocalName("test");
#if 1
  blePeripheral.setAdvertisedServiceUuid(testService.uuid());
#else
  const char manufacturerData[4] = {0x12, 0x34, 0x56, 0x78};
  blePeripheral.setManufacturerData(manufacturerData, sizeof(manufacturerData));
#endif

  // set device name and appearance
  blePeripheral.setDeviceName("Test");
  blePeripheral.setAppearance(0x0080);

  // add service, characteristic, and decriptor to peripheral
  blePeripheral.addAttribute(testService);
  blePeripheral.addAttribute(testCharacteristic);
  blePeripheral.addAttribute(testDescriptor);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  testCharacteristic.setEventHandler(BLEWritten, characteristicWritten);
  testCharacteristic.setEventHandler(BLESubscribed, characteristicSubscribed);
  testCharacteristic.setEventHandler(BLEUnsubscribed, characteristicUnsubscribed);

  // set initial value for characteristic
  testCharacteristic.setValue(0);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Peripheral"));

#ifdef SHOW_FREE_MEMORY
  Serial.print(F("Free memory = "));
  Serial.println(freeMemory());
#endif
}

void loop() {
  BLECentral central = blePeripheral.central();

  if (central) {
    // central connected to peripheral
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    // reset counter value
    testCharacteristic.setValue(0);

    while (central.connected()) {
      // central still connected to peripheral
      if (testCharacteristic.written()) {
        // central wrote new value to characteristic
        Serial.println(F("counter written, reset"));

         // reset counter value
        lastSent = 0;
        testCharacteristic.setValue(0);
      }

      if (millis() > 1000 && (millis() - 1000) > lastSent) {
        // atleast one second has passed since last increment
        lastSent = millis();

        // increment characteristic value
        testCharacteristic.setValue(testCharacteristic.value() + 1);

        Serial.print(F("counter = "));
        Serial.println(testCharacteristic.value(), DEC);
      }
    }

    // central disconnected
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print(F("Connected event, central: "));
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) {
  // central disconnected event handler
  Serial.print(F("Disconnected event, central: "));
  Serial.println(central.address());
}

void characteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  // characteristic value written event handler
  Serial.print(F("Characteristic event, writen: "));
  Serial.println(testCharacteristic.value(), DEC);
}

void characteristicSubscribed(BLECentral& central, BLECharacteristic& characteristic) {
  // characteristic subscribed event handler
  Serial.println(F("Characteristic event, subscribed"));
}

void characteristicUnsubscribed(BLECentral& central, BLECharacteristic& characteristic) {
  // characteristic unsubscribed event handler
  Serial.println(F("Characteristic event, unsubscribed"));
}
