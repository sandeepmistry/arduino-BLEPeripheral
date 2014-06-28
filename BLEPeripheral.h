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

    bool isPipeOpen(char pipe);
    void setLocalData(char pipe, char value[], char length);
    bool sendData(char pipe, char value[], char length);
    bool sendAck(char pipe);
    bool sendNack(char pipe, char errorCode);

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
    uint64_t       _openPipes;

    int            _numCustomSetupMessages;
    unsigned short _nextHandle;
    int            _numAttributes;
    BLEAttribute*  _attributes[10];
};

#endif
