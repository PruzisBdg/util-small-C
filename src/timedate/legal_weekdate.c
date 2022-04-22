#include "libs_support.h"
#include "util.h"

/* ----------------------- Legal_WeekDate ----------------------------------

   Return FALSE if 'wd.day' is not 1..7 OR 'wd.week' is not 1..53 OR if 'wd' is outside
   the range of 'T_Seconds32' of this Time/Date module, which spans from start of 2000AD
   to Tues Feb 7th 2136.

   Note this functions does NOT fail correctly-formed Week-Dates 2000AD-2136 which do not
   map to an actual Gregorian date. E,g 2000AD has no week 53 but 2000-W53-1 will be accepted
   as legal (for now).
*/
PUBLIC BOOL Legal_WeekDate(S_WeekDate const *wd)
{
   return
      Legal_WeekDateFields(wd) == FALSE ||
      WeekDate_A_GT_B(wd, &(S_WeekDate){.yr = 2136, .week = 6, .day = 1}) ||
      WeekDate_A_LT_B(wd, &(S_WeekDate){.yr = 1999, .week = 52, .day = 6})
         ? FALSE : TRUE;

}

/* ----------------------- Legal_WeekDateFields ----------------------------------

   Return FALSE if 'wd.day' is not 1..7 OR 'wd.week' is not 1..53.
*/
PUBLIC BOOL Legal_WeekDateFields(S_WeekDate const *wd) {
   return
      wd->day < 1 || wd->day > 7 ||
      wd->week < 1 || wd->week > 53
         ? FALSE : TRUE;
}

// --------------------------------- EOF -----------------------------------------
