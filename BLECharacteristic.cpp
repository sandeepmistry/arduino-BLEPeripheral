#include "Arduino.h"

#include "BLECharacteristic.h"

BLECharacteristic::BLECharacteristic(const char* uuid, unsigned char properties, unsigned char valueSize) :
  BLEAttribute(uuid, BLETypeCharacteristic),
  _properties(properties),
  _valueSize(min(valueSize, BLE_ATTRIBUTE_MAX_VALUE_LENGTH)),
  _valueLength(0),
  _hasNotifySubscriber(false),
  _hasIndicateSubscriber(false),
  _hasNewValue(false),
  _newValueHandler(NULL),
  _listener(NULL)
{
  _value = (unsigned char*)malloc(this->_valueSize);
}

BLECharacteristic::BLECharacteristic(const char* uuid, unsigned char properties, const char* value) :
  BLEAttribute(uuid, BLETypeCharacteristic),
  _properties(properties),
  _valueSize(min(strlen(value), BLE_ATTRIBUTE_MAX_VALUE_LENGTH)),
  _valueLength(0),
  _hasNotifySubscriber(false),
  _hasIndicateSubscriber(false),
  _hasNewValue(false),
  _newValueHandler(NULL),
  _listener(NULL)
{
  _value = (unsigned char*)malloc(this->_valueSize);
  this->setValue(value);
}

BLECharacteristic::~BLECharacteristic() {
  if (this->_value) {
    free(this->_value);
  }
}

unsigned char BLECharacteristic::properties() {
  return this->_properties;
}

unsigned char BLECharacteristic::valueSize() {
  return this->_valueSize;
}

unsigned const char* BLECharacteristic::value() {
  return this->_value;
}

unsigned char BLECharacteristic::valueLength() {
  return this->_valueLength;
}

void BLECharacteristic::setValue(const unsigned char value[], unsigned char length) {
  this->_valueLength = min(length, this->_valueSize);

  memcpy(this->_value, value, this->_valueLength);

  if (this->_listener) {
    _listener->characteristicValueChanged(*this);
  }
}

void BLECharacteristic::setValue(const char* value) {
  this->setValue((const unsigned char *)value, strlen(value));
}

bool BLECharacteristic::hasNotifySubscriber() {
  this->_hasNotifySubscriber;
}

void BLECharacteristic::setHasNotifySubscriber(bool hasNotifySubscriber) {
  this->_hasNotifySubscriber = hasNotifySubscriber;
}

bool BLECharacteristic::hasIndicateSubscriber() {
  return this->_hasIndicateSubscriber;
}

void BLECharacteristic::setHasIndicateSubscriber(bool hasIndicateSubscriber) {
  this->_hasIndicateSubscriber = hasIndicateSubscriber;
}

bool BLECharacteristic::hasNewValue() {
  bool hasNewValue = this->_hasNewValue;

  if (hasNewValue) {
    this->_hasNewValue = false;
  }

  return hasNewValue;
}

void BLECharacteristic::setNewValueHandler(BLECharacteristicNewValueHandler newValueHandler) {
  this->_newValueHandler = newValueHandler;
}

void BLECharacteristic::setHasNewValue(bool hasNewValue) {
  this->_hasNewValue = hasNewValue;

  if (hasNewValue && this->_newValueHandler) {
    this->_newValueHandler();
  }
}

void BLECharacteristic::setCharacteristicValueListener(BLECharacteristicValueChangeListener& listener) {
  this->_listener = &listener;
}
