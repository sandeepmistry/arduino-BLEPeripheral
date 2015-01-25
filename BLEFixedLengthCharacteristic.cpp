#include "BLEFixedLengthCharacteristic.h"

BLEFixedLengthCharacteristic::BLEFixedLengthCharacteristic(const char* uuid, unsigned char properties, unsigned char valueSize) :
  BLECharacteristic(uuid, properties, valueSize)
{
}

BLEFixedLengthCharacteristic::BLEFixedLengthCharacteristic(const char* uuid, unsigned char properties, const char* value) :
  BLECharacteristic(uuid, properties, value)
{
}

bool BLEFixedLengthCharacteristic::fixedLength() const {
  return true;
}
