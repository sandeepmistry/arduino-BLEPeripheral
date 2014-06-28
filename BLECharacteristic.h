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
    bool isNotifySubscribed();
    bool isIndicateSubscribed();

protected:
    unsigned short valueHandle();
    void setValueHandle(unsigned short valueHandle);

    unsigned short configHandle();
    void setConfigHandle(unsigned short configHandle);

    char pipeStart();
    void setPipeStart(char pipesStart);

    char txPipe();
    void setTxPipe(char txPipe);

    char txAckPipe();
    void setTxAckPipe(char txAckPipe);

    char rxPipe();
    void setRxPipe(char rxPipe);

    char rxAckPipe();
    void setRxAckPipe(char rxAckPipe);

    char setPipe();
    void setSetPipe(char setPipe);

    void setValueUpdated(bool valueUpdated);
    void setIsNotifySubscribed(bool isNotifySubscribed);
    void setIsIndicateSubscribed(bool isNotifySubscribed);

  private:
    char _properties;
    unsigned int _valueSize;
    char _value[BLE_CHARACTERISTIC_MAX_VALUE_LENGTH];
    unsigned int _valueLength;

    unsigned short _valueHandle;
    unsigned short _configHandle;


    char _pipeStart;
    char _txPipe;
    char _txAckPipe;
    char _rxPipe;
    char _rxAckPipe;
    char _setPipe;

    bool _valueUpdated;
    bool _isNotifySubscribed;
    bool _isIndicateSubscribed;
};

#endif
