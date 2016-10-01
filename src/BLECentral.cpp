// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "Arduino.h"

#include "BLEPeripheral.h"

#include "BLECentral.h"
#include "BLEUtil.h"

BLECentral::BLECentral(BLEPeripheral* peripheral) :
  _peripheral(peripheral)
{
  this->clearAddress();
}

BLECentral::operator bool() const {
  unsigned char zero[6] = {0, 0, 0, 0, 0, 0};

  return (memcmp(this->_address, zero, sizeof(this->_address)) != 0);
}

bool BLECentral::operator==(const BLECentral& rhs) const {
  return (memcmp(this->_address, rhs._address, sizeof(this->_address)) == 0);
}

bool BLECentral::operator!=(const BLECentral& rhs) const {
  return !(*this == rhs);
}

bool BLECentral::connected() {
  this->poll();

  return (*this && *this == this->_peripheral->central());
}

const char* BLECentral::address() const {
  static char address[18];

  BLEUtil::addressToString(this->_address, address);

  return address;
}

void BLECentral::poll() {
  this->_peripheral->poll();
}

void BLECentral::disconnect() {
  if (this->connected()) {
    this->_peripheral->disconnect();
  }
}

void BLECentral::setAddress(const unsigned char* address) {
  memcpy(this->_address, address, sizeof(this->_address));
}

void BLECentral::clearAddress() {
  memset(this->_address, 0x00, sizeof(this->_address));
}
