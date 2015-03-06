#include "BLEHIDReportMapCharacteristic.h"

BLEHIDReportMapCharacteristic::BLEHIDReportMapCharacteristic() :
  BLEConstantCharacteristic("2a4b", NULL, 0),
  _hids(NULL),
  _numHids(0)
{

}

unsigned char BLEHIDReportMapCharacteristic::valueSize() const {
  unsigned char valueSize = 0;

  for (unsigned char i = 0; i < this->_numHids; i++) {
    valueSize += this->_hids[i]->getDescriptorLength();
  }

  return valueSize;
}

unsigned char BLEHIDReportMapCharacteristic::valueLength() const {
  return this->valueSize();
}

unsigned char BLEHIDReportMapCharacteristic::operator[] (int offset) const {
  unsigned char value = 0x00;
  unsigned char totalOffset = 0;

  for (unsigned char i = 0; i < this->_numHids; i++) {
    unsigned char descriptorLength = this->_hids[i]->getDescriptorLength();

    if ((offset >= totalOffset) && (offset < (totalOffset + descriptorLength))) {
      value = this->_hids[i]->getDescriptorValueAtOffset(offset - totalOffset);
      break;
    }

    totalOffset += descriptorLength;
  }

  return value;
}

void BLEHIDReportMapCharacteristic::setHids(BLEHID** hids, unsigned char numHids) {
  this->_hids = hids;
  this->_numHids = numHids;
}
