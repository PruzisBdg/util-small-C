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

/* ------------------------------------ test_U16toBCD -------------------------------------- */

void test_U16toBCD(void)
{
   typedef struct {U16 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,   .out = 0x00, .rtn = true},
      {.in = 99,  .out = 0x99, .rtn = true},
      {.in = 12,  .out = 0x12, .rtn = true},
      {.in = 34,  .out = 0x34, .rtn = true},

      {.in = 1234,  .out = 0x1234, .rtn = true},

      {.in = 9999,   .out = 0x9999, .rtn = true},
      {.in = 10000,  .out = 0x00, .rtn = false},
      {.in = 0xFFFF, .out = 0x00, .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U16 prefill = t->out + 1;
      U16 out = prefill;

      bool ret = U16toBCD(&out, t->in);

      if(ret != t->rtn) {
         printf("U16toBCD() failed tst #%d  Return: expected %s got %s\r\n",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("U16toBCD() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X\r\n",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("U16toBCD() failed tst #%d  Expected %u -> 0x%04X got 0x%04X\r\n",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}


/* ------------------------------------ test_U32toBCD -------------------------------------- */

void test_U32toBCD(void)
{
   typedef struct {U32 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,   .out = 0x00, .rtn = true},
      {.in = 99,  .out = 0x99, .rtn = true},
      {.in = 12,  .out = 0x12, .rtn = true},
      {.in = 34,  .out = 0x34, .rtn = true},

      {.in = 12345678UL,  .out = 0x12345678UL, .rtn = true},

      {.in = 99999999,  .out = 0x99999999, .rtn = true},
      {.in = 100000000,  .out = 0x00, .rtn = false},
      {.in = 0xFFFFFFFF, .out = 0x00, .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U32 prefill = t->out + 1;
      U32 out = prefill;

      bool ret = U32toBCD(&out, t->in);

      if(ret != t->rtn) {
         printf("U32toBCD() failed tst #%d  Return: expected %s got %s\r\n",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("U32toBCD() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%08lX\r\n",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("U32toBCD() failed tst #%d  Expected %lu -> 0x%08X got 0x%08X\r\n",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_U64toBCD -------------------------------------- */

void test_U64toBCD(void)
{
   typedef struct {U64 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,   .out = 0x00, .rtn = true},
      {.in = 99,  .out = 0x99, .rtn = true},
      {.in = 12,  .out = 0x12, .rtn = true},
      {.in = 34,  .out = 0x34, .rtn = true},

      {.in = 1234567852437192ULL,  .out = 0x1234567852437192ULL, .rtn = true},

      {.in = 9999999999999999ULL,  .out = 0x9999999999999999ULL, .rtn = true},
      {.in = 10000000000000000ULL,  .out = 0x00, .rtn = false},
      {.in = 0xFFFFFFFFFFFFFFFF, .out = 0x00, .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U64 prefill = t->out + 1;
      U64 out = prefill;

      bool ret = U64toBCD(&out, t->in);

      if(ret != t->rtn) {
         printf("U64toBCD() failed tst #%d  Return: expected %s got %s\r\n",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("U64toBCD() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%16llX\r\n",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("U64toBCD() failed tst #%d  Expected %llu -> 0x%16llX got 0x%16llX\r\n",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_BCDtoU16 -------------------------------------------- */

void test_BCDtoU16(void)
{
   typedef struct {U16 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,      .out = 00,     .rtn = true},
      {.in = 0x99,   .out = 99,     .rtn = true},
      {.in = 0x12,   .out = 12,     .rtn = true},
      {.in = 0x34,   .out = 34,     .rtn = true},

      {.in = 0x1234, .out = 1234,   .rtn = true},

      {.in = 0x9999, .out = 9999,   .rtn = true},
      {.in = 0x999A, .out = 0,      .rtn = false},
      {.in = 0x9A99, .out = 0,      .rtn = false},
      {.in = 0xFFFF, .out = 0,      .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U16 prefill = t->out + 1;
      U16 out = prefill;

      bool ret = BCDtoU16(&out, t->in);

      if(ret != t->rtn) {
         printf("BCDtoU16() failed tst #%d  Return: expected %s got %s\r\n",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCDtoU16() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X\r\n",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCDtoU16() failed tst #%d  Expected %u -> 0x%04X got 0x%04X\r\n",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_BCDtoU32 -------------------------------------- */

void test_BCDtoU32(void)
{
   typedef struct {U32 in, out; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      {.in = 0,      .out = 00, .rtn = true},
      {.in = 0x99,   .out = 99, .rtn = true},
      {.in = 0x12,   .out = 12, .rtn = true},
      {.in = 0x34,   .out = 34, .rtn = true},

      {.in = 0x1234, .out = 1234,   .rtn = true},

      {.in = 0x9999, .out = 9999,   .rtn = true},
      {.in = 0x999A, .out = 0,      .rtn = false},
      {.in = 0x9A99, .out = 0,      .rtn = false},
      {.in = 0xFFFF, .out = 0,      .rtn = false},

      {.in = 0x12345678UL,  .out = 12345678UL, .rtn = true},

      {.in = 0x99999999,   .out = 99999999,  .rtn = true},
      {.in = 0x9999999A,   .out = 0x00,      .rtn = false},
      {.in = 0x999999A9,   .out = 0x00,      .rtn = false},
      {.in = 0x99999A99,   .out = 0x00,      .rtn = false},
      {.in = 0x9999A999,   .out = 0x00,      .rtn = false},
      {.in = 0x999A9999,   .out = 0x00,      .rtn = false},
      {.in = 0x99A99999,   .out = 0x00,      .rtn = false},
      {.in = 0x9A999999,   .out = 0x00,      .rtn = false},
      {.in = 0xA9999999,   .out = 0x00,      .rtn = false},
      {.in = 0xFFFFFFFF,   .out = 0x00,      .rtn = false},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      /* Pre-fill out with something other than the expected value. If input is out of range
         then 'out' should be left unchanged.
      */
      U32 prefill = t->out + 1;
      U32 out = prefill;

      bool ret = BCDtoU32(&out, t->in);

      if(ret != t->rtn) {
         printf("BCDtoU32() failed tst #%d  Return: expected %s got %s\r\n",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("BCDtoU32() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%08lX\r\n",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("BCDtoU32() failed tst #%d  Expected %lu -> 0x%08X got 0x%08X\r\n",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

// ----------------------------------------- eof --------------------------------------------
