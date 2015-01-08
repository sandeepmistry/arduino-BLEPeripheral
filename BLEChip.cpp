#include "Arduino.h"

#include "BLEChip.h"

#define DEFAULT_ADVERTISING_INTERVAL 100
#define DEFAULT_CONNECTABLE          true

BLEChip::BLEChip() :
  _eventListener(NULL),
  _advertisingInterval(DEFAULT_ADVERTISING_INTERVAL),
  _connectable(DEFAULT_CONNECTABLE)
{
}

BLEChip::~BLEChip() {
}

void BLEChip::setEventListener(BLEChipEventListener* eventListener) {
  this->_eventListener = eventListener;
}

void BLEChip::setAdvertisingInterval(unsigned short advertisingInterval) {
  this->_advertisingInterval = advertisingInterval;
}

void BLEChip::setConnectable(bool connectable) {
  this->_connectable = connectable;
}
