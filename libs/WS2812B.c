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

#include "WS2812B.h"

#include <iostm8l151f3.h>

#include "ClockUartLed.h"

unsigned char LedsArray[NB_LEDS];
unsigned int nbLedsBytes = NB_LEDS*3;


const RGBColor_t RED = {255,0,0};
const RGBColor_t GREEN = {0,255,0};
const RGBColor_t BLUE = {0,0,255};
const RGBColor_t BLACK = {0,0,0};
const RGBColor_t WHITE = {255,255,255};


//RGBLedPIN has to be defined by the user 0x02 for PIN_A2, 0x03 for PIN_A3

#if(RGBLedPIN_A == 2)
#define RGBLedPin_Set   "BSET    L:0x5000,      #0x02         \n"
#define RGBLedPin_ReSet   "BRES    L:0x5000,      #0x02         \n"
#else
#define RGBLedPin_Set   "BSET    L:0x5000,      #0x03         \n"
#define RGBLedPin_ReSet   "BRES    L:0x5000,      #0x03         \n"
#endif

void SendLedsArray()
{
  asm(
        "lb_intiLoop:                          \n"
        "LDW      X,             #0xFFFF       \n"// set -1 in X, so that first inc gets 0, as inc has to be in the beginning of the loop
        "LDW      Y,             L:nbLedsBytes \n"//2
        "lb_begin_loop:                        \n"
//---------------------------------------------------------------
//--------------- bit 0 -----------------------------------------
        "INCW    X                           \n"// L t+2
        "lb_start_bit0:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//
        RGBLedPin_Set
        "AND     A,             #0x80        \n"// H t2
        "JREQ    L:lb_bit0_Send_0            \n"// H0 t3 t4 : 2 jump to Zero, 1 Stay in One + next nop
        "lb_bit0_Send_1:                     \n"//------Send 1 : 800ns High, 450ns Low (12,8) 750ns,500ns
        "nop                                 \n"// H1 t5
        "nop                                 \n"// H1 t6
        "nop                                 \n"// H1 t7
        "nop                                 \n"// H1 t8
        "nop                                 \n"// H1 t9
        "nop                                 \n"// H1 t10
        "nop                                 \n"// H1 t11
        "nop                                 \n"// H1 t12
        RGBLedPin_ReSet                         // L1 t1
        "nop                                 \n"// L1 t2
        "JRA     L:lb_start_bit1             \n"// L1 JRA:2 t4 
                                                // L1 NextBitTest:4  t8
        "lb_bit0_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (7,13) 375ns,875ns
                                                // H0 t4
        RGBLedPin_ReSet                         // L0 t1
        "nop                                 \n"// L0 t1
        "nop                                 \n"// L0 t2
        "nop                                 \n"// L0 t3
        "nop                                 \n"// L0 t4
        "nop                                 \n"// L0 t5
        "nop                                 \n"// L0 t6
        "nop                                 \n"// L0 t7
        "nop                                 \n"// L0 t8
                                                //NextBitTest:4+SP = 5. L t13
//--------------- bit 1 -----------------------------------------
        "lb_start_bit1:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//1
        "AND     A,             #0x40        \n"//1
        "JREQ    L:lb_bit1_Send_0            \n"//2 jump to Zero, 1 Stay in One + next nop
        "lb_bit1_Send_1:                     \n"
        "nop                                 \n"//1 to have send0 send1 equality
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        "nop                                 \n"// H t7
        "nop                                 \n"// H t8
        "nop                                 \n"// H t9
        "nop                                 \n"// H t10
        "nop                                 \n"// H t11
        "nop                                 \n"// H t12
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t2
        "JRA     L:lb_start_bit2             \n"// L JRA:2 t4 
                                                // L NextBitTest:4  t8
        "lb_bit1_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t1
        "nop                                 \n"// L t2
        "nop                                 \n"// L t3
        "nop                                 \n"// L t4
        "nop                                 \n"// L t5
        "nop                                 \n"// L t6
        "nop                                 \n"// L t7
        "nop                                 \n"// L t8
        "nop                                 \n"// L t9
                                                //NextBitTest:4+SP = 5. L t14
//--------------- bit 2 -----------------------------------------
        "lb_start_bit2:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//1
        "AND     A,             #0x20        \n"//1
        "JREQ    L:lb_bit2_Send_0            \n"//2 jump to Zero, 1 Stay in One + next nop
        "lb_bit2_Send_1:                     \n"
        "nop                                 \n"//1 to have send0 send1 equality
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        "nop                                 \n"// H t7
        "nop                                 \n"// H t8
        "nop                                 \n"// H t9
        "nop                                 \n"// H t10
        "nop                                 \n"// H t11
        "nop                                 \n"// H t12
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t2
        "JRA     L:lb_start_bit3             \n"// L JRA:2 t4 
                                                // L NextBitTest:4  t8
        "lb_bit2_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t1
        "nop                                 \n"// L t2
        "nop                                 \n"// L t3
        "nop                                 \n"// L t4
        "nop                                 \n"// L t5
        "nop                                 \n"// L t6
        "nop                                 \n"// L t7
        "nop                                 \n"// L t8
        "nop                                 \n"// L t9
                                                //NextBitTest:4+SP = 5. L t14
//--------------- bit 3 -----------------------------------------
        "lb_start_bit3:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//1
        "AND     A,             #0x10        \n"//1
        "JREQ    L:lb_bit3_Send_0            \n"//2 jump to Zero, 1 Stay in One + next nop
        "lb_bit3_Send_1:                     \n"
        "nop                                 \n"//1 to have send0 send1 equality
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        "nop                                 \n"// H t7
        "nop                                 \n"// H t8
        "nop                                 \n"// H t9
        "nop                                 \n"// H t10
        "nop                                 \n"// H t11
        "nop                                 \n"// H t12
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t2
        "JRA     L:lb_start_bit4             \n"// L JRA:2 t4 
                                                // L NextBitTest:4  t8
        "lb_bit3_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t1
        "nop                                 \n"// L t2
        "nop                                 \n"// L t3
        "nop                                 \n"// L t4
        "nop                                 \n"// L t5
        "nop                                 \n"// L t6
        "nop                                 \n"// L t7
        "nop                                 \n"// L t8
        "nop                                 \n"// L t9
                                                //NextBitTest:4+SP = 5. L t14
//--------------- bit 4 -----------------------------------------
        "lb_start_bit4:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//1
        "AND     A,             #0x08        \n"//1
        "JREQ    L:lb_bit4_Send_0            \n"//2 jump to Zero, 1 Stay in One + next nop
        "lb_bit4_Send_1:                     \n"
        "nop                                 \n"//1 to have send0 send1 equality
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        "nop                                 \n"// H t7
        "nop                                 \n"// H t8
        "nop                                 \n"// H t9
        "nop                                 \n"// H t10
        "nop                                 \n"// H t11
        "nop                                 \n"// H t12
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t2
        "JRA     L:lb_start_bit5             \n"// L JRA:2 t4 
                                                // L NextBitTest:4  t8
        "lb_bit4_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t1
        "nop                                 \n"// L t2
        "nop                                 \n"// L t3
        "nop                                 \n"// L t4
        "nop                                 \n"// L t5
        "nop                                 \n"// L t6
        "nop                                 \n"// L t7
        "nop                                 \n"// L t8
        "nop                                 \n"// L t9
                                                //NextBitTest:4+SP = 5. L t14
//--------------- bit 5 -----------------------------------------
        "lb_start_bit5:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//1
        "AND     A,             #0x04        \n"//1
        "JREQ    L:lb_bit5_Send_0            \n"//2 jump to Zero, 1 Stay in One + next nop
        "lb_bit5_Send_1:                     \n"
        "nop                                 \n"//1 to have send0 send1 equality
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        "nop                                 \n"// H t7
        "nop                                 \n"// H t8
        "nop                                 \n"// H t9
        "nop                                 \n"// H t10
        "nop                                 \n"// H t11
        "nop                                 \n"// H t12
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t2
        "JRA     L:lb_start_bit6             \n"// L JRA:2 t4 
                                                // L NextBitTest:4  t8
        "lb_bit5_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t1
        "nop                                 \n"// L t2
        "nop                                 \n"// L t3
        "nop                                 \n"// L t4
        "nop                                 \n"// L t5
        "nop                                 \n"// L t6
        "nop                                 \n"// L t7
        "nop                                 \n"// L t8
        "nop                                 \n"// L t9
                                                //NextBitTest:4+SP = 5. L t14
//--------------- bit 6 -----------------------------------------
        "lb_start_bit6:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//1
        "AND     A,             #0x02        \n"//1
        "JREQ    L:lb_bit6_Send_0            \n"//2 jump to Zero, 1 Stay in One + next nop
        "lb_bit6_Send_1:                     \n"
        "nop                                 \n"//1 to have send0 send1 equality
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        "nop                                 \n"// H t7
        "nop                                 \n"// H t8
        "nop                                 \n"// H t9
        "nop                                 \n"// H t10
        "nop                                 \n"// H t11
        "nop                                 \n"// H t12
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t2
        "JRA     L:lb_start_bit7             \n"// L JRA:2 t4 
                                                // L NextBitTest:4  t8
        "lb_bit6_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t1
        "nop                                 \n"// L t2
        "nop                                 \n"// L t3
        "nop                                 \n"// L t4
        "nop                                 \n"// L t5
        "nop                                 \n"// L t6
        "nop                                 \n"// L t7
        "nop                                 \n"// L t8
        "nop                                 \n"// L t9
                                                //NextBitTest:4+SP = 5. L t14
//--------------- bit 7 -----------------------------------------
        "lb_start_bit7:                      \n"//
        "LD      A,           (L:LedsArray,X)\n"//1
        "AND     A,             #0x01        \n"//1
        "JREQ    L:lb_bit7_Send_0            \n"//2 jump to Zero, 1 Stay in One + next nop
        "lb_bit7_Send_1:                     \n"
        "nop                                 \n"//1 to have send0 send1 equality
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        "nop                                 \n"// H t7
        "nop                                 \n"// H t8
        "nop                                 \n"// H t9
        "nop                                 \n"// H t10
        "nop                                 \n"// H t11
        "nop                                 \n"// H t12
        RGBLedPin_ReSet                         // L t1
        "DECW    Y                           \n"//2
        "JREQ    L:lb_exit                   \n"//1 on loop, 2 jmp to exit
        "JP      L:lb_begin_loop             \n"//5
                                                // 
        "lb_bit7_Send_0:                     \n"//------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
        RGBLedPin_Set                           // H t1
        "nop                                 \n"// H t2
        "nop                                 \n"// H t3
        "nop                                 \n"// H t4
        "nop                                 \n"// H t5
        "nop                                 \n"// H t6
        RGBLedPin_ReSet                         // L t1
        "nop                                 \n"// L t1
        "nop                                 \n"// L t2
        "nop                                 \n"// L t3
        "nop                                 \n"// L t4
        "nop                                 \n"// L t5
        "nop                                 \n"// L t6
                                                // L DECW 2, JREQ 1, 2 = 5 =>   t14
//--------------------------------------------------------
//--------------------------------------------------------
        "DECW    Y                           \n"//2
        "JREQ    L:lb_exit                   \n"//1 on loop, 2 jmp to exit
        "JP      L:lb_begin_loop             \n"//5
        "lb_exit:nop");
  
}


