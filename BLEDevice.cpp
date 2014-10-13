#include "Arduino.h"

#include "BLEDevice.h"

BLEDevice::BLEDevice() :
  _eventListener(NULL),
  _connectable(true)
{
}

BLEDevice::~BLEDevice() {
}

void BLEDevice::setEventListener(BLEDeviceEventListener* eventListener) {
  this->_eventListener = eventListener;
}

void BLEDevice::setConnectable(bool connectable) {
  this->_connectable = connectable;
}
