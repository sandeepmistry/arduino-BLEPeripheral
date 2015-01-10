#ifndef _NRF_51822_H_
#define _NRF_51822_H_

#include <ble_gatts.h>

#include "BLEDevice.h"

class nRF51822 : public BLEDevice
{
  friend class BLEPeripheral;

  protected:
    struct characteristicInfo {
      BLECharacteristic* characteristic;
      BLEService* service;

      ble_gatts_char_handles_t handles;
      bool notifySubscribed;
      bool indicateSubscribed;
    };

    nRF51822();

    virtual ~nRF51822();

    virtual void begin(unsigned char advertisementDataType,
                unsigned char advertisementDataLength,
                const unsigned char* advertisementData,
                unsigned char scanDataType,
                unsigned char scanDataLength,
                const unsigned char* scanData,
                BLEAttribute** attributes,
                unsigned char numAttributes);

    virtual void poll();

    virtual void startAdvertising();
    virtual void disconnect();

    virtual bool updateCharacteristicValue(BLECharacteristic& characteristic);
    virtual bool broadcastCharacteristic(BLECharacteristic& characteristic);
    virtual bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    virtual bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    virtual void requestAddress();
    virtual void requestTemperature();
    virtual void requestBatteryLevel();

  private:

    unsigned char                _advData[31];
    unsigned char                _advDataLen;
    BLECharacteristic*           _broadcastCharacteristic;

    uint16_t                     _connectionHandle;

    unsigned char                _numCharacteristics;
    struct characteristicInfo*   _characteristicInfo;
};

#endif
