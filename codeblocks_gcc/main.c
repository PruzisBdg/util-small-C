#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "sensus_codec.h"

int main (void)
{
   #define _dt(_yr, _mnth, _day, _hr, _min, _sec) \
      &(S_DateTime){.yr = _yr, .mnth = _mnth, .day = _day, .hr = _hr, .min = _min, .sec = _sec}

   S_DateTime out;

   Full_YMDHMS_AddSecs(&out, _dt(2000,1,1,0,0,0), -(365+365+365+366)*24*3600L);
}

// ------------------------------------- eof ---------------------------------------------
