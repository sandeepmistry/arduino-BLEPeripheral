#include <lib_aci.h>
#include <ble_assert.h>
#include <dfu.h>

static uint8_t state = ST_ANY;
static uint32_t firmware_len;
static uint16_t notify_every;
static uint32_t bytes_received;
static uint16_t packets_received;

static void dfu_notify (aci_state_t *aci_state);
static bool dfu_send (aci_state_t *aci_state, uint8_t *buffer, uint8_t buffer_len);

static uint8_t dfu_data_pkt_handle (aci_state_t *aci_state, aci_evt_t *aci_evt);
static uint8_t dfu_init_pkt_handle (aci_state_t *aci_state, aci_evt_t *aci_evt);
static uint8_t dfu_image_activate  (aci_state_t *aci_state, aci_evt_t *aci_evt);
static uint8_t dfu_image_size_set (aci_state_t *aci_state, aci_evt_t *aci_evt);
static uint8_t dfu_image_validate  (aci_state_t *aci_state, aci_evt_t *aci_evt);

#define DEBUG

enum
{
    OP_CODE_START_DFU            = 1,                                               /**< Value of the Op code field for 'Start DFU' command.*/
    OP_CODE_RECEIVE_INIT         = 2,                                               /**< Value of the Op code field for 'Initialize DFU parameters' command.*/
    OP_CODE_RECEIVE_FW           = 3,                                               /**< Value of the Op code field for 'Receive firmware image' command.*/
    OP_CODE_VALIDATE             = 4,                                               /**< Value of the Op code field for 'Validate firmware' command.*/
    OP_CODE_ACTIVATE_N_RESET     = 5,                                               /**< Value of the Op code field for 'Activate & Reset' command.*/
    OP_CODE_SYS_RESET            = 6,                                               /**< Value of the Op code field for 'Reset System' command.*/
    OP_CODE_IMAGE_SIZE_REQ       = 7,                                               /**< Value of the Op code field for 'Report received image size' command.*/
    OP_CODE_PKT_RCPT_NOTIF_REQ   = 8,                                               /**< Value of the Op code field for 'Request packet receipt notification.*/
    OP_CODE_RESPONSE             = 16,                                              /**< Value of the Op code field for 'Response.*/
    OP_CODE_PKT_RCPT_NOTIF       = 17                                               /**< Value of the Op code field for 'Packets Receipt Notification'.*/
};

/* State transitions */

static uint8_t dfu_begin_init (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);
  ble_assert (state == ST_RDY);

  return ST_RX_INIT_PKT;
}

static uint8_t dfu_begin_transfer (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);
  ble_assert ((state == ST_RDY) | (state == ST_RX_INIT_PKT));

  return ST_RX_DATA_PKT;
}

static uint8_t dfu_data_pkt_handle (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  static uint8_t response[3] = {OP_CODE_RESPONSE, BLE_DFU_RECEIVE_APP_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS};

  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);
  ble_assert (state == ST_RX_DATA_PKT);

  bytes_received += aci_evt->len-2;
  packets_received++;

  /* Send notification for every N packets */
  if (0 == (packets_received % notify_every))
  {
      #ifdef DEBUG
      Serial.println(F(" Writing notification"));
      #endif DEBUG

      dfu_notify (aci_state);
  }

  if (firmware_len == bytes_received)
  {
    #ifdef DEBUG
    Serial.print(F(" All bytes received ("));
    Serial.print(bytes_received, DEC);
    Serial.println(F(")"));
    #endif

    dfu_send (aci_state, response, 3);
  }

  return ST_RX_DATA_PKT;
}

static uint8_t dfu_image_size_set(aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  static uint8_t response[3] = {OP_CODE_RESPONSE, BLE_DFU_START_PROCEDURE, BLE_DFU_RESP_VAL_SUCCESS};

  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);
  ble_assert (state == ST_IDLE);

  firmware_len = (uint32_t)aci_evt->params.data_received.rx_data.aci_data[3] << 24 |
                 (uint32_t)aci_evt->params.data_received.rx_data.aci_data[2] << 16 |
                 (uint32_t)aci_evt->params.data_received.rx_data.aci_data[1] << 8  |
                 (uint32_t)aci_evt->params.data_received.rx_data.aci_data[0];

  #ifdef DEBUG
  Serial.print(F("Received length: "));
  Serial.println(firmware_len, DEC);
  #endif

  /* Write response */
  dfu_send (aci_state, response, 3);

  return ST_RDY;
}

