#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#include "Arduino.h"

#include "nRF8001.h"

#include "BLEAttribute.h"
#include "BLECentral.h"
#include "BLEDescriptor.h"
#include "BLEService.h"
#include "BLETypedCharacteristics.h"

enum BLEPeripheralEvent {
  BLEConnected = 0,
  BLEDisconnected = 1
};

typedef void (*BLEPeripheralEventHandler)(BLECentral& central);

class BLEPeripheral : public nRF8001EventListener, public BLECharacteristicValueChangeListener
{
  public:
    BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst);
    virtual ~BLEPeripheral();

    void begin();
    void poll();

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength);
    void setLocalName(const char *localName);

    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    void addAttribute(BLEAttribute& attribute);

    void disconnect();

    BLECentral central();
    bool connected();

    void setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler);

  protected:
    bool characteristicValueChanged(BLECharacteristic& characteristic);
    bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    virtual void nRF8001Connected(nRF8001& nRF8001, const unsigned char* address);
    virtual void nRF8001Disconnected(nRF8001& nRF8001);

    virtual void nRF8001CharacteristicValueChanged(nRF8001& nRF8001, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength);
    virtual void nRF8001CharacteristicSubscribedChanged(nRF8001& nRF8001, BLECharacteristic& characteristic, bool subscribed);

    virtual void nRF8001AddressReceived(nRF8001& nRF8001, const unsigned char* address);
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
    BLEService                     _genericAttributeService;
    BLECharacteristic              _servicesChangedCharacteristic;

    BLECentral                     _central;
    BLEPeripheralEventHandler      _eventHandlers[2];
};

#endif
