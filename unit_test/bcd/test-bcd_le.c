#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

// =============================== Tests start here ==================================

/* Harnesses below assume little-endian; they are for X86 so should be true (for now).

*/
#ifdef __BYTE_ORDER__
   #if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
      #error "__BYTE_ORDER__ must be __ORDER_LITTLE_ENDIAN__."
   #endif
#else
   #error "__BYTE_ORDER__ must be defined for this harness."
#endif


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

// --------------------------------------------------------------------------------------------
static void failwMsg(U16 tstNum, C8 const *msg) {
   printf("tst #%d\r\n   %s\r\n", tstNum, msg);
   TEST_FAIL(); }


// =================================== From Integer to BCD ====================================


/* ------------------------------------ test_BCD2_LE -------------------------------------- */

void test_BCD2_LE(void)
{
   typedef struct {U8 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,   .out = 0x00, .rtn = true},
      {.in = 99,  .out = 0x99, .rtn = true},
      {.in = 12,  .out = 0x12, .rtn = true},
      {.in = 34,  .out = 0x34, .rtn = true},

      {.in = 100,  .out = 0x00, .rtn = false},
      {.in = 0xFF, .out = 0x00, .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U8 prefill = t->out + 1;
      U8 out = prefill;

      bool ret = BCD2_LE(&out, t->in);

      if(ret != t->rtn) {
         printf("BCD2_LE() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCD2_LE() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCD2_LE() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_BCD4_LE -------------------------------------- */

void test_BCD4_LE(void)
{
   typedef struct {U16 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,         .out = 0x0000,    .rtn = true},
      {.in = 9999,      .out = 0x9999,    .rtn = true},
      {.in = 1234,      .out = 0x1234,    .rtn = true},
      {.in = 6789,      .out = 0x6789,    .rtn = true},

      {.in = 10000,     .out = 0x0000,    .rtn = false},
      {.in = 0xFFFF,    .out = 0x0000,    .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U16 prefill= (t->out + 1) << 1;
      U16 out = prefill;

      bool ret = BCD4_LE( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("BCD4_LE() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCD4_LE() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCD4_LE() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_BCD6_LE -------------------------------------- */

void test_BCD6_LE(void)
{
   typedef struct {U32 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,         .out = 0x00000000,   .rtn = true},
      {.in = 999999,    .out = 0x999999,     .rtn = true},
      {.in = 123456,    .out = 0x123456,     .rtn = true},
      {.in = 890123,    .out = 0x890123,     .rtn = true},

      {.in = 1000000,   .out = 0x0000,       .rtn = false},
      {.in = 0xFFFFFF,  .out = 0x0000,       .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U32 prefill= ((t->out + 1) << 1) & 0xFFFFFF;    // Output is 24bit, so must zero top byte of U32.
      U32 out = prefill;

      bool ret = BCD6_LE( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("BCD6_LE() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCD6_LE() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCD6_LE() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}



/* ------------------------------------ test_BCD8_LE -------------------------------------- */

void test_BCD8_LE(void)
{
   typedef struct {U32 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,         .out = 0x00000000,   .rtn = true},
      {.in = 99999999,  .out = 0x99999999,   .rtn = true},
      {.in = 12345678,  .out = 0x12345678,   .rtn = true},
      {.in = 67890123,  .out = 0x67890123,   .rtn = true},

      {.in = 100000000, .out = 0x0000,       .rtn = false},
      {.in = 0xFFFFFFFF,.out = 0x0000,       .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U32 prefill= (t->out + 1) << 1;
      U32 out = prefill;

      bool ret = BCD8_LE( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("BCD8_LE() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCD8_LE() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCD8_LE() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_BCD12_LE -------------------------------------- */

void test_BCD12_LE(void)
{
   typedef struct {U64 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,                  .out = 0x0000000000000000,   .rtn = true},
      {.in = 999999999999,       .out = 0x0000999999999999,   .rtn = true},
      {.in = 123456789012,       .out = 0x0000123456789012,   .rtn = true},
      {.in = 678901234567,       .out = 0x0000678901234567,   .rtn = true},

      {.in = 1000000000000,      .out = 0x0000,                .rtn = false},
      {.in = 0xFFFFFFFFFFFFFFFF, .out = 0x0000,                .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U64 prefill= ((t->out+1) << 1) & 0xFFFFFFFFFFFF;   // Output is 48bit so must zero top 16bits of U64.
      U64 out = prefill;

      bool ret = BCD12_LE( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("BCD12_LE() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCD12_LE() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCD12_LE() failed tst #%d  Expected %lu -> 0x%02lX got 0x%02lX",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_bcdLE_U64 -------------------------------------- */

void test_bcdLE_U64(void)
{
   typedef struct {U64 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,                  .out = 0x0000000000000000,   .rtn = true},
      {.in = 9999999999999999,   .out = 0x9999999999999999,   .rtn = true},
      {.in = 1234567890124680,   .out = 0x1234567890124680,   .rtn = true},
      {.in = 6789012345676321,   .out = 0x6789012345676321,   .rtn = true},

      {.in = 10000000000000000,  .out = 0x0000,               .rtn = false},
      {.in = 0xFFFFFFFFFFFFFFFF, .out = 0x0000,               .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U64 prefill= (t->out+1) << 1;
      U64 out = prefill;

      bool ret = BCD16_LE( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("BCD16_LE() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCD16_LE() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCD16_LE() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}


// ================================== From BCD to Integer ===========================================

/* -------------------------------------- test_BCDtoU8 ------------------------------------------ */

void test_BCDtoU8(void)
{
   typedef struct { U8 src; bool rtn; U8 res; } S_Tst;

   S_Tst const tsts[] = {
      { .src = 0x00, .rtn = true, .res = 0  },
      { .src = 0x99, .rtn = true, .res = 99  },
      { .src = 0x15, .rtn = true, .res = 15  },

      // Fails.
      { .src = 0xA2, .rtn = false, .res = 0x5A  },
      { .src = 0x3E, .rtn = false, .res = 0x5A  },
   };

   C8 b0[200];

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U8 out = 0x5A;
      bool rtn = BCDtoU8(&out, t->src);

      if(t->rtn == false) {
         if(rtn == true) {
            sprintf(b0, "Expected false; got true");
            failwMsg(i, b0); }
         else if(out != 0x5A) {
            sprintf(b0, "Returned false but 'out' was modified to %d", out);
            failwMsg(i, b0); }
      }
      else {
         if(rtn == false) {
            sprintf(b0, "Expected true; got false");
            failwMsg(i, b0); }
         else if(out != t->res) {
            sprintf(b0, "Wrong output: expected %d, got %d", t->res, out);
            failwMsg(i, b0); }
      }
   }
}

/* -------------------------------------- test_BCD4le_toU16 ------------------------------------------ */

void test_BCD4le_toU16(void)
{
   typedef struct { U8 const *src; bool rtn; U16 res; } S_Tst;

   S_Tst const tsts[] = {
      { .src = (U8[]){0x00, 0x00}, .rtn = true, .res = 0  },
      { .src = (U8[]){0x34, 0x12}, .rtn = true, .res = 1234 },
      { .src = (U8[]){0x99, 0x99}, .rtn = true, .res = 9999 },

      // Fails.
      { .src = (U8[]){0xF4, 0x12}, .rtn = false, .res = 0x5A5A  },
      { .src = (U8[]){0x3E, 0x12}, .rtn = false, .res = 0x5A5A  },
      { .src = (U8[]){0x34, 0xA2}, .rtn = false, .res = 0x5A5A  },
      { .src = (U8[]){0x34, 0x1C}, .rtn = false, .res = 0x5A5A  },
   };

   C8 b0[200];

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U16 out = 0x5A5A;
      bool rtn = BCD4le_toU16(&out, t->src);

      if(t->rtn == false) {
         if(rtn == true) {
            sprintf(b0, "Expected false; got true");
            failwMsg(i, b0); }
         else if(out != 0x5A5A) {
            sprintf(b0, "Returned false but 'out' was modified to %u/0x%04x", out, out);
            failwMsg(i, b0); }
      }
      else {
         if(rtn == false) {
            sprintf(b0, "Expected true; got false");
            failwMsg(i, b0); }
         else if(out != t->res) {
            sprintf(b0, "Wrong output: expected %u, got %u", t->res, out);
            failwMsg(i, b0); }
      }
   }
}

/* -------------------------------------- test_BCD6le_toU32 ------------------------------------------ */

void test_BCD6le_toU32(void)
{
   typedef struct { U8 const *src; bool rtn; U32 res; } S_Tst;

   S_Tst const tsts[] = {
      { .src = (U8[]){0x00, 0x00, 0x00}, .rtn = true, .res = 0  },
      { .src = (U8[]){0x56, 0x34, 0x12}, .rtn = true, .res = 123456 },
      { .src = (U8[]){0x99, 0x99, 0x99}, .rtn = true, .res = 999999 },

      // Fails.
      { .src = (U8[]){0xC6, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x5D, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x56, 0xE4, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x56, 0x3F, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x56, 0x34, 0xA2}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x56, 0x34, 0x1B}, .rtn = false, .res = 0x5A5A5A5A  },
   };

   C8 b0[200];

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U32 out = 0x5A5A5A5A;
      bool rtn = BCD6le_toU32(&out, t->src);

      if(t->rtn == false) {
         if(rtn == true) {
            sprintf(b0, "Expected false; got true");
            failwMsg(i, b0); }
         else if(out != 0x5A5A5A5A) {
            sprintf(b0, "Returned false but 'out' was modified to %u/0x%04x", out, out);
            failwMsg(i, b0); }
      }
      else {
         if(rtn == false) {
            sprintf(b0, "Expected true; got false");
            failwMsg(i, b0); }
         else if(out != t->res) {
            sprintf(b0, "Wrong output: expected %u, got %u", t->res, out);
            failwMsg(i, b0); }
      }
   }
}


/* -------------------------------------- test_BCD8le_toU32 ------------------------------------------ */

void test_BCD8le_toU32(void)
{
   typedef struct { U8 const *src; bool rtn; U32 res; } S_Tst;

   S_Tst const tsts[] = {
      { .src = (U8[]){0x00, 0x00, 0x00, 0x00}, .rtn = true, .res = 0  },
      { .src = (U8[]){0x78, 0x56, 0x34, 0x12}, .rtn = true, .res = 12345678 },
      { .src = (U8[]){0x99, 0x99, 0x99, 0x99}, .rtn = true, .res = 99999999 },

      // Fails.
      { .src = (U8[]){0xA8, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x7B, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x78, 0xC6, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x78, 0x5D, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x78, 0x56, 0xE4, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x78, 0x56, 0x3F, 0x12}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x78, 0x56, 0x34, 0xA2}, .rtn = false, .res = 0x5A5A5A5A  },
      { .src = (U8[]){0x78, 0x56, 0x34, 0x1B}, .rtn = false, .res = 0x5A5A5A5A  },
   };

   C8 b0[200];

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U32 out = 0x5A5A5A5A;
      bool rtn = BCD8le_toU32(&out, t->src);

      if(t->rtn == false) {
         if(rtn == true) {
            sprintf(b0, "Expected false; got true");
            failwMsg(i, b0); }
         else if(out != 0x5A5A5A5A) {
            sprintf(b0, "Returned false but 'out' was modified to %lu/0x%04x", out, out);
            failwMsg(i, b0); }
      }
      else {
         if(rtn == false) {
            sprintf(b0, "Expected true; got false");
            failwMsg(i, b0); }
         else if(out != t->res) {
            sprintf(b0, "Wrong output: expected %lu, got %lu", t->res, out);
            failwMsg(i, b0); }
      }
   }
}

/* -------------------------------------- test_BCD12le_toU64 ------------------------------------------ */

void test_BCD12le_toU64(void)
{
   typedef struct { U8 const *src; bool rtn; U64 res; } S_Tst;

   S_Tst const tsts[] = {
      { .src = (U8[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, .rtn = true, .res = 0  },
      { .src = (U8[]){0x91, 0x50, 0x78, 0x56, 0x34, 0x12}, .rtn = true, .res = 123456785091ULL },
      { .src = (U8[]){0x99, 0x99, 0x99, 0x99, 0x99, 0x99}, .rtn = true, .res = 999999999999ULL },

      // Fails.
      { .src = (U8[]){0xA1, 0x50, 0x78, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x9B, 0x50, 0x78, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0xC0, 0x78, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x5D, 0x78, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0xE8, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0x7F, 0x56, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0x78, 0xA6, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0x78, 0x5B, 0x34, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0x78, 0x56, 0xC4, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0x78, 0x56, 0x3D, 0x12}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0x78, 0x56, 0x34, 0xE2}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
      { .src = (U8[]){0x91, 0x50, 0x78, 0x56, 0x34, 0x1F}, .rtn = false, .res = 0x5A5A5A5A5A5A5A5AULL },
   };

   C8 b0[200];

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U64 out = 0x5A5A5A5A5A5A5A5AULL;
      bool rtn = BCD12le_toU64(&out, t->src);

      if(t->rtn == false) {
         if(rtn == true) {
            sprintf(b0, "Expected false; got true");
            failwMsg(i, b0); }
         else if(out != 0x5A5A5A5A5A5A5A5AULL) {
            sprintf(b0, "Returned false but 'out' was modified to %llu/0x%04x", out, out);
            failwMsg(i, b0); }
      }
      else {
         if(rtn == false) {
            sprintf(b0, "Expected true; got false");
            failwMsg(i, b0); }
         else if(out != t->res) {
            sprintf(b0, "Wrong output: expected %llu, got %llu", t->res, out);
            failwMsg(i, b0); }
      }
   }
}

/* -------------------------------------- test_BCDle_toU64 ------------------------------------------ */

void test_BCDle_toU64(void)
{
   typedef struct { U8 const *src; U8 numBytes; bool rtn; U64 res; } S_Tst;

   S_Tst const tsts[] = {
      { .src = (U8[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  .numBytes = 0, .rtn = true, .res = 0  },
      { .src = (U8[]){0x12},                                .numBytes = 1, .rtn = true, .res = 12 },
      { .src = (U8[]){0x34, 0x12},                          .numBytes = 2, .rtn = true, .res = 1234 },
      { .src = (U8[]){0x56, 0x34, 0x12},                    .numBytes = 3, .rtn = true, .res = 123456UL },
      { .src = (U8[]){0x78, 0x56, 0x34, 0x12},              .numBytes = 4, .rtn = true, .res = 12345678UL },
      { .src = (U8[]){0x43, 0x78, 0x56, 0x34, 0x12},        .numBytes = 5, .rtn = true, .res = 1234567843UL },
      { .src = (U8[]){0x91, 0x50, 0x78, 0x56, 0x34, 0x12},  .numBytes = 6, .rtn = true, .res = 123456785091ULL },
      { .src = (U8[]){0x28, 0x91, 0x50, 0x78, 0x56, 0x34, 0x12},  .numBytes = 7, .rtn = true, .res = 12345678509128ULL },
      { .src = (U8[]){0x72, 0x28, 0x91, 0x50, 0x78, 0x56, 0x34, 0x12},  .numBytes = 8, .rtn = true, .res = 1234567850912872ULL },
   };

   C8 b0[200];

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      U64 out = 0x5A5A5A5A5A5A5A5AULL;
      bool rtn = BCDle_toU64(&out, t->src, t->numBytes);

      if(t->rtn == false) {
         if(rtn == true) {
            sprintf(b0, "Expected false; got true");
            failwMsg(i, b0); }
         else if(out != 0x5A5A5A5A5A5A5A5AULL) {
            sprintf(b0, "Returned false but 'out' was modified to %llu/0x%04x", out, out);
            failwMsg(i, b0); }
      }
      else {
         if(rtn == false) {
            sprintf(b0, "Expected true; got false");
            failwMsg(i, b0); }
         else if(out != t->res) {
            sprintf(b0, "Wrong output: expected %llu, got %llu", t->res, out);
            failwMsg(i, b0); }
      }
   }

}

// ----------------------------------------- eof --------------------------------------------
