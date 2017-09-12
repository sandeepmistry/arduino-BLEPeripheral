// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)

#ifdef __RFduino__
  #include <utility/RFduino/ble.h>
  #include <utility/RFduino/ble_hci.h>
  #include <utility/RFduino/nrf_sdm.h>
#elif defined(NRF5) || defined(NRF51_S130)
  #include <ble.h>
  #include <ble_hci.h>
  #include <nrf_sdm.h>
#elif defined(NRF52) && defined(S132) // ARDUINO_RBL_nRF52832
  #ifndef ARDUINO_RBL_nRF52832
    #define ARDUINO_RBL_nRF52832
  #endif

  #include <sdk/softdevice/s132/headers/nrf_ble.h>
  #include <sdk/softdevice/s132/headers/nrf_ble_hci.h>
  #include <sdk/softdevice/s132/headers/nrf_sdm.h>
#else
  #include <s110/ble.h>
  #include <s110/ble_hci.h>
  #include <s110/nrf_sdm.h>
#endif

#if defined(NRF5) || defined(NRF51_S130) || defined(ARDUINO_RBL_nRF52832)
uint32_t sd_ble_gatts_value_set(uint16_t handle, uint16_t offset, uint16_t* const p_len, uint8_t const * const p_value) {
  ble_gatts_value_t val;

  val.len = *p_len;
  val.offset = offset;
  val.p_value = (uint8_t*)p_value;
  return sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, handle, &val);
}
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

#define BLE_STACK_EVT_MSG_BUF_SIZE       (sizeof(ble_evt_t) + (GATT_MTU_SIZE_DEFAULT))

#ifndef BLE_GATTS_ATTR_TAB_SIZE
  #define BLE_GATTS_ATTR_TAB_SIZE BLE_GATTS_ATTR_TAB_SIZE_DEFAULT
#endif

nRF51822::nRF51822() :
  BLEDevice(),

  _advDataLen(0),
  _hasScanData(false),
  _broadcastCharacteristic(NULL),

  _connectionHandle(BLE_CONN_HANDLE_INVALID),

  _txBufferCount(0),

  _numLocalCharacteristics(0),
  _localCharacteristicInfo(NULL),

  _numRemoteServices(0),
  _remoteServiceInfo(NULL),
  _remoteServiceDiscoveryIndex(0),
  _numRemoteCharacteristics(0),
  _remoteCharacteristicInfo(NULL),
  _remoteRequestInProgress(false)
{
#if defined(NRF5) || defined(NRF51_S130)
  this->_encKey = (ble_gap_enc_key_t*)&this->_bondData;
  memset(&this->_bondData, 0, sizeof(this->_bondData));
#else
  this->_authStatus = (ble_gap_evt_auth_status_t*)&this->_authStatusBuffer;
  memset(&this->_authStatusBuffer, 0, sizeof(this->_authStatusBuffer));
#endif
}

nRF51822::~nRF51822() {
  this->end();
}

void nRF51822::begin(unsigned char advertisementDataSize,
                      BLEEirData *advertisementData,
                      unsigned char scanDataSize,
                      BLEEirData *scanData,
                      BLELocalAttribute** localAttributes,
                      unsigned char numLocalAttributes,
                      BLERemoteAttribute** remoteAttributes,
                      unsigned char numRemoteAttributes)
{

#ifdef __RFduino__
  sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, NULL);
#elif defined(NRF5) && !defined(S110)
  #if defined(USE_LFRC)
    nrf_clock_lf_cfg_t cfg = {
      .source        = NRF_CLOCK_LF_SRC_RC,
      .rc_ctiv       = 8, //16
      .rc_temp_ctiv  = 2,
      .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM
    };
  #elif defined(USE_LFSYNT)
    nrf_clock_lf_cfg_t cfg = {
      .source        = NRF_CLOCK_LF_SRC_SYNTH,
      .rc_ctiv       = 0,
      .rc_temp_ctiv  = 0,
      .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM
    };
  #else
    //default USE_LFXO
    nrf_clock_lf_cfg_t cfg = {
      .source        = NRF_CLOCK_LF_SRC_XTAL,
      .rc_ctiv       = 0,
      .rc_temp_ctiv  = 0,
      .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM
    };
  #endif

  sd_softdevice_enable(&cfg, NULL);
#else
  #if defined(USE_LFRC)
    sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION, NULL);
  #elif defined(USE_LFSYNT)
    sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, NULL);
  #else
    //default USE_LFXO
    sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);
  #endif
#endif

#if defined(NRF5) && !defined(S110)
  extern uint32_t __data_start__;
  uint32_t app_ram_base = (uint32_t) &__data_start__;
  ble_enable_params_t enableParams;

  memset(&enableParams, 0, sizeof(ble_enable_params_t));
  enableParams.common_enable_params.vs_uuid_count   = 10;
  enableParams.gatts_enable_params.attr_tab_size    = BLE_GATTS_ATTR_TAB_SIZE;
  enableParams.gatts_enable_params.service_changed  = 1;
  enableParams.gap_enable_params.periph_conn_count  = 1;
  enableParams.gap_enable_params.central_conn_count = 0;
  enableParams.gap_enable_params.central_sec_count  = 0;

  sd_ble_enable(&enableParams, &app_ram_base);
#elif defined(S110)
  ble_enable_params_t enableParams = {
      .gatts_enable_params = {
          .service_changed = true,
          .attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE
      }
  };

  sd_ble_enable(&enableParams);
#elif defined(NRF51_S130)
  ble_enable_params_t enableParams = {
      .gatts_enable_params = {
          .service_changed = true
      }
  };

  sd_ble_enable(&enableParams);
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

  ble_gap_conn_params_t gap_conn_params;

  gap_conn_params.min_conn_interval = 40;  // in 1.25ms units
  gap_conn_params.max_conn_interval = 80;  // in 1.25ms unit
  gap_conn_params.slave_latency     = 0;
  gap_conn_params.conn_sup_timeout  = 4000 / 10; // in 10ms unit

  sd_ble_gap_ppcp_set(&gap_conn_params);
  sd_ble_gap_tx_power_set(0);

  unsigned char srData[31];
  unsigned char srDataLen = 0;

  this->_advDataLen = 0;

  // flags
  this->_advData[this->_advDataLen + 0] = 2;
  this->_advData[this->_advDataLen + 1] = 0x01;
  this->_advData[this->_advDataLen + 2] = 0x06;

  this->_advDataLen += 3;

  if (advertisementDataSize && advertisementData) {
    for (int i = 0; i < advertisementDataSize; i++) {
      this->_advData[this->_advDataLen + 0] = advertisementData[i].length + 1;
      this->_advData[this->_advDataLen + 1] = advertisementData[i].type;
      this->_advDataLen += 2;

      memcpy(&this->_advData[this->_advDataLen], advertisementData[i].data, advertisementData[i].length);

      this->_advDataLen += advertisementData[i].length;
    }
  }

  if (scanDataSize && scanData) {
    for (int i = 0; i < scanDataSize; i++) {
      srData[srDataLen + 0] = scanData[i].length + 1;
      srData[srDataLen + 1] = scanData[i].type;
      srDataLen += 2;

      memcpy(&srData[srDataLen], scanData[i].data, scanData[i].length);

      srDataLen += scanData[i].length;
      _hasScanData = true;
    }
  }

  sd_ble_gap_adv_data_set(this->_advData, this->_advDataLen, srData, srDataLen);
  sd_ble_gap_appearance_set(0);

  for (int i = 0; i < numLocalAttributes; i++) {
    BLELocalAttribute *localAttribute = localAttributes[i];

    if (localAttribute->type() == BLETypeCharacteristic) {
      this->_numLocalCharacteristics++;
    }
  }

  this->_numLocalCharacteristics -= 3; // 0x2a00, 0x2a01, 0x2a05

  this->_localCharacteristicInfo = (struct localCharacteristicInfo*)malloc(sizeof(struct localCharacteristicInfo) * this->_numLocalCharacteristics);

  unsigned char localCharacteristicIndex = 0;

  uint16_t handle = 0;
  BLEService *lastService = NULL;

  for (int i = 0; i < numLocalAttributes; i++) {
    BLELocalAttribute *localAttribute = localAttributes[i];
    BLEUuid uuid = BLEUuid(localAttribute->uuid());
    const unsigned char* uuidData = uuid.data();
    unsigned char value[255];

    ble_uuid_t nordicUUID;

    if (uuid.length() == 2) {
      nordicUUID.uuid = (uuidData[1] << 8) | uuidData[0];
      nordicUUID.type = BLE_UUID_TYPE_BLE;
    } else {
      unsigned char uuidDataTemp[16];

      memcpy(&uuidDataTemp, uuidData, sizeof(uuidDataTemp));

      nordicUUID.uuid = (uuidData[13] << 8) | uuidData[12];

      uuidDataTemp[13] = 0;
      uuidDataTemp[12] = 0;

      sd_ble_uuid_vs_add((ble_uuid128_t*)&uuidDataTemp, &nordicUUID.type);
    }

    if (localAttribute->type() == BLETypeService) {
      BLEService *service = (BLEService *)localAttribute;

      if (strcmp(service->uuid(), "1800") == 0 || strcmp(service->uuid(), "1801") == 0) {
        continue; // skip
      }

      sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &nordicUUID, &handle);

      lastService = service;
    } else if (localAttribute->type() == BLETypeCharacteristic) {
      BLECharacteristic *characteristic = (BLECharacteristic *)localAttribute;

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

        this->_localCharacteristicInfo[localCharacteristicIndex].characteristic = characteristic;
        this->_localCharacteristicInfo[localCharacteristicIndex].notifySubscribed = false;
        this->_localCharacteristicInfo[localCharacteristicIndex].indicateSubscribed = false;
        this->_localCharacteristicInfo[localCharacteristicIndex].service = lastService;

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
        characteristicValueAttributeMetaData.vlen       = !characteristic->fixedLength();

        for (int j = (i + 1); j < numLocalAttributes; j++) {
          localAttribute = localAttributes[j];

          if (localAttribute->type() != BLETypeDescriptor) {
            break;
          }

          BLEDescriptor *descriptor = (BLEDescriptor *)localAttribute;

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

        sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID, &characteristicMetaData, &characteristicValueAttribute, &this->_localCharacteristicInfo[localCharacteristicIndex].handles);

        if (valueLength) {
          for (int j = 0; j < valueLength; j++) {
            value[j] = (*characteristic)[j];
          }

          sd_ble_gatts_value_set(this->_localCharacteristicInfo[localCharacteristicIndex].handles.value_handle, 0, &valueLength, value);
        }

        localCharacteristicIndex++;
      }
    } else if (localAttribute->type() == BLETypeDescriptor) {
      BLEDescriptor *descriptor = (BLEDescriptor *)localAttribute;

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
        for (int j = 0; j < valueLength; j++) {
          value[j] = (*descriptor)[j];
        }

        sd_ble_gatts_value_set(handle, 0, &valueLength, value);
      }
    }
  }

  if ( numRemoteAttributes > 0) {
    numRemoteAttributes -= 2; // 0x1801, 0x2a05
  }

  for (int i = 0; i < numRemoteAttributes; i++) {
    BLERemoteAttribute *remoteAttribute = remoteAttributes[i];

    if (remoteAttribute->type() == BLETypeService) {
      this->_numRemoteServices++;
    } else if (remoteAttribute->type() == BLETypeCharacteristic) {
      this->_numRemoteCharacteristics++;
    }
  }

  this->_remoteServiceInfo = (struct remoteServiceInfo*)malloc(sizeof(struct remoteServiceInfo) * this->_numRemoteServices);
  this->_remoteCharacteristicInfo = (struct remoteCharacteristicInfo*)malloc(sizeof(struct remoteCharacteristicInfo) * this->_numRemoteCharacteristics);

  BLERemoteService *lastRemoteService = NULL;
  unsigned char remoteServiceIndex = 0;
  unsigned char remoteCharacteristicIndex = 0;

  for (int i = 0; i < numRemoteAttributes; i++) {
    BLERemoteAttribute *remoteAttribute = remoteAttributes[i];
    BLEUuid uuid = BLEUuid(remoteAttribute->uuid());
    const unsigned char* uuidData = uuid.data();

    ble_uuid_t nordicUUID;

    if (uuid.length() == 2) {
      nordicUUID.uuid = (uuidData[1] << 8) | uuidData[0];
      nordicUUID.type = BLE_UUID_TYPE_BLE;
    } else {
      unsigned char uuidDataTemp[16];

      memcpy(&uuidDataTemp, uuidData, sizeof(uuidDataTemp));

      nordicUUID.uuid = (uuidData[13] << 8) | uuidData[12];

      uuidDataTemp[13] = 0;
      uuidDataTemp[12] = 0;

      sd_ble_uuid_vs_add((ble_uuid128_t*)&uuidDataTemp, &nordicUUID.type);
    }

    if (remoteAttribute->type() == BLETypeService) {
      this->_remoteServiceInfo[remoteServiceIndex].service = lastRemoteService = (BLERemoteService *)remoteAttribute;
      this->_remoteServiceInfo[remoteServiceIndex].uuid = nordicUUID;

      memset(&this->_remoteServiceInfo[remoteServiceIndex].handlesRange, 0, sizeof(this->_remoteServiceInfo[remoteServiceIndex].handlesRange));

      remoteServiceIndex++;
    } else if (remoteAttribute->type() == BLETypeCharacteristic) {
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].characteristic = (BLERemoteCharacteristic *)remoteAttribute;
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].service = lastRemoteService;
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].uuid = nordicUUID;

      memset(&this->_remoteCharacteristicInfo[remoteCharacteristicIndex].properties, 0, sizeof(this->_remoteCharacteristicInfo[remoteCharacteristicIndex].properties));
      this->_remoteCharacteristicInfo[remoteCharacteristicIndex].valueHandle = 0;

      remoteCharacteristicIndex++;
    }
  }

  if (this->_bondStore && this->_bondStore->hasData()) {
#ifdef NRF_51822_DEBUG
    Serial.println(F("Restoring bond data"));
#endif
#if defined(NRF5) || defined(NRF51_S130)
    this->_bondStore->getData(this->_bondData, 0, sizeof(this->_bondData));
#else
    this->_bondStore->getData(this->_authStatusBuffer, 0, sizeof(this->_authStatusBuffer));
#endif
  }

  this->startAdvertising();

#ifdef __RFduino__
  RFduinoBLE_enabled = 1;
#endif
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
        this->_txBufferCount += bleEvt->evt.common_evt.params.tx_complete.count;
        break;

      case BLE_GAP_EVT_CONNECTED:
#ifdef NRF_51822_DEBUG
        char address[18];

        BLEUtil::addressToString(bleEvt->evt.gap_evt.params.connected.peer_addr.addr, address);

        Serial.print(F("Evt Connected "));
        Serial.println(address);
#endif

        this->_connectionHandle = bleEvt->evt.gap_evt.conn_handle;

#if defined(NRF5) && !defined(S110)
        {
          uint8_t count;

          sd_ble_tx_packet_count_get(this->_connectionHandle, &count);

          this->_txBufferCount = count;
        }
#else
        sd_ble_tx_buffer_count_get(&this->_txBufferCount);
#endif

        if (this->_eventListener) {
          this->_eventListener->BLEDeviceConnected(*this, bleEvt->evt.gap_evt.params.connected.peer_addr.addr);
        }

        if (this->_minimumConnectionInterval >= BLE_GAP_CP_MIN_CONN_INTVL_MIN &&
            this->_maximumConnectionInterval <= BLE_GAP_CP_MAX_CONN_INTVL_MAX) {
          ble_gap_conn_params_t gap_conn_params;

          gap_conn_params.min_conn_interval = this->_minimumConnectionInterval;  // in 1.25ms units
          gap_conn_params.max_conn_interval = this->_maximumConnectionInterval;  // in 1.25ms unit
          gap_conn_params.slave_latency     = 0;
          gap_conn_params.conn_sup_timeout  = 4000 / 10; // in 10ms unit

          sd_ble_gap_conn_param_update(this->_connectionHandle, &gap_conn_params);
        }

        if (this->_numRemoteServices > 0) {
          sd_ble_gattc_primary_services_discover(this->_connectionHandle, 1, NULL);
        }
        break;

      case BLE_GAP_EVT_DISCONNECTED:
