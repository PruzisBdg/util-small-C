/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC T_Seconds32 EpochTo2000AD(T_Seconds32 epochSecs)
{
   return
      epochSecs < _12am_Jan_1st_2000_Epoch_secs
         ? 0
         : epochSecs - _12am_Jan_1st_2000_Epoch_secs;
}

// =============================== eof =======================================
