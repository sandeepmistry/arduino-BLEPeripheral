#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "nRF8001.h"

#include "BLEAttribute.h"
#include "BLEService.h"
#include "BLECharacteristic.h"
#include "BLEDescriptor.h"

class BLEPeripheral
{
  public:
    BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst);
  
    void begin();
    void poll();

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setManufacturerData(const unsigned char* manufacturerData, unsigned char manufacturerDataLength);
    void setLocalName(const char *localName);

    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    void addAttribute(BLEAttribute& attribute);

  private:
    nRF8001                   _nRF8001;

    const char*               _advertisedServiceUuid;
    const unsigned char*      _manufacturerData;
    unsigned char             _manufacturerDataLength;
    const char*               _localName;

    BLEAttribute**            _attributes;
    unsigned char             _numAttributes;


    BLEService                _genericAccessService;
    BLECharacteristic         _deviceNameCharacteristic;
    BLECharacteristic         _appearanceCharacteristic;
};

#endif
