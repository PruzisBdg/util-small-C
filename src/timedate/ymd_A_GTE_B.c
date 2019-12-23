#include "libs_support.h"
#include "util.h"

/* -------------------------------- YMD_aGTEb ----------------------------------------

   Return true if 'a' id the same date & time as 'b' or later.

   Handles wildcards; if any field (year, month rtc...) in 'a' or 'b' is a wildcard then
   that field in 'a' is NOT greater or equal than that field in 'b'. That is... wildcards
   are ignored in testing for true.

   So:
      2001-2-3     >= 2000-2-3
      2001-2-3     >= 2001-2-3

      2001-2-3     >= ****-2-3   (years are not compared)
      ****-2-3 NOT >= 2001-2-3      "        "     "
*/

// True if neither 'a' or 'b' s a wildcard AND 'a' >= 'b'.
static BOOLEAN dtGTEu8(U8 a, U8 b) {
   return a != _YMD_AnyMDHMS && b != _YMD_AnyMDHMS && a >= b;}

PUBLIC BOOLEAN YMD_aGTEb(S_YMD const *a, S_YMD const *b) {
   return
      (a->yr != _YMD_AnyYear && b->yr != _YMD_AnyYear && a->yr >= b->yr)   // Year 'a' < year 'b'?
         ? true                                                            // then 'a' cannot be GTE 'b'
         : (dtGTEu8(a->mnth, b->mnth) == true                              // else month 'a' < month 'b'?
            ? true                                                         // then 'a' cannot be GTE 'b'.
            : (dtGTEu8(a->day, b->day) == true                             // etc...
               ? true
               : false )); }

// ------------------------------------- eof ---------------------------------------------------
