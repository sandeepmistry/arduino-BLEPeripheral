// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _EDDYSTONE_BEACON_H_
#define _EDDYSTONE_BEACON_H_

#include "BLEPeripheral.h"
#include "BLEUuid.h"

class EddystoneBeacon : public BLEPeripheral
{
  public:
    EddystoneBeacon(unsigned char req = BLE_DEFAULT_REQ, unsigned char rdy = BLE_DEFAULT_RDY, unsigned char rst = BLE_DEFAULT_RST);

    void begin(char power, const BLEUuid& uid);
    void begin(char power, const char* uri);
    void loop();

    void setURI(const char* uri);

  private:
    unsigned char compressURI(const char* uri, char *compressedUri, unsigned char compressedUriSize);

    char              _power;

    BLEService        _bleService;
    BLECharacteristic _bleCharacteristic;
};

#endif
