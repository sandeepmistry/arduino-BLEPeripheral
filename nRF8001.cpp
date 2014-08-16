#include <SPI.h>

#include "BLEAttribute.h"
#include "BLEService.h"
#include "BLECharacteristic.h"
#include "BLEDescriptor.h"
#include "BLEUuid.h"

#include "nRF8001.h"

//#define NRF_8001_DEBUG

#define ADVERTISING_INTERVAL 0x050

struct setupMsgData {
  unsigned char length;
  unsigned char cmd;
  unsigned char type;
  unsigned char offset;
  unsigned char data[28];
};

#define NB_BASE_SETUP_MESSAGES 7

/* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
static hal_aci_data_t baseSetupMsgs[NB_BASE_SETUP_MESSAGES] PROGMEM = {\
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
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x90,0x00,0xff,\
    },\
  },\
  {0x00,\
    {\
      0x1f,0x06,0x10,0x38,0xff,0xff,0x02,0x58,0x0a,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x00,0x00,\
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

void __ble_assert(const char *file, uint16_t line)
{
  Serial.print(F("ERROR "));
  Serial.print(file);
  Serial.print(F(": "));
  Serial.print(line);
  Serial.print(F("\n"));
  while(1);
}

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
  _setupRequired(false),
  _isSetup(false),

  _pipeInfo(NULL),
  _numPipeInfo(0),

  _eventListener(NULL)
{
  this->_aciState.aci_pins.reqn_pin               = req;
  this->_aciState.aci_pins.rdyn_pin               = rdy;
  this->_aciState.aci_pins.mosi_pin               = MOSI;
  this->_aciState.aci_pins.miso_pin               = MISO;
  this->_aciState.aci_pins.sck_pin                = SCK;

  if (rst == UNUSED) {
    this->_aciState.aci_pins.board_name           = REDBEARLAB_SHIELD_V1_1;
  } else {
    this->_aciState.aci_pins.board_name           = BOARD_DEFAULT;
  }

  this->_aciState.aci_pins.spi_clock_divider      = SPI_CLOCK_DIV8;

  this->_aciState.aci_pins.reset_pin              = rst;
  this->_aciState.aci_pins.active_pin             = UNUSED;
  this->_aciState.aci_pins.optional_chip_sel_pin  = UNUSED;

  this->_aciState.aci_pins.interface_is_interrupt = false;
  this->_aciState.aci_pins.interrupt_number       = 1;
}

nRF8001::~nRF8001() {
  if (this->_pipeInfo) {
    free(this->_pipeInfo);
  }

  if (this->_aciState.aci_setup_info.setup_msgs) {
    for (int i = 0; i < this->_aciState.aci_setup_info.num_setup_msgs; i++) {
      if (this->_aciState.aci_setup_info.setup_msgs[i]) {
        free(this->_aciState.aci_setup_info.setup_msgs[i]);
      }
    }

    free(this->_aciState.aci_setup_info.setup_msgs);
  }
}



void nRF8001::setEventListener(nRF8001EventListener* eventListener) {
  this->_eventListener = eventListener;
}

void nRF8001::begin(const unsigned char* advertisementData,
                      unsigned char advertisementDataLength,
                      const unsigned char* scanData,
                      unsigned char scanDataLength,
                      BLEAttribute** attributes,
                      unsigned char numAttributes)
{
  hal_aci_data_t* setupMsg;
  struct setupMsgData* setupMsgData;
  int setupMsgIndex = 0;
  unsigned char numPipedCharacteristics = 0;

  this->_aciState.aci_setup_info.num_setup_msgs = NB_BASE_SETUP_MESSAGES + 2; // GATT terminator + CRC

  for (int i = 0; i < numAttributes; i++) {
    BLEAttribute* attribute = attributes[i];

    if (attribute->type() == BLETypeService) {
      this->_aciState.aci_setup_info.num_setup_msgs++;
    } else if (attribute->type() == BLETypeCharacteristic) {
      BLECharacteristic* characteristic = (BLECharacteristic *)attribute;

      this->_aciState.aci_setup_info.num_setup_msgs += 2;

      if (characteristic->properties()) {
        this->_aciState.aci_setup_info.num_setup_msgs++;
        numPipedCharacteristics++;
      }

      if (characteristic->properties() & (BLENotify | BLEIndicate)){
        this->_aciState.aci_setup_info.num_setup_msgs++;
      }
    } else if (attribute->type() == BLETypeDescriptor) {
      this->_aciState.aci_setup_info.num_setup_msgs++;
    }
  }

  this->_aciState.aci_setup_info.setup_msgs = (hal_aci_data_t**)malloc(sizeof(hal_aci_data_t*) * this->_aciState.aci_setup_info.num_setup_msgs);
  this->_pipeInfo = (struct pipeInfo*)malloc(sizeof(struct pipeInfo) * numPipedCharacteristics);


  for (int i = 0; i < NB_BASE_SETUP_MESSAGES; i++) {
    this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(pgm_read_byte_near(&baseSetupMsgs[i].buffer[0]) + 2);
    setupMsgIndex++;
    int setupMsgSize = 2 + setupMsg->buffer[0];

    memcpy_P(setupMsg, &baseSetupMsgs[i], setupMsgSize);
  }

  if (advertisementData && advertisementDataLength) {
    setupMsg = this->_aciState.aci_setup_info.setup_msgs[2];
    setupMsgData = (struct setupMsgData*)setupMsg->buffer;

    setupMsgData->data[22] = 0x40;

    setupMsg = this->_aciState.aci_setup_info.setup_msgs[5];
    setupMsgData = (struct setupMsgData*)setupMsg->buffer;

    memcpy(setupMsgData->data, advertisementData, advertisementDataLength);
  }

  if (scanData && scanDataLength) {
    setupMsg = this->_aciState.aci_setup_info.setup_msgs[3];
    setupMsgData = (struct setupMsgData*)setupMsg->buffer;

    setupMsgData->data[12] = 0x40;

    setupMsg = this->_aciState.aci_setup_info.setup_msgs[6];
    setupMsgData = (struct setupMsgData*)setupMsg->buffer;

    memcpy(setupMsgData->data, scanData, scanDataLength);
  }

  // GATT
  unsigned char  gattSetupMsgOffset = 0;
  unsigned short handle             = 1;
  unsigned char  pipe               = 1;
  unsigned char  numPiped           = 0;

  for (int i = 0; i < numAttributes; i++) {
    BLEAttribute* attribute = attributes[i];
    BLEUuid uuid = BLEUuid(attribute->uuid());

    if (attribute->type() == BLETypeService) {
      BLEService* service = (BLEService *)attribute;

      this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(14 + uuid.length());
      setupMsgIndex++;
      setupMsgData = (struct setupMsgData*)setupMsg->buffer;

      setupMsg->status_byte = 0;
      setupMsgData->length  = 12 + uuid.length();
      setupMsgData->cmd     = ACI_CMD_SETUP;
      setupMsgData->type    = 0x20;
      setupMsgData->offset  = gattSetupMsgOffset;

      setupMsgData->data[0] = 0x04;
      setupMsgData->data[1] = 0x04;
      setupMsgData->data[2] = uuid.length();
      setupMsgData->data[3] = uuid.length();

      setupMsgData->data[4] = (handle >> 8) & 0xff;
      setupMsgData->data[5] = handle & 0xff;
      handle++;

      setupMsgData->data[6] = (service->type() >> 8) & 0xff;
      setupMsgData->data[7] = service->type() & 0xff;

      setupMsgData->data[8] = ACI_STORE_LOCAL;

      memcpy(&setupMsgData->data[9], uuid.data(), uuid.length());

      gattSetupMsgOffset += 9 + uuid.length();
    } else if (attribute->type() == BLETypeCharacteristic) {
      BLECharacteristic* characteristic = (BLECharacteristic *)attribute;

      struct pipeInfo* pipeInfo = &this->_pipeInfo[numPiped];

      memset(pipeInfo, 0, sizeof(struct pipeInfo));

      pipeInfo->characteristic = characteristic;

      if (characteristic->properties()) {
        numPiped++;

        pipeInfo->startPipe = pipe;

        if (characteristic->properties() & BLENotify) {
          pipeInfo->txPipe = pipe;

          pipe++;
        }

        if (characteristic->properties() & BLEIndicate) {
          pipeInfo->txAckPipe = pipe;

          pipe++;
        }

        if (characteristic->properties() & BLEWriteWithoutResponse) {
          pipeInfo->rxPipe = pipe;

          pipe++;
        }

        if (characteristic->properties() & BLEWrite) {
          pipeInfo->rxAckPipe = pipe;

          pipe++;
        }

        if (characteristic->properties() & BLERead) {
          pipeInfo->setPipe = pipe;

          pipe++;
        }
      }

      this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(17 + uuid.length());
      setupMsgIndex++;
      setupMsgData = (struct setupMsgData*)setupMsg->buffer;

      setupMsg->status_byte  = 0;
      setupMsgData->length   = 15 + uuid.length();
      setupMsgData->cmd      = ACI_CMD_SETUP;
      setupMsgData->type     = 0x20;
      setupMsgData->offset   = gattSetupMsgOffset;

      setupMsgData->data[0]  = 0x04;
      setupMsgData->data[1]  = 0x04;
      setupMsgData->data[2]  = 3 + uuid.length();
      setupMsgData->data[3]  = 3 + uuid.length();

      setupMsgData->data[4]  = (handle >> 8) & 0xff;
      setupMsgData->data[5]  = handle & 0xff;
      handle++;

      setupMsgData->data[6]  = (characteristic->type() >> 8) & 0xff;
      setupMsgData->data[7]  = characteristic->type() & 0xff;

      setupMsgData->data[8]  = ACI_STORE_LOCAL;
      setupMsgData->data[9]  = characteristic->properties();

      setupMsgData->data[10] = handle & 0xff;
      setupMsgData->data[11] = (handle >> 8) & 0xff;
      pipeInfo->valueHandle = handle;
      handle++;

      memcpy(&setupMsgData->data[12], uuid.data(), uuid.length());

      gattSetupMsgOffset += 12 + uuid.length();

      this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(14 + characteristic->valueSize());
      setupMsgIndex++;
      setupMsgData = (struct setupMsgData*)setupMsg->buffer;

      setupMsg->status_byte  = 0;
      setupMsgData->length   = 12 + characteristic->valueSize();
      setupMsgData->cmd      = ACI_CMD_SETUP;
      setupMsgData->type     = 0x20;
      setupMsgData->offset   = gattSetupMsgOffset;

      setupMsgData->data[0]  = 0x00;
      setupMsgData->data[1]  = 0x00;

      if (characteristic->properties() & BLERead) {
        setupMsgData->data[0] |= 0x06;
        setupMsgData->data[1] |= 0x04;
      }

      if (characteristic->properties() & (BLEWrite | BLEWriteWithoutResponse)) {
        setupMsgData->data[0] |= 0x46;
        setupMsgData->data[1] |= 0x10;
      }

      if (characteristic->properties() & BLENotify) {
        setupMsgData->data[0] |= 0x16;
      }

      if (characteristic->properties() & BLEIndicate) {
        setupMsgData->data[0] |= 0x26;
      }

      setupMsgData->data[2]  = characteristic->valueSize() + 1;
      setupMsgData->data[3]  = characteristic->valueLength();

      setupMsgData->data[4]  = (pipeInfo->valueHandle >> 8) & 0xff;
      setupMsgData->data[5]  = pipeInfo->valueHandle & 0xff;

      setupMsgData->data[6]  = 0x00;
      setupMsgData->data[7]  = 0x00;

      setupMsgData->data[8]  = pipeInfo->startPipe;

      memset(&setupMsgData->data[9], 0x00, characteristic->valueSize());
      memcpy(&setupMsgData->data[9], characteristic->value(), characteristic->valueLength());

      gattSetupMsgOffset += 9 + characteristic->valueSize();

      if (characteristic->properties() & (BLENotify | BLEIndicate)) {
        this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(16);
        setupMsgIndex++;
        setupMsgData = (struct setupMsgData*)setupMsg->buffer;

        setupMsg->status_byte  = 0;
        setupMsgData->length   = 14;
        setupMsgData->cmd      = ACI_CMD_SETUP;
        setupMsgData->type     = 0x20;
        setupMsgData->offset   = gattSetupMsgOffset;

        setupMsgData->data[0]  = 0x46;
        setupMsgData->data[1]  = 0x14;

        setupMsgData->data[2]  = 0x03;
        setupMsgData->data[3]  = 0x02;

        setupMsgData->data[4]  = (handle >> 8) & 0xff;
        setupMsgData->data[5]  = handle & 0xff;
        pipeInfo->configHandle = handle;
        handle++;

        setupMsgData->data[6]  = 0x29;
        setupMsgData->data[7]  = 0x02;

        setupMsgData->data[8]  = ACI_STORE_LOCAL;

        setupMsgData->data[9]  = 0x00;
        setupMsgData->data[10] = 0x00;

        gattSetupMsgOffset += 11;
      }
    } else if (attribute->type() == BLETypeDescriptor) {
      BLEDescriptor* descriptor = (BLEDescriptor *)attribute;

      this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(14 + descriptor->valueSize());
      setupMsgIndex++;
      setupMsgData = (struct setupMsgData*)setupMsg->buffer;

      setupMsg->status_byte  = 0;
      setupMsgData->length   = 12 + descriptor->valueSize();
      setupMsgData->cmd      = ACI_CMD_SETUP;
      setupMsgData->type     = 0x20;
      setupMsgData->offset   = gattSetupMsgOffset;

      setupMsgData->data[0]  = 0x04;
      setupMsgData->data[1]  = 0x04;

      setupMsgData->data[2]  = descriptor->valueSize();
      setupMsgData->data[3]  = descriptor->valueLength();

      setupMsgData->data[4]  = (handle >> 8) & 0xff;
      setupMsgData->data[5]  = handle & 0xff;
      handle++;

      setupMsgData->data[6]  = uuid.data()[1];
      setupMsgData->data[7]  = uuid.data()[0];

      setupMsgData->data[8]  = ACI_STORE_LOCAL;

      memcpy(&setupMsgData->data[9], descriptor->value(), descriptor->valueLength());

      gattSetupMsgOffset += 9 + descriptor->valueSize();
    }
  }

  // terminator
  this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(6);
  setupMsgIndex++;
  setupMsgData = (struct setupMsgData*)setupMsg->buffer;

  setupMsg->status_byte  = 0;
  setupMsgData->length   = 4;
  setupMsgData->cmd      = ACI_CMD_SETUP;
  setupMsgData->type     = 0x20;
  setupMsgData->offset   = gattSetupMsgOffset;

  setupMsgData->data[0]  = 0x00;

  gattSetupMsgOffset += 6;


  // update number of piped handles
  setupMsg = this->_aciState.aci_setup_info.setup_msgs[1];
  setupMsgData = (struct setupMsgData*)setupMsg->buffer;

  setupMsgData->data[6] = numPiped;
  setupMsgData->data[8] = numPiped;
  this->_numPipeInfo = numPiped;

  // pipes
  unsigned char pipeSetupMsgOffet  = 0;

  for (int i = 0; i < numPiped; i++) {
    struct pipeInfo pipeInfo = this->_pipeInfo[i];

    this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(15);
    setupMsgIndex++;
    setupMsgData = (struct setupMsgData*)setupMsg->buffer;

    setupMsg->status_byte  = 0;
    setupMsgData->length   = 13;
    setupMsgData->cmd      = ACI_CMD_SETUP;
    setupMsgData->type     = 0x40;
    setupMsgData->offset   = pipeSetupMsgOffet;

    setupMsgData->data[0]  = 0x00;
    setupMsgData->data[1]  = 0x00;

    setupMsgData->data[2]  = pipeInfo.startPipe;

    setupMsgData->data[3]  = 0x00;
    setupMsgData->data[4]  = 0x00;

    setupMsgData->data[5]  = 0x04;

    setupMsgData->data[6]  = (pipeInfo.valueHandle >> 8) & 0xff;
    setupMsgData->data[7]  = pipeInfo.valueHandle & 0xff;

    setupMsgData->data[8]  = (pipeInfo.configHandle >> 8) & 0xff;
    setupMsgData->data[9]  = pipeInfo.configHandle & 0xff;

    if (pipeInfo.characteristic->properties() & BLEIndicate) {
      setupMsgData->data[4] |= 0x04; // TX Ack
    }

    if (pipeInfo.characteristic->properties() & BLENotify) {
      setupMsgData->data[4] |= 0x02; // TX
    }

    if (pipeInfo.characteristic->properties() & BLEWriteWithoutResponse) {
      setupMsgData->data[4] |= 0x08; // RX Ack
    }

    if (pipeInfo.characteristic->properties() & BLEWrite) {
      setupMsgData->data[4] |= 0x10; // RX Ack
    }

    if (pipeInfo.characteristic->properties() & BLERead) {
      setupMsgData->data[4] |= 0x80; // Set
    }

    pipeSetupMsgOffet += 10;
  }

  // crc
  unsigned short crcSeed = 0xFFFF;
  unsigned char msgLen;

  //Run the CRC algorithm on the modified Setup to find the new CRC
  for (int i = 0; i < this->_aciState.aci_setup_info.num_setup_msgs; i++) {
    setupMsg = this->_aciState.aci_setup_info.setup_msgs[i];

    if (this->_aciState.aci_setup_info.num_setup_msgs - 1 == i) {
      msgLen = setupMsg->buffer[0] - 1; //since the 2 bytes of CRC itself should not be used
                                                    //to calculate the CRC
    } else {
      msgLen = setupMsg->buffer[0] + 1;
    }
    crcSeed = crc_16_ccitt(crcSeed, setupMsg->buffer, msgLen);
  }

  this->_aciState.aci_setup_info.setup_msgs[setupMsgIndex] = setupMsg = (hal_aci_data_t*)malloc(8);
  setupMsgIndex++;
  setupMsgData = (struct setupMsgData*)setupMsg->buffer;

  setupMsg->status_byte = 0;

  setupMsgData->length  = 3 + 3;
  setupMsgData->cmd     = ACI_CMD_SETUP;
  setupMsgData->type    = 0xf0;
  setupMsgData->offset  = 0x00;

  setupMsgData->data[0] = 0x03;
  setupMsgData->data[1] = (crcSeed >> 8) & 0xff;
  setupMsgData->data[2] = crcSeed & 0xff;

#ifdef NRF_8001_DEBUG
  Serial.println();
  for (int i = 0; i < this->_aciState.aci_setup_info.num_setup_msgs; i++) {
    setupMsg = this->_aciState.aci_setup_info.setup_msgs[i];

    for (int j = 0; j < (setupMsg->buffer[0] + 1); j++) {
      if ((setupMsg->buffer[j] & 0xf0) == 00) {
        Serial.print("0");
      }

      Serial.print(setupMsg->buffer[j], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
#endif

  lib_aci_init(&this->_aciState, false);

  while (!this->_isSetup) {
    this->poll();
  }
}

void nRF8001::poll() {
  // We enter the if statement only when there is a ACI event available to be processed
  if (lib_aci_event_get(&this->_aciState, &this->_aciData)) {
    aci_evt_t* aciEvt;
    aciEvt = &this->_aciData.evt;

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
            this->_setupRequired = true;
            break;

          case ACI_DEVICE_STANDBY:
#ifdef NRF_8001_DEBUG
            Serial.println(F("Evt Device Started: Standby"));
#endif
            //Looking for an iPhone by sending radio advertisements
            //When an iPhone connects to us we will get an ACI_EVT_CONNECTED event from the nRF8001
            if (aciEvt->params.device_started.hw_error) {
              delay(20); //Handle the HW error event correctly.
            } else {
              lib_aci_connect(0/* in seconds : 0 means forever */, ADVERTISING_INTERVAL);
