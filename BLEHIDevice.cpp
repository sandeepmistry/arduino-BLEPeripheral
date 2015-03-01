#include "BLEHID.h"

#include "BLEHIDDevice.h"

unsigned char BLEHIDDevice::_numDevices = 0;

BLEHIDDevice::BLEHIDDevice() {
  _numDevices++;
}

unsigned char BLEHIDDevice::numDevices() {
  return _numDevices;
}

void BLEHIDDevice::sendData(BLECharacteristic& characteristic, unsigned char data[], unsigned char length) {
  // wait until we can notify
  while(!characteristic.canNotify()) {
    BLEHID::instance()->poll();
  }

  characteristic.setValue(data, length);
}
