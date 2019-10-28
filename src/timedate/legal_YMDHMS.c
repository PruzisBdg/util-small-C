/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* ---------------------------- Legal_YMDHMS ------------------------------------

   Return TRUE if 't' is a legal date and time for use as a 32 bit internal time which
   starts at 200AD.. Hours, minutes etc must be legal; the year must be 2000AD - 2130AD.

   Also allows wildcards e.g _YMD_AnyYear (0xFEFE) -> every year.
*/
PUBLIC BOOLEAN Legal_YMDHMS(S_DateTime const *t)
{
   return
      Legal_HMS( &(S_TimeHMS const){.hr = t->hr, .min = t->min, .sec = t->sec} ) == true &&
      Legal_YMD( &(S_YMD const){.yr = t->yr, .mnth = t->mnth, .day = t->day}  ) == true;
}


// ----------------------------------- eof ----------------------------------------
