#ifdef NRF51

#include <ble_conn_params.h>
#include <ble_gatts.h>
#include <ble_hci.h>
#include <nordic_common.h>
#include <softdevice_handler.h>

#include "Arduino.h"

#include "BLEAttribute.h"
#include "BLEService.h"
#include "BLECharacteristic.h"
#include "BLEDescriptor.h"
#include "BLEUuid.h"

#include "nRF51822.h"

#define NRF_51822_DEBUG

void assert_nrf_callback(uint16_t line_num, const uint8_t *file_name) {
}

nRF51822* nRF51822::_instance = NULL;

nRF51822::nRF51822() :
  BLEDevice(),

  _connectionHandle(BLE_CONN_HANDLE_INVALID),

  _advDataLen(0),
  _broadcastCharacteristic(NULL),

  _numCharacteristics(0),
  _characteristicInfo(NULL)
{
  _instance = this;
}

nRF51822::~nRF51822() {
  if (this->_characteristicInfo) {
    free(this->_characteristicInfo);
  }
}

void nRF51822::eventHandler(ble_evt_t* bleEvent) {
  _instance->handleEvent(bleEvent);
}

void nRF51822::systemEventHandler(uint32_t sysEvent) {

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

  unsigned char srData[31];
  unsigned char srDataLen = 0;

  this->_advDataLen = 0;

  if (this->_connectable) {
    this->_advData[this->_advDataLen + 0] = 2;
    this->_advData[this->_advDataLen + 1] = 0x01;
    this->_advData[this->_advDataLen + 2] = 0x06;

    this->_advDataLen += 3;
  }

  if (advertisementDataType && advertisementDataLength && advertisementData) {
    this->_advData[this->_advDataLen + 0] = advertisementDataLength + 1;
    this->_advData[this->_advDataLen + 1] = advertisementDataType;
    this->_advDataLen += 2;

    memcpy(&this->_advData[this->_advDataLen], advertisementData, advertisementDataLength);

    this->_advDataLen += advertisementDataLength;
  }

  if (scanDataType && scanDataLength && srData) {
    srData[0] = scanDataLength + 1;
    srData[1] = scanDataType;
    memcpy(&srData[2], scanData, scanDataLength);

    srDataLen = 2 + scanDataLength;
  }

  sd_ble_gap_adv_data_set(this->_advData, this->_advDataLen, srData, srDataLen);
  sd_ble_gap_appearance_set(0);

  for (int i = 0; i < numAttributes; i++) {
    BLEAttribute *attribute = attributes[i];

    if (attribute->type() == BLETypeCharacteristic) {
      this->_numCharacteristics++;
    }
  }

  this->_numCharacteristics -= 3; // 0x2a00, 0x2a01, 0x2a05

  this->_characteristicInfo = (struct characteristicInfo*)malloc(sizeof(struct characteristicInfo) * this->_numCharacteristics);

  ble_gap_conn_sec_mode_t secMode;
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&secMode); // no security is needed

  unsigned char characteristicIndex = 0;

  uint16_t handle = 0;
  BLEService *lastService = NULL;

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

      sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &nordicUUID, &handle);

      lastService = service;
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
        uint8_t properties = characteristic->properties() & 0xfe;
        uint16_t valueLength = characteristic->valueLength();

        this->_characteristicInfo[characteristicIndex].characteristic = characteristic;
        this->_characteristicInfo[characteristicIndex].notifySubscribed = false;
        this->_characteristicInfo[characteristicIndex].indicateSubscribed = false;
        this->_characteristicInfo[characteristicIndex].service = lastService;

        ble_gatts_char_md_t characteristicMetaData;
        ble_gatts_attr_md_t clientCharacteristicConfigurationMetaData;
        ble_gatts_attr_t    characteristicValueAttribute;
        ble_gatts_attr_md_t characteristicValueAttributeMetaData;

        memset(&characteristicMetaData, 0, sizeof(characteristicMetaData));

        memcpy(&characteristicMetaData.char_props, &properties, 1);

        characteristicMetaData.p_char_user_desc  = NULL;
        characteristicMetaData.p_char_pf         = NULL;
        characteristicMetaData.p_user_desc_md    = NULL;
        characteristicMetaData.p_cccd_md         = NULL;
        characteristicMetaData.p_sccd_md         = NULL;

        if (properties & (BLENotify | BLEIndicate)) {
          memset(&clientCharacteristicConfigurationMetaData, 0, sizeof(clientCharacteristicConfigurationMetaData));

          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&clientCharacteristicConfigurationMetaData.read_perm);
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&clientCharacteristicConfigurationMetaData.write_perm);

          clientCharacteristicConfigurationMetaData.vloc = BLE_GATTS_VLOC_STACK;

          characteristicMetaData.p_cccd_md = &clientCharacteristicConfigurationMetaData;
        }

        memset(&characteristicValueAttributeMetaData, 0, sizeof(characteristicValueAttributeMetaData));

        if (properties & (BLERead | BLENotify | BLEIndicate)) {
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueAttributeMetaData.read_perm);
        }

        if (properties & (BLEWriteWithoutResponse | BLEWrite)) {
          BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueAttributeMetaData.write_perm);
        }

        characteristicValueAttributeMetaData.vloc       = BLE_GATTS_VLOC_STACK;
        characteristicValueAttributeMetaData.rd_auth    = 0;
        characteristicValueAttributeMetaData.wr_auth    = 0;
        characteristicValueAttributeMetaData.vlen       = 0;

        for (int j = (i + 1); j < numAttributes; j++) {
          attribute = attributes[j];

          if (attribute->type() != BLETypeDescriptor) {
            break;
          }

          BLEDescriptor *descriptor = (BLEDescriptor *)attribute;

          if (strcmp(descriptor->uuid(), "2901") == 0) {
            characteristicMetaData.p_char_user_desc        = (uint8_t*)descriptor->value();
            characteristicMetaData.char_user_desc_max_size = descriptor->valueSize();
            characteristicMetaData.char_user_desc_size     = descriptor->valueLength();
          } else if (strcmp(descriptor->uuid(), "2904") == 0) {
            characteristicMetaData.p_char_pf = (ble_gatts_char_pf_t *)descriptor->value();
          }
        }

        memset(&characteristicValueAttribute, 0, sizeof(characteristicValueAttribute));

        characteristicValueAttribute.p_uuid       = &nordicUUID;
        characteristicValueAttribute.p_attr_md    = &characteristicValueAttributeMetaData;
        characteristicValueAttribute.init_len     = valueLength;
        characteristicValueAttribute.init_offs    = 0;
        characteristicValueAttribute.max_len      = characteristic->valueSize();
        characteristicValueAttribute.p_value      = NULL;

        sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID, &characteristicMetaData, &characteristicValueAttribute, &this->_characteristicInfo[characteristicIndex].handles);

        if (valueLength) {
          sd_ble_gatts_value_set(this->_characteristicInfo[characteristicIndex].handles.value_handle, 0, &valueLength, characteristic->value());
        }

        characteristicIndex++;
      }
    } else if (attribute->type() == BLETypeDescriptor) {
      BLEDescriptor *descriptor = (BLEDescriptor *)attribute;

      if (strcmp(descriptor->uuid(), "2901") == 0 ||
          strcmp(descriptor->uuid(), "2902") == 0 ||
          strcmp(descriptor->uuid(), "2903") == 0 ||
          strcmp(descriptor->uuid(), "2904") == 0) {
        continue; // skip
      }

      uint16_t valueLength = descriptor->valueLength();

      ble_gatts_attr_t descriptorAttribute;
      ble_gatts_attr_md_t descriptorMetaData;

      memset(&descriptorAttribute, 0, sizeof(descriptorAttribute));
      memset(&descriptorMetaData, 0, sizeof(descriptorMetaData));

      descriptorMetaData.vloc = BLE_GATTS_VLOC_STACK;
      descriptorMetaData.vlen = (valueLength == descriptor->valueSize()) ? 0 : 1;

      BLE_GAP_CONN_SEC_MODE_SET_OPEN(&descriptorMetaData.read_perm);

      descriptorAttribute.p_uuid    = &nordicUUID;
      descriptorAttribute.p_attr_md = &descriptorMetaData;
      descriptorAttribute.init_len  = valueLength;
      descriptorAttribute.max_len   = descriptor->valueSize();
      descriptorAttribute.p_value   = NULL;

      sd_ble_gatts_descriptor_add(BLE_GATT_HANDLE_INVALID, &descriptorAttribute, &handle);

      if (valueLength) {
        sd_ble_gatts_value_set(handle, 0, &valueLength, descriptor->value());
      }
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
      if (&characteristic == this->_broadcastCharacteristic) {
        this->broadcastCharacteristic(characteristic);
      }

      uint16_t valueLength = characteristic.valueLength();

      sd_ble_gatts_value_set(characteristicInfo->handles.value_handle, 0, &valueLength, characteristic.value());

      ble_gatts_hvx_params_t hvxParams;

      memset(&hvxParams, 0, sizeof(hvxParams));

      hvxParams.handle = characteristicInfo->handles.value_handle;
      hvxParams.offset = 0;
      hvxParams.p_data = NULL;
      hvxParams.p_len  = &valueLength;

      if (characteristicInfo->notifySubscribed) {
        hvxParams.type = BLE_GATT_HVX_NOTIFICATION;

        sd_ble_gatts_hvx(this->_connectionHandle, &hvxParams);
      }

      if (characteristicInfo->indicateSubscribed) {
        hvxParams.type = BLE_GATT_HVX_INDICATION;

        sd_ble_gatts_hvx(this->_connectionHandle, &hvxParams);
      }
    }
  }

  return true;
}

