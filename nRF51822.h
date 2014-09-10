#if !defined(_NRF_51822_H_) && defined(NRF51)
#define _NRF_51822_H_

#include <ble.h>
#include <ble_srv_common.h>

#include "BLEAttribute.h"
#include "BLECharacteristic.h"
#include "BLEDescriptor.h"
#include "BLEService.h"

class nRF51822;

void        assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name);

class nRF51822EventListener
{
  public:
    virtual void nRF51822Connected(nRF51822& nRF51822, const unsigned char* address) = 0;
    virtual void nRF51822Disconnected(nRF51822& nRF51822) = 0;

    virtual void nRF51822CharacteristicValueChanged(nRF51822& nRF51822, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength) = 0;
    virtual void nRF51822CharacteristicSubscribedChanged(nRF51822& nRF51822, BLECharacteristic& characteristic, bool subscribed) = 0;

    virtual void nRF51822AddressReceived(nRF51822& nRF51822, const unsigned char* address) = 0;
    virtual void nRF51822TemperatureReceived(nRF51822& nRF51822, float temperature) = 0;
    virtual void nRF51822BatteryLevelReceived(nRF51822& nRF51822, float batteryLevel) = 0;
};


class nRF51822
{
  friend class BLEPeripheral;

  protected:
    struct serviceInfo {
      BLEService* service;

      ble_uuid_t uuid;
      uint16_t handle;
    };

    struct characteristicInfo {
      BLECharacteristic* characteristic;

      ble_uuid_t uuid;
      ble_gatts_char_handles_t handles;
    };

    struct descriptorInfo {
      BLEDescriptor* descriptor;

      ble_uuid_t uuid;
      uint16_t handle;
    };

    nRF51822();

    virtual ~nRF51822();

    void setEventListener(nRF51822EventListener* eventListener);

    void begin(unsigned char advertisementDataType,
                unsigned char advertisementDataLength,
                const unsigned char* advertisementData,
                unsigned char scanDataType,
                unsigned char scanDataLength,
                const unsigned char* scanData,
                BLEAttribute** attributes,
                unsigned char numAttributes);

    void poll();

    void startAdvertising();
    void disconnect();

    bool updateCharacteristicValue(BLECharacteristic& characteristic);
    bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    void requestAddress();
    void requestTemperature();
    void requestBatteryLevel();

    void handleEvent(ble_evt_t* bleEvent);

    static void eventHandler(ble_evt_t* bleEvent);
    static void systemEventHandler(uint32_t sysEvent);

  private:
    static nRF51822*             _instance;

    uint16_t                     _connectionHandle;

    unsigned char                _numServices;
    unsigned char                _numCharacteristics;
    unsigned char                _numDescriptors;

    struct serviceInfo*          _serviceInfo;
    struct characteristicInfo*   _characteristicInfo;
    struct descriptorInfo*       _descriptorInfo;

    nRF51822EventListener*       _eventListener;
};

#endif
