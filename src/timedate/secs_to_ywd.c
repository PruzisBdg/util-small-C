/*---------------------------------------------------------------------------
|
| SecsToYWD()
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* -------------------------- DaysToYWD -----------------------------------------

   Given 'daysSince2000AD', days elapsed since Midnite Dec 31 1999 return the
   ISO8601 Week-Date.

   'daysSince2000AD' is days (fully) elapsed, so Jan 1st 2000 is 0 (zero).
*/
PUBLIC S_WeekDate const * DaysToYWD(U32 daysSince2000AD, S_WeekDate *wd)
{
   U8    yearsRem;
   U16   yr4, daysRem;

   #define _4yr_days (365+365+365+366)

   yr4     = daysSince2000AD/_4yr_days;                           // Divide total secs into 4-year chunks...
   daysRem = daysSince2000AD - ((U32)yr4 * _4yr_days);                                  // ... which are 0 - 1461 days left over.

   /* Centurial leap year correction.

      1600, 2000 & 2400 are leap years but 1900, 2100 & 2300 are not. S_DateTime spans 2000AD
      to 2136, so we must account for no Feb 29th 2100.
   */
   #define _Midnite_Feb28_2100 (4107542399 - _12am_Jan_1st_2000_Epoch_secs)
   #define _Feb28_2100 (_Midnite_Feb28_2100/(24*3600L))

   if(daysSince2000AD > _Feb28_2100) {                            // Past Feb 28th 2100?
      daysRem += 1;                                               // there's no Feb 29th; pre-add the 1 day which will be subtracted (below).
      if(daysRem >= _4yr_days-1) {                                // Another whole 4 years?
            yr4 += 1;                                             // then bump 4-year count.
            daysRem = 0; }                                        // No days left over
      else {
         daysRem += 1; }}                                         // else bump the day count.

   yearsRem =                                                     // ... or 0 - 3 years left over
      daysRem <= 366-1
         ? 0
         : (daysRem <= (366+365-1)
            ? 1
            : (daysRem <= (366+365+365-1) ? 2 : 3));

   wd->yr = _2000AD + (4*yr4) + yearsRem;                         // Year is 2000AD + 4 x 4-year chunks + years-left-over

   daysRem =                                                      // Days left over after the last year is...
      daysRem -                                                   // days remaining after 4-year chunks, minus...
      (yearsRem == 3                                              // the number of days in the (0-3) left-over years
         ? (366+365+365)
         : (yearsRem == 2
            ? (366+365)
            : (yearsRem == 1
               ? 366
               : 0)));

   U16 daysThisYr = yearsRem == 0 ? 366 : 365;
   U16 remThisYr = daysThisYr - daysRem;

   // Jan 1st 2000AD is Sat. So Week 01 2000AD starts on Mon 3rd. (ISO 8601 weeks are Mon-Sun).
   #define _Days_To_Week01_2000AD   2  // i.e bypass Sat 1st & Sun 2nd.

   /* The days before the first business week in 2000AD are the last business week of 1999.
      Jan 1st 1999 is a Friday but since Jan 1st is a holiday ISO week 1 starts Mon Jan 4th
      Therefore Dec 31st 1999 is Fri of ISO week 52 (some leap years have 52 ISO weeks).
   */
   if(daysSince2000AD < _Days_To_Week01_2000AD) {
      wd->week = 53;
      wd->day = 7 + daysSince2000AD - _Days_To_Week01_2000AD + 1;    // i.e (6 - daysSince2000AD)
   }
   // Otherwise for Monday Jan 3rd 2000AD and onward...
   else {
      /* Calculate the number of ISO weekdays in this year which were carried over from previous
         year (below).

         At the start of 2000AD (the beginning of our 32bit time), there are 2 ISO weekdays
         carried over from 1999 (Sat&Sun; '_Days_To_Week01_2000AD' defined above). First ISO
         week of our reckoning starts on Mon Jan 3rd 2000AD . Count forward by weeks from there.

                                                                  |
            1/1/2000 ---------------- daysSince2000AD -----------------|-----------------------> today
                                                                       |
            1/1/2000 +2 -> 1/3/2000                                    |------ daysRem -------->
                                |                                      |
                                |-------/----- ISO weeks -/-------/----|---/
                                |                                 |    |   |
                                |<------- (since - rem - 2) ------|--->|   |
                                |                                 |    |   |
                                |          (since-rem-2) mod 7 -->|    |<--|---
                                |                                      |   |
                                        7 - ((since-rem-2) mod 7)  --->|   |<---

           Modulus arithmetic on signed types mirrors at zero (i.e it's not modular; programmers
           are not mathematicians). So to keep it positive in week 1 of 2000A add 7 i.e

                 7 - ((since-rem-2) mod 7) -> 7 - ((7+since-rem-2) mod 7)
      */
      U16 weekdaysCarriedOver = (7 - ((7 + (S32)daysSince2000AD - _Days_To_Week01_2000AD - daysRem) % 7))%7;

      /* ISO Week 01 is
            - the week with the 1st business day of the year (Jan 1st is a holiday) OR
            - the week with 4 January in it,

         So ISO Week 01 can start in Dec of the previous year, i.e:

               Dec             Jan
           27 28  29 30 31 1st 2  3 4th                 carried- carried-  1st whole
                                                          over    under    ISO week
                            M  T  W  T  F  S  S  M         0        7        2
                            01 01 01 01 01 01 01 02

                         M  T  W  T  F  S  S  M            6        1        2
                         01 01 01 01 01 01 01 O2

                      M  T  W  T  F  S  S  M               5        2        2
                      01 01 01 01 01 01 01 02

                   M  T  W  T  F  S  S  M                  4        3        2
                   01 01 01 01 01 01 01 02

                M  T  W  T  F  S  S  M                     3        4        1
                p  p  p  p  p  p  p  01

             M  T  W  T  F  S  S  M                        2        5        1
             p  p  p  p  p  p  p  01

          M  T  W  T  F  S  S  M                           1        6        1
          p  p  p  p  p  p  p  01

          where 'p' is last ISO week of previous year, usually 52
      */
      U16 weekdaysToFinishYr =  (7 + ((S32)daysSince2000AD - _Days_To_Week01_2000AD - daysRem + daysThisYr)) % 7;

      printf("since2000 %u daysThisYr %u remThisYr %u carried over %u finish this yr %u\r\n",
           daysSince2000AD, daysRem, remThisYr, weekdaysCarriedOver, weekdaysToFinishYr);

      U16 daysRem_After1stISOWeek = daysRem - weekdaysCarriedOver;

      U8 firstFullIsoWeekThisYr = weekdaysCarriedOver <= 3 ? 1 : 2;

      /* Start of a new year; the week carried does NOT enter a new business year

         So we are still in last ISO week of previous year.
      */
      if(weekdaysCarriedOver <= 3 && daysRem < weekdaysCarriedOver) {
         wd->week = 52;
         wd->day = weekdaysCarriedOver;
      }
      /* Start of new year; the week carried over DOES enter a new business year.

         So we are already in Week 01 of this new year
      */
      else if(daysRem < weekdaysCarriedOver) {
         wd->week = 1;
         wd->day = 1 + 7 + daysRem - weekdaysCarriedOver;
      }
      /* End of previous year; the week will carry over into the next business year

         So this is already Week 01 of the next year, even though its still December.
      */
      else if(weekdaysToFinishYr <= 3 && remThisYr <= weekdaysToFinishYr) {
         wd->week = 1;
         wd->day = 1 + (daysRem_After1stISOWeek % 7);
      }
      // else somewhere in the middle of a year.
      else {
         // ISO weeks start at 1 so add 1.
         wd->week = firstFullIsoWeekThisYr + (daysRem_After1stISOWeek/7);

         // ISO weekdays are 1-7 so add 1.
         wd->day = 1 + (daysRem_After1stISOWeek % 7);
      }
   }

   return wd;

}