bool nRF51822::broadcastCharacteristic(BLECharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numCharacteristics; i++) {
    struct characteristicInfo* characteristicInfo = &this->_characteristicInfo[i];

    if (characteristicInfo->characteristic == &characteristic) {
      if (characteristic.properties() & BLEBroadcast && characteristicInfo->service) {
        unsigned char advData[31];
        unsigned char advDataLen = this->_advDataLen;

        // copy the existing advertisement data
        memcpy(advData, this->_advData, advDataLen);

        advDataLen += (4 + characteristic.valueLength());

        if (advDataLen <= 31) {
          BLEUuid uuid = BLEUuid(characteristicInfo->service->uuid());

          advData[this->_advDataLen + 0] = 3 + characteristic.valueLength();
          advData[this->_advDataLen + 1] = 0x16;

          memcpy(&advData[this->_advDataLen + 2], uuid.data(), 2);
          memcpy(&advData[this->_advDataLen + 4], characteristic.value(), characteristic.valueLength());

          sd_ble_gap_adv_data_set(advData, advDataLen, NULL, 0); // update advertisement data
          success = true;

          this->_broadcastCharacteristic = &characteristic;
        }
      }
    }
    break;
  }

  return success;
}

bool nRF51822::canNotifyCharacteristic(BLECharacteristic& characteristic) {
  uint8_t count = 0;

  sd_ble_tx_buffer_count_get(&count);

  return (count > 0);
}

