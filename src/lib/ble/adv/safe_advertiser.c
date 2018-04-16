/*
 * safe_advertiser.c
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "app_config.h"
#include "app_error.h"

#include "nrf_drv_twi.h"
#include "app_util_platform.h"

#include "ble_advdata.h"
#include "ble_advertising.h"
#include "safe_advertiser.h"

static ble_gap_adv_params_t m_adv_params;

void advertising_init()
{
    uint32_t err_code;
	ble_advdata_t adv_data;
    ble_advdata_t scrsp_data;

    /** Initialize advertising parameters (used when starting advertising). */
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    m_adv_params.type = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
    m_adv_params.fp = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval = MSEC_TO_UNITS(200, UNIT_0_625_MS);
    m_adv_params.timeout = APP_CFG_CONNECTABLE_ADV_TIMEOUT;

    /** Build and set advertising data. */
    memset(&adv_data, 0, sizeof(ble_advdata_t));
    adv_data.name_type = BLE_ADVDATA_FULL_NAME;
    adv_data.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    /** Build and set scan response data. */
    memset(&scrsp_data, 0, sizeof(scrsp_data));
    scrsp_data.name_type = BLE_ADVDATA_FULL_NAME;
    scrsp_data.include_appearance = false;

    err_code = ble_advdata_set(&adv_data, &scrsp_data);

    advertising_start();
}

void advertising_start()
{
    uint32_t err_code = sd_ble_gap_adv_start(&m_adv_params);
    if (err_code != NRF_ERROR_BUSY && err_code != NRF_ERROR_CONN_COUNT)
    {
        APP_ERROR_CHECK(err_code);
    }
}
void advertising_stop()
{
    uint32_t err_code = sd_ble_gap_adv_stop();

    if (err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_CHECK(err_code);
    }
}
