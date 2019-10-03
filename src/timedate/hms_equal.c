/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC BOOLEAN HMS_Equal(S_TimeHMS const *a, S_TimeHMS const *b)
{
   return
      a->hr == b->hr && a->min == b->min && a->sec == b->sec
         ? TRUE : FALSE;
}

// ----------------------------- EOF ------------------------------------------