#ifdef NRF_8001_DEBUG
              Serial.println(F("Advertising started"));
#endif
            }
            break;
        }
      }
      break; //ACI Device Started Event

      case ACI_EVT_CMD_RSP:
        //If an ACI command response event comes with an error -> stop
        if (ACI_STATUS_SUCCESS != aciEvt->params.cmd_rsp.cmd_status) {
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
            case ACI_CMD_GET_DEVICE_VERSION:
              break;

            case ACI_CMD_GET_DEVICE_ADDRESS: {
#ifdef NRF_8001_DEBUG
              char address[18];

              sprintf(address, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own[5],
                aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own[4],
                aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own[3],
                aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own[2],
                aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own[1],
                aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own[0]);
              Serial.print(F("Device address = "));
              Serial.println(address);

              Serial.print(F("Device address type = "));
              Serial.println(aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_type, DEC);
#endif
              if (this->_eventListener) {
                this->_eventListener->nRF8001AddressReceived(*this, aciEvt->params.cmd_rsp.params.get_device_address.bd_addr_own);
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
                this->_eventListener->nRF8001BatteryLevelReceived(*this, batteryLevel);
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
                this->_eventListener->nRF8001TemperatureReceived(*this, temperature);
              }
              break;
            }
          }
        }
        break;

      case ACI_EVT_CONNECTED:

