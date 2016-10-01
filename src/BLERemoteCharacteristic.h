// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_REMOTE_CHARACTERISTIC_H_
#define _BLE_REMOTE_CHARACTERISTIC_H_

#include "BLERemoteAttribute.h"
#include "BLEDeviceLimits.h"

enum BLERemoteCharacteristicEvent {
  BLEValueUpdated = 0
};

class BLECentral;
class BLERemoteCharacteristic;

class BLERemoteCharacteristicValueChangeListener
{
  public:
    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }

    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/, const unsigned char /*value*/[], unsigned char /*length*/) { return false; }

    virtual bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
};

typedef void (*BLERemoteCharacteristicEventHandler)(BLECentral& central, BLERemoteCharacteristic& characteristic);

class BLERemoteCharacteristic : public BLERemoteAttribute
{
  friend class BLEPeripheral;

  public:
    BLERemoteCharacteristic(const char* uuid, unsigned char properties);

    virtual ~BLERemoteCharacteristic();

    unsigned char properties() const;

    const unsigned char* value() const;
    unsigned char valueLength() const;

    bool canRead();
    bool read();
    bool canWrite();
    bool write(const unsigned char value[], unsigned char length);
    bool canSubscribe();
    bool subscribe();
    bool canUnsubscribe();
    bool unsubscribe();

    bool valueUpdated();

    void setEventHandler(BLERemoteCharacteristicEvent event, BLERemoteCharacteristicEventHandler eventHandler);

  protected:
    void setValue(BLECentral& central, const unsigned char value[], unsigned char length);

    void setValueChangeListener(BLERemoteCharacteristicValueChangeListener& listener);

  private:
    unsigned char                                     _properties;

    unsigned char                                     _valueLength;
    unsigned char                                     _value[BLE_REMOTE_ATTRIBUTE_MAX_VALUE_LENGTH];

    bool                                              _valueUpdated;

    BLERemoteCharacteristicValueChangeListener*       _listener;
    BLERemoteCharacteristicEventHandler               _eventHandlers[1];
};

#endif
