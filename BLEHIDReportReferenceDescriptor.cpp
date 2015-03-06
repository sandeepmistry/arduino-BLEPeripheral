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
