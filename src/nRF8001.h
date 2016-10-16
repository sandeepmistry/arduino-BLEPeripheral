// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _NRF_8001_H_
#define _NRF_8001_H_

#include "Arduino.h"

#include <utility/lib_aci.h>

#include "BLEDevice.h"

class nRF8001 : protected BLEDevice
{
  friend class BLEPeripheral;

  protected:
    struct localPipeInfo {
      BLECharacteristic* characteristic;

      unsigned short     valueHandle;
      unsigned short     configHandle;

      unsigned char      advPipe;
      unsigned char      txPipe;
      unsigned char      txAckPipe;
      unsigned char      rxPipe;
      unsigned char      rxAckPipe;
      unsigned char      setPipe;

      bool               txPipeOpen;
      bool               txAckPipeOpen;
    };

    struct remotePipeInfo {
      BLERemoteCharacteristic*  characteristic;

      unsigned char             txPipe;
      unsigned char             txAckPipe;
      unsigned char             rxPipe;
      unsigned char             rxAckPipe;
      unsigned char             rxReqPipe;
    };

    nRF8001(unsigned char req, unsigned char rdy, unsigned char rst);

    virtual ~nRF8001();

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
    void waitForSetupMode();
    void sendSetupMessage(hal_aci_data_t* data, bool withCrc = false);
    void sendSetupMessage(hal_aci_data_t* setupMsg, unsigned char type, unsigned short& offset, bool withCrc = false);

    struct localPipeInfo* localPipeInfoForCharacteristic(BLECharacteristic& characteristic);
    struct remotePipeInfo* remotePipeInfoForCharacteristic(BLERemoteCharacteristic& characteristic);

  private:
    struct aci_state_t          _aciState;
    hal_aci_evt_t               _aciData;

    struct localPipeInfo*       _localPipeInfo;
    unsigned char               _numLocalPipeInfo;
    unsigned char               _broadcastPipe;

    bool                        _timingChanged;
    bool                        _closedPipesCleared;
    bool                        _remoteServicesDiscovered;
    struct remotePipeInfo*      _remotePipeInfo;
    unsigned char               _numRemotePipeInfo;

    unsigned int                _dynamicDataOffset;
    unsigned char               _dynamicDataSequenceNo;
    bool                        _storeDynamicData;

    unsigned short              _crcSeed;
};

#endif
