#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include <string.h>

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

#define _TestBufSize 10
PRIVATE U8 srcBuf[_TestBufSize] = {0};
PRIVATE U8 destBuf[_TestBufSize] = {0};

PRIVATE bool rds(U8 *to, bit64K_T_ByteOfs from, bit64K_T_Cnt cnt )      { memcpy(to, &destBuf[from], cnt); return true; }
PRIVATE bool wrs(bit64K_T_ByteOfs to, U8 const *from, bit64K_T_Cnt cnt) { memcpy(&destBuf[to], from, cnt); return true; }
PRIVATE bool getss(U8 *to, bit64K_T_ByteOfs from, bit64K_T_Cnt cnt)     { memcpy(to, &srcBuf[from], cnt); return true; }

PRIVATE S_Bit64KPorts const port1 = {.rdDest = rds, .wrDest = wrs, .getSrc = getss };

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
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly just the lsbit is copied.
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x01, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0xFE, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TestBufSize-1] = 0xFF} },

      // src[7] -> dest[7].    Test copy of '1' and '0'.
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly just the msbit is copied.
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0x80, .destFill = 0x00, .result = (U8[]){0x80, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,7}, .nBits = 1 }, .srcFill = 0x7F, .destFill = 0xFF, .result = (U8[]){0x7F, [1 ... _TestBufSize-1] = 0xFF} },

      // From src b7 to dest b0.    Test copy of '1' and '0'.
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly just the msbit is copied.
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x80, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,7}, .to = {0,0}, .nBits = 1 }, .srcFill = 0x7F, .destFill = 0xFF, .result = (U8[]){0xFE, [1 ... _TestBufSize-1] = 0xFF} },

      // src[6] -> dest[1]
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x02, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFD, [1 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly src[6] is copied.
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0x40, .destFill = 0x00, .result = (U8[]){0x02, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,6}, .to = {0,1}, .nBits = 1 }, .srcFill = 0xBF, .destFill = 0xFF, .result = (U8[]){0xFD, [1 ... _TestBufSize-1] = 0xFF} },

      // src[5] -> dest[2]
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x04, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xFB, [1 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly src[5] is copied.
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0x20, .destFill = 0x00, .result = (U8[]){0x04, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,5}, .to = {0,2}, .nBits = 1 }, .srcFill = 0xDF, .destFill = 0xFF, .result = (U8[]){0xFB, [1 ... _TestBufSize-1] = 0xFF} },

      // src[1] -> dest[4]
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x10, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0x00, .destFill = 0xFF, .result = (U8[]){0xEF, [1 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly src[1] is copied.
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0x02, .destFill = 0x00, .result = (U8[]){0x10, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,1}, .to = {0,4}, .nBits = 1 }, .srcFill = 0xFD, .destFill = 0xFF, .result = (U8[]){0xEF, [1 ... _TestBufSize-1] = 0xFF} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TestBufSize );
      memset(destBuf, t->destFill, _TestBufSize );

      S_CpySpec const * cpy = &t->cpy;

      bit64K_Copy(
         &port1,
         bit64K_MakeBE(cpy->to._byte,   cpy->to._bit),
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits);

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
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){[0 ... _TestBufSize-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x01,0x80,  [2 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){[0 ... _TestBufSize-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFE,0x7F,  [2 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly src[(0,0):(1,7)] are copied.
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){0x01,0x80, [2 ... _TestBufSize-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x01,0x80,  [2 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 2 }, .src = (U8[]){0xFE,0x7F, [2 ... _TestBufSize-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFE,0x7F,  [2 ... _TestBufSize-1] = 0xFF} },

      // src[(0,0):(1,7)] -> dest[(0,1:0)].
      // Src and dest are not aligned. Read 2 src bytes to fill 1 dest byte.
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){[0 ... _TestBufSize-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x03, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){[0 ... _TestBufSize-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFC, [1 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly src[(0,0):(1,7)] are copied.
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){0x01,0x80, [2 ... _TestBufSize-1] = 0x00}, .destFill = 0x00, .result = (U8[]){0x03, [1 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,0}, .to = {0,1}, .nBits = 2 }, .src = (U8[]){0xFE,0x7F, [2 ... _TestBufSize-1] = 0xFF}, .destFill = 0xFF, .result = (U8[]){0xFC, [1 ... _TestBufSize-1] = 0xFF} },

      // src[(0,3:0)] -> dest[(0,1:0):(1,7:6)].
      // Src and dest are not aligned. Read 1 src byte to fill 2 dest bytes.
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){[0 ... _TestBufSize-1] = 0xFF}, .destFill = 0x00, .result = (U8[]){0x03,0xC0, [2 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){[0 ... _TestBufSize-1] = 0x00}, .destFill = 0xFF, .result = (U8[]){0xFC,0x3F, [2 ... _TestBufSize-1] = 0xFF} },
      // Make sure exactly src[(0,3:0)] are copied.
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){0x0F,[1 ... _TestBufSize-1] = 0x00}, .destFill = 0x00, .result = (U8[]){0x03,0xC0, [2 ... _TestBufSize-1] = 0} },
      { .cpy = {.from = {0,3}, .to = {0,1}, .nBits = 4 }, .src = (U8[]){0xF0,[1 ... _TestBufSize-1] = 0xFF}, .destFill = 0xFF, .result = (U8[]){0xFC,0x3F, [2 ... _TestBufSize-1] = 0xFF} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memcpy(srcBuf,  t->src, _TestBufSize );
      memset(destBuf, t->destFill, _TestBufSize );

      S_CpySpec const * cpy = &t->cpy;

      bit64K_Copy(
         &port1,
         bit64K_MakeBE(cpy->to._byte,   cpy->to._bit),
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits);

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1], cpy->from._byte ,cpy->from._bit, cpy->nBits,
            cpy->to._byte ,cpy->to._bit, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, _TestBufSize, b0);
   }
}


/* ------------------------------- test_Bit64_Out_LE --------------------------------------------------- */

void test_Bit64_Out_LE(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      { .cpy = {.from = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TestBufSize );
      memset(destBuf, t->destFill, _TestBufSize );

      S_CpySpec const * cpy = &t->cpy;

      bit64K_Out(
         &port1,
         destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eLittleEndian);

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {(%d,%d){%d} -> dest[0]}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1], cpy->from._byte ,cpy->from._bit, cpy->nBits,
            destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}

/* ------------------------------- test_Bit64_Out_BE --------------------------------------------------- */

void test_Bit64_Out_BE(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      { .cpy = {.from = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TestBufSize );
      memset(destBuf, t->destFill, _TestBufSize );

      S_CpySpec const * cpy = &t->cpy;

      bit64K_Out(
         &port1,
         destBuf,
         bit64K_MakeBE(cpy->from._byte, cpy->from._bit),
         cpy->nBits,
         eBigEndian);

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {(%d,%d){%d} -> dest[0]}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1], cpy->from._byte ,cpy->from._bit, cpy->nBits,
            destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}


/* ------------------------------- test_Bit64_In_LE --------------------------------------------------- */

void test_Bit64_In_LE(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TestBufSize );
      memset(destBuf, t->destFill, _TestBufSize );

      S_CpySpec const * cpy = &t->cpy;

      bit64K_In(
         &port1,
         bit64K_MakeBE(cpy->to._byte, cpy->to._bit),
         srcBuf,
         cpy->nBits,
         eLittleEndian);

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1],
            cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}

/* ------------------------------- test_Bit64_In_BE --------------------------------------------------- */

void test_Bit64_In_BE(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      { .cpy = {.to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = (U8[]){0x01, [1 ... _TestBufSize-1] = 0} },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TestBufSize );
      memset(destBuf, t->destFill, _TestBufSize );

      S_CpySpec const * cpy = &t->cpy;

      bit64K_In(
         &port1,
         bit64K_MakeBE(cpy->to._byte, cpy->to._bit),
         srcBuf,
         cpy->nBits,
         eBigEndian);

      C8 b0[100];
      sprintf(b0, "tst #%d:  src[0x%x 0x%x] map {src[0] -> (%d,%d){%d}}.  dest[0x%x 0x%x]",
            i,
            srcBuf[0], srcBuf[1],
            cpy->to._byte ,cpy->to._bit, cpy->nBits, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(t->result, destBuf, 3, b0);
   }
}


// ----------------------------------------- eof --------------------------------------------
