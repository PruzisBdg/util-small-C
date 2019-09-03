/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC BOOLEAN YMDHMS_Equal(S_DateTime const *a, S_DateTime const *b)
{
   return a->yr == b->yr && a->mnth == b->mnth && a->day == b->day &&
          a->hr == b->hr && a->min == b->min && a->sec == b->sec
            ? TRUE : FALSE;
}

// ----------------------------- EOF ------------------------------------------
