// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_ATTRIBUTE_H_
#define _BLE_ATTRIBUTE_H_

enum BLEAttributeType {
  BLETypeService        = 0x2800,
  BLETypeCharacteristic = 0x2803,
  BLETypeDescriptor     = 0x2900
};

enum BLEProperty {
  BLEBroadcast            = 0x01,
  BLERead                 = 0x02,
  BLEWriteWithoutResponse = 0x04,
  BLEWrite                = 0x08,
  BLENotify               = 0x10,
  BLEIndicate             = 0x20
};

class BLEAttribute
{
  public:
    BLEAttribute(const char* uuid, enum BLEAttributeType type);
    const char* uuid() const;

    enum BLEAttributeType type() const;

  private:
    const char*                 _uuid;
    enum BLEAttributeType       _type;
};

#endif
