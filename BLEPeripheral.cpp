#include <SPI.h>

#include <utility/lib_aci.h>
#include <utility/aci_setup.h>

#include "BLEUuid.h"

#include "BLEPeripheral.h"

#define MAX_BLE_DATA_LENGTH 20

#define DEFAULT_DEVICE_NAME "Arduino"
#define DEFAULT_APPEARANCE 0x0000

static struct aci_state_t aci_state;

#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
    static services_pipe_type_mapping_t
        services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
    #define NUMBER_OF_PIPES 0
    static services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif

#define NB_BASE_SETUP_MESSAGES 10

/* Store the setup for the nRF8001 in the flash of the AVR to save on RAM */
static hal_aci_data_t base_setup_msgs[NB_BASE_SETUP_MESSAGES] PROGMEM = {\
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
      0x19,0x06,0x70,0x16,0x09,0x02,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
      0x00,0x00,0x00,0x00,0x00,0x00,\
    },\
  },\
  {0x00,\
    {\
      0x1f,0x06,0x20,0x00,0x04,0x04,0x02,0x02,0x00,0x01,0x28,0x00,0x01,0x00,0x18,0x04,0x04,0x05,0x05,0x00,\
      0x02,0x28,0x03,0x01,0x02,0x03,0x00,0x00,0x2a,0x04,0x04,0x14,\
    },\
  },\
  {0x00,\
    {\
      0x1f,0x06,0x20,0x1c,0x00,0x00,0x03,0x2a,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,\
    },\
  },\
  {0x00,\
    {\
      0x1a,0x06,0x20,0x38,0x05,0x05,0x00,0x04,0x28,0x03,0x01,0x02,0x05,0x00,0x01,0x2a,0x06,0x04,0x03,0x02,\
      0x00,0x05,0x2a,0x01,0x01,0x00,0x00,\
    },\
  },\
};

static hal_aci_data_t* setup_msgs = NULL;
static uint8_t num_setup_msgs = NB_BASE_SETUP_MESSAGES + 2;

/*
Temporary buffers for sending ACI commands
*/
static hal_aci_evt_t  aci_data;


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

BLEPeripheral::BLEPeripheral(int8_t req, int8_t rdy, int8_t rst) {
  this->setLocalName(NULL);
  this->setManufacturerData(NULL, 0);
  this->setAdvertisedServiceUuid(NULL);

  this->setDeviceName(DEFAULT_DEVICE_NAME);
  this->setAppearance(DEFAULT_APPEARANCE);

  aci_state.aci_pins.reqn_pin               = req;
  aci_state.aci_pins.rdyn_pin               = rdy;
  aci_state.aci_pins.mosi_pin               = MOSI;
  aci_state.aci_pins.miso_pin               = MISO;
  aci_state.aci_pins.sck_pin                = SCK;

  aci_state.aci_pins.spi_clock_divider      = SPI_CLOCK_DIV8;

  aci_state.aci_pins.reset_pin              = rst;
  aci_state.aci_pins.active_pin             = UNUSED;
  aci_state.aci_pins.optional_chip_sel_pin  = UNUSED;

  aci_state.aci_pins.interface_is_interrupt = false;
  aci_state.aci_pins.interrupt_number       = 1;
}