#ifdef NRF_8001_DEBUG
        char address[18];
        sprintf(address, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
          aciEvt->params.connected.dev_addr[5],
          aciEvt->params.connected.dev_addr[4],
          aciEvt->params.connected.dev_addr[3],
          aciEvt->params.connected.dev_addr[2],
          aciEvt->params.connected.dev_addr[1],
          aciEvt->params.connected.dev_addr[0]);
        Serial.print(F("Evt Connected "));
        Serial.println(address);
#endif
        if (this->_eventListener) {
          this->_eventListener->nRF8001Connected(*this, aciEvt->params.connected.dev_addr);
        }

        this->_aciState.data_credit_available = this->_aciState.data_credit_total;
        break;

      case ACI_EVT_PIPE_STATUS:
#ifdef NRF_8001_DEBUG
        Serial.println(F("Evt Pipe Status "));

        uint64_t openPipes;
        uint64_t closedPipes;

        memcpy(&openPipes, aciEvt->params.pipe_status.pipes_open_bitmap, sizeof(openPipes));
        memcpy(&closedPipes, aciEvt->params.pipe_status.pipes_closed_bitmap, sizeof(closedPipes));

        Serial.println((unsigned long)openPipes, HEX);
        Serial.println((unsigned long)closedPipes, HEX);
