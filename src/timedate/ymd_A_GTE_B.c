#include "libs_support.h"
#include "util.h"

/* -------------------------------- YMD_aGTEb ----------------------------------------

   Return true if 'a' id the same date & time as 'b' or later.

   Handles wildcards; if year or month in 'a' or 'b' is a wildcard then that field in 'a'
   is NOT greater or equal than that field in 'b'. That is... year or month wildcards are
   ignored in testing for true. A day wildcard in 'a' or 'b' evaluates to true; it has to
   because there's no smaller time unit to continue to.

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
         : (a->mnth != _YMD_AnyMnth && b->mnth != _YMD_AnyMnth && a->mnth != b->mnth
            ? (a->mnth > b->mnth ? true : false)
            : ( a->day != _YMD_AnyDay && b->day != _YMD_AnyDay
               ? (DayOfYMD(a) >= DayOfYMD(b) ? true : false)
               : true )); }

// ------------------------------------- eof ---------------------------------------------------
