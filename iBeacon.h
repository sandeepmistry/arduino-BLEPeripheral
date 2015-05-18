#ifndef _I_BEACON_H_
#define _I_BEACON_H_

#if defined(NRF51) || !defined(__RFduino__)

#include "BLEPeripheral.h"

class iBeacon
{
  public:
    static void setData(BLEPeripheral& peripheral, const char* uuidString, uint16_t major, uint16_t minor, int8_t measuredPower);
};

#endif

#endif
