/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ---------------------------- Legal_YMD ------------------------------------

   Return TRUE if 't' is a legal date. Wildcards are legal.
*/
PUBLIC BOOLEAN Legal_YMD(S_YMD const *t) {

   #define _Years32Bit  (MAX_U32/366/24/3600)            // 32 bits can count 135years in seconds

   return
      (
         t->yr == _YMD_WildYear ||
         (t->yr >= _2000AD && t->yr <= (_2000AD+_Years32Bit))     // Between 2000AD and 2130AD ? AND
      ) &&
      (
         t->mnth == _YMD_Wilds ||
         (t->mnth >= 1 && t->mnth <= 12)                          // Jan -> Dec?
      ) &&
      (
         t->day == _YMD_Wilds ||
         (
            t->day >= 1 &&                                        // 1st...
            t->day <= (t->mnth == _YMD_Wilds                   // If wildcard month...
                           ? 31                                   // the 31st
                           : DaysInMonth(t->yr, t->mnth))         // else 28(29)30,31st?
         )

      );
}


// ----------------------------------- eof ----------------------------------------
