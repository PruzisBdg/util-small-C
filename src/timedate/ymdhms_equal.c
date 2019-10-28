/*---------------------------------------------------------------------------
|
| Return TRUE if 'a', 'b' are the same Date/Time.
|
| Allows wildcards e.g _YMD_AnyYear (0xFEFE) -> any year; '_DateTime_AnyMDHMS' (0xFE)
| any month, day, hour etc.
|
| Does NOT check that 'a','b' are a legal S_DateTime. Most times 'a', 'b' or both will
| will be a constant so that check won't be needed for one or the other.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC BOOLEAN YMDHMS_Equal(S_DateTime const *a, S_DateTime const *b)
{
   return
      (a->yr == _YMD_AnyYear     || b->yr == _YMD_AnyYear     || a->yr == b->yr) &&
      (a->mnth == _DateTime_AnyMDHMS || b->mnth == _DateTime_AnyMDHMS || a->mnth == b->mnth) &&
      (a->day == _DateTime_AnyMDHMS  || b->day == _DateTime_AnyMDHMS  || a->day == b->day) &&
      (a->hr == _DateTime_AnyMDHMS   || b->hr == _DateTime_AnyMDHMS   || a->hr == b->hr) &&
      (a->min == _DateTime_AnyMDHMS  || b->min == _DateTime_AnyMDHMS  || a->min == b->min) &&
      (a->sec == _DateTime_AnyMDHMS  || b->sec == _DateTime_AnyMDHMS  || a->sec == b->sec)
            ? TRUE : FALSE;
}

// ----------------------------- EOF ------------------------------------------