bool BLEPeripheral::begin() {
  if (NULL != services_pipe_type_mapping) {
    aci_state.aci_setup_info.services_pipe_type_mapping = &services_pipe_type_mapping[0];
  } else {
    aci_state.aci_setup_info.services_pipe_type_mapping = NULL;
  }

  setup_msgs = (hal_aci_data_t*)malloc(sizeof(hal_aci_data_t) * num_setup_msgs);
  memcpy_P(setup_msgs, base_setup_msgs, sizeof(base_setup_msgs));

  Serial.println();
  Serial.print(F("ACI setup message size = "));
  Serial.print((sizeof(hal_aci_data_t) * num_setup_msgs));
  Serial.println();

  aci_state.aci_setup_info.number_of_pipes    = NUMBER_OF_PIPES;
  aci_state.aci_setup_info.setup_msgs         = setup_msgs;
  aci_state.aci_setup_info.num_setup_msgs     = num_setup_msgs;

  hal_aci_data_t* advertising_data_setup_msg = &setup_msgs[5];
  hal_aci_data_t* scan_response_data_setup_msg = &setup_msgs[6];

  if (this->_advertisedServiceUuid) {
    BLEUuid advertisedServiceUuid = BLEUuid(this->_advertisedServiceUuid);

    char advertisedServiceUuidLength = advertisedServiceUuid.length();

    // assign the EIR data
    advertising_data_setup_msg->buffer[4] = (advertisedServiceUuidLength > 2) ? 0x06 : 0x02;
    advertising_data_setup_msg->buffer[5] = advertisedServiceUuidLength;
    memcpy(&advertising_data_setup_msg->buffer[6], advertisedServiceUuid.data(), advertisedServiceUuidLength);

    // enable advertising data
    setup_msgs[2].buffer[26] = 0x40;
  } else if (this->_manufacturerData && this->_manufacturerDataLength > 0) {
    if (this->_manufacturerDataLength > 20) {
      this->_manufacturerDataLength = MAX_BLE_DATA_LENGTH;
    }

    // assign the EIR data
    advertising_data_setup_msg->buffer[4] = 0xff;
    advertising_data_setup_msg->buffer[5] = this->_manufacturerDataLength;
    memcpy(&advertising_data_setup_msg->buffer[6], this->_manufacturerData, this->_manufacturerDataLength);

    // enable advertising data
    setup_msgs[2].buffer[26] = 0x40;
  }

  if (this->_localName) {
    char localNameLength = strlen(this->_localName);
    char localNameType = 0x09;

    if (localNameLength > MAX_BLE_DATA_LENGTH) {
      localNameLength = MAX_BLE_DATA_LENGTH;
      localNameType = 0x08;
    }

    // assign the EIR data
    scan_response_data_setup_msg->buffer[4] = localNameType;
    scan_response_data_setup_msg->buffer[5] = localNameLength;
    memcpy(&scan_response_data_setup_msg->buffer[6], this->_localName, localNameLength);

    // enable scan response data
    setup_msgs[3].buffer[16] = 0x40;
  }

  if (this->_deviceName) {
    uint8_t deviceNameLength = min(strlen(this->_deviceName), MAX_BLE_DATA_LENGTH);

    setup_msgs[8].buffer[4] = deviceNameLength;
    memcpy(&setup_msgs[8].buffer[10], this->_deviceName, deviceNameLength);
  }

  // appearance
  setup_msgs[9].buffer[25] = this->_appearance & 0xff;
  setup_msgs[9].buffer[26] = (this->_appearance >> 8) & 0xff;

  uint8_t gatt_setup_msg_offset = 79;


  // terminator
  hal_aci_data_t* gatt_terminator_setup_msg = &setup_msgs[num_setup_msgs - 2];

  gatt_terminator_setup_msg->status_byte = 0;
  gatt_terminator_setup_msg->buffer[0] = 0x04;
  gatt_terminator_setup_msg->buffer[1] = 0x06;
  gatt_terminator_setup_msg->buffer[2] = 0x20;
  gatt_terminator_setup_msg->buffer[3] = gatt_setup_msg_offset;
  gatt_terminator_setup_msg->buffer[4] = 0x00;

  uint16_t crc_seed = 0xFFFF;
  uint8_t msg_len;
  uint8_t crc_loop;

  //Run the CRC algorithm on the modified Setup to find the new CRC
  for (crc_loop = 0; crc_loop < num_setup_msgs; crc_loop++) {
    if (num_setup_msgs - 1 == crc_loop) {
      msg_len = setup_msgs[crc_loop].buffer[0] - 1; //since the 2 bytes of CRC itself should not be used
                                                    //to calculate the CRC
    } else {
      msg_len = setup_msgs[crc_loop].buffer[0] + 1;
    }
    crc_seed = crc_16_ccitt(crc_seed, &setup_msgs[crc_loop].buffer[0], msg_len);
  }

  // update the CRC
  hal_aci_data_t* crc_setup_msg = &setup_msgs[num_setup_msgs - 1];

  crc_setup_msg->status_byte = 0;
  crc_setup_msg->buffer[0] = 0x06;
  crc_setup_msg->buffer[1] = 0x06;
  crc_setup_msg->buffer[2] = 0xf0;
  crc_setup_msg->buffer[3] = 0x00;
  crc_setup_msg->buffer[4] = 0x03;
  crc_setup_msg->buffer[5] = (crc_seed >> 8) & 0xff;
  crc_setup_msg->buffer[6] = crc_seed & 0xff;

  //We reset the nRF8001 here by toggling the RESET line connected to the nRF8001
  //If the RESET line is not available we call the ACI Radio Reset to soft reset the nRF8001
  //then we initialize the data structures required to setup the nRF8001
  //The second parameter is for turning debug printing on for the ACI Commands and Events so they be printed on the Serial
  lib_aci_init(&aci_state, false);

  return true;
}

