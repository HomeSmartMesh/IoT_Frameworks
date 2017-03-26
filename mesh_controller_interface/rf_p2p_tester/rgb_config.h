/** @file rgb_config.h
 *
 * @author Wassim FILALI
 * 
 *
 *
 * $Date: 27.02.2017
 * $Revision:
 *
 */
#ifndef _RGB_CONFIG_
#define _RGB_CONFIG_

//NB_LEDS : Number of LED to be supported which would derive the size of the memory 
//          to be allocated Table[NB_LEDS*3] as that table will contain {R,G,B} Bytes per LED
//1 - 255 : this parameter has for limit the size of the RAM and the 8bit counter => 255
//the value of 256 == 0 was not tested
//255 => 75% of RAM
//#define NB_LEDS 216
#define NB_LEDS 1

//reverse the polarity of the PIO when going through a level shifter inverter
// 1 : invert
// 0 : do not invert
#define RGB_IO_NEG 0


//RGBLedPIN_A : the Port A pin number to be used, supported values for the Fixed Node v2 Target Board :
// 2 => Not recommended as not High speed
// 3 => Recommend with High speed setting 3.3 V driving 5V TTL input
//  recommended settings Vin min = 0.7Vdd ; 5V => 3.5V so 3.3 is not enough, not recommended but working
#define RGBLedPIN_A 3

#endif /*_RGB_CONFIG_*/
