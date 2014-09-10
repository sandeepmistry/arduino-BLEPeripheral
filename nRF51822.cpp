#ifdef NRF51
//
#include <ble_stack_handler_types.h>
#include <ble_radio_notification.h>
#include <ble_flash.h>
#include <ble_bondmngr.h>
#include <ble_conn_params.h>

#include <nordic_common.h>
#include <softdevice_handler.h>
#include <pstorage.h>

#include <ble_gatts.h>
#include <ble_hci.h>
//

#include "Arduino.h"

#include "BLEAttribute.h"
#include "BLEService.h"
#include "BLECharacteristic.h"
#include "BLEDescriptor.h"
#include "BLEUuid.h"

#include "nRF51822.h"

#define NRF_51822_DEBUG

#define ADVERTISING_INTERVAL 0x050

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {

}

nRF51822* nRF51822::_instance = NULL;

nRF51822::nRF51822() :
  _connectionHandle(BLE_CONN_HANDLE_INVALID),

  _numServices(0),
  _numCharacteristics(0),
  _numDescriptors(0),

  _serviceInfo(NULL),
  _characteristicInfo(NULL),
  _descriptorInfo(NULL),

  _eventListener(NULL)
{
  _instance = this;
}

nRF51822::~nRF51822() {
}

void nRF51822::eventHandler(ble_evt_t* bleEvent) {
  _instance->handleEvent(bleEvent);
}

void nRF51822::systemEventHandler(uint32_t sysEvent) {

}

void nRF51822::setEventListener(nRF51822EventListener* eventListener) {
  this->_eventListener = eventListener;
}

