
#include <iostm8l151f3.h>
#include <intrinsics.h>

//should be before other files that use it such as clock_led.h
#include "deviceType.h"
//for config
#include "nRF.h"
//for transmit
#include "nRF_Tx.h"

#include "uart.h"
#include "clock_led.h"

#include "i2c_stm8x.h"
#include "commonTypes.h"

#include "bme280.h"

//to format the tx data
#include "rf_protocol.h"

#define EEPROM_Offset 0x1000
#define NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

//---------------------- Active Halt Mode :
// - CPU and Peripheral clocks stopped, RTC running
// - wakeup from RTC, or external/Reset

void rf_send_alive()
{
	BYTE tx_data[3];
	rf_get_tx_alive_3B(NodeId, tx_data);
	nRF_Transmit(tx_data,3);
}

void RfSwitch(unsigned char state)
{
	unsigned char Tx_Data[4];
	Tx_Data[0]=0xC5;
	Tx_Data[1]=NodeId;
	Tx_Data[2]=state;
	Tx_Data[3]= Tx_Data[0] ^ NodeId ^ state;
	nRF_Transmit(Tx_Data,4);
}

void rf_send_bme280_measures()
{
	BYTE tx_data[11];
	bme280_get_tx_measures_11B(NodeId, tx_data);
	nRF_Transmit(tx_data,11);
}

void LogMagnets()
{
      delay_100us();
      unsigned char Magnet_B0,Magnet_D0;
      Magnet_B0 = PB_IDR_IDR0;
      Magnet_D0 = PD_IDR_IDR0;
      //printf(" LVD0 ");
      UARTPrintf_uint(Magnet_D0);
      //printf(" ; HH B0 ");
      UARTPrintf_uint(Magnet_B0);
      printf("\n");
      delay_100us();
      delay_100us();
}

//bit 0 - pin interrupt
#pragma vector = EXTI0_vector
__interrupt void IRQHandler_Pin0(void)
{
  if(EXTI_SR1_P0F == 1)
  {
    //printf("Pin0_Interrupt ");LogMagnets();
    RfSwitch(PB_IDR_IDR0);
  }
  EXTI_SR1 = 0xFF;//acknowledge all interrupts pins
}


#pragma vector = RTC_WAKEUP_vector
__interrupt void IRQHandler_RTC(void)
{
  if(RTC_ISR2_WUTF)
  {
    RTC_ISR2_WUTF = 0;
    
    //rf_send_alive();
    
    //LogMagnets();
  }
  
}


void SMT8L_Switch_ToHSI()
{
  CLK_SWCR_SWEN = 1;                  //  Enable switching.
  CLK_SWR = 0x01;                     //  Use HSI as the clock source.
  while (CLK_SWCR_SWBSY != 0);        //  Pause while the clock switch is busy.
}


void Init_Magnet_PB0()
{
    PB_DDR_bit.DDR0 = 0;//  0: Input
    PB_CR1_bit.C10 = 0; //  0: Floating
    PB_CR2_bit.C20 = 1; // Exernal interrupt enabled
    
    EXTI_CR1_P0IS = 3;//Rising and Falling edges, interrupt on events - bit 0
    //EXTI_CR3_PBIS = 00;//Falling edge and low level - Port B
    
}

void Init_Magnet_PD0()
{
    PD_DDR_bit.DDR0 = 0;//  0: Input
    PD_CR1_bit.C10 = 0; //  0: Floating
}


void Initialise_Test_GPIO_A2()
{
    PA_DDR_bit.DDR2 = 1;//  1: Output
    PA_CR1_bit.C12 = 1; //  1: Push-pull
}

void GPIO_B3_High()
{
    PB_ODR_bit.ODR3 = 1;
}

void GPIO_B3_Low()
{
    PB_ODR_bit.ODR3 = 0;
}

void PingColor()
{
      unsigned char Tx_Data[5];
      Tx_Data[0]=128;
      Tx_Data[1]=255;
      Tx_Data[2]=100;
      Tx_Data[3]=0x59;
      nRF_Transmit(Tx_Data,4);
}

