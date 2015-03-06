#include "BLEHID.h"

#include "BLEHIDDevice.h"

unsigned char BLEHIDDevice::_numDevices = 0;

BLEHIDDevice::BLEHIDDevice(const unsigned char* descriptor, unsigned char descriptorLength, unsigned char reportIdOffset) :
  _reportId(0),
  _descriptor(descriptor),
  _descriptorLength(descriptorLength),
  _reportIdOffset(reportIdOffset)
{
  _numDevices++;
}

void BLEHIDDevice::setReportId(unsigned char reportId) {
  this->_reportId = reportId;
}

unsigned char BLEHIDDevice::getDescriptorLength() {
  return this->_descriptorLength;
}

unsigned char BLEHIDDevice::getDescriptorValueAtOffset(unsigned char offset) {
  if (offset == this->_reportIdOffset) {
    return this->_reportId;
  } else {
    return pgm_read_byte_near(&this->_descriptor[offset]);
  }
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
