// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEHIDReportReferenceDescriptor.h"

BLEHIDReportReferenceDescriptor::BLEHIDReportReferenceDescriptor(BLEHIDDescriptorType type) :
  BLEDescriptor("2908", this->_value, sizeof(_value))
{
  this->_value[0] = 0x00;
  this->_value[1] = type;
}

BLEHIDReportReferenceDescriptor::~BLEHIDReportReferenceDescriptor() {
}

void BLEHIDReportReferenceDescriptor::setReportId(unsigned char reportId) {
  this->_value[0] = reportId;
}
