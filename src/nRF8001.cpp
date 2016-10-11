// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(NRF51) && !defined(NRF52) &&!defined(__RFduino__)

// #define NRF_8001_DEBUG
// #define NRF_8001_ENABLE_DC_DC_CONVERTER

#include <SPI.h>

#include "BLECharacteristic.h"
#include "BLEDescriptor.h"
#include "BLERemoteService.h"
#include "BLERemoteCharacteristic.h"
#include "BLEService.h"
#include "BLEUtil.h"
#include "BLEUuid.h"

#include "nRF8001.h"

struct setupMsgData {
  unsigned char length;
  unsigned char cmd;
  unsigned char type;
  unsigned char offset;
  unsigned char data[28];
};

#define NB_BASE_SETUP_MESSAGES                  7
#define MAX_ATTRIBUTE_VALUE_PER_SETUP_MSG       28

#define DYNAMIC_DATA_MAX_CHUNK_SIZE             26
#define DYNAMIC_DATA_SIZE                       (DYNAMIC_DATA_MAX_CHUNK_SIZE * 6)

#if defined (__AVR__)
  /* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
  #if ARDUINO < 150
static /*const*/ hal_aci_data_t baseSetupMsgs[NB_BASE_SETUP_MESSAGES] PROGMEM =
  #else
static const hal_aci_data_t baseSetupMsgs[NB_BASE_SETUP_MESSAGES] PROGMEM =
  #endif
#else
/* Having PROGMEM here caused the setup messages to be zeroed out */
static const hal_aci_data_t baseSetupMsgs[NB_BASE_SETUP_MESSAGES] /*PROGMEM*/ =
#endif
{\
  {0x00,\
    {\
      0x07,0x06,0x00,0x00,0x03,0x02,0x41,0xfe,\
    },\
  },\
  {0x00,\
    {\
      0x1f,0x06,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x06,0x00,0x06,\
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
    },\
  },\
  {0x00,\
    {\
      0x1f,0x06,0x10,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
      0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x03,0x90,0x00,0xff,\
    },\
  },\
  {0x00,\
    {\
      0x1f,0x06,0x10,0x38,0xff,0xff,0x02,0x58,0x0a,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
      0x10,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x01,0x02,0x01,0x02,\
    },\
  },\
  {0x00,\
    {\
      0x05,0x06,0x10,0x54,0x00,0x02,\
    },\
  },\
  {0x00,\
    {\
      0x19,0x06,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
      0x00,0x00,0x00,0x00,0x00,0x00,\
    },\
  },\
  {0x00,\
    {\
      0x19,0x06,0x70,0x16,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
      0x00,0x00,0x00,0x00,0x00,0x00,\
    },\
  },\
};

