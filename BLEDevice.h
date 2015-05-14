#ifndef _BLE_DEVICE_H_
#define _BLE_DEVICE_H_

#include "BLEBondStore.h"
#include "BLECharacteristic.h"
#include "BLELocalAttribute.h"
#include "BLERemoteAttribute.h"
#include "BLERemoteCharacteristic.h"
#include "BLERemoteService.h"

class BLEDevice;

class BLEDeviceEventListener
{
  public:
    virtual void BLEDeviceConnected(BLEDevice& device, const unsigned char* address) { }
    virtual void BLEDeviceDisconnected(BLEDevice& device) { }
    virtual void BLEDeviceBonded(BLEDevice& device) { }
    virtual void BLEDeviceRemoteServicesDiscovered(BLEDevice& device) { }

    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& device, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) { }
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& device, BLECharacteristic& characteristic, bool subscribed) { }

    virtual void BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& device, BLERemoteCharacteristic& remoteCharacteristic, const unsigned char* value, unsigned char valueLength) { }


    virtual void BLEDeviceAddressReceived(BLEDevice& device, const unsigned char* address) { }
    virtual void BLEDeviceTemperatureReceived(BLEDevice& device, float temperature) { }
    virtual void BLEDeviceBatteryLevelReceived(BLEDevice& device, float batteryLevel) { }
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
                unsigned char numRemoteAttributes) { }

    virtual void poll() { }

    virtual void startAdvertising() { }
    virtual void disconnect() { }

    virtual bool updateCharacteristicValue(BLECharacteristic& characteristic) { }
    virtual bool broadcastCharacteristic(BLECharacteristic& characteristic) { }
    virtual bool canNotifyCharacteristic(BLECharacteristic& characteristic) { }
    virtual bool canIndicateCharacteristic(BLECharacteristic& characteristic) { }

    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& remoteCharacteristic) { }
    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) { }
    virtual bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }
    virtual bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) { }

    virtual void requestAddress() { }
    virtual void requestTemperature() { }
    virtual void requestBatteryLevel() { }

  protected:
    unsigned short                _advertisingInterval;
    bool                          _connectable;
    BLEBondStore*                 _bondStore;
    BLEDeviceEventListener*       _eventListener;
};

#endif
