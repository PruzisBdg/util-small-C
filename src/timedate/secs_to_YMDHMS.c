/*---------------------------------------------------------------------------
|
| SecsToYMDHMS()
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ------------------------ SecsToYMDHMS ----------------------------

   Convert seconds since 00:00:00 Jan 1st 2000AD ('secsSince1AD') into
   Y:M:D:H:M:S in a date/time struct 'dt'.

   This routine does NOT handle leap-seconds.
*/
extern U8 splitDaysIntoMD(U16 allDays, U8 *monthOut, BOOLEAN isLeapYear);

PUBLIC S_DateTime const * SecsToYMDHMS(T_Seconds32 secsSince2000AD, S_DateTime *dt) {

   U8    yearsRem;
   U16   yr4, daysRem;
   U32   secsRem;

   #define _4yr_days (365+365+365+366)
   #define _4yr_secs (3600L*24*_4yr_days)                         // Seconds in 4 years

   yr4 = secsSince2000AD/_4yr_secs;                               // Divide total secs into 4-year chunks...
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

   dt->ymd.yr = _2000AD + (4*yr4) + yearsRem;                     // Year is 2000AD + 4 x 4-year chunks + years-left-over

   daysRem =                                                      // Days left over after the last year is...
      daysRem -                                                   // days remaining after 4-year chunks, minus...
      (yearsRem == 3                                              // the number of days in the (0-3) left-over years
         ? (366+365+365)
         : (yearsRem == 2
            ? (366+365)
            : (yearsRem == 1
               ? 366
               : 0)));

   /* Split days-left-over into a month and day-of-month. Note we must convert
      'daysRem' into a calender count; 1 upwards.
   */
   dt->ymd.day = splitDaysIntoMD(daysRem+1, &dt->ymd.mnth, IsaLeapYear(dt->ymd.yr));

   secsRem =                                                      // Seconds remaining in the final day are....
      secsRem -                                                   // secs left over after 4-years chunks removed, minus...
      (yearsRem == 3                                              // ...secs in the remaining 0 - 3 complete years...
         ? 3600L*24*(366+365+365)
         : (yearsRem == 2
            ? 3600L*24*(366+365)
            : (yearsRem == 1
               ? 3600L*24*366
               : 0)))
      - (3600L*24 * daysRem);                                     // ... minus secs left over remaining days subtracted.

   dt->hr = secsRem/3600;                                         // Hours in the final day are..
   dt->min = (secsRem - (3600L * dt->hr)) / 60;                   // Minutes left over are.. (subtract hour*3600)
   dt->sec = secsRem - (3600L * dt->hr) - (60 * dt->min);         // Secs left over are..(subtract hour*3600 + minutes*60)

   return dt;
}

// -------------------------------- eof ---------------------------------------------

