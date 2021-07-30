/*---------------------------------------------------------------------------
|
| IsaLeapYear()
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC BOOLEAN IsaLeapYear(U16 yr) {
   return
      (yr % 4) == 0 &&
      yr != 1900 && yr != 2100 && yr != 2200 && yr != 2300; }    // These centurial years are NOT leap years.

// ----------------------------- eof ----------------------------------------
