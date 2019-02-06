/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC T_Seconds32 AD2000toEpoch(T_Seconds32 secsSince2000AD)
{
   #define _2000AD_Epoch_secs 946728000UL

   return
      secsSince2000AD > (_Max_T_Seconds32 - _2000AD_Epoch_secs)
         ? _Max_T_Seconds32
         : _2000AD_Epoch_secs + secsSince2000AD;
}

// =============================== eof =======================================
