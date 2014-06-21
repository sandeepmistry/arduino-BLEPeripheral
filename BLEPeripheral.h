#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class BLEPeripheral
{
  public:
    BLEPeripheral(int8_t req, int8_t rdy, int8_t rst);
  
    bool begin();
    void poll(void);

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setLocalName(const char *localName);

    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

  private:
    const char*    _localName;
    const char*    _advertisedServiceUuid;
};

#endif