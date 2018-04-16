/*
 * app_config.h
 *
 *  Created on: 12 de ago de 2017
 *      Author: Gabriel
 */

#ifndef SRC_APP_CONFIG_H_
#define SRC_APP_CONFIG_H_

#include "custom_board.h"

#define MAX_PASSWORD_TRIES								3
#define PASSWORD_SIZE									6
#define LOCK_DEFAULT_PIN								8
#define CLIENT_NTF_PIN									BLE400_LED_3_PIN
#define LOCK_NTF_PIN									BLE400_LED_4_PIN

/** Timer configurations. */
#define APP_TIMER_PRESCALER                             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE                         10                                          /**< Size of timer operation queues. */
#define APP_TIMER_MAX_TIMERS                            10

// Scheduler configurations. */
#define SCHED_MAX_EVENT_DATA_SIZE                       sizeof(app_timer_event_t)
#define SCHED_QUEUE_SIZE                                10

/** Advertising characteristics. */
#define DEFAULT_ADVERTISING_TX_POWER                    0xF4 /** -12 dBm */                         /**< Advertising Tx Power. */
#define ADVERTISING_RSSI_0M                             0xC3                                        /**< The Beacon's measured RSSI at 0 meter distance in dBm. */
#define ADVERTISING_MINIMUM_PERIOD_MS                   100                                         /**< Set the lower limit to advertising period. Must not be minor than 20. */
#define ADVERTISING_MAXIMUM_PERIOD_MS                   10240

/** BLE configuration */
#define APP_DEVICE_NAME                                 "NeonSafe"                               /**< Advertised device name inside scan response when in connectable mode **/
#define CENTRAL_LINK_COUNT								0
#define PERIPHERAL_LINK_COUNT                           1                                           /**<number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define APP_CFG_CONNECTABLE_ADV_TIMEOUT                 0                                           /**< Time for which the device must be advertising in connectable mode (in seconds). 0 disables the time-out. */

#define MIN_CONN_INTERVAL                               MSEC_TO_UNITS(50, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL                               MSEC_TO_UNITS(90, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY                  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY                   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT                    3

/** Sensing configuration */
#define TWI_SCL_PIN		1
#define TWI_SDA_PIN 	0


#endif /* SRC_APP_CONFIG_H_ */
