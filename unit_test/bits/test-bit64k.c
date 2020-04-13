#include "libs_support.h"
   #if _TARGET_IS == _TARGET_UNITY_TDD
#include "unity.h"
   #endif
#include <stdlib.h>
#include <stdio.h>
#include "tdd_common.h"
#include <time.h>
#include "util.h"
#include <string.h>

   #if _TARGET_IS != _TARGET_UNITY_TDD
#define TEST_FAIL()
#define TEST_ASSERT_EQUAL_UINT8_MESSAGE(...)
#define TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(...)
#define TEST_FAIL_MESSAGE(...)
   #endif // _TARGET_IS

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

#define _TstBufSz 10
PRIVATE U8 srcBuf[_TstBufSz] = {0};
PRIVATE U8 destBuf[_TstBufSz] = {0};

PRIVATE bool rds(U8 *to, bit64K_atByte from, bit64K_T_Cnt cnt )      { memcpy(to, &destBuf[from], cnt); return true; }
PRIVATE bool wrs(bit64K_atByte to, U8 const *from, bit64K_T_Cnt cnt) { memcpy(&destBuf[to], from, cnt); return true; }
PRIVATE bool getss(U8 *to, bit64K_atByte from, bit64K_T_Cnt cnt)     { memcpy(to, &srcBuf[from], cnt); return true; }

PRIVATE bit64K_Ports const portNoCache = {
   .dest = {.rd = rds, .wr = wrs},
   .src = {.get = getss, .range = {.bits = {.min = 0, .max = 0}}, .maxOutBytes = _TstBufSz} };

/* Cache for bit64K_Out(). Is added with bit64K_NewPort().

   The cache is just 3 bytes; small so can easily check that it reloads when necessary. Note
   that the cache
*/
#define _RdCacheBufSize 3
PRIVATE U8 rdCacheBuf[_RdCacheBufSize];
PRIVATE bit64K_Cache rdCache;

PRIVATE bit64K_Ports portWCache = {
   .dest = {.rd = rds, .wr = wrs},
   .src = {.get = getss, .range = {.bits = {.min = 0, .max = 0}}, .maxOutBytes = _TstBufSz },
   .cache = NULL };


typedef struct { U8 _byte, _bit; } S_BitAddr;
typedef struct { S_BitAddr from, to; U8 nBits; } S_CpySpec;

/* ------------------------------- test_Bit64_Copy_1Bit --------------------------------------------------- */

