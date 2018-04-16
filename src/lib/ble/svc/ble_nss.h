/*
 * nss.h
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */

#ifndef SRC_LIB_BLE_SVC_BLE_NSS_H_
#define SRC_LIB_BLE_SVC_BLE_NSS_H_

#define BLE_SERVICE_BASE_UUID                    { 0x55, 0x46, 0x41, 0x42, 0x43 ,0x49, 0x41, 0x52, 0x32, 0x30, 0x31, 0x38, 0x00, 0x00, 0x47, 0x4E }

/* UUID for Neon Safe Service. */
#define BLE_NSS_SERVICE_UUID                    0x00AA

#define BLE_NSS_AUTH_CHAR  	         			0x0C00
#define BLE_NSS_PW_UPDATE_CHAR 					0x0C01
#define BLE_NSS_LOCK_UNLOCK_CHAR				0X0C02

typedef struct ble_nss
{
    uint16_t                    conn_handle;              /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle;           /**< Handle of Air Pressure Sensor Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    auth_char_handle;         /**< Handle of Sensing Interval Characteristic. */
    ble_gatts_char_handles_t    pw_update_char_handle;    /**< Handle of Sensor Status Characteristic. */
    ble_gatts_char_handles_t    lock_char_handle;         /**< Handle of Sensor Integration Time Characteristic. */
}ble_nss_t;

uint32_t ble_lss_on_ble_evt(ble_nss_t * p_nss, ble_evt_t * p_ble_evt);


void ble_lss_init(ble_nss_t * p_lss);


#endif /* SRC_LIB_BLE_SVC_BLE_NSS_H_ */
