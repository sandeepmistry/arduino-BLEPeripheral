// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_MULTIMEDIA_H_
#define _BLE_MULTIMEDIA_H_

#include "Arduino.h"

#include "BLECharacteristic.h"
#include "BLEHIDReportReferenceDescriptor.h"
#include "BLEHID.h"

// From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidCombo.h
//       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

// multimedia keys
#define MMKEY_KB_VOL_UP     0x80 // do not use
#define MMKEY_KB_VOL_DOWN   0x81 // do not use
#define MMKEY_VOL_UP      0xE9
#define MMKEY_VOL_DOWN      0xEA
#define MMKEY_SCAN_NEXT_TRACK 0xB5
#define MMKEY_SCAN_PREV_TRACK 0xB6
#define MMKEY_STOP        0xB7
#define MMKEY_PLAYPAUSE     0xCD
#define MMKEY_MUTE        0xE2
#define MMKEY_BASSBOOST     0xE5
#define MMKEY_LOUDNESS      0xE7
#define MMKEY_KB_EXECUTE    0x74
#define MMKEY_KB_HELP     0x75
#define MMKEY_KB_MENU     0x76
#define MMKEY_KB_SELECT     0x77
#define MMKEY_KB_STOP     0x78
#define MMKEY_KB_AGAIN      0x79
#define MMKEY_KB_UNDO     0x7A
#define MMKEY_KB_CUT      0x7B
#define MMKEY_KB_COPY     0x7C
#define MMKEY_KB_PASTE      0x7D
#define MMKEY_KB_FIND     0x7E
#define MMKEY_KB_MUTE     0x7F // do not use

class BLEMultimedia : public BLEHID
{
  public:
    BLEMultimedia();

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
