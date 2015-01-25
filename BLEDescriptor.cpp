#include "Arduino.h"

#include "BLEDeviceLimits.h"

#include "BLEDescriptor.h"

BLEDescriptor::BLEDescriptor(const char* uuid, const unsigned char value[], unsigned char valueLength) :
  BLEAttribute(uuid, BLETypeDescriptor),
  _value(value),
  _valueLength(valueLength)
{
}

BLEDescriptor::BLEDescriptor(const char* uuid, const char* value) :
  BLEAttribute(uuid, BLETypeDescriptor),
  _value((const unsigned char*)value),
  _valueLength(strlen(value))
{
}

BLEDescriptor::~BLEDescriptor() {
}

const unsigned char* BLEDescriptor::value() const {
  return this->_value;
}

unsigned char BLEDescriptor::valueLength() const {
  return this->_valueLength;
}

