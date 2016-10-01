// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_CHARACTERISTIC_H_
#define _BLE_CHARACTERISTIC_H_

#include "BLELocalAttribute.h"

enum BLECharacteristicEvent {
  BLEWritten = 0,
  BLESubscribed = 1,
  BLEUnsubscribed = 2
};

class BLECentral;
class BLECharacteristic;

typedef void (*BLECharacteristicEventHandler)(BLECentral& central, BLECharacteristic& characteristic);

class BLECharacteristicValueChangeListener
{
  public:
    virtual bool characteristicValueChanged(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool broadcastCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool canNotifyCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool canIndicateCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }
};

class BLECharacteristic : public BLELocalAttribute
{
  friend class BLEPeripheral;

  public:
    BLECharacteristic(const char* uuid, unsigned char properties, unsigned char valueSize);
    BLECharacteristic(const char* uuid, unsigned char properties, const char* value);

    virtual ~BLECharacteristic();

    unsigned char properties() const;

    virtual unsigned char valueSize() const;
    virtual const unsigned char* value() const;
    virtual unsigned char valueLength() const;
    virtual unsigned char operator[] (int offset) const;

    virtual bool fixedLength() const;

    virtual bool setValue(const unsigned char value[], unsigned char length);
    virtual bool setValue(const char* value);

    bool broadcast();

    bool written();
    bool subscribed();
    bool canNotify();
    bool canIndicate();

    void setEventHandler(BLECharacteristicEvent event, BLECharacteristicEventHandler eventHandler);

  protected:
    virtual void setValue(BLECentral& central, const unsigned char value[], unsigned char length);
    void setSubscribed(BLECentral& central, bool written);

    void setValueChangeListener(BLECharacteristicValueChangeListener& listener);

    unsigned char                         _valueSize;
    unsigned char*                        _value;
    unsigned char                         _valueLength;

  private:
    unsigned char                         _properties;

    bool                                  _written;
    bool                                  _subscribed;

    BLECharacteristicValueChangeListener* _listener;
    BLECharacteristicEventHandler         _eventHandlers[3];
};

#endif