/** crc function to re-calulate the CRC after making changes to the setup data.
*/
uint16_t crc_16_ccitt(uint16_t crc, uint8_t * data_in, uint16_t data_len) {

  uint16_t i;

  for(i = 0; i < data_len; i++)
  {
    crc  = (unsigned char)(crc >> 8) | (crc << 8);
    crc ^= data_in[i];
    crc ^= (unsigned char)(crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
  }

  return crc;
}

nRF8001::nRF8001(unsigned char req, unsigned char rdy, unsigned char rst) :
  BLEDevice(),

  _localPipeInfo(NULL),
  _numLocalPipeInfo(0),
  _broadcastPipe(0),

  _timingChanged(false),
  _closedPipesCleared(false),
  _remoteServicesDiscovered(false),
  _remotePipeInfo(NULL),
  _numRemotePipeInfo(0),

  _dynamicDataOffset(0),
  _dynamicDataSequenceNo(0),
  _storeDynamicData(false),

  _crcSeed(0xFFFF)
{
  this->_aciState.aci_pins.reqn_pin               = req;
  this->_aciState.aci_pins.rdyn_pin               = rdy;

#if !defined(SPI_HAS_TRANSACTION) || defined(__SAMD21G18A__)
#if defined(__SAM3X8E__)
  this->_aciState.aci_pins.spi_clock_divider      = 42;
#else
  this->_aciState.aci_pins.spi_clock_divider      = SPI_CLOCK_DIV8;
#endif
#endif

  this->_aciState.aci_pins.reset_pin              = rst;
  this->_aciState.aci_pins.active_pin             = UNUSED;
  this->_aciState.aci_pins.optional_chip_sel_pin  = UNUSED;
}

nRF8001::~nRF8001() {
  this->end();
}

void nRF8001::begin(unsigned char advertisementDataSize,
                      BLEEirData *advertisementData,
                      unsigned char scanDataSize,
                      BLEEirData *scanData,
                      BLELocalAttribute** localAttributes,
                      unsigned char numLocalAttributes,
                      BLERemoteAttribute** remoteAttributes,
                      unsigned char numRemoteAttributes)
{
  unsigned char numLocalPipedCharacteristics = 0;
  unsigned char numLocalPipes = 0;

  this->_crcSeed = 0xFFFF;

  for (int i = 0; i < numLocalAttributes; i++) {
    BLELocalAttribute* localAttribute = localAttributes[i];

    if (localAttribute->type() == BLETypeCharacteristic) {
      BLECharacteristic* characteristic = (BLECharacteristic *)localAttribute;
      unsigned char properties = characteristic->properties();

      if (properties) {
        numLocalPipedCharacteristics++;

        if (properties & BLEBroadcast) {
          numLocalPipes++;
        }

        if (properties & BLENotify) {
          numLocalPipes++;
        }

        if (properties & BLEIndicate) {
          numLocalPipes++;
        }

        if (properties & BLEWriteWithoutResponse) {
          numLocalPipes++;
        }

        if (properties & BLEWrite) {
          numLocalPipes++;
        }

        if (properties & BLERead) {
          numLocalPipes++;
        }
      }
    }
  }

  this->_localPipeInfo = (struct localPipeInfo*)malloc(sizeof(struct localPipeInfo) * numLocalPipedCharacteristics);

  unsigned char numRemoteServices = 0;
  unsigned char numRemotePipedCharacteristics = 0;
  unsigned char numRemotePipes = 0;
  unsigned char numCustomUuids = 0;

  for (int i = 0; i < numRemoteAttributes; i++) {
    BLERemoteAttribute* remoteAttribute = remoteAttributes[i];
    unsigned short remoteAttributeType = remoteAttribute->type();
    BLEUuid uuid = BLEUuid(remoteAttribute->uuid());

    if (uuid.length() > 2) {
      numCustomUuids++;
    }

    if (remoteAttributeType == BLETypeService) {
      numRemoteServices++;
    } else if (remoteAttributeType == BLETypeCharacteristic) {
      BLERemoteCharacteristic* characteristic = (BLERemoteCharacteristic *)remoteAttribute;
      unsigned char properties = characteristic->properties();

      if (properties) {
        numRemotePipedCharacteristics++;

        if (properties & BLENotify) {
          numRemotePipes++;
        }

        if (properties & BLEIndicate) {
          numRemotePipes++;
        }

        if (properties & BLEWriteWithoutResponse) {
          numRemotePipes++;
        }

        if (properties & BLEWrite) {
          numRemotePipes++;
        }

        if (properties & BLERead) {
          numRemotePipes++;
        }
      }
    }
  }

  this->_remotePipeInfo = (struct remotePipeInfo*)malloc(sizeof(struct remotePipeInfo) * numRemotePipedCharacteristics);

  lib_aci_init(&this->_aciState, false);

  if (this->_bondStore) {
    this->_aciState.bonded = ACI_BOND_STATUS_FAILED;

    this->_storeDynamicData = false;
  }

  this->waitForSetupMode();

  hal_aci_data_t setupMsg;
  struct setupMsgData* setupMsgData = (struct setupMsgData*)setupMsg.buffer;

  setupMsg.status_byte = 0;

  bool hasAdvertisementData = advertisementDataSize && advertisementData;
  bool hasScanData          = scanDataSize && scanData;

  for (int i = 0; i < NB_BASE_SETUP_MESSAGES; i++) {
    int setupMsgSize = pgm_read_byte_near(&baseSetupMsgs[i].buffer[0]) + 2;

    memcpy_P(&setupMsg, &baseSetupMsgs[i], setupMsgSize);

    if (i == 1) {
      setupMsgData->data[5] = numRemoteServices;
      setupMsgData->data[6] = numLocalPipedCharacteristics;
      setupMsgData->data[7] = numRemotePipedCharacteristics;
      setupMsgData->data[8] = (numLocalPipes + numRemotePipes);

      if (this->_bondStore) {
        setupMsgData->data[4] |= 0x02;
      }

#ifdef NRF_8001_ENABLE_DC_DC_CONVERTER
      setupMsgData->data[13] |= 0x01;
#endif
    } else if (i == 2 && hasAdvertisementData) {
      setupMsgData->data[18] |= 0x40;

      setupMsgData->data[22] |= 0x40;

      setupMsgData->data[26] = numCustomUuids;
    } else if (i == 3) {
      if (hasAdvertisementData) {
        setupMsgData->data[16] |= 0x40;
      }

      if (hasScanData) {
        setupMsgData->data[8] |= 0x40;

        setupMsgData->data[12] |= 0x40;

        setupMsgData->data[20] |= 0x40;
      }
    } else if (i == 4) {
      if (this->_bondStore) {
        setupMsgData->data[0] |= 0x01;
      }
    } else if (i == 5 && hasAdvertisementData) {
      setupMsgData->data[0] = advertisementData[0].type;
      setupMsgData->data[1] = advertisementData[0].length;
      memcpy(&setupMsgData->data[2], advertisementData[0].data, advertisementData[0].length);
    } else if (i == 6 && hasScanData) {
      setupMsgData->data[0] = scanData[0].type;
      setupMsgData->data[1] = scanData[0].length;
      memcpy(&setupMsgData->data[2], scanData[0].data, scanData[0].length);
    }

    this->sendSetupMessage(&setupMsg);
  }

  // GATT
  unsigned short gattSetupMsgOffset = 0;
  unsigned short handle             = 1;
  unsigned char  pipe               = 1;
  unsigned char  numLocalPiped      = 0;
  unsigned char  numRemotePiped     = 0;

  for (int i = 0; i < numLocalAttributes; i++) {
    BLELocalAttribute* localAttribute = localAttributes[i];
    unsigned short localAttributeType = localAttribute->type();
    BLEUuid uuid = BLEUuid(localAttribute->uuid());
    const unsigned char* uuidData = uuid.data();
    unsigned char uuidLength = uuid.length();

    if (localAttributeType == BLETypeService) {
      setupMsgData->length  = 12 + uuidLength;

      setupMsgData->data[0] = 0x04;
      setupMsgData->data[1] = 0x04;
      setupMsgData->data[2] = uuidLength;
      setupMsgData->data[3] = uuidLength;

      setupMsgData->data[4] = (handle >> 8) & 0xff;
      setupMsgData->data[5] = handle & 0xff;
      handle++;

      setupMsgData->data[6] = (localAttributeType >> 8) & 0xff;
      setupMsgData->data[7] = localAttributeType & 0xff;

      setupMsgData->data[8] = ACI_STORE_LOCAL;

      memcpy(&setupMsgData->data[9], uuidData, uuidLength);

      this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);
    } else if (localAttributeType == BLETypeCharacteristic) {
      BLECharacteristic* characteristic = (BLECharacteristic *)localAttribute;
      unsigned char properties = characteristic->properties();
      const char* characteristicUuid = characteristic->uuid();

      struct localPipeInfo* localPipeInfo = &this->_localPipeInfo[numLocalPiped];

      memset(localPipeInfo, 0, sizeof(struct localPipeInfo));

      localPipeInfo->characteristic = characteristic;

      if (properties) {
        numLocalPiped++;

        if (properties & BLEBroadcast) {
          localPipeInfo->advPipe = pipe;

          pipe++;
        }

        if (properties & BLENotify) {
          localPipeInfo->txPipe = pipe;

          pipe++;
        }

        if (properties & BLEIndicate) {
          localPipeInfo->txAckPipe = pipe;

          pipe++;
        }

        if (properties & BLEWriteWithoutResponse) {
          localPipeInfo->rxPipe = pipe;

          pipe++;
        }

        if (properties & BLEWrite) {
          localPipeInfo->rxAckPipe = pipe;

          pipe++;
        }

        if (properties & BLERead) {
          localPipeInfo->setPipe = pipe;

          pipe++;
        }
      }

      setupMsgData->length   = 15 + uuidLength;

      setupMsgData->data[0]  = 0x04;
      setupMsgData->data[1]  = 0x04;
      setupMsgData->data[2]  = 3 + uuidLength;
      setupMsgData->data[3]  = 3 + uuidLength;

      setupMsgData->data[4]  = (handle >> 8) & 0xff;
      setupMsgData->data[5]  = handle & 0xff;
      handle++;

      setupMsgData->data[6]  = (localAttributeType >> 8) & 0xff;
      setupMsgData->data[7]  = localAttributeType & 0xff;

      setupMsgData->data[8]  = ACI_STORE_LOCAL;
      setupMsgData->data[9]  = properties & 0xfe;

      setupMsgData->data[10] = handle & 0xff;
      setupMsgData->data[11] = (handle >> 8) & 0xff;
      localPipeInfo->valueHandle = handle;
      handle++;

      memcpy(&setupMsgData->data[12], uuidData, uuidLength);

      this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);

      setupMsgData->length   = 12;

      setupMsgData->data[0]  = 0x04;
      setupMsgData->data[1]  = 0x00;

      if (characteristic->fixedLength()) {
        setupMsgData->data[0] |= 0x02;
      }

      if (properties & BLERead) {
        setupMsgData->data[1] |= 0x04;
      }

      if (this->_bondStore &&
          strcmp(characteristicUuid, "2a00") != 0 &&
          strcmp(characteristicUuid, "2a01") != 0 &&
          strcmp(characteristicUuid, "2a05") != 0) {
        setupMsgData->data[1] |= 0x08;
      }

      if (properties & (BLEWrite | BLEWriteWithoutResponse)) {
        setupMsgData->data[0] |= 0x40;
        setupMsgData->data[1] |= 0x10;
      }

      if (properties & BLENotify) {
        setupMsgData->data[0] |= 0x10;
      }

      if (properties & BLEIndicate) {
        setupMsgData->data[0] |= 0x20;
      }

      unsigned char valueSize = characteristic->valueSize();
      unsigned char valueLength = characteristic->valueLength();

      setupMsgData->data[2]  = valueSize;
      if (characteristic->fixedLength()) {
        setupMsgData->data[2]++;
      }

      setupMsgData->data[3]  = valueLength;

      setupMsgData->data[4]  = (localPipeInfo->valueHandle >> 8) & 0xff;
      setupMsgData->data[5]  = localPipeInfo->valueHandle & 0xff;

      if (uuidLength == 2) {
        setupMsgData->data[6]  = uuidData[1];
        setupMsgData->data[7]  = uuidData[0];
      } else {
        setupMsgData->data[6]  = 0x00;
        setupMsgData->data[7]  = 0x00;
      }

      setupMsgData->data[8]  = ACI_STORE_LOCAL;

      this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);

      int valueOffset = 0;

      while(valueOffset < valueSize) {
        int chunkSize = min(valueSize - valueOffset, MAX_ATTRIBUTE_VALUE_PER_SETUP_MSG);
        int valueCopySize = min(valueLength - valueOffset, chunkSize);

        setupMsgData->length = 3 + chunkSize;

        memset(setupMsgData->data, 0x00, chunkSize);

        if (valueCopySize > 0) {
          for (int j = 0; j < chunkSize; j++) {
            setupMsgData->data[j] = (*characteristic)[j + valueOffset];
          }
        }

        this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);

        valueOffset += chunkSize;
      }

      if (properties & (BLENotify | BLEIndicate)) {
        setupMsgData->length   = 14;

        setupMsgData->data[0]  = 0x46;
        setupMsgData->data[1]  = 0x14;

        setupMsgData->data[2]  = 0x03;
        setupMsgData->data[3]  = 0x02;

        setupMsgData->data[4]  = (handle >> 8) & 0xff;
        setupMsgData->data[5]  = handle & 0xff;
        localPipeInfo->configHandle = handle;
        handle++;

        setupMsgData->data[6]  = 0x29;
        setupMsgData->data[7]  = 0x02;

        setupMsgData->data[8]  = ACI_STORE_LOCAL;

        setupMsgData->data[9]  = 0x00;
        setupMsgData->data[10] = 0x00;

        this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);
      }
    } else if (localAttributeType == BLETypeDescriptor) {
      BLEDescriptor* descriptor = (BLEDescriptor *)localAttribute;

      setupMsgData->length   = 12;

      setupMsgData->data[0]  = 0x04;
      setupMsgData->data[1]  = 0x04;

      unsigned char valueLength = descriptor->valueLength();

      setupMsgData->data[2]  = valueLength;
      setupMsgData->data[3]  = valueLength;

      setupMsgData->data[4]  = (handle >> 8) & 0xff;
      setupMsgData->data[5]  = handle & 0xff;
      handle++;

      setupMsgData->data[6]  = uuidData[1];
      setupMsgData->data[7]  = uuidData[0];

      setupMsgData->data[8]  = ACI_STORE_LOCAL;

      this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);

      int valueOffset = 0;

      while(valueOffset < valueLength) {
        int chunkSize = min(valueLength - valueOffset, MAX_ATTRIBUTE_VALUE_PER_SETUP_MSG);

        setupMsgData->length = 3 + chunkSize;

        for (int j = 0; j < chunkSize; j++) {
          setupMsgData->data[j] = (*descriptor)[j + valueOffset];
        }

        this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);

        valueOffset += chunkSize;
      }
    }
  }

  this->_numLocalPipeInfo = numLocalPiped;

  // terminator
  setupMsgData->length   = 4;

  setupMsgData->data[0]  = 0x00;

  this->sendSetupMessage(&setupMsg, 0x2, gattSetupMsgOffset);

  unsigned short remoteServiceSetupMsgOffset  = 0;
  unsigned char  customUuidIndex = 2;

  for (int i = 0; i < numRemoteAttributes; i++) {
    BLERemoteAttribute* remoteAttribute = remoteAttributes[i];
    BLEUuid uuid = BLEUuid(remoteAttribute->uuid());
    const unsigned char* uuidData = uuid.data();
    unsigned char uuidLength = uuid.length();

    if (remoteAttribute->type() == BLETypeService) {
      unsigned char numRemoteCharacteristics = 0;
      setupMsgData->data[3]    = (pipe - 1);

      for (int j = (i + 1); j < numRemoteAttributes; j++) {
        if (remoteAttributes[j]->type() == BLETypeCharacteristic) {
          BLERemoteCharacteristic* remoteCharateristic = (BLERemoteCharacteristic*)remoteAttributes[j];

          struct remotePipeInfo* remotePipeInfo = &this->_remotePipeInfo[numRemotePiped];
          memset(remotePipeInfo, 0, sizeof(struct remotePipeInfo));

          unsigned char properties = remoteCharateristic->properties();

          remotePipeInfo->characteristic = remoteCharateristic;

          if (properties & BLEWriteWithoutResponse) {
            remotePipeInfo->txPipe = pipe;

            pipe++;
          }

          if (properties & BLEWrite) {
            remotePipeInfo->txAckPipe = pipe;

            pipe++;
          }

          if (properties & BLERead) {
            remotePipeInfo->rxReqPipe = pipe;

            pipe++;
          }

          if (properties & BLENotify) {
            remotePipeInfo->rxPipe = pipe;

            pipe++;
          }

          if (properties & BLEIndicate) {
            remotePipeInfo->rxAckPipe = pipe;

            pipe++;
          }

          numRemoteCharacteristics++;

          numRemotePiped++;
        } else {
          break;
        }
      }

      setupMsgData->length = 8;

      if (uuidLength == 2) {
        setupMsgData->data[0]  = uuidData[1];
        setupMsgData->data[1]  = uuidData[0];
        setupMsgData->data[2]  = 0x01;
      } else {
        setupMsgData->data[0]  = uuidData[13];
        setupMsgData->data[1]  = uuidData[12];
        setupMsgData->data[2]  = customUuidIndex;

        customUuidIndex++;
      }

      setupMsgData->data[4]    = numRemoteCharacteristics;

      this->sendSetupMessage(&setupMsg, 0x3, remoteServiceSetupMsgOffset);
    }
  }

  this->_numRemotePipeInfo = numRemotePiped;

  // pipes
  unsigned short pipeSetupMsgOffset  = 0;

  for (int i = 0; i < numLocalPiped; i++) {
    struct localPipeInfo localPipeInfo = this->_localPipeInfo[i];

    setupMsgData->length   = 13;

    setupMsgData->data[0]  = 0x00;
    setupMsgData->data[1]  = 0x00;

    setupMsgData->data[2]  = 0x01;

    setupMsgData->data[3]  = 0x00;
    setupMsgData->data[4]  = 0x00;

    setupMsgData->data[5]  = 0x04;

    setupMsgData->data[6]  = (localPipeInfo.valueHandle >> 8) & 0xff;
    setupMsgData->data[7]  = localPipeInfo.valueHandle & 0xff;

    setupMsgData->data[8]  = (localPipeInfo.configHandle >> 8) & 0xff;
    setupMsgData->data[9]  = localPipeInfo.configHandle & 0xff;

    unsigned char properties = localPipeInfo.characteristic->properties();

    if (properties & BLEBroadcast) {
      setupMsgData->data[4] |= 0x01; // Adv
    }

    if (properties & BLENotify) {
      setupMsgData->data[4] |= 0x02; // TX
    }

    if (properties & BLEIndicate) {
      setupMsgData->data[4] |= 0x04; // TX Ack
    }

    if (properties & BLEWriteWithoutResponse) {
      setupMsgData->data[4] |= 0x08; // RX Ack
    }

    if (properties & BLEWrite) {
      setupMsgData->data[4] |= 0x10; // RX Ack
    }

    if (properties & BLERead) {
      setupMsgData->data[4] |= 0x80; // Set
    }

    this->sendSetupMessage(&setupMsg, 0x4, pipeSetupMsgOffset);
  }

  for (int i = 0; i < numRemotePiped; i++) {
    struct remotePipeInfo remotePipeInfo = this->_remotePipeInfo[i];

    BLERemoteCharacteristic *remoteCharacteristic = remotePipeInfo.characteristic;
    BLEUuid uuid = BLEUuid(remoteCharacteristic->uuid());
    const unsigned char* uuidData = uuid.data();
    unsigned char uuidLength = uuid.length();

    setupMsgData->length   = 13;

    if (uuidLength== 2) {
      setupMsgData->data[0]  = uuidData[1];
      setupMsgData->data[1]  = uuidData[0];
      setupMsgData->data[2]  = 0x01;
    } else {
      setupMsgData->data[0]  = uuidData[13];
      setupMsgData->data[1]  = uuidData[12];
      setupMsgData->data[2]  = customUuidIndex;

      customUuidIndex++;
    }

    setupMsgData->data[3]  = 0x00; // pipe properties
    setupMsgData->data[4]  = 0x00; // pipe properties

    setupMsgData->data[5]  = 0x04;

    setupMsgData->data[6]  = 0x00;
    setupMsgData->data[7]  = 0x00;

    setupMsgData->data[8]  = 0x00;
    setupMsgData->data[9]  = 0x00;

    unsigned char properties = remoteCharacteristic->properties();

    if (properties & BLERead) {
      setupMsgData->data[4] |= 0x40; // ACI_RX_REQ
    }

    if (properties & BLEWriteWithoutResponse) {
      setupMsgData->data[4] |= 0x02; // ACI_TX
    }

    if (properties & BLEWrite) {
      setupMsgData->data[4] |= 0x04; // ACI_TX_ACK
    }

    if (properties & BLENotify) {
      setupMsgData->data[4] |= 0x08; // ACI_RX
    }

    if (properties & BLEIndicate) {
      setupMsgData->data[4] |= 0x10; // ACI_RX_ACK
    }

    this->sendSetupMessage(&setupMsg, 0x4, pipeSetupMsgOffset);
  }

  // custom uuid's (remote only for now)
  unsigned short customUuidSetupMsgOffset = 0;

  for (int i = 0; i < numRemoteAttributes; i++) {
    BLERemoteAttribute* remoteAttribute = remoteAttributes[i];
    BLEUuid uuid = BLEUuid(remoteAttribute->uuid());
    const unsigned char* uuidData = uuid.data();
    unsigned char uuidLength = uuid.length();

    setupMsgData->length = 3 + uuidLength;
    memcpy(&setupMsgData->data, uuidData, uuidLength);

    setupMsgData->data[12]  = 0;
    setupMsgData->data[13]  = 0;

    this->sendSetupMessage(&setupMsg, 0x5, customUuidSetupMsgOffset);
  }

  // crc
  unsigned short crcOffset = 0;
  setupMsgData->length   = 6;
  setupMsgData->data[0]  = 3;

  this->sendSetupMessage(&setupMsg, 0xf, crcOffset, true);
}

