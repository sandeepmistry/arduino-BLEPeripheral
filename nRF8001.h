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

struct nRF8001PipeInfo {
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

class nRF8001 : public BLECharacteristicValueListener
{
  public:
    nRF8001(unsigned char req, unsigned char rdy, unsigned char rst);

    virtual ~nRF8001();
  
    void begin(const unsigned char* advertisementData,
                unsigned char advertisementDataLength,
                const unsigned char* scanData,
                unsigned char scanDataLength,
                BLEAttribute** attributes,
                unsigned char numAttributes);
    void poll();

    void characteristicValueUpdated(BLECharacteristic& characteristic);

  private:
    struct aci_state_t          _aciState;
    hal_aci_evt_t               _aciData;

    struct nRF8001PipeInfo*     _pipeInfo;
    unsigned char               _numPipeInfo;

    bool                        _setupRequired;
    bool                        _isSetup;
};

#endif