#ifdef NRF_51822_DEBUG
        Serial.println(F("Evt Disconnected"));
#endif
        this->_connectionHandle = BLE_CONN_HANDLE_INVALID;
        this->_txBufferCount = 0;

        for (int i = 0; i < this->_numLocalCharacteristics; i++) {
          struct localCharacteristicInfo* localCharacteristicInfo = &this->_localCharacteristicInfo[i];

          localCharacteristicInfo->notifySubscribed = false;
          localCharacteristicInfo->indicateSubscribed = false;

          if (localCharacteristicInfo->characteristic->subscribed()) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceCharacteristicSubscribedChanged(*this, *localCharacteristicInfo->characteristic, false);
            }
          }
        }

        if (this->_eventListener) {
          this->_eventListener->BLEDeviceDisconnected(*this);
        }

        // clear remote handle info
        for (int i = 0; i < this->_numRemoteServices; i++) {
          memset(&this->_remoteServiceInfo[i].handlesRange, 0, sizeof(this->_remoteServiceInfo[i].handlesRange));
        }

        for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
          memset(&this->_remoteCharacteristicInfo[i].properties, 0, sizeof(this->_remoteCharacteristicInfo[i].properties));
          this->_remoteCharacteristicInfo[i].valueHandle = 0;
        }

        this->_remoteRequestInProgress = false;

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
#if !defined(NRF5) && !defined(NRF51_S130)
        Serial.print(bleEvt->evt.gap_evt.params.sec_params_request.peer_params.timeout);
        Serial.print(F(" "));
#endif
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

          memset(&gapSecParams, 0x00, sizeof(ble_gap_sec_params_t));

#if defined(NRF5) && !defined(S110)
          gapSecParams.kdist_own.enc = 1;
#elif defined(NRF51_S130)
          gapSecParams.kdist_periph.enc = 1;
#elif !defined(NRF5)
          gapSecParams.timeout          = 30; // must be 30s
#endif
          gapSecParams.bond             = true;
          gapSecParams.mitm             = false;
          gapSecParams.io_caps          = BLE_GAP_IO_CAPS_NONE;
          gapSecParams.oob              = false;
          gapSecParams.min_key_size     = 7;
          gapSecParams.max_key_size     = 16;

#if defined(NRF5) && !defined(S110)
          ble_gap_sec_keyset_t keyset;

          keyset.keys_peer.p_enc_key  = NULL;
          keyset.keys_peer.p_id_key   = NULL;
          keyset.keys_peer.p_sign_key = NULL;
          keyset.keys_own.p_enc_key   = this->_encKey;
          keyset.keys_own.p_id_key    = NULL;
          keyset.keys_own.p_sign_key  = NULL;

          sd_ble_gap_sec_params_reply(this->_connectionHandle, BLE_GAP_SEC_STATUS_SUCCESS, &gapSecParams, &keyset);
#elif defined(NRF51_S130) || defined(S110)
          ble_gap_sec_keyset_t keyset;

          keyset.keys_central.p_enc_key  = NULL;
          keyset.keys_central.p_id_key   = NULL;
          keyset.keys_central.p_sign_key = NULL;
          keyset.keys_periph.p_enc_key   = this->_encKey;
          keyset.keys_periph.p_id_key    = NULL;
          keyset.keys_periph.p_sign_key  = NULL;

          sd_ble_gap_sec_params_reply(this->_connectionHandle, BLE_GAP_SEC_STATUS_SUCCESS, &gapSecParams, &keyset);
#else
          sd_ble_gap_sec_params_reply(this->_connectionHandle, BLE_GAP_SEC_STATUS_SUCCESS, &gapSecParams);
