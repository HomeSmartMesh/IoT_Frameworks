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
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

#include "mpu6050_app.h"
#include "bmp180_app.h"
#include "ap3216c.h"

#define NodeId         NRF_UICR->CUSTOMER[0]
#define RF_CHANNEL     NRF_UICR->CUSTOMER[1]
#define SLEEP_SEC      NRF_UICR->CUSTOMER[2]


#define Mesh_Pid_Alive 0x05
#define Mesh_Pid_Reset 0x04

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

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x01, 0x00);
static volatile bool esb_completed = false;

void blink_green()
{
    nrf_gpio_pin_write(LED_RGB_GREEN, 0 );
    nrf_delay_ms(1);
    nrf_gpio_pin_write(LED_RGB_GREEN, 1 );
}

void blink_blue()
{
    nrf_gpio_pin_write(LED_RGB_BLUE, 0 );
    nrf_delay_ms(1);
    nrf_gpio_pin_write(LED_RGB_BLUE, 1 );
}

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
            break;
    }

    esb_completed = true;
}


void mesh_wait_tx()
{
    while(!esb_completed);
}
//
void mesh_tx_data(uint8_t pid,uint8_t * data,uint8_t size)
{
    esb_completed = false;//reset the check

    tx_payload.length   = 2 + size;//length,ctrl payload(pid,source,rf_payload)(crc not included in length)
    tx_payload.control = 0x80 | 2;// broadcast | ttl = 2
    tx_payload.noack    = true;//it is a broadcast
    tx_payload.pipe     = 0;

    tx_payload.data[0] = pid;
    
    tx_payload.data[1] = NodeId;//source

    for(int i=0;i<size;i++)
    {
        tx_payload.data[i+2]     = data[i];
    }
    
    tx_payload.noack = true;
    nrf_esb_write_payload(&tx_payload);

    //wait till the transmission is complete
    while(!esb_completed);
}

void mesh_tx_pid(uint8_t pid)
{
    esb_completed = false;//reset the check

    tx_payload.length   = 2;//payload + header (crc length not included)
    tx_payload.control = 0x80 | 2;// broadcast | ttl = 2
    tx_payload.noack    = true;//it is a broadcast
    tx_payload.pipe     = 0;

    tx_payload.data[0] = pid;
    
    tx_payload.data[1] = NodeId;//source
    
    tx_payload.noack = true;
    nrf_esb_write_payload(&tx_payload);

    //wait till the transmission is complete
    while(!esb_completed);
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

    err_code = nrf_esb_set_rf_channel(RF_CHANNEL);
    VERIFY_SUCCESS(err_code);

//    err_code = nrf_esb_set_tx_power(RADIO_TXPOWER_TXPOWER_Neg4dBm);
//    VERIFY_SUCCESS(err_code);

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
    ret_code_t err_code;

    err_code = nrf_drv_clock_init();
    if(err_code)
    {
        DEBUG_PRINTF("nrf_drv_clock_init() fail\r\n");
    }

    nrf_drv_clock_hfclk_request(NULL);
    while(!nrf_drv_clock_hfclk_is_running());
    
}


void send_accell()
{
    //mpu_wakeup();
    uint8_t accell_data[6];
    mpu_get_accell_data(accell_data);
    DEBUG_PRINTF("xh(%d)\r\n",accell_data[0]);
    //DEBUG_PRINTF("x(%d) y(%d) z(%d)\r\n",x,y,z);
    //nrf_delay_ms(1000);
    //mpu_sleep();
    mesh_tx_data(0x13,accell_data,6);//sends and waits tx
}

void send_temperature()
{
    uint8_t data[2];
    bmp_get_temperature(data);
    mesh_tx_data(0x08,data,4);//sends and waits tx
}

void send_pressure()
{
    uint8_t data[2];
    bmp_get_pressure(data);
    mesh_tx_data(0x12,data,4);//sends and waits tx
}

void send_light()
{
    uint8_t data[2];
    ap_get_light(data);
    mesh_tx_data(0x14,data,2);//0x14 new light
}

uint32_t mesh_tx_alive()
{
    static uint32_t live_count = 0;

    uint8_t data[5];
    data[0] = 0xFF & (uint8_t)(live_count >> 24);
    data[1] = 0xFF & (uint8_t)(live_count >> 16);
    data[2] = 0xFF & (uint8_t)(live_count >> 8);
    data[3] = 0xFF & (uint8_t)(live_count );
    data[4] = NRF_RADIO->TXPOWER;

    mesh_tx_data(Mesh_Pid_Alive,data,5);
    
    return live_count++;//returns the first used value before the increment
}

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        nrf_drv_rtc_counter_clear(&rtc);
        nrf_drv_rtc_int_enable(&rtc, NRF_RTC_INT_COMPARE0_MASK);
        DEBUG_PRINTF("rtc_handler(COMPARE)\r\n");

        mesh_tx_alive();
        blink_green();
        mesh_wait_tx();

    }
}
/** @brief Function initialization and configuration of RTC driver instance.
 */
static void rtc_config(void)
{
    uint32_t err_code;

    //Initialize RTC instance
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    config.prescaler = 4095;
    err_code = nrf_drv_rtc_init(&rtc, &config, rtc_handler);
    if(err_code)
    {
        DEBUG_PRINTF("nrf_drv_rtc_init() fail(%d)\r\n",err_code);
    }

    //Enable tick event & interrupt
    //nrf_drv_rtc_tick_enable(&rtc,true);

    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
    err_code = nrf_drv_rtc_cc_set(&rtc,0,SLEEP_SEC * 8,true);
    if(err_code)
    {
        DEBUG_PRINTF("nrf_drv_rtc_cc_set() fail(%d)\r\n",err_code);
    }

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc);
}

void init()
{
    // Initialize
    clocks_start();

    uint32_t err_code = esb_init();
    APP_ERROR_CHECK(err_code);
    gpio_init();

    nrf_drv_clock_lfclk_request(NULL);

    nrf_gpio_pin_write(LED_RGB_BLUE, 0 );
    nrf_delay_ms(60);
    nrf_gpio_pin_write(LED_RGB_BLUE, 1 );
    nrf_delay_ms(60);

    //only allow interrupts to start after init is done
    rtc_config();

    DEBUG_PRINTF("=> Hello Debug nRF51 sensors Sleep\r\n");

}

int main(void)
{
    init();

    DEBUG_PRINTF("=> Hello Debug nRF51 sensors Sleep\r\n");

    mesh_tx_pid(Mesh_Pid_Reset);

    while(true)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}
