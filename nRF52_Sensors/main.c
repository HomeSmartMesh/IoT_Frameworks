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
#include "clocks.h"
#include "twi.h"
#include "mesh.h"


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

    // ------------------------- Start Init ------------------------- 
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    clocks_start();

    err_code = mesh_init();
    APP_ERROR_CHECK(err_code);

    twi_init(&m_twi);

    //only allow interrupts to start after init is done
    rtc_config();
    // ------------------------- Start App ------------------------- 

    NRF_LOG_INFO("____________________________");
    NRF_LOG_INFO("Hello from nRF52 Sensors");
    NRF_LOG_INFO("____________________________");


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

    mesh_tx_reset();
    //mesh_wait_tx();

    //system_off();

    // ------------------------- Start Events ------------------------- 

    while(true)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}
/*lint -restore */
