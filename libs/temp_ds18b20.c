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

#include <iostm8l151f3.h>

#include "ClockUartLed.h"

//  !!!!! The one wire ds18B20 is using pin A2 on the STM8 Node Module !!!!!

//release is input
#define OneWire_Release(); PA_DDR_bit.DDR2 = 0;

//down is output, and low
#define OneWire_Down(); PA_DDR_bit.DDR2 = 1;PA_ODR_bit.ODR2 = 0;

#define OneWire_State() PA_IDR_bit.IDR2



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

void Initialise_OneWire_Pin_A2()
{
	PA_DDR_bit.DDR2 = 1;//output

	PA_CR1_bit.C12 = 0;//open Drain

	//   0: Output speed up to  2 MHz
	//   1: Output speed up to 10 MHz
	//
	PA_CR2_bit.C22 = 1;
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
  UARTPrintf(".");
  UARTPrintf_uint(frac);
}