void nRF8001::poll() {
  // We enter the if statement only when there is a ACI event available to be processed
  if (lib_aci_event_get(&this->_aciState, &this->_aciData)) {
    aci_evt_t* aciEvt = &this->_aciData.evt;

    switch(aciEvt->evt_opcode) {
      /**
      As soon as you reset the nRF8001 you will get an ACI Device Started Event
      */
      case ACI_EVT_DEVICE_STARTED: {
        this->_aciState.data_credit_total = aciEvt->params.device_started.credit_available;
        switch(aciEvt->params.device_started.device_mode) {
          case ACI_DEVICE_SETUP:
            /**
            When the device is in the setup mode
            */
#ifdef NRF_8001_DEBUG
            Serial.println(F("Evt Device Started: Setup"));
#endif
            break;

          case ACI_DEVICE_STANDBY:
#ifdef NRF_8001_DEBUG
            Serial.println(F("Evt Device Started: Standby"));
#endif
            //Looking for an iPhone by sending radio advertisements
            //When an iPhone connects to us we will get an ACI_EVT_CONNECTED event from the nRF8001
            if (aciEvt->params.device_started.hw_error) {
              delay(20); //Handle the HW error event correctly.
            } else if (this->_bondStore && this->_bondStore->hasData()) {
              this->_dynamicDataSequenceNo = 1;
              this->_dynamicDataOffset = 0;

              unsigned char chunkSize;
              this->_bondStore->getData(&chunkSize, this->_dynamicDataOffset, sizeof(chunkSize));
              this->_dynamicDataOffset++;

              unsigned char chunkData[DYNAMIC_DATA_MAX_CHUNK_SIZE];
              this->_bondStore->getData(chunkData, this->_dynamicDataOffset, chunkSize);
              this->_dynamicDataOffset += chunkSize;

              lib_aci_write_dynamic_data(this->_dynamicDataSequenceNo, chunkData, chunkSize);
            } else {
              this->startAdvertising();
            }
            break;

          default:
            break;
        }
      }
      break; //ACI Device Started Event

      case ACI_EVT_CMD_RSP:
        //If an ACI command response event comes with an error -> stop
        if (ACI_STATUS_SUCCESS != aciEvt->params.cmd_rsp.cmd_status &&
            ACI_STATUS_TRANSACTION_CONTINUE != aciEvt->params.cmd_rsp.cmd_status &&
            ACI_STATUS_TRANSACTION_COMPLETE != aciEvt->params.cmd_rsp.cmd_status) {
          //ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
          //TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
          //all other ACI commands will have status code of ACI_STATUS_SCUCCESS for a successful command
#ifdef NRF_8001_DEBUG
          Serial.print(F("ACI Command "));
          Serial.println(aciEvt->params.cmd_rsp.cmd_opcode, HEX);
          Serial.print(F("Evt Cmd respone: Status "));
          Serial.println(aciEvt->params.cmd_rsp.cmd_status, HEX);
#endif
        } else {
          switch (aciEvt->params.cmd_rsp.cmd_opcode) {
            case ACI_CMD_READ_DYNAMIC_DATA: {
#ifdef NRF_8001_DEBUG
              Serial.print(F("Dynamic data read sequence "));
              Serial.print(aciEvt->params.cmd_rsp.params.read_dynamic_data.seq_no);
              Serial.print(F(": "));
              Serial.print(aciEvt->len - 4);
              Serial.print(F(": "));

              BLEUtil::printBuffer(aciEvt->params.cmd_rsp.params.read_dynamic_data.dynamic_data, aciEvt->len - 4);
#endif
              if (aciEvt->params.cmd_rsp.params.read_dynamic_data.seq_no == 1) {
                this->_dynamicDataOffset = 0;
              }

              unsigned char chunkLength = aciEvt->len - 4;

              this->_bondStore->putData(&chunkLength, this->_dynamicDataOffset, sizeof(chunkLength));
              this->_dynamicDataOffset++;

              this->_bondStore->putData(aciEvt->params.cmd_rsp.params.read_dynamic_data.dynamic_data, this->_dynamicDataOffset, chunkLength);
              this->_dynamicDataOffset += chunkLength;

              if (aciEvt->params.cmd_rsp.cmd_status == ACI_STATUS_TRANSACTION_CONTINUE) {
                lib_aci_read_dynamic_data();
              } else if (aciEvt->params.cmd_rsp.cmd_status == ACI_STATUS_TRANSACTION_COMPLETE) {
                this->startAdvertising();
              }
              break;
            }

            case ACI_CMD_WRITE_DYNAMIC_DATA: {
#ifdef NRF_8001_DEBUG
              Serial.print(F("Dynamic data write sequence "));
              Serial.print(this->_dynamicDataSequenceNo);
              Serial.println(F(" complete"));
#endif
              if (aciEvt->params.cmd_rsp.cmd_status == ACI_STATUS_TRANSACTION_CONTINUE) {
                this->_dynamicDataSequenceNo++;

                unsigned char chunkSize;
                this->_bondStore->getData(&chunkSize, this->_dynamicDataOffset, sizeof(chunkSize));
                this->_dynamicDataOffset++;

                unsigned char chunkData[DYNAMIC_DATA_MAX_CHUNK_SIZE];
                this->_bondStore->getData(chunkData, this->_dynamicDataOffset, chunkSize);
                this->_dynamicDataOffset += chunkSize;

                lib_aci_write_dynamic_data(this->_dynamicDataSequenceNo, chunkData, chunkSize);
              } else if (aciEvt->params.cmd_rsp.cmd_status == ACI_STATUS_TRANSACTION_COMPLETE) {
                delay(20);
                this->startAdvertising();
              }
              break;
            }

            case ACI_CMD_GET_DEVICE_VERSION:
              break;

            case ACI_CMD_GET_DEVICE_ADDRESS: {
#ifdef NRF_8001_DEBUG
              char address[18];

              BLEUtil::addressToString(aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own, address);

              Serial.print(F("Device address = "));
              Serial.println(address);

              Serial.print(F("Device address type = "));
              Serial.println(aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_type, DEC);
#endif
              if (this->_eventListener) {
                this->_eventListener->BLEDeviceAddressReceived(*this, aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own);
              }
              break;
            }

            case ACI_CMD_GET_BATTERY_LEVEL: {
              float batteryLevel = aciEvt->params.cmd_rsp.params.get_battery_level.battery_level * 0.00352;
#ifdef NRF_8001_DEBUG
              Serial.print(F("Battery level = "));
              Serial.println(batteryLevel);
#endif
              if (this->_eventListener) {
                this->_eventListener->BLEDeviceBatteryLevelReceived(*this, batteryLevel);
              }
              break;
            }

            case ACI_CMD_GET_TEMPERATURE: {
              float temperature = aciEvt->params.cmd_rsp.params.get_temperature.temperature_value / 4.0;
#ifdef NRF_8001_DEBUG
              Serial.print(F("Temperature = "));
              Serial.println(temperature);
#endif
              if (this->_eventListener) {
                this->_eventListener->BLEDeviceTemperatureReceived(*this, temperature);
              }
              break;
            }

            default:
              break;
          }
        }
        break;

      case ACI_EVT_CONNECTED:
#ifdef NRF_8001_DEBUG
        char address[18];
        BLEUtil::addressToString(aciEvt->params.connected.dev_addr, address);

        Serial.print(F("Evt Connected "));
        Serial.println(address);
#endif
        this->_timingChanged = false;
        this->_closedPipesCleared = false;
        this->_remoteServicesDiscovered = false;

        if (this->_eventListener) {
          this->_eventListener->BLEDeviceConnected(*this, aciEvt->params.connected.dev_addr);
        }

        this->_aciState.data_credit_available = this->_aciState.data_credit_total;
        break;

      case ACI_EVT_PIPE_STATUS: {
#ifdef NRF_8001_DEBUG
        Serial.println(F("Evt Pipe Status "));
#endif
        uint64_t openPipes;
        uint64_t closedPipes;

        memcpy(&openPipes, aciEvt->params.pipe_status.pipes_open_bitmap, sizeof(openPipes));
        memcpy(&closedPipes, aciEvt->params.pipe_status.pipes_closed_bitmap, sizeof(closedPipes));

#ifdef NRF_8001_DEBUG
        Serial.println((unsigned long)openPipes, HEX);
        Serial.println((unsigned long)closedPipes, HEX);
#endif
        if (this->_minimumConnectionInterval >= ACI_PPCP_MIN_CONN_INTVL_MIN &&
            this->_maximumConnectionInterval <= ACI_PPCP_MAX_CONN_INTVL_MAX &&
            openPipes & 0x03 &&
            !this->_timingChanged) {
          this->_timingChanged = true;
          lib_aci_change_timing(this->_minimumConnectionInterval, this->_maximumConnectionInterval, 0, 4000 / 10);
        }

        bool discoveryFinished = lib_aci_is_discovery_finished(&this->_aciState);
        if (closedPipes == 0 && !discoveryFinished) {
          this->_closedPipesCleared = true;
        }

        for (int i = 0; i < this->_numLocalPipeInfo; i++) {
          struct localPipeInfo* localPipeInfo = &this->_localPipeInfo[i];

          if (localPipeInfo->txPipe) {
            localPipeInfo->txPipeOpen = lib_aci_is_pipe_available(&this->_aciState, localPipeInfo->txPipe);
          }

          if (localPipeInfo->txAckPipe) {
            localPipeInfo->txAckPipeOpen = lib_aci_is_pipe_available(&this->_aciState, localPipeInfo->txAckPipe);
          }

          bool subscribed = (localPipeInfo->txPipeOpen || localPipeInfo->txAckPipeOpen);

          if (localPipeInfo->characteristic->subscribed() != subscribed) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceCharacteristicSubscribedChanged(*this, *localPipeInfo->characteristic, subscribed);
            }
          }
        }

        if (this->_closedPipesCleared && discoveryFinished && !this->_remoteServicesDiscovered) {
          if (!this->_remoteServicesDiscovered && this->_eventListener) {
            this->_remoteServicesDiscovered = true;

            this->_eventListener->BLEDeviceRemoteServicesDiscovered(*this);
          }
        }
        break;
      }

      case ACI_EVT_TIMING:
#ifdef NRF_8001_DEBUG
        Serial.print(F("Timing change received conn Interval: 0x"));
        Serial.println(aciEvt->params.timing.conn_rf_interval, HEX);
#endif
        break;

      case ACI_EVT_DISCONNECTED:
#ifdef NRF_8001_DEBUG
        Serial.println(F("Evt Disconnected/Advertising timed out"));
#endif
        // all characteristics unsubscribed on disconnect
        for (int i = 0; i < this->_numLocalPipeInfo; i++) {
          struct localPipeInfo* localPipeInfo = &this->_localPipeInfo[i];

          if (localPipeInfo->characteristic->subscribed()) {
            if (this->_eventListener) {
              this->_eventListener->BLEDeviceCharacteristicSubscribedChanged(*this, *localPipeInfo->characteristic, false);
            }
          }
        }

        if (this->_eventListener) {
          this->_eventListener->BLEDeviceDisconnected(*this);
        }

        if (this->_storeDynamicData) {
          lib_aci_read_dynamic_data();

          this->_storeDynamicData = false;
        } else {
          this->startAdvertising();
        }
        break;

      case ACI_EVT_BOND_STATUS:
#ifdef NRF_8001_DEBUG
        Serial.println(F("Evt Bond Status"));
        Serial.println(aciEvt->params.bond_status.status_code);
#endif
        this->_storeDynamicData = (aciEvt->params.bond_status.status_code == ACI_BOND_STATUS_SUCCESS) &&
                                    (this->_aciState.bonded != ACI_BOND_STATUS_SUCCESS);

        this->_aciState.bonded = aciEvt->params.bond_status.status_code;

        this->_remoteServicesDiscovered = false;

        if (aciEvt->params.bond_status.status_code == ACI_BOND_STATUS_SUCCESS && this->_eventListener) {
          this->_eventListener->BLEDeviceBonded(*this);
        }
        break;

      case ACI_EVT_DATA_RECEIVED: {
        uint8_t dataLen = aciEvt->len - 2;
        uint8_t pipe = aciEvt->params.data_received.rx_data.pipe_number;
#ifdef NRF_8001_DEBUG
        Serial.print(F("Data Received, pipe = "));
        Serial.println(aciEvt->params.data_received.rx_data.pipe_number, DEC);

        BLEUtil::printBuffer(aciEvt->params.data_received.rx_data.aci_data, dataLen);
#endif

        for (int i = 0; i < this->_numLocalPipeInfo; i++) {
          struct localPipeInfo* localPipeInfo = &this->_localPipeInfo[i];

          if (localPipeInfo->rxAckPipe == pipe || localPipeInfo->rxPipe == pipe) {
            if (localPipeInfo->rxAckPipe == pipe) {
              lib_aci_send_ack(&this->_aciState, localPipeInfo->rxAckPipe);
            }

            if (this->_eventListener) {
              this->_eventListener->BLEDeviceCharacteristicValueChanged(*this, *localPipeInfo->characteristic, aciEvt->params.data_received.rx_data.aci_data, dataLen);
            }
            break;
          }
        }

        for (int i = 0; i < this->_numRemotePipeInfo; i++) {
          struct remotePipeInfo* remotePipeInfo = &this->_remotePipeInfo[i];

          if (remotePipeInfo->rxPipe == pipe || remotePipeInfo->rxAckPipe == pipe || remotePipeInfo->rxReqPipe) {
            if (remotePipeInfo->rxAckPipe == pipe) {
             lib_aci_send_ack(&this->_aciState, remotePipeInfo->rxAckPipe);
            }

            if (this->_eventListener) {
              this->_eventListener->BLEDeviceRemoteCharacteristicValueChanged(*this, *remotePipeInfo->characteristic, aciEvt->params.data_received.rx_data.aci_data, dataLen);
            }
            break;
          }
        }
        break;
      }

      case ACI_EVT_DATA_CREDIT:
        this->_aciState.data_credit_available = this->_aciState.data_credit_available + aciEvt->params.data_credit.credit;
        break;

      case ACI_EVT_PIPE_ERROR:
        //See the appendix in the nRF8001 Product Specication for details on the error codes
#ifdef NRF_8001_DEBUG
        Serial.print(F("ACI Evt Pipe Error: Pipe #:"));
        Serial.print(aciEvt->params.pipe_error.pipe_number, DEC);
        Serial.print(F("  Pipe Error Code: 0x"));
        Serial.println(aciEvt->params.pipe_error.error_code, HEX);
#endif

        //Increment the credit available as the data packet was not sent.
        //The pipe error also represents the Attribute protocol Error Response sent from the peer and that should not be counted
        //for the credit.
        if (ACI_STATUS_ERROR_PEER_ATT_ERROR != aciEvt->params.pipe_error.error_code) {
          this->_aciState.data_credit_available++;
        } else if (this->_bondStore) {
          lib_aci_bond_request();
        }
        break;

      case ACI_EVT_HW_ERROR:
#ifdef NRF_8001_DEBUG
        Serial.print(F("HW error: "));
        Serial.println(aciEvt->params.hw_error.line_num, DEC);

        for(uint8_t counter = 0; counter <= (aciEvt->len - 3); counter++) {
          Serial.write(aciEvt->params.hw_error.file_name[counter]); //uint8_t file_name[20];
        }
        Serial.println();
#endif
        this->startAdvertising();
        break;

      default:
        break;
    }
  } else {
    //Serial.println(F("No ACI Events available"));
    // No event in the ACI Event queue and if there is no event in the ACI command queue the arduino can go to sleep
    // Arduino can go to sleep now
    // Wakeup from sleep from the RDYN line
  }
}

