#ifndef _BLE_DEVICE_H_
#define _BLE_DEVICE_H_

#include "BLEBondStore.h"
#include "BLECharacteristic.h"
#include "BLELocalAttribute.h"
#include "BLERemoteAttribute.h"
#include "BLERemoteCharacteristic.h"

class BLEDevice;

class BLEDeviceEventListener
{
  public:
    virtual void BLEDeviceConnected(BLEDevice& device, const unsigned char* address) = 0;
    virtual void BLEDeviceDisconnected(BLEDevice& device) = 0;

    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& device, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) = 0;
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& device, BLECharacteristic& characteristic, bool subscribed) = 0;

    virtual void BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& device, BLERemoteCharacteristic& remoteCharacteristic, const unsigned char* value, unsigned char valueLength) = 0;


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

    void setAdvertisingInterval(unsigned short advertisingInterval);
    void setConnectable(bool connectable);
    void setBondStore(BLEBondStore& bondStore);

    virtual void begin(unsigned char advertisementDataType,
                unsigned char advertisementDataLength,
                const unsigned char* advertisementData,
                unsigned char scanDataType,
                unsigned char scanDataLength,
                const unsigned char* scanData,
                BLELocalAttribute** localAttributes,
                unsigned char numLocalAttributes,
                BLERemoteAttribute** remoteAttributes,
                unsigned char numRemoteAttributes) = 0;

    virtual void poll() = 0;

    virtual void startAdvertising() = 0;
    virtual void disconnect() = 0;

    virtual bool updateCharacteristicValue(BLECharacteristic& characteristic) = 0;
    virtual bool broadcastCharacteristic(BLECharacteristic& characteristic) = 0;
    virtual bool canNotifyCharacteristic(BLECharacteristic& characteristic) = 0;
    virtual bool canIndicateCharacteristic(BLECharacteristic& characteristic) = 0;

    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) = 0;
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& remoteCharacteristic) = 0;
    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) = 0;
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) = 0;

    virtual void requestAddress() = 0;
    virtual void requestTemperature() = 0;
    virtual void requestBatteryLevel() = 0;

  protected:
    unsigned short                _advertisingInterval;
    bool                          _connectable;
    BLEBondStore*                 _bondStore;
    BLEDeviceEventListener*       _eventListener;
};

#endif