// -----------------------------------------------------------------------------------
PUBLIC S_WeekDateTime const * SecsToYWD(T_Seconds32 secsSince2000AD, S_WeekDateTime *dt)
{
   U8    yearsRem;
   U16   yr4, daysRem, allDays;
   U32   secsRem;

   #define _4yr_days (365+365+365+366)
   #define _4yr_secs (3600L*24*_4yr_days)                         // Seconds in 4 years

   allDays = secsSince2000AD/(3600L*24);                           // Whole number of days.
   yr4     = secsSince2000AD/_4yr_secs;                           // Divide total secs into 4-year chunks...
   secsRem = secsSince2000AD - ((U32)yr4 * _4yr_secs);            // and (0 - 126,230,400) seconds. ...

   daysRem = secsRem/(3600L*24);                                  // ... which are 0 - 1461 days left over.

   /* Centurial leap year correction.

      1600, 2000 & 2400 are leap years but 1900, 2100 & 2300 are not. S_DateTime spans 2000AD
      to 2136, so we must account for no Feb 29th 2100.
   */
   #define _Midnite_Feb28_2100 (4107542399 - _12am_Jan_1st_2000_Epoch_secs)

   if(secsSince2000AD > _Midnite_Feb28_2100) {                    // Past midnite Feb 28th 2100?
      secsRem += (24*3600UL);                                     // there's no Feb 29th; pre-add the 1 day which will be subtracted (below).
      if(daysRem >= _4yr_days-1) {                                // Another whole 4 years?
            yr4 += 1;                                             // then bump 4-year count.
            daysRem = 0;                                          // No days left over
            secsRem -= _4yr_secs; }                               // 4 years less of seconds.
      else {
         daysRem += 1; }}                                         // else bump the day count.

   yearsRem =                                                     // ... or 0 - 3 years left over
      daysRem <= 366-1
         ? 0
         : (daysRem <= (366+365-1)
            ? 1
            : (daysRem <= (366+365+365-1) ? 2 : 3));

   dt->ywd.yr = _2000AD + (4*yr4) + yearsRem;                     // Year is 2000AD + 4 x 4-year chunks + years-left-over

   daysRem =                                                      // Days left over after the last year is...
      daysRem -                                                   // days remaining after 4-year chunks, minus...
      (yearsRem == 3                                              // the number of days in the (0-3) left-over years
         ? (366+365+365)
         : (yearsRem == 2
            ? (366+365)
            : (yearsRem == 1
               ? 366
               : 0)));

   // Jan 1st 2000AD is Sat. So Week 01 2000AD starts on Mon 3rd. (ISO 8601 weeks are Mon-Sun).
   #define _Days_To_Week01_2000AD   2  // i.e bypass Sat 1st & Sun 2nd.
}