#endif
        } else {
#if defined(NRF5) || defined(NRF51_S130)
          sd_ble_gap_sec_params_reply(this->_connectionHandle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
#else
          sd_ble_gap_sec_params_reply(this->_connectionHandle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL);
#endif
        }
        break;

      case BLE_GAP_EVT_SEC_INFO_REQUEST:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Sec Info Request "));
        // Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.peer_addr);
        // Serial.print(F(" "));
#if defined(NRF5) || defined(NRF51_S130)
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.master_id.ediv);
#else
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.div);
#endif
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.enc_info);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.id_info);
        Serial.print(F(" "));
        Serial.print(bleEvt->evt.gap_evt.params.sec_info_request.sign_info);
        Serial.println();
#endif
#if defined(NRF5) || defined(NRF51_S130)
        if (this->_encKey->master_id.ediv == bleEvt->evt.gap_evt.params.sec_info_request.master_id.ediv) {
          sd_ble_gap_sec_info_reply(this->_connectionHandle, &this->_encKey->enc_info, NULL, NULL);
        } else {
          sd_ble_gap_sec_info_reply(this->_connectionHandle, NULL, NULL, NULL);
        }
#else
        if (this->_authStatus->periph_keys.enc_info.div == bleEvt->evt.gap_evt.params.sec_info_request.div) {
          sd_ble_gap_sec_info_reply(this->_connectionHandle, &this->_authStatus->periph_keys.enc_info, NULL);
        } else {
          sd_ble_gap_sec_info_reply(this->_connectionHandle, NULL, NULL);
        }
#endif
        break;

      case BLE_GAP_EVT_AUTH_STATUS:
#ifdef NRF_51822_DEBUG
        Serial.println(F("Evt Auth Status"));
        Serial.println(bleEvt->evt.gap_evt.params.auth_status.auth_status);
#endif
        if (BLE_GAP_SEC_STATUS_SUCCESS == bleEvt->evt.gap_evt.params.auth_status.auth_status) {
#if !defined(NRF5) && !defined(NRF51_S130)
          *this->_authStatus = bleEvt->evt.gap_evt.params.auth_status;
#endif
          if (this->_bondStore) {
#ifdef NRF_51822_DEBUG
            Serial.println(F("Storing bond data"));
#endif
#if defined(NRF5) || defined(NRF51_S130)
            this->_bondStore->putData(this->_bondData, 0, sizeof(this->_bondData));
#else
            this->_bondStore->putData(this->_authStatusBuffer, 0, sizeof(this->_authStatusBuffer));
#endif
          }

          if (this->_eventListener) {
            this->_eventListener->BLEDeviceBonded(*this);
          }
        }
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

        BLEUtil::printBuffer(bleEvt->evt.gatts_evt.params.write.data, bleEvt->evt.gatts_evt.params.write.len);
#endif

        uint16_t handle = bleEvt->evt.gatts_evt.params.write.handle;

        for (int i = 0; i < this->_numLocalCharacteristics; i++) {
          struct localCharacteristicInfo* localCharacteristicInfo = &this->_localCharacteristicInfo[i];

          if (localCharacteristicInfo->handles.value_handle == handle) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceCharacteristicValueChanged(*this, *localCharacteristicInfo->characteristic, bleEvt->evt.gatts_evt.params.write.data, bleEvt->evt.gatts_evt.params.write.len);
            }
            break;
          } else if (localCharacteristicInfo->handles.cccd_handle == handle) {
            uint8_t* data  = &bleEvt->evt.gatts_evt.params.write.data[0];
            uint16_t value = data[0] | (data[1] << 8);

            localCharacteristicInfo->notifySubscribed = (value & 0x0001);
            localCharacteristicInfo->indicateSubscribed = (value & 0x0002);

            bool subscribed = (localCharacteristicInfo->notifySubscribed || localCharacteristicInfo->indicateSubscribed);

            if (subscribed != localCharacteristicInfo->characteristic->subscribed()) {
              if (this->_eventListener) {
                this->_eventListener->BLEDeviceCharacteristicSubscribedChanged(*this, *localCharacteristicInfo->characteristic, subscribed);
              }
              break;
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
#if defined(NRF5) || defined(NRF51_S130)
        sd_ble_gatts_sys_attr_set(this->_connectionHandle, NULL, 0, 0);
#else
        sd_ble_gatts_sys_attr_set(this->_connectionHandle, NULL, 0);
#endif
        break;

      case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Prim Srvc Disc Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
#endif
        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS) {
          uint16_t count = bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.count;
          for (int i = 0; i < count; i++) {
            for (int j = 0; j < this->_numRemoteServices; j++) {
              if ((bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].uuid.type == this->_remoteServiceInfo[j].uuid.type) &&
                  (bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].uuid.uuid == this->_remoteServiceInfo[j].uuid.uuid)) {
                this->_remoteServiceInfo[j].handlesRange = bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].handle_range;
                break;
              }
            }
          }

          uint16_t startHandle = bleEvt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[count - 1].handle_range.end_handle + 1;

          sd_ble_gattc_primary_services_discover(this->_connectionHandle, startHandle, NULL);
        } else {
          // done discovering services
          for (int i = 0; i < this->_numRemoteServices; i++) {
            if (this->_remoteServiceInfo[i].handlesRange.start_handle != 0 && this->_remoteServiceInfo[i].handlesRange.end_handle != 0) {
              this->_remoteServiceDiscoveryIndex = i;

              sd_ble_gattc_characteristics_discover(this->_connectionHandle, &this->_remoteServiceInfo[i].handlesRange);
              break;
            }
          }
        }
        break;

      case BLE_GATTC_EVT_CHAR_DISC_RSP:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Char Disc Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
