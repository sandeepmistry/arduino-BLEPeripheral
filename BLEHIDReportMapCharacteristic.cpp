#include "BLEHIDReportMapCharacteristic.h"

BLEHIDReportMapCharacteristic::BLEHIDReportMapCharacteristic() :
  BLEConstantCharacteristic("2a4b", NULL, 0),
  _devices(NULL),
  _numDevices(0)
{

}

unsigned char BLEHIDReportMapCharacteristic::valueSize() const {
  unsigned char valueSize = 0;

  for (unsigned char i = 0; i < this->_numDevices; i++) {
    valueSize += this->_devices[i]->getDescriptorLength();
  }

  return valueSize;
}

unsigned char BLEHIDReportMapCharacteristic::valueLength() const {
  return this->valueSize();
}

unsigned char BLEHIDReportMapCharacteristic::operator[] (int offset) const {
  unsigned char value = 0x00;
  unsigned char totalOffset = 0;

  for (unsigned char i = 0; i < this->_numDevices; i++) {
    unsigned char descriptorLength = this->_devices[i]->getDescriptorLength();

    if ((offset >= totalOffset) && (offset < (totalOffset + descriptorLength))) {
      value = this->_devices[i]->getDescriptorValueAtOffset(offset - totalOffset);
      break;
    }

    totalOffset = descriptorLength;
  }

  return value;
}

void BLEHIDReportMapCharacteristic::setDevices(BLEHIDDevice** devices, unsigned char numDevices) {
  this->_devices = devices;
  this->_numDevices = numDevices;
}
