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

// Shorthand for S_YMD inside S_DateTime
#define _yr    ymd.yr
#define _mnth  ymd.mnth
#define _day   ymd.day

PUBLIC BOOLEAN YMDHMS_Equal(S_DateTime const *a, S_DateTime const *b)
{
   return
      (a->_yr == _YMD_AnyYear         || b->_yr == _YMD_AnyYear         || a->_yr == b->_yr) &&
      (a->_mnth == _DateTime_AnyMDHMS || b->_mnth == _DateTime_AnyMDHMS || a->_mnth == b->_mnth) &&
      (a->_day == _DateTime_AnyMDHMS  || b->_day == _DateTime_AnyMDHMS  || a->_day == b->_day) &&
      (a->hr == _DateTime_AnyMDHMS    || b->hr == _DateTime_AnyMDHMS    || a->hr == b->hr) &&
      (a->min == _DateTime_AnyMDHMS   || b->min == _DateTime_AnyMDHMS   || a->min == b->min) &&
      (a->sec == _DateTime_AnyMDHMS   || b->sec == _DateTime_AnyMDHMS   || a->sec == b->sec)
            ? TRUE : FALSE;
}

// ----------------------------- EOF ------------------------------------------