void SetLedColor(BYTE LedId, BYTE R, BYTE G, BYTE B)
{
  LedsArray[LedId*3] = G;
  LedsArray[LedId*3+1] = R;
  LedsArray[LedId*3+2] = B;
}

void SetLedColors(BYTE LedId,RGBColor_t Color)
{
  LedsArray[LedId*3] = Color.G;
  LedsArray[LedId*3+1] = Color.R;
  LedsArray[LedId*3+2] = Color.B;
}

void RGBLeds_PIO_Init_A2()
{
	PA_DDR_bit.DDR2 = 1;//output

	PA_CR1_bit.C12 = 0;//0:open Drain - 1:PushPull
}

void RGBLeds_PIO_Init_A3()
{
	PA_DDR_bit.DDR3 = 1;//output

	PA_CR1_bit.C13 = 0;//0:open Drain - 1:PushPull
}

void RGBLeds_PIO_Init()
{
  #if(RGBLedPIN_A == 2)
  RGBLeds_PIO_Init_A2();
  #else
  RGBLeds_PIO_Init_A3();
  #endif
}

//useful when only one Led is beeing used
// updates the color memory, send it, then keeps the minimal reset delay
void SendLedColor(BYTE R, BYTE G, BYTE B)
{
  LedsArray[0] = G;
  LedsArray[1] = R;
  LedsArray[2] = B;
  SendLedsArray();
  delay_100us();//minimal delay for reset to restart the same color is 50 us
}

