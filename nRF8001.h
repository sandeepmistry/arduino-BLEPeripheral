#ifndef _NRF_8001_H_
#define _NRF_8001_H_

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <utility/lib_aci.h>
#include <utility/aci_setup.h>

#include "BLEAttribute.h"
#include "BLECharacteristic.h"

class nRF8001;

class nRF8001EventListener
{
  public:
    virtual void nRF8001Connected(nRF8001& nRF8001, const char* address) = 0;
    virtual void nRF8001Disconnected(nRF8001& nRF8001) = 0;

    virtual void nRF8001AddressReceived(nRF8001& nRF8001, const char* address) = 0;
    virtual void nRF8001TemperatureReceived(nRF8001& nRF8001, float temperature) = 0;
    virtual void nRF8001BatteryLevelReceived(nRF8001& nRF8001, float batteryLevel) = 0;
};


class nRF8001 : public BLECharacteristicValueChangeListener
{
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

  public:
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

    void characteristicValueChanged(BLECharacteristic& characteristic);

    void disconnect();

    void requestAddress();
    void requestTemperature();
    void requestBatteryLevel();

  private:
    struct aci_state_t          _aciState;
    hal_aci_evt_t               _aciData;

    struct pipeInfo*            _pipeInfo;
    unsigned char               _numPipeInfo;

    bool                        _setupRequired;
    bool                        _isSetup;

    nRF8001EventListener*       _eventListener;
};

#endif
