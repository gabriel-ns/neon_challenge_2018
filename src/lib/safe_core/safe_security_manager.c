/*
 * safe_security_manager.c
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "app_config.h"
#include "safe_internal_protocol.h"
#include "neon_safe_core.h"
#include "safe_security_manager.h"

static sec_manager_rsp_t safe_sec_mngr_authenticate(safe_msg_t* msg);
static void safe_sec_mngr_set_password(safe_msg_t* msg);

static bool    is_access_allowed = false;
static uint8_t retries_count = 0;

static uint8_t curr_password[PASSWORD_SIZE];

void safe_sec_mngr_init()
{
	/* start with 0000000000000000 password */
	memset(curr_password, 0x00, PASSWORD_SIZE);

	neon_safe_core_init();
}

sec_manager_rsp_t safe_sec_mngr_send_cmd(safe_msg_t* msg)
{

	switch(msg->type)
	{
	case SAFE_EVT_AUTHENTICATION_TRY:
		return safe_sec_mngr_authenticate(msg);

	case SAFE_CMD_SET_PASSWORD:
		if(is_access_allowed)
		{
			safe_sec_mngr_set_password(msg);
		}
		else
		{
			return SECURITY_DENIED;
		}
		break;

	/* Events are always dispatched */
	case SAFE_EVT_AUTHENTICATION_SUCCESS:
	case SAFE_EVT_CLIENT_CONNECTED:
	case SAFE_EVT_CLIENT_DISCONNECTED:
		neon_safe_core_handler(msg);
		break;

	/* commands need authorization */
	case SAFE_CMD_OPEN:
	case SAFE_CMD_CLOSE:
		if(is_access_allowed)
		{
			neon_safe_core_handler(msg);
		}
		else
		{
			return SECURITY_DENIED;
		}
		break;

	default:
	}

	return SECURITY_DENIED;
}

bool safe_sec_mngr_is_authorized()
{
	return is_access_allowed;
}

static sec_manager_rsp_t safe_sec_mngr_authenticate(safe_msg_t* msg)
{
	if(memcmp(curr_password, msg->auth_try.password, PASSWORD_SIZE) == 0)
	{
		retries_count = 0;
		is_access_allowed = true;

		safe_msg_t msg;
		msg.type = SAFE_EVT_AUTHENTICATION_SUCCESS;
		safe_sec_mngr_send_cmd(&msg);
		return SECURITY_SUCCESS;
	}
	else
	{
		retries_count++;
		if(retries_count > MAX_PASSWORD_TRIES)
		{
			//TODO bluetooth disconnect
		}
		return SECURITY_DENIED;
	}
}

static void safe_sec_mngr_set_password(safe_msg_t* msg)
{
	memcpy(curr_password, msg->new_pw.password, PASSWORD_SIZE);
}

uint8_t* safe_sec_mngr_get_pw_ptr()
{
	return curr_password;
}