#endif
        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS) {
          ble_gattc_handle_range_t serviceHandlesRange = this->_remoteServiceInfo[this->_remoteServiceDiscoveryIndex].handlesRange;

          uint16_t count = bleEvt->evt.gattc_evt.params.char_disc_rsp.count;

          for (int i = 0; i < count; i++) {
            for (int j = 0; j < this->_numRemoteCharacteristics; j++) {
              if ((this->_remoteServiceInfo[this->_remoteServiceDiscoveryIndex].service == this->_remoteCharacteristicInfo[j].service) &&
                  (bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.type == this->_remoteCharacteristicInfo[j].uuid.type) &&
                  (bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.uuid == this->_remoteCharacteristicInfo[j].uuid.uuid)) {
                this->_remoteCharacteristicInfo[j].properties = bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].char_props;
                this->_remoteCharacteristicInfo[j].valueHandle = bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].handle_value;
              }
            }

            serviceHandlesRange.start_handle = bleEvt->evt.gattc_evt.params.char_disc_rsp.chars[i].handle_value;
          }

          sd_ble_gattc_characteristics_discover(this->_connectionHandle, &serviceHandlesRange);
        } else {
          bool discoverCharacteristics = false;

          for (int i = this->_remoteServiceDiscoveryIndex + 1; i < this->_numRemoteServices; i++) {
            if (this->_remoteServiceInfo[i].handlesRange.start_handle != 0 && this->_remoteServiceInfo[i].handlesRange.end_handle != 0) {
              this->_remoteServiceDiscoveryIndex = i;

              sd_ble_gattc_characteristics_discover(this->_connectionHandle, &this->_remoteServiceInfo[i].handlesRange);
              discoverCharacteristics = true;
              break;
            }
          }

          if (!discoverCharacteristics) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceRemoteServicesDiscovered(*this);
            }
          }
        }
        break;

      case BLE_GATTC_EVT_READ_RSP: {
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Read Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
        Serial.println(bleEvt->evt.gattc_evt.params.read_rsp.handle, DEC);
        BLEUtil::printBuffer(bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp.len);
#endif
        this->_remoteRequestInProgress = false;

        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_ATTERR_INSUF_AUTHENTICATION &&
            this->_bondStore) {
          ble_gap_sec_params_t gapSecParams;

          memset(&gapSecParams, 0x00, sizeof(ble_gap_sec_params_t));

#if defined(NRF5) && !defined(S110)
          gapSecParams.kdist_own.enc = 1;
#elif defined(NRF51_S130)
          gapSecParams.kdist_periph.enc = 1;
#elif !defined(NRF5)
          gapSecParams.timeout          = 30; // must be 30s
#endif
          gapSecParams.bond             = true;
          gapSecParams.mitm             = false;
          gapSecParams.io_caps          = BLE_GAP_IO_CAPS_NONE;
          gapSecParams.oob              = false;
          gapSecParams.min_key_size     = 7;
          gapSecParams.max_key_size     = 16;

          sd_ble_gap_authenticate(this->_connectionHandle, &gapSecParams);
        } else {
          uint16_t handle = bleEvt->evt.gattc_evt.params.read_rsp.handle;

          for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
            if (this->_remoteCharacteristicInfo[i].valueHandle == handle) {
              if (this->_eventListener) {
                this->_eventListener->BLEDeviceRemoteCharacteristicValueChanged(*this, *this->_remoteCharacteristicInfo[i].characteristic, bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp. len);
              }
              break;
            }
          }
        }
        break;
      }

      case BLE_GATTC_EVT_WRITE_RSP:
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Write Rsp 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
        Serial.println(bleEvt->evt.gattc_evt.params.write_rsp.handle, DEC);
