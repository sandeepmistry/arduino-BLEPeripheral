#include "Arduino.h"

#include "BLEPeripheral.h"

#include "BLECentral.h"

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

  sprintf(address, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
    this->_address[5],
    this->_address[4],
    this->_address[3],
    this->_address[2],
    this->_address[1],
    this->_address[0]);

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
