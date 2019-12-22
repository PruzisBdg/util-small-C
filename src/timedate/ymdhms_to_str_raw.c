/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

/* ----------------------- YMDHMS_ToStr ---------------------------------

   Make S_DateTime 't' into an ISO8601 date+time string in 'outStr' e.g "2002-03-07T14:21:05".

   The function doesn't check if 't' is a legal date time. Use legalDateTime().

   Returns: the number the printed 'out'
*/

PUBLIC C8 const *YMDHStoStr_Raw(C8 *out, S_DateTime const *dt) {
   sprintf(out, "%04d-%02d-%02dT%02d:%02d:%02d", dt->ymd.yr, dt->ymd.mnth, dt->ymd.day, dt->hr, dt->min, dt->sec);
   return out; }

// --------------------------------- eof ---------------------------------------------------

