/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <stdio.h>


/* ------------------------ SecsToHMSStr ----------------------------

   Print 'secsCnt' into hh:mm:ss, with at least 2 hours digits but
   allowing up to 4 hours digits if necessary
      e.g  04:23:17,  923:04:01

   Returns the length of the resulting 'strOut', which may be 8,9 or 10.
*/
PUBLIC C8 const HMSFormatter[] = "%02d:%02d:%02d";  // Make public to avoid ARM gcc linker bug.

PUBLIC U8 SecsToHMSStr(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 9999:59:59.
   secsCnt = MinU32(secsCnt, 3600L*24*9999 + 60*59 + 59);

   /* Note: must evaluate hours, minutes, secs explicitly because 'mins' uses 'hours'
      and 'secs' uses 'mins' AND the evaluation order of the expressions passed to
      a function is not guaranteed.
   */
   U16 hours = secsCnt/3600;
   U8 mins   = (secsCnt - (3600 * (U32)hours))/60;
   U8 secs   = secsCnt - (3600 * (U32)hours) - (60 * (U32)mins);
   U8 rtn;

   // Make sure resulting string is no more than "hhhh:mm:ss".
   rtn = MinS16(sprintf(strOut, HMSFormatter, hours, mins, secs), 10);
   strOut[10] = '\0';
   return rtn;
}

/* ------------------------ SecsToHMSStrOut ----------------------------

    Same as above but chains 'strOut'.
*/
PUBLIC C8 const * SecsToHMSStrRtn(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 9999:59:59.
   secsCnt = MinU32(secsCnt, 3600L*24*9999 + 60*59 + 59);

   /* Note: must evaluate hours, minutes, secs explicitly because 'mins' uses 'hours'
      and 'secs' uses 'mins' AND the evaluation order of the expressions passed to
      a function is not guaranteed.
   */
   U16 hours = secsCnt/3600;
   U8 mins   = (secsCnt - (3600 * (U32)hours))/60;
   U8 secs   = secsCnt - (3600 * (U32)hours) - (60 * (U32)mins);

   // Make sure resulting string is no more than "hhhh:mm:ss".
   sprintf(strOut, HMSFormatter, hours, mins, secs);
   strOut[10] = '\0';
   return strOut;
}


// ------------------------------ eof -----------------------------------
