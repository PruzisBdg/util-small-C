/*---------------------------------------------------------------------------
|
| Return TRUE if 'a', 'b' are the same Date
|
| Allows wildcards e.g _YMD_AnyYear (0xFEFE) -> any year; '_DateTime_AnyMDHMS' (0xFE)
| any month, day, hour etc.
|
| Does NOT check that 'a','b' are a legal S_YMD. Most times 'a', 'b' or both will
| will be a constant so that check won't be needed for one or the other.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC BOOLEAN YMD_Equal(S_YMD const *a, S_YMD const *b)
{
   return
      (a->yr == _YMD_AnyYear     || b->yr == _YMD_AnyYear     || a->yr == b->yr) &&
      (a->mnth == _DateTime_AnyMDHMS || b->mnth == _DateTime_AnyMDHMS || a->mnth == b->mnth) &&
      (a->day == _DateTime_AnyMDHMS  || b->day == _DateTime_AnyMDHMS  || a->day == b->day)
            ? TRUE : FALSE;
}

// ----------------------------- EOF ------------------------------------------
