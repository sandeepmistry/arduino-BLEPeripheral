#ifndef _NRF_8001_H_
#define _NRF_8001_H_

#include "Arduino.h"

#include <utility/lib_aci.h>
#include <utility/aci_setup.h>

#include "BLEDevice.h"

class nRF8001 : protected BLEDevice
{
  friend class BLEPeripheral;

  protected:
    struct pipeInfo {
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

    nRF8001(unsigned char req, unsigned char rdy, unsigned char rst);

    virtual ~nRF8001();

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
    void waitForSetupMode();
    void sendSetupMessage(hal_aci_data_t* data);
    void sendSetupMessage(hal_aci_data_t* setupMsg, unsigned char type, unsigned short& offset);
    void sendCrc();

  private:
    struct aci_state_t          _aciState;
    hal_aci_evt_t               _aciData;

    struct pipeInfo*            _pipeInfo;
    unsigned char               _numPipeInfo;
    unsigned char               _broadcastPipe;

    bool                        _newBond;
    unsigned char*              _dynamicData;
    unsigned char               _dynamicDataOffset;
    unsigned char               _dynamicDataSequenceNo;

    unsigned short              _crcSeed;
};

#endif
