#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "BLEDescriptor.h"

BLEDescriptor::BLEDescriptor(const char* uuid, unsigned int valueSize)
  : BLEAttribute(uuid, BLE_TYPE_DESCRIPTOR)
{
  this->_valueSize = min(valueSize, BLE_DESCRIPTOR_MAX_VALUE_LENGTH);
  this->_valueLength = 0;
}

unsigned int BLEDescriptor::valueSize() {
  return this->_valueSize;
}

char* BLEDescriptor::value() {
  return this->_value;
}

unsigned int BLEDescriptor::valueLength() {
  return this->_valueLength;
}

void BLEDescriptor::setValue(char value[], unsigned int length) {
  this->_valueLength = min(length, this->_valueSize);

  memcpy(this->_value, value, this->_valueLength);
}
