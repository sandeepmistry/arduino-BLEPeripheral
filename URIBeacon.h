#ifndef _URI_BEACON_H_
#define _URI_BEACON_H_

#include "BLEPeripheral.h"

class URIBeacon
{
  public:
    URIBeacon(unsigned char req, unsigned char rdy, unsigned char rst);

    void begin(unsigned char flags, unsigned char power, const char* uri);
    void loop();

  private:
    unsigned char compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize);

    BLEPeripheral     _blePeripheral;
    BLEService        _bleService;
    BLECharacteristic _bleCharacteristic;
};

#endif
