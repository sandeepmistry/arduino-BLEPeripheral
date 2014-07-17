#ifndef _BLE_CHARACTERISTIC_H_
#define _BLE_CHARACTERISTIC_H_

#include "BLEAttribute.h"

enum BLEProperty {
  BLEPropertyRead                 = 0x02,
  BLEPropertyWriteWithoutResponse = 0x04,
  BLEPropertyWrite                = 0x08,
  BLEPropertyNotify               = 0x10,
  BLEPropertyIndicate             = 0x20
};

typedef void (*BLECharacteristicNewValueHandler)();

class BLECharacteristic;

class BLECharacteristicValueChangeListener
{
  public:
    virtual void characteristicValueChanged(BLECharacteristic& characteristic) = 0;
};

class BLECharacteristic : public BLEAttribute
{
  public:
    BLECharacteristic(const char* uuid, unsigned char properties, unsigned char valueSize);
    BLECharacteristic(const char* uuid, unsigned char properties, const char* value);

    virtual ~BLECharacteristic();

    unsigned char properties();
    unsigned char valueSize();

    const unsigned char* value();
    unsigned char valueLength();
    void setValue(const unsigned char value[], unsigned char length);

    void setValue(const char* value);

    bool hasNotifySubscriber();
    void setHasNotifySubscriber(bool hasNotifySubscriber);

    bool hasIndicateSubscriber();
    void setHasIndicateSubscriber(bool hasIndicateSubscriber);

    bool hasNewValue();
    void setHasNewValue(bool hasNewValue);
    void setNewValueHandler(BLECharacteristicNewValueHandler newValueHandler);

    void setCharacteristicValueListener(BLECharacteristicValueChangeListener& listener);

  private:
    unsigned char                         _properties;
    unsigned char                         _valueSize;
    unsigned char*                        _value;
    unsigned char                         _valueLength;

    bool                                  _hasNotifySubscriber;
    bool                                  _hasIndicateSubscriber;
    bool                                  _hasNewValue;
    BLECharacteristicNewValueHandler      _newValueHandler;
    BLECharacteristicValueChangeListener* _listener;
};

#endif
