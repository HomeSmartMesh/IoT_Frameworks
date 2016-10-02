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

#define NB_LEDS 60
#define RGBLedPIN_A 2

#include "commonTypes.h"

typedef struct
{
  BYTE R;
  BYTE G;
  BYTE B;
}RGBColor_t;


extern const RGBColor_t RED;
extern const RGBColor_t GREEN;
extern const RGBColor_t BLUE;
extern const RGBColor_t BLACK;
extern const RGBColor_t WHITE;


// user configuration

#define SwitchOffLed(); SendLedColor(0, 0, 0);

// shared variables

void RGBLeds_PIO_Init();

void SetLedColor(BYTE LedId, BYTE R, BYTE G, BYTE B);
void SetLedColors(BYTE LedId,RGBColor_t Color);

void SendLedsArray();

void RGB_SwitchOff(BYTE first,BYTE NbLeds);

void SendLedColor(BYTE R, BYTE G, BYTE B);
void SendLedColors(RGBColor_t Color);


void RampColors(BYTE delay,BYTE RUp,BYTE GUp,BYTE BUp,BYTE R,BYTE G,BYTE B);

void BlinkColors(BYTE R, BYTE G, BYTE B);

void FlashColors(BYTE delay, RGBColor_t Color);

RGBColor_t ColorScale(int iCount,int nbCount,RGBColor_t ColorStart,RGBColor_t ColorEnd);

void ShadeLeds(BYTE LedStart, BYTE LedEnd, RGBColor_t ColorStart, RGBColor_t ColorEnd);
