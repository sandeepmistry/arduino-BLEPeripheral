#ifndef _BLE_CHARACTERISTIC_T_H_
#define _BLE_CHARACTERISTIC_T_H_

#include "BLECharacteristic.h"

template<typename T> class BLECharacteristicT : public BLECharacteristic
{
  public:
    BLECharacteristicT(const char* uuid, unsigned char properties);
 
    void setValue(T value);
    T value();
};

template<typename T> BLECharacteristicT<T>::BLECharacteristicT(const char* uuid, unsigned char properties) :
  BLECharacteristic(uuid, properties, sizeof(T))
{
  T value;
  memset(&value, 0x00, sizeof(value));

  this->setValue(value);
}

template<typename T> void BLECharacteristicT<T>::setValue(T value) {
  this->BLECharacteristic::setValue((unsigned char*)&value, sizeof(T));
}

template<typename T> T BLECharacteristicT<T>::value() {
  T value;

  memcpy(&value, (unsigned char*)this->BLECharacteristic::value(), this->BLECharacteristic::valueSize());

  return value;
}

#endif
