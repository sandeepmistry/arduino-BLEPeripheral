#ifndef _I_BEACON_H_
#define _I_BEACON_H_

#if defined(NRF51) || defined(__RFduino__)

#include "BLEPeripheral.h"

class iBeacon : public BLEPeripheral
{
  public:
    iBeacon();

    void begin(const char* uuidString, unsigned short major, unsigned short minor, char measuredPower);
    void loop();
};

#endif

#endif
