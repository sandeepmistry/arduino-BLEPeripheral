#include "Arduino.h"

#include "BLEDevice.h"

BLEDevice::BLEDevice() :
  _eventListener(NULL)
{
}

BLEDevice::~BLEDevice() {
}

void BLEDevice::setEventListener(BLEDeviceEventListener* eventListener) {
  this->_eventListener = eventListener;
}
