#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "sensus_codec.h"

int main (void)
{
   enc_S_MsgData md = {
      .encoderType = mADE | mGen1,
      .weGot.serWord = 1,
      .rawTot = 1234, .dials = 6 };

   C8 b0[200];
   Sensus_PrintMsgData(b0,&md);
   printf("%s", b0);
}

// ------------------------------------- eof ---------------------------------------------
