#include <SPI.h>

#include "BLEPeripheral.h"

BLEPeripheral::BLEPeripheral(int8_t req, int8_t rdy, int8_t rst) {
  this->setLocalName(NULL);
  this->setAdvertisedServiceUuid(NULL);
}

bool BLEPeripheral::begin() {
  return true;
}

void BLEPeripheral::poll() {
}

void BLEPeripheral::setAdvertisedServiceUuid(const char* advertisedServiceUuid) {
  this->_advertisedServiceUuid = advertisedServiceUuid;
}

void BLEPeripheral::setLocalName(const char *localName) {
  this->_localName = localName;
}
