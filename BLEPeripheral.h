#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#include "Arduino.h"

#include "BLEBondStore.h"
#include "BLECentral.h"
#include "BLEConstantCharacteristic.h"
#include "BLEDescriptor.h"
#include "BLEDevice.h"
#include "BLEFixedLengthCharacteristic.h"
#include "BLELocalAttribute.h"
#include "BLEProgmemConstantCharacteristic.h"
#include "BLERemoteAttribute.h"
#include "BLERemoteCharacteristic.h"
#include "BLERemoteService.h"
#include "BLEService.h"
#include "BLETypedCharacteristics.h"

#if defined(NRF51) || defined(__RFduino__)
  #include "nRF51822.h"
#else
  #include "nRF8001.h"
#endif

enum BLEPeripheralEvent {
  BLEConnected = 0,
  BLEDisconnected = 1,
  BLEBonded = 2,
  BLERemoteServicesDiscovered = 3
};

typedef void (*BLEPeripheralEventHandler)(BLECentral& central);


class BLEPeripheral : public BLEDeviceEventListener,
                        public BLECharacteristicValueChangeListener,
                        public BLERemoteCharacteristicValueChangeListener
{
  public:
    BLEPeripheral(BLE_Tx_Power_Level tx_power_level, unsigned char req, unsigned char rdy, unsigned char rst);
    virtual ~BLEPeripheral();

    void begin();
    void poll();

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setServiceSolicitationUuid(const char* serviceSolicitationUuid);
    void setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength);
    void setLocalName(const char *localName);

    void setAdvertisingInterval(unsigned short advertisingInterval);
    void setConnectable(bool connectable);
    void setBondStore(BLEBondStore& bondStore);


    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    void addAttribute(BLELocalAttribute& attribute);
    void addLocalAttribute(BLELocalAttribute& localAttribute);
    void addRemoteAttribute(BLERemoteAttribute& remoteAttribute);

    void disconnect();

    BLECentral central();
    bool connected();

    void setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler);

  protected:
    bool characteristicValueChanged(BLECharacteristic& characteristic);
    bool broadcastCharacteristic(BLECharacteristic& characteristic);
    bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool readRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length);
    bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);

    virtual void BLEDeviceConnected(BLEDevice& device, const unsigned char* address);
    virtual void BLEDeviceDisconnected(BLEDevice& device);
    virtual void BLEDeviceBonded(BLEDevice& device);
    virtual void BLEDeviceRemoteServicesDiscovered(BLEDevice& device);

    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& device, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength);
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& device, BLECharacteristic& characteristic, bool subscribed);

    virtual void BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& device, BLERemoteCharacteristic& remoteCharacteristic, const unsigned char* value, unsigned char valueLength);

    virtual void BLEDeviceAddressReceived(BLEDevice& device, const unsigned char* address);
    virtual void BLEDeviceTemperatureReceived(BLEDevice& device, float temperature);
    virtual void BLEDeviceBatteryLevelReceived(BLEDevice& device, float batteryLevel);

  private:
    void initLocalAttributes();

  private:
    BLEDevice*                     _device;

#if defined(NRF51) || defined(__RFduino__)
    nRF51822                       _nRF51822;
#else
    nRF8001                        _nRF8001;
#endif

    const char*                    _advertisedServiceUuid;
    const char*                    _serviceSolicitationUuid;
    const unsigned char*           _manufacturerData;
    unsigned char                  _manufacturerDataLength;
    const char*                    _localName;

    BLELocalAttribute**            _localAttributes;
    unsigned char                  _numLocalAttributes;
    BLERemoteAttribute**           _remoteAttributes;
    unsigned char                  _numRemoteAttributes;

    BLEService                     _genericAccessService;
    BLECharacteristic              _deviceNameCharacteristic;
    BLECharacteristic              _appearanceCharacteristic;
    BLEService                     _genericAttributeService;
    BLECharacteristic              _servicesChangedCharacteristic;

    BLERemoteService               _remoteGenericAttributeService;
    BLERemoteCharacteristic        _remoteServicesChangedCharacteristic;

    BLECentral                     _central;
    BLEPeripheralEventHandler      _eventHandlers[4];
};

#endif