void SendLedColors(RGBColor_t Color)
{
  LedsArray[0] = Color.G;
  LedsArray[1] = Color.R;
  LedsArray[2] = Color.B;
  SendLedsArray();
  delay_100us();//minimal delay for reset to restart the same color is 50 us
}

void RGB_SwitchOff(BYTE first,BYTE NbLeds)
{
	for(BYTE i=first;i<NbLeds;i++)
	{
		SetLedColor(i,0,0,0);
	}
	SendLedsArray();
	delay_100us();//minimal delay for reset to restart the same color is 50 us
}
//delay : x255 meas 10 => 2.55 sec for cycle completion
//XUp : 0 = unmodified, 1 = Up, 2 = Down
//RGB : used when XUp = 0; unmodified stays at any user provided value
void RampColors(BYTE delay,BYTE RUp,BYTE GUp,BYTE BUp,BYTE R,BYTE G,BYTE B)
{
        if(RUp == 1)
        {
          R=0;
        }
        else if(RUp == 2)
        {
          R=255;
        }
        if(GUp == 1)
        {
          G=0;
        }
        else if(GUp == 2)
        {
          G=255;
        }
        if(BUp == 1)
        {
          B=0;
        }
        else if(BUp == 2)
        {
          B=255;
        }
	for(int ml=0;ml<256;ml++)
	{
	  SendLedColor(R,G,B);//first values sent are 0, last are 255
          if(RUp == 1)
          {
            R++;
          }
          else if(RUp == 2)
          {
            R--;
          }
          if(GUp == 1)
          {
            G++;
          }
          else if(GUp == 2)
          {
            G--;
          }
          if(BUp == 1)
          {
            B++;
          }
          else if(BUp == 2)
          {
            B--;
          }
          delay_1ms_Count(delay);
	}
  
}

