/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <stdio.h>


/* ------------------------ SecsToHrMinStr ----------------------------

   Print 'secsCnt' into hh:mm, e.g  23:17

   Returns the length of the resulting 'strOut', which is 5.
*/
PUBLIC C8 const HMFormatter[] = "%02d:%02d";  // Make public to avoid ARM gcc linker bug.

PUBLIC U8 SecsToHrMinStr(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 59:59.
   secsCnt = MinU32(secsCnt, 60*(23*60 + 59));

   U8 hrs   = secsCnt/3600;
   U8 mins  = (secsCnt - (3600 * (U32)hrs))/60;

   return sprintf(strOut, HMFormatter, hrs, mins);
}

/* ------------------------ SecsToHrMinStrRtn ----------------------------

    Same as above but returns 'strOut'.
*/
PUBLIC C8 const * SecsToHrMinStrRtn(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 59:59.
   secsCnt = MinU32(secsCnt, 60*(23*60 + 59));

   U8 hrs   = secsCnt/3600;
   U8 mins  = (secsCnt - (3600 * (U32)hrs))/60;

   sprintf(strOut, HMFormatter, hrs, mins);
   return strOut; }


// ------------------------------ eof -----------------------------------