void test_Bit64_Copy_1Bit(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      // ----- From the 1st source source byte to the 1st dest byte
      //       Checks just the bit arithmetic.

      // lsbit -> lsbit src[0] -> dest[0].  Test copy of '1' and '0'.
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly just the lsbit is copied.
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0xFE, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },

      // src[7] -> dest[7].    Test copy of '1' and '0'.
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly just the msbit is copied.
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0x80, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0x7F, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TstBufSz-1] = 0xFF} },

      // From src b7 to dest b0.    Test copy of '1' and '0'.
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly just the msbit is copied.
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x80, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x7F, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },

      // src[6] -> dest[1]
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x02, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFD, [1 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly src[6] is copied.
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0x40, .destFill = 0x00, .result = (U8[]){0x02, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0xBF, .destFill = 0xFF, .result = (U8[]){0xFD, [1 ... _TstBufSz-1] = 0xFF} },

      // src[5] -> dest[2]
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x04, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFB, [1 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly src[5] is copied.
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0x20, .destFill = 0x00, .result = (U8[]){0x04, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0xDF, .destFill = 0xFF, .result = (U8[]){0xFB, [1 ... _TstBufSz-1] = 0xFF} },

      // src[1] -> dest[4]
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x10, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xEF, [1 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly src[1] is copied.
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0x02, .destFill = 0x00, .result = (U8[]){0x10, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0xFD, .destFill = 0xFF, .result = (U8[]){0xEF, [1 ... _TstBufSz-1] = 0xFF} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_Copy(
         &portNoCache,
         bit64K_MakeBE(cpy->to._byte,   cpy->to._bit),
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_Copy_1Bit() should return true");

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1], cpy->from._byte ,cpy->from._bit, cpy->nBits,
            cpy->to._byte ,cpy->to._bit, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}

/* ------------------------------- test_Bit64_Copy_StraddlesDestBytes ------------------------------------------ */

void test_Bit64_Copy_StraddlesDestBytes(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      // ----- Multi-bit copies which straddle multiple destination bytes

      // src[(0,0):(1,7)] -> dest[(0,0):(1,7)].
      // Src and dest are aligned; read 2 src bytes to fill 2 dest bytes
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){[0 ... _TstBufSz-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x01,0x80,  [2 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){[0 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFE,0x7F,  [2 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly src[(0,0):(1,7)] are copied.
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){0x01,0x80, [2 ... _TstBufSz-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x01,0x80,  [2 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){0xFE,0x7F, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFE,0x7F,  [2 ... _TstBufSz-1] = 0xFF} },

      // src[(0,0):(1,7)] -> dest[(0,1:0)].
      // Src and dest are not aligned. Read 2 src bytes to fill 1 dest byte.
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){[0 ... _TstBufSz-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x03, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){[0 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFC, [1 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly src[(0,0):(1,7)] are copied.
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){0x01,0x80, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x00, .result = (U8[]){0x03, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){0xFE,0x7F, [2 ... _TstBufSz-1] = 0xFF}, .destFill = 0xFF, .result = (U8[]){0xFC, [1 ... _TstBufSz-1] = 0xFF} },

      // src[(0,3:0)] -> dest[(0,1:0):(1,7:6)].
      // Src and dest are not aligned. Read 1 src byte to fill 2 dest bytes.
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){[0 ... _TstBufSz-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x03,0xC0, [2 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){[0 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFC,0x3F, [2 ... _TstBufSz-1] = 0xFF} },
      // Make sure exactly src[(0,3:0)] are copied.
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){0x0F,[1 ... _TstBufSz-1] = 0x00}, .destFill = 0x00, .result = (U8[]){0x03,0xC0, [2 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){0xF0,[1 ... _TstBufSz-1] = 0xFF}, .destFill = 0xFF, .result = (U8[]){0xFC,0x3F, [2 ... _TstBufSz-1] = 0xFF} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_Copy(
         &portNoCache,
         bit64K_MakeBE(cpy->to._byte,   cpy->to._bit),
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_Copy_StraddlesDestBytes() should return true");

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1], cpy->from._byte ,cpy->from._bit, cpy->nBits,
            cpy->to._byte ,cpy->to._bit, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
   }
}

// ------------------------------------------------------------------------------------------------------
static C8 const * printOne_Bit64_Out(U8 tstIdx, S_CpySpec const *cpy) {
   static C8 b0[100];
   sprintf(b0, "tst #%d:  src[0x%x 0x%x 0x%x] map {(%d,%d){%d} -> dest[0]}.  dest[0x%x 0x%x 0x%x]",
         tstIdx,
         srcBuf[0], srcBuf[1], srcBuf[2], cpy->from._byte ,cpy->from._bit, cpy->nBits,
         destBuf[0], destBuf[1], destBuf[2]);
   return b0;
}

/* ------------------------------- test_Bit64_Out_LE --------------------------------------------------- */

void test_Bit64_Out_LE(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      // --- Copy out from single source byte to just the 1st dest byte; remaining dest bytes should be undisturbed.

      // Without shift; source bits already aligned.
      #define _destFill 0x05A
      // Just the lsb
      { .cpy = {.from = {0,0}, .nBits = 1 }, .src = (U8[]){0x01, [1 ... _TstBufSz-1] = 0x00}, .destFill = _destFill, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = _destFill} },
      { .cpy = {.from = {0,0}, .nBits = 1 }, .src = (U8[]){0xFF, [1 ... _TstBufSz-1] = 0x00}, .destFill = _destFill, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = _destFill} },
      { .cpy = {.from = {0,0}, .nBits = 1 }, .src = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF}, .destFill = _destFill, .result = (U8[]){0x00, [1 ... _TstBufSz-1] = _destFill} },

      // e.g b2:0 (lowest 3 bits)
      { .cpy = {.from = {0,2}, .nBits = 3 }, .src = (U8[]){0xFF, [1 ... _TstBufSz-1] = 0x00}, .destFill = 0x00, .result = (U8[]){0x07, [1 ... _TstBufSz-1] = 0x00} },
      { .cpy = {.from = {0,2}, .nBits = 3 }, .src = (U8[]){0x05, [1 ... _TstBufSz-1] = 0xFF}, .destFill = 0x55, .result = (U8[]){0x05, [1 ... _TstBufSz-1] = 0x55} },

      // Whole byte
      { .cpy = {.from = {0,7}, .nBits = 8 }, .src = (U8[]){0x85, [1 ... _TstBufSz-1] = 0x00}, .destFill = _destFill, .result = (U8[]){0x85, [1 ... _TstBufSz-1] = _destFill} },
      { .cpy = {.from = {0,7}, .nBits = 8 }, .src = (U8[]){0x7B, [1 ... _TstBufSz-1] = 0xFF}, .destFill = _destFill, .result = (U8[]){0x7B, [1 ... _TstBufSz-1] = _destFill} },

      // With shift; source bit field must be right-justified into dest byte.

      // b1 -> b0
      { .cpy = {.from = {0,1}, .nBits = 1 }, .src = (U8[]){0x02, [1 ... _TstBufSz-1] = 0x00}, .destFill = 0x55, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0x55} },
      { .cpy = {.from = {0,1}, .nBits = 1 }, .src = (U8[]){0xFD, [1 ... _TstBufSz-1] = 0xFF}, .destFill = 0x55, .result = (U8[]){0x00, [1 ... _TstBufSz-1] = 0x55} },
      // b[7:6] -> b[1:0]
      { .cpy = {.from = {0,7}, .nBits = 2 }, .src = (U8[]){0xC0, [1 ... _TstBufSz-1] = 0x00}, .destFill = 0x55, .result = (U8[]){0x03, [1 ... _TstBufSz-1] = 0x55} },
      { .cpy = {.from = {0,7}, .nBits = 2 }, .src = (U8[]){0x3F, [1 ... _TstBufSz-1] = 0xFF}, .destFill = 0x55, .result = (U8[]){0x00, [1 ... _TstBufSz-1] = 0x55} },
      // b[7:1] -> b[6:0]
      { .cpy = {.from = {0,7}, .nBits = 7 }, .src = (U8[]){0xAA, [1 ... _TstBufSz-1] = 0x00}, .destFill = 0x55, .result = (U8[]){0x55, [1 ... _TstBufSz-1] = 0x55} },

      // Multiple bytes; no shift
      { .cpy = {.from = {0,7}, .nBits = 16 }, .src = (U8[]){0x01, 0x02, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x55, .result = (U8[]){0x01,0x02, [2 ... _TstBufSz-1] = 0x55} },

      // Source bit field extends over 2-bytes but is < 8bits.  Field must be right-justified into dest byte.
      { .cpy = {.from = {0,1}, .nBits = 4 }, .src = (U8[]){0x03,0xC0, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x55, .result = (U8[]){0x0F, [1 ... _TstBufSz-1] = 0x55} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_Out(
         &portNoCache,
         destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eLittleEndian);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_Out_LE() should return true");
      C8 const *b0 = printOne_Bit64_Out(i, cpy);
      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
   }
}

/* ------------------------------- test_Bit64_Out_BE --------------------------------------------------- */

void test_Bit64_Out_BE(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      // Read lsbit '1' from 1 byte
      { .cpy = {.from = {0,0}, .nBits = 1 }, .src = (U8[]){0x01}, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      // Ensure just b0<-1 is copied to out
      { .cpy = {.from = {0,0}, .nBits = 1 }, .src = (U8[]){0xFF}, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      // read lsbit '0' .
      { .cpy = {.from = {0,0}, .nBits = 1 }, .src = (U8[]){0x00}, .destFill = 0xFF, .result = (U8[]){0x00, [1 ... _TstBufSz-1] = 0xFF} },
      // Ensure just b0<-0 is copied to out
      { .cpy = {.from = {0,0}, .nBits = 1 }, .src = (U8[]){0xFE}, .destFill = 0xFF, .result = (U8[]){0x00, [1 ... _TstBufSz-1] = 0xFF} },

      // e.g b2:0 (lowest 3 bits)
      { .cpy = {.from = {0,2}, .nBits = 3 }, .src = (U8[]){0xFF, [1 ... _TstBufSz-1] = 0x00}, .destFill = 0x00, .result = (U8[]){0x07, [1 ... _TstBufSz-1] = 0x00} },
      { .cpy = {.from = {0,2}, .nBits = 3 }, .src = (U8[]){0x05, [1 ... _TstBufSz-1] = 0xFF}, .destFill = 0x55, .result = (U8[]){0x05, [1 ... _TstBufSz-1] = 0x55} },

      // A whole byte.
      { .cpy = {.from = {0,7}, .nBits = 8 }, .src = (U8[]){0x5A}, .destFill = 0x00, .result = (U8[]){0x5A, [1 ... _TstBufSz-1] = 0} },

      // msbit... same tests as lsbit above.
      { .cpy = {.from = {0,7}, .nBits = 1 }, .src = (U8[]){0x80}, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,7}, .nBits = 1 }, .src = (U8[]){0xFF}, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },

      { .cpy = {.from = {0,7}, .nBits = 1 }, .src = (U8[]){0x00}, .destFill = 0xFF, .result = (U8[]){0x00, [1 ... _TstBufSz-1] = 0xFF} },
      { .cpy = {.from = {0,7}, .nBits = 1 }, .src = (U8[]){0x7F}, .destFill = 0xFF, .result = (U8[]){0x00, [1 ... _TstBufSz-1] = 0xFF} },

      // Multiple bytes; no shift
      { .cpy = {.from = {0,7}, .nBits = 16 }, .src = (U8[]){1,2,     [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x55, .result = (U8[]){2,1,     [2 ... _TstBufSz-1] = 0x55} },
      { .cpy = {.from = {0,7}, .nBits = 24 }, .src = (U8[]){1,2,3, [3 ... _TstBufSz-1] = 0}, .destFill = 0, .result = (U8[]){3,2,1, [3 ... _TstBufSz-1] = 0} },
      { .cpy = {.from = {0,7}, .nBits = 32 }, .src = (U8[]){1,2,3,4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0x55, .result = (U8[]){4,3,2,1, [4 ... _TstBufSz-1] = 0x55} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_Out(
         &portNoCache,
         destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eBigEndian);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_Out_BE() should return true");
      C8 const *b0 = printOne_Bit64_Out(i, cpy);
      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}

/* ------------------------------- test_Bit64_Out_BE_wCache ---------------------------------------------------

   This test checks the read cache. The cache is bytes, so not much need to re-check bit addressing.
*/
void test_Bit64_Out_BE_wCache(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; bool preFlushCache; } S_Tst;

   S_Tst const tsts[] = {
      // 1. Read a byte from 'src'. That byte should get cached.
      { .cpy = {.from = {0,7}, .nBits = 8 }, .src = (U8[]){0x5A}, .destFill = 0x00, .result = (U8[]){0x5A, [1 ... _TstBufSz-1] = 0} },
      // 2. Clear 'src' then re-read the same byte.  We should get the previous byte (0x5A) back from the cache.
      { .cpy = {.from = {0,7}, .nBits = 8 }, .src = (U8[]){0x22}, .destFill = 0x00, .result = (U8[]){0x5A, [1 ... _TstBufSz-1] = 0} },
      // 3. Same as 2. but force a cache-flush. Now should get updated value from 'src'
      { .cpy = {.from = {0,7}, .nBits = 8 }, .src = (U8[]){0x33}, .destFill = 0x00, .result = (U8[]){0x33, [1 ... _TstBufSz-1] = 0}, .preFlushCache = true },
      // 4. Change 'src' and re-read. Should get cache-value (again).
      { .cpy = {.from = {0,7}, .nBits = 8 }, .src = (U8[]){0x99}, .destFill = 0x00, .result = (U8[]){0x33, [1 ... _TstBufSz-1] = 0} },

      // Above, but with 3 bytes; the size of the test cache. Target is this X86, little-endian, so bytes are reversed.
      { .cpy = {.from = {0,7}, .nBits = 24}, .src = (U8[]){1,2,3, [3 ... _TstBufSz-1] = 0}, .destFill = 0, .result = (U8[]){3,2,1, [3 ... _TstBufSz-1] = 0}, .preFlushCache = true },
   };

   bool newPortRtn = bit64K_NewPort(&portWCache, &rdCache, rdCacheBuf, _RdCacheBufSize);

   TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, newPortRtn, "test_Bit64_Out_BE_wCache() bit64K_NewPort() should return true");

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      if(t->preFlushCache == true) {
         bit64K_FlushCache(&portWCache); }

      bool rtn = bit64K_Out(
         &portWCache,
         destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eBigEndian);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_Out_BE_wCache() should return true");
      C8 const *b0 = printOne_Bit64_Out(i, cpy);
      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}

/* ------------------------------- test_Bit64_Out_NoEndian_wCache ---------------------------------------------------

   This test checks the read cache. The cache is bytes, so not much need to re-check bit addressing.
*/
void test_Bit64_Out_NoEndian_wCache(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; bool preFlushCache; } S_Tst;

   S_Tst const tsts[] = {
      // 1. Same as in test_Bit64_Out_BE_wCache() (above), but bytes are NOT reversed.
      { .cpy = {.from = {0,7}, .nBits = 24}, .src = (U8[]){1,2,3, [3 ... _TstBufSz-1] = 0}, .destFill = 0, .result = (U8[]){1,2,3, [3 ... _TstBufSz-1] = 0}, .preFlushCache = true },
      // Clear 'src' and re-read. Should get back what's in the cache.
      { .cpy = {.from = {0,7}, .nBits = 24}, .src = (U8[]){[0 ... _TstBufSz-1] = 0}, .destFill = 0, .result = (U8[]){1,2,3, [3 ... _TstBufSz-1] = 0} },

      /* 2. Another 3 bytes, but move 'get' up 2 bytes. The cache is just 3 bytes ('_RdCacheBufSize') so this next 'get'
            is partly outside the cache and so should force a reload (of [3,4,5] )
      */
      { .cpy = {.from = {2,7}, .nBits = 24}, .src = (U8[]){1,2,3,4,5, [7 ... _TstBufSz-1] = 0}, .destFill = 0, .result = (U8[]){3,4,5, [3 ... _TstBufSz-1] = 0} },

      /* 3. Keep 'get' where it is, but get more bytes. Again, the cache should reload.
      */
      { .cpy = {.from = {2,7}, .nBits = 40}, .src = (U8[]){1,2,3,4,5,6,7, [8 ... _TstBufSz-1] = 0}, .destFill = 0, .result = (U8[]){3,4,5,6,7, [5 ... _TstBufSz-1] = 0} },
   };

   bool newPortRtn = bit64K_NewPort(&portWCache, &rdCache, rdCacheBuf, _RdCacheBufSize);

   TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, newPortRtn, "test_Bit64_Out_NoEndian_wCache() bit64K_NewPort() should return true");

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      if(t->preFlushCache == true) {
         bit64K_FlushCache(&portWCache); }

      bool rtn = bit64K_Out(
         &portWCache,
         destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eNoEndian);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_Out_NoEndian_wCache() should return true");
      C8 const *b0 = printOne_Bit64_Out(i, cpy);
      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}


/* ------------------------------- test_Bit64_Out_AddrRange ---------------------------------------------------

*/
void test_Bit64_Out_AddrRange(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src; U8 destFill; U8 const *result;
                     struct{T_bit64K lo, hi;}bitRange; bit64K_atByte maxOutBytes;
                     bool rtn, destIsNull; } S_Tst;

   S_Tst const tsts[] = {
      // Get (3) bytes; no-range checking; no output buffer limit. Does transfer; returns 'true'.
      // Note this test treats the source address as big-endian, meaning (0,7) -> bit-address 0.
      { .cpy = {.from = {0,7}, .nBits = 24 }, .src = (U8[]){1,2,3}, .destFill = 0x00, .result = (U8[]){3,2,1, [3 ... _TstBufSz-1] = 0},
        .bitRange = {.lo = 0, .hi = 0}, .maxOutBytes = 0, .rtn = true },

      // (3) bytes from bit-address 0-23. Make limits exactly to size. Should get, same as above.
      { .cpy = {.from = {0,7}, .nBits = 24 }, .src = (U8[_TstBufSz]){1,2,3}, .destFill = 0x00, .result = (U8[]){3,2,1, [3 ... _TstBufSz-1] = 0},
        .bitRange = {.lo = 0, .hi = 23}, .maxOutBytes = 0, .rtn = true },

      // Lo limit 1 too high. Should fail (and no 'result' into 'destBuf')
      { .cpy = {.from = {0,7}, .nBits = 24 }, .src = (U8[_TstBufSz]){1,2,3}, .destFill = 0x00, .result = (U8[_TstBufSz]){0},
        .bitRange = {.lo = 1, .hi = 23}, .maxOutBytes = 0, .rtn = false },

      // Hi limit 1 too low. Should also fail...
      { .cpy = {.from = {0,7}, .nBits = 24 }, .src = (U8[_TstBufSz]){1,2,3}, .destFill = 0x00, .result = (U8[_TstBufSz]){0},
        .bitRange = {.lo = 0, .hi = 22}, .maxOutBytes = 0, .rtn = false },

      // A 1-bit range, where 'lo' == 'hi' is legal. For example, get the 9th bit, which is the msbit of src[1] (big-endian)
      { .cpy = {.from = {1,7}, .nBits = 1 }, .src = (U8[_TstBufSz]){0,0x80,0}, .destFill = 0x00, .result = (U8[]){0x01, [2 ... _TstBufSz-1] = 0},
        .bitRange = {.lo = 8, .hi = 8}, .maxOutBytes = 0, .rtn = true },

      // ...but trying to get 2 bits from a 1-bit range fails
      { .cpy = {.from = {1,7}, .nBits = 2 }, .src = (U8[_TstBufSz]){0,0x80,0}, .destFill = 0x00, .result = (U8[_TstBufSz]){0},
        .bitRange = {.lo = 8, .hi = 8}, .maxOutBytes = 0, .rtn = false },

      // An upside-down range spec is ignored... transfer proceeds.
      { .cpy = {.from = {0,7}, .nBits = 24 }, .src = (U8[]){1,2,3}, .destFill = 0x00, .result = (U8[]){3,2,1, [3 ... _TstBufSz-1] = 0},
        .bitRange = {.lo = 5, .hi = 3}, .maxOutBytes = 0, .rtn = true },
   };

   bit64K_Ports portA = portNoCache;            // Local copy of port; which we add limits to.

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      // Add to the port limits on source address and dest bytes for the test.
      portA.src.maxOutBytes = t->maxOutBytes;
      portA.src.range.bits.min = t->bitRange.lo;
      portA.src.range.bits.max = t->bitRange.hi;

      bool rtn = bit64K_Out(
         &portA,
         t->destIsNull == true ? NULL : destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eBigEndian);

      if(t->rtn == false) {
         if(rtn == true) {
            C8 b0[100];
            sprintf(b0, "tst #%d: bit64K_Out() should return false; returns true", i);
            TEST_FAIL_MESSAGE(b0); }
      }
      else {
         if(rtn == false) {
            C8 b0[100];
            sprintf(b0, "tst #%d: bit64K_Out() should return true; returns false", i);
            TEST_FAIL_MESSAGE(b0); }
         else {
            C8 const *b0 = printOne_Bit64_Out(i, cpy);
            TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0); }}
   }
}

/* ------------------------------- test_Bit64_Out_OutputBytesLimit ---------------------------------------------------

*/
void test_Bit64_Out_OutputBytesLimit(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src; U8 destFill; U8 const *result;
                     struct{T_bit64K lo, hi;}bitRange; bit64K_atByte maxOutBytes;
                     bool rtn, destIsNull; } S_Tst;

   S_Tst const tsts[] = {
      // Get (5) bytes; no-range checking; no output buffer limit. Does transfer; returns 'true'.
      { .cpy = {.from = {0,7}, .nBits = 40 }, .src = (U8[]){1,2,3,4,5}, .destFill = 0x00,
        .result = (U8[]){5,4,3,2,1, [5 ... _TstBufSz-1] = 0},
        .bitRange = {.lo = 0, .hi = 0}, .maxOutBytes = 0, .rtn = true },

      // Request (5) bytes; but limit output to just 4. Returns false; no transfer.
      { .cpy = {.from = {0,7}, .nBits = 40 }, .src = (U8[]){1,2,3,4,5}, .destFill = 0x00,
        .result = (U8[_TstBufSz]){0},
        .bitRange = {.lo = 0, .hi = 0}, .maxOutBytes = 4, .rtn = false },

      // Get (5) bytes and limit output to 5. Does transfer; returns 'true'.
      { .cpy = {.from = {0,7}, .nBits = 40 }, .src = (U8[]){1,2,3,4,5}, .destFill = 0x00,
        .result = (U8[]){5,4,3,2,1, [5 ... _TstBufSz-1] = 0},
        .bitRange = {.lo = 0, .hi = 0}, .maxOutBytes = 0, .rtn = true },

      // Request (5) bytes, limit output to just 4, but 'dest' is NULL so ignore the output limit.
      // No transfer; just reload cache (cuz 'dest' == NULL). Returns true.
      { .cpy = {.from = {0,7}, .nBits = 40 }, .src = (U8[]){1,2,3,4,5}, .destFill = 0x00,
        .result = (U8[_TstBufSz]){0},
        .bitRange = {.lo = 0, .hi = 0}, .maxOutBytes = 4, .rtn = true, .destIsNull = true },

      // Request zero bits. Does nothing, returns true.
      { .cpy = {.from = {0,7}, .nBits = 0 }, .src = (U8[]){1,2,3,4,5}, .destFill = 0x00,
        .result = (U8[_TstBufSz]){0},
        .bitRange = {.lo = 0, .hi = 0}, .maxOutBytes = 4, .rtn = true, .destIsNull = true },
   };

   bit64K_Ports portA = portNoCache;            // Local copy of port; which we add limits to.

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      // Add to the port limits on source address and dest bytes for the test.
      portA.src.maxOutBytes = t->maxOutBytes;
      portA.src.range.bits.min = t->bitRange.lo;
      portA.src.range.bits.max = t->bitRange.hi;

      bool rtn = bit64K_Out(
         &portA,
         t->destIsNull == true ? NULL : destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eBigEndian);

      if(t->rtn == false) {
         if(rtn == true) {
            C8 b0[100];
            sprintf(b0, "tst #%d: bit64K_Out() should return false; returns true", i);
            TEST_FAIL_MESSAGE(b0); }
      }
      else {
         if(rtn == false) {
            C8 b0[100];
            sprintf(b0, "tst #%d: bit64K_Out() should return true; returns false", i);
            TEST_FAIL_MESSAGE(b0); }
         else {
            C8 const *b0 = printOne_Bit64_Out(i, cpy);
            TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0); }}
   }

}

// -----------------------------------------------------------------------------------------------------
#define _SrcIsNotEndian false
#define _SrcIsEndian    true

/* ------------------------------- test_Bit64_In_LE --------------------------------------------------- */

void test_Bit64_In_LE(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; bool srcIsEndian; } S_Tst;

   S_Tst const tsts[] = {
      // Into just one dest byte.... set lsbit
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      // ...clear lsbit
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0xFE, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },
      // Set msbit...
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TstBufSz-1] = 0} },
      // ... clear msbit
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TstBufSz-1] = 0xFF} },
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0xFE, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TstBufSz-1] = 0xFF} },

      { .cpy = {.to = {0,0}, .nBits = 2 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x03, [1 ... _TstBufSz-1] = 0x00} },

      // Straddle 2 bytes .... Set all bits in the field
      { .cpy = {.to = {0,6}, .nBits = 4 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0xC0, 0x03, [2 ... _TstBufSz-1] = 0x00} },
      { .cpy = {.to = {0,6}, .nBits = 4 }, .srcFill = 0x0F, .destFill = 0x00, .result = (U8[]){0xC0, 0x03, [2 ... _TstBufSz-1] = 0x00} },
      // Clear all bits in the field.
      { .cpy = {.to = {0,6}, .nBits = 4 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0x3F, 0xFC, [2 ... _TstBufSz-1] = 0xFF} },
      { .cpy = {.to = {0,6}, .nBits = 4 }, .srcFill = 0xF0, .destFill = 0xFF, .result = (U8[]){0x3F, 0xFC, [2 ... _TstBufSz-1] = 0xFF} },

      // A pattern straddling 2 bytes
      { .cpy = {.to = {0,4}, .nBits = 8 }, .srcFill = 0x5A, .destFill = 0x00, .result = (U8[]){0xA0, 0x05, [2 ... _TstBufSz-1] = 0x00} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_In(
         &portNoCache,
         bit64K_MakeLE(cpy->to._byte, cpy->to._bit),
         srcBuf,
         cpy->nBits,
         eLittleEndian,
         _SrcIsNotEndian);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_In_LE() should return true");

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1],
            cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
   }
}

