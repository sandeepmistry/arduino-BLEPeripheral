#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#include "Arduino.h"

#include "BLEAttribute.h"
#include "BLECentral.h"
#include "BLEDescriptor.h"
#include "BLEDevice.h"
#include "BLEService.h"
#include "BLETypedCharacteristics.h"

#ifdef NRF51
  #include "nRF51822.h"
#else
  #include "nRF8001.h"
#endif

enum BLEPeripheralEvent {
  BLEConnected = 0,
  BLEDisconnected = 1
};

typedef void (*BLEPeripheralEventHandler)(BLECentral& central);

class BLEPeripheral : public BLEDeviceEventListener, public BLECharacteristicValueChangeListener
{
  public:
    BLEPeripheral(unsigned char req, unsigned char rdy, unsigned char rst);
    virtual ~BLEPeripheral();

    void begin();
    void poll();

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength);
    void setLocalName(const char *localName);

    void setConnectable(bool connectable);

    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    void addAttribute(BLEAttribute& attribute);

    void disconnect();

    BLECentral central();
    bool connected();

    void setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler);

  protected:
    bool characteristicValueChanged(BLECharacteristic& characteristic);
    bool broadcastCharacteristic(BLECharacteristic& characteristic);
    bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    virtual void BLEDeviceConnected(BLEDevice& device, const unsigned char* address);
    virtual void BLEDeviceDisconnected(BLEDevice& device);

    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& device, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength);
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& device, BLECharacteristic& characteristic, bool subscribed);

    virtual void BLEDeviceAddressReceived(BLEDevice& device, const unsigned char* address);
    virtual void BLEDeviceTemperatureReceived(BLEDevice& device, float temperature);
    virtual void BLEDeviceBatteryLevelReceived(BLEDevice& device, float batteryLevel);

  private:
    BLEDevice*                     _device;

#ifdef NRF51
    nRF51822                       _nRF51822;
#else
    nRF8001                        _nRF8001;
#endif

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
