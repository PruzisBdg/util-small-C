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
PUBLIC C8 const HMFormatter[] = "P%02d:%02d";  // Make public to avoid ARM gcc linker bug.

PUBLIC U8 SecsToHrMinStr(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 23:59.
   secsCnt = MinU32(secsCnt, 60*(23*60 + 59));

   U8 hrs   = secsCnt/3600;
   U8 mins  = (secsCnt - (3600 * (U32)hrs))/60;

   return sprintf(strOut, HMFormatter, hrs, mins);
}

/* ------------------------ SecsToHrMinStrRtn ----------------------------

    Same as above but returns 'strOut'.
*/
PUBLIC C8 const * SecsToHrMinStrRtn(T_Seconds32 secsCnt, C8 *strOut) {
   SecsToHrMinStr(secsCnt, strOut);
   return strOut;
}


/* -------------------------- SecsToDHMinStr ----------------------------

   Up to 9999 days 23hrs 59 minutes
*/
PUBLIC C8 const DHMFormatter[] = "P%02uD%02dH%02dM";  // Make public to avoid ARM gcc linker bug.

PUBLIC S_BufC8 const * SecsToDHMStr(T_Seconds32 secsCnt, S_BufC8 *out) {
   // limit to 9999:23:59
   secsCnt = MinU32(secsCnt, 60*(((9999*24UL) + 23)*60 + 59));

   U16 days = secsCnt/(3600*24);
   U32 dysec = (3600*24*(U32)days);
   U8  hrs  = (secsCnt - dysec)/3600;
   U8  mins = (secsCnt - dysec - (3600*(U32)hrs))/60;

   out->cnt = MinU16(out->cnt, snprintf(out->cs, out->cnt, DHMFormatter, days, hrs, mins));
   return out;
}


// ------------------------------ eof -----------------------------------
