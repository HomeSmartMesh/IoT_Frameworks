/** @file i2c.c
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

#include "i2c_m.h"
#include "ClockUartLed.h"

#include <iostm8s103f3.h>


struct i2c_s {
        BYTE    Step;
	BYTE	SlaveAddress;
	BYTE*	masterBuffer;
	BYTE	masterTransactionLength;
	BYTE	buffer_index;
	BYTE    reg;
	BYTE	masterMode;
	BYTE	readwrite;
	BYTE*	slaveBuffer;
	BYTE	slaveTransactionLength;
	
}i2c;

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
void I2C_Init()
{

    PB_DDR_DDR4 = 0;// 0: Input - 1: Output
    PB_CR1_C14 = 0;// Input mode: 0: Floating input - 1: Input with pull-up // Output mode: 0: Pseudo open drain 1: Push-pull
    PB_DDR_DDR5 = 0;// 0: Input - 1: Output
    PB_CR1_C15 = 0;// Input mode: 0: Floating input - 1: Input with pull-up // Output mode: 0: Pseudo open drain 1: Push-pull
    //PB_CR2_bit.C25 = 0;// Input mode:  0: External interrupt disabled - 1: External interrupt enabled // Output mode: 0: Output speed up to  2 MHz - 1: Output speed up to 10 MHz
	//I2C_Init_Pins();//B5 SDA, B4 SCL
	
	I2C_CR1 = 0;				// clearing (PE) if this is a re - init, this does not stop the ongoing communication
								// CR1_NOSTRETCH	Clock Strtching enabled
								// CR1_ENGC 		General call disabled
								// CR2_POS 			ACK controls the current byte
	I2C_FREQR = 16;             // clk at least 1 MHz for Standard and 4MHz for Fast
	I2C_CCRH_F_S = 0;           // I2C running is standard mode.
	//I2C_CCRL = 0x50;            // I2C period = 2 * CCR * tMASTER 100KHz : tabe 50 RM0016 P 315
	//I2C_CCRH = 0x00;			// CCR[11:8] = 0
	I2C_CCRL = 0xA0;            // I2C period = 2 * CCR * tMASTER 100KHz : tabe 50 RM0016 P 315
	I2C_CCRH = 0x00;			// CCR[11:8] = 0
								// I2C_CCRH_F_S : Standard mode, DUTY unused in standard mode

	I2C_OARH_ADDMODE = 0;               // 7-bit slave address
	I2C_OARH_ADDCONF = 1;               // This bit must be set by software
										// ADD[9:8] unused

	I2C_TRISER = 17;			//Maximum time used by the feedback loop to keep SCL Freq stable whatever SCL rising time is
								//Standard mode max rise time is 1000ns
								//example for 8MHz : (1000ns / 125 ns = 8 ) + 1 = 9
								//for 16 MHz : (1000 ns / 62.5 ns = 16 ) + 1 = 17

	// ------------------------ Interrupts are enabled ------------------------ 
	I2C_ITR_ITEVTEN = 1;                //Event  Enables 				: SB, ADDR, ADD10, STOPF, BTF, WUFH
	I2C_ITR_ITBUFEN = 1;                //Buffer Enables (if ITEVTEN) 	: RXNE, TXE
	I2C_ITR_ITERREN = 1;				//Error  Enables				: BERR, ARLO, AF, OVR

	//i2c.slaveTransactionLength = 0;			//if not configured, no data will be used and no read/write acknowledge
	
	
	I2C_CR1_PE = 1;						//Enable the I2C Peripheral
}

void I2C_Transaction(BYTE read,BYTE slaveAddress, BYTE* buffer,BYTE count)
{
	i2c.readwrite = read;
	i2c.SlaveAddress = slaveAddress;
	i2c.buffer_index = 0;
	i2c.masterBuffer = buffer;
	i2c.masterTransactionLength = count;
	i2c.masterMode = 1;
        i2c.Step = 0;
	
	//wait for the Bus to get Free to avoid collisions
	while(I2C_SR3_BUSY);
	
	I2C_CR2_ACK = 1;	//Acknowledge Enable : Acknowledge returned after a byte is received (matched address or data)
	//The start will enter the Master Mode (when the Busy bit is cleared)
	//If already in Master Mode, then ReStart will be generated at the end of the current transfer
	I2C_CR2_START = 1;	//Launch the process
}

void I2C_Read(BYTE slaveAddress, BYTE* buffer,BYTE count)
{
	I2C_Transaction(0x01,slaveAddress,buffer,count);
}

void I2C_Write(BYTE slaveAddress, BYTE* buffer,BYTE count)
{
	I2C_Transaction(0x00,slaveAddress,buffer,count);
}


//SR1: 	TXE(Tx Empty) RXNE(Rx Not Empty) STOPF(Stop detection, slave mode) 
//		ADD10(10bit header sent, master mode) BTF(Byte Transfer Ffinished) 
//		ADDR(Address sent, master mode / matched in slave mode) SB(Start bit, master mode)

//SR2: 	WUFH(wake up from halt, slave/master) OVR(Overrun underrun) AF(Acknowledge Failure)
//		ARLO(Arbitration lost, master mode) BERR(Bus Error misplaced start or stop)

//SR3: 	DUALF(reserved ?) GENCALL(General call if used) TRA(Transmitted not received as of R/W address bit)
//		BUSY(Bus busy updated even if PE=0) MSL(Master mode set after Start Bit, cleared after Stop or on Arbitration Lost)
#pragma vector = I2C_TXE_vector
//#pragma vector = I2C_RXNE_vector	// all have same vector
__interrupt void I2C_IRQ()
{
	if(i2c.masterMode)				//using soft flag not MSL because MSL is 0 when the master receives the last RxNE data that is 0
	{
		if (I2C_SR1_SB)			//(SB) The Start Byte has been sent
		{
			i2c.reg = I2C_SR1;									//clear the start condition by reading SR1 then writing DR
			I2C_DR = (i2c.SlaveAddress << 1) | i2c.readwrite;	//the slave address - read or write is set here
                        i2c.Step = 1;
		}
		else if (I2C_SR1_ADDR)		//(ADDR) The Slave Address Has been sent
		{
			i2c.reg = I2C_SR1;			//clearing status registers
			i2c.reg = I2C_SR3;			//clearing status registers
			i2c.buffer_index = 0;		//init the counter
                        i2c.Step = 2;
		}
		else if(I2C_SR3_TRA)			//(TRA) we are writing to the slave
		{
			if (I2C_SR1_TXE)		//(TXE) Data Register Empty
			{
				
				I2C_DR = i2c.masterBuffer[i2c.buffer_index++];
                                i2c.Step = 3;
				if (i2c.buffer_index == i2c.masterTransactionLength)
				{
					I2C_CR2_STOP = 1;	//Generate Stop condition
                                        i2c.Step = 4;
					i2c_user_Tx_Callback(i2c.masterBuffer,i2c.masterTransactionLength);//Notify the user
				}
			}
			else if(!I2C_SR1_STOPF)//could only be a Stop Event then...
			{
				I2C_IRQ_Printf("TRA, no TXE and not Stop !\n\r");
			}
		}
		else					//we should read from the slave
		{
			if (I2C_SR1_RXNE)		//(RXNE) Data Register Not empty
			{
				BYTE data = I2C_DR;
				if(i2c.buffer_index < i2c.masterTransactionLength)
				{
					i2c.masterBuffer[i2c.buffer_index] = data;
                                        i2c.Step = 5;
				}
				if(i2c.buffer_index == (i2c.masterTransactionLength - 1))//The last byte is received
				{
					I2C_CR2_ACK = 0;	//Nack during the last operation
					I2C_CR2_STOP = 1;	//Generate a Stop condition
                                        i2c.Step = 6;
					i2c_user_Rx_Callback(i2c.masterBuffer,i2c.masterTransactionLength);//Notify the user
				}
				//(no more data while rxNE = 1) This is EV7 from Figure 108 of the reference manual RM0016 in page 297
				else if(i2c.buffer_index >= i2c.masterTransactionLength)//The last byte is received 
				{
					//The master mode is over
					i2c.masterMode = 0;		//back to slave mode
					I2C_CR2_ACK = 1;		//Slave mode listen with Acknowledge
                                        i2c.Step = 7;
				}
				i2c.buffer_index++;
			}
			else if(!I2C_SR1_STOPF)//could only be a Stop Event then...
			{
				I2C_IRQ_Printf("Not TRA, no RXNE and not StopF !\n\r");
			}
		}
	}
	else						//(MSL = 0) we are in slave mode
	{
		if (I2C_SR1_ADDR)		//(ADDR) Our Slave Address has matched
		{
			i2c.reg = I2C_SR1;			//clearing status registers
			i2c.reg = I2C_SR3;			//clearing status registers
			I2C_CR2_ACK = 1;			//there will be an acknowledge
			i2c.buffer_index = 0;		//reset the counter after every restart
		}
		else if(I2C_SR1_RXNE)		//(RXNE) Data Register Not empty
		{
			if(i2c.buffer_index < i2c.slaveTransactionLength)//accept the received data
			{
				i2c.slaveBuffer[i2c.buffer_index++] = I2C_DR;
				I2C_CR2_ACK = 1;
				if (i2c.buffer_index == i2c.slaveTransactionLength)
				{
					//i2c_user_Slave_Rx_Callback(i2c.slaveBuffer,i2c.slaveTransactionLength);//Notify the user
				}
			}
			else//cannot accept more than expected
			{
				i2c.reg = I2C_DR;	//data thrown out but read for confirmation
				I2C_CR2_ACK = 1;	//should send NACK, but have no state machine to set it back to 1
			}
		}
		//else we must be in transmission as we did not receive anything
		else if (I2C_SR1_TXE)		//(TXE) Data Register Empty
		{
			if(i2c.buffer_index < i2c.slaveTransactionLength)//accept to transmit data
			{
				I2C_DR = i2c.slaveBuffer[i2c.buffer_index++];
				if (i2c.buffer_index == i2c.slaveTransactionLength)
				{
					//i2c_user_Slave_Tx_Callback(i2c.slaveBuffer,i2c.slaveTransactionLength);//Notify the user
				}
			}
			else
			{
				I2C_DR = 0;			//keep sending 0
			}
		}
	}
	
	//in either cases, handle the stop notification
	//reading SR1 register followed by a write in the CR2 register	
	
	BYTE error_status = I2C_SR2 & 0x0F;
	if(error_status)				//(OVR)
	{
		if(I2C_SR2_AF)
		{
			I2C_SR2_AF = 0;//This is not an error, it is the end of the slave transmission
                        if(i2c.Step == 1)//but if this happens at the beginning then it is an error
                        {
                          I2C_IRQ_Printf("Slave Address - Acknowledge failure !\n\r");
                        }
		}
		if(I2C_SR2_OVR)
		{
			I2C_SR2_OVR = 0;
		}
		if(I2C_SR2_ARLO)
		{
			I2C_SR2_ARLO = 0;
		}
		if(I2C_SR2_BERR)
		{
			I2C_SR2_BERR = 0;
		}
		if(I2C_SR3_MSL)//If we are still Master of the bus, only then stop the transaction
		{
			I2C_CR2_STOP = 1;	//Generate a Stop condition as a master
		}
		i2c_user_Error_Callback(error_status);
	}
	if(I2C_SR1_STOPF)
	{
		i2c.reg = I2C_SR1;
		I2C_CR2 = 0x00;		//a write to this register is needed to clear the STOP Flag
		
	}
	
}


