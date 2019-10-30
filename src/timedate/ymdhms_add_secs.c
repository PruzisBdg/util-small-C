#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* -------------------------------- YMDHMS_AddSecs ------------------------------------

   Add 'secs' to 'dt, result in 'out'. 'secs' is signed, so may be negative.

   'dt' and 'out' may reference the same object.

   'dt' must be in the 'legal' S_DateTime' range 2000AD to 2136. 'out' is clipped to the
   same range. For a function which adds outside that range use YMDHMS_AddSecs_Full().
*/
PUBLIC S_DateTime const * YMDHMS_AddSecs(S_DateTime *out, S_DateTime const *dt, S32 secs) {
   return
      SecsToYMDHMS(
         U32plusS32_toU32(
            YMDHMS_To_Secs(dt), secs),
            out ); }

// ------------------------------------ eof ------------------------------------------
