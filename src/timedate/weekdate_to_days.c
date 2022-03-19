
#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* -------------------------- WeekDateToDays -----------------------------------------

   Given IOS Week-Date 'wd' between 2000AD and Tues Feb 7th 2136 (Gregorian), return
   days elapsed since start of 2000AD.

   Returns _Illegal_Days16 (0xFFFF) if 'wd' was not the range above, which is the span
   of this modules 'T_Seconds32'.
*/
PUBLIC T_Days16 WeekDateToDays(S_WeekDate const *wd)
{
   if(Legal_WeekDate(wd) == false) {
      return _Illegal_Days16;
   }
   else if(wd->yr == 1999) {
      if(wd->week == 52) {
         if(wd->day == 6) {
            return 0;}
         else if(wd->day == 7) {
            return 1; }}

      return _Illegal_Days16; // GCOVR_EXCL_LINE. Neer reached because of Legal_WeekDate() above.
   }
   else {
      /* Start by assuming that the ISO week-year in 'wd' is the same as the Gregorian year.
         Usually it will be; only around new years might the ISO week-year be +/-1.
      */
      #define _4yr_days (366+365+365+365)
      #define _Century (25UL*(366+365+365+365))
      U16 yr = wd->yr - 2000;
      U16 yr4 = yr/4;
      U8 yrsRem = yr - (4 * yr4);

      // Days in the whole number of Gregorian years.
      U16 gregDays =
         (yr4 * _4yr_days) +
         (yrsRem == 3 ? (366+365+365) : (yrsRem == 2 ? (366+365): (yrsRem == 1 ? 366 : 0)));

      // 2100 is not a leap year. If past 2100 remove a day to correct for that.
      gregDays = gregDays > _Century+_4yr_days-1
            ? gregDays-1
            : gregDays;

      // Split whole Gregorian years into weeks plus 0-6 days.
      U16 weeks = gregDays/7;
      U8 daysRem = gregDays - (7*(T_Days16)weeks);

      /* Number of days in the 1st week of the final Gregorian year.

         Jan 1st 2000AD is Sat. So Week 01 2000AD starts on Mon 3rd. (ISO 8601 weeks are Mon-Sun).
         So add 2 days and subtract the final partial week
      */
      #define _Days_To_Week01_2000AD   2  // i.e bypass Sat 1st & Sun 2nd.

      U8 carriedOver = (7 - daysRem + _Days_To_Week01_2000AD) % 7;
      U8 daysIn1stWeek = carriedOver == 0 ? 7 : carriedOver;

      /* The 1st week of the Gregorian year will be either ISO week 1 or the last week of the
         previous ISO year, which will be 53 or 53, depending (below) on the numbers of days
         carried over and whether the previous year was a leap year.

                       The previous year - was a non-leap year
                       53 weeks if days carried E (3,4,5,6)
                                                                            prev yr             this yr
            Jan                                                Dec  carried  starts   last      starts
            1st (is a holiday)                                 31st  over   on week#  week#     on week#

            |S| _____________________ 52wks/364days ____________ |    0       p     0+52   = 52    1
            | _____________________ 52/364 ____________________|M|    6       1     52+1   = 53    1
            |T|W|T|F|S|S| _________ 51/357 __________________|M|T|    5       1     1+51+1 = 53    1
            |W|T|F|S|S| _________ 51/357 __________________|M|T|W|    4       1     1+51+1 = 53    1
            |T|F|S|S| _________ 51/357 __________________|M|T|W|T|    3       1     1+52+1 = 53    53
            |F|S|S| _________ 51/357 __________________|M|T|W|T|F|    2       p     0+51+1 = 52    52
            |S|S| _________ 51/357 __________________|M|T|W|T|F|S|    1       p     0+51+1 = 52    52


                      The previous year - was a leap year
                     53 weeks if days-carried E (2,3,4,5,6)
                                                                            prev yr             this yr
            Jan                                                Dec  carried  starts   last      starts
            1st (is a holiday)                                 31st  over   on week#  week#     on week#

            |S| _____________________ 52wks/364days ____________ |M|    6       p   0+52+1 = 53   1
            | _____________________ 52/364 ____________________|M|T|    5       1   52+1   = 53   1
            |T|W|T|F|S|S| _________ 51/357 __________________|M|T|W|    4       1   1+51+1 = 53   1
            |W|T|F|S|S| _________ 51/357 __________________|M|T|W|T|    3       1   1+51+1 = 53   53
            |T|F|S|S| _________ 51/357 __________________|M|T|W|T|F|    2       1   1+51+1 = 53   53
            |F|S|S| _________ 51/357 __________________|M|T|W|T|F|S|    1       p   0+51+1 = 52   52
            |S|S| _________ 51/357 __________________|M|T|W|T|F|S|S|    0       p   0+51+1 = 52   1
      */
      bool prevYrWasLeapYr = wd->yr % 4 == 1 || wd->yr == 2101;

      U8 firstWeek =
         daysIn1stWeek > 3
            ? 1
            : (daysIn1stWeek >= (prevYrWasLeapYr == true ? 2 : 3) ? 53 : 52);

      /* Get the number of days we are into this Gregorian year. Note that this can be negative if the
         Week-Date year started before the Gregorian year and haven't yet crossed into the Gregorian
         year.
      */
      S16 leftoverDays =
         daysIn1stWeek +
         (firstWeek == 1
            ? (7 * (U16)wd->week + wd->day - 14)
            : (7 * (U16)wd->week + wd->day - 7));

   printf("yr %u -> yr4 %u + yrsRem %u gregDays %u -> weeks %u + daysRem %u\r\n carriedOver %u prevYrLeap %u 1stweek %u leftoverDays %d\r\n",
          yr, yr4, yrsRem, gregDays, weeks, daysRem, carriedOver, prevYrWasLeapYr, firstWeek, leftoverDays);

      return gregDays + leftoverDays;
      }
}

// ========================================= EOF =============================================
