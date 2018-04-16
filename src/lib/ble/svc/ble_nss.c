/*
 * ble_nss.c
 *
 *  Created on: Apr 16, 2018
 *      Author: gabriel
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "app_config.h"
#include "app_error.h"

#include "ble.h"
#include "ble_gatts.h"
#include "ble_nss.h"

static uint8_t cmd;

uint32_t ble_lss_on_ble_evt(ble_nss_t * p_nss, ble_evt_t * p_ble_evt)
{

	switch (p_ble_evt->header.evt_id)
	{
	case BLE_GAP_EVT_CONNECTED:
		p_nss->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
		break;
	case BLE_GAP_EVT_DISCONNECTED:
		p_nss->conn_handle = BLE_CONN_HANDLE_INVALID;
		break;
	case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
		on_rw_authorize_req(p_nss,p_ble_evt);
		break;
	default:
		// No implementation needed.
		break;
	}
	return NRF_SUCCESS;
}

void ble_lss_init(ble_nss_t * p_nss)
{
	uint32_t        err_code;
	ble_uuid_t      service_uuid;
	ble_uuid128_t   base_uuid = { BLE_SERVICE_BASE_UUID };
	uint32_t        default_password;

	service_uuid.uuid = BLE_NSS_SERVICE_UUID;

	/* Start the service with invalid handle */
	p_nss->conn_handle = BLE_CONN_HANDLE_INVALID;

	err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
			&service_uuid,
			&p_nss->service_handle);
	APP_ERROR_CHECK(err_code);

	/* Add all the characteristics */
	ble_nss_auth_char_init(p_nss);
	ble_nss_pw_update_char_init(p_nss);
	ble_nss_lock_char_init(p_nss);
}

static uint32_t on_rw_authorize_req(ble_nss_t *p_nss, ble_evt_t *p_ble_evt)
{
	if (p_ble_evt->evt.gatts_evt.params.authorize_request.type ==
			BLE_GATTS_AUTHORIZE_TYPE_WRITE)
	{
		on_write(p_nss, p_ble_evt);
	}
	if(p_ble_evt->evt.gatts_evt.params.authorize_request.type ==
			BLE_GATTS_AUTHORIZE_TYPE_READ)
	{
		on_read(p_nss, p_ble_evt);
	}
	else
	{
		return NRF_ERROR_INVALID_STATE;
	}

	return NRF_SUCCESS;
}

static void on_write(ble_nss_t *p_nss, ble_evt_t *p_ble_evt)
{
	ret_code_t err_code;
	ble_gatts_rw_authorize_reply_params_t reply;

	// Initialize reply.
	reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
	reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
	reply.params.write.update = 1;
	reply.params.write.offset = 0;
	reply.params.write.len = 0;
	reply.params.write.p_data = NULL;

	// Write to appropriate characteristic.
	ble_gatts_evt_write_t *p_evt_write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;


	if(p_evt_write->handle == p_nss->auth_char_handle.value_handle)
	{
		safe_msg_t msg;
		msg.type = SAFE_EVT_AUTHENTICATION_TRY;
		memcpy(msg.auth_try.password, p_evt_write->data, PASSWORD_SIZE);

		sec_manager_rsp_t rsp;
		rsp = safe_sec_mngr_send_cmd(&rsp);

		if(rsp == SECURITY_SUCCESS)
		{
			reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
			reply.params.write.len = PASSWORD_SIZE;
			reply.params.write.p_data = (uint8_t *) msg.auth_try.password;
		}
		else
		{
			reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
			reply.params.write.len = 0;
			reply.params.write.p_data = NULL;
		}
	}

	else if(p_evt_write->handle == p_nss->pw_update_char_handle.value_handle)
	{
		safe_msg_t msg;
		msg.type = SAFE_CMD_SET_PASSWORD;
		memcpy(msg.new_pw.password, p_evt_write->data, PASSWORD_SIZE);

		sec_manager_rsp_t rsp;
		rsp = safe_sec_mngr_send_cmd(&rsp);

		if(rsp == SECURITY_SUCCESS)
		{
			reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
			reply.params.write.len = PASSWORD_SIZE;
			reply.params.write.p_data = (uint8_t *) msg.new_pw.password;
		}
		else
		{
			reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
			reply.params.write.len = 0;
			reply.params.write.p_data = NULL;
		}
	}
	else if(p_evt_write->handle == p_nss->lock_char_handle.value_handle)
	{
		uint8_t cmd = *((uint8_t *) p_evt_write->data);

		safe_msg_t msg;
		if(cmd == 0x01)
		{
			msg.type = SAFE_CMD_OPEN;
		}
		else
		{
			msg.type = SAFE_CMD_CLOSE;
		}

		sec_manager_rsp_t rsp;
		rsp = safe_sec_mngr_send_cmd(&rsp);

		if(rsp == SECURITY_SUCCESS)
		{
			reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
			reply.params.write.len = PASSWORD_SIZE;
			reply.params.write.p_data = (uint8_t *) msg.new_pw.password;
		}
		else
		{
			reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
			reply.params.write.len = 0;
			reply.params.write.p_data = NULL;
		}
	}
	else
	{
		// Return without replying. This event is not relevant for this service.
		return;
	}

	// Send write reply.
	if (p_nss->conn_handle != BLE_CONN_HANDLE_INVALID )
	{
		err_code = sd_ble_gatts_rw_authorize_reply(p_nss->conn_handle, &reply);
		if (err_code != NRF_SUCCESS)
		{
			APP_ERROR_CHECK(err_code);
		}
	}
}

