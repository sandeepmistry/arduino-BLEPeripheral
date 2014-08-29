#ifndef _NRF_8001_H_
#define _NRF_8001_H_

#include "Arduino.h"

#include <utility/lib_aci.h>
#include <utility/aci_setup.h>

#include "BLEAttribute.h"
#include "BLECharacteristic.h"

class nRF8001;

class nRF8001EventListener
{
  public:
    virtual void nRF8001Connected(nRF8001& nRF8001, const unsigned char* address) = 0;
    virtual void nRF8001Disconnected(nRF8001& nRF8001) = 0;

    virtual void nRF8001CharacteristicValueChanged(nRF8001& nRF8001, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) = 0;
    virtual void nRF8001CharacteristicSubscribedChanged(nRF8001& nRF8001, BLECharacteristic& characteristic, bool subscribed) = 0;

    virtual void nRF8001AddressReceived(nRF8001& nRF8001, const unsigned char* address) = 0;
    virtual void nRF8001TemperatureReceived(nRF8001& nRF8001, float temperature) = 0;
    virtual void nRF8001BatteryLevelReceived(nRF8001& nRF8001, float batteryLevel) = 0;
};


class nRF8001
{
  friend class BLEPeripheral;

  protected:
    struct pipeInfo {
      BLECharacteristic* characteristic;

      unsigned short     valueHandle;
      unsigned short     configHandle;

      unsigned char      startPipe;
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

    void setEventListener(nRF8001EventListener* eventListener);

    void begin(const unsigned char* advertisementData,
                unsigned char advertisementDataLength,
                const unsigned char* scanData,
                unsigned char scanDataLength,
                BLEAttribute** attributes,
                unsigned char numAttributes);

    void poll();

    void disconnect();

    void updateCharacteristicValue(BLECharacteristic& characteristic);

    void requestAddress();
    void requestTemperature();
    void requestBatteryLevel();

  private:
    void waitForSetupMode();
    void sendSetupMessage(hal_aci_data_t* data);
    void sendCrc();

  private:
    struct aci_state_t          _aciState;
    hal_aci_evt_t               _aciData;

    struct pipeInfo*            _pipeInfo;
    unsigned char               _numPipeInfo;

    unsigned short              _crcSeed;

    nRF8001EventListener*       _eventListener;
};

#endif
