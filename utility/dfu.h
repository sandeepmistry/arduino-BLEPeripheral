#ifndef DFU_H_
#define DFU_H_

#define ST_IDLE		          0
#define ST_INIT_ERROR       1
#define ST_RDY		          2
#define ST_RX_INIT_PKT	    3
#define ST_RX_DATA_PKT	    4
#define ST_FW_VALID         5
#define ST_FW_INVALID       6
#define ST_ANY	  	        255

#define EV_ANY              255

#define PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_PACKET_RX                   8
#define PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_CONTROL_POINT_TX            9
#define PIPE_DEVICE_FIRMWARE_UPDATE_BLE_SERVICE_DFU_CONTROL_POINT_RX_ACK_AUTO   10

/**@brief   DFU Event type.
 *
 * @details This enumeration contains the types of events that will be received from the DFU Service.
 */
typedef enum
{
    BLE_DFU_START                      = 1,                            /**< The event indicating that the peer wants the application to prepare for a new firmware update. */
    BLE_DFU_RECEIVE_INIT_DATA          = 2,                            /**< The event indicating that the peer wants the application to prepare to receive init parameters. */
    BLE_DFU_RECEIVE_APP_DATA           = 3,                            /**< The event indicating that the peer wants the application to prepare to receive the new firmware image. */
    BLE_DFU_VALIDATE                   = 4,                            /**< The event indicating that the peer wants the application to validate the newly received firmware image. */
    BLE_DFU_ACTIVATE_N_RESET           = 5,                            /**< The event indicating that the peer wants the application to undergo activate new firmware and restart with new valid application */
    BLE_DFU_SYS_RESET                  = 6,                            /**< The event indicating that the peer wants the application to undergo a reset and start the currently valid application image.*/
    BLE_DFU_PKT_RCPT_NOTIF_ENABLED     = 7,                            /**< The event indicating that the peer has enabled packet receipt notifications. It is the responsibility of the application to call @ref ble_dfu_pkts_rcpt_notify each time the number of packets indicated by num_of_pkts field in @ref ble_dfu_evt_t is received.*/
    BLE_DFU_PKT_RCPT_NOTIF_DISABLED    = 8,                            /**< The event indicating that the peer has disabled the packet receipt notifications.*/
    BLE_DFU_PACKET_WRITE               = 9,                            /**< The event indicating that the peer has written a value to the 'DFU Packet' characteristic. The data received from the peer will be present in the @ref ble_dfu_pkt_write element contained within @ref ble_dfu_evt_t.*/
    BLE_DFU_BYTES_RECEIVED_SEND        = 10                            /**< The event indicating that the peer is requesting for the number of bytes of firmware data last received by the application. It is the responsibility of the application to call @ref ble_dfu_pkts_rcpt_notify in response to this event. */
} ble_dfu_evt_type_t;

/**@brief   DFU Procedure type.
 *
 * @details This enumeration contains the types of DFU procedures.
 */
typedef enum
{
    BLE_DFU_START_PROCEDURE        = 1,                                 /**< DFU Start procedure.*/
    BLE_DFU_INIT_PROCEDURE         = 2,                                 /**< DFU Initialization procedure.*/
    BLE_DFU_RECEIVE_APP_PROCEDURE  = 3,                                 /**< Firmware receiving procedure.*/
    BLE_DFU_VALIDATE_PROCEDURE     = 4,                                 /**< Firmware image validation procedure .*/
    BLE_DFU_PKT_RCPT_REQ_PROCEDURE = 8                                  /**< Packet receipt notification request procedure. */
} ble_dfu_procedure_t;

/**@brief   DFU Response value type.
 */
typedef enum
{
    BLE_DFU_RESP_VAL_SUCCESS        = 1,                                /**< Success.*/
    BLE_DFU_RESP_VAL_INVALID_STATE  = 2,                                /**< Invalid state.*/
    BLE_DFU_RESP_VAL_NOT_SUPPORTED  = 3,                                /**< Operation not supported.*/
    BLE_DFU_RESP_VAL_DATA_SIZE      = 4,                                /**< Data size exceeds limit.*/
    BLE_DFU_RESP_VAL_CRC_ERROR      = 5,                                /**< CRC Error.*/
    BLE_DFU_RESP_VAL_OPER_FAILED    = 6,                                /**< Operation failed.*/
} ble_dfu_resp_val_t;

/**
 *  @brief FSM transition
 *
 * Struct that describes a transition in the FSM.
 */
typedef struct
{   /**
	 * @brief State
	 */
	uint8_t st;

    /**
	 * @brief Event
	 */
	uint8_t ev;

    /**
	 * @brief Transition function
	 */
	uint8_t (*fn)(aci_state_t *aci_state, aci_evt_t *aci_evt);
} dfu_transition_t;

void dfu_initialize (void);
void dfu_update (aci_state_t *aci_state, aci_evt_t *aci_evt);

#endif /* DFU_H_ */