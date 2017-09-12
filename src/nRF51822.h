// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _NRF_51822_H_
#define _NRF_51822_H_

#if defined(__RFduino__)
  #include <utility/RFduino/ble_gatts.h>
  #include <utility/RFduino/ble_gattc.h>
#elif defined(NRF5) || defined(NRF51_S130)
  #include <ble_gatts.h>
  #include <ble_gattc.h>
  #include <nrf_soc.h>
#elif defined(NRF52) && defined(S132) // ARDUINO_RBL_nRF52832
  #ifndef ARDUINO_RBL_nRF52832
    #define ARDUINO_RBL_nRF52832
  #endif
  #define NRF5

  #include <sdk/softdevice/s132/headers/nrf_ble_gatts.h>
  #include <sdk/softdevice/s132/headers/nrf_ble_gattc.h>
  #include <sdk/softdevice/s132/headers/nrf_soc.h>
#else
  #include <s110/ble_gatts.h>
  #include <s110/ble_gattc.h>
#endif

#include "BLEDevice.h"

class nRF51822 : public BLEDevice
{
  friend class BLEPeripheral;

  protected:
    struct localCharacteristicInfo {
      BLECharacteristic* characteristic;
      BLEService* service;

      ble_gatts_char_handles_t handles;
      bool notifySubscribed;
      bool indicateSubscribed;
    };

    struct remoteServiceInfo {
      BLERemoteService* service;

      ble_uuid_t uuid;
      ble_gattc_handle_range_t handlesRange;
    };

    struct remoteCharacteristicInfo {
      BLERemoteCharacteristic* characteristic;
      BLERemoteService* service;

      ble_uuid_t uuid;
      ble_gatt_char_props_t properties;
      uint16_t valueHandle;
    };

    nRF51822();

    virtual ~nRF51822();

    virtual void begin(unsigned char advertisementDataSize,
                BLEEirData *advertisementData,
                unsigned char scanDataSize,
                BLEEirData *scanData,
                BLELocalAttribute** localAttributes,
                unsigned char numLocalAttributes,
                BLERemoteAttribute** remoteAttributes,
                unsigned char numRemoteAttributes);

    virtual void poll();

    virtual void end();

    virtual bool setTxPower(int txPower);
    virtual void startAdvertising();
    virtual void disconnect();

    virtual bool updateCharacteristicValue(BLECharacteristic& characteristic);
    virtual bool broadcastCharacteristic(BLECharacteristic& characteristic);
    virtual bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    virtual bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    virtual bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool readRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length);
    virtual bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    virtual bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);

    virtual void requestAddress();
    virtual void requestTemperature();
    virtual void requestBatteryLevel();

  private:

    unsigned char                     _advData[31];
    unsigned char                     _advDataLen;
    bool                              _hasScanData;
    BLECharacteristic*                _broadcastCharacteristic;

    uint16_t                          _connectionHandle;
#if defined(NRF5) || defined(NRF51_S130)
    uint8_t                           _bondData[((sizeof(ble_gap_enc_key_t) + 3) / 4) * 4]  __attribute__ ((__aligned__(4)));
    ble_gap_enc_key_t*                _encKey;
#else
    uint8_t                           _authStatusBuffer[((sizeof(ble_gap_evt_auth_status_t) + 3) / 4) * 4]  __attribute__ ((__aligned__(4)));
    ble_gap_evt_auth_status_t*        _authStatus;
#endif
    unsigned char                     _txBufferCount;

    unsigned char                     _numLocalCharacteristics;
    struct localCharacteristicInfo*   _localCharacteristicInfo;

    unsigned char                     _numRemoteServices;
    struct remoteServiceInfo*         _remoteServiceInfo;
    unsigned char                     _remoteServiceDiscoveryIndex;
    unsigned char                     _numRemoteCharacteristics;
    struct remoteCharacteristicInfo*  _remoteCharacteristicInfo;
    bool                              _remoteRequestInProgress;
};

#endif
