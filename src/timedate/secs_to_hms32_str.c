/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <stdio.h>


/* ------------------------ SecsToHMS32_Str ----------------------------

   Print 'secsCnt' into hh:mm:ss, with at least 2 hours digits but allowing
   a full 32bits of seconds to be counted as hours. That's 1,193,046hrs (7 digits)
      e.g  04:23:17,  923621:04:01

   Returns the length of the resulting 'strOut', which may be 8,9 or 10.
*/
PUBLIC C8 const HMS32_Formatter[] = "%7lu:%02d:%02d";  // Make public to avoid ARM gcc linker bug.

PUBLIC U8 SecsToHMS32_Str(T_Seconds32 secsCnt, C8 *strOut) {

   /* Note: must evaluate hours, minutes, secs explicitly because 'mins' uses 'hours'
      and 'secs' uses 'mins' AND the evaluation order of the expressions passed to
      a function is not guaranteed.
   */
   U32 hours = secsCnt/3600;
   U8 mins   = (secsCnt - (3600 * hours))/60;
   U8 secs   = secsCnt - (3600 * hours) - (60 * (U32)mins);

   // Make sure resulting string is no more than "hhhhhhh:mm:ss".
   U8 rtn = MinS16(sprintf(strOut, HMS32_Formatter, (unsigned long)hours, mins, secs), sizeof("hhhhhhh:mm:ss"));
   strOut[sizeof("hhhhhhh:mm:ss")] = '\0';
   return rtn;
}

/* ------------------------ SecsToHMS32_StrRtn ----------------------------

    Same as above but chains 'strOut'.
*/
PUBLIC C8 const * SecsToHMS32_StrRtn(T_Seconds32 secsCnt, C8 *strOut) {

   // Limit to 9999:59:59.
   secsCnt = MinU32(secsCnt, 3600L*24*9999 + 60*59 + 59);

   /* Note: must evaluate hours, minutes, secs explicitly because 'mins' uses 'hours'
      and 'secs' uses 'mins' AND the evaluation order of the expressions passed to
      a function is not guaranteed.
   */
   U32 hours = secsCnt/3600;
   U8 mins   = (secsCnt - (3600 * hours))/60;
   U8 secs   = secsCnt - (3600 * hours) - (60 * (U32)mins);

   sprintf(strOut, HMS32_Formatter, (unsigned long)hours, mins, secs);
   // Make sure resulting string is no more than "hhhh:mm:ss".
   strOut[sizeof("hhhhhhh:mm:ss")] = '\0';
   return strOut;
}


// ------------------------------ eof -----------------------------------
