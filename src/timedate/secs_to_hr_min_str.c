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

   Print 'secsCnt' into mm:ss, e.g  23:17,  923:04:01

   Returns the length of the resulting 'strOut', which is 5.
*/
PUBLIC C8 const HMFormatter[] = "%02d:%02d";  // Make public to avoid ARM gcc linker bug.

PUBLIC U8 SecsToHrMinStr(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 59:59.
   secsCnt = MinU32(secsCnt, 60*59 + 59);

   /* Note: must evaluate hours, minutes, secs explicitly because 'mins' uses 'hours'
      and 'secs' uses 'mins' AND the evaluation order of the expressions passed to
      a function is not guaranteed.
   */
   U8 mins   = secsCnt/60;
   U8 secs   = secsCnt - (60 * (U32)mins);
   U8 rtn;

   // Make sure resulting string is no more than "hhhh:mm:ss".
   rtn = MinS16(sprintf(strOut, HMFormatter, mins, secs), 10);
   strOut[6] = '\0';
   return rtn;
}

/* ------------------------ SecsToHrMinStrRtn ----------------------------

    Same as above but chains 'strOut'.
*/
PUBLIC C8 const * SecsToHrMinStrRtn(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 9999:59:59.
   secsCnt = MinU32(secsCnt, 60*59 + 59);

   /* Note: must evaluate hours, minutes, secs explicitly because 'mins' uses 'hours'
      and 'secs' uses 'mins' AND the evaluation order of the expressions passed to
      a function is not guaranteed.
   */
   U8 mins   = secsCnt/60;
   U8 secs   = secsCnt - (60 * (U32)mins);

   // Make sure resulting string is no more than "hhhh:mm:ss".
   sprintf(strOut, HMFormatter, mins, secs);
   strOut[6] = '\0';
   return strOut;
}


// ------------------------------ eof -----------------------------------
