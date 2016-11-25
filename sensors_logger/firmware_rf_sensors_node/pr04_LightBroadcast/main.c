#include <iostm8l151f3.h>
#include <intrinsics.h>

#include "nRF_SPI.h"
//for nRF_SetMode_TX()
#include "nRF.h"

#include "nRF_Tx.h"

#include "clock_led.h"
#include "uart.h"

#include "i2c_stm8x.h"
#include "commonTypes.h"

#include "max44009.h"

#define EEPROM_Offset 0x1000
#define NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

//---------------------- Active Halt Mode :
// - CPU and Peripheral clocks stopped, RTC running
// - wakeup from RTC, or external/Reset

void RfAlive()
{
      unsigned char Tx_Data[3];
      Tx_Data[0]=0x75;
      Tx_Data[1]=NodeId;
      Tx_Data[2]= Tx_Data[0] ^ NodeId;
      nRF_Transmit(Tx_Data,3);
}

void Rf_Light(uint16_t light)
{
      unsigned char Tx_Data[5];
      Tx_Data[0]=0x3B;//Light is 0x3B
      Tx_Data[1]=NodeId;
      Tx_Data[2]=light>>4;
      Tx_Data[3]=light&0x0F;
      Tx_Data[4]= Tx_Data[0] ^ NodeId;
      nRF_Transmit(Tx_Data,5);
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




void SMT8L_Switch_ToHSI()
{
  CLK_SWCR_SWEN = 1;                  //  Enable switching.
  CLK_SWR = 0x01;                     //  Use HSI as the clock source.
  while (CLK_SWCR_SWBSY != 0);        //  Pause while the clock switch is busy.
}

void Initialise_STM8L_Clock()
{
  //Set Clock to full speed
  CLK_CKDIVR_CKM = 0; // Set to 0x00 => /1 ; Reset is 0x03 => /8
  //unsigned char cal = CLK_HSICALR-45;//default is 0x66 lowered by 45
  //Unlock the trimming
  /*CLK_HSIUNLCKR = 0xAC;
  CLK_HSIUNLCKR = 0x35;
  CLK_HSITRIMR = cal;
  */
  
  
  //Enable RTC Peripheral Clock
  CLK_PCKENR2_PCKEN22 = 1;
  
  CLK_CRTCR_RTCDIV = 0;//reset value : RTC Clock source /1
  CLK_CRTCR_RTCSEL = 2;// 2:LSI; reset value 0
  while (CLK_CRTCR_RTCSWBSY != 0);        //  Pause while the RTC clock changes.
    
}

void Initialise_STM8L_RTC_LowPower()
{
    //unlock the write protection for RTC
    RTC_WPR = 0xCA;
    RTC_WPR = 0x53;
    
    RTC_ISR1_INIT = 1;//Initialisation mode
    
    //RTC_SPRERH_PREDIV_S = 0;// 7bits 0x00 Sychronous prescaler factor MSB
    //RTC_SPRERL_PREDIV_S = 0;// 8bits 0xFF Sychronous prescaler factor MSB
    //RTC_APRER_PREDIV_A = 0;// 7bits 0x7F Asynchronous prescaler factor

    RTC_CR1_FMT = 0;//24h format
    RTC_CR1_RATIO = 0;//fsysclk >= 2x fRTCclk
    // N.A RTC_CR1_BYPSHAD = 0;//shadow used no direct read from counters
    
    RTC_CR2_WUTE = 0;//Wakeup timer Disable to update the timer
	uint16_t count = 65535;
    while((RTC_ISR1_WUTWF==0)&&(count!=0) )
	{
		count--;
	}//
	if(count== 0)
	{
		printf_ln(">>>Error : RTC_ISR1_WUTWF does not get to 1");
	}
    
	
    RTC_CR1_WUCKSEL = 0;//-b00 RTCCCLK/16 ; -b011 RTCCCLK/2 
    
    //with 38KHz has about 61us resolution
    //225-0 with RTC_CR1_WUCKSEL = 3
	//255 255 => 28 sec
    RTC_WUTRH_WUT = 40;// to match a bit less than 10s
    RTC_WUTRL_WUT = 255;//
    
    RTC_CR2_WUTE = 1;//Wakeup timer enable - starts downcounting
    RTC_CR2_WUTIE = 1;//Wakeup timer interrupt enable
    
    RTC_ISR1_INIT = 0;//Running mode

    //locking the write protection for RTC
    RTC_WPR = 0x00;
    RTC_WPR = 0x00;
    
    //wait that the internal VRef is stabilized before changing the WU options (Reference Manual)
	count = 65535;
    while((PWR_CSR2_VREFINTF==0)&&(count!=0) )
	{
		count--;
	}//
	if(count== 0)
	{
		printf_ln(">>>Error : PWR_CSR2_VREFINTF does not get to 1");
	}
    PWR_CSR2_ULP = 1;//Internal Voltage Reference Stopped in Halt Active Halt
    PWR_CSR2_FWU = 1;//Fast wakeup time
    
    
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
void PingUart(unsigned char index)
{
      printf("Ping Color STM8L ");
      UARTPrintf_uint(index);
      printf(" \n");
}


void i2c_user_Rx_Callback(BYTE *userdata,BYTE size)
{
	/*printf("I2C Transaction complete, received:\n\r");
	UARTPrintfHexTable(userdata,size);
	printf("\n\r");*/
	//cannot call the state machine from interruption context
	//i2c_ReadLight_StateMachine();
        
}

void i2c_user_Tx_Callback(BYTE *userdata,BYTE size)
{
	/*printf("I2C Transaction complete, Transmitted:\n\r");
	UARTPrintfHexTable(userdata,size);
	printf("\n\r");*/
	//cannot call the state machine from interruption context
	//i2c_ReadLight_StateMachine();
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

#pragma vector = RTC_WAKEUP_vector
__interrupt void IRQHandler_RTC(void)
{
	putc('Q');
  if(RTC_ISR2_WUTF)
  {
    RTC_ISR2_WUTF = 0;
    RfAlive();

	//delay_ms(10);//some time is needed to recover the right clock
	//printf("...\nRTC IRQ\n");
    
	
    
	/*printf("Now Log Magnet\r\n");
    LogMagnets();
	
	delay_ms(1000);

	printf("Initialise_STM8L_Clock\r\n");
	Initialise_STM8L_Clock();
	printf("I2C Init\r\n");
	I2C_Init();

	delay_ms(1000);	
	printf("Now Read Light\r\n");
	delay_ms(1);
    ReadLight_sm();
	delay_ms(1);*/
    
  }
  
}

int main( void )
{

    NodeId = *NODE_ID;
  //unsigned char index = 0;
    Initialise_STM8L_Clock();
    
    SYSCFG_RMPCR1_USART1TR_REMAP = 1; // Remap 01: USART1_TX on PA2 and USART1_RX on PA3
    uart_init();//Tx only
    
    printf("ws04_Node_LowSimple\\pr02_AmbientLight\n\r");
    delay_ms(1000);

    I2C_Init();
    __enable_interrupt();
    
    
    //Applies the compile time configured parameters from nRF_Configuration.h
    nRF_Config();

    //The TX Mode is independently set from nRF_Config() and can be changed on run time
    //nRF_SetMode_TX();
      
    
    //Init_Magnet_PB0();
    //Init_Magnet_PD0();
    
    printf("Initialise_STM8L_RTC_LowPower()\n");
    Initialise_STM8L_RTC_LowPower();

    printf("main loop()\n");
    //
    // Main loop
    //
    while (1)
    {
		putc('M');
		//RfAlive();
		//printf_ln("main() max44009_read_light()");
		uint16_t light = max44009_read_light();
		putc('T');
		//send through UART
		//printf("Light: ");		printf_uint(light);		printf_eol();
		//send through RF
		putc('F');
                RfAlive();
		nRF_Wait_Transmit();
                delay_ms(10);
		Rf_Light(light);//no wait transmit here let it finish and go to sleep
                nRF_Wait_Transmit();
		//printf("Back to __halt()\n\r");
		putc('H');
		delay_100us();
		__halt();
      
    }
}
