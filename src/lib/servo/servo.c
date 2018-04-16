/*
 * servo.c
 *
 *  Created on: Apr 15, 2018
 *      Author: gabriel
 */
#include <stdint.h>
#include "app_config.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "servo.h"

#define SERVO_CYCLE_US	20000
#define APP_TIMER_TICKS_US(US, PRESCALER)\
            ((uint32_t)ROUNDED_DIV((US) * (uint64_t)APP_TIMER_CLOCK_FREQ, ((PRESCALER) + 1) * 1000000))


static void cycle_timeout_handler(void *p_context);
static void servo_timeout_handler(void *p_context);

APP_TIMER_DEF(cycle_timer);
APP_TIMER_DEF(pulse_timer);

bool m_state;
uint32_t m_pin;
uint8_t m_angle;

void servo_init(uint32_t servo_pin)
{
	uint32_t err_code;
	// set gpio as output
	m_pin = servo_pin;
	nrf_gpio_cfg_output(m_pin);
	nrf_gpio_pin_clear(m_pin);
	m_state = false;

	//start timer
	err_code = app_timer_create(&cycle_timer, APP_TIMER_MODE_REPEATED, cycle_timeout_handler);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_create(&pulse_timer, APP_TIMER_MODE_SINGLE_SHOT, servo_timeout_handler);
	APP_ERROR_CHECK(err_code);


	servo_set_angle(0);
	app_timer_start(cycle_timer, APP_TIMER_TICKS(20, APP_TIMER_PRESCALER), NULL);

}

void servo_set_angle(uint8_t angle)
{
	if (angle <= 180)
	{
		m_angle = angle;
	}
}

static void cycle_timeout_handler(void *p_context)
{
	nrf_gpio_pin_set(m_pin);
	uint32_t timer_restart_value = 1000 + ((m_angle * 1000) / 180);
	app_timer_start(pulse_timer, APP_TIMER_TICKS_US(timer_restart_value, APP_TIMER_PRESCALER), NULL);

}

static void servo_timeout_handler(void *p_context)
{
	nrf_gpio_pin_clear(m_pin);
}