void nRF51822::begin(unsigned char advertisementDataType,
                      unsigned char advertisementDataLength,
                      const unsigned char* advertisementData,
                      unsigned char scanDataType,
                      unsigned char scanDataLength,
                      const unsigned char* scanData,
                      BLEAttribute** attributes,
                      unsigned char numAttributes)
{
  SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);

  softdevice_ble_evt_handler_set(nRF51822::eventHandler);
  softdevice_sys_evt_handler_set(nRF51822::systemEventHandler);

  ble_gap_conn_params_t gap_conn_params = {0};

  gap_conn_params.min_conn_interval = 40;  // in 1.25ms units
  gap_conn_params.max_conn_interval = 80;  // in 1.25ms unit
  gap_conn_params.slave_latency     = 0;
  gap_conn_params.conn_sup_timeout  = 4000 / 10; // in 10ms unit

  sd_ble_gap_ppcp_set(&gap_conn_params);
  sd_ble_gap_tx_power_set(0);

  /* Connection Parameters */
  enum {
      FIRST_UPDATE_DELAY = APP_TIMER_TICKS(5000, /*CFG_TIMER_PRESCALER*/0),
      NEXT_UPDATE_DELAY  = APP_TIMER_TICKS(5000, /*CFG_TIMER_PRESCALER*/0),
      MAX_UPDATE_COUNT   = 3
  };

  ble_conn_params_init_t cp_init = {0};

  cp_init.p_conn_params                  = NULL;
  cp_init.first_conn_params_update_delay = FIRST_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay  = NEXT_UPDATE_DELAY;
  cp_init.max_conn_params_update_count   = MAX_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail             = true;
  cp_init.evt_handler                    = NULL;
  cp_init.error_handler                  = NULL; //error_callback;

  ble_conn_params_init(&cp_init);

  delay(500);

  /* Wait for the radio to come back up */
  delay(1000);

  unsigned char advData[31];
  unsigned char srData[31];
  unsigned char advDataLen = 0;
  unsigned char srDataLen = 0;

  advData[0] = 2;
  advData[1] = 0x01;
  advData[2] = 0x06;

  advData[3] = advertisementDataLength + 1;
  advData[4] = advertisementDataType;
  memcpy(&advData[5], advertisementData, advertisementDataLength);

  advDataLen = 5 + advertisementDataLength;

  srData[0] = scanDataLength + 1;
  srData[1] = scanDataType;
  memcpy(&srData[2], scanData, scanDataLength);

  srDataLen = 2 + scanDataLength;

  sd_ble_gap_adv_data_set(advData, advDataLen, srData, srDataLen);
  sd_ble_gap_appearance_set(0);

  for (int i = 0; i < numAttributes; i++) {
    BLEAttribute *attribute = attributes[i];

    if (attribute->type() == BLETypeService) {
      this->_numServices++;
    } else if (attribute->type() == BLETypeCharacteristic) {
      this->_numCharacteristics++;
    } else if (attribute->type() == BLETypeDescriptor) {
      this->_numDescriptors++;
    }
  }

  this->_numServices -= 2; // 0x1800, 0x1801
  this->_numCharacteristics -= 3; // 0x2a00, 0x2a01, 0x2a05

  this->_serviceInfo = (struct serviceInfo*)malloc(sizeof(struct serviceInfo) * this->_numServices);
  this->_characteristicInfo = (struct characteristicInfo*)malloc(sizeof(struct characteristicInfo) * this->_numCharacteristics);
  this->_descriptorInfo = (struct descriptorInfo*)malloc(sizeof(struct descriptorInfo) * this->_numDescriptors);

  ble_gap_conn_sec_mode_t secMode;
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&secMode); // no security is needed

  uint16_t serviceHandle            = 0;
  uint16_t characteristicHandle     = 0;
  uint16_t descriptorHandle         = 0;

  unsigned char serviceIndex        = 0;
  unsigned char characteristicIndex = 0;
  unsigned char descriptorIndex     = 0;

  for (int i = 0; i < numAttributes; i++) {
    BLEAttribute *attribute = attributes[i];
    BLEUuid uuid = BLEUuid(attribute->uuid());
    const unsigned char* uuidData = uuid.data();

    ble_uuid_t nordicUUID;

    if (uuid.length() == 2) {
      nordicUUID.uuid = (uuidData[1] << 8) | uuidData[0];
      nordicUUID.type = BLE_UUID_TYPE_BLE;
    } else {
      nordicUUID.uuid = (uuidData[13] << 8) | uuidData[12];
      sd_ble_uuid_vs_add((ble_uuid128_t*)&uuidData, &nordicUUID.type);
    }

    if (attribute->type() == BLETypeService) {
      BLEService *service = (BLEService *)attribute;

      if (strcmp(service->uuid(), "1800") == 0 || strcmp(service->uuid(), "1801") == 0) {
        continue; // skip
      }

      this->_serviceInfo[serviceIndex].service = service;
      this->_serviceInfo[serviceIndex].uuid = nordicUUID;

      sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &this->_serviceInfo[serviceIndex].uuid, &this->_serviceInfo[serviceIndex].handle);

      serviceHandle = this->_serviceInfo[serviceIndex].handle;

      serviceIndex++;
    } else if (attribute->type() == BLETypeCharacteristic) {
      BLECharacteristic *characteristic = (BLECharacteristic *)attribute;

      if (strcmp(characteristic->uuid(), "2a00") == 0) {
        sd_ble_gap_device_name_set(&secMode, characteristic->value(), characteristic->valueLength());
      } else if (strcmp(characteristic->uuid(), "2a01") == 0) {
        const uint16_t *appearance = (const uint16_t*)characteristic->value();

        sd_ble_gap_appearance_set(*appearance);
      } else if (strcmp(characteristic->uuid(), "2a05") == 0) {
        // do nothing
      } else {
        uint8_t properties = characteristic->properties();
        uint16_t valueLength = characteristic->valueLength();

        this->_characteristicInfo[characteristicIndex].characteristic = characteristic;
        this->_characteristicInfo[characteristicIndex].uuid = nordicUUID;

        ble_gatts_char_md_t characteristicMetaData = { 0 };
        ble_gatts_attr_md_t clientCharacteristicConfigurationMetaData;

        if (properties & (BLENotify | BLEIndicate)) {
          /* Notification requires cccd */
          memset(&clientCharacteristicConfigurationMetaData, 0, sizeof(clientCharacteristicConfigurationMetaData));
          clientCharacteristicConfigurationMetaData.vloc = BLE_GATTS_VLOC_STACK;
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&clientCharacteristicConfigurationMetaData.read_perm);
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&clientCharacteristicConfigurationMetaData.write_perm);

          characteristicMetaData.p_cccd_md  = &clientCharacteristicConfigurationMetaData;
        } else {
          characteristicMetaData.p_cccd_md  = NULL;
        }

        memcpy(&characteristicMetaData.char_props, &properties, sizeof(characteristicMetaData.char_props));

        /* Attribute declaration */
        ble_gatts_attr_md_t characteristicValueMetaData = { 0 };

        characteristicValueMetaData.vloc = BLE_GATTS_VLOC_STACK;
        characteristicValueMetaData.vlen = (valueLength == characteristic->valueSize()) ? 0 : 1;

        if (properties & (BLERead | BLENotify | BLEIndicate)) {
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueMetaData.read_perm);
        }

        if (properties & (BLEWrite | BLEWriteWithoutResponse)) {
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueMetaData.write_perm);
        }

        ble_gatts_attr_t characteristicValueAttribute = { 0 };

        characteristicValueAttribute.p_uuid    = &this->_characteristicInfo[characteristicIndex].uuid;
        characteristicValueAttribute.p_attr_md = &characteristicValueMetaData;
        characteristicValueAttribute.init_len  = valueLength;
        characteristicValueAttribute.max_len   = characteristic->valueSize();
        characteristicValueAttribute.p_value   = NULL;

        sd_ble_gatts_characteristic_add(serviceHandle, &characteristicMetaData, &characteristicValueAttribute, &this->_characteristicInfo[characteristicIndex].handles);

        if (valueLength) {
          sd_ble_gatts_value_set(this->_characteristicInfo[characteristicIndex].handles.value_handle, 0, &valueLength, characteristic->value());
        }

        characteristicIndex++;
      }
    } else if (attribute->type() == BLETypeDescriptor) {
    }
  }

  this->startAdvertising();
}

