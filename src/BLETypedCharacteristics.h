// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_TYPED_CHARACTERISTICS_H_
#define _BLE_TYPED_CHARACTERISTICS_H_

#include "BLETypedCharacteristic.h"

class BLEBoolCharacteristic : public BLETypedCharacteristic<bool> {
  public:
    BLEBoolCharacteristic(const char* uuid, unsigned char properties);
};

class BLECharCharacteristic : public BLETypedCharacteristic<char> {
  public:
    BLECharCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedCharCharacteristic : public BLETypedCharacteristic<unsigned char> {
  public:
    BLEUnsignedCharCharacteristic(const char* uuid, unsigned char properties);
};

class BLEShortCharacteristic : public BLETypedCharacteristic<short> {
  public:
    BLEShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedShortCharacteristic : public BLETypedCharacteristic<unsigned short> {
  public:
    BLEUnsignedShortCharacteristic(const char* uuid, unsigned char properties);
};

class BLEIntCharacteristic : public BLETypedCharacteristic<int> {
  public:
    BLEIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedIntCharacteristic : public BLETypedCharacteristic<unsigned int> {
  public:
    BLEUnsignedIntCharacteristic(const char* uuid, unsigned char properties);
};

class BLELongCharacteristic : public BLETypedCharacteristic<long> {
  public:
    BLELongCharacteristic(const char* uuid, unsigned char properties);
};

class BLEUnsignedLongCharacteristic : public BLETypedCharacteristic<unsigned long> {
  public:
    BLEUnsignedLongCharacteristic(const char* uuid, unsigned char properties);
};

class BLEFloatCharacteristic : public BLETypedCharacteristic<float> {
  public:
    BLEFloatCharacteristic(const char* uuid, unsigned char properties);
};

class BLEDoubleCharacteristic : public BLETypedCharacteristic<double> {
  public:
    BLEDoubleCharacteristic(const char* uuid, unsigned char properties);
};

#endif
