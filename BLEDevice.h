#ifndef _BLE_DEVICE_H_
#define _BLE_DEVICE_H_

#include "BLEAttribute.h"
#include "BLECharacteristic.h"

#define ADVERTISING_INTERVAL 0x0A0

class BLEDevice;

class BLEDeviceEventListener
{
  public:
    virtual void BLEDeviceConnected(BLEDevice& device, const unsigned char* address) = 0;
    virtual void BLEDeviceDisconnected(BLEDevice& device) = 0;

    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& device, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) = 0;
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& device, BLECharacteristic& characteristic, bool subscribed) = 0;

    virtual void BLEDeviceAddressReceived(BLEDevice& device, const unsigned char* address) = 0;
    virtual void BLEDeviceTemperatureReceived(BLEDevice& device, float temperature) = 0;
    virtual void BLEDeviceBatteryLevelReceived(BLEDevice& device, float batteryLevel) = 0;
};


class BLEDevice
{
  friend class BLEPeripheral;

  protected:
    BLEDevice();

    virtual ~BLEDevice();

    void setEventListener(BLEDeviceEventListener* eventListener);

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

    virtual void disconnect() = 0;

    virtual bool updateCharacteristicValue(BLECharacteristic& characteristic) = 0;
    virtual bool broadcastCharacteristic(BLECharacteristic& characteristic) = 0;
    virtual bool canNotifyCharacteristic(BLECharacteristic& characteristic) = 0;
    virtual bool canIndicateCharacteristic(BLECharacteristic& characteristic) = 0;

    virtual void requestAddress() = 0;
    virtual void requestTemperature() = 0;
    virtual void requestBatteryLevel() = 0;

  protected:
    bool                          _connectable;
    BLEDeviceEventListener*       _eventListener;
};

#endif
