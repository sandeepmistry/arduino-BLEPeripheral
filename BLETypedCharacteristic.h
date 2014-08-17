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

    void setValueLE(T value);
    T valueLE();

    void setValueBE(T value);
    T valueBE();

  private:
    T byteSwap(T value);
};

template<typename T> BLETypedCharacteristic<T>::BLETypedCharacteristic(const char* uuid, unsigned char properties) :
  BLECharacteristic(uuid, properties, sizeof(T), true)
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

template<typename T> void BLETypedCharacteristic<T>::setValueLE(T value) {
  this->setValue(value);
}

template<typename T> T BLETypedCharacteristic<T>::valueLE() {
  return this->getValue();
}

template<typename T> void BLETypedCharacteristic<T>::setValueBE(T value) {
  this->setValue(this->byteSwap(value));
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
