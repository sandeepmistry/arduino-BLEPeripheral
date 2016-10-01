// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_REMOTE_ATTRIBUTE_H_
#define _BLE_REMOTE_ATTRIBUTE_H_

#include "BLEAttribute.h"

class BLERemoteAttribute : public BLEAttribute
{
  friend class BLEPeripheral;

  public:
    BLERemoteAttribute(const char* uuid, enum BLEAttributeType type);

  protected:
    static unsigned char numAttributes();

  private:
    static unsigned char _numAttributes;
};

#endif
