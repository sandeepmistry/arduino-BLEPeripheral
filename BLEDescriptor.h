#ifndef _BLE_DESCRIPTOR_H_
#define _BLE_DESCRIPTOR_H_

#include "BLEAttribute.h"

#define BLE_DESCRIPTOR_MAX_VALUE_LENGTH 19

class BLEDescriptor : public BLEAttribute
{
  public:
    BLEDescriptor(const char* uuid, unsigned int valueSize);

    unsigned int valueSize();

    char* value();
    unsigned int valueLength();
    void setValue(char value[], unsigned int length);

  private:
    unsigned int _valueSize;
    char _value[BLE_DESCRIPTOR_MAX_VALUE_LENGTH];
    unsigned int _valueLength;
};

#endif
