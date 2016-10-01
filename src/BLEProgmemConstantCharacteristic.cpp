// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEProgmemConstantCharacteristic.h"

BLEProgmemConstantCharacteristic::BLEProgmemConstantCharacteristic(const char* uuid, const unsigned char value[], unsigned char length) :
  BLEConstantCharacteristic(uuid, value, length)
{
}

BLEProgmemConstantCharacteristic::BLEProgmemConstantCharacteristic(const char* uuid, const char* value) :
#ifdef __AVR__
  BLEConstantCharacteristic(uuid, (const unsigned char *)value, strlen_P(value))
#else
  BLEConstantCharacteristic(uuid, value)
#endif
{
}

BLEProgmemConstantCharacteristic::~BLEProgmemConstantCharacteristic() {
}

unsigned char BLEProgmemConstantCharacteristic::operator[] (int offset) const {
  return pgm_read_byte_near(&this->_value[offset]);
}
