#include "Arduino.h"

#include "BLEDeviceLimits.h"

#include "BLEDescriptor.h"

BLEDescriptor::BLEDescriptor(const char* uuid, unsigned char valueSize) :
  BLEAttribute(uuid, BLETypeDescriptor),
  _valueSize(min(valueSize, BLE_ATTRIBUTE_MAX_VALUE_LENGTH)),
  _valueLength(0)
{
  _value = (unsigned char*)malloc(this->_valueSize);
}

BLEDescriptor::BLEDescriptor(const char* uuid, const char* value) :
  BLEAttribute(uuid, BLETypeDescriptor),
  _valueSize(min(strlen(value), BLE_ATTRIBUTE_MAX_VALUE_LENGTH)),
  _valueLength(0)
{
  _value = (unsigned char*)malloc(this->_valueSize);
  this->setValue(value);
}

BLEDescriptor::~BLEDescriptor() {
  if (this->_value) {
    free(this->_value);
  }
}

unsigned char BLEDescriptor::valueSize() const {
  return this->_valueSize;
}

const unsigned char* BLEDescriptor::value() const {
  return this->_value;
}

unsigned char BLEDescriptor::valueLength() const {
  return this->_valueLength;
}

void BLEDescriptor::setValue(const unsigned char value[], unsigned char length) {
  this->_valueLength = min(length, this->_valueSize);

  memcpy(this->_value, value, this->_valueLength);
}

void BLEDescriptor::setValue(const char* value) {
  this->setValue((const unsigned char *)value, strlen(value));
}
