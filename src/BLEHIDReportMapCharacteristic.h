// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_HID_REPORT_MAP_CHARACTERISTIC_H_
#define _BLE_HID_REPORT_MAP_CHARACTERISTIC_H_

#include "BLEHID.h"
#include "BLEConstantCharacteristic.h"

class BLEHIDReportMapCharacteristic : public BLEConstantCharacteristic
{
  friend class BLEHID;

  public:
    BLEHIDReportMapCharacteristic();

    virtual unsigned char valueSize() const;
    virtual unsigned char valueLength() const;
    virtual unsigned char operator[] (int offset) const;

    void setHids(BLEHID** hids, unsigned char numHids);

  private:
    BLEHID**                  _hids;
    unsigned char             _numHids;
};

#endif
