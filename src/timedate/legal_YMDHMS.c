/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ---------------------------- Legal_YMDHMS ------------------------------------

   Return TRUE if 't' is a legal date and time for use as a 32 bit internal time which
   starts at 200AD.. Hours, minutes etc must be legal; the year must be 2000AD - 2130AD.

   Also allows wildcards e.g _YMD_AnyYear (0xFEFE) -> every year.
*/
PUBLIC BOOLEAN Legal_YMDHMS(S_DateTime const *t)
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wshadow"
   
   // 2136-2-7T06:28:15 is the last of 32 bit secs.
   BOOLEAN pastFinalSecsOnLastDay(S_DateTime const *t) {
      return
         t->ymd.yr == 2136 && t->ymd.mnth == 2 && t->ymd.day == 7 &&    // 2136-2-7? AND
         (
            t->hr > 6 ||                                                // 7am or later? OR
            (
               t->hr == 6 &&                                            // 6:28:15 or later?
               (
                  t->min > 28 ||
                  (t->min == 28 && t->sec > 15)
               )
            )
         ) ? true : false; }

   #pragma GCC diagnostic pop
   
   return
      pastFinalSecsOnLastDay(t) == true
         ? false
         : (Legal_HMS( &(S_TimeHMS const){.hr = t->hr, .min = t->min, .sec = t->sec} ) == true &&
            Legal_YMD( &(S_YMD const){.yr = t->ymd.yr, .mnth = t->ymd.mnth, .day = t->ymd.day}  ) == true);
}


// ----------------------------------- eof ----------------------------------------
