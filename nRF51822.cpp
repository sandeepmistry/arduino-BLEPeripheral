#if defined(NRF51) || defined(__RFduino__)

#ifdef __RFduino__
  #include <utility/nrf51822/s110/ble_gatts.h>
  #include <utility/nrf51822/s110/ble_hci.h>
  #include <utility/nrf51822/sd_common/ble_stack_handler_types.h>
  #include <utility/nrf51822/nordic_common.h>
  #include <utility/nrf51822/s110/nrf_sdm.h>
  #include <utility/nrf51822/s110/nrf_soc.h>
#else
  #include <s110/ble_gatts.h>
  #include <s110/ble_hci.h>
  #include <sd_common/ble_stack_handler_types.h>
  #include <nordic_common.h>
  #include <s110/nrf_sdm.h>
  #include <s110/nrf_soc.h>
#endif

#include "Arduino.h"

#include "BLEAttribute.h"
#include "BLEService.h"
#include "BLECharacteristic.h"
#include "BLEDescriptor.h"
#include "BLEUtil.h"
#include "BLEUuid.h"

#include "nRF51822.h"

// #define NRF_51822_DEBUG

nRF51822::nRF51822() :
  BLEDevice(),

  _connectionHandle(BLE_CONN_HANDLE_INVALID),
  _storeAuthStatus(false),

  _advDataLen(0),
  _broadcastCharacteristic(NULL),

  _numCharacteristics(0),
  _characteristicInfo(NULL)
{
  this->_authStatus = (ble_gap_evt_auth_status_t*)&this->_authStatusBuffer;
  memset(&this->_authStatusBuffer, 0, sizeof(this->_authStatusBuffer));
}

nRF51822::~nRF51822() {
  if (this->_characteristicInfo) {
    free(this->_characteristicInfo);
  }
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

#ifdef __RFduino__
  sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, NULL);
#else
  sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL); // sd_nvic_EnableIRQ(SWI2_IRQn);
#endif

#ifdef NRF_51822_DEBUG
  ble_version_t version;

  sd_ble_version_get(&version);

  Serial.print(F("version = "));
  Serial.print(version.version_number);
  Serial.print(F(" "));
  Serial.print(version.company_id);
  Serial.print(F(" "));
  Serial.print(version.subversion_number);
  Serial.println();
#endif

  ble_gap_conn_params_t gap_conn_params = {0};

  gap_conn_params.min_conn_interval = 40;  // in 1.25ms units
  gap_conn_params.max_conn_interval = 80;  // in 1.25ms unit
  gap_conn_params.slave_latency     = 0;
  gap_conn_params.conn_sup_timeout  = 4000 / 10; // in 10ms unit

  sd_ble_gap_ppcp_set(&gap_conn_params);
  sd_ble_gap_tx_power_set(0);

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
        ble_gap_conn_sec_mode_t secMode;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&secMode); // no security is needed

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
          if (this->_bondStore) {
            BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&characteristicValueAttributeMetaData.read_perm);
          } else {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueAttributeMetaData.read_perm);
          }
        }

        if (properties & (BLEWriteWithoutResponse | BLEWrite)) {
          if (this->_bondStore) {
            BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&characteristicValueAttributeMetaData.write_perm);
          } else {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&characteristicValueAttributeMetaData.write_perm);
          }
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
            characteristicMetaData.char_user_desc_max_size = descriptor->valueLength();
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
      descriptorMetaData.vlen = (valueLength == descriptor->valueLength()) ? 0 : 1;

      if (this->_bondStore) {
        BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&descriptorMetaData.read_perm);
      } else {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&descriptorMetaData.read_perm);
      }

      descriptorAttribute.p_uuid    = &nordicUUID;
      descriptorAttribute.p_attr_md = &descriptorMetaData;
      descriptorAttribute.init_len  = valueLength;
      descriptorAttribute.max_len   = descriptor->valueLength();
      descriptorAttribute.p_value   = NULL;

      sd_ble_gatts_descriptor_add(BLE_GATT_HANDLE_INVALID, &descriptorAttribute, &handle);

      if (valueLength) {
        sd_ble_gatts_value_set(handle, 0, &valueLength, descriptor->value());
      }
    }
  }

  if (this->_bondStore && this->_bondStore->hasData()) {
#ifdef NRF_51822_DEBUG
    Serial.println(F("Restoring bond data"));
#endif
    this->_bondStore->restoreData(this->_authStatusBuffer, sizeof(this->_authStatusBuffer));
  }

  this->startAdvertising();
}

