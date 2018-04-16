/*
 * safe_security_manager.h
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */

#ifndef SRC_LIB_SAFE_CORE_INC_SAFE_SECURITY_MANAGER_H_
#define SRC_LIB_SAFE_CORE_INC_SAFE_SECURITY_MANAGER_H_

typedef enum sec_manager_rsp
{
	SECURITY_SUCCESS,
	SECURITY_DENIED
}sec_manager_rsp_t;

void safe_sec_mngr_init();

sec_manager_rsp_t safe_sec_mngr_send_cmd(safe_msg_t msg);

bool safe_sec_mngr_is_authorized();


#endif /* SRC_LIB_SAFE_CORE_INC_SAFE_SECURITY_MANAGER_H_ */
