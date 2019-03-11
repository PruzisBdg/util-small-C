#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "wordlist.h"
#include <string.h>
#include "tiny1_stdio.h"
#include "tiny1_printf_test_support.h"

PRIVATE int prntBitAddr(C8 *out, S_Bit64K bf)
   { return sprintf( out, "(%u,%u)", bit64K_Byte(bf),  bit64K_BitBE(bf) ); }

#define _BufSize 10
PRIVATE U8 srcBuf[_BufSize] = {0x01,0x80, [2 ... _BufSize-1] = 0x00};
PRIVATE U8 destBuf[_BufSize] = {0};

PRIVATE bool rds(U8 *to, bit64K_T_ByteOfs from, bit64K_T_Cnt cnt ) { memcpy(to, &destBuf[from], cnt); return true; }
PRIVATE bool wrs(bit64K_T_ByteOfs to, U8 const *from, bit64K_T_Cnt cnt) { memcpy(&destBuf[to], from, cnt); return true; }
PRIVATE bool getss(U8 *to, bit64K_T_ByteOfs from, bit64K_T_Cnt cnt) { memcpy(to, &srcBuf[from], cnt); return true; }
PRIVATE S_Bit64KPorts const port1 = {.rdDest = rds, .wrDest = wrs, .getSrc = getss };

int main (void)
{
   typedef struct { U8 _byte, _bit; } S_BA;
   typedef struct { S_BA from, to; U8 nBits; } S_Cpy;

   S_Cpy cp = { .from = {._byte = 0, ._bit = 0}, .to = {._byte = 0, ._bit = 1}, .nBits = 2 };
   bit64K_Copy(&port1, bit64K_MakeBE(cp.to._byte ,cp.to._bit), bit64K_MakeBE(cp.from._byte ,cp.from._bit), cp.nBits);
   printf("src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]\r\n",
          srcBuf[0], srcBuf[1], cp.from._byte ,cp.from._bit, cp.nBits,
          cp.to._byte ,cp.to._bit, destBuf[0], destBuf[1]);
}

#if 0
int main(void)
{bit64K_
   OStream_Reset();
   tiny1_printf("%10S", "\xCB" "abcdefg");
   OStream_Print();

   S_Bit64K bf0 = bit64K_MakeBE(2, 3);


   C8 b0[50];
   prntBitAddr(b0, bf0);

   S16 n = -2;
   S_Bit64K bf1 = bit64K_AddBits(bf0, n);
   C8 b1[50];
   prntBitAddr(b1, bf1);

   printf("bitfld: %s(0x%x) + %d -> %s(0x%x)\r\n", b0, bf0, n, b1, bf1 );

    return 0;
}
#endif
