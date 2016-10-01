// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>

#include <BLEUtil.h>

// define pins (varies per shield/board)
#define BLE_REQ   6
#define BLE_RDY   7
#define BLE_RST   4

// create peripheral instance, see pinouts above
BLEPeripheral                    blePeripheral                            = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEBondStore                     bleBondStore;

// remote services
BLERemoteService                 ancsService                              = BLERemoteService("7905f431b5ce4e99a40f4b1e122d00d0");

// remote characteristics
BLERemoteCharacteristic          ancsNotificationSourceCharacteristic     = BLERemoteCharacteristic("9fbf120d630142d98c5825e699a21dbd", BLENotify);
//BLERemoteCharacteristic          ancsControlPointCharacteristic           = BLERemoteCharacteristic("69d1d8f345e149a898219bbdfdaad9d9", BLEWrite);
//BLERemoteCharacteristic          ancsDataSourceCharacteristic             = BLERemoteCharacteristic("22eac6e924d64bb5be44b36ace7c7bfb", BLENotify);


void setup() {
  Serial.begin(9600);
#if defined (__AVR_ATmega32U4__)
  while(!Serial);
#endif

  // clears bond data on every boot
  bleBondStore.clearData();

  blePeripheral.setBondStore(bleBondStore);

  blePeripheral.setServiceSolicitationUuid(ancsService.uuid());
  blePeripheral.setLocalName("ANCS");

  // set device name and appearance
  blePeripheral.setDeviceName("Arduino ANCS");
  blePeripheral.setAppearance(0x0080);

  blePeripheral.addRemoteAttribute(ancsService);
  blePeripheral.addRemoteAttribute(ancsNotificationSourceCharacteristic);
//  blePeripheral.addRemoteAttribute(ancsControlPointCharacteristic);
//  blePeripheral.addRemoteAttribute(ancsDataSourceCharacteristic);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  blePeripheral.setEventHandler(BLEBonded, blePeripheralBondedHandler);
  blePeripheral.setEventHandler(BLERemoteServicesDiscovered, blePeripheralRemoteServicesDiscoveredHandler);

  // assign event handlers for characteristic
  ancsNotificationSourceCharacteristic.setEventHandler(BLEValueUpdated, ancsNotificationSourceCharacteristicValueUpdated);
//  ancsDataSourceCharacteristic.setEventHandler(BLEValueUpdated, ancsDataSourceCharacteristicCharacteristicValueUpdated);

  // begin initialization
  blePeripheral.begin();

  Serial.println(F("BLE Peripheral - ANCS"));
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

void blePeripheralBondedHandler(BLECentral& central) {
  // central bonded event handler
  Serial.print(F("Remote bonded event, central: "));
  Serial.println(central.address());

  if (ancsNotificationSourceCharacteristic.canSubscribe()) {
    ancsNotificationSourceCharacteristic.subscribe();
  }
}

void blePeripheralRemoteServicesDiscoveredHandler(BLECentral& central) {
  // central remote services discovered event handler
  Serial.print(F("Remote services discovered event, central: "));
  Serial.println(central.address());

  if (ancsNotificationSourceCharacteristic.canSubscribe()) {
    ancsNotificationSourceCharacteristic.subscribe();
  }
}

enum AncsNotificationEventId {
  AncsNotificationEventIdAdded    = 0,
  AncsNotificationEventIdModified = 1,
  AncsNotificationEventIdRemoved  = 2
};

enum AncsNotificationEventFlags {
  AncsNotificationEventFlagsSilent         = 1,
  AncsNotificationEventFlagsImportant      = 2,
  AncsNotificationEventFlagsPositiveAction = 4,
  AncsNotificationEventFlagsNegativeAction = 8
};

enum AncsNotificationCategoryId {
  AncsNotificationCategoryIdOther              = 0,
  AncsNotificationCategoryIdIncomingCall       = 1,
  AncsNotificationCategoryIdMissedCall         = 2,
  AncsNotificationCategoryIdVoicemail          = 3,
  AncsNotificationCategoryIdSocial             = 4,
  AncsNotificationCategoryIdSchedule           = 5,
  AncsNotificationCategoryIdEmail              = 6,
  AncsNotificationCategoryIdNews               = 7,
  AncsNotificationCategoryIdHealthAndFitness   = 8,
  AncsNotificationCategoryIdBusinessAndFinance = 9,
  AncsNotificationCategoryIdLocation           = 10,
  AncsNotificationCategoryIdEntertainment      = 11
};

struct AncsNotification {
  unsigned char eventId;
  unsigned char eventFlags;
  unsigned char catergoryId;
  unsigned char catergoryCount;
  unsigned long notificationUid;
};

void ancsNotificationSourceCharacteristicValueUpdated(BLECentral& central, BLERemoteCharacteristic& characteristic) {
  Serial.println(F("ANCS Notification Source Value Updated:"));
  struct AncsNotification notification;

  memcpy(&notification, characteristic.value(), sizeof(notification));

  Serial.print("\tEvent ID: ");
  Serial.println(notification.eventId);
  Serial.print("\tEvent Flags: 0x");
  Serial.println(notification.eventFlags, HEX);
  Serial.print("\tCategory ID: ");
  Serial.println(notification.catergoryId);
  Serial.print("\tCategory Count: ");
  Serial.println(notification.catergoryCount);
  Serial.print("\tNotification UID: ");
  Serial.println(notification.notificationUid);

//  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
}

//void ancsDataSourceCharacteristicCharacteristicValueUpdated(BLECentral& central, BLERemoteCharacteristic& characteristic) {
//  Serial.print(F("ANCS Data Source Value Updated: "));
//
//  BLEUtil::printBuffer(characteristic.value(), characteristic.valueLength());
//}