/* ------------------------------- test_Bit64_In_LE_multiSrc ---------------------------------------------------

   Same as test_Bit64_In_LE() but with an array of (different) source bytes.
*/

void test_Bit64_In_LE_multiSrc(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; bool srcIsEndian; } S_Tst;

   S_Tst const tsts[] = {
      // Straddling 3 or 4 bytes..
      { .cpy = {.to = {0,4}, .nBits = 16 }, .src = (U8[]){0x5A, 0x3C, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
                                             .result = (U8[]){0xA0, 0xC5, 0x03, [3 ... _TstBufSz-1] = 0x00} },

      { .cpy = {.to = {1,4}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
                                             .result = (U8[]){0x00, 0xA0, 0xC5, 0x63, 0x09, [5 ... _TstBufSz-1] = 0x00} }
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_In(
         &portNoCache,
         bit64K_MakeLE(cpy->to._byte, cpy->to._bit),
         srcBuf,
         cpy->nBits,
         eLittleEndian,
         _SrcIsNotEndian);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_In_LE_multiSrc() should return true");

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1],
            cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
   }
}



/* ------------------------------- test_Bit64_In_LE_multiSrc_Endian ---------------------------------------------------

   Same as test_Bit64_In_BE_multiSrc() but with an array of (different) source bytes that are little endian dependent.
*/

