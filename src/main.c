#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "nrf51.h"
#include "app_config.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "app_timer_appsh.h"
#include "app_scheduler.h"
#include "app_error.h"

#include "softdevice_handler.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "safe_internal_protocol.h"
#include "safe_security_manager.h"
#include "neon_safe_bluetooth.h"


/**
 * @brief Function for doing power management.
 */
static void power_manage(void);

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Start the application scheduler
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    // Start the application timer
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, app_timer_evt_schedule);

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    NRF_LOG_INFO("nrf51 test\n");
    NRF_LOG_FLUSH();

    // Set the clock configuration
    nrf_clock_lf_cfg_t lf_clock_config;
    lf_clock_config.source = NRF_CLOCK_LF_SRC_XTAL;
    lf_clock_config.rc_ctiv = 0;
    lf_clock_config.rc_temp_ctiv = 0;
    lf_clock_config.xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM;

    // Initialize the SoftDevice handler module.
     SOFTDEVICE_HANDLER_INIT(&lf_clock_config, NULL);
    NRF_LOG_INFO("Initializing FW\n");
    NRF_LOG_FLUSH();

    bluetooth_init();
    safe_sec_mngr_init();

    // Enter main loop.
    for (;;)
    {
        app_sched_execute();
        power_manage();
        NRF_LOG_FLUSH();
    }
}

static void power_manage(void)
{
     ret_code_t err_code = sd_app_evt_wait();
     APP_ERROR_CHECK(err_code);
}

