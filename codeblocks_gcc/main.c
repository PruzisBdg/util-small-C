#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "sensus_codec.h"

int main (void)
{

   Legal_YMD( &(S_YMD){.yr = 2001, .mnth = 0xFE, .day = 1});

   enc_S_MsgData md = {
      .encoderType = mADE | mGen1,
      .weGot.bs.serWord = 1,
      .fwdTot = 1234, .dials = 6,
      .noMag.pres = {._min=0xFF, ._max=0xFF, ._avg=0xFF} };

   C8 b0[200];
   Sensus_PrintMsgData(b0,&md);
   printf("%s", b0);
}

// ------------------------------------- eof ---------------------------------------------
