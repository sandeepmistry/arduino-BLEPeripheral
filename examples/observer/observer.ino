// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define NRF51
#undef __RFduino__
// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include <BLEUtil.h>
#include <ble_gap.h>

//custom boards may override default pin definitions with BLEPeripheral(PIN_REQ, PIN_RDY, PIN_RST)
BLEPeripheral                    blePeripheral                            = BLEPeripheral();

void setup() {
  Serial.begin(115200);
#if defined (__AVR_ATmega32U4__)
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#endif

  blePeripheral.setLocalName("foobaz"); // optional
  
  // begin initialization
  blePeripheral.begin();
  blePeripheral.setConnectable(false);
  blePeripheral.setAdvertisingInterval(500);
  blePeripheral.startAdvertising();
  blePeripheral.setEventHandler(BLEAdvertisementReceived, advHandler);
}

void advHandler(const void* adv) {
  ble_gap_evt_adv_report_t* report = (ble_gap_evt_adv_report_t*)adv;
  char address[18];
  BLEUtil::addressToString(report->peer_addr.addr, address);
  Serial.print(F("Evt Adv Report from "));
  Serial.println(address);
  Serial.print(F("got adv with payload "));
  Serial.println(report->dlen);
}

void loop() {
  if (Serial.available() > 0) {
    Serial.read();
    Serial.println("start scanning");
    blePeripheral.startScanning();
    blePeripheral.setLocalName("foobarg"); // optional
    blePeripheral.startAdvertising();
  }
  blePeripheral.poll();
}
