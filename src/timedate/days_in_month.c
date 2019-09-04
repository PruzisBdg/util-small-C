/* ------------------------------ daysInMonth ---------------------------------

   Return days in 'month' of 'yr'.  Does Feb 29th.
*/

#include "libs_support.h"
#include "util.h"

PUBLIC U8 DaysInMonth(U16 yr, U8 month)
{
   U8 const daysInMonthTbl[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};  // daysInMonthTbl[0] is pad.

   return
      yr % 4 == 0 && month == 2        // Feb leap-year?
         ? 29
         : daysInMonthTbl[month];
}

// ----------------------------------- eof ---------------------------------------------
