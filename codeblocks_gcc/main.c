#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "wordlist.h"
#include <string.h>
#include "tiny1_stdio.h"
#include "tiny1_printf_test_support.h"

PRIVATE int prntBitAddr(C8 *out, S_BitAddr16 bf)
   { return sprintf( out, "(%u,%u)", bitAddr16_Byte(bf),  bitAddr16_Bit(bf) ); }

int main(void)
{
   OStream_Reset();
   tiny1_printf("%10S", "\xCB" "abcdefg");
   OStream_Print();

   S_BitAddr16 bf0 = bitAddr16_Make(2, 3);


   C8 b0[50];
   prntBitAddr(b0, bf0);

   S16 n = -2;
   S_BitAddr16 bf1 = bitAddr16_AddBits(bf0, n);
   C8 b1[50];
   prntBitAddr(b1, bf1);

   printf("bitfld: %s(0x%x) + %d -> %s(0x%x)\r\n", b0, bf0, n, b1, bf1 );

    return 0;
}
