/** @file i2c.h
 *
 * @author Wassim FILALI
 *
 * 
 * @compiler IAR STM8
 *
 *
 * $Date: 04.01.2016
 * $Revision:
 *
 */

#include "commonTypes.h"

//------------------------------------------------------------------------------------------------------------------
//					User configuration
//------------------------------------------------------------------------------------------------------------------
#define Enable_Debug_I2C_IRQ 0


//------------------------------------------------------------------------------------------------------------------

//configuration application section, not to be edited
//---------------------------------------------------------------------------------------------
#if (Enable_Debug_I2C_IRQ == 1)
#define I2C_IRQ_Printf(x) 					UARTPrintf(x)
#define I2C_IRQ_PrintfHex(x)				UARTPrintfHex(x)
#define I2C_IRQ_PrintStatus(x)				nRF_PrintStatus(x)
#else
#define I2C_IRQ_Printf(x) 					(void)0
#define I2C_IRQ_PrintfHex(x)				(void)0
#define I2C_IRQ_PrintStatus(x)				(void)0
#endif

//---------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
//					I2C Registers
//------------------------------------------------------------------------------------------------------------------
//	I2C_CR1		Control Register 1
//	I2C_CR2		Control Register 2
//	I2C_FREQR	Frequency Register
//	I2C_OARL	Own Address Register LSB
//	I2C_OARH	Own Address Register MSB
//	I2C_DR		Data Register
//	I2C_SR1		Status Register 1
//	I2C_SR2		Status Register 2
//	I2C_SR3		Status Register 3
//	I2C_ITR		Interrupt Register
//	I2C_CCRL	Clock Control Register Low
//	I2C_CCRH	Clock Control Register High
//	I2C_TRISER	Tristate Enable register
//	funny the I2C_PECR is documented in STM8S103F3 specification while the Packet Error Checking is not part of the S family
//
//all reset to 0
//------------------------------------------------------------------------------------------------------------------

//
// STM8 I2C system.
// default is slave, goes to master on Start and back to slave on stop
// Addresses are 7/10 bits (one or two bytes), a Genral Call address can be enabled or disabled
// 9th bit is acknowledge from the slave
void I2C_Init();

void I2C_Transaction(BYTE read,BYTE slaveAddress, BYTE* buffer,BYTE count);

void I2C_Read(BYTE slaveAddress, BYTE* buffer,BYTE count);

void I2C_Write(BYTE slaveAddress, BYTE* buffer,BYTE count);

void i2c_user_Rx_Callback(BYTE *userdata,BYTE size);

void i2c_user_Tx_Callback(BYTE *userdata,BYTE size);


void i2c_user_Error_Callback(BYTE l_sr2);