void nRF51822::poll() {
  uint32_t   evtBuf[BLE_STACK_EVT_MSG_BUF_SIZE] __attribute__ ((__aligned__(BLE_EVTS_PTR_ALIGNMENT)));
  uint16_t   evtLen = sizeof(evtBuf);
  ble_evt_t* bleEvt = (ble_evt_t*)evtBuf;

  if (sd_ble_evt_get((uint8_t*)evtBuf, &evtLen) == NRF_SUCCESS) {
    switch (bleEvt->header.evt_id) {
      case BLE_EVT_TX_COMPLETE:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt TX complete "));
        Serial.println(bleEvt->evt.common_evt.params.tx_complete.count);
#endif
        break;

      case BLE_GAP_EVT_CONNECTED:
#ifdef NRF_51822_DEBUG
        char address[18];

        BLEUtil::addressToString(bleEvt->evt.gap_evt.params.connected.peer_addr.addr, address);

        Serial.print(F("Evt Connected "));
        Serial.println(address);
#endif

        this->_connectionHandle = bleEvt->evt.gap_evt.conn_handle;
        this->_storeAuthStatus = false;

        if (this->_eventListener) {
          this->_eventListener->BLEDeviceConnected(*this, bleEvt->evt.gap_evt.params.connected.peer_addr.addr);
        }
        break;

      case BLE_GAP_EVT_DISCONNECTED:
#ifdef NRF_51822_DEBUG
        Serial.println(F("Evt Disconnected"));
#endif
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

        if (this->_bondStore && this->_storeAuthStatus) {
#ifdef NRF_51822_DEBUG
          Serial.println(F("Storing bond data"));
#endif
          this->_bondStore->storeData(this->_authStatusBuffer, sizeof(this->_authStatusBuffer));

          this->_storeAuthStatus = false;
        }

        this->startAdvertising();
        break;

      case BLE_GAP_EVT_CONN_PARAM_UPDATE:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Conn Param Update 0x"));
        Serial.print(bleEvt->evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval, HEX);
        Serial.print(F(" 0x"));
        Serial.print(bleEvt->evt.gap_evt.params.conn_param_update.conn_params.max_conn_interval, HEX);
        Serial.print(F(" 0x"));
        Serial.print(bleEvt->evt.gap_evt.params.conn_param_update.conn_params.slave_latency, HEX);
        Serial.print(F(" 0x"));
        Serial.print(bleEvt->evt.gap_evt.params.conn_param_update.conn_params.conn_sup_timeout, HEX);
        Serial.println();
#endif
        break;

      case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Sec Params Request "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.timeout);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.bond);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.mitm);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.io_caps);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.oob);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.min_key_size);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.max_key_size);
        Serial.println();
#endif

        if (this->_bondStore && !this->_bondStore->hasData()) {
          // only allow bonding if bond store exists and there is no data

          ble_gap_sec_params_t gapSecParams;

          gapSecParams.timeout      = 30; // must be 30s
          gapSecParams.bond         = true;
          gapSecParams.mitm         = false;
          gapSecParams.io_caps      = BLE_GAP_IO_CAPS_NONE;
          gapSecParams.oob          = false;
          gapSecParams.min_key_size = 7;
          gapSecParams.max_key_size = 16;

          sd_ble_gap_sec_params_reply(this->_connectionHandle, BLE_GAP_SEC_STATUS_SUCCESS, &gapSecParams);
        } else {
          sd_ble_gap_sec_params_reply(this->_connectionHandle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL);
        }
        break;

      case BLE_GAP_EVT_SEC_INFO_REQUEST:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Sec Info Request "));
        // Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.peer_addr);
        // Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.div);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.enc_info);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.id_info);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.sign_info);
        Serial.println();
#endif
        if (this->_authStatus->periph_keys.enc_info.div == bleEvt->evt.gap_evt.params.sec_info_request.div) {
          sd_ble_gap_sec_info_reply(this->_connectionHandle, &this->_authStatus->periph_keys.enc_info, NULL);
        } else {
          sd_ble_gap_sec_info_reply(this->_connectionHandle, NULL, NULL);
        }
        break;

      case BLE_GAP_EVT_AUTH_STATUS:
#ifdef NRF_51822_DEBUG
        Serial.println(F("Evt Auth Status"));
#endif
        this->_storeAuthStatus = true;
        *this->_authStatus = bleEvt->evt.gap_evt.params.auth_status;
        break;

      case BLE_GAP_EVT_CONN_SEC_UPDATE:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Conn Sec Update "));
        Serial.print(bleEvt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.sm);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.conn_sec_update.conn_sec.encr_key_size);
        Serial.println();
#endif
        break;

      case BLE_GATTS_EVT_WRITE: {
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Write, handle = "));
        Serial.println(bleEvt->evt.gatts_evt.params.write.handle, DEC);

        for (int i = 0; i < bleEvt->evt.gatts_evt.params.write.len; i++) {
          if ((bleEvt->evt.gatts_evt.params.write.data[i] & 0xf0) == 00) {
            Serial.print(F("0"));
          }

          Serial.print(bleEvt->evt.gatts_evt.params.write.data[i], HEX);
          Serial.print(F(" "));
        }
        Serial.println();
#endif

        uint16_t handle = bleEvt->evt.gatts_evt.params.write.handle;

        for (int i = 0; i < this->_numCharacteristics; i++) {
          struct characteristicInfo* characteristicInfo = &this->_characteristicInfo[i];

          if (characteristicInfo->handles.value_handle == handle) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceCharacteristicValueChanged(*this, *characteristicInfo->characteristic, bleEvt->evt.gatts_evt.params.write.data, bleEvt->evt.gatts_evt.params.write.len);
            }
            break;
          } else if (characteristicInfo->handles.cccd_handle == handle) {
            uint16_t value = bleEvt->evt.gatts_evt.params.write.data[0] | (bleEvt->evt.gatts_evt.params.write.data[1] << 8);

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

      case BLE_GATTS_EVT_SYS_ATTR_MISSING:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Sys Attr Missing "));
        Serial.println(bleEvt->evt.gatts_evt.params.sys_attr_missing.hint);
#endif

        sd_ble_gatts_sys_attr_set(this->_connectionHandle, NULL, 0);
        break;

      default:
#ifdef NRF_51822_DEBUG
        Serial.print(F("bleEvt->header.evt_id = 0x"));
        Serial.print(bleEvt->header.evt_id, HEX);
        Serial.print(F(" "));
        Serial.println(bleEvt->header.evt_len);
#endif
        break;
    }
  }

  // sd_app_evt_wait();
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
#ifdef NRF_51822_DEBUG
  Serial.println(F("Start advertisement"));
#endif

  ble_gap_adv_params_t advertisingParameters = {0};

  advertisingParameters.type        = this->_connectable ? BLE_GAP_ADV_TYPE_ADV_IND : BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
  advertisingParameters.p_peer_addr = NULL;
  advertisingParameters.fp          = BLE_GAP_ADV_FP_ANY;
  advertisingParameters.p_whitelist = NULL;
  advertisingParameters.interval    = (this->_advertisingInterval * 16) / 10; // advertising interval (in units of 0.625 ms)
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
#ifndef __RFduino__
  int32_t rawTemperature = 0;

  sd_temp_get(&rawTemperature);

  float temperature = rawTemperature / 4.0;

  if (this->_eventListener) {
    this->_eventListener->BLEDeviceTemperatureReceived(*this, temperature);
  }
#endif
}

void nRF51822::requestBatteryLevel() {
}

#endif