bool nRF51822::canIndicateCharacteristic(BLECharacteristic& characteristic) {
  uint8_t count = 0;

  sd_ble_tx_buffer_count_get(&count);

  return (count > 0);
}

void nRF51822::startAdvertising() {
  ble_gap_adv_params_t advertisingParameters = {0};

  advertisingParameters.type        = this->_connectable ? BLE_GAP_ADV_TYPE_ADV_IND : BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
  advertisingParameters.p_peer_addr = NULL;
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
    this->_eventListener->BLEDeviceAddressReceived(*this, gapAddress.addr);
  }
}

void nRF51822::requestTemperature() {
  int32_t rawTemperature = 0;

  sd_temp_get(&rawTemperature);

  float temperature = rawTemperature / 4.0;

  if (this->_eventListener) {
    this->_eventListener->BLEDeviceTemperatureReceived(*this, temperature);
  }
}

void nRF51822::requestBatteryLevel() {
}

void nRF51822::handleEvent(ble_evt_t* bleEvent) {
  ble_conn_params_on_ble_evt(bleEvent);

  switch (bleEvent->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      this->_connectionHandle = bleEvent->evt.gap_evt.conn_handle;

      if (this->_eventListener) {
        this->_eventListener->BLEDeviceConnected(*this, bleEvent->evt.gap_evt.params.connected.peer_addr.addr);
      }
      break;

    case BLE_GAP_EVT_DISCONNECTED:
      this->_connectionHandle = BLE_CONN_HANDLE_INVALID;

      for (int i = 0; i < this->_numCharacteristics; i++) {
        struct characteristicInfo* characteristicInfo = &this->_characteristicInfo[i];

        characteristicInfo->notifySubscribed = false;
        characteristicInfo->indicateSubscribed = false;

        if (characteristicInfo->characteristic->subscribed()) {
          if (this->_eventListener) {
            this->_eventListener->BLEDeviceCharacteristicSubscribedChanged(*this, *characteristicInfo->characteristic, false);
          }
        }
      }

      if (this->_eventListener) {
        this->_eventListener->BLEDeviceDisconnected(*this);
      }

      this->startAdvertising();
      break;

    case BLE_GATTS_EVT_WRITE: {
      uint16_t handle = bleEvent->evt.gatts_evt.params.write.handle;

      for (int i = 0; i < this->_numCharacteristics; i++) {
        struct characteristicInfo* characteristicInfo = &this->_characteristicInfo[i];

        if (characteristicInfo->handles.value_handle == handle) {
          if (this->_eventListener) {
            this->_eventListener->BLEDeviceCharacteristicValueChanged(*this, *characteristicInfo->characteristic, bleEvent->evt.gatts_evt.params.write.data, bleEvent->evt.gatts_evt.params.write.len);
          }
          break;
        } else if (characteristicInfo->handles.cccd_handle == handle) {
          uint16_t value = bleEvent->evt.gatts_evt.params.write.data[0] | (bleEvent->evt.gatts_evt.params.write.data[1] << 8);

          characteristicInfo->notifySubscribed = (value & 0x0001);
          characteristicInfo->indicateSubscribed = (value & 0x0002);

          bool subscribed = (characteristicInfo->notifySubscribed || characteristicInfo->indicateSubscribed);

          if (subscribed != characteristicInfo->characteristic->subscribed()) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceCharacteristicSubscribedChanged(*this, *characteristicInfo->characteristic, subscribed);
            }
          }
        }
      }
      break;
    }

    default:
      break;
  }
}

#endif
