// Copyright (c) Bosch Software Innovations GmbH. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
 * Example sketch for requesting the nRF SoC temperature and 
 * offer it via a temperature service charachteristic.
 */

#include <BLEPeripheral.h>

class MyBle : public BLEPeripheral {
public:
  MyBle(unsigned char req, unsigned char rdy, unsigned char rst)
      : BLEPeripheral(req, rdy, rst) {};
  void init() {
    addAttribute(srvTemp);
    addAttribute(chrTemp);
    addAttribute(dscTemp);
    setDeviceName("Soc Temp");      // optional
    setLocalName ("Soc Temp");      // max 20 char!
    setAdvertisedServiceUuid(srvTemp.uuid()); // optional 
  };
private:
  BLEService             srvTemp = BLEService            ("CCC0");
  BLEFloatCharacteristic chrTemp = BLEFloatCharacteristic("CCC1", BLERead | BLENotify);
  BLEDescriptor          dscTemp = BLEDescriptor         ("2901", "Temp. Celsius");
  // overload callback:
  void   BLEDeviceTemperatureReceived(BLEDevice& device, float temp) {
    if (chrTemp.value()!=temp) {
        chrTemp.setValue(temp);
        Serial.print("Temp.[C]: "); Serial.println(temp);
    }  
  };
};

// for extern nRF8001 on shield/board
// enable SPI & define pins 
//#include <SPI.h>
//#define BLE_REQ   10
//#define BLE_RDY   2
//#define BLE_RST   9

// for nrf51 or nRF52 variants:
#define BLE_REQ   0
#define BLE_RDY   0
#define BLE_RST   0

// create ble temperature instance, see pinouts above
MyBle    ble(BLE_REQ, BLE_RDY, BLE_RST);
uint32_t timeStamp;

void handleBleConnect(BLECentral& central) {
  Serial.print("Connected event,    central: ");
  Serial.println(central.address());
}
void handleBleDisconnect(BLECentral& central) {
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void setup() {
  Serial.begin(115200);
  ble.init();
  ble.setEventHandler(BLEConnected,    handleBleConnect);
  ble.setEventHandler(BLEDisconnected, handleBleDisconnect);
  ble.begin();
  ble.requestTemperature();
  timeStamp = millis();
  Serial.println("... nRF BLE setup done!");
}

void loop() {
  ble.poll();
  if ((millis()-timeStamp)> 1000) { //msec
    timeStamp = millis();
    ble.requestTemperature();
  }
}

