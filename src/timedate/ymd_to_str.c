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

PUBLIC C8 const * ymd_Format2Digits(C8 *out, U8 n) {
   if(n == _YMD_Wilds) {
      strcpy(out, "**"); }
   else if(n > 99) {
      strcpy(out, "??"); }
   else {
      sprintf(out, "%02d", n); }
   return out;
}

PUBLIC C8 const * ymd_Format4Digits(C8 *out, U16 n) {
   if(n == _YMD_WildYear) {
      strcpy(out, "****"); }
   else if(n > 9999) {
      strcpy(out, "????"); }
   else {
      sprintf(out, "%04d", n); }
   return out;
}

PUBLIC U8 YMD_ToStr(S_YMD const *t, C8 *outStr) {
   C8 y[10], m[10], d[10];

   return sprintf(outStr, "%s-%s-%s",
                           ymd_Format4Digits(y, t->yr),
                           ymd_Format2Digits(m, t->mnth),
                           ymd_Format2Digits(d, t->day) ); }

// --------------------------------- eof ---------------------------------------------------