void nRF8001::end() {
  lib_aci_pin_reset();

  if (this->_localPipeInfo) {
    free(this->_localPipeInfo);
  }

  if (this->_remotePipeInfo) {
    free(this->_remotePipeInfo);
  }

  this->_numLocalPipeInfo = 0;
  this->_numRemotePipeInfo = 0;
}

bool nRF8001::updateCharacteristicValue(BLECharacteristic& characteristic) {
  bool success = true;

  struct localPipeInfo* localPipeInfo = this->localPipeInfoForCharacteristic(characteristic);

  if (localPipeInfo) {
    if (localPipeInfo->advPipe && (this->_broadcastPipe == localPipeInfo->advPipe)) {
      success &= lib_aci_set_local_data(&this->_aciState, localPipeInfo->advPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());
    }

    if (localPipeInfo->setPipe) {
      success &= lib_aci_set_local_data(&this->_aciState, localPipeInfo->setPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());
    }

    if (localPipeInfo->txPipe && localPipeInfo->txPipeOpen) {
      if (this->canNotifyCharacteristic(characteristic)) {
        this->_aciState.data_credit_available--;
        success &= lib_aci_send_data(localPipeInfo->txPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());
      } else {
        success = false;
      }
    }

    if (localPipeInfo->txAckPipe && localPipeInfo->txAckPipeOpen) {
      if (this->canIndicateCharacteristic(characteristic)) {
        this->_aciState.data_credit_available--;
        success &= lib_aci_send_data(localPipeInfo->txAckPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());
      } else {
        success = false;
      }
    }
  }

  return success;
}

