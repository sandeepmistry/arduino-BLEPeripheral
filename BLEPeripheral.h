#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "BLEAttribute.h"
#include "BLEService.h"
#include "BLECharacteristic.h"
#include "BLEDescriptor.h"

class BLEPeripheral
{
  friend class BLECharacteristic;

  public:
    BLEPeripheral(int8_t req, int8_t rdy, int8_t rst);
  
    bool begin();
    void poll(void);

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setManufacturerData(const char* manufacturerData, int manufacturerDataLength);
    void setLocalName(const char *localName);

    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    void addAttribute(BLEAttribute& attribute);

    bool isConnected();

  protected:
    static BLEPeripheral* instance();

    void setLocalData(char pipe, char value[], char length);

  private:
    static BLEPeripheral* _instance;

    const char*    _localName;
    const char*    _advertisedServiceUuid;
    const char*    _manufacturerData;
    int            _manufacturerDataLength;

    const char*    _deviceName;
    unsigned short _appearance;

    bool           _isSetup;
    bool           _isConnected;

    int            _numCustomSetupMessages;
    unsigned short _nextHandle;
    int            _numAttributes;
    BLEAttribute*  _attributes[10];
};

#endif
