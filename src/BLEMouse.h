// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_MOUSE_H_
#define _BLE_MOUSE_H_

#include "Arduino.h"

#include "BLECharacteristic.h"
#include "BLEHIDReportReferenceDescriptor.h"
#include "BLEHID.h"

// From: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidCombo.h
//       permission to use under MIT license by @ladyada (https://github.com/adafruit/Adafruit-Trinket-USB/issues/10)

// use these masks with the "move" function
#define MOUSEBTN_LEFT_MASK    0x01
#define MOUSEBTN_RIGHT_MASK   0x02
#define MOUSEBTN_MIDDLE_MASK  0x04

class BLEMouse : public BLEHID
{
  public:
    BLEMouse();

    void click(uint8_t b = MOUSEBTN_LEFT_MASK);
    void move(signed char x, signed char y, signed char wheel = 0);
    void press(uint8_t b = MOUSEBTN_LEFT_MASK);
    void release(uint8_t b = MOUSEBTN_LEFT_MASK);
    bool isPressed(uint8_t b = MOUSEBTN_LEFT_MASK);

  protected:
    virtual void setReportId(unsigned char reportId);
    virtual unsigned char numAttributes();
    virtual BLELocalAttribute** attributes();

  private:
    BLECharacteristic                 _reportCharacteristic;
    BLEHIDReportReferenceDescriptor   _reportReferenceDescriptor;

    unsigned char                     _button;
};

#endif
