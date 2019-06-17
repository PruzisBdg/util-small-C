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


/* ------------------------------------ test_bcdLE_U8 -------------------------------------- */

void test_bcdLE_U8(void)
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

      bool ret = bcdLE_U8(&out, t->in);

      if(ret != t->rtn) {
         printf("bcdLE_U8() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("bcdLE_U8() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("bcdLE_U8() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_bcdLE_U16 -------------------------------------- */

void test_bcdLE_U16(void)
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

      bool ret = bcdLE_U16( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("bcdLE_U16() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("bcdLE_U16() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("bcdLE_U16() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_bcdLE_U32 -------------------------------------- */

void test_bcdLE_U32(void)
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

      bool ret = bcdLE_U32( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("bcdLE_U32() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("bcdLE_U32() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("bcdLE_U32() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

/* ------------------------------------ test_bcdLE_U48 -------------------------------------- */

void test_bcdLE_U48(void)
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
      U64 prefill= ((t->out+1) << 1) & 0xFFFFFFFFFFFF;
      U64 out = prefill;

      bool ret = bcdLE_U48( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("bcdLE_U48() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("bcdLE_U48() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("bcdLE_U48() failed tst #%d  Expected %lu -> 0x%02lX got 0x%02lX",
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

      bool ret = bcdLE_U64( (U8*)&out, t->in);

      if(ret != t->rtn) {
         printf("bcdLE_U64() failed tst #%d  Return: expected %s got %s",
            i, t->rtn == true ? "true" : "false", ret == true ? "true" : "false");
         TEST_ASSERT_TRUE(FALSE); }

      if(out != t->out) {
         if(t->rtn == false) {
            if(out != prefill) {
               printf("bcdLE_U64() failed tst #%d  Returned 'false' as expected, but 'out' was overwritten to 0x%02X",
                  i, out);
               TEST_ASSERT_TRUE(FALSE); }}
         else {
            printf("bcdLE_U64() failed tst #%d  Expected %u -> 0x%02X got 0x%02X",
               i, t->in, t->out, out);
            TEST_ASSERT_TRUE(FALSE); }}
   }
}

// ----------------------------------------- eof --------------------------------------------
