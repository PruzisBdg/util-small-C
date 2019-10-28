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
         t->yr == _YMD_AnyYear ||
         (t->yr >= _2000AD && t->yr <= _2136AD)          // Between 2000AD and 2130AD ? AND
      ) &&
      (
         t->mnth == _YMD_AnyMDHMS ||
         (t->mnth >= 1 &&
          t->mnth <= (t->yr < _2136AD ? 12 : 2))         // Jan -> Dec? (in 2136 just Jan,Feb)
      ) &&
      (
         t->day == _YMD_AnyMDHMS ||
         (
            t->day >= 1 &&                               // > 1st...
            t->day <= (                                  // and LTE then
               t->yr == _2136AD && t->mnth == 2
                  ? 7                                    // Feb 7th 2136, else...
                  : (t->mnth == _YMD_AnyMDHMS            // ...If wildcard month...
                     ? 31                                // the 31st
                     : DaysInMonth(t->yr, t->mnth)))     // else 28(29)30,31st?
         )
      );
}


// ----------------------------------- eof ----------------------------------------
