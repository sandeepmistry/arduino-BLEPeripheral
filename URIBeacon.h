#ifndef _URI_BEACON_H_
#define _URI_BEACON_H_

#include "BLEPeripheral.h"

class URIBeacon
{
  public:
    URIBeacon(unsigned char req, unsigned char rdy, unsigned char rst);

    void begin(unsigned char flags, unsigned char power, const char* uri);
    void loop();

    void setURI(const char* uri);

    void setLocalName(const char *localName);
    void setConnectable(bool connectable);
    void addAttribute(BLELocalAttribute& attribute);
    void setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler);

  private:
    unsigned char compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize);

    unsigned char     _flags;
    unsigned char     _power;

    BLEPeripheral     _blePeripheral;
    BLEService        _bleService;
    BLECharacteristic _bleCharacteristic;
};

#endif
