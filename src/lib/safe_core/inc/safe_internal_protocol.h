/*
 * safe_internal_protocol.h
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */

#ifndef SRC_LIB_SAFE_CORE_INC_SAFE_INTERNAL_PROTOCOL_H_
#define SRC_LIB_SAFE_CORE_INC_SAFE_INTERNAL_PROTOCOL_H_

typedef enum safe_protocol
{
	SAFE_CMD_OPEN = 0,
	SAFE_CMD_CLOSE,
	SAFE_CMD_SET_PASSWORD,
	SAFE_EVT_AUTHENTICATION_TRY,
	SAFE_EVT_AUTHENTICATION_SUCCESS,
	SAFE_EVT_CLIENT_CONNECTED,
	SAFE_EVT_CLIENT_DISCONNECTED
}safe_protocol_t;

typedef struct safe_password
{
	uint8_t password[8];
}safe_password_t;

typedef struct safe_msg
{
	safe_protocol_t type;
	union
	{
		safe_password_t auth_try;
		safe_password_t new_pw;
	};
}safe_msg_t;


#endif /* SRC_LIB_SAFE_CORE_INC_SAFE_INTERNAL_PROTOCOL_H_ */
