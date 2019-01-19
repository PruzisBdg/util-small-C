/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* ------------------------- SecsToHMS --------------------------------

   Split seconds into H:M:S, result in a S_TimeHMS which limits hours to
   65535 (U16).
*/

PUBLIC void SecsToHMS(T_Seconds32 secs, S_TimeHMS *hms) {

   secs = MinU32(secs, 3600*MAX_U16 + 60*59 + 59);          // Limit to 65535 hrs, 59 minutes, 59 secs.

   hms->hr = secs/3600;                                     // Whole hours
   hms->min = (secs - (3600L * hms->hr)) / 60;              // minutes left over (subtract hour*3600)
   hms->sec = secs - (3600L * hms->hr) - (60 * hms->min);   // Secs left over (subtract hour*3600 + minutes*60)
   
}


// ----------------------------- eof ------------------------------
