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

   yr4     = daysSince2000AD/_4yr_days;                           // Divide total days into 4-year chunks...
   daysRem = daysSince2000AD - ((U32)yr4 * _4yr_days);            // ... which are 0 - 1461 days left over.

   /* Centurial leap year correction.

      1600, 2000 & 2400 are leap years but 1900, 2100 & 2300 are not. S_DateTime spans 2000AD
      to 2136, so we must account for no Feb 29th 2100.
   */
   #define _Midnite_Feb28_2100 (4107542399 - _12am_Jan_1st_2000_Epoch_secs)
   #define _Feb28_2100 (_Midnite_Feb28_2100/(24*3600L))

   #define _Century (25UL*(366+365+365+365)
#if 1
   if(daysSince2000AD > _Feb28_2100) {                            // Past Feb 28th 2100?
      daysRem += 1;                                               // there's no Feb 29th; pre-add the 1 day which will be subtracted (below).
      if(daysRem >= _4yr_days-1) {                                // Another whole 4 years?
            yr4 += 1;                                             // then bump 4-year count.
            daysRem = 0; }}                                        // No days left over
#else
   if(daysSince2000AD > _Century+ (4*365)) {
      daysRem += 1;                                               // there's no Feb 29th; pre-add the 1 day which will be subtracted (below).
      if(daysRem >= _4yr_days-1) {                                // Another whole 4 years?
            yr4 += 1;                                             // then bump 4-year count.
            daysRem = 0; }}                                        // No days left over

#endif
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

   U16 daysThisYr = yearsRem == 0 && wd->yr != 2100 ? 366 : 365;
   U16 remThisYr = daysThisYr - daysRem;

   /* ---- Week-Day (easy)

      Jan 1st 2000AD is Sat. So Week 01 2000AD starts on Mon 3rd. (ISO 8601 weeks are Mon-Sun).

      After that, to get the day just count by 7's
   */
   #define _Days_To_Week01_2000AD   2  // i.e bypass Sat 1st & Sun 2nd.

   // ISO weekdays are 1-7 so add 1.
   wd->day = 1+(7 + daysSince2000AD - _Days_To_Week01_2000AD)%7;

   /* ---- Week & Year (must align with holidays and Gregorian calender, so complicated)

      Get the number of ISO weekdays in this year which were carried over from previous
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
                         |                             over    under    ISO week
                         M  T  W  T  F  S  S  M         0        7        2
                         01 01 01 01 01 01 01 02
                         |
                      M  T  W  T  F  S  S  M            6        1        2
                      01 01 01 01 01 01 01 O2
                         |
                   M  T  W  T  F  S  S  M               5        2        2
                   01 01 01 01 01 01 01 02
                         |
                M  T  W  T  F  S  S  M                  4        3        2
                01 01 01 01 01 01 01 02
                         |
             M  T  W  T  F  S  S  M                     3        4        1
             p  p  p  p  p  p  p  01
                         |
          M  T  W  T  F  S  S  M                        2        5        1
          p  p  p  p  p  p  p  01
                         |
       M  T  W  T  F  S  S  M                           1        6        1
       p  p  p  p  p  p  p  01

       where 'p' is last ISO week of previous year, usually 52
   */
   U16 weekdaysToFinishYr =  (7 + ((S32)daysSince2000AD - _Days_To_Week01_2000AD - daysRem + daysThisYr)) % 7;

   printf("since2000 %u daysThisYr %u remThisYr %u carried over %u finish this yr %u\r\n",
        daysSince2000AD, daysRem, remThisYr, weekdaysCarriedOver, weekdaysToFinishYr);

   /* Start of a new year; and the last week of the previous year does NOT enter a new business
      year

      We are still in last ISO week of previous year.., so this week must be labeled with the
      PREVIOUS year that it started in.

      This last week was either ISO week 52 or 53, depending (below) on the numbers of days
      carried over and whether the previous year was a leap year.

                           For a non-leap year
                    53 weeks if days carried E (3,4,5,6)
         Jan                                                Dec  carried yr starts last
         1st (is a holiday)                                 31st  over   on week#  week#

         |S| _____________________ 52wks/364days ____________ |    0       p     0+52   = 52
         | _____________________ 52/364 ____________________|M|    6       1     52+1   = 53
         |T|W|T|F|S|S| _________ 51/357 __________________|M|T|    5       1     1+51+1 = 53
         |W|T|F|S|S| _________ 51/357 __________________|M|T|W|    4       1     1+51+1 = 53
         |T|F|S|S| _________ 51/357 __________________|M|T|W|T|    3       1     1+52+1 = 53
         |F|S|S| _________ 51/357 __________________|M|T|W|T|F|    2       p     0+51+1 = 52
         |S|S| _________ 51/357 __________________|M|T|W|T|F|S|    1       p     0+51+1 = 52


                             For a leap year
                  53 weeks if days-carried E (2,3,4,5,6)
         Jan                                                Dec  carried yr starts last
         1st (is a holiday)                                 31st  over   on week#  week#

         |S| _____________________ 52wks/364days ____________ |M|    6       p   0+52+1 = 53
         | _____________________ 52/364 ____________________|M|T|    5       1   52+1   = 53
         |T|W|T|F|S|S| _________ 51/357 __________________|M|T|W|    4       1   1+51+1 = 53
         |W|T|F|S|S| _________ 51/357 __________________|M|T|W|T|    3       1   1+51+1 = 53
         |T|F|S|S| _________ 51/357 __________________|M|T|W|T|F|    2       1   1+51+1 = 53
         |F|S|S| _________ 51/357 __________________|M|T|W|T|F|S|    1       p   0+51+1 = 52
         |S|S| _________ 51/357 __________________|M|T|W|T|F|S|S|    0       p   0+51+1 = 52
   */
   if(weekdaysCarriedOver <= 3 && daysRem < weekdaysCarriedOver)           // Start of year, current week does NOT start a new business year?
   {
      wd->yr--;                                                            // then this week belongs to the previous year; back up one.

      bool prevYrWasLeapYr = yearsRem == 1 ? true : false;                 // In 2nd year (of 4)? => previous was leap year

      wd->week =                                                           // Week is? ... see above.
         weekdaysCarriedOver >= (prevYrWasLeapYr == true ? 2 : 3) &&
         weekdaysCarriedOver <= 6
            ? 53 : 52;
   }
   /* Start of new year; the week carried over DOES enter a new business year.
   */
   else if(daysRem < weekdaysCarriedOver) {
      wd->week = 1; }

   /* End of previous year; the week will carry over into the next business year

      So this is already Week 01 of the next year, even though its still December.
      It must be labeled with the NEXT year.
   */
   else if(weekdaysToFinishYr <= 3 && remThisYr <= weekdaysToFinishYr) {
      wd->yr++;
      wd->week = 1; }

   // else somewhere in the middle of a year.
   else {
      U16 daysRem_After1stISOWeek = daysRem - weekdaysCarriedOver;
      /* Jan 1st is holiday so an ISO week which straddles 2 years must have at least
         Thurs Fri Sat Sun (4 days) in the new year. This puts Fri is on Jan 2nd which
         is a work day and so that week becomes ISO week 01. If Week 01 straddles
         then Week 02 is the first full one.
      */
      U8 firstFullIsoWeekThisYr = weekdaysCarriedOver <= 3 ? 1 : 2;

      // ISO weeks start at 1 so add 1.
      wd->week = firstFullIsoWeekThisYr + (daysRem_After1stISOWeek/7);
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

