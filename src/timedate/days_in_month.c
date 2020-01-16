/* ------------------------------ daysInMonth ---------------------------------

   Return days in 'month' of 'yr'.  Does Feb 29th. Also handles the centurial
   non-leap years 2100, which is in the 2000AD -> 2136AD range of S_DateTime.
*/

#include "libs_support.h"
#include "util.h"

PUBLIC U8 DaysInMonth(U16 yr, U8 month)
{
   U8 const daysInMonthTbl[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};  // daysInMonthTbl[0] is pad.

   return
      month == 2 &&                                // Feb? AND
      yr % 4 == 0 &&                               // Leap year? AND
      yr != 2100                                   // not this centurial year (which is not a leap year)
         ? 29                                      // then Feb 29th
         : daysInMonthTbl[month];                  // else regular days-in-month.
}

// ----------------------------------- eof ---------------------------------------------
