/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
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

#include "nrf_esb.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sdk_common.h"
#include "nrf.h"
#include "nrf_error.h"
#include "nrf_esb_error_codes.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_util.h"

#include "mpu6050_app.h"

#ifdef NRF_LOG_USES_RTT
#include "SEGGER_RTT.h"
#define DEBUG_PRINTF(...)           SEGGER_RTT_printf(0, __VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif


#define RESET_MEMORY_TEST_BYTE  (0x0DUL)        /**< Known sequence written to a special register to check if this wake up is from System OFF. */

#define BTN_PRESSED     0                       /**< Value of a pressed button. */
#define BTN_RELEASED    1                       /**< Value of a released button. */

/*lint -save -esym(40, BUTTON_1) -esym(40, BUTTON_2) -esym(40, BUTTON_3) -esym(40, BUTTON_4) -esym(40, LED_1) -esym(40, LED_2) -esym(40, LED_3) -esym(40, LED_4) */

static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x01, 0x00);
static nrf_esb_payload_t rx_payload;
static uint32_t button_state_1;
static volatile bool esb_completed = false;


void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            (void) nrf_esb_flush_tx();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            // Get the most recent element from the RX FIFO.
            while (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS) ;

            // For each LED, set it as indicated in the rx_payload, but invert it for the button
            // which is pressed. This is because the ack payload from the PRX is reflecting the
            // state from before receiving the packet.
            nrf_gpio_pin_write(LED_RGB_GREEN, !( ((rx_payload.data[0] & 0x01) == 0) ^ (button_state_1 == BTN_PRESSED)) );
            break;
    }

    esb_completed = true;
}

uint32_t esb_tx_button(uint8_t state)
{
    uint32_t err_code;
    tx_payload.length   = 3;//payload + header (crc length not included)
    tx_payload.control = 0x80 | 2;// broadcast | ttl = 2
    tx_payload.noack    = true;//it is a broadcast
    tx_payload.pipe     = 0;
    
    tx_payload.data[0] = 0x06;//pid
    tx_payload.data[1] = 45;//source - on_off_tag
    tx_payload.data[2] = state;//Up or Down
    
    tx_payload.noack = true;
    err_code = nrf_esb_write_payload(&tx_payload);
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}

uint32_t esb_tx_alive()
{
    uint32_t err_code;
    tx_payload.length   = 1;//payload + header (crc length not included)
    tx_payload.control = 0xF5;// broadcast | 0x75
    tx_payload.noack    = true;//it is a broadcast
    tx_payload.pipe     = 0;
    
    tx_payload.data[0] = 0x15;//source
    
    tx_payload.noack = true;
    err_code = nrf_esb_write_payload(&tx_payload);
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}

uint32_t esb_init( void )
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

    nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;

    nrf_esb_config.retransmit_count         = 0;
    nrf_esb_config.selective_auto_ack       = true;//payload.noack  decides
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
    nrf_esb_config.payload_length           = 8;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
    nrf_esb_config.event_handler            = nrf_esb_event_handler;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;
    nrf_esb_config.crc                      = NRF_ESB_CRC_16BIT;

    err_code = nrf_esb_init(&nrf_esb_config);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_0(base_addr_0);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_1(base_addr_1);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_prefixes(addr_prefix, 8);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_rf_channel(2);
    VERIFY_SUCCESS(err_code);

    tx_payload.length  = 8;
    tx_payload.pipe    = 0;
    tx_payload.data[0] = 0x00;

    return NRF_SUCCESS;
}


void gpio_init( void )
{
    nrf_gpio_cfg_sense_input(BUTTON_1, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);

    bsp_board_leds_init();
}

void system_off( void )
{
        NRF_POWER->RAMON |= (POWER_RAMON_OFFRAM0_RAM0Off << POWER_RAMON_OFFRAM0_Pos) |
                            (POWER_RAMON_OFFRAM1_RAM1Off << POWER_RAMON_OFFRAM1_Pos);
    // Turn off LEDs before sleeping to conserve energy.
    bsp_board_leds_off();

    // Set nRF5 into System OFF. Reading out value and looping after setting the register
    // to guarantee System OFF in nRF52.
    NRF_POWER->SYSTEMOFF = 0x1;
    (void) NRF_POWER->SYSTEMOFF;
    while (true);
}


//power up = on chip reset = (power on reset, brown out reset)
bool is_power_up_reset()
{
    bool is_on_chip = (NRF_POWER->RESETREAS == 0);
    if(!is_on_chip)
    {
        //clear the cumulated reset reasons
        NRF_POWER->RESETREAS = 0x03000F;//All acknowledged
    }
    return is_on_chip;
}

void clocks_start( void )
{
    // Start HFCLK and wait for it to start.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

void power_up_init()
{
    mpu_start();
    
    nrf_delay_ms(300);
    nrf_gpio_pin_write(LED_RGB_RED, 0 );
    nrf_delay_ms(300);
    nrf_gpio_pin_write(LED_RGB_RED, 10 );
}

void wakeup_init()
{
    // Initialize
    clocks_start();
    uint32_t err_code = esb_init();
    APP_ERROR_CHECK(err_code);
    gpio_init();

    nrf_gpio_pin_write(LED_RGB_BLUE, 0 );
    nrf_delay_ms(50);
    nrf_gpio_pin_write(LED_RGB_BLUE, 1 );
}

int main(void)
{
    wakeup_init();

    if(is_power_up_reset())
    {
        power_up_init();
        DEBUG_PRINTF("=> Power Up - Hello Debug nRF51 sensors\r\n");
    }
    else
    {
        DEBUG_PRINTF("=> Just woke up - Hello\r\n");
    }

    int8_t x,y,z;
    for(int i=0;i<10;i++)
    {
        mpu_get_accell(&x,&y,&z);
        DEBUG_PRINTF("x(%d) y(%d) z(%d)\r\n",x,y,z);
        nrf_delay_ms(1000);
    }
    mpu_sleep();

    // Wait for esb completed and all buttons released before going to system off.
    esb_completed = false;//reset the check
    esb_tx_button(0);//down - passive

    while(!esb_completed);
    system_off();

    while(true);
}
/*lint -restore */
