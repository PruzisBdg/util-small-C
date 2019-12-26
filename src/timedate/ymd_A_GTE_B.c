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

PUBLIC BOOLEAN YMD_aGTEb(S_YMD const *a, S_YMD const *b) {
   return
      a->yr != _YMD_AnyYear && b->yr != _YMD_AnyYear && a->yr != b->yr
         ? (a->yr > b->yr ? true : false)
         : (a->mnth != _YMD_AnyMDHMS && b->mnth != _YMD_AnyMDHMS && a->mnth != b->mnth
            ? (a->mnth > b->mnth ? true : false)
            : (a->day != _YMD_AnyMDHMS && b->day != _YMD_AnyMDHMS
               ? (a->day >= b->day ? true : false)
               : false )); }

// ------------------------------------- eof ---------------------------------------------------
