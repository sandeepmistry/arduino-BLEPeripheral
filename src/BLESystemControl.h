// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_SYSTEM_CONTROL_H_
#define _BLE_SYSTEM_CONTROL_H_

#include "Arduino.h"

#include "BLECharacteristic.h"
#include "BLEHIDReportReferenceDescriptor.h"
#include "BLEHID.h"

// From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidCombo.h
//       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

// system control keys
#define SYSCTRLKEY_POWER    0x01
#define SYSCTRLKEY_SLEEP    0x02
#define SYSCTRLKEY_WAKE     0x03

class BLESystemControl : public BLEHID
{
  public:
    BLESystemControl();

    size_t write(uint8_t k);

  protected:
    virtual void setReportId(unsigned char reportId);
    virtual unsigned char numAttributes();
    virtual BLELocalAttribute** attributes();

  private:
    BLECharacteristic                 _reportCharacteristic;
    BLEHIDReportReferenceDescriptor   _reportReferenceDescriptor;
};

#endif
