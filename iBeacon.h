#ifndef _I_BEACON_H_
#define _I_BEACON_H_

#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)

#include "BLEPeripheral.h"
#include "BLEUuid.h"

class iBeacon : public BLEPeripheral
{
  public:
    iBeacon();

    void begin(const char* uuidString, unsigned short major, unsigned short minor, char measuredPower);
    void loop();

  private:
    unsigned char _manufacturerData[MAX_UUID_LENGTH + 9]; // 4 bytes of header and 5 bytes of trailer.
};

#endif

#endif