static void on_read(ble_nss_t * p_nss,ble_evt_t * p_ble_evt)
{
    ret_code_t err_code;
    ble_gatts_rw_authorize_reply_params_t reply = {0};

    // Initialize reply.
    reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
    reply.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
    reply.params.read.update = 1;
    reply.params.read.offset = 0;

    // Read from appropriate characteristic.
    ble_gatts_evt_read_t *p_evt_read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
    if (p_evt_read->handle == p_nss->pw_update_char_handle.value_handle && safe_sec_mngr_is_authorized())
    {
        // Read Data Fragment buffer filled by application.
        reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
        reply.params.read.update = 1;
        reply.params.read.offset = 0;
        reply.params.read.len = PASSWORD_SIZE;
        reply.params.read.p_data = ( uint8_t *) safe_sec_mngr_get_pw_ptr();
    }
    else
    {
        // Return without replying. This event is not relevant for this service.
        return;
    }

    // Send reply.
    if (p_nss->conn_handle != BLE_CONN_HANDLE_INVALID )
    {
        err_code = sd_ble_gatts_rw_authorize_reply(p_nss->conn_handle, &reply);
        if (err_code != NRF_SUCCESS)
        {
            APP_ERROR_CHECK(err_code);
        }
    }
}

static void ble_nss_auth_char_init(ble_nss_t *p_nss)
{
	uint32_t            err_code;
	ble_uuid_t          char_uuid;
	ble_uuid128_t       base_uuid = { BLE_SERVICE_BASE_UUID };

	char_uuid.uuid = BLE_NSS_AUTH_CHAR;

	err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
	APP_ERROR_CHECK(err_code);

	//Add read/write properties
	ble_gatts_char_md_t char_md;
	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.write = 1;

	//Configure the attribute metadata
	ble_gatts_attr_md_t attr_md;
	memset(&attr_md, 0, sizeof(attr_md));

	attr_md.vloc = BLE_GATTS_VLOC_USER;

	//Set read/write security levels to our characteristic
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.wr_auth = 1;

	//Configure the characteristic value attribute
	ble_gatts_attr_t attr_char_value;
	memset(&attr_char_value, 0, sizeof(attr_char_value));

	attr_char_value.p_uuid      = &char_uuid;
	attr_char_value.p_attr_md   = &attr_md;

	//Set characteristic length in number of bytes and value
	attr_char_value.max_len     = PASSWORD_SIZE;
	attr_char_value.init_len    = PASSWORD_SIZE;
	attr_char_value.p_value     = safe_sec_mngr_get_pw_ptr();

	//Add characteristic to the service
	err_code = sd_ble_gatts_characteristic_add(p_nss->service_handle,
			&char_md,
			&attr_char_value,
			&p_nss->auth_char_handle);

	APP_ERROR_CHECK(err_code);
}

static void ble_nss_pw_update_char_init(ble_nss_t *p_nss)
{
	uint32_t            err_code;
	ble_uuid_t          char_uuid;
	ble_uuid128_t       base_uuid = { BLE_SERVICE_BASE_UUID };

	char_uuid.uuid = BLE_NSS_PW_UPDATE_CHAR;

	err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
	APP_ERROR_CHECK(err_code);

	//Add read/write properties
	ble_gatts_char_md_t char_md;
	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.write = 1;
	char_md.char_props.read = 1;

	//Configure the attribute metadata
	ble_gatts_attr_md_t attr_md;
	memset(&attr_md, 0, sizeof(attr_md));

	attr_md.vloc = BLE_GATTS_VLOC_USER;

	//Set read/write security levels to our characteristic
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.wr_auth = 1;
	attr_md.rd_auth = 1;

	//Configure the characteristic value attribute
	ble_gatts_attr_t attr_char_value;
	memset(&attr_char_value, 0, sizeof(attr_char_value));

	attr_char_value.p_uuid      = &char_uuid;
	attr_char_value.p_attr_md   = &attr_md;

	//Set characteristic length in number of bytes and value
	attr_char_value.max_len     = PASSWORD_SIZE;
	attr_char_value.init_len    = PASSWORD_SIZE;
	attr_char_value.p_value     = safe_sec_mngr_get_pw_ptr();

	//Add characteristic to the service
	err_code = sd_ble_gatts_characteristic_add(p_nss->service_handle,
			&char_md,
			&attr_char_value,
			&p_nss->pw_update_char_handle);

	APP_ERROR_CHECK(err_code);
}

static void ble_nss_lock_char_init(ble_nss_t *p_nss)
{
	uint32_t            err_code;
	ble_uuid_t          char_uuid;
	ble_uuid128_t       base_uuid = { BLE_SERVICE_BASE_UUID };

	char_uuid.uuid = BLE_NSS_LOCK_UNLOCK_CHAR;

	err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
	APP_ERROR_CHECK(err_code);

	//Add read/write properties
	ble_gatts_char_md_t char_md;
	memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.write = 1;
	char_md.char_props.read = 1;

	//Configure the attribute metadata
	ble_gatts_attr_md_t attr_md;
	memset(&attr_md, 0, sizeof(attr_md));

	attr_md.vloc = BLE_GATTS_VLOC_USER;

	//Set read/write security levels to our characteristic
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
	attr_md.wr_auth = 1;

	//Configure the characteristic value attribute
	ble_gatts_attr_t attr_char_value;
	memset(&attr_char_value, 0, sizeof(attr_char_value));

	attr_char_value.p_uuid      = &char_uuid;
	attr_char_value.p_attr_md   = &attr_md;

	//Set characteristic length in number of bytes and value
	attr_char_value.max_len     = 1;
	attr_char_value.init_len    = 1;
	attr_char_value.p_value     = &cmd;

	//Add characteristic to the service
	err_code = sd_ble_gatts_characteristic_add(p_nss->service_handle,
			&char_md,
			&attr_char_value,
			&p_nss->lock_char_handle);

	APP_ERROR_CHECK(err_code);
}

