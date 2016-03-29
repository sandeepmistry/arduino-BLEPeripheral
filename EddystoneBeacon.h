#ifndef _EDDYSTONE_BEACON_H_
#define _EDDYSTONE_BEACON_H_

#include "BLEPeripheral.h"
#include "BLEUuid.h"

class EddystoneBeacon : public BLEPeripheral
{
  public:
    EddystoneBeacon(unsigned char req, unsigned char rdy, unsigned char rst);

    void begin(char power, const BLEUuid& uid);
    void begin(char power, const char* uri);
    void begin(const unsigned char* temp, unsigned char len); //For TLM
    void loop();

    void setURI(const char* uri);

  private:
    unsigned char compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize);

    char              _power;

    BLEService        _bleService;
    BLECharacteristic _bleCharacteristic;
};

#endif
