// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLERemoteAttribute.h"

unsigned char BLERemoteAttribute::_numAttributes = 0;

BLERemoteAttribute::BLERemoteAttribute(const char* uuid, enum BLEAttributeType type) :
  BLEAttribute(uuid, type)
{
  _numAttributes++;
}

unsigned char BLERemoteAttribute::numAttributes() {
  return _numAttributes;
}
