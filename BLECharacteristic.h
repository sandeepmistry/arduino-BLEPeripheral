#ifndef _BLE_CHARACTERISTIC_H_
#define _BLE_CHARACTERISTIC_H_

#include "BLEAttribute.h"

#define BLE_CHARACTERISTIC_MAX_VALUE_LENGTH 19


class BLECharacteristic : public BLEAttribute
{
  friend class BLEPeripheral;

  public:
    BLECharacteristic(const char* uuid, char properties, unsigned int valueSize);

    char properties();
    unsigned int valueSize();

    char* value();
    unsigned int valueLength();
    void setValue(char value[], unsigned int length);

    bool valueUpdated();

protected:
    unsigned short valueHandle();
    void setValueHandle(unsigned short valueHandle);

    char pipeStart();
    void setPipeStart(char pipesStart);

    char setPipe();
    void setSetPipe(char setPipe);

    char rxPipe();
    void setRxPipe(char rxPipe);

    void setValueUpdated(bool _valueUpdated);

  private:
    char _properties;
    unsigned int _valueSize;
    char _value[BLE_CHARACTERISTIC_MAX_VALUE_LENGTH];
    unsigned int _valueLength;

    unsigned short _valueHandle;

    char _pipeStart;
    char _setPipe;
    char _rxPipe;

    bool _valueUpdated;
};

#endif
