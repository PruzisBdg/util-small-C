/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC T_Seconds32 AD2000toEpoch(T_Seconds32 secsSince2000AD)
{
   return
      secsSince2000AD > (_Max_T_Seconds32 - _12am_Jan_1st_2000_Epoch_secs)
         ? _Max_T_Seconds32
         : _12am_Jan_1st_2000_Epoch_secs + secsSince2000AD;
}

// =============================== eof =======================================
