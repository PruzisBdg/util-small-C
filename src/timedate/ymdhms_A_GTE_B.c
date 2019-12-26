#include "libs_support.h"
#include "util.h"

/* -------------------------------- YMDHMS_aGTEb ----------------------------------------

   Return true if 'a' id the same date & time as 'b' or later.

   Handles wildcards; if any field (year, month rtc...) in 'a' or 'b' is a wildcard then
   that field in 'a' is NOT greater or equal than that field in 'b'. That is... wildcards
   are ignored in testing for true.

   So:
      2001-2-3T4:5:6     >= 2000-2-3T4:5:6
      2001-2-3T4:5:9     >= 2001-2-3T4:5:6

      2001-2-3T4:5:9     >= ****-2-3T4:5:6   (years are not compared)
      2001-2-3T4:5:6 NOT >= ****-2-3T4:5:9      "        "     "
      ****-2-3T4:5:6 NOT >= 2001-2-3T4:5:9      "        "     "
*/

// True if neither 'a' or 'b' s a wildcard AND 'a' < 'b'.
static BOOLEAN dtNEu8(U8 a, U8 b) {
   return a != _YMD_AnyMDHMS && b != _YMD_AnyMDHMS && a != b;}

PUBLIC BOOLEAN YMDHMS_aGTEb(S_DateTime const *a, S_DateTime const *b) {
   return
      (a->ymd.yr != _YMD_AnyYear && b->ymd.yr != _YMD_AnyYear && a->ymd.yr != b->ymd.yr)
         ? (a->ymd.yr > b->ymd.yr ? true : false)
         : (dtNEu8(a->ymd.mnth, b->ymd.mnth) == true                                      // else month 'a' < month 'b'?
            ? (a->ymd.mnth > b->ymd.mnth ? true : false)
            : (dtNEu8(a->ymd.day, b->ymd.day) == true                                     // etc...
               ? (a->ymd.day > b->ymd.day ? true : false)
               :(dtNEu8(a->hr, b->hr) == true
                 ? (a->hr > b->hr ? true : false)
                 : (dtNEu8(a->min, b->min) == true
                    ? (a->min > b->min ? true : false)
                    : ((a->sec != _YMD_AnyMDHMS && b->sec != _YMD_AnyMDHMS)
                       ? (a->sec >= b->sec ? true : false)
                       : false))))); }

// ------------------------------------- eof ---------------------------------------------------
