/** @file temp_ds18b20.c
 *
 * @author Wassim FILALI
 * Inferred from the Datasheet
 * 
 *
 * @compiler IAR STM8
 *
 *
 * started on local project on 12.10.2015
 * this file was created for refactoring on 26.12.2015
 * $Date: 26.12.2015
 * $Revision:
 *
 */

#include "temp_ds18b20.h"

#include "deviceType.h"

#if DEVICE_STM8L == 1
  #error This sensor was not planned to be used with the low power device 3.3V, not even for RF reception
#elif DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

#include "uart.h"
#include "clock_led.h"
#define PinSensor_A3

//  !!!!! The one wire ds18B20 pin configuration : A2 !!!!!
#ifdef PinSensor_A2
#define PinSensor_DDR   PA_DDR_bit.DDR2
#define PinSensor_ODR   PA_ODR_bit.ODR2
#define PinSensor_IDR   PA_IDR_bit.IDR2
#define PinSensor_CR1   PA_CR1_bit.C12
#define PinSensor_CR2   PA_CR2_bit.C22
#endif
#ifdef PinSensor_A3
#define PinSensor_DDR   PA_DDR_bit.DDR3
#define PinSensor_ODR   PA_ODR_bit.ODR3
#define PinSensor_IDR   PA_IDR_bit.IDR3
#define PinSensor_CR1   PA_CR1_bit.C13
#define PinSensor_CR2   PA_CR2_bit.C23
#endif

//---------------------------------------------------------------


//release is input
#define OneWire_Release(); PinSensor_DDR = 0;

//down is output, and low
#define OneWire_Down(); PinSensor_DDR = 1;PinSensor_ODR = 0;

#define OneWire_State() PinSensor_IDR



#define OneWire_WriteOne();      OneWire_Down();delay_10us();OneWire_Release();delay_50us();
#define OneWire_WriteZero();     OneWire_Down();delay_10us();delay_50us();OneWire_Release();


#define DS18B20_ROM_SearchRom   0xF0
#define DS18B20_ROM_ReadRom     0x33
#define DS18B20_ROM_MatchRom    0x55
#define DS18B20_ROM_SkipRom     0xCC
#define DS18B20_ROM_AlarmSearch 0xEC

#define DS18B20_Func_Convert            0x44
#define DS18B20_Func_WriteScratchPad    0x4E
#define DS18B20_Func_ReadScratchPad     0xBE
#define DS18B20_Func_CopyScratchPad     0x48
#define DS18B20_Func_RecallE2           0xB8
#define DS18B20_Func_ReadPowerSupply    0xB4

//Write one is a pulse shorter than 15us


BYTE DS18B20_ScratchPad[9];

void Initialise_OneWire_Pin()
{
	PinSensor_DDR = 1;//output

	PinSensor_CR1 = 0;//open Drain

	//   0: Output speed up to  2 MHz
	//   1: Output speed up to 10 MHz
	//
	PinSensor_CR2 = 1;
}

void OneWire_Reset()
{
  OneWire_Down();
  for(int i=0;i<5;i++)//delay a bit more than 500 us 
  {
    delay_100us();
  }

  OneWire_Release();
}

BYTE OneWire_CheckPresence()
{
  BYTE res = 0;
  
  for(int i=0;i<10;i++)//delay a bit more than 100 us 
  {
    delay_10us();
  }

  if(OneWire_State() == 0)
  {
    res = 1;//present
  }
  else
  {
    res = 0;//not present
  }

  //wait 200 us  to have total 300 us = max of 60(response time) and 240(pulse time)
  for(int i=0;i<14;i++)//delay a bit more than 60 us 
  {
    delay_10us();
  }
  
  return res;
}

BYTE OneWire_Init()
{


	OneWire_Reset();
	return OneWire_CheckPresence();
}


BYTE OneWire_ReadBit()
{
  BYTE res;

  OneWire_Down();
  delay_1us();delay_1us();//min 1us
  OneWire_Release();
  delay_10us();
  //must read within 15 us from edge low start
  
  if(OneWire_State() == 0)
  {
    res = 0;//Pull low for 0
  }
  else
  {
    res = 1;//up for 1
  }
  
  //delay 50us to reach more than 60 us bit time + 1 us inter bit time
  for(int i=0;i<5;i++)//delay a bit more than 100 us 
  {
    delay_10us();
  }
  
  return res;
}


void OneWire_WriteByte(BYTE byte)
{
  for(int i=0;i<8;i++)
  {
    if(byte & 0x01)
    {
      OneWire_WriteOne();
    }
    else
    {
      OneWire_WriteZero();
    }
    byte>>=1;
  }
}

BYTE OneWire_ReadByte()
{
  BYTE res = 0;
  
  for(int i=0;i<7;i++)
  {
    if(OneWire_ReadBit())
    {
      res |= 0x80;
    }
    res >>=1;
  }
  // no shift on the last loop
  if(OneWire_ReadBit())
  {
    res |= 0x80;
  }
  return res;
}


BYTE DS18B20_Single_ReadRom(BYTE * rom)
{
  BYTE res = 0;
  
  if(OneWire_Init())
  {
    OneWire_WriteByte(DS18B20_ROM_ReadRom);
    
    for(int i=0;i<8;i++)
    {
      rom[7-i] = OneWire_ReadByte();
      
    }
    
    res = 1;
  }
  
  return res;
}

BYTE DS18B20_Single_ReadScratchPad(BYTE * sp,BYTE length)
{
  BYTE res = 0;

  if(OneWire_Init())
  {
    OneWire_WriteByte(DS18B20_ROM_SkipRom);
    
    OneWire_WriteByte(DS18B20_Func_ReadScratchPad);

    for(int i=0;i<length;i++)
    {
        sp[i] = OneWire_ReadByte();
    }
    
    res = 1;
  }
  
  return res;
}

BYTE DS18B20_Single_ReadTemperature(BYTE * data)
{
  BYTE res = 0;
  
  if(OneWire_Init())
  {
    OneWire_WriteByte(DS18B20_ROM_SkipRom);
    
    OneWire_WriteByte(DS18B20_Func_Convert);

    //keeps checking while the sensor is converting
    while(OneWire_ReadBit() == 0);
    
    
    if(DS18B20_Single_ReadScratchPad(data,2))
    {
      res = 1;
    }
    
  }
  
  
  return res;
}

void UARTPrint_DS18B20_Temperature(BYTE * data)
{
  
  signed int trunc = data[1];
  
  trunc = trunc * 16;
  
  trunc = trunc + (data[0] >> 4);

  unsigned int frac = 0;
  if(data[0] & 0x01)
  {
    frac+=625;
  }
  if(data[0] & 0x02)
  {
    frac+=1250;
  }
  if(data[0] & 0x04)
  {
    frac+=2500;
  }
  if(data[0] & 0x08)
  {
    frac+=5000;
  }
  UARTPrintf_sint(trunc);
  putc('.');
  printf_uint(frac);
}

void rx_temperature_ds18b20(BYTE *rxData,BYTE rx_DataSize)
{
	BYTE crc = rxData[0];
	for(int i=1;i<4;i++)
	{
	  crc ^= rxData[i];
	}
	if(crc == rxData[4])
	{
	  printf("NodeId:");
	  UARTPrintf_uint(rxData[1]);
	  printf(";Temperature:");
	  UARTPrint_DS18B20_Temperature(rxData+2);
	  UARTPrintfLn("");
	}
	else
	{
	  printf_ln("Pid:Temperature;Error:CRC Fail");
	}
}