void i2c_user_Rx_Callback(BYTE *userdata,BYTE size)
{
	/*printf("I2C Transaction complete, received:\n\r");
	UARTPrintfHexTable(userdata,size);
	printf("\n\r");*/
        
}

void i2c_user_Tx_Callback(BYTE *userdata,BYTE size)
{
  /*
	printf("I2C Transaction complete, Transmitted:\n\r");
	UARTPrintfHexTable(userdata,size);
	printf("\n\r");
        */
}

void i2c_user_Error_Callback(BYTE l_sr2)
{
	if(l_sr2 & 0x01)
	{
		printf("[I2C Bus Error]\n\r");
	}
	if(l_sr2 & 0x02)
	{
		printf("[I2C Arbitration Lost]\n\r");
	}
	if(l_sr2 & 0x04)
	{
		printf("[I2C no Acknowledge]\n\r");//this is ok for the slave
	}
	if(l_sr2 & 0x08)
	{
		printf("[I2C Bus Overrun]\n\r");
	}
}

void startup_info()
{
	printf_eol();
	printf_ln("_________________________________");
	printf_ln("sensors_logger/firmware_rf_sensors_node/");
	printf("Node id ");
	printf_hex(NodeId);
	printf_eol();

	//this is only for verification, if this fails, the error will be printed and no matter what happens next
	bme280_check_id();

	bme280_print_CalibData();
		
}

void clear_unused_PIO()
{
	//C5
	PC_DDR_bit.DDR5 = 1;//output
	PC_ODR_bit.ODR5 = 0;//Low
	//C6
	PC_DDR_bit.DDR6 = 1;//output
	PC_ODR_bit.ODR6 = 0;//Low
	//A3
	PA_DDR_bit.DDR3 = 1;//output
	PA_ODR_bit.ODR3 = 0;//Low
	//B2
	PB_DDR_bit.DDR2 = 1;//output
	PB_ODR_bit.ODR2 = 0;//Low
	//A0
	/*PA_DDR_bit.DDR3 = 1;//output
	PA_ODR_bit.ODR3 = 0;//Low
	//A1
	PA_DDR_bit.DDR3 = 1;//output
	PA_ODR_bit.ODR3 = 0;//Low
	*/
}

void clear_all_PIO()
{
	//All out Low
	PA_DDR = 0xFF;
	PA_ODR = 0x00;
	PB_DDR = 0xFF;
	PB_ODR = 0x00;
	PC_DDR = 0xFF;
	PC_ODR = 0x00;
	PD_DDR = 0xFF;
	PD_ODR = 0x00;
}

int main( void )
{
	BYTE counter = 0;
	NodeId = *NODE_ID;

	//clear_unused_PIO();//no noticable effect
	
	Initialise_STM8L_Clock();			//here the RTC clock source is set to LSI
	Initialise_STM8L_RTC_LowPower();
	__enable_interrupt();

	
	
#ifdef CheckMinimalPower
	clear_all_PIO();					//with all pio cleared goes down from 6 uA down to 1 uA !!!
	while (1)
	{
		__halt();
	}
#endif

	SYSCFG_RMPCR1_USART1TR_REMAP = 1; // Remap 01: USART1_TX on PA2 and USART1_RX on PA3
	uart_init();//Tx only
	I2C_Init();
        

	//Applies the compile time configured parameters from nRF_Configuration.h
	nRF_Config();


	//
	// Main loop
	//
	while (1)
	{
		//printf("Measure---------------\n");
		if(counter == 1)
		{
			//startup info are only sent once after a sleep cycle to avoid continuous restarts
			//that kill the battery with a lot of uart that drops again and loops in another restart cycle
			startup_info();
		}
		bme280_force_OneMeasure(1,1,1);//Pressure, Temperature, Humidity
		bme280_wait_measures();
		//bme280_print_measures();
		//printf("rf_send---------------\n");
		rf_send_bme280_measures();
		//delay_ms(60000);//down to one minute
		nRF_Wait_Transmit();
		//delay_ms(10);
		nRF_SetMode_PowerDown();
		
		if(counter == 200)
		{
			counter = 2;
		}
		counter++;
                
		__halt();
	}
}