bool nRF8001::broadcastCharacteristic(BLECharacteristic& characteristic) {
  bool success = false;

  struct localPipeInfo* localPipeInfo = this->localPipeInfoForCharacteristic(characteristic);

  if (localPipeInfo) {
    if (localPipeInfo->advPipe) {
      uint64_t advPipes = ((uint64_t)1) << (localPipeInfo->advPipe);

      success = lib_aci_open_adv_pipes((uint8_t*)&advPipes) &&
                  lib_aci_set_local_data(&this->_aciState, localPipeInfo->advPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());

      if (success) {
        this->_broadcastPipe = localPipeInfo->advPipe;
      }
    }
  }

  return success;
}

bool nRF8001::canNotifyCharacteristic(BLECharacteristic& /*characteristic*/) {
  return (this->_aciState.data_credit_available > 0);
}

bool nRF8001::canIndicateCharacteristic(BLECharacteristic& /*characteristic*/) {
  return (this->_aciState.data_credit_available > 0);
}

bool nRF8001::canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  struct remotePipeInfo* remotePipeInfo = this->remotePipeInfoForCharacteristic(characteristic);

  if (remotePipeInfo) {
    unsigned char rxReqPipe = remotePipeInfo->rxReqPipe;

    if (rxReqPipe) {
      success = lib_aci_is_pipe_available(&this->_aciState, rxReqPipe);
    }
  }

  return success;
}

