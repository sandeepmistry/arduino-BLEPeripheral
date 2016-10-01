#ifndef _BLE_HID_REPORT_REFERENCE_DESCRIPTOR_H_
#define _BLE_HID_REPORT_REFERENCE_DESCRIPTOR_H_

#include "BLEDescriptor.h"

enum BLEHIDDescriptorType {
  BLEHIDDescriptorTypeInput = 0x01,
  BLEHIDDescriptorTypeOutput = 0x02
};

class BLEHIDReportReferenceDescriptor : public BLEDescriptor
{
  public:
    BLEHIDReportReferenceDescriptor(BLEHIDDescriptorType type);

    virtual ~BLEHIDReportReferenceDescriptor();

    void setReportId(unsigned char reportId);

  private:
    unsigned char         _value[2];
};

#endif
