/*---------------------------------------------------------------------------
|
| From SysTime to printouts of elapsed HMS & centi-secs.
|
| Uses SystemTicks_perSec() supplied by the Host App.
|
|--------------------------------------------------------------------------*/


#include "libs_support.h"
#include "util.h"
#include "systime.h"

/* ---------------------------------- T_Time_to_HMS_centiSecs -------------------------------------

   Print e.g "04:17:21.92:" into 'out'.  'out->cnt' is the (chars) space available to print; on return
   'out->cnt' is the number of chars actually printed. If there's not enough
*/
PUBLIC S_BufC8 const * T_Time_to_HMS_centiSecs(S_BufC8 *out, T_Time tm) {
   C8 hms[_HMSStr_Maxlen+1];

   U32 secs = tm/SystemTicks_perSec();
   U16 csec = tm - (SystemTicks_perSec() * secs);
   SecsToHMSStr(secs, hms);
   return Print_BufC8(out, "%s.%02u:", hms, csec); }


/* ------------------------------ T_Time_to_secs_centiSecs -------------------------------------------

   As T_Time_to_HMS_centiSecs() above but if 'tm' is < 60secs print just e.g '3.72s'
*/
PUBLIC S_BufC8 const * T_Time_to_secs_centiSecs(S_BufC8 *out, T_Time tm) {

   U32 secs = tm/SystemTicks_perSec();
   U16 csec = tm - (SystemTicks_perSec() * secs);

   if(secs > 59) {
      C8 hms[_HMSStr_Maxlen+1];
      SecsToHMSStr(secs, hms);
      return Print_BufC8(out, "%s.%02u:", hms, csec); }
   else {
      return Print_BufC8(out, "%lu.%02us", secs, csec); }}

// ----------------------------------- eof -----------------------------------------------
