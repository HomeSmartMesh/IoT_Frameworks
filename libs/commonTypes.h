/** @file commonTypes.h
 *
 * @author Wassim FILALI
 * 
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 13.12.2015
 * $Revision:
 *
 */

// UINT type definition
#ifndef _COMMON_TYPES_
#define _COMMON_TYPES_


typedef void* NULL;
typedef unsigned int UINT;
typedef unsigned char BYTE;

typedef unsigned char   U8_t;
typedef signed char     S8_t;
typedef unsigned short  U16_t;
typedef signed int      S16_t;
typedef unsigned long   U32_t;
typedef signed long     S32_t;

typedef unsigned char   uint8_t;
typedef signed char     int8_t;
typedef unsigned short  uint16_t;
typedef signed int      int16_t;
typedef unsigned long   uint32_t;
typedef signed long     int32_t;

typedef struct
{
  BYTE R;
  BYTE G;
  BYTE B;
}RGBColor_t;


#define E_OK 				0x00

#define E_Already_Initialised 		0x01

#endif  /*_COMMON_TYPES_*/
