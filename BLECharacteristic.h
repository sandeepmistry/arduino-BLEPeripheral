#ifndef _BLE_CHARACTERISTIC_H_
#define _BLE_CHARACTERISTIC_H_

#include "BLEAttribute.h"

#define BLE_CHARACTERISTIC_MAX_VALUE_LENGTH 19

class BLECharacteristic : public BLEAttribute
{
  public:
    BLECharacteristic(const char* uuid, char properties, unsigned int valueSize);

    char properties();
    unsigned int valueSize();

    char* value();
    unsigned int valueLength();
    void setValue(char value[], unsigned int length);

    unsigned short valueHandle();
    void setValueHandle(unsigned short valueHandle);

  private:
    char _properties;
    unsigned int _valueSize;
    char _value[BLE_CHARACTERISTIC_MAX_VALUE_LENGTH];
    unsigned int _valueLength;

    unsigned short _valueHandle;
};

#endif
