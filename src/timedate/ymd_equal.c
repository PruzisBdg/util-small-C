/*---------------------------------------------------------------------------
|
| Return TRUE if 'a', 'b' are the same Date
|
| Allows wildcards e.g _YMD_WildYear (0xFEFE) -> any year; '_DateTime_Wilds' (0xFE)
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
      (a->yr == _YMD_WildYear     || b->yr == _YMD_WildYear     || a->yr == b->yr) &&
      (a->mnth == _DateTime_Wilds || b->mnth == _DateTime_Wilds || a->mnth == b->mnth) &&
      (a->day == _DateTime_Wilds  || b->day == _DateTime_Wilds  || a->day == b->day)
            ? TRUE : FALSE;
}

// ----------------------------- EOF ------------------------------------------