static uint8_t dfu_image_activate(aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);
  ble_assert (state == ST_FW_VALID);

  lib_aci_disconnect(aci_state, ACI_REASON_TERMINATE);

  return ST_FW_VALID;
}

static uint8_t dfu_init_pkt_handle (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);
  ble_assert (state == ST_RX_INIT_PKT);

  return ST_RX_INIT_PKT;
}

static uint8_t dfu_image_validate (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  uint8_t new_state;
  uint8_t response[3] = {OP_CODE_RESPONSE, BLE_DFU_VALIDATE_PROCEDURE};

  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);
  ble_assert (state == ST_RX_DATA_PKT);

  /* TODO: Implement CRC validation */
  if (bytes_received == firmware_len)
  {
    #ifdef DEBUG
    Serial.println(F(" Validation successful"));
    #endif
    /* Completed successfully */
    response[3] = BLE_DFU_RESP_VAL_SUCCESS;
    dfu_send(aci_state, response, 3);

    state = ST_FW_VALID;
  }
  else
  {
    #ifdef DEBUG
    Serial.println(F(" Validation failed"));
    #endif

    /* CRC error */
    response[3] = BLE_DFU_RESP_VAL_CRC_ERROR;
    dfu_send(aci_state, response, 3);

    state = ST_FW_VALID;
  }

  return state;
}

static uint8_t dfu_notification_set (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);

  notify_every = (uint16_t)aci_evt->params.data_received.rx_data.aci_data[2] << 8 |
                 (uint16_t)aci_evt->params.data_received.rx_data.aci_data[1];



  #ifdef DEBUG
  Serial.print(F(" Remote device requested notification every "));
  Serial.print(notify_every, DEC);
  Serial.println(F(" packets"));
  #endif

  return state;
}

