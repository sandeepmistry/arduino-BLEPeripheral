#ifndef _BLE_TYPED_CHARACTERISTIC_H_
#define _BLE_TYPED_CHARACTERISTIC_H_

#include "Arduino.h"

#include "BLECharacteristic.h"

template<typename T> class BLETypedCharacteristic : public BLECharacteristic
{
  public:
    BLETypedCharacteristic(const char* uuid, unsigned char properties);

    void setValue(T value);
    T value();
};

template<typename T> BLETypedCharacteristic<T>::BLETypedCharacteristic(const char* uuid, unsigned char properties) :
  BLECharacteristic(uuid, properties, sizeof(T))
{
  T value;
  memset(&value, 0x00, sizeof(value));

  this->setValue(value);
}

template<typename T> void BLETypedCharacteristic<T>::setValue(T value) {
  this->BLECharacteristic::setValue((unsigned char*)&value, sizeof(T));
}

template<typename T> T BLETypedCharacteristic<T>::value() {
  T value;

  memcpy(&value, (unsigned char*)this->BLECharacteristic::value(), this->BLECharacteristic::valueSize());

  return value;
}

#endif
