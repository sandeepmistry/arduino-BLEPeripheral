#ifndef _BLE_FIXED_LENGTH_CHARACTERISTIC_H_
#define _BLE_FIXED_LENGTH_CHARACTERISTIC_H_

#include "BLECharacteristic.h"

class BLEFixedLengthCharacteristic : public BLECharacteristic {
  public:
    BLEFixedLengthCharacteristic(const char* uuid, unsigned char properties, unsigned char valueSize);
    BLEFixedLengthCharacteristic(const char* uuid, unsigned char properties, const char* value);

    virtual bool fixedLength() const;
};

#endif
