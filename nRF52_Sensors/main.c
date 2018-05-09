/**
 * Copyright (c) 2014 - 2018, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sdk_common.h"
#include "nrf.h"
#include "nrf_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_util.h"

//for the log
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// --------------------- inputs from sdk_config --------------------- 
// ---> TWI0_ENABLED ---> TWI1_ENABLED

//drivers
#include "bme280.h"
#include "max44009.h"
//apps
#include "twi.h"
#include "mesh.h"

#define RESET_MEMORY_TEST_BYTE  (0x0DUL)        /**< Known sequence written to a special register to check if this wake up is from System OFF. */
#define RAM_RETENTION_OFF       (0x00000003UL)  /**< The flag used to turn off RAM retention on nRF52. */

/*lint -save -esym(40, BUTTON_1) -esym(40, BUTTON_2) -esym(40, BUTTON_3) -esym(40, BUTTON_4) -esym(40, LED_1) -esym(40, LED_2) -esym(40, LED_3) -esym(40, LED_4) */

void system_off( void )
{
    #ifdef NRF51
        NRF_POWER->RAMON |= (POWER_RAMON_OFFRAM0_RAM0Off << POWER_RAMON_OFFRAM0_Pos) |
                            (POWER_RAMON_OFFRAM1_RAM1Off << POWER_RAMON_OFFRAM1_Pos);
    #endif //NRF51
    #ifdef NRF52
        NRF_POWER->RAM[0].POWER = RAM_RETENTION_OFF;
        NRF_POWER->RAM[1].POWER = RAM_RETENTION_OFF;
        NRF_POWER->RAM[2].POWER = RAM_RETENTION_OFF;
        NRF_POWER->RAM[3].POWER = RAM_RETENTION_OFF;
        NRF_POWER->RAM[4].POWER = RAM_RETENTION_OFF;
        NRF_POWER->RAM[5].POWER = RAM_RETENTION_OFF;
        NRF_POWER->RAM[6].POWER = RAM_RETENTION_OFF;
        NRF_POWER->RAM[7].POWER = RAM_RETENTION_OFF;
    #endif //NRF52

    // Turn off LEDs before sleeping to conserve energy.
    bsp_board_leds_off();

    // Set nRF5 into System OFF. Reading out value and looping after setting the register
    // to guarantee System OFF in nRF52.
    NRF_POWER->SYSTEMOFF = 0x1;
    (void) NRF_POWER->SYSTEMOFF;
    while (true);
}


void clocks_start( void )
{
    // Start HFCLK and wait for it to start.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

void recover_state()
{
    uint32_t            loop_count = 0;
    if ((NRF_POWER->GPREGRET >> 4) == RESET_MEMORY_TEST_BYTE)
    {
        // Take the loop_count value.
        loop_count          = (uint8_t)(NRF_POWER->GPREGRET & 0xFUL);
        NRF_POWER->GPREGRET = 0;
    }

    loop_count++;
    NRF_POWER->GPREGRET = ( (RESET_MEMORY_TEST_BYTE << 4) | loop_count);

}

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

void bme_measures_log()
{
    uint32_t err_code;
    err_code = bme280_init(&m_twi);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("bme280_init() done");

    bme280_measure();
    float temperature = (float)bme280_get_temperature();
    NRF_LOG_INFO("temperature = "NRF_LOG_FLOAT_MARKER,NRF_LOG_FLOAT(temperature/100));
    float humidity = (float)bme280_get_humidity();
    NRF_LOG_INFO("humidity = "NRF_LOG_FLOAT_MARKER,NRF_LOG_FLOAT(humidity/1024));
    float pressure = bme280_get_pressure();
    NRF_LOG_INFO("pressure = "NRF_LOG_FLOAT_MARKER,NRF_LOG_FLOAT(pressure/(256*100)));
    NRF_LOG_RAW_INFO("\r");
}

int main(void)
{
    uint32_t err_code;

    clocks_start();

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    err_code = mesh_init();
    APP_ERROR_CHECK(err_code);


    // Recover state if the device was woken from System OFF.
    recover_state();

    NRF_LOG_INFO("____________________________");
    NRF_LOG_INFO("Hello from nRF52 Sensors");
    NRF_LOG_INFO("____________________________");

    twi_init(&m_twi);

    //twi_scan();

    //max44009_test();

    for(int i=0;i<10;i++)
    {
        uint32_t light = max44009_read_light(&m_twi);
        float light_f = light;
        NRF_LOG_INFO("light = "NRF_LOG_FLOAT_MARKER,NRF_LOG_FLOAT(light_f/1000));
        nrf_delay_ms(1000);
    }

    //bme_measures_log();


    // Check state of all buttons and send an esb packet with the button press if there is exactly one.
    //err_code = gpio_check_and_esb_tx();
    //err_code = esb_tx_alive();
    mesh_tx_button(1);//down - active

    nrf_delay_ms(200);
    // Wait for esb completed and all buttons released before going to system off.
    //reset the check
    mesh_tx_button(0);//down - passive
    mesh_wait_tx();

    system_off();

    while(true);
}
/*lint -restore */
