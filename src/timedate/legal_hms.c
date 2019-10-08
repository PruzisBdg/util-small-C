/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ---------------------------- Legal_HMS ------------------------------------

   Return TRUE if 't' is a legal time of day. Allows wildcards.
*/
PUBLIC BOOLEAN Legal_HMS(S_TimeHMS const *t) {

   return
      (t->hr == _HMS_WildHr || t->hr <= 23) &&
      (t->min == _HMS_Wilds || t->min <= 59) &&
      (t->min == _HMS_Wilds || t->sec <= 59);
}


// ----------------------------------- eof ----------------------------------------
