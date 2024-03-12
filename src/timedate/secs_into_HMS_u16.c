/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ------------------------- Secs_IntoHMS_U16 --------------------------------

   Given 'secs', return from 0secs to 16383 hrs encoded into U16.

    <b15:14 time_units,  b13:0 count> where
       time_units  00b = secs, 01b = minutes, 10b = hrs.

   Return 0 if 'secs' is negative; 0xFFFF if 'secs' is too large to be encoded. This keeps strict
   ordering i.e Secs_IntoHMS_U16() will never return a smaller number for a larger 'secs'.


   Note Secs_IntoHMS_U16() rounds down minutes and hours, e,g returns 5000 minutes
   for 5000:00 -> 5000:59.
*/
PUBLIC U16 Secs_IntoHMS_U16(S32 secs)
{
   return
      secs < 0                        ? 0x0 :(                          // Negative? Cannot encode. For strict ordering return as 0
      secs < (1U << 14)               ? (U16)secs :(                    // < 16384 secs? encode as seconds.
      secs < (60 * (U32)(1U << 14))   ? (0x4000 | (U16)(secs / 60)) :(  // < 16384 mins? encode as minutes
      secs < (3600 * (U32)(1U << 14)) ? (0x8000 | (U16)(secs / 3600))   // < 16384 hrs?  encode as hrs.
                                      : (0xFFFF))));                    // else >= 16384hrs, cannot encode.
}
