#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* -------------------------------- Full_YMDHMS_AddSecs -------------------------------

   Add 'secs' to 'dt, result in 'out'. 'secs' is signed, so may be negative.

   Same as YMDHMS_AddSecs() but is not limited to the 32-bit S_DateTime range 2000AD
   to 2136AD.

   'dt' and 'out' may reference the same object.
*/

PUBLIC S_DateTime const * Full_YMDHMS_AddSecs(S_DateTime *out, S_DateTime const *in, S32 secs) {

   /* Get a multiple of 4 years to bring 'in' to 2000-2004AD so can use Full_YMDHMS_AddSecs() to
      add 'secs'.
   */
   S_DateTime dt = *in;                               // A local copy because 'in' is const.

   /* If 'secs' is positive then adjust dt->yr to > ~2000AD so that sum will be [2000...2068];
      otherwise (secs is negative) adjust to < ~2072AD so that sum is [2004..2072].

      Note that we choose these ranges to stay below 2100 because 2100 is NOT a leap year.
      If we fold a calculation which does not span 2100 into a range which includes 2100 and
      give this calculation to YMDHMS_AddSecs() the result will be 1 day off because 2100 is
      366 days.

      'ofs4yr' is the smallest multiple of 4 years which brings the dt->yr into the correct
      range for summation.
   */
   S16 n =
      (secs > 0
         ? (_2000AD - dt.ymd.yr)
         : ( 2096 - dt.ymd.yr));

   S16 ofs4yr = 4 * (n / 4);

   dt.ymd.yr += ofs4yr;

   /* Bump 'n' to avoid round-down. So we don't get e.g
         4*((2000-1998)/4) -> 0
      which would leave 1998 at 1998 + 0 -> 1998, which is outside 2000AD -> 2136AD.
   */
   if(dt.ymd.yr < 2068) {
      dt.ymd.yr += 4;
      ofs4yr += 4; }
   else {
      dt.ymd.yr -= 4;
      ofs4yr -= 4; }

   YMDHMS_AddSecs(&dt, &dt, secs);

   dt.ymd.yr -= ofs4yr;
   *out = dt;
   return out; }

// ------------------------------------ eof ------------------------------------------