bool nRF8001::readRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  struct remotePipeInfo* remotePipeInfo = this->remotePipeInfoForCharacteristic(characteristic);

  if (remotePipeInfo) {
    unsigned char rxReqPipe = remotePipeInfo->rxReqPipe;

    if (rxReqPipe) {
      success = lib_aci_is_pipe_available(&this->_aciState, rxReqPipe) &&
                lib_aci_request_data(&this->_aciState, rxReqPipe);
    }
  }

  return success;
}

bool nRF8001::canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  struct remotePipeInfo* remotePipeInfo = this->remotePipeInfoForCharacteristic(characteristic);

  if (remotePipeInfo) {
    unsigned char pipe = remotePipeInfo->txAckPipe ? remotePipeInfo->txAckPipe : remotePipeInfo->txPipe;

    if (pipe) {
      success = lib_aci_is_pipe_available(&this->_aciState, pipe);
    }
  }

  return success;
}

bool nRF8001::writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length) {
  bool success = false;

  struct remotePipeInfo* remotePipeInfo = this->remotePipeInfoForCharacteristic(characteristic);

  if (remotePipeInfo) {
    unsigned char pipe = remotePipeInfo->txAckPipe ? remotePipeInfo->txAckPipe : remotePipeInfo->txPipe;

    if (pipe) {
      success = lib_aci_is_pipe_available(&this->_aciState, pipe) &&
                lib_aci_send_data(pipe, (uint8_t*)value, length);
    }
  }

  return success;
}