#endif

        for (int i = 0; i < this->_numPipeInfo; i++) {
          struct pipeInfo* pipeInfo = &this->_pipeInfo[i];

          if (pipeInfo->txPipe) {
            pipeInfo->txPipeOpen = lib_aci_is_pipe_available(&this->_aciState, pipeInfo->txPipe);
          }

          if (pipeInfo->txAckPipe) {
            pipeInfo->txAckPipeOpen = lib_aci_is_pipe_available(&this->_aciState, pipeInfo->txAckPipe);
          }

          bool subscribed = (pipeInfo->txPipeOpen || pipeInfo->txAckPipeOpen);

          if (pipeInfo->characteristic->subscribed() != subscribed) {
            if (this->_eventListener) {
              this->_eventListener->nRF8001CharacteristicSubscribedChanged(*this, *pipeInfo->characteristic, subscribed);
            }
          }
        }
        break;

      case ACI_EVT_TIMING:
        break;

      case ACI_EVT_DISCONNECTED:
#ifdef NRF_8001_DEBUG
        Serial.println(F("Evt Disconnected/Advertising timed out"));
#endif
        // all characteristics unsubscribed on disconnect
        for (int i = 0; i < this->_numPipeInfo; i++) {
          struct pipeInfo* pipeInfo = &this->_pipeInfo[i];

          if (pipeInfo->characteristic->subscribed()) {
            if (this->_eventListener) {
              this->_eventListener->nRF8001CharacteristicSubscribedChanged(*this, *pipeInfo->characteristic, false);
            }
          }
        }

        if (this->_eventListener) {
          this->_eventListener->nRF8001Disconnected(*this);
        }

        lib_aci_connect(0/* in seconds  : 0 means forever */, ADVERTISING_INTERVAL);