void test_Bit64_In_LE_multiSrc_Endian(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; bool srcIsEndian; } S_Tst;

   S_Tst const tsts[] = {

      // Straddling 3 or 4 bytes..
      { .cpy = {.to = {0,4}, .nBits = 16 }, .src = (U8[]){0x5A, 0x3C, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
                                             .result = (U8[]){0xA0, 0xC5, 0x03, [3 ... _TstBufSz-1] = 0x00} },

      { .cpy = {.to = {1,4}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
                                             .result = (U8[]){0x00, 0xA0, 0xC5, 0x63, 0x09, [5 ... _TstBufSz-1] = 0x00} },

	  { .cpy = {.to = {1,2}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
											 .result = (U8[]){0x00, 0x68, 0xF1, 0x58, 0x02, [5 ... _TstBufSz-1] = 0x00} },

	  { .cpy = {.to = {1,2}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF,
											 .result = (U8[]){0xFF, 0x6B, 0xF1, 0x58, 0xFE, [5 ... _TstBufSz-1] = 0xFF} },

	  { .cpy = {.to = {0,6}, .nBits = 11 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
											 .result = (U8[]){0x80, 0x16, 0x01, [3 ... _TstBufSz-1] = 0x00} },

	  { .cpy = {.to = {0,7}, .nBits = 17 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
											 .result = (U8[]){0x00, 0x2D, 0x1E, [3 ... _TstBufSz-1] = 0x00} },

	  { .cpy = {.to = {0,2}, .nBits = 21 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
											 .result = (U8[]){0x68, 0xF1, 0x58, 0x00, 0x00, [5 ... _TstBufSz-1] = 0x00} },

	  { .cpy = {.to = {0,2}, .nBits = 21 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF,
											 .result = (U8[]){0x6B, 0xF1, 0xD8, 0xFF, 0xFF, [5 ... _TstBufSz-1] = 0xFF} },

	  { .cpy = {.to = {0,5}, .nBits = 27 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF,
											.result = (U8[]){0x5F, 0x8B, 0xC7, 0x92, 0xFF, [5 ... _TstBufSz-1] = 0xFF} },

	  { .cpy = {.to = {0,6}, .nBits = 29 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
											.result = (U8[]){0x80, 0x16, 0x8F, 0x25, 0x05, [5 ... _TstBufSz-1] = 0x00} },

	  { .cpy = {.to = {0,6}, .nBits = 29 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF,
											.result = (U8[]){0xBF, 0x16, 0x8F, 0x25, 0xFD, [5 ... _TstBufSz-1] = 0xFF} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_In(
         &portNoCache,
         bit64K_MakeLE(cpy->to._byte, cpy->to._bit),
         srcBuf,
         cpy->nBits,
         eLittleEndian,
         _SrcIsEndian);

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_In_LE_multiSrc() should return true");

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x 0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x 0x%x 0x%x 0x%x]]",
            i,
            srcBuf[0], srcBuf[1],srcBuf[2],srcBuf[3],
            cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1], destBuf[2], destBuf[3],destBuf[4]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
   }
}

/* ------------------------------- test_Bit64_In_BE --------------------------------------------------- */

void test_Bit64_In_BE(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; bool srcIsEndian; } S_Tst;

   S_Tst const tsts[] = {
      // Into just the 1st dest byte.... Set lsbit
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TstBufSz-1] = 0x00} },
      // Clear lsbit.
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0xFE, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TstBufSz-1] = 0xFF} },
      // Set msbit
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TstBufSz-1] = 0} },
      // Clear msbit
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0xFE, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TstBufSz-1] = 0xFF} },
      { .cpy = {.to = {0,7}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TstBufSz-1] = 0xFF} },
      // Multiple bits in the middle.
      { .cpy = {.to = {0,4}, .nBits = 4 }, .srcFill = 0x0F, .destFill = 0x00, .result = (U8[]){0x1E, [1 ... _TstBufSz-1] = 0} },
      { .cpy = {.to = {0,5}, .nBits = 4 }, .srcFill = 0xF0, .destFill = 0xFF, .result = (U8[]){0xC3, [1 ... _TstBufSz-1] = 0xFF} },

      // 2nd byte onward.
      { .cpy = {.to = {1,4}, .nBits = 4 }, .srcFill = 0x0F, .destFill = 0x00, .result = (U8[]){0x00, 0x1E, [2 ... _TstBufSz-1] = 0} },
      { .cpy = {.to = {1,5}, .nBits = 4 }, .srcFill = 0xF0, .destFill = 0xFF, .result = (U8[]){0xFF, 0xC3, [2 ... _TstBufSz-1] = 0xFF} },

      { .cpy = {.to = {2,0}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x00, 0x00, 0x01, [3 ... _TstBufSz-1] = 0} },
      { .cpy = {.to = {3,7}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x00, 0x00, 0x00, 0x80, [4 ... _TstBufSz-1] = 0} },

      // Straddling 2 bytes
      { .cpy = {.to = {0,1}, .nBits = 4 }, .srcFill = 0x0F, .destFill = 0x00, .result = (U8[]){0x03, 0xC0, [2 ... _TstBufSz-1] = 0} },
      // A pattern (straddling 2 bytes)
      { .cpy = {.to = {0,3}, .nBits = 8 }, .srcFill = 0x5A, .destFill = 0x00, .result = (U8[]){0x05, 0xA0, [2 ... _TstBufSz-1] = 0x00} },
      { .cpy = {.to = {0,3}, .nBits = 8 }, .srcFill = 0x5A, .destFill = 0xFF, .result = (U8[]){0xF5, 0xAF, [2 ... _TstBufSz-1] = 0xFF} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_In(
         &portNoCache,
         bit64K_MakeBE(cpy->to._byte, cpy->to._bit),
         srcBuf,
         cpy->nBits,
         eBigEndian,
         _SrcIsNotEndian );

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_In_BE() should return true");

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1],
            cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
   }
}

