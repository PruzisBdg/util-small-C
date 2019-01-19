/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ------------------------------ daysInMonth --------------------------------- 

   Return days in 'month' of a non-leap year.
*/

PRIVATE U8 const daysInMonthTbl[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

PRIVATE U8 daysInMonth(U8 month) {
   return daysInMonthTbl[month];
}

/* ---------------------------- Legal_YMDHMS ------------------------------------

   Return TRUE if 't' is a legal date and time for use as a 32 bit internal time which
   starts at 200AD.. Hours, minutes etc must be legal; the year must be 2000AD - 2130AD.
*/
PUBLIC BOOLEAN Legal_YMDHMS(S_DateTime const *t) {

   #define _Years32Bit  (MAX_U32/366/24/3600)            // 32 bits can count 132years in seconds

   return
   t->yr >= _2000AD && t->yr <= (_2000AD+_Years32Bit) && // Between 2000AD and 2130AD ? AND
   t->mnth >= 1 && t->mnth <= 12 &&                      // Jan -> Dec?
   t->day >= 1 && t->day <= daysInMonth(t->mnth) &&      // 1st -> 28(29)30,31st?
   t->hr <= 23 &&                                        // hours 0-23?
   t->min <= 59 &&                                       // 
   t->sec <= 59;
}


// ----------------------------------- eof ----------------------------------------
