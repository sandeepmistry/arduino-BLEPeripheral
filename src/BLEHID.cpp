// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEHIDPeripheral.h"

#include "BLEHID.h"

unsigned char BLEHID::_numHids = 0;

BLEHID::BLEHID(const unsigned char* descriptor, unsigned char descriptorLength, unsigned char reportIdOffset) :
  _reportId(0),
  _descriptor(descriptor),
  _descriptorLength(descriptorLength),
  _reportIdOffset(reportIdOffset)
{
  _numHids++;
}

void BLEHID::setReportId(unsigned char reportId) {
  this->_reportId = reportId;
}

unsigned char BLEHID::getDescriptorLength() {
  return this->_descriptorLength;
}

unsigned char BLEHID::getDescriptorValueAtOffset(unsigned char offset) {
  if (offset == this->_reportIdOffset && this->_reportIdOffset) {
    return this->_reportId;
  } else {
    return pgm_read_byte_near(&this->_descriptor[offset]);
  }
}

unsigned char BLEHID::numHids() {
  return _numHids;
}

void BLEHID::sendData(BLECharacteristic& characteristic, unsigned char data[], unsigned char length) {
  // wait until we can notify
  while(!characteristic.canNotify()) {
    BLEHIDPeripheral::instance()->poll();
  }

  characteristic.setValue(data, length);
}
