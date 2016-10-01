// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)

#include "iBeacon.h"

iBeacon::iBeacon() :
  BLEPeripheral()
{
  this->setConnectable(false);
}

void iBeacon::begin(const char* uuidString, unsigned short major, unsigned short minor, char measuredPower) {
  BLEUuid uuid(uuidString);
  int i = 0;

  // 0x004c = Apple, see https://www.bluetooth.org/en-us/specification/assigned-numbers/company-identifiers
  this->_manufacturerData[i++] = 0x4c; // Apple Company Identifier LE (16 bit)
  this->_manufacturerData[i++] = 0x00;

  // See "Beacon type" in "Building Applications with IBeacon".
  this->_manufacturerData[i++] = 0x02;
  this->_manufacturerData[i++] = uuid.length() + 5;

  for (int j = (uuid.length() - 1); j >= 0; j--) {
    this->_manufacturerData[i++] = uuid.data()[j];
  }

  this->_manufacturerData[i++] = major >> 8;
  this->_manufacturerData[i++] = major;
  this->_manufacturerData[i++] = minor >> 8;
  this->_manufacturerData[i++] = minor;
  this->_manufacturerData[i++] = measuredPower;

  this->setManufacturerData(this->_manufacturerData, i);

  BLEPeripheral::begin();
}

void iBeacon::loop() {
  this->poll();
}

#endif
