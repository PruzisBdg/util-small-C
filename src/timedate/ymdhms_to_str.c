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

extern C8 const * ymd_Format2Digits(C8 *out, U8 n);
extern C8 const * ymd_Format4Digits(C8 *out, U16 n);

#if 0
PUBLIC U8 YMDHMS_ToStr(S_DateTime const *t, C8 *outStr) {
   return sprintf(outStr, ISO8601Formatter, t->yr, t->mnth, t->day, t->hr, t->min, t->sec);
}
#else
PUBLIC U8 YMDHMS_ToStr(S_DateTime const *t, C8 *outStr) {
   C8 b0[10], b1[10], b2[10], b3[10], b4[10], b5[10];

   return sprintf(outStr, "%s-%s-%sT%s:%s:%s",
                  ymd_Format4Digits(b0, t->yr),
                  ymd_Format2Digits(b1, t->mnth),
                  ymd_Format2Digits(b2, t->day),
                  ymd_Format2Digits(b3, t->hr),
                  ymd_Format2Digits(b4, t->min),
                  ymd_Format2Digits(b5, t->sec) ); }

#endif

// --------------------------------- eof ---------------------------------------------------

