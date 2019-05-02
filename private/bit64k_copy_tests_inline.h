/* -------------------------------------------------------------------------
|
|  Manual test code for bit64K_Cpy...
|
|  To be inlined in a main.c
|
----------------------------------------------------------------------------- */

PRIVATE int prntBitAddr(C8 *out, T_bit64K bf)
   { return sprintf( out, "(%u,%u)", bit64K_Byte(bf),  bit64K_BitBE(bf) ); }

// Test buffers
#define _BufSize 10
PRIVATE U8 srcBuf[_BufSize]  = {0x80,0x00, [2 ... _BufSize-1] = 0x00};
PRIVATE U8 destBuf[_BufSize] = {0xAA,0x00, [2 ... _BufSize-1] = 0x00};

// Port read(), write(), get()
PRIVATE bool rds(U8 *to, bit64K_atByte from, bit64K_T_Cnt cnt ) { memcpy(to, &destBuf[from], cnt); return true; }
PRIVATE bool wrs(bit64K_atByte to, U8 const *from, bit64K_T_Cnt cnt) { memcpy(&destBuf[to], from, cnt); return true; }
PRIVATE bool getss(U8 *to, bit64K_atByte from, bit64K_T_Cnt cnt) { memcpy(to, &srcBuf[from], cnt); return true; }

// Ports, with and without cache.
PRIVATE bit64K_Ports const portNoCache = {.dest.rd = rds, .dest.wr = wrs, .src.get = getss, .cache = NULL };
// Cache is added with bit64K_NewPort().
#define _RdCacheBufSize 3
PRIVATE U8 rdCacheBuf[_RdCacheBufSize];
PRIVATE bit64K_Cache rdCache;
PRIVATE bit64K_Ports portWCache = {.dest.rd = rds, .dest.wr = wrs, .src.get = getss, .cache = NULL };


typedef struct { U8 _byte, _bit; } S_BA;
typedef struct { S_BA from, to; U8 nBits; E_EndianIs destEndian; } S_Cpy;
typedef struct { S_BA to; U8 nBits; U8 destFill; E_EndianIs destEndian; } S_CpyIn;
typedef struct { S_BA from; U8 nBits; U8 destFill; E_EndianIs srcEndian; } S_CpyOut;

// -----------------------------------------------------------------------------------------
static void cpyIn(S_CpyIn const *in, U8 const *src)
{
   memset(destBuf, in->destFill, _BufSize);

   bit64K_In(
      &portNoCache,
      // Convert destination byte/bit into a bit address in the endian of the destination.
      // The resulting bit addresses are monotonic in the destination endianness.
      in->destEndian == eBigEndian
         ? bit64K_MakeBE(in->to._byte, in->to._bit)
         : bit64K_MakeLE(in->to._byte, in->to._bit),
      src,
      in->nBits,
      in->destEndian,
      false);

   printf("CpyIn: src[0x%x 0x%x](%s) -> (%d,%d){%d}} -> dest[0x%x 0x%x]\r\n",
          src[0], src[1], PrintEndian(in->destEndian),
          in->to._byte ,in->to._bit, in->nBits, destBuf[0], destBuf[1]);
}

// -----------------------------------------------------------------------------------------
static void cpyOut(S_CpyOut const *out, U8 const *src)
{
   memcpy(srcBuf, src, _BufSize);               // bit64K_Out() will read from here.
   memset(destBuf, out->destFill, _BufSize);

   bit64K_Out(
      &portNoCache,
      destBuf,
      // Convert destination byte/bit into a bit address in the endian of the destination.
      // The resulting bit addresses are monotonic in the destination endianness.
      out->srcEndian == eBigEndian
         ? bit64K_MakeBE(out->from._byte, out->from._bit)
         : bit64K_MakeLE(out->from._byte, out->from._bit),
      out->nBits,
      out->srcEndian);

   printf("CpyOut: .... src[0x%x 0x%x](%d,%d){%d}} (%s) -> dest[0x%x 0x%x]\r\n",
          src[0], src[1],
          out->from._byte ,out->from._bit, out->nBits,
          PrintEndian(out->srcEndian),
          destBuf[0], destBuf[1]);
}

