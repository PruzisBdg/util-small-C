#include "libs_support.h"
#include "util.h"


PUBLIC BOOL WeekDate_Equal(S_WeekDate const *a, S_WeekDate const *b)
{
   return
      a->yr == b->yr && a->week == b->week && a->day == b->day
         ? TRUE : FALSE;
}

// --------------------------------- EOF -----------------------------------------
