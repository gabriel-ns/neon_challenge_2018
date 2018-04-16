/*
 * lock_controller.c
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */

#include "app_config.h"

#include "servo.h"
#include "lock_controller.h"

void lock_controller_init()
{
	servo_init(LOCK_DEFAULT_PIN);
	lock_controller_lock();
}

void lock_controller_lock()
{
	servo_set_angle(0);
}

void lock_controller_unlock()
{
	servo_set_angle(180);
}
