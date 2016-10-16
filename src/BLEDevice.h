// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _BLE_DEVICE_H_
#define _BLE_DEVICE_H_

#include "BLEBondStore.h"
#include "BLECharacteristic.h"
#include "BLELocalAttribute.h"
#include "BLERemoteAttribute.h"
#include "BLERemoteCharacteristic.h"
#include "BLERemoteService.h"

struct BLEEirData
{
  unsigned char length;
  unsigned char type;
  unsigned char data[BLE_EIR_DATA_MAX_VALUE_LENGTH];
};

class BLEDevice;

class BLEDeviceEventListener
{
  public:
    virtual void BLEDeviceConnected(BLEDevice& /*device*/, const unsigned char* /*address*/) { }
    virtual void BLEDeviceDisconnected(BLEDevice& /*device*/) { }
    virtual void BLEDeviceBonded(BLEDevice& /*device*/) { }
    virtual void BLEDeviceRemoteServicesDiscovered(BLEDevice& /*device*/) { }

    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& /*device*/, BLECharacteristic& /*characteristic*/, const unsigned char* /*value*/, unsigned char /*valueLength*/) { }
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& /*device*/, BLECharacteristic& /*characteristic*/, bool /*subscribed*/) { }

    virtual void BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& /*device*/, BLERemoteCharacteristic& /*characteristic*/, const unsigned char* /*value*/, unsigned char /*valueLength*/) { }


    virtual void BLEDeviceAddressReceived(BLEDevice& /*device*/, const unsigned char* /*address*/) { }
    virtual void BLEDeviceTemperatureReceived(BLEDevice& /*device*/, float /*temperature*/) { }
    virtual void BLEDeviceBatteryLevelReceived(BLEDevice& /*device*/, float /*batteryLevel*/) { }
};


class BLEDevice
{
  friend class BLEPeripheral;

  protected:
    BLEDevice();

    virtual ~BLEDevice();

    void setEventListener(BLEDeviceEventListener* eventListener);

    void setAdvertisingInterval(unsigned short advertisingInterval);
    void setConnectionInterval(unsigned short minimumConnectionInterval, unsigned short maximumConnectionInterval);
    void setConnectable(bool connectable);
    void setBondStore(BLEBondStore& bondStore);

    virtual void begin(unsigned char /*advertisementDataSize*/,
                BLEEirData * /*advertisementData*/,
                unsigned char /*scanDataSize*/,
                BLEEirData * /*scanData*/,
                BLELocalAttribute** /*localAttributes*/,
                unsigned char /*numLocalAttributes*/,
                BLERemoteAttribute** /*remoteAttributes*/,
                unsigned char /*numRemoteAttributes*/) { }

    virtual void poll() { }

    virtual void end() { }

    virtual bool setTxPower(int /*txPower*/) { return false; }

    virtual void startAdvertising() { }
    virtual void disconnect() { }

    virtual bool updateCharacteristicValue(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool broadcastCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool canNotifyCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }
    virtual bool canIndicateCharacteristic(BLECharacteristic& /*characteristic*/) { return false; }

    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/, const unsigned char /*value*/[], unsigned char /*length*/) { return false; }
    virtual bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }
    virtual bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& /*characteristic*/) { return false; }

    virtual void requestAddress() { }
    virtual void requestTemperature() { }
    virtual void requestBatteryLevel() { }

  protected:
    unsigned short                _advertisingInterval;
    unsigned short                _minimumConnectionInterval;
    unsigned short                _maximumConnectionInterval;
    bool                          _connectable;
    BLEBondStore*                 _bondStore;
    BLEDeviceEventListener*       _eventListener;
};

#endif
