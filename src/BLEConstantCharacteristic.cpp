// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLEConstantCharacteristic.h"

BLEConstantCharacteristic::BLEConstantCharacteristic(const char* uuid, const unsigned char value[], unsigned char length) :
  BLEFixedLengthCharacteristic(uuid, BLERead, (unsigned char)0)
{
  this->_valueLength = this->_valueSize = length;
  this->_value = (unsigned char*)value;
}

BLEConstantCharacteristic::BLEConstantCharacteristic(const char* uuid, const char* value) :
  BLEFixedLengthCharacteristic(uuid, BLERead, (unsigned char)0)
{
  this->_valueLength = this->_valueSize = strlen(value);
  this->_value = (unsigned char*)value;
}

BLEConstantCharacteristic::~BLEConstantCharacteristic() {
  this->_value = NULL; // null so super destructor doesn't try to free
}

bool BLEConstantCharacteristic::setValue(const unsigned char /*value*/[], unsigned char /*length*/) {
  return false;
}

bool BLEConstantCharacteristic::setValue(const char* /*value*/) {
  return false;
}
