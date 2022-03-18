#include "libs_support.h"
#include "util.h"

/* ------------------------ WeekDate_A_GT_B ----------------------------------

   Return TRUE if a,b are properly formed Week-Dates AND a > b.
*/
PUBLIC BOOL WeekDate_A_GT_B(S_WeekDate const *a, S_WeekDate const *b)
{
   return
      Legal_WeekDateFields(a) == false || Legal_WeekDateFields(b) == false
         ? FALSE
         : (a->yr > b->yr
            ? TRUE
            : (a->yr == b->yr && a->week > b->week
               ? TRUE
               : (a->yr == b->yr && a->week == b->week && a->day > b->day
                  ? TRUE
                  : FALSE)));
}

// --------------------------------- EOF -----------------------------------------
