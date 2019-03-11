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

PRIVATE bool rds(U8 *to, U8 from, U8 cnt )      { memcpy(to, &destBuf[from], cnt); return true; }
PRIVATE bool wrs(U8 to, U8 const *from, U8 cnt) { memcpy(&destBuf[to], from, cnt); return true; }
PRIVATE bool getss(U8 *to, U8 from, U8 cnt)     { memcpy(to, &srcBuf[from], cnt); return true; }

PRIVATE S_Bit64KPort const port1 = {.rdDest = rds, .wrDest = wrs, .getSrc = getss };

typedef struct { U8 _byte, _bit; } S_BitAddr;
typedef struct { S_BitAddr from, to; U8 nBits; } S_CpySpec;

/* ------------------------------- test_Bit64_Copy --------------------------------------------------- */

void test_Bit64_Copy(void)
{
   typedef struct { S_CpySpec cpy; U8 srcFill, destFill; U8 const *result; } S_Tst;

   S_Tst const tsts[] = {
      { .cpy = {.from = {0,0}, .to = {0,0}, .nBits = 1 }, .srcFill = 0xFF, .destFill = 0x00, .result = "\x01\x00" },
   };

   for(U8 i = 0; i <  RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      memset(srcBuf,  t->srcFill, _TestBufSize );
      memset(destBuf, t->destFill, _TestBufSize );

      S_CpySpec const * cpy = &t->cpy;

      Bit64K_Copy(
         &port1,
         Bit64K_MakeBE(cpy->to._byte ,cpy->to._bit),
         Bit64K_MakeBE(cpy->from._byte ,cpy->from._bit),
         cpy->nBits);

      C8 b0[100];
      sprintf(b0, "src[0x%x 0x%x] map {(%d,%d){%d} -> (%d,%d)} -> dest[0x%x 0x%x]",
             srcBuf[0], srcBuf[1], cpy->from._byte ,cpy->from._bit, cpy->nBits,
             cpy->to._byte ,cpy->to._bit, destBuf[0], destBuf[1]);

      TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(destBuf, tsts->result, 2, b0);
   }
}


// ----------------------------------------- eof --------------------------------------------
