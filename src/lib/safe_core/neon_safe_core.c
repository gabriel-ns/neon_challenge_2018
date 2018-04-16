/*
 * neon_safe_core.c
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */

#include "app_config.h"

#include "nrf_gpio.h"
#include "app_timer.h"

#include "lock_controller.h"
#include "safe_internal_protocol.h"

#define CLIENT_AUTH_PERIOD	5000
#define CLIENT_CONN_PERIOD	1000
#define CLIENT_DUTY 		500

APP_TIMER_DEF(client_notification_timer);
APP_TIMER_DEF(lock_notification_timer);

static bool is_cli_led_on = false;
static void neon_safe_open();
static void neon_safe_close();

static void client_timer_timeout(void *p_context);
static void lock_timer_timeout(void *p_context);
static void on_disconnect_event();
static void on_auth_event();
static void on_open_event();
static void on_connect_event();
static void on_auth_event();

void neon_safe_core_init()
{
	uint32_t err_code;

	lock_controller_init();

	nrf_gpio_cfg_output(CLIENT_NTF_PIN);
	nrf_gpio_pin_clear(CLIENT_NTF_PIN);
	is_cli_led_on = false;

	nrf_gpio_cfg_output(LOCK_NTF_PIN);
	nrf_gpio_pin_clear(LOCK_NTF_PIN);

	err_code = app_timer_init(&client_notification_timer, APP_TIMER_MODE_SINGLE_SHOT, client_timer_timeout);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_init(&lock_notification_timer, APP_TIMER_MODE_REPEATED, lock_timer_timeout);
	APP_ERROR_CHECK(err_code);
}

void neon_safe_core_handler(safe_msg_t *msg)
{
	switch(msg->type)
	{
	case SAFE_CMD_OPEN:
		neon_safe_open();
		break;

	case SAFE_CMD_CLOSE:
		neon_safe_close();
		break;

	case SAFE_EVT_AUTHENTICATION_SUCCESS:
		on_auth_event();
		break;

	case SAFE_EVT_CLIENT_CONNECTED:
		on_connect_event();
		break;

	case SAFE_EVT_CLIENT_DISCONNECTED:
		on_disconnect_event();
		break;

	default:
		break;
	}
}

static void neon_safe_open()
{
	if(safe_sec_mngr_is_authorized())
	{
		on_open_event();
		lock_controller_unlock();
	}
}

static void neon_safe_close()
{
	on_close_event();
	lock_controller_lock();
}

static void on_open_event()
{
	uint32_t err_code;
	nrf_gpio_pin_set(LOCK_NTF_PIN);
	err_code = app_timer_start(lock_notification_timer, APP_TIMER_TICKS(500, APP_TIMER_PRESCALER), on_open_event);
	APP_ERROR_CHECK(err_code);
}

static void on_close_event()
{
	uint32_t err_code;
	nrf_gpio_pin_clear(LOCK_NTF_PIN);
	err_code = app_timer_stop(lock_notification_timer);
	APP_ERROR_CHECK(err_code);
}

static void on_disconnect_event()
{
	app_timer_stop(client_notification_timer);
	nrf_gpio_pin_clear(CLIENT_NTF_PIN);
}

static void on_connect_event()
{
	uint32_t err_code;
	is_cli_led_on = true;
	nrf_gpio_pin_set(CLIENT_NTF_PIN);
	err_code = app_timer_start(client_notification_timer, APP_TIMER_TICKS(CLIENT_DUTY, APP_TIMER_PRESCALER), on_connect_event);
	APP_ERROR_CHECK(err_code);
}

static void on_auth_event()
{
	uint32_t err_code;
	app_timer_stop(client_notification_timer);

	is_cli_led_on = true;
	nrf_gpio_pin_set(CLIENT_NTF_PIN);
	err_code = app_timer_start(client_notification_timer, APP_TIMER_TICKS(CLIENT_DUTY, APP_TIMER_PRESCALER), on_auth_event);
	APP_ERROR_CHECK(err_code);
}

static void client_timer_timeout(void *p_context)
{
	uint32_t reload = CLIENT_CONN_PERIOD;
	if(is_cli_led_on)
	{
		nrf_gpio_pin_clear(CLIENT_NTF_PIN);
		is_cli_led_on = false;
		if(p_context == on_auth_event) reload = CLIENT_AUTH_PERIOD;
	}
	else
	{
		nrf_gpio_pin_set(CLIENT_NTF_PIN);
		is_cli_led_on = true;
		reload = CLIENT_DUTY;
	}

	uint32_t err_code;
	err_code = app_timer_start(client_notification_timer, APP_TIMER_TICKS(reload, APP_TIMER_PRESCALER), p_context);
	APP_ERROR_CHECK(err_code);
}

static void lock_timer_timeout(void *p_context)
{
	nrf_gpio_pin_toggle(LOCK_NTF_PIN);
}
