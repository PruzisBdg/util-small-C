#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* ------------------------------- test_IdxTo1stBitSet_U16 --------------------------------------------------- */

void test_IdxTo1stBitSet_U16 (void)
{
   typedef struct { U16 in; U8 posn; } S_Tst;

   S_Tst const tsts[] = {
      { .in = 0x0,      .posn = 0xFF },   // No bits set -> 0xFF.
      { .in = 0xFFFF,   .posn = 0x00 },
      { .in = 0xFFFE,   .posn = 0x01 },
      { .in = 0xFFFC,   .posn = 0x02 },
      { .in = 0xFFF8,   .posn = 0x03 },
      { .in = 0xFFF0,   .posn = 0x04 },
      { .in = 0xFFE0,   .posn = 0x05 },
      { .in = 0xFFC0,   .posn = 0x06 },
      { .in = 0xFF80,   .posn = 0x07 },
      { .in = 0xFF00,   .posn = 0x08 },
      { .in = 0xFE00,   .posn = 0x09 },
      { .in = 0xFC00,   .posn = 0x0A },
      { .in = 0xF800,   .posn = 0x0B },
      { .in = 0xF000,   .posn = 0x0C },
      { .in = 0xE000,   .posn = 0x0D },
      { .in = 0xC000,   .posn = 0x0E },
      { .in = 0x8000,   .posn = 0x0F },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U8 rtn = IdxTo1stBitSet_U16(t->in);

      TEST_ASSERT_EQUAL_UINT8(t->posn, rtn);
   }
}

/* ------------------------------- test_IdxTo1stBitSet_U32 --------------------------------------------------- */

void test_IdxTo1stBitSet_U32 (void)
{
   typedef struct { U32 in; U8 posn; } S_Tst;

   S_Tst const tsts[] = {
      { .in = 0x0,          .posn = 0xFF },   // No bits set -> 0xFF.
      { .in = 0xFFFFFFFF,   .posn = 0x00 },
      { .in = 0xFFFFFFFE,   .posn = 0x01 },
      { .in = 0xFFFFFFFC,   .posn = 0x02 },
      { .in = 0xFFFFFFF8,   .posn = 0x03 },
      { .in = 0xFFFFFFF0,   .posn = 0x04 },
      { .in = 0xFFFFFFE0,   .posn = 0x05 },
      { .in = 0xFFFFFFC0,   .posn = 0x06 },
      { .in = 0xFFFFFF80,   .posn = 0x07 },
      { .in = 0xFFFFFF00,   .posn = 0x08 },
      { .in = 0xFFFFFE00,   .posn = 0x09 },
      { .in = 0xFFFFFC00,   .posn = 0x0A },
      { .in = 0xFFFFF800,   .posn = 0x0B },
      { .in = 0xFFFFF000,   .posn = 0x0C },
      { .in = 0xFFFFE000,   .posn = 0x0D },
      { .in = 0xFFFFC000,   .posn = 0x0E },
      { .in = 0xFFFF8000,   .posn = 0x0F },
      { .in = 0xFFFF0000,   .posn = 0x10 },
      { .in = 0xFFFE0000,   .posn = 0x11 },
      { .in = 0xFFFC0000,   .posn = 0x12 },
      { .in = 0xFFF80000,   .posn = 0x13 },
      { .in = 0xFFF00000,   .posn = 0x14 },
      { .in = 0xFFE00000,   .posn = 0x15 },
      { .in = 0xFFC00000,   .posn = 0x16 },
      { .in = 0xFF800000,   .posn = 0x17 },
      { .in = 0xFF000000,   .posn = 0x18 },
      { .in = 0xFE000000,   .posn = 0x19 },
      { .in = 0xFC000000,   .posn = 0x1A },
      { .in = 0xF8000000,   .posn = 0x1B },
      { .in = 0xF0000000,   .posn = 0x1C },
      { .in = 0xE0000000,   .posn = 0x1D },
      { .in = 0xC0000000,   .posn = 0x1E },
      { .in = 0x80000000,   .posn = 0x1F },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U8 rtn = IdxTo1stBitSet_U32(t->in);

      TEST_ASSERT_EQUAL_UINT8(t->posn, rtn);
   }
}

/* ------------------------------- test_NumBitsSet_U16 --------------------------------------------------- */

void test_NumBitsSet_U16 (void)
{
   typedef struct { U16 in; U8 cnt; } S_Tst;

   S_Tst const tsts[] = {
      { .in = 0x0000,   .cnt = 0x00 },    // None set
      { .in = 0xFFFF,   .cnt = 0x10 },    // All set
      { .in = 0x000F,   .cnt = 0x04 },    // Try some others.
      { .in = 0x00F0,   .cnt = 0x04 },
      { .in = 0x0F00,   .cnt = 0x04 },
      { .in = 0xF00F,   .cnt = 0x08 },
      { .in = 0xFEF7,   .cnt = 0x0E },
      { .in = 0x3008,   .cnt = 0x03 },
      { .in = 0xF10F,   .cnt = 0x09 },
      { .in = 0xFF0F,   .cnt = 0x0C },
      { .in = 0xFEAF,   .cnt = 0x0D },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      U8 rtn = NumBitsSet_U16(t->in);
      TEST_ASSERT_EQUAL_UINT8(t->cnt, rtn);
   }
}

/* ------------------------------- test_NumBitsSet_U32 --------------------------------------------------- */

void test_NumBitsSet_U32 (void)
{
   typedef struct { U32 in; U8 cnt; } S_Tst;

   S_Tst const tsts[] = {
      { .in = 0x00000000,     .cnt = 0x00 },    // None set
      { .in = 0xFFFFFFFF,     .cnt = 0x20 },    // All set
      { .in = 0x000F0000,     .cnt = 0x04 },    // Try some others.
      { .in = 0x00F00090,     .cnt = 0x06 },
      { .in = 0x0F000000,     .cnt = 0x04 },
      { .in = 0xF00F0107,     .cnt = 0x0C },
      { .in = 0x00FEF701,     .cnt = 0x0F },
      { .in = 0x30080020,     .cnt = 0x04 },
      { .in = 0x0F100F03,     .cnt = 0x0B },
      { .in = 0xFF0F00A5,     .cnt = 0x10 },
      { .in = 0xFEAFA0A0,     .cnt = 0x11 },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      U8 rtn = NumBitsSet_U32(t->in);
      TEST_ASSERT_EQUAL_UINT8(t->cnt, rtn);
   }
}



// ----------------------------------------- eof --------------------------------------------
