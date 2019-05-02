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

/* ----------------------------------- test_bit64K_MakeBE ---------------------------------------- */

void test_bit64K_MakeBE(void)
{
   typedef struct {U16 _byte; U8 _bit; U16 res; } S_Tst;

   S_Tst const tsts[] = {
      // Big-endian so bits count in reverse.
      {._byte = 0,      ._bit = 7,     .res = 0  },
      {._byte = 0,      ._bit = 6,     .res = 1  },
      {._byte = 0,      ._bit = 5,     .res = 2  },
      {._byte = 0,      ._bit = 4,     .res = 3  },
      {._byte = 0,      ._bit = 3,     .res = 4  },
      {._byte = 0,      ._bit = 2,     .res = 5  },
      {._byte = 0,      ._bit = 1,     .res = 6  },
      {._byte = 0,      ._bit = 0,     .res = 7  },

      // Count up bits offset from e.g byte 1.  _bit <- 7,8.. are legal
      {._byte = 1,      ._bit = 7,     .res = 8  },
      {._byte = 1,      ._bit = 6,     .res = 9  },
      {._byte = 1,      ._bit = 5,     .res = 0xA  },
      {._byte = 1,      ._bit = 4,     .res = 0xB  },
      {._byte = 1,      ._bit = 3,     .res = 0xC  },
      {._byte = 1,      ._bit = 2,     .res = 0xD  },
      {._byte = 1,      ._bit = 1,     .res = 0xE  },
      {._byte = 1,      ._bit = 0,     .res = 0xF },

      {._byte = 1,      ._bit = 0xF,   .res = 0x10  },
      {._byte = 1,      ._bit = 0xE,   .res = 0x11  },
      {._byte = 1,      ._bit = 0xD,   .res = 0x12  },
      {._byte = 1,      ._bit = 0xC,   .res = 0x13  },
      {._byte = 1,      ._bit = 0xB,   .res = 0x14  },
      {._byte = 1,      ._bit = 0xA,   .res = 0x15  },
      {._byte = 1,      ._bit = 9,     .res = 0x16  },
      {._byte = 1,      ._bit = 8,     .res = 0x17  },

      {._byte = 1,      ._bit = 0x17,  .res = 0x18  },
      {._byte = 1,      ._bit = 0x16,  .res = 0x19  },
      {._byte = 1,      ._bit = 0x15,  .res = 0x1A  },
      {._byte = 1,      ._bit = 0x14,  .res = 0x1B  },
      {._byte = 1,      ._bit = 0x13,  .res = 0x1C  },
      {._byte = 1,      ._bit = 0x12,  .res = 0x1D  },
      {._byte = 1,      ._bit = 0x11,  .res = 0x1E  },
      {._byte = 1,      ._bit = 0x10,  .res = 0x1F  },

      {._byte = 1,      ._bit = 0x18,  .res = 0x27  },

      // Kiss upper limit
      {._byte = 0xFFFF, ._bit = 0,     .res = 0xFFFF  },
      {._byte = 0x1FFF, ._bit = 0,     .res = 0xFFFF  },
      {._byte = 0x1FFE, ._bit = 0,     .res = 0xFFF7  },
      {._byte = 0x1FFE, ._bit = 7,     .res = 0xFFF0  },
      {._byte = 0x1FE0, ._bit = 0xFF,  .res = 0xFFF8  },
      {._byte = 0x1FE0, ._bit = 0xF8,  .res = 0xFFFF  },

      // Clip on overrange
      {._byte = 0x1FE1, ._bit = 0xFF,  .res = 0xFFFF  },
      {._byte = 0x1FFF, ._bit = 0xFF,  .res = 0xFFFF  },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U16 rtn = bit64K_MakeBE(t->_byte, t->_bit);

      C8 b0[500];
      sprintf(b0, "tst #%d:  (0x%02x,0x%02x) -> 0x%02x", i, t->_byte, t->_bit, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}

/* ----------------------------------- test_bit64K_MakeLE ---------------------------------------- */

void test_bit64K_MakeLE(void)
{
   typedef struct {U16 _byte; U8 _bit; U16 res; } S_Tst;

   S_Tst const tsts[] = {
      // Big-endian so bits count in reverse.
      {._byte = 0,      ._bit = 0,     .res = 0  },
      {._byte = 0,      ._bit = 1,     .res = 1  },
      {._byte = 0,      ._bit = 2,     .res = 2  },
      {._byte = 0,      ._bit = 3,     .res = 3  },
      {._byte = 0,      ._bit = 4,     .res = 4  },
      {._byte = 0,      ._bit = 5,     .res = 5  },
      {._byte = 0,      ._bit = 6,     .res = 6  },
      {._byte = 0,      ._bit = 7,     .res = 7  },

      // Count up bits offset from e.g byte 1.  _bit <- 7,8.. are legal
      {._byte = 1,      ._bit = 0,     .res = 8  },
      {._byte = 1,      ._bit = 1,     .res = 9  },
      {._byte = 1,      ._bit = 2,     .res = 0xA  },
      {._byte = 1,      ._bit = 3,     .res = 0xB  },
      {._byte = 1,      ._bit = 4,     .res = 0xC  },
      {._byte = 1,      ._bit = 5,     .res = 0xD  },
      {._byte = 1,      ._bit = 6,     .res = 0xE  },
      {._byte = 1,      ._bit = 7,     .res = 0xF },

      {._byte = 1,      ._bit = 8,     .res = 0x10  },
      {._byte = 1,      ._bit = 9,     .res = 0x11  },
      {._byte = 1,      ._bit = 0xA,   .res = 0x12  },
      {._byte = 1,      ._bit = 0xB,   .res = 0x13  },
      {._byte = 1,      ._bit = 0xC,   .res = 0x14  },
      {._byte = 1,      ._bit = 0xD,   .res = 0x15  },
      {._byte = 1,      ._bit = 0xE,   .res = 0x16  },
      {._byte = 1,      ._bit = 0xF,   .res = 0x17  },

      {._byte = 1,      ._bit = 0x10,  .res = 0x18  },
      {._byte = 1,      ._bit = 0x11,  .res = 0x19  },
      {._byte = 1,      ._bit = 0x12,  .res = 0x1A  },
      {._byte = 1,      ._bit = 0x13,  .res = 0x1B  },
      {._byte = 1,      ._bit = 0x14,  .res = 0x1C  },
      {._byte = 1,      ._bit = 0x15,  .res = 0x1D  },
      {._byte = 1,      ._bit = 0x16,  .res = 0x1E  },
      {._byte = 1,      ._bit = 0x17,  .res = 0x1F  },


      // Kiss upper limit.
      {._byte = 0x1FE0, ._bit = 0xFF,  .res = 0xFFFF  },
      {._byte = 0x1FE0, ._bit = 0xFE,  .res = 0xFFFE  },

      // Clip on overrange
      {._byte = 0x1FFF, ._bit = 0xFF,  .res = 0xFFFF  },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U16 rtn = bit64K_MakeLE(t->_byte, t->_bit);

      C8 b0[500];
      sprintf(b0, "tst #%d:  (0x%02x,0x%02x) -> 0x%02x", i, t->_byte, t->_bit, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}

/* ----------------------------------- test_bit64K_BitBE ---------------------------------------- */

void test_bit64K_BitBE(void)
{
   typedef struct {T_bit64K ba; U8 res; } S_Tst;

   S_Tst const tsts[] = {
      // Big-endian, so bit count is reversed relative to bit-address.
      { .ba = 0,     .res = 7 },
      { .ba = 1,     .res = 6 },
      { .ba = 2,     .res = 5 },
      { .ba = 3,     .res = 4 },
      { .ba = 4,     .res = 3 },
      { .ba = 5,     .res = 2 },
      { .ba = 6,     .res = 1 },
      { .ba = 7,     .res = 0 },

      { .ba = 8,     .res = 7 },
      { .ba = 9,     .res = 6 },

      { .ba = 0xFFFF,   .res = 0 },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U8 rtn = bit64K_BitBE(t->ba);

      C8 b0[500];
      sprintf(b0, "tst #%d:  (0x%02x) -> 0x%02x", i, t->ba, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}

/* ----------------------------------- test_bit64K_BitLE ---------------------------------------- */

void test_bit64K_BitLE(void)
{
   typedef struct {T_bit64K ba; U8 res; } S_Tst;

   S_Tst const tsts[] = {
      // Little-endian, so bit count increases with bit address
      { .ba = 0,     .res = 0 },
      { .ba = 1,     .res = 1 },
      { .ba = 2,     .res = 2 },
      { .ba = 3,     .res = 3 },
      { .ba = 4,     .res = 4 },
      { .ba = 5,     .res = 5 },
      { .ba = 6,     .res = 6 },
      { .ba = 7,     .res = 7 },

      { .ba = 8,     .res = 0 },
      { .ba = 9,     .res = 1 },

      { .ba = 0xFFFF,   .res = 7 },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U8 rtn = bit64K_BitLE(t->ba);

      C8 b0[500];
      sprintf(b0, "tst #%d:  (0x%02x) -> 0x%02x", i, t->ba, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}

/* ----------------------------------- test_bit64K_Byte ---------------------------------------- */

void test_bit64K_Byte(void)
{
   typedef struct {T_bit64K ba; U16 res; } S_Tst;

   S_Tst const tsts[] = {
      // Little-endian, so bit count increases with bit address
      { .ba = 0,     .res = 0 },
      { .ba = 1,     .res = 0 },
      { .ba = 2,     .res = 0 },
      { .ba = 3,     .res = 0 },
      { .ba = 4,     .res = 0 },
      { .ba = 5,     .res = 0 },
      { .ba = 6,     .res = 0 },
      { .ba = 7,     .res = 0 },

      { .ba = 8,     .res = 1 },
      { .ba = 9,     .res = 1 },

      { .ba = 0xFFFF,   .res = 0x1FFF },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U16 rtn = bit64K_Byte(t->ba);

      C8 b0[500];
      sprintf(b0, "tst #%d:  (0x%02x) -> 0x%02x", i, t->ba, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}

/* ----------------------------------- test_bit64K_AddBits ---------------------------------------- */

void test_bit64K_AddBits(void)
{
   typedef struct {T_bit64K ba; S16 bits; T_bit64K res; } S_Tst;

   S_Tst const tsts[] = {
      { .ba = 0,       .bits = 0,     .res = 0 },
      { .ba = 10,      .bits = 23,    .res = 33 },

      // Subtraction too.
      { .ba = 20,      .bits = -8,   .res = 12 },
      { .ba = 10,      .bits = -10,   .res = 0 },

      // Underflow is clipped.
      { .ba = 10,      .bits = -20,   .res = 0 },

      // Overflow is clipped.
      { .ba = 0xFFFE,  .bits = 0x10,   .res = 0xFFFF },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      T_bit64K rtn = bit64K_AddBits(t->ba, t->bits);

      C8 b0[500];
      sprintf(b0, "tst #%d:  0x%02x + 0x%02x bits -> 0x%02x", i, t->ba, t->bits, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}


/* ----------------------------------- test_bit64K_AddBytes ---------------------------------------- */

void test_bit64K_AddBytes(void)
{
   typedef struct {T_bit64K ba; S16 bytes; T_bit64K res; } S_Tst;

   S_Tst const tsts[] = {
      { .ba = 0,       .bytes = 0,     .res = 0  },
      { .ba = 0,       .bytes = 1,     .res = 8  },
      { .ba = 2,       .bytes = 1,     .res = 10 },

      // Subtraction
      { .ba = 0,       .bytes = -1,    .res = 0 },
      { .ba = 8,       .bytes = -1,    .res = 0 },
      { .ba = 16,       .bytes = -1,   .res = 8 },

      // Upper limit.
      { .ba = 0xFFF6,   .bytes = 1,    .res = 0xFFFE },
      { .ba = 0xFFF7,   .bytes = 1,    .res = 0xFFFF },

      // Limit overrange
      { .ba = 0xFFF8,   .bytes = 1,    .res = 0xFFFF },
      { .ba = 0,   .bytes = MAX_S16,    .res = 0xFFFF },
      { .ba = 0,   .bytes = MIN_S16,    .res = 0 },

   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      T_bit64K rtn = bit64K_AddBytes(t->ba, t->bytes);

      C8 b0[500];
      sprintf(b0, "tst #%d:  0x%02x + 0x%02x bytes -> 0x%02x", i, t->ba, t->bytes, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}


/* ----------------------------------- test_bit64K_Add ---------------------------------------- */

void test_bit64K_Add(void)
{
   typedef struct {T_bit64K a, b;T_bit64K res; } S_Tst;

   S_Tst const tsts[] = {
      { .a = 0,       .b = 0,       .res = 0 },
      { .a = 1,       .b = 2,       .res = 3 },

      // Upper limit
      { .a = 0xFFFD,  .b = 1,       .res = 0xFFFE },
      { .a = 1,       .b = 0xFFFD,  .res = 0xFFFE },

      // Overrange is clipped.
      { .a = 0xFFFF,  .b = 0xFFFF,  .res = 0xFFFF },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      T_bit64K rtn = bit64K_Add(t->a, t->b);

      C8 b0[500];
      sprintf(b0, "tst #%d:  0x%02x + 0x%02x -> 0x%02x", i, t->a, t->b, rtn);
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->res, rtn, b0);
   }
}



// ====================================== EOF ==============================================
