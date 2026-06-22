#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <stdio.h>

U16 SystemTicks_perSec() {return 100;}

int main (void)
{
   S_BufC8 b0 = (S_BufC8){.cs = (C8[100]){0}, .cnt = 20};
   SecsToDHMStr(889200, &b0);
   printf("--------------- done %s\r\n", b0.cs);
}


// ------------------------------------- eof ---------------------------------------------
