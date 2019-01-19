/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

/* ----------------------- YMDHMS_ToStr --------------------------------- 

   Make S_DateTime 't' into an ISO8601 date+time string in 'outStr' e.g 
   "2002-03-07T14:21:05". 

   The function doesn't check if 't' is a legal date time. Use legalDateTime().

   Returns: the number of characters in the strings
*/
PUBLIC C8 const ISO8601Formatter[] = "%04d-%02d-%02dT%02d:%02d:%02d";   // Make public to avoid ARM gcc linker bug.

PUBLIC U8 YMDHMS_ToStr(S_DateTime const *t, C8 *outStr) {
   return sprintf(outStr, ISO8601Formatter, t->yr, t->mnth, t->day, t->hr, t->min, t->sec);
}

// --------------------------------- eof ---------------------------------------------------

