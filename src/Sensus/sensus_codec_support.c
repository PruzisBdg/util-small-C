/* -------------------------------- Sensus Codec (Test Harness) Support --------------------------------- */

#include "libs_support.h"
#include "sensus_codec.h"
#include <stdio.h>
#include <string.h>
#include "util.h"

#if 0
PUBLIC U16 Sensus_PrintMsgData(C8 *out, enc_S_MsgData const *ed)
{
   C8 const *p = out;
   // Print the number of dials
   C8 b0[100];
   if(ed->dials > 6 && ed->dials <= 9) {
      sprintf(b0, "Tot %%%dd\r\n", ed->dials);
   }
   else {
      strcpy(b0, "Tot %d\r\n");
   }

   return sprintf(out, b0, ed->rawTot);
}
#endif


// --------------------------------------------- eof -----------------------------------------------------