// -----------------------------------------------------------------------------------------
static void cpyOutCache(S_CpyOut const *out, U8 const *src)
{
   bit64K_NewPort(&portWCache, &rdCache, rdCacheBuf, _RdCacheBufSize);

   memcpy(srcBuf, src, _BufSize);               // bit64K_Out() will read from here.
   memset(destBuf, out->destFill, _BufSize);

   bit64K_Out(
      &portWCache,
      destBuf,
      // Convert destination byte/bit into a bit address in the endian of the destination.
      // The resulting bit addresses are monotonic in the destination endianness.
      out->srcEndian == eBigEndian
         ? bit64K_MakeBE(out->from._byte, out->from._bit)
         : bit64K_MakeLE(out->from._byte, out->from._bit),
      out->nBits,
      out->srcEndian);

   printf("CpyOutCache: src[0x%x 0x%x 0x%x 0x%x](%d,%d){%d}} (%s) -> dest[0x%x 0x%x 0x%x 0x%x]\r\n",
          src[0], src[1], src[2], src[3],
          out->from._byte ,out->from._bit, out->nBits,
          PrintEndian(out->srcEndian),
          destBuf[0], destBuf[1], destBuf[2], destBuf[3]);

}

// ---------------------------------------------------------------------------------------------------
static void cpys(void)
{
   S_Cpy cp0 = { .from = {._byte = 0, ._bit = 0}, .to = {._byte = 0, ._bit = 1}, .nBits = 2 };
   bit64K_Copy(&portNoCache, bit64K_MakeBE(cp0.to._byte ,cp0.to._bit), bit64K_MakeBE(cp0.from._byte ,cp0.from._bit), cp0.nBits);
   printf("src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]\r\n",
          srcBuf[0], srcBuf[1], cp0.from._byte ,cp0.from._bit, cp0.nBits,
          cp0.to._byte ,cp0.to._bit, destBuf[0], destBuf[1]);


   S_Cpy cp1 = { .from = {._byte = 0, ._bit = 7}, .nBits = 16 };
   bit64K_Out(&portNoCache, destBuf, bit64K_MakeBE(cp1.from._byte ,cp1.from._bit), cp1.nBits, eBigEndian);
   printf("src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]\r\n",
          srcBuf[0], srcBuf[1], cp1.from._byte ,cp1.from._bit, cp1.nBits,
          cp1.to._byte ,cp1.to._bit, destBuf[0], destBuf[1]);

   bool rtn = bit64K_ParmFitsField( (U8[1]){0x3F}, 1, 6, false);

   C8 b0[100];
   strcpy(b0, "aaaaaaaaaaaaaaaaaaa");

   tiny1_sprintf(b0, "%02d:%02d:%02d  rtn %d", 12, 34, 56, rtn);
   printf("b0 = %s\r\n", b0);
}

// -------------------------------------------------------------------------------------------------
static void cpyIns(void)
{
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
}

// -------------------------------------------------------------------------------------------------
static void cpyOuts(void)
{
   cpyOut( (S_CpyOut[]){{.from = {._byte = 0, ._bit = 0}, .nBits = 1, .destFill = 0x00, .srcEndian = eBigEndian}}, (U8[]){0x01, [1 ... _BufSize-1] = 0x00} );
}

// -------------------------------------------------------------------------------------------------
static void cpyOutsCached(void)
{
   cpyOutCache( (S_CpyOut[]){{.from = {._byte = 0, ._bit = 0}, .nBits = 1,  .destFill = 0x00, .srcEndian = eBigEndian}}, (U8[]){0x01, [1 ... _BufSize-1] = 0x00} );
   cpyOutCache( (S_CpyOut[]){{.from = {._byte = 0, ._bit = 7}, .nBits = 24, .destFill = 0x00, .srcEndian = eBigEndian}}, (U8[]){1,2,3, [3 ... _BufSize-1] = 0x00} );
}

// ============================================== eof ========================================================