#ifdef NRF_8001_DEBUG
        Serial.println(F("Advertising started."));
#endif
        break;

      case ACI_EVT_DATA_RECEIVED: {
        uint8_t dataLen = aciEvt->len - 2;
        uint8_t pipe = aciEvt->params.data_received.rx_data.pipe_number;
#ifdef NRF_8001_DEBUG
        Serial.print(F("Data Received, pipe = "));
        Serial.println(aciEvt->params.data_received.rx_data.pipe_number, DEC);

        for (int i = 0; i < dataLen; i++) {
          if ((aciEvt->params.data_received.rx_data.aci_data[i] & 0xf0) == 00) {
            Serial.print("0");
          }

          Serial.print(aciEvt->params.data_received.rx_data.aci_data[i], HEX);
          Serial.print(F(" "));
        }
        Serial.println();
#endif

        for (int i = 0; i < this->_numPipeInfo; i++) {
          struct pipeInfo* pipeInfo = &this->_pipeInfo[i];

          if (pipeInfo->rxAckPipe == pipe || pipeInfo->rxPipe == pipe) {
            if (pipeInfo->rxAckPipe == pipe) {
              lib_aci_send_ack(&this->_aciState, pipeInfo->rxAckPipe);
            }

            if (this->_eventListener) {
              this->_eventListener->nRF8001CharacteristicValueChanged(*this, *pipeInfo->characteristic, aciEvt->params.data_received.rx_data.aci_data, dataLen);
            }
            break;
          }
        }
      }
        break;

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
        lib_aci_connect(0/* in seconds, 0 means forever */, ADVERTISING_INTERVAL);
