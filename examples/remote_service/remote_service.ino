// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

// create peripheral instance, see pinouts above
BLEPeripheral                    blePeripheral                            = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);

// create remote services
BLERemoteService                 remoteGenericAttributeService            = BLERemoteService("1800");

// create remote characteristics
BLERemoteCharacteristic          remoteDeviceNameCharacteristic           = BLERemoteCharacteristic("2a00", BLERead);


void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  while(!Serial); // wait for serial
#endif

  blePeripheral.setLocalName("remote-attributes");

  // set device name and appearance
  blePeripheral.setDeviceName("Remote Attributes");
  blePeripheral.setAppearance(0x0080);

  blePeripheral.addRemoteAttribute(remoteGenericAttributeService);
  blePeripheral.addRemoteAttribute(remoteDeviceNameCharacteristic);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  remoteDeviceNameCharacteristic.setEventHandler(BLEValueUpdated, bleRemoteDeviceNameCharacteristicValueUpdatedHandle);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Peripheral - remote attributes"));
}

void loop() {
  blePeripheral.poll();
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

void blePeripheralRemoteServicesDiscoveredHandler(BLECentral& central) {
  // central remote services discovered event handler
  Serial.print(F("Remote services discovered event, central: "));
  Serial.println(central.address());

  if (remoteDeviceNameCharacteristic.canRead()) {
    remoteDeviceNameCharacteristic.read();
  }
}

void bleRemoteDeviceNameCharacteristicValueUpdatedHandle(BLECentral& central, BLERemoteCharacteristic& characteristic) {
  char remoteDeviceName[BLE_REMOTE_ATTRIBUTE_MAX_VALUE_LENGTH + 1];
  memset(remoteDeviceName, 0, sizeof(remoteDeviceName));
  memcpy(remoteDeviceName, remoteDeviceNameCharacteristic.value(), remoteDeviceNameCharacteristic.valueLength());

  Serial.print(F("Remote device name: "));
  Serial.println(remoteDeviceName);
}

