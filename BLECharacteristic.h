#ifndef _BLE_CHARACTERISTIC_H_
#define _BLE_CHARACTERISTIC_H_

#include "BLEAttribute.h"

#define BLEPropertyRead                         0x02
#define BLEPropertyWriteWithoutResponse         0x04
#define BLEPropertyWrite                        0x08
#define BLEPropertyNotify                       0x10
#define BLEPropertyIndicate                     0x20

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
    BLECharacteristic(const char* uuid, unsigned char properties, char* value);

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

    void setCharacteristicValueListener(BLECharacteristicValueChangeListener& listener);

  private:
    unsigned char                         _properties;
    unsigned char                         _valueSize;
    unsigned char*                        _value;
    unsigned char                         _valueLength;

    bool                                  _hasNotifySubscriber;
    bool                                  _hasIndicateSubscriber;
    bool                                  _hasNewValue;
    BLECharacteristicValueChangeListener* _listener;
};

template<class T> class BLECharacteristicTyped : public BLECharacteristic
{
    BLECharacteristicTyped(const char* uuid, unsigned char properties, T value);

    void setValue(T value);
    T value();
};

#endif