//1 : Blink, 0 : Keep off
void BlinkColors(BYTE R, BYTE G, BYTE B)
{
  const BYTE delay = 1;
  RampColors(delay,1&R,1&G,1&B,0,0,0);//Selected Colors Up
  RampColors(delay,2&R,2&G,2&B,0,0,0);//Selected Colors Down
}

//input : two colors, and ratio integers
//output : the interpolaetd color
//Sets the interpolated color between Colors Start and End, to the ratio of iCount/nbCount
RGBColor_t ColorScale(int iCount,int nbCount,RGBColor_t ColorStart,RGBColor_t ColorEnd)
{
    RGBColor_t resColor;
    
    int Red = (ColorEnd.R - ColorStart.R);//255
    Red = Red * iCount;//0 - 2550
    Red = ColorStart.R + (Red / (nbCount - 1));//0 - 255
    resColor.R = Red;

    int Green = (ColorEnd.G - ColorStart.G);//255
    Green = Green * iCount;//0 - 2550
    Green = ColorStart.G + (Green / (nbCount - 1));//0 - 255
    resColor.G = Green;

    int Blue = (ColorEnd.B - ColorStart.B);//255
    Blue = Blue * iCount;//0 - 2550
    Blue = ColorStart.B + (Blue / (nbCount - 1));//0 - 255
    resColor.B = Blue;

    return resColor;
}

//inputs: the leds section to use defined by which led to start with and which one to end before
//inputs: The color of the first led to use and the color of the last led to use
//action: updates the leds memory with the first and last led colors and all intermediate leds are interpolated
//comment: This function does not send the led data bits on the bus, so that multiple operations can 
//         be applied before sending the whole result together with SendLedsArray();
void ShadeLeds(BYTE LedStart, BYTE LedEnd, RGBColor_t ColorStart, RGBColor_t ColorEnd)
{
  int nbLeds = LedEnd - LedStart;
  BYTE LedId = LedStart;
  for(int iCount=0;iCount<nbLeds;iCount++)//0-10
  {
    RGBColor_t Ci = ColorScale(iCount,nbLeds,ColorStart,ColorEnd);
    SetLedColors(LedId,Ci);
    LedId++;
  }
}

void FlashColors(BYTE delay, RGBColor_t Color)
{
  for(int iCount=0;iCount<255;iCount++)//0-10
  {
    RGBColor_t Ci = ColorScale(iCount,255,BLACK,Color);
    SendLedColors(Ci);
    delay_1ms_Count(delay);
  }
  for(int iCount=0;iCount<255;iCount++)//0-10
  {
    RGBColor_t Ci = ColorScale(iCount,255,Color,BLACK);
    SendLedColors(Ci);
    delay_1ms_Count(delay);
  }
  SwitchOffLed();
}

