/** @file cmdutils.h

 @author Wassim FILALI  STM8 S

 @compiler IAR STM8


 $Date: 18.11.2016
 $Revision:

*/

#ifndef __CMDUTILS__
#define __CMDUTILS__

//for BYTE
#include "commonTypes.h"

BYTE strcmp(BYTE * s1, const char * s2);

BYTE strbegins(BYTE * s1, const char * s2);

BYTE get_hex_char(BYTE c);

BYTE get_hex(BYTE* buffer,BYTE pos);

BYTE line_length(BYTE*rxData,BYTE max_size);

#endif /*__CMDUTILS__*/