/* ------------------------------- test_Bit64_In_BE_multiSrc ---------------------------------------------------

   Same as test_Bit64_In_BE() but with an array of (different) source bytes.
*/

void test_Bit64_In_BE_multiSrc(void)
{
   typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; bool srcIsEndian; } S_Tst;

   S_Tst const tsts[] = {
      // Straddling 3 or 4 bytes..
      { .cpy = {.to = {0,3}, .nBits = 16 }, .src = (U8[]){0x5A, 0x3C, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
                                             .result = (U8[]){0x05, 0xA3, 0xC0, [3 ... _TstBufSz-1] = 0x00} },

      { .cpy = {.to = {1,3}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
                                             .result = (U8[]){0x00, 0x05, 0xA3, 0xC9, 0x60, [5 ... _TstBufSz-1] = 0x00} },

      { .cpy = {.to = {0,3}, .nBits = 28 }, .src = (U8[]){0x86, 0x5A, 0x3C, 0x96, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
                                             .result = (U8[]){0x06, 0x5A, 0x3C, 0x96, [4 ... _TstBufSz-1] = 0x00} },

   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TstBufSz );
      memset(destBuf, t->destFill, _TstBufSz );

      S_CpySpec const * cpy = &t->cpy;

      bool rtn = bit64K_In(
         &portNoCache,
         bit64K_MakeBE(cpy->to._byte, cpy->to._bit),
         srcBuf,
         cpy->nBits,
         eBigEndian,
         _SrcIsNotEndian );

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_In_BE_multiSrc() should return true");

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1],
            cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
   }
}

