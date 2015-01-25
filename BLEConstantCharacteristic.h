#ifndef _BLE_CONSTANT_CHARACTERISTIC_H_
#define _BLE_CONSTANT_CHARACTERISTIC_H_

#include "Arduino.h"

#include "BLEFixedLengthCharacteristic.h"

class BLEConstantCharacteristic : public BLEFixedLengthCharacteristic {
  public:
    BLEConstantCharacteristic(const char* uuid, const unsigned char value[], unsigned char length);
    BLEConstantCharacteristic(const char* uuid, const char* value);

    virtual ~BLEConstantCharacteristic();

    virtual bool constantValue() const;

    virtual bool setValue(const unsigned char value[], unsigned char length);
    virtual bool setValue(const char* value);
};

#endif
