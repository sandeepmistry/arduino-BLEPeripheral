#ifndef _BLE_PROGMEM_CONSTANT_CHARACTERISTIC_H_
#define _BLE_PROGMEM_CONSTANT_CHARACTERISTIC_H_

#include "Arduino.h"

#include "BLEConstantCharacteristic.h"

class BLEProgmemConstantCharacteristic : public BLEConstantCharacteristic {
  public:
    BLEProgmemConstantCharacteristic(const char* uuid, const unsigned char value[], unsigned char length);
    BLEProgmemConstantCharacteristic(const char* uuid, const char* value);

    virtual ~BLEProgmemConstantCharacteristic();

    virtual unsigned char operator[] (int offset) const;
};

#endif
