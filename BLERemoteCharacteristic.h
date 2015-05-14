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
    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }

    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) { }

    virtual bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
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
