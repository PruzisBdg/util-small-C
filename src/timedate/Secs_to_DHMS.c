/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* ------------------------- SecsToDHMS --------------------------------

   Split seconds into D:H:M:S, result in a S_TimeDHMS which limits hours to
   9999 (U16).
*/
PUBLIC S_TimeDHMS const * SecsToDHMS(T_Seconds32 secs, S_TimeDHMS *dhms)
{
   // limit to 9999:23:59:59
   #define _9999D23H59M59S  ((60*(((9999*24UL) + 23)*60 + 59))+59)
   secs = MinU32(secs, _9999D23H59M59S);

   dhms->days = secs/(3600*24);                                   // Whole days
   U32 dysec  = (3600*24*(U32)dhms->days);                        // Whole days as secs
   dhms->hr   = (secs - dysec)/3600;                              // hrs remainder
   U32 hrsec  = dhms->hr * 3600UL;                                // Whole hrs (remainder) as secs
   dhms->min  = (secs - dysec - hrsec)/60;                        // Whole minutes remainder
   dhms->sec  = secs - dysec - hrsec - (60 * (U16)dhms->min);     // Seconds remainder.
   return dhms;
}


// ----------------------------- eof ------------------------------