#endif
        this->_remoteRequestInProgress = false;

        if (bleEvt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_ATTERR_INSUF_AUTHENTICATION &&
            this->_bondStore) {
          ble_gap_sec_params_t gapSecParams;

          memset(&gapSecParams, 0x00, sizeof(ble_gap_sec_params_t));

#if defined(NRF5) && !defined(S110)
          gapSecParams.kdist_own.enc = 1;
#elif defined(NRF51_S130)
          gapSecParams.kdist_periph.enc = 1;
#elif !defined(NRF5)
          gapSecParams.timeout          = 30; // must be 30s
#endif
          gapSecParams.bond             = true;
          gapSecParams.mitm             = false;
          gapSecParams.io_caps          = BLE_GAP_IO_CAPS_NONE;
          gapSecParams.oob              = false;
          gapSecParams.min_key_size     = 7;
          gapSecParams.max_key_size     = 16;

          sd_ble_gap_authenticate(this->_connectionHandle, &gapSecParams);
        }
        break;

      case BLE_GATTC_EVT_HVX: {
#ifdef NRF_51822_DEBUG
        Serial.print(F("Evt Hvx 0x"));
        Serial.println(bleEvt->evt.gattc_evt.gatt_status, HEX);
        Serial.println(bleEvt->evt.gattc_evt.params.hvx.handle, DEC);
#endif
        uint16_t handle = bleEvt->evt.gattc_evt.params.hvx.handle;

        if (bleEvt->evt.gattc_evt.params.hvx.type == BLE_GATT_HVX_INDICATION) {
          sd_ble_gattc_hv_confirm(this->_connectionHandle, handle);
        }

        for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
          if (this->_remoteCharacteristicInfo[i].valueHandle == handle) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceRemoteCharacteristicValueChanged(*this, *this->_remoteCharacteristicInfo[i].characteristic, bleEvt->evt.gattc_evt.params.read_rsp.data, bleEvt->evt.gattc_evt.params.read_rsp. len);
            }
            break;
          }
        }
        break;
      }

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

void nRF51822::end() {
  sd_softdevice_disable();

  if (this->_remoteCharacteristicInfo) {
    free(this->_remoteCharacteristicInfo);
  }

  if (this->_remoteServiceInfo) {
    free(this->_remoteServiceInfo);
  }

  if (this->_localCharacteristicInfo) {
    free(this->_localCharacteristicInfo);
  }

  this->_numLocalCharacteristics = 0;
  this->_numRemoteServices = 0;
  this->_numRemoteCharacteristics = 0;
}

bool nRF51822::updateCharacteristicValue(BLECharacteristic& characteristic) {
  bool success = true;

  for (int i = 0; i < this->_numLocalCharacteristics; i++) {
    struct localCharacteristicInfo* localCharacteristicInfo = &this->_localCharacteristicInfo[i];

    if (localCharacteristicInfo->characteristic == &characteristic) {
      if (&characteristic == this->_broadcastCharacteristic) {
        this->broadcastCharacteristic(characteristic);
      }

      uint16_t valueLength = characteristic.valueLength();

      sd_ble_gatts_value_set(localCharacteristicInfo->handles.value_handle, 0, &valueLength, characteristic.value());

      ble_gatts_hvx_params_t hvxParams;

      memset(&hvxParams, 0, sizeof(hvxParams));

      hvxParams.handle = localCharacteristicInfo->handles.value_handle;
      hvxParams.offset = 0;
      hvxParams.p_data = NULL;
      hvxParams.p_len  = &valueLength;

      if (localCharacteristicInfo->notifySubscribed) {
        if (this->_txBufferCount > 0) {
          this->_txBufferCount--;

          hvxParams.type = BLE_GATT_HVX_NOTIFICATION;

          sd_ble_gatts_hvx(this->_connectionHandle, &hvxParams);
        } else {
          success = false;
        }
      }

      if (localCharacteristicInfo->indicateSubscribed) {
        if (this->_txBufferCount > 0) {
          this->_txBufferCount--;

          hvxParams.type = BLE_GATT_HVX_INDICATION;

          sd_ble_gatts_hvx(this->_connectionHandle, &hvxParams);
        } else {
          success = false;
        }
      }
    }
  }

  return success;
}

bool nRF51822::broadcastCharacteristic(BLECharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numLocalCharacteristics; i++) {
    struct localCharacteristicInfo* localCharacteristicInfo = &this->_localCharacteristicInfo[i];

    if (localCharacteristicInfo->characteristic == &characteristic) {
      if (characteristic.properties() & BLEBroadcast && localCharacteristicInfo->service) {
        unsigned char advData[31];
        unsigned char advDataLen = this->_advDataLen;

        // copy the existing advertisement data
        memcpy(advData, this->_advData, advDataLen);

        advDataLen += (4 + characteristic.valueLength());

        if (advDataLen <= 31) {
          BLEUuid uuid = BLEUuid(localCharacteristicInfo->service->uuid());

          advData[this->_advDataLen + 0] = 3 + characteristic.valueLength();
          advData[this->_advDataLen + 1] = 0x16;

          memcpy(&advData[this->_advDataLen + 2], uuid.data(), 2);
          memcpy(&advData[this->_advDataLen + 4], characteristic.value(), characteristic.valueLength());

          sd_ble_gap_adv_data_set(advData, advDataLen, NULL, 0); // update advertisement data
          success = true;

          this->_broadcastCharacteristic = &characteristic;
        }
      }
      break;
    }
  }

  return success;
}

bool nRF51822::canNotifyCharacteristic(BLECharacteristic& /*characteristic*/) {
  return (this->_txBufferCount > 0);
}

bool nRF51822::canIndicateCharacteristic(BLECharacteristic& /*characteristic*/) {
  return (this->_txBufferCount > 0);
}

bool nRF51822::canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      success = (this->_remoteCharacteristicInfo[i].valueHandle &&
                  this->_remoteCharacteristicInfo[i].properties.read &&
                  !this->_remoteRequestInProgress);
      break;
    }
  }

  return success;
}

