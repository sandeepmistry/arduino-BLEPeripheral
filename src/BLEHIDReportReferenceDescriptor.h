// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
