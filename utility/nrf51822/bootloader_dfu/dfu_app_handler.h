/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
  
/** @file
 *
 * @defgroup nrf_dfu_app_handler DFU BLE packet handling in Application
 * @{
 *
 * @brief DFU BLE packet handling for application.
 *
 * @details This module implements handling of DFU packets transmitted over BLE for switching from 
 *          application mode to Bootloader running full DFU service.
 *          This module only handles the StartDFU packet allowing for any BLE application to expose
 *          support for the DFU service.
 *          Actual DFU service will execute in dedicated environment after a BLE disconnect and 
 *          reset of the nRF51 chip.
 *          The host must then reconnect and can continue the update procedure with access to full
 *          DFU service.
 *
 * @note The application must propagate dfu events to the DFU App handler module by calling
 *       dfu_app_on_dfu_evt() from the from the @ref ble_dfu_evt_handler_t callback.
 */
 
#ifndef DFU_APP_HANDLER_H__
#define DFU_APP_HANDLER_H__

#include "dfu_app_handler.h"
#include "ble_dfu.h"

/**@brief DFU Application reset prepare function. This function is a callback which allows the 
 *        application to prepare for an upcoming application reset. 
 */
typedef void (*dfu_app_reset_prepare_t)(void);


/**@brief   Function for handling of \ref ble_dfu_evt_t from DFU Service. 
 *
 * @details The application must inject this function into the DFU service or propagate DFU events
 *          to dfu_app_handler module by calling this function in application specific DFU event 
 *          handler.
 * 
 * @param[in] p_dfu  Pointer to the DFU Service structure for which the include event relates.
 * @param[in] p_evt  Pointer to the DFU event.
 */
void dfu_app_on_dfu_evt(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt);


/**@brief Function for registering for reset prepare calls. 
 *
 * @details The function provided will be executed before reseting the system into Bootloader/DFU
 *          mode. By setting this function the caller will be notified prior to the reset and can
 *          thus prepare the application for reset. As example the application can gracefully
 *          disconnect any peers on BLE, turning of LEDS, ensure all pending flash operations
 *          has completed, etc.
 *
 * @param[in] reset_prepare_func  Function to be execute prior to a reset.
 */
void dfu_app_reset_prepare_set(dfu_app_reset_prepare_t reset_prepare_func);


#endif // DFU_APP_HANDLER_H__

/** @} */
