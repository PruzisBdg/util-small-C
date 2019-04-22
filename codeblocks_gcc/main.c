#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "wordlist.h"
#include <string.h>
#include "tiny1_stdio.h"
#include "tiny1_printf_test_support.h"

#if 1
PRIVATE int prntBitAddr(C8 *out, S_Bit64K bf)
   { return sprintf( out, "(%u,%u)", bit64K_Byte(bf),  bit64K_BitBE(bf) ); }

#define _BufSize 10
PRIVATE U8 srcBuf[_BufSize] = {0x80,0x00, [2 ... _BufSize-1] = 0x00};
PRIVATE U8 destBuf[_BufSize] = {0xAA,0x00, [2 ... _BufSize-1] = 0x00};

PRIVATE bool rds(U8 *to, bit64K_atByte from, bit64K_T_Cnt cnt ) { memcpy(to, &destBuf[from], cnt); return true; }
PRIVATE bool wrs(bit64K_atByte to, U8 const *from, bit64K_T_Cnt cnt) { memcpy(&destBuf[to], from, cnt); return true; }
PRIVATE bool getss(U8 *to, bit64K_atByte from, bit64K_T_Cnt cnt) { memcpy(to, &srcBuf[from], cnt); return true; }
PRIVATE bit64K_Ports const port1 = {.dest.rd = rds, .dest.wr = wrs, .src.get = getss };
#endif

typedef struct { U8 _byte, _bit; } S_BA;
typedef struct { S_BA from, to; U8 nBits; E_EndianIs destEndian; } S_Cpy;
typedef struct { S_BA to; U8 nBits; U8 destFill; E_EndianIs destEndian; } S_CpyIn;

static void cpyIn(S_CpyIn const *in, U8 const *src)
{
   memset(destBuf, in->destFill, _BufSize);

   bit64K_In(
      &port1,
      // Convert destination byte/bit into a bit address in the endian of the destination.
      // The resulting bit addresses are monotonic in the destination endianness.
      in->destEndian == eBigEndian
         ? bit64K_MakeBE(in->to._byte, in->to._bit)
         : bit64K_MakeLE(in->to._byte, in->to._bit),
      src,
      in->nBits,
      in->destEndian);

   printf("CpyIn: src[0x%x 0x%x](%s) -> (%d,%d){%d}} -> dest[0x%x 0x%x]\r\n",
          src[0], src[1], PrintEndian(in->destEndian),
          in->to._byte ,in->to._bit, in->nBits, destBuf[0], destBuf[1]);
}

int main (void)
{

   #if 0

   S_Cpy cp0 = { .from = {._byte = 0, ._bit = 0}, .to = {._byte = 0, ._bit = 1}, .nBits = 2 };
   bit64K_Copy(&port1, bit64K_MakeBE(cp0.to._byte ,cp0.to._bit), bit64K_MakeBE(cp0.from._byte ,cp0.from._bit), cp0.nBits);
   printf("src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]\r\n",
          srcBuf[0], srcBuf[1], cp0.from._byte ,cp0.from._bit, cp0.nBits,
          cp0.to._byte ,cp0.to._bit, destBuf[0], destBuf[1]);


   S_Cpy cp1 = { .from = {._byte = 0, ._bit = 7}, .nBits = 16 };
   bit64K_Out(&port1, destBuf, bit64K_MakeBE(cp1.from._byte ,cp1.from._bit), cp1.nBits, eBigEndian);
   printf("src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]\r\n",
          srcBuf[0], srcBuf[1], cp1.from._byte ,cp1.from._bit, cp1.nBits,
          cp1.to._byte ,cp1.to._bit, destBuf[0], destBuf[1]);

   bool rtn = bit64K_ParmFitsField( (U8[1]){0x3F}, 1, 6, false);

   C8 b0[100];
   strcpy(b0, "aaaaaaaaaaaaaaaaaaa");

   tiny1_sprintf(b0, "%02d:%02d:%02d  rtn %d", 12, 34, 56, rtn);
   printf("b0 = %s\r\n", b0);

   #else

   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 3}, .nBits = 8, .destEndian = eBigEndian}}, (U8[]){0x5A,0x00, [2 ... _BufSize-1] = 0x00} );

   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 4}, .nBits = 8, .destEndian = eLittleEndian}}, (U8[]){0x5A,0x00, [2 ... _BufSize-1] = 0x00} );
   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 6}, .nBits = 4, .destEndian = eLittleEndian}}, (U8[]){0x0F,0x00, [2 ... _BufSize-1] = 0x00} );
   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 0}, .nBits = 2, .destEndian = eBigEndian}}, (U8[]){0x03,0x00, [2 ... _BufSize-1] = 0x00} );

   // Big-endian reverses bits in a byte to make the bit address
   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 1}, .nBits = 4, .destFill = 0x00, .destEndian = eBigEndian}}, (U8[]){0x0F,0x00, [2 ... _BufSize-1] = 0x00} );

   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 3}, .nBits = 4, .destFill = 0x00, .destEndian = eBigEndian}}, (U8[]){0x0F,0x00, [2 ... _BufSize-1] = 0x00} );
   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 3}, .nBits = 4, .destFill = 0xFF, .destEndian = eBigEndian}}, (U8[]){0xF0,0x00, [2 ... _BufSize-1] = 0x00} );

   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 0}, .nBits = 1, .destFill = 0x00, .destEndian = eBigEndian}}, (U8[]){0x01,0x00, [2 ... _BufSize-1] = 0x00} );
   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 0}, .nBits = 1, .destFill = 0x00, .destEndian = eBigEndian}}, (U8[]){0xFF,0x00, [2 ... _BufSize-1] = 0x00} );
   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 7}, .nBits = 1, .destFill = 0x00, .destEndian = eBigEndian}}, (U8[]){0x01,0x00, [2 ... _BufSize-1] = 0x00} );
   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 0}, .nBits = 1, .destFill = 0xFF, .destEndian = eBigEndian}}, (U8[]){0x00,0x00, [2 ... _BufSize-1] = 0x00} );

   cpyIn( (S_CpyIn[]){{.to = {._byte = 0, ._bit = 0}, .nBits = 1, .destEndian = eLittleEndian}}, (U8[]){0x01,0x00, [2 ... _BufSize-1] = 0x00} );


   #endif

}
