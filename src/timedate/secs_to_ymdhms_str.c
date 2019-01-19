/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* -------------------------- SecsTo_YMDHMS_Str ---------------------------------

   Given 'secs' since 1st Jan 2000AD, return an ISO8601 date/time in 'strOut',
   e.g  "2002-04-17T03:43:12"
*/

PUBLIC C8 * SecsTo_YMDHMS_Str(C8 *strOut, T_Seconds32 secs) {
   S_DateTime dt;
   SecsToYMDHMS(secs, &dt);
   YMDHMS_ToStr(&dt, strOut);
   return strOut;
}

// ------------------------------------ eof ---------------------------------------