/* ------------------------------- test_Bit64_In_BE_multiSrc_Endian ---------------------------------------------------

   Same as test_Bit64_In_BE_multiSrc() but with an array of (different) source bytes that are big endian dependent.
*/
void test_Bit64_In_BE_multiSrc_Endian(void)
{
	typedef struct { S_CpySpec cpy; U8 const *src, destFill; U8 const *result; bool srcIsEndian; } S_Tst;

	S_Tst const tsts[] = {

		// Straddling 3 or 4 bytes..
		{ .cpy = {.to = {0,3}, .nBits = 16 }, .src = (U8[]){0x5A, 0x3C, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
		.result = (U8[]){0x03, 0xC5, 0xA0, [3 ... _TstBufSz-1] = 0x00} },

		{ .cpy = {.to = {1,3}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
		.result = (U8[]){0x00, 0x09, 0x63, 0xC5, 0xA0, [5 ... _TstBufSz-1] = 0x00} },

		{ .cpy = {.to = {1,2}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
		.result = (U8[]){0x00, 0x04, 0xB1, 0xE2, 0xD0, [5 ... _TstBufSz-1] = 0x00} },

		{ .cpy = {.to = {1,2}, .nBits = 24 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF,
		.result = (U8[]){0xFF, 0xFC, 0xB1, 0xE2, 0xD7, [5 ... _TstBufSz-1] = 0xFF} },

		{ .cpy = {.to = {0,6}, .nBits = 11 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
		.result = (U8[]){0x45, 0xA0, [2 ... _TstBufSz-1] = 0x00} },

		{ .cpy = {.to = {0,7}, .nBits = 17 }, .src = (U8[]){0x5A, 0x3C, 0x96, [3 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
		.result = (U8[]){0x1E, 0x2D, 0x00, [3 ... _TstBufSz-1] = 0x00} },

		{ .cpy = {.to = {0,2}, .nBits = 21 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
		.result = (U8[]){0x05, 0x8F, 0x16, 0x80, 0x00, [5 ... _TstBufSz-1] = 0x00} },

		{ .cpy = {.to = {0,2}, .nBits = 21 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF,
		.result = (U8[]){0xFD, 0x8F, 0x16, 0xBF, 0xFF, [5 ... _TstBufSz-1] = 0xFF} },

		{ .cpy = {.to = {0,5}, .nBits = 27 }, .src = (U8[]){0x5A, 0x3C, 0x96, 0xF4, [4 ... _TstBufSz-1] = 0x00}, .destFill = 0xFF,
		.result = (U8[]){0xE4, 0xB1, 0xE2, 0xD7, 0xFF, [5 ... _TstBufSz-1] = 0xFF} },

      { .cpy = {.to = {0,1}, .nBits = 10 }, .src = (U8[]){0xFF, 0x03, [2 ... _TstBufSz-1] = 0x00}, .destFill = 0x00,
      .result = (U8[]){0x03, 0xFF, [2 ... _TstBufSz-1] = 0x00} },
	};

	for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
	{
		S_Tst const *t = &tsts[i];
		memcpy(srcBuf,  t->src, _TstBufSz );
		memset(destBuf, t->destFill, _TstBufSz );

		S_CpySpec const * cpy = &t->cpy;

		bool rtn = bit64K_In(
		&portNoCache,
		bit64K_MakeBE(cpy->to._byte, cpy->to._bit),
		srcBuf,
		cpy->nBits,
		eBigEndian,
		_SrcIsEndian );

		TEST_ASSERT_EQUAL_UINT8_MESSAGE(true, rtn, "All test_Bit64_In_BE_multiSrc_Endian should return true");

		C8 b0[100];
		sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x]",
		i,
		srcBuf[0], srcBuf[1],
		cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1]);

		TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TstBufSz, b0);
	}
}

/* ----------------------------------- test_Bit64_ParmFitsField ----------------------------------------- */
PUBLIC bool bit64K_ParmFitsField(U8 const *parm, U8 parmBytes, bit64K_T_Cnt fieldBits, bool parmHasEndian);

void test_Bit64_ParmFitsField(void)
{
   typedef struct { U8 const *in; U8 inBytes; bit64K_T_Cnt fieldBits; bool isEndian; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      // No input or no destination field; always return true.
      { .in = (U8[1]){0xFF}, .inBytes = 1, .fieldBits = 0, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0xFF}, .inBytes = 0, .fieldBits = 1, .isEndian = false, .rtn = true },

      // Too many source bytes for dest field, even if bits clear (or no)
      { .in = (U8[]){0xFF,0xFF},       .inBytes = 2, .fieldBits = 8,  .isEndian = false, .rtn = false },    // An 8 bit field takes just 1 byte.
      { .in = (U8[]){0x00,0x00},       .inBytes = 2, .fieldBits = 9,  .isEndian = false, .rtn = true },     // A  9bit takes 1 or 2 bytes.
      { .in = (U8[]){0x00,0x00,0x00},  .inBytes = 3, .fieldBits = 16, .isEndian = false, .rtn = false },    // A 16bit field will not accept 3 bytes.
      { .in = (U8[]){0x00,0x00,0x00},  .inBytes = 3, .fieldBits = 17, .isEndian = false, .rtn = true },     // A 17 bit field accepts up to 3 bytes

      // 1-byte.
      { .in = (U8[1]){0xFF}, .inBytes = 1, .fieldBits = 8, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0xFF}, .inBytes = 1, .fieldBits = 7, .isEndian = false, .rtn = false },
      { .in = (U8[1]){0x7F}, .inBytes = 1, .fieldBits = 7, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0x7F}, .inBytes = 1, .fieldBits = 6, .isEndian = false, .rtn = false },
      { .in = (U8[1]){0x3F}, .inBytes = 1, .fieldBits = 6, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0x3F}, .inBytes = 1, .fieldBits = 5, .isEndian = false, .rtn = false },
      { .in = (U8[1]){0x1F}, .inBytes = 1, .fieldBits = 5, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0x1F}, .inBytes = 1, .fieldBits = 4, .isEndian = false, .rtn = false },
      { .in = (U8[1]){0x0F}, .inBytes = 1, .fieldBits = 4, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0x0F}, .inBytes = 1, .fieldBits = 3, .isEndian = false, .rtn = false },
      { .in = (U8[1]){0x07}, .inBytes = 1, .fieldBits = 3, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0x07}, .inBytes = 1, .fieldBits = 2, .isEndian = false, .rtn = false },
      { .in = (U8[1]){0x03}, .inBytes = 1, .fieldBits = 2, .isEndian = false, .rtn = true },
      { .in = (U8[1]){0x03}, .inBytes = 1, .fieldBits = 1, .isEndian = false, .rtn = false },
      { .in = (U8[1]){0x01}, .inBytes = 1, .fieldBits = 1, .isEndian = false, .rtn = true },

      // 2-byte endian. - Picks from msb, down
      { .in = (U8*)(U16[]){0xFFFF},    .inBytes = 2, .fieldBits = 16, .isEndian = true, .rtn = true },
      { .in = (U8*)(U16[]){0xFFFF},    .inBytes = 2, .fieldBits = 15, .isEndian = true, .rtn = false },
      { .in = (U8*)(U16[]){0x7FFF},    .inBytes = 2, .fieldBits = 15, .isEndian = true, .rtn = true },
      { .in = (U8*)(U16[]){0x7FFF},    .inBytes = 2, .fieldBits = 14, .isEndian = true, .rtn = false },
      { .in = (U8*)(U16[]){0x3FFF},    .inBytes = 2, .fieldBits = 14, .isEndian = true, .rtn = true },
      { .in = (U8*)(U16[]){0x3FFF},    .inBytes = 2, .fieldBits = 13, .isEndian = true, .rtn = false },
      { .in = (U8*)(U16[]){0x1FFF},    .inBytes = 2, .fieldBits = 13, .isEndian = true, .rtn = true },
      { .in = (U8*)(U16[]){0x1FFF},    .inBytes = 2, .fieldBits = 12, .isEndian = true, .rtn = false },
      { .in = (U8*)(U16[]){0x0FFF},    .inBytes = 2, .fieldBits = 12, .isEndian = true, .rtn = true },

      { .in = (U8*)(U16[]){0x01FF},    .inBytes = 2, .fieldBits = 9, .isEndian = true, .rtn = true },


      // 2-byte non-endian. - Picks from low address up
      { .in = (U8[]){0xFF,0xFF},    .inBytes = 2, .fieldBits = 16, .isEndian = false, .rtn = true },
      { .in = (U8[]){0xFF,0xFF},    .inBytes = 2, .fieldBits = 15, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x7F,0xFF},    .inBytes = 2, .fieldBits = 15, .isEndian = false, .rtn = true },
      { .in = (U8[]){0x7F,0xFF},    .inBytes = 2, .fieldBits = 14, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x3F,0xFF},    .inBytes = 2, .fieldBits = 14, .isEndian = false, .rtn = true },
      { .in = (U8[]){0x3F,0xFF},    .inBytes = 2, .fieldBits = 13, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x1F,0xFF},    .inBytes = 2, .fieldBits = 13, .isEndian = false, .rtn = true },
      { .in = (U8[]){0x1F,0xFF},    .inBytes = 2, .fieldBits = 12, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x0F,0xFF},    .inBytes = 2, .fieldBits = 12, .isEndian = false, .rtn = true },

      { .in = (U8[]){0x01,0xFF},    .inBytes = 2, .fieldBits = 9, .isEndian = false, .rtn = true },

      // e.g 3-byte non-endian. - Picks from low address up
      { .in = (U8[]){0xFF,0xFF,0xFF},    .inBytes = 3, .fieldBits = 24, .isEndian = false, .rtn = true },
      { .in = (U8[]){0xFF,0xFF,0xFF},    .inBytes = 3, .fieldBits = 23, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x7F,0xFF,0xFF},    .inBytes = 3, .fieldBits = 23, .isEndian = false, .rtn = true },
      { .in = (U8[]){0x7F,0xFF,0xFF},    .inBytes = 3, .fieldBits = 22, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x3F,0xFF,0xFF},    .inBytes = 3, .fieldBits = 22, .isEndian = false, .rtn = true },
      { .in = (U8[]){0x3F,0xFF,0xFF},    .inBytes = 3, .fieldBits = 21, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x1F,0xFF,0xFF},    .inBytes = 3, .fieldBits = 21, .isEndian = false, .rtn = true },
      { .in = (U8[]){0x1F,0xFF,0xFF},    .inBytes = 3, .fieldBits = 20, .isEndian = false, .rtn = false },
      { .in = (U8[]){0x0F,0xFF,0xFF},    .inBytes = 3, .fieldBits = 20, .isEndian = false, .rtn = true },

      { .in = (U8[]){0x01,0xFF,0xFF},    .inBytes = 3, .fieldBits = 17, .isEndian = false, .rtn = true },

      // 4-byte endian. - Picks from msb, down
      { .in = (U8*)(U32[]){0xFFFFFFFF},    .inBytes = 4, .fieldBits = 32, .isEndian = true, .rtn = true },
      { .in = (U8*)(U32[]){0xFFFFFFFF},    .inBytes = 4, .fieldBits = 31, .isEndian = true, .rtn = false },
      { .in = (U8*)(U32[]){0x7FFFFFFF},    .inBytes = 4, .fieldBits = 31, .isEndian = true, .rtn = true },
      { .in = (U8*)(U32[]){0x7FFFFFFF},    .inBytes = 4, .fieldBits = 30, .isEndian = true, .rtn = false },
      { .in = (U8*)(U32[]){0x3FFFFFFF},    .inBytes = 4, .fieldBits = 30, .isEndian = true, .rtn = true },
      { .in = (U8*)(U32[]){0x3FFFFFFF},    .inBytes = 4, .fieldBits = 29, .isEndian = true, .rtn = false },
      { .in = (U8*)(U32[]){0x1FFFFFFF},    .inBytes = 4, .fieldBits = 29, .isEndian = true, .rtn = true },
      { .in = (U8*)(U32[]){0x1FFFFFFF},    .inBytes = 4, .fieldBits = 28, .isEndian = true, .rtn = false },
      { .in = (U8*)(U32[]){0x0FFFFFFF},    .inBytes = 4, .fieldBits = 28, .isEndian = true, .rtn = true },

      { .in = (U8*)(U32[]){0x01FFFFFF},    .inBytes = 4, .fieldBits = 25, .isEndian = true, .rtn = true },

      { .in = (U8*)(U32[]){0x0013},    .inBytes = 2, .fieldBits = 10, .isEndian = false, .rtn = false },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = bit64K_ParmFitsField(t->in, t->inBytes, t->fieldBits, t->isEndian);

      C8 b0[100];
      #define _b1Len 50
      C8 b1[_b1Len];
      sprintf(b0, "tst #%d:  {%s}[%d] %s into {%d} -> %s",
            i,
            PrintU8s_1Line(b1, _b1Len, "0x%X,", t->in, t->inBytes), t->inBytes,
            t->isEndian == true ? "(isEndian)" : "",
            t->fieldBits,
            rtn == true ? "true" : "false");

      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->rtn, rtn, b0);
   }
}


// ----------------------------------------- eof --------------------------------------------
