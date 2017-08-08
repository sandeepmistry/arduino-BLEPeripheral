// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "BLETypedCharacteristics.h"

BLEBoolCharacteristic::BLEBoolCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<bool>(uuid, properties) {
}

BLECharCharacteristic::BLECharCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<char>(uuid, properties) {
}

BLEUnsignedCharCharacteristic::BLEUnsignedCharCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned char>(uuid, properties) {
}

BLEShortCharacteristic::BLEShortCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<short>(uuid, properties) {
}

BLEUnsignedShortCharacteristic::BLEUnsignedShortCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned short>(uuid, properties) {
}

BLEIntCharacteristic::BLEIntCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<int>(uuid, properties) {
}

BLEUnsignedIntCharacteristic::BLEUnsignedIntCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned int>(uuid, properties) {
}

BLELongCharacteristic::BLELongCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<long>(uuid, properties) {
}

BLEUnsignedLongCharacteristic::BLEUnsignedLongCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<unsigned long>(uuid, properties) {
}

BLEFloatCharacteristic::BLEFloatCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<float>(uuid, properties) {
}

BLEDoubleCharacteristic::BLEDoubleCharacteristic(const char* uuid, unsigned char properties) :
  BLETypedCharacteristic<double>(uuid, properties) {
}