void BLEPeripheral::poll() {
  static bool setup_required = false;

  // We enter the if statement only when there is a ACI event available to be processed
  if (lib_aci_event_get(&aci_state, &aci_data)) {
    aci_evt_t* aci_evt;
    aci_evt = &aci_data.evt;

    switch(aci_evt->evt_opcode) {
      /**
      As soon as you reset the nRF8001 you will get an ACI Device Started Event
      */
      case ACI_EVT_DEVICE_STARTED: {
        aci_state.data_credit_total = aci_evt->params.device_started.credit_available;
        switch(aci_evt->params.device_started.device_mode) {
          case ACI_DEVICE_SETUP:
            /**
            When the device is in the setup mode
            */
            Serial.println(F("Evt Device Started: Setup"));
            setup_required = true;
            break;

          case ACI_DEVICE_STANDBY:
            Serial.println(F("Evt Device Started: Standby"));
            //Looking for an iPhone by sending radio advertisements
            //When an iPhone connects to us we will get an ACI_EVT_CONNECTED event from the nRF8001
            if (aci_evt->params.device_started.hw_error) {
              delay(20); //Handle the HW error event correctly.
            } else {
              lib_aci_connect(0/* in seconds : 0 means forever */, 0x0050 /* advertising interval 50ms*/);
              Serial.println(F("Advertising started"));
            }
            break;
        }
      }
      break; //ACI Device Started Event

      case ACI_EVT_CMD_RSP:
        //If an ACI command response event comes with an error -> stop
        if (ACI_STATUS_SUCCESS != aci_evt->params.cmd_rsp.cmd_status) {
          //ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
          //TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
          //all other ACI commands will have status code of ACI_STATUS_SCUCCESS for a successful command
          Serial.print(F("ACI Command "));
          Serial.println(aci_evt->params.cmd_rsp.cmd_opcode, HEX);
          Serial.print(F("Evt Cmd respone: Status "));
          Serial.println(aci_evt->params.cmd_rsp.cmd_status, HEX);
        }
        break;

      case ACI_EVT_CONNECTED:
        Serial.println(F("Evt Connected"));
        aci_state.data_credit_available = aci_state.data_credit_total;
        break;

      case ACI_EVT_PIPE_STATUS:
        break;

      case ACI_EVT_TIMING:
        break;

      case ACI_EVT_DISCONNECTED:
        Serial.println(F("Evt Disconnected/Advertising timed out"));
        lib_aci_connect(0/* in seconds  : 0 means forever */, 0x0050 /* advertising interval 50ms*/);
        Serial.println(F("Advertising started."));
        break;

      case ACI_EVT_DATA_RECEIVED:
        Serial.print(F("Pipe Number: "));
        Serial.println(aci_evt->params.data_received.rx_data.pipe_number, DEC);
        break;

      case ACI_EVT_DATA_CREDIT:
        aci_state.data_credit_available = aci_state.data_credit_available + aci_evt->params.data_credit.credit;
        break;

      case ACI_EVT_PIPE_ERROR:
        //See the appendix in the nRF8001 Product Specication for details on the error codes
        Serial.print(F("ACI Evt Pipe Error: Pipe #:"));
        Serial.print(aci_evt->params.pipe_error.pipe_number, DEC);
        Serial.print(F("  Pipe Error Code: 0x"));
        Serial.println(aci_evt->params.pipe_error.error_code, HEX);

        //Increment the credit available as the data packet was not sent.
        //The pipe error also represents the Attribute protocol Error Response sent from the peer and that should not be counted
        //for the credit.
        if (ACI_STATUS_ERROR_PEER_ATT_ERROR != aci_evt->params.pipe_error.error_code) {
          aci_state.data_credit_available++;
        }
        break;

      case ACI_EVT_HW_ERROR:
        Serial.print(F("HW error: "));
        Serial.println(aci_evt->params.hw_error.line_num, DEC);

        for(uint8_t counter = 0; counter <= (aci_evt->len - 3); counter++) {
          Serial.write(aci_evt->params.hw_error.file_name[counter]); //uint8_t file_name[20];
        }
        Serial.println();
        lib_aci_connect(0/* in seconds, 0 means forever */, 0x0050 /* advertising interval 50ms*/);
        Serial.println(F("Advertising started."));
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
  if(setup_required && SETUP_SUCCESS == do_aci_setup(&aci_state))
  {
    setup_required = false;
  }
}

void BLEPeripheral::setAdvertisedServiceUuid(const char* advertisedServiceUuid) {
  this->_advertisedServiceUuid = advertisedServiceUuid;
}

void BLEPeripheral::setManufacturerData(const char* manufacturerData, int manufacturerDataLength) {
  this->_manufacturerData = manufacturerData;
  this->_manufacturerDataLength = manufacturerDataLength;
}

void BLEPeripheral::setLocalName(const char *localName) {
  this->_localName = localName;
}

void BLEPeripheral::setDeviceName(const char* deviceName) {
  this->_deviceName = deviceName;
}

void BLEPeripheral::setAppearance(unsigned short appearance) {
  this->_appearance = appearance;
}