bool nRF51822::readRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle && this->_remoteCharacteristicInfo[i].properties.read) {
        this->_remoteRequestInProgress = true;
        success = (sd_ble_gattc_read(this->_connectionHandle, this->_remoteCharacteristicInfo[i].valueHandle, 0) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}

bool nRF51822::canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle) {
        if (this->_remoteCharacteristicInfo[i].properties.write) {
          success = !this->_remoteRequestInProgress;
        } else if (this->_remoteCharacteristicInfo[i].properties.write_wo_resp) {
          success = (this->_txBufferCount > 0);
        }
      }
      break;
    }
  }

  return success;
}

bool nRF51822::writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle &&
                  (this->_remoteCharacteristicInfo[i].properties.write_wo_resp || this->_remoteCharacteristicInfo[i].properties.write) &&
                  (this->_txBufferCount > 0)) {

        ble_gattc_write_params_t writeParams;

        writeParams.write_op = (this->_remoteCharacteristicInfo[i].properties.write) ? BLE_GATT_OP_WRITE_REQ : BLE_GATT_OP_WRITE_CMD;
#ifndef __RFduino__
        writeParams.flags = 0;
#endif
        writeParams.handle = this->_remoteCharacteristicInfo[i].valueHandle;
        writeParams.offset = 0;
        writeParams.len = length;
        writeParams.p_value = (uint8_t*)value;

        this->_remoteRequestInProgress = true;

        this->_txBufferCount--;

        success = (sd_ble_gattc_write(this->_connectionHandle, &writeParams) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}

bool nRF51822::canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      success = (this->_remoteCharacteristicInfo[i].valueHandle &&
                (this->_remoteCharacteristicInfo[i].properties.notify || this->_remoteCharacteristicInfo[i].properties.indicate));
      break;
    }
  }

  return success;
}

bool nRF51822::subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle &&
                  (this->_remoteCharacteristicInfo[i].properties.notify || this->_remoteCharacteristicInfo[i].properties.indicate)) {

        ble_gattc_write_params_t writeParams;

        uint16_t value = (this->_remoteCharacteristicInfo[i].properties.notify ? 0x0001 : 0x002);

        writeParams.write_op = BLE_GATT_OP_WRITE_REQ;
#ifndef __RFduino__
        writeParams.flags = 0;
#endif
        writeParams.handle = (this->_remoteCharacteristicInfo[i].valueHandle + 1); // don't discover descriptors for now
        writeParams.offset = 0;
        writeParams.len = sizeof(value);
        writeParams.p_value = (uint8_t*)&value;

        this->_remoteRequestInProgress = true;

        success = (sd_ble_gattc_write(this->_connectionHandle, &writeParams) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}

bool nRF51822::canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->canSubscribeRemoteCharacteristic(characteristic);
}

bool nRF51822::unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  for (int i = 0; i < this->_numRemoteCharacteristics; i++) {
    if (this->_remoteCharacteristicInfo[i].characteristic == &characteristic) {
      if (this->_remoteCharacteristicInfo[i].valueHandle &&
                  (this->_remoteCharacteristicInfo[i].properties.notify || this->_remoteCharacteristicInfo[i].properties.indicate)) {

        ble_gattc_write_params_t writeParams;

        uint16_t value = 0x0000;

        writeParams.write_op = BLE_GATT_OP_WRITE_REQ;
#ifndef __RFduino__
        writeParams.flags = 0;
#endif
        writeParams.handle = (this->_remoteCharacteristicInfo[i].valueHandle + 1); // don't discover descriptors for now
        writeParams.offset = 0;
        writeParams.len = sizeof(value);
        writeParams.p_value = (uint8_t*)&value;

        this->_remoteRequestInProgress = true;

        success = (sd_ble_gattc_write(this->_connectionHandle, &writeParams) == NRF_SUCCESS);
      }
      break;
    }
  }

  return success;
}

bool nRF51822::setTxPower(int txPower) {
  if (txPower <= -40) {
    txPower = -40;
  } else if (txPower <= -30) {
    txPower = -30;
  } else if (txPower <= -20) {
    txPower = -20;
  } else if (txPower <= -16) {
    txPower = -16;
  } else if (txPower <= -12) {
    txPower = -12;
  } else if (txPower <= -8) {
    txPower = -8;
  } else if (txPower <= -4) {
    txPower = -4;
  } else if (txPower <= 0) {
    txPower = 0;
  } else {
    txPower = 4;
  }

  return (sd_ble_gap_tx_power_set(txPower) == NRF_SUCCESS);
}

void nRF51822::startAdvertising() {
#ifdef NRF_51822_DEBUG
  Serial.println(F("Start advertisement"));
#endif

  ble_gap_adv_params_t advertisingParameters;

  memset(&advertisingParameters, 0x00, sizeof(advertisingParameters));

  advertisingParameters.type        = this->_connectable ? BLE_GAP_ADV_TYPE_ADV_IND : ( this->_hasScanData ? BLE_GAP_ADV_TYPE_ADV_SCAN_IND : BLE_GAP_ADV_TYPE_ADV_NONCONN_IND );
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
