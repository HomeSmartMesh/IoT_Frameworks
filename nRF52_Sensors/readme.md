# nRF52 Sensor Tag
* Soc : nRF52832 QF AA
* BME280
* APDS9960

# Board PIO connections
* SCL       : P0_29
* SDA       : P0_30
* APDS_Int  : P0_31
* UART_Tx   : P0_06

# Prerequisites
* GNU Tools ARM Embedded version : 6 2017-q2-update

# Included dependencies
* nRF SDK 15.0.0_a53641a linked as a submodule and stripped down to the required content
* nRF SDK size reduced to ~ 2.8 MB instead of the original 693 MB and can thus easily be treated as an always loaded submodule

# uart
* main.c        : uart_init() -> APP_UART_FIFO_INIT(), app_uart_put(), app_uart_get()
* app_uart.h    : APP_UART_FIFO_INIT() -> app_uart_init()
* app_uart.c    : app_uart_init() -> nrf_drv_uart_init() "legacy\nrf_drv_uart.c"
* nrf_serial.c  : nrf_serial_init(),nrf_serial_write(),nrf_serial_read()
                  nrf_serial_init() -> nrf_drv_uart_init() "legacy\nrf_drv_uart.c"

# nrf logger module
* http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.0.0%2Fgroup__nrf__log.html&cp=4_0_0_6_11_16
## sdk config
* NRF_LOG + NRF_LOG_BACKEND_UART_XXX
* NRFX_UARTE_ENABLED 1, NRFX_UARTE0_ENABLED 0
* NRFX_UART_ENABLED 1, NRFX_UART0_ENABLED 0
* UART_ENABLED 1
* NRFX_PRS_ENABLED 1, NRFX_PRS_BOX_4_ENABLED 1
## user macros
* NRF_LOG_INSTANCE_REGISTER()
* NRF_LOG_INFO("Hi\r\n");   
* NRF_LOG_FLUSH(); must be called if not deferred
* timestamp only when using app_timer.h

# Tools

## Python jlink wrapper
* https://github.com/square/pylink
* http://pylink.readthedocs.io/en/latest/pylink.html
* have to be used with a 32 bit python version

## sdk_config GUI
```make conf```
* http://helmpcb.com/software/cmsis-configuration-wizard