void nRF51822::poll() {
//  sd_app_evt_wait();
}

bool nRF51822::updateCharacteristicValue(BLECharacteristic& characteristic) {

  for (int i = 0; i < this->_numCharacteristics; i++) {
    struct characteristicInfo* characteristicInfo = &this->_characteristicInfo[i];

    if (characteristicInfo->characteristic == &characteristic) {
      uint16_t valueLength = characteristicInfo->characteristic->valueLength();

      sd_ble_gatts_value_set(characteristicInfo->handles.value_handle, 0, &valueLength, characteristicInfo->characteristic->value());
    }
  }
  return true;
}

bool nRF51822::canNotifyCharacteristic(BLECharacteristic& characteristic) {
  return true;
}

bool nRF51822::canIndicateCharacteristic(BLECharacteristic& characteristic) {
  return true;
}

void nRF51822::startAdvertising() {
  ble_gap_adv_params_t advertisingParameters = {0};

  advertisingParameters.type        = 0;
  advertisingParameters.p_peer_addr = NULL;                         // Undirected advertisement
  advertisingParameters.fp          = BLE_GAP_ADV_FP_ANY;
  advertisingParameters.p_whitelist = NULL;
  advertisingParameters.interval    = ADVERTISING_INTERVAL; // advertising interval (in units of 0.625 ms)
  advertisingParameters.timeout     = 0;

  sd_ble_gap_adv_start(&advertisingParameters);
}

void nRF51822::disconnect() {
  sd_ble_gap_disconnect(this->_connectionHandle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}

void nRF51822::requestAddress() {
  ble_gap_addr_t gapAddress;

  sd_ble_gap_address_get(&gapAddress);

  if (this->_eventListener) {
    this->_eventListener->nRF51822AddressReceived(*this, gapAddress.addr);
  }
}

void nRF51822::requestTemperature() {
  int32_t rawTemperature = 0;

  sd_temp_get(&rawTemperature);

  float temperature = rawTemperature / 4.0;

  if (this->_eventListener) {
    this->_eventListener->nRF51822TemperatureReceived(*this, temperature);
  }
}

void nRF51822::requestBatteryLevel() {
}

void nRF51822::handleEvent(ble_evt_t* bleEvent) {
  ble_conn_params_on_ble_evt(bleEvent);

  switch (bleEvent->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      this->_connectionHandle = _connectionHandle;

      if (this->_eventListener) {
        this->_eventListener->nRF51822Connected(*this, bleEvent->evt.gap_evt.params.connected.peer_addr.addr);
      }
      break;

    case BLE_GAP_EVT_DISCONNECTED:
      this->_connectionHandle = BLE_CONN_HANDLE_INVALID;

      if (this->_eventListener) {
        this->_eventListener->nRF51822Disconnected(*this);
      }

      this->startAdvertising();
      break;

    case BLE_GATTS_EVT_WRITE: {
      uint16_t handle = bleEvent->evt.gatts_evt.params.write.handle;

      for (int i = 0; i < this->_numCharacteristics; i++) {
        struct characteristicInfo* characteristicInfo = &this->_characteristicInfo[i];

        if (characteristicInfo->handles.value_handle == handle) {
          if (this->_eventListener) {
            this->_eventListener->nRF51822CharacteristicValueChanged(*this, *characteristicInfo->characteristic, bleEvent->evt.gatts_evt.params.write.data, bleEvent->evt.gatts_evt.params.write.len);
          }
          break;
        } else if (characteristicInfo->handles.cccd_handle == handle) {
          // TODO: notify/indicate
        }
      }
      break;
    }

    default:
      break;
  }
}

#endif
