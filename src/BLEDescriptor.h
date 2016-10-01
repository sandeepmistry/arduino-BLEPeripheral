// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_DESCRIPTOR_H_
#define _BLE_DESCRIPTOR_H_

#include "BLELocalAttribute.h"

class BLEDescriptor : public BLELocalAttribute
{
  public:
    BLEDescriptor(const char* uuid, const unsigned char value[], unsigned char valueLength);
    BLEDescriptor(const char* uuid, const char* value);

    virtual ~BLEDescriptor();

    virtual const unsigned char* value() const;
    virtual unsigned char valueLength() const;
    virtual unsigned char operator[] (int offset) const;

  private:
    const char*           _uuid;
    const unsigned char*  _value;
    unsigned char         _valueLength;
};

#endif
