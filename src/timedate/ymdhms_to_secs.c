/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ------------------------ daysToMonthStart ---------------------------------

   Return the number of days up to the start of 'month' in 'year' (which may
   be a leap year).

   'monthIdx'  is 1 - 12 for Jan -> Dec.
*/

PRIVATE S16 daysToMonthStart(U8 month, U16 year) {
   return
      DaysToMonthStartTbl[month-1] +               // days to e.g May 1st AND
      ( IsaLeapYear(year) == true && (month-1) >= 2 ? 1 : 0);  // add a day if a leap year AND March onwards.
}



/* --------------------------- days2000ToYear ---------------------------------

   Days from 1st Jan 2000 to 1st Jan of 'year'
*/

PRIVATE U32 days2000ToYear_wo_Centurial(U16 year) {
   return
      year < _2000AD
         ? 0
         : (((365+365+365+366) * (U32)((year-_2000AD) / 4)) // Count by 4-years from 2000 (a leap year)
            + (year % 4 == 1                             // then one more year is a leap year
               ?  366
               : (year % 4 == 2                          // leap year plus regular year
                  ? 366+365
                  : (year % 4 == 3                       // leap year plus 2 regular years.
                     ? 366+365+365
                     : 0 ))));                           // else no extra years.
}

/* Centurial leap year correction.

   1600, 2000 & 2400 are leap years but 1900, 2100 & 2300 are not. S_DateTime spans 2000AD
   to 2136, so we must account for no Feb 29th 2100.
*/
PRIVATE U32 days2000ToYear(U16 year) {
   U32 d = days2000ToYear_wo_Centurial(year);
   return year > 2100 ? d-1 : d; }              // If 2101 onwards then subtract 1 day for missing Feb 29th.

/* -------------------------------- YMDHMS_To_Secs --------------------------------

   Given a S_DateTime 'dt' return a count of seconds since 00:00:00 (midnight) on 1st Jan 2000.

   If 'dt' is illegal then return 0 i.e 1/1/2000 0:0:0. Already past so can never be.
*/

PUBLIC T_Seconds32 YMDHMS_To_Secs(S_DateTime const *dt) {
   return
      Legal_YMDHMS(dt) == false
         ? 0
         : dt->sec + (60 * dt->min) + (3600L * dt->hr) +
               24*3600L * ((dt->day-1) + daysToMonthStart(dt->mnth, dt->yr) + days2000ToYear(dt->yr)); }

// -------------------------------- eof ---------------------------------------

