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
static BOOLEAN dtLTu8(U8 a, U8 b) {
   return a != _YMD_AnyMDHMS && b != _YMD_AnyMDHMS && a < b;}

PUBLIC BOOLEAN YMDHMS_aGTEb(S_DateTime const *a, S_DateTime const *b) {
   return
      (a->yr != _YMD_AnyYear && b->yr != _YMD_AnyYear && a->yr < b->yr)    // Year 'a' < year 'b'?
         ? false                                                           // then 'a' cannot be GTE 'b'
         : (dtLTu8(a->mnth, b->mnth) == true                               // else month 'a' < month 'b'?
            ? false                                                        // then 'a' cannot be GTE 'b'.
            : (dtLTu8(a->day, b->day) == true                              // etc...
               ? false
               :(dtLTu8(a->hr, b->hr) == true
                 ? false
                 : (dtLTu8(a->min, b->min) == true
                    ? false
                    : (dtLTu8(a->sec, b->sec) == true
                       ? false
                       : true))))); }

// ------------------------------------- eof ---------------------------------------------------
