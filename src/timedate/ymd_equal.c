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
      (a->yr == _YMD_AnyYear     || b->yr == _YMD_AnyYear     || a->yr == b->yr) &&					// Check for any year or same year
      (a->mnth == _DateTime_AnyMDHMS || b->mnth == _DateTime_AnyMDHMS || a->mnth == b->mnth) &&		// Check for any month or same month
      (a->day == _DateTime_AnyMDHMS  || b->day == _DateTime_AnyMDHMS  ||							// Check for any day or both last days of the month
	  (a->day == _YMD_LastDay && b->day == DaysInMonth(b->yr,b->mnth)) ||							// Check if other day is also the last day of the month
	  (b->day == _YMD_LastDay && a->day == DaysInMonth(a->yr,a->mnth)) ||							// Check if other day is also the last day of the month
	  DayOfYMD(a) == DayOfYMD(b) )																	// Check for same day
            ? TRUE : FALSE;
}

// ----------------------------- EOF ------------------------------------------
