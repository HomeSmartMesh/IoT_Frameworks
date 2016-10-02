/** @file temp_ds18b20.h
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

#include "commonTypes.h"

void Initialise_OneWire_Pin_A2();
 
void OneWire_Reset();

BYTE OneWire_CheckPresence();

BYTE OneWire_Init();

BYTE OneWire_ReadBit();

void OneWire_WriteByte(BYTE byte);

BYTE OneWire_ReadByte();

BYTE DS18B20_Single_ReadRom(BYTE * rom);

BYTE DS18B20_Single_ReadScratchPad(BYTE * sp,BYTE length);

BYTE DS18B20_Single_ReadTemperature(BYTE * data);

void UARTPrint_DS18B20_Temperature(BYTE * data);
