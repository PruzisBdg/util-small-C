/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

/* ----------------------- YMD_ToStr ---------------------------------

   Make S_YMD 't' into an ISO8601 date string in 'outStr' e.g "2002-03-07".

   The function doesn't check if 't' is a legal date time.

   Returns: the number of characters in the strings
*/
PUBLIC C8 const ISO8601_DateFormatter[] = "%04d-%02d-%02d";   // Make public to avoid ARM gcc linker bug.

PUBLIC U8 YMD_ToStr(S_YMD const *t, C8 *outStr) {
   return sprintf(outStr, ISO8601_DateFormatter, t->yr, t->mnth, t->day);
}

// --------------------------------- eof ---------------------------------------------------

