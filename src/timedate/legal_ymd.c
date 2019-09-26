/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ---------------------------- Legal_YMD ------------------------------------

   Return TRUE if 't' is a legal time of day.
*/
PUBLIC BOOLEAN Legal_YMD(S_YMD const *t) {

   #define _Years32Bit  (MAX_U32/366/24/3600)            // 32 bits can count 135years in seconds

   return
      t->yr >= _2000AD && t->yr <= (_2000AD+_Years32Bit) &&       // Between 2000AD and 2130AD ? AND
      t->mnth >= 1 && t->mnth <= 12 &&                            // Jan -> Dec?
      t->day >= 1 && t->day <= DaysInMonth(t->yr, t->mnth);       // 1st -> 28(29)30,31st?
}


// ----------------------------------- eof ----------------------------------------
