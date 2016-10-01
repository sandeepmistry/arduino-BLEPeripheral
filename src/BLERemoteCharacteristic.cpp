// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "Arduino.h"

#include "BLERemoteCharacteristic.h"

BLERemoteCharacteristic::BLERemoteCharacteristic(const char* uuid, unsigned char properties) :
  BLERemoteAttribute(uuid, BLETypeCharacteristic),
  _properties(properties),
  _valueLength(0),
  _valueUpdated(false),
  _listener(NULL)
{
  memset(this->_eventHandlers, 0x00, sizeof(this->_eventHandlers));
}

BLERemoteCharacteristic::~BLERemoteCharacteristic() {
}

unsigned char BLERemoteCharacteristic::properties() const {
  return this->_properties;
}

const unsigned char* BLERemoteCharacteristic::value() const {
  return this->_value;
}

unsigned char BLERemoteCharacteristic::valueLength() const {
  return this->_valueLength;
}

bool BLERemoteCharacteristic::canRead() {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->canReadRemoteCharacteristic(*this);
  }

  return result;
}

bool BLERemoteCharacteristic::read() {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->readRemoteCharacteristic(*this);
  }

  return result;
}

bool BLERemoteCharacteristic::canWrite() {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->canWriteRemoteCharacteristic(*this);
  }

  return result;
}

bool BLERemoteCharacteristic::write(const unsigned char value[], unsigned char length) {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->writeRemoteCharacteristic(*this, value, length);
  }

  return result;
}

bool BLERemoteCharacteristic::canSubscribe() {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->canSubscribeRemoteCharacteristic(*this);
  }

  return result;
}

bool BLERemoteCharacteristic::subscribe() {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->subscribeRemoteCharacteristic(*this);
  }

  return result;
}

bool BLERemoteCharacteristic::canUnsubscribe() {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->canUnsubscribeRemoteCharacteristic(*this);
  }

  return result;
}

bool BLERemoteCharacteristic::unsubscribe() {
  bool result = false;

  if (this->_listener) {
    result = this->_listener->unsubcribeRemoteCharacteristic(*this);
  }

  return result;
}

bool BLERemoteCharacteristic::valueUpdated() {
  bool valueUpdated = this->_valueUpdated;

  this->_valueUpdated = false;

  return valueUpdated;
}

void BLERemoteCharacteristic::setEventHandler(BLERemoteCharacteristicEvent event, BLERemoteCharacteristicEventHandler eventHandler) {
  if (event < sizeof(this->_eventHandlers)) {
    this->_eventHandlers[event] = eventHandler;
  }
}

void BLERemoteCharacteristic::setValue(BLECentral& central, const unsigned char value[], unsigned char length) {
  this->_valueLength = length;
  memcpy(this->_value, value, length);

  this->_valueUpdated = true;

  BLERemoteCharacteristicEventHandler eventHandler = this->_eventHandlers[BLEValueUpdated];
  if (eventHandler) {
    eventHandler(central, *this);
  }
}

void BLERemoteCharacteristic::setValueChangeListener(BLERemoteCharacteristicValueChangeListener& listener) {
  this->_listener = &listener;
}