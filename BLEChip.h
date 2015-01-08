#ifndef _BLE_CHIP_H_
#define _BLE_CHIP_H_

#include "BLEAttribute.h"
#include "BLECharacteristic.h"

class BLEChip;

class BLEChipEventListener
{
  public:
    virtual void BLEChipConnected(BLEChip& chip, const unsigned char* address) = 0;
    virtual void BLEChipDisconnected(BLEChip& chip) = 0;

    virtual void BLEChipCharacteristicValueChanged(BLEChip& chip, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) = 0;
    virtual void BLEChipCharacteristicSubscribedChanged(BLEChip& chip, BLECharacteristic& characteristic, bool subscribed) = 0;

    virtual void BLEChipAddressReceived(BLEChip& chip, const unsigned char* address) = 0;
    virtual void BLEChipTemperatureReceived(BLEChip& chip, float temperature) = 0;
    virtual void BLEChipBatteryLevelReceived(BLEChip& chip, float batteryLevel) = 0;
};


class BLEChip
{
  friend class BLEPeripheral;

  protected:
    BLEChip();

    virtual ~BLEChip();

    void setEventListener(BLEChipEventListener* eventListener);

    void setAdvertisingInterval(unsigned short advertisingInterval);
    void setConnectable(bool connectable);

    virtual void begin(unsigned char advertisementDataType,
                unsigned char advertisementDataLength,
                const unsigned char* advertisementData,
                unsigned char scanDataType,
                unsigned char scanDataLength,
                const unsigned char* scanData,
                BLEAttribute** attributes,
                unsigned char numAttributes) = 0;

    virtual void poll() = 0;

    virtual void startAdvertising() = 0;
    virtual void disconnect() = 0;

    virtual bool updateCharacteristicValue(BLECharacteristic& characteristic) = 0;
    virtual bool broadcastCharacteristic(BLECharacteristic& characteristic) = 0;
    virtual bool canNotifyCharacteristic(BLECharacteristic& characteristic) = 0;
    virtual bool canIndicateCharacteristic(BLECharacteristic& characteristic) = 0;

    virtual void requestAddress() = 0;
    virtual void requestTemperature() = 0;
    virtual void requestBatteryLevel() = 0;

  protected:
    unsigned short                _advertisingInterval;
    bool                          _connectable;
    BLEChipEventListener*         _eventListener;
};

#endif