bool nRF8001::canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  struct remotePipeInfo* remotePipeInfo = this->remotePipeInfoForCharacteristic(characteristic);

  if (remotePipeInfo) {
    if (remotePipeInfo->characteristic == &characteristic) {
      unsigned char pipe = remotePipeInfo->rxPipe ? remotePipeInfo->rxPipe : remotePipeInfo->rxAckPipe;

      if (pipe) {
        success = lib_aci_is_pipe_closed(&this->_aciState, pipe);
      }
    }
  }

  return success;
}

bool nRF8001::subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  struct remotePipeInfo* remotePipeInfo = this->remotePipeInfoForCharacteristic(characteristic);

  if (remotePipeInfo) {
    unsigned char pipe = remotePipeInfo->rxPipe ? remotePipeInfo->rxPipe : remotePipeInfo->rxAckPipe;

    if (pipe) {
      success = lib_aci_is_pipe_closed(&this->_aciState, pipe) && lib_aci_open_remote_pipe(&this->_aciState, pipe);
    }
  }

  return success;
}

bool nRF8001::canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  return this->canSubscribeRemoteCharacteristic(characteristic);
}

bool nRF8001::unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic) {
  bool success = false;

  struct remotePipeInfo* remotePipeInfo = this->remotePipeInfoForCharacteristic(characteristic);

  if (remotePipeInfo) {
    unsigned char pipe = remotePipeInfo->rxPipe ? remotePipeInfo->rxPipe : remotePipeInfo->rxAckPipe;

    if (pipe) {
      success = lib_aci_close_remote_pipe(&this->_aciState, pipe);
    }
  }

  return success;
}

