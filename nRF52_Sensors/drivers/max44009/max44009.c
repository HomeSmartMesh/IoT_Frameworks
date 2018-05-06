/** @file max44009.c
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 29.10.2016 - creation out of refactoring
 * $Revision: 1 
 *
*/

#include "max44009.h"

#define NRF_LOG_MODULE_NAME max

#if (MAX_CONFIG_LOG_ENABLED == 1)
#define NRF_LOG_LEVEL MAX_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR MAX_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR MAX_CONFIG_DEBUG_COLOR
#else //MAX_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL 0
#endif //MAX_CONFIG_LOG_ENABLED

#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();



static const uint8_t address = 0x4A;
static const nrf_drv_twi_t *p_twi = NULL;

//The MAX44009 does not support auto reg address increment
//reg address have to be sent for every byte either with I²C stop or with a new transaction
uint8_t max44009_read_reg(uint8_t reg)
{
  uint8_t res;
  ret_code_t err_code;
  err_code = nrf_drv_twi_tx(p_twi, address, &reg, 1,true);
  APP_ERROR_CHECK(err_code);
  err_code = nrf_drv_twi_rx(p_twi, address, &res, 1);
  APP_ERROR_CHECK(err_code);
  //NRF_LOG_DEBUG("@ 0x%02x => 0x%02x", reg,res);

	return res;
}


uint16_t max44009_read_light(nrf_drv_twi_t *l_twi)
{
    p_twi = l_twi;
    uint16_t Val = max44009_read_reg(0x03);
    Val <<= 4;//shift to make place for the 4 LSB
    Val         += 0x0F & max44009_read_reg(0x04);
	return Val;
}