#ifdef NRF_8001_DEBUG
        Serial.println(F("Advertising started."));
#endif
        break;
    }
  } else {
    //Serial.println(F("No ACI Events available"));
    // No event in the ACI Event queue and if there is no event in the ACI command queue the arduino can go to sleep
    // Arduino can go to sleep now
    // Wakeup from sleep from the RDYN line
  }

  /* setup_required is set to true when the device starts up and enters setup mode.
   * It indicates that do_aci_setup() should be called. The flag should be cleared if
   * do_aci_setup() returns ACI_STATUS_TRANSACTION_COMPLETE.
   */
  if(this->_setupRequired && SETUP_SUCCESS == do_aci_setup(&this->_aciState))
  {
    this->_setupRequired = false;

    this->_isSetup = true;
  }
}

void nRF8001::updateCharacteristicValue(BLECharacteristic& characteristic) {
  for (int i = 0; i < this->_numPipeInfo; i++) {
    struct pipeInfo* pipeInfo = &this->_pipeInfo[i];

    if (pipeInfo->characteristic == &characteristic) {
      if (pipeInfo->setPipe) {
        lib_aci_set_local_data(&this->_aciState, pipeInfo->setPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());
      }

      if (pipeInfo->txPipe && pipeInfo->txPipeOpen && lib_aci_get_nb_available_credits(&this->_aciState)) {
        lib_aci_send_data(pipeInfo->txPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());
      }

      if (pipeInfo->txAckPipe && pipeInfo->txAckPipeOpen && lib_aci_get_nb_available_credits(&this->_aciState)) {
        lib_aci_send_data(pipeInfo->txAckPipe, (uint8_t*)characteristic.value(), characteristic.valueLength());
      }

      break;
    }
  }
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
