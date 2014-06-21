#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "BLECharacteristic.h"

BLECharacteristic::BLECharacteristic(const char* uuid, char properties, unsigned int valueSize)
  : BLEAttribute(uuid, BLE_TYPE_CHARACTERISTIC)
{
  this->_properties = properties;
  this->_valueSize = min(valueSize, BLE_CHARACTERISTIC_MAX_VALUE_LENGTH);
  this->_valueLength = 0;
}

char BLECharacteristic::properties() {
  return this->_properties;
}

unsigned int BLECharacteristic::valueSize() {
  return this->_valueSize;
}

char* BLECharacteristic::value() {
  return this->_value;
}

unsigned int BLECharacteristic::valueLength() {
  return this->_valueLength;
}

void BLECharacteristic::setValue(char value[], unsigned int length) {
  this->_valueLength = min(length, this->_valueSize);

  memcpy(this->_value, value, this->_valueLength);
}

unsigned short BLECharacteristic::valueHandle() {
  return this->_valueHandle;
}

void BLECharacteristic::setValueHandle(unsigned short valueHandle) {
  this->_valueHandle = valueHandle;
}
