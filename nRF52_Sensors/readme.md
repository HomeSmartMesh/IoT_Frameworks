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
