#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#include "Arduino.h"

#include "nRF8001.h"

#include "BLEAttribute.h"
#include "BLEService.h"
#include "BLECharacteristic.h"
#include "BLECharacteristicT.h"
#include "BLEDescriptor.h"

typedef void (*BLEPeripheralConnectHandler)(const char* address);
typedef void (*BLEPeripheralDisconnectHandler)();

class BLEPeripheral : public nRF8001EventListener
{
  public:
    BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst);
    virtual ~BLEPeripheral();
  
    void begin();
    void poll();

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setManufacturerData(const unsigned char* manufacturerData, unsigned char manufacturerDataLength);
    void setLocalName(const char *localName);

    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    void addAttribute(BLEAttribute& attribute);

    void disconnect();

    bool isConnected();
    void setConnectHandler(BLEPeripheralConnectHandler connectHandler);
    void setDisconnectHandler(BLEPeripheralDisconnectHandler disconnectHandler);

    virtual void nRF8001Connected(nRF8001& nRF8001, const char* address);
    virtual void nRF8001Disconnected(nRF8001& nRF8001);

    virtual void nRF8001AddressReceived(nRF8001& nRF8001, const char* address);
    virtual void nRF8001TemperatureReceived(nRF8001& nRF8001, float temperature);
    virtual void nRF8001BatteryLevelReceived(nRF8001& nRF8001, float batteryLevel);

  private:
    nRF8001                        _nRF8001;

    const char*                    _advertisedServiceUuid;
    const unsigned char*           _manufacturerData;
    unsigned char                  _manufacturerDataLength;
    const char*                    _localName;

    BLEAttribute**                 _attributes;
    unsigned char                  _numAttributes;

    BLEService                     _genericAccessService;
    BLECharacteristic              _deviceNameCharacteristic;
    BLECharacteristic              _appearanceCharacteristic;

    bool                           _isConnected;
    BLEPeripheralConnectHandler    _connectHandler;
    BLEPeripheralDisconnectHandler _disconnectHandler;
};

#endif
