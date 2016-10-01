// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_TYPED_CHARACTERISTIC_H_
#define _BLE_TYPED_CHARACTERISTIC_H_

#include "Arduino.h"

#include "BLEFixedLengthCharacteristic.h"

template<typename T> class BLETypedCharacteristic : public BLEFixedLengthCharacteristic
{
  public:
    BLETypedCharacteristic(const char* uuid, unsigned char properties);

    bool setValue(T value);
    T value();

    bool setValueLE(T value);
    T valueLE();

    bool setValueBE(T value);
    T valueBE();

  private:
    T byteSwap(T value);
};

template<typename T> BLETypedCharacteristic<T>::BLETypedCharacteristic(const char* uuid, unsigned char properties) :
  BLEFixedLengthCharacteristic(uuid, properties, sizeof(T))
{
  T value;
  memset(&value, 0x00, sizeof(value));

  this->setValue(value);
}

template<typename T> bool BLETypedCharacteristic<T>::setValue(T value) {
  return this->BLECharacteristic::setValue((unsigned char*)&value, sizeof(T));
}

template<typename T> T BLETypedCharacteristic<T>::value() {
  T value;

  memcpy(&value, (unsigned char*)this->BLECharacteristic::value(), this->BLECharacteristic::valueSize());

  return value;
}

template<typename T> bool BLETypedCharacteristic<T>::setValueLE(T value) {
  return this->setValue(value);
}

template<typename T> T BLETypedCharacteristic<T>::valueLE() {
  return this->getValue();
}

template<typename T> bool BLETypedCharacteristic<T>::setValueBE(T value) {
  return this->setValue(this->byteSwap(value));
}

template<typename T> T BLETypedCharacteristic<T>::valueBE() {
  return this->byteSwap(this->value());
}

template<typename T> T BLETypedCharacteristic<T>::byteSwap(T value) {
  T result;
  unsigned char* src = (unsigned char*)&value;
  unsigned char* dst = (unsigned char*)&result;

  for (int i = 0; i < sizeof(T); i++) {
    dst[i] = src[sizeof(T) - i - 1];
  }

  return result;
}

#endif
