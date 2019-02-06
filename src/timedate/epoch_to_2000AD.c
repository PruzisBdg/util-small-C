/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC T_Seconds32 EpochTo2000AD(T_Seconds32 epochSecs)
{
   #define _2000AD_Epoch_secs 946728000UL

   return
      epochSecs < _2000AD_Epoch_secs
         ? 0
         : epochSecs - _2000AD_Epoch_secs;
}

// =============================== eof =======================================