bool nRF8001::setTxPower(int txPower) {
  aci_device_output_power_t outputPower;

  if (txPower <= -18) {
    outputPower = ACI_DEVICE_OUTPUT_POWER_MINUS_18DBM;
  } else if (txPower <= -12) {
    outputPower = ACI_DEVICE_OUTPUT_POWER_MINUS_12DBM;
  } else if (txPower <= -6) {
    outputPower = ACI_DEVICE_OUTPUT_POWER_MINUS_6DBM;
  } else {
    outputPower = ACI_DEVICE_OUTPUT_POWER_0DBM;
  }

  return lib_aci_set_tx_power(outputPower);
}

void nRF8001::startAdvertising() {
  uint16_t advertisingInterval = (this->_advertisingInterval * 16) / 10;

  if (this->_connectable) {
    if (this->_bondStore == NULL || this->_bondStore->hasData())   {
      lib_aci_connect(0/* in seconds, 0 means forever */, advertisingInterval);
    } else {
      lib_aci_bond(180/* in seconds, 0 means forever */, advertisingInterval);
    }
  } else {
    lib_aci_broadcast(0/* in seconds, 0 means forever */, advertisingInterval);
  }

#ifdef NRF_8001_DEBUG
  Serial.println(F("Advertising started."));
#endif
}

void nRF8001::disconnect() {
  lib_aci_disconnect(&this->_aciState, ACI_REASON_TERMINATE);
}

void nRF8001::requestAddress() {
  lib_aci_get_address();
}

void nRF8001::requestTemperature() {
  lib_aci_get_temperature();
}

void nRF8001::requestBatteryLevel() {
  lib_aci_get_battery_level();
}

void nRF8001::waitForSetupMode()
{
  bool setupMode = false;

  while (!setupMode) {
    if (lib_aci_event_get(&this->_aciState, &this->_aciData)) {
      aci_evt_t* aciEvt = &this->_aciData.evt;

      switch(aciEvt->evt_opcode) {
        case ACI_EVT_DEVICE_STARTED: {
          switch(aciEvt->params.device_started.device_mode) {
            case ACI_DEVICE_SETUP:
              /**
              When the device is in the setup mode
              */
#ifdef NRF_8001_DEBUG
              Serial.println(F("Evt Device Started: Setup"));
#endif
              setupMode = true;
              break;

            default:
              break;
          }
        }

        case ACI_EVT_CMD_RSP:
          setupMode = true;
          break;

        default:
          break;
      }
    } else {
      delay(1);
    }
  }
}

void nRF8001::sendSetupMessage(hal_aci_data_t* data, bool withCrc)
{
  this->_crcSeed = crc_16_ccitt(this->_crcSeed, data->buffer, data->buffer[0] + (withCrc ? -1 : 1));

  if (withCrc) {
    data->buffer[5] = (this->_crcSeed >> 8) & 0xff;
    data->buffer[6] = this->_crcSeed & 0xff;
  }

#ifdef NRF_8001_DEBUG
  BLEUtil::printBuffer(data->buffer, data->buffer[0] + 1);
#endif

  hal_aci_tl_send(data);

  bool setupMsgSent = false;

  while (!setupMsgSent) {
    if (lib_aci_event_get(&this->_aciState, &this->_aciData)) {
      aci_evt_t* aciEvt = &this->_aciData.evt;

      switch(aciEvt->evt_opcode) {
        case ACI_EVT_CMD_RSP: {
          switch(aciEvt->params.cmd_rsp.cmd_status) {
            case ACI_STATUS_TRANSACTION_CONTINUE:
#ifdef NRF_8001_DEBUG
              Serial.println(F("Evt Cmd Rsp: Transaction Continue"));
#endif
              setupMsgSent = true;
              break;

            case ACI_STATUS_TRANSACTION_COMPLETE:
#ifdef NRF_8001_DEBUG
              Serial.println(F("Evt Cmd Rsp: Transaction Complete"));
#endif
              setupMsgSent = true;
              break;

            default:
              break;
          }
        }

        default:
          break;
      }
    } else {
      delay(1);
    }
  }
}

void nRF8001::sendSetupMessage(hal_aci_data_t* setupMsg, unsigned char type, unsigned short& offset, bool withCrc) {
  struct setupMsgData* setupMsgData = (struct setupMsgData*)(setupMsg->buffer);

  setupMsgData->cmd      = ACI_CMD_SETUP;
  setupMsgData->type     = (type << 4) | ((offset >> 8) & 0x0f);
  setupMsgData->offset   = (offset & 0xff);

  this->sendSetupMessage(setupMsg, withCrc);

  offset += (setupMsgData->length - 3);
}

struct nRF8001::localPipeInfo* nRF8001::localPipeInfoForCharacteristic(BLECharacteristic& characteristic) {
  struct localPipeInfo* result = NULL;

  for (int i = 0; i < this->_numLocalPipeInfo; i++) {
    struct localPipeInfo* localPipeInfo = &this->_localPipeInfo[i];

    if (localPipeInfo->characteristic == &characteristic) {
      result = localPipeInfo;
      break;
    }
  }

  return result;
}

struct nRF8001::remotePipeInfo* nRF8001::remotePipeInfoForCharacteristic(BLERemoteCharacteristic& characteristic) {
  struct remotePipeInfo* result = NULL;

  for (int i = 0; i < this->_numRemotePipeInfo; i++) {
    struct remotePipeInfo* remotePipeInfo = &this->_remotePipeInfo[i];

    if (remotePipeInfo->characteristic == &characteristic) {
      result = remotePipeInfo;
      break;
    }
  }

  return result;
}

#endif
