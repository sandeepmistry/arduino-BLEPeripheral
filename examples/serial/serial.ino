// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
 * Serial Port over BLE
 * Create UART service compatible with Nordic's *nRF Toolbox* and Adafruit's *Bluefruit LE* iOS/Android apps.
 *
 * BLESerial class implements same protocols as Arduino's built-in Serial class and can be used as it's wireless
 * replacement. Data transfers are routed through a BLE service with TX and RX characteristics. To make the
 * service discoverable all UUIDs are NUS (Nordic UART Service) compatible.
 *
 * Please note that TX and RX characteristics use Notify and WriteWithoutResponse, so there's no guarantee
 * that the data will make it to the other end. However, under normal circumstances and reasonable signal
 * strengths everything works well.
 */


// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include "BLESerial.h"

//custom boards may override default pin definitions with BLESerial(PIN_REQ, PIN_RDY, PIN_RST)
BLESerial bleSerial;


void setup() {
  // custom services and characteristics can be added as well
  bleSerial.setLocalName("UART");

  Serial.begin(115200);
  bleSerial.begin();
}

void loop() {
  bleSerial.poll();

  forward();
  // loopback();
  // spam();
}


// forward received from Serial to BLESerial and vice versa
void forward() {
  if (bleSerial && Serial) {
    int byte;
    while ((byte = bleSerial.read()) > 0) Serial.write((char)byte);
    while ((byte = Serial.read()) > 0) bleSerial.write((char)byte);
  }
}

// echo all received data back
void loopback() {
  if (bleSerial) {
    int byte;
    while ((byte = bleSerial.read()) > 0) bleSerial.write(byte);
  }
}

// periodically sent time stamps
void spam() {
  if (bleSerial) {
    bleSerial.print(millis());
    bleSerial.println(" tick-tacks!");
    delay(1000);
  }
}
