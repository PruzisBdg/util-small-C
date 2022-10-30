/*---------------------------------------------------------------------------
|
| SecsToYMDHMS()
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

// Days to start/end of each month, for a non-leap year.
PUBLIC S16 const DaysToMonthStartTbl[] = {
   0,                               // Needs leading zero for search in splitDaysIntoMD[] below
   31,                              // Days to end of Jan
   31+28,                           // Days to end of Feb etc
   31+28+31,
   31+28+31+30,
   31+28+31+30+31,
   31+28+31+30+31+30,
   31+28+31+30+31+30+31,
   31+28+31+30+31+30+31+31,
   31+28+31+30+31+30+31+31+30,
   31+28+31+30+31+30+31+31+30+31,
   31+28+31+30+31+30+31+31+30+31+30  // End of Dec is 365 days
   };

// Avoids '-Wmissing-prototypes' when this file is used directly.
PUBLIC U8 splitDaysIntoMD(U16 allDays, U8 *monthOut, BOOLEAN isLeapYear);

/* ---------------------- splitDaysIntoMD -----------------------------------

   Split 'allDays' (1-366) into a 'monthOut' (1-12) and a day-of-month (1-28/29),
   (which is returned from the function.)

   Note: days and months are calender counts; 1 is the first day; January
   is month 1.

   If 'allDays' is zero (not a legal day) the return Jan 1st anyway. If 'allDays'
   is > 356/366 then return Dec 31st anyway.
*/
PUBLIC U8 splitDaysIntoMD(U16 allDays, U8 *monthOut, BOOLEAN isLeapYear) {

   U8 c;

   // 'allDays' should never be zero, which is not a day. But if it is, bump to Jan 1st.
   // LCOV_EXCL_START
   if(allDays == 0)
      { allDays = 1; }
   // LCOV_EXCL_STOP

   if( isLeapYear ) {               // Leap year?
      if(allDays == 31+29) {        // and it's Feb 29th?
         *monthOut = 2;             // then month = Feb
         return 29;                 // and day is 29th
         }
      else if(allDays > 31+29) {    // Leap year and beyond Feb 29th?
         allDays--;                 // then knock off a day for the non-leap-year lookup below.
         }
      }

   for( c = 0; c < RECORDS_IN(DaysToMonthStartTbl); c++ ) {    // From the start of the day-totals table
      if( allDays <= DaysToMonthStartTbl[c] )                  // Overshoot 'allDays'
         { break; }                                            // then break; at one-past the month which 'allDays' is in.
      }
   // Month is array-index when we overran DaysToMonthStartTbl[] e.g Jan exits at 2nd
   // table entry, index = 1.
   *monthOut = c;

   // To return the days remaining, subtract the day-total of the PREVIOUS table entry.
   // i.e the one we overshot.
   return allDays - DaysToMonthStartTbl[c-1];   //
}


/* ------------------------ YearWeekDay_to_YMD -----------------------------------

   Given a week and weekday, return and to 'out' month and day (of the year).
   This conversion only works over the 32bit millenial range 2000AD to Feb 7th 2136,
   06:28:15am. Outside that this function returns S_YMD zeros

   If 'year'/'week'/'weekday' are not legal then S_YMD is zeros.
*/
PUBLIC S_YMD YearWeekDay_to_YMD(U16 yr, U8 week, U8 weekday) {
   if(week > 52 || weekday > 7) {
      return (S_YMD){.yr=0, .mnth=0, .day=0}; }
   else {
      U8 mnth;
      U8 day = splitDaysIntoMD(7*(U16)week + weekday, &mnth, IsaLeapYear(yr));

      S_YMD ymd = (S_YMD){.yr=yr, .mnth=mnth, .day=day};
      if(Legal_YMD(&ymd) == true) {
         return ymd; }
      else {
         return (S_YMD){.yr=0, .mnth=0, .day=0}; }
   }
}


// ====================================== EOF ======================================
