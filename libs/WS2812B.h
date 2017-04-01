/** @file WS2812B.h
 *
 * @author Wassim FILALI
 * Inferred from the Datasheet
 * 
 *
 * @compiler IAR STM8
 *
 *
 * started on local project on 10.10.2015
 * this file was created for refactoring on 03.01.2016
 * $Date:
 * $Revision:
 *
 */

#include "commonTypes.h"


extern const RGBColor_t RED;
extern const RGBColor_t GREEN;
extern const RGBColor_t BLUE;
extern const RGBColor_t BLACK;
extern const RGBColor_t WHITE;


// user configuration

#define rgb_SwitchOff(); rgb_SendColor(0, 0, 0);

// shared variables

void rgb_PIO_Init();

//These functions only update the LED colors memory without sending the data
void rgb_SetColor(BYTE LedId, BYTE R, BYTE G, BYTE B);
void rgb_SetColors(BYTE LedId,RGBColor_t Color);

void rgb_SetColors_range(BYTE first,BYTE NbLeds,RGBColor_t Color);
void rgb_SetColor_range(BYTE first,BYTE NbLeds,BYTE R,BYTE G,BYTE B);

//The Function that serialises the RGB bit values from memory into pulses for the LED WS2812B
//this is used manually by the user in case of complex combination of partial shading of the array
//the sending the complete LED sequence together.
void rgb_SendArray();

//----------------- Functions with direct impact ------------------------
void rgb_SwitchOff_Range(BYTE first,BYTE NbLeds);


void rgb_SendColor(BYTE R, BYTE G, BYTE B); // 1st LED Function
void rgb_SendColors(RGBColor_t Color);      // 1st LED Function

//delay : x255 meas 10 => 2.55 sec for cycle completion
//XUp : 0 = unmodified, 1 = Up, 2 = Down
//RGB : used when XUp = 0; unmodified stays at any user provided value
void rgb_RampColors(BYTE delay,BYTE RUp,BYTE GUp,BYTE BUp,BYTE R,BYTE G,BYTE B);
void rgb_RampColors_range(BYTE delay,BYTE nbLED,BYTE RUp,BYTE GUp,BYTE BUp,BYTE R,BYTE G,BYTE B);

void rgb_BlinkColors(BYTE R, BYTE G, BYTE B);

void rgb_FlashColors(BYTE delay, RGBColor_t Color);

//--------------------- Functions for Colors Memory update only --------------------
RGBColor_t rgb_ColorScale(int iCount,int nbCount,RGBColor_t ColorStart,RGBColor_t ColorEnd);

void rgb_Shade(BYTE LedStart, BYTE LedEnd, RGBColor_t ColorStart, RGBColor_t ColorEnd);

//--------------------- Demo functions with animation sequence ----------------------
void rgb_TestColors();

void rgb_Loop_BlueRedBlue(BYTE nbLeds,BYTE delay);

//--------------------- RF Protocol handling ----------------------------------------
void rgb_decode_rf(BYTE *rxData,BYTE rx_DataSize);
void rgb_encode_rf(RGBColor_t ColorRx,BYTE *rxPayload,BYTE *p_rxPayloadSize);


void rgb_rf_get_tx_Color_6B(BYTE Target_NodeId,BYTE *txData,RGBColor_t Color);
