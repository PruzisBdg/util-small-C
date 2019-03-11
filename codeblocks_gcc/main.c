#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "wordlist.h"
#include <string.h>
#include "tiny1_stdio.h"
#include "tiny1_printf_test_support.h"

PRIVATE int prntBitAddr(C8 *out, S_Bit64K bf)
   { return sprintf( out, "(%u,%u)", Bit64K_Byte(bf),  Bit64K_BitBE(bf) ); }

PRIVATE U8 srcBuf[10] = {0x00,2,3,4,5,6,7,8,9,10};
PRIVATE U8 destBuf[10] = {0xFF, 0xFF, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F};

PRIVATE bool rds(U8 *to, U8 from, U8 cnt ) { memcpy(to, &destBuf[from], cnt); return true; }
PRIVATE bool wrs(U8 to, U8 const *from, U8 cnt) { memcpy(&destBuf[to], from, cnt); return true; }
PRIVATE bool getss(U8 *to, U8 from, U8 cnt) { memcpy(to, &srcBuf[from], cnt); return true; }
PRIVATE S_Bit64KPort const port1 = {.rdDest = rds, .wrDest = wrs, .getSrc = getss };

int main (void)
{
   typedef struct { U8 _byte, _bit; } S_BA;
   typedef struct { S_BA from, to; U8 nBits; } S_Cpy;

   S_Cpy cp = { .from = {._byte = 0, ._bit = 0}, .to = {._byte = 0, ._bit = 0}, .nBits = 1 };
   Bit64K_Copy(&port1, Bit64K_MakeBE(cp.to._byte ,cp.to._bit), Bit64K_MakeBE(cp.from._byte ,cp.from._bit), cp.nBits);
   printf("src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]\r\n",
          srcBuf[0], srcBuf[1], cp.from._byte ,cp.from._bit, cp.nBits,
          cp.to._byte ,cp.to._bit, destBuf[0], destBuf[1]);
}

#if 0
int main(void)
{
   OStream_Reset();
   tiny1_printf("%10S", "\xCB" "abcdefg");
   OStream_Print();

   S_Bit64K bf0 = Bit64K_MakeBE(2, 3);


   C8 b0[50];
   prntBitAddr(b0, bf0);

   S16 n = -2;
   S_Bit64K bf1 = Bit64K_AddBits(bf0, n);
   C8 b1[50];
   prntBitAddr(b1, bf1);

   printf("bitfld: %s(0x%x) + %d -> %s(0x%x)\r\n", b0, bf0, n, b1, bf1 );

    return 0;
}
#endif