dfu_transition_t trans[] = {
	{ ST_IDLE,		    BLE_DFU_PACKET_WRITE,            &dfu_image_size_set          },
	{ ST_RDY,		      BLE_DFU_RECEIVE_INIT_DATA,       &dfu_begin_init              },
	{ ST_RX_INIT_PKT, BLE_DFU_PACKET_WRITE,            &dfu_init_pkt_handle         },
  { ST_RDY,		      BLE_DFU_RECEIVE_APP_DATA,        &dfu_begin_transfer          },
  { ST_RX_INIT_PKT,	BLE_DFU_RECEIVE_APP_DATA,        &dfu_begin_transfer          },
	{ ST_RX_DATA_PKT,	BLE_DFU_PACKET_WRITE,            &dfu_data_pkt_handle         },
  { ST_RX_DATA_PKT,	BLE_DFU_VALIDATE,                &dfu_image_validate          },
  { ST_FW_VALID,    BLE_DFU_SYS_RESET,               &dfu_image_activate          },
  { ST_FW_VALID,    BLE_DFU_ACTIVATE_N_RESET,        &dfu_image_activate          },
  { ST_ANY,         BLE_DFU_PKT_RCPT_NOTIF_ENABLED,  &dfu_notification_set        },
  { ST_ANY,         BLE_DFU_PKT_RCPT_NOTIF_DISABLED, &dfu_notification_set        }
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

/* Write receive notification */
static void dfu_notify (aci_state_t *aci_state)
{
  uint8_t response[6] = {OP_CODE_PKT_RCPT_NOTIF,
                         0,
                         (uint8_t) bytes_received,
                         (uint8_t) (bytes_received >> 8),
                         (uint8_t) (bytes_received >> 16),
                         (uint8_t) (bytes_received >> 24)
                         };

  ble_assert (aci_state != NULL);

  dfu_send (aci_state, response, 6);
}

static bool dfu_send (aci_state_t *aci_state, uint8_t *buffer, uint8_t buffer_len)
{
	bool status = false;

  ble_assert (aci_state != NULL);
  ble_assert (buffer != NULL);

	if (lib_aci_is_pipe_available(aci_state,
                                PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_CONTROL_POINT_TX) &&
                                (aci_state->data_credit_available >= 1))
	{
		status = lib_aci_send_data(PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_CONTROL_POINT_TX,
                               buffer,
                               buffer_len);
		if (status)
		{
			aci_state->data_credit_available--;
		}
	}

	return status;
}

static uint8_t dfu_get_event (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  uint8_t event;

  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);

  switch (aci_evt->params.data_received.rx_data.pipe_number)
  {
    case PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_PACKET_RX:
      event = BLE_DFU_PACKET_WRITE;
      break;

    case PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_CONTROL_POINT_RX_ACK_AUTO:
      #ifdef DEBUG
      Serial.print(F("PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_CONTROL_POINT_RX_ACK_AUTO. Opcode: "));
      Serial.println(aci_evt->params.data_received.rx_data.aci_data[0], DEC);
      #endif

      switch (aci_evt->params.data_received.rx_data.aci_data[0])
      {
        /* Start DFU */
        case OP_CODE_START_DFU:
          event = BLE_DFU_START;
          break;

        /* Initialize DFU parameters */
        case OP_CODE_RECEIVE_INIT:
          event = BLE_DFU_RECEIVE_INIT_DATA;
          break;

        /* Receive firmware image */
        case OP_CODE_RECEIVE_FW:
          event = BLE_DFU_RECEIVE_APP_DATA;
          break;

        /* Validate firmware image */
        case OP_CODE_VALIDATE:
          event = BLE_DFU_VALIDATE;
          break;

        /* Activate image and reset */
        case OP_CODE_ACTIVATE_N_RESET:
          event = BLE_DFU_ACTIVATE_N_RESET;
          break;

        /* Reset system */
        case OP_CODE_SYS_RESET:
          event = BLE_DFU_SYS_RESET;
          break;

        /* Report received image size */
        case OP_CODE_IMAGE_SIZE_REQ:
          event = BLE_DFU_BYTES_RECEIVED_SEND;
          break;

        /* Packet receipt notification request */
        case OP_CODE_PKT_RCPT_NOTIF_REQ:
          if (aci_evt->params.data_received.rx_data.aci_data[1] == 0)
          {
            event = BLE_DFU_PKT_RCPT_NOTIF_DISABLED;
          }
          else
          {
            event = BLE_DFU_PKT_RCPT_NOTIF_ENABLED;
          }
          break;

        default:
          event = EV_ANY;
          break;
      }
      break;

    default:
      event = EV_ANY;
  }
  return event;
}

void dfu_initialize (void)
{
  #ifdef DEBUG
  Serial.println(F(" Initializing DFU state machine"));
  #endif

  if (true)
  {
    state = ST_IDLE;
  }
  else
  {
    state = ST_INIT_ERROR;
  }
}

void dfu_update (aci_state_t *aci_state, aci_evt_t *aci_evt)
{
  hal_aci_evt_t aci_data;
  uint8_t event;
  uint8_t i;

  ble_assert (aci_state != NULL);
  ble_assert (aci_evt != NULL);

  event = dfu_get_event(aci_state, aci_evt);

  for (i = 0; i < TRANS_COUNT; i++)
	{
    if ((state == trans[i].st) || (ST_ANY == trans[i].st))
		{
		  if ((event == trans[i].ev) || (EV_ANY == trans[i].ev))
			{
        #ifdef DEBUG
        Serial.print(F("Transition (state = "));
        Serial.print(trans[i].st);
        Serial.print(F(", event = "));
        Serial.print(trans[i].ev);
        Serial.println(F(")"));
        #endif

        state = (trans[i].fn)(aci_state, aci_evt);
        return;
      }
    }
  }

  /* Unhandled event */
  ble_assert(false);
}