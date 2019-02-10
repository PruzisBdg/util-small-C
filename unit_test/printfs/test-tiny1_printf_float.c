#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include "tiny1_stdio.h"
#include <string.h>
#include "tiny1_printf_test_support.h"

PRIVATE void chkStr(C8 const *str, C8 const *out, T_PrintCnt cnt)
{
   OStream_Reset();
   T_PrintCnt rtn = tiny1_printf(str);
   TEST_ASSERT_EQUAL_STRING(out, OStream_Get());
   TEST_ASSERT_EQUAL_INT(cnt, rtn);
   OStream_Print();
}

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}


/* --------------------------------- test_JustStrings -------------------------------------- */

void test_JustStrings(void) {
   typedef struct { C8 const *str; C8 const *out; T_PrintCnt cnt; } S_Tst;

   S_Tst const tsts[] = {
      { .str = "", .out = "", .cnt = 0 },                                                // Empty
      { .str = "My 1st string\r\n", .out = "My 1st string\r\n",   .cnt = sizeof("My 1st string\r\n")-1 },   // Return is number of chars written

      // This printf condenses escape sequences.
      { .str = "\\\\a\r\n",         .out = "\\a\r\n",             .cnt = sizeof("\\a\r\n")-1 },
      { .str = "\\\\tZ\r\n",        .out = "\\tZ\r\n",             .cnt = sizeof("\\tZ\r\n")-1 },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++) {
      S_Tst const *t = &tsts[i];
      chkStr(t->str, t->out, t->cnt);
   }
}

/* --------------------------------- test_Decimal16_Signed -------------------------------------- */

void test_Decimal16_Signed(void)
{
   typedef struct { C8 const *fmt; C8 const *out; S16 n; } S_Tst;

   S_Tst const tsts[] = {
      // Basic zero and range
      { .fmt = "%d",    .n = 0,          .out = "0" },
      { .fmt = "%d",    .n = MAX_S16,    .out = "32767" },
      { .fmt = "%d",    .n = MIN_S16,    .out = "-32768" },
      { .fmt = "%d",    .n = MAX_S16-1,  .out = "32766" },
      { .fmt = "%d",    .n = MIN_S16+1,  .out = "-32767" },

      // Zeros with width
      { .fmt = "abc%1d",   .n = 0,        .out = "abc0" },
      { .fmt = "abc%01d",  .n = 0,        .out = "abc0" },

      { .fmt = "abc%3d",   .n = 0,        .out = "abc  0" },
      { .fmt = "abc%03d",  .n = 0,        .out = "abc000" },

      { .fmt = "abc%5d",   .n = 0,        .out = "abc    0" },             // For an int, print up to 5 leading zeros.
      { .fmt = "abc%05d",  .n = 0,        .out = "abc00000" },

      { .fmt = "abc%12d",   .n = 0,        .out = "abc           0" },     // Specifying more than 5 leading zeros results in spaces.
      { .fmt = "abc%012d",  .n = 0,        .out = "abc       00000" },

      // Numbers
      { .fmt = "...%d",    .n = 1,        .out = "...1" },
      { .fmt = "...%+d",   .n = 1,        .out = "...+1" },
      { .fmt = "...%d",    .n = 345,      .out = "...345" },
      { .fmt = "...%4d",   .n = 73,       .out = "...  73" },
      { .fmt = "...%+4d",  .n = 73,       .out = "... +73" },
      { .fmt = "...%+4d",  .n = -73,      .out = "... -73" },

      { .fmt = "...%+04d", .n = 81,      .out = "...+081" },
      { .fmt = "...%+04d", .n = -81,     .out = "...-081" },
      { .fmt = "...%04d",  .n = -81,     .out = "...-081" },

      // All these 5-digit corners should print the same
      { .fmt = "%+d",      .n = 12345,    .out = "+12345" },
      { .fmt = "%+5d",     .n = 12345,    .out = "+12345" },
      { .fmt = "%+05d",    .n = 12345,    .out = "+12345" },

      // Just-under and just over field/significant digits.
      { .fmt = "%+4d",     .n = 12345,    .out = "+12345" },
      { .fmt = "%+04d",    .n = 12345,    .out = "+12345" },

      { .fmt = "%+6d",     .n = 12345,    .out = "+12345" },
      { .fmt = "%+06d",    .n = 12345,    .out = "+12345" },

      { .fmt = "a%+7d",    .n = 12345,    .out = "a +12345" },
      { .fmt = "a%+07d",   .n = 12345,    .out = "a +12345" },

      // Same under/over cases but with negative 'n'. Also test mid and trailing digits.
      { .fmt = "%+4d",     .n = -12340,    .out = "-12340" },
      { .fmt = "%+04d",    .n = -10005,    .out = "-10005" },

      { .fmt = "%+6d",     .n = -10005,    .out = "-10005" },
      { .fmt = "%+06d",    .n = -10000,    .out = "-10000" },

      { .fmt = "a%+7d",    .n = -12345,    .out = "a -12345" },
      { .fmt = "a%+07d",   .n = -12345,    .out = "a -12345" },

      { .fmt = "abc%3ddef",.n = 79,        .out = "abc 79def" },     // Continue with rest of formatter.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Wrong output string: fmt = \"%s\",%d", t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Wrong output length: \"%s\",%d -> \"%s\"", t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      OStream_Print();
   }

}

/* --------------------------------- test_Decimal16_Unsigned -------------------------------------- */

void test_Decimal16_Unsigned(void)
{
   typedef struct { C8 const *fmt; C8 const *out; U16 n; } S_Tst;

   S_Tst const tsts[] = {
      // Basic zero and range
      { .fmt = "%u",    .n = 0,          .out = "0" },
      { .fmt = "%u",    .n = MAX_U16,    .out = "65535" },

      { .fmt = "%u",    .n = MAX_U16-1,  .out = "65534" },
      // Zeros with width
      { .fmt = "abc%1u",   .n = 0,        .out = "abc0" },
      { .fmt = "abc%01u",  .n = 0,        .out = "abc0" },

      { .fmt = "abc%3u",   .n = 0,        .out = "abc  0" },
      { .fmt = "abc%03u",  .n = 0,        .out = "abc000" },

      { .fmt = "abc%5u",   .n = 0,        .out = "abc    0" },             // For an int, print up to 5 leading zeros.
      { .fmt = "abc%05u",  .n = 0,        .out = "abc00000" },

      { .fmt = "abc%12u",   .n = 0,        .out = "abc           0" },     // Specifying more than 5 leading zeros results in spaces.
      { .fmt = "abc%012u",  .n = 0,        .out = "abc       00000" },

      // Numbers
      { .fmt = "...%u",    .n = 1,        .out = "...1" },
      { .fmt = "...%+u",   .n = 1,        .out = "...+1" },
      { .fmt = "...%u",    .n = 345,      .out = "...345" },
      { .fmt = "...%4u",   .n = 73,       .out = "...  73" },
      { .fmt = "...%+4u",  .n = 73,       .out = "... +73" },

      { .fmt = "...%+04u", .n = 81,      .out = "...+081" },

      // All these 5-digit corners should print the same
      { .fmt = "%+u",      .n = 12345,    .out = "+12345" },
      { .fmt = "%+5u",     .n = 12345,    .out = "+12345" },
      { .fmt = "%+05u",    .n = 12345,    .out = "+12345" },

      // Just-under and just over field/significant digits.
      { .fmt = "%+4u",     .n = 12345,    .out = "+12345" },
      { .fmt = "%+04u",    .n = 12345,    .out = "+12345" },

      { .fmt = "%+6u",     .n = 12345,    .out = "+12345" },
      { .fmt = "%+06u",    .n = 12345,    .out = "+12345" },

      { .fmt = "a%+7u",    .n = 12345,    .out = "a +12345" },
      { .fmt = "a%+07u",   .n = 12345,    .out = "a +12345" },

      { .fmt = "abc%3udef",.n = 79,        .out = "abc 79def" },     // Continue with rest of formatter.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Wrong output string: fmt = \"%s\",%u", t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Wrong output length: \"%s\",%u -> \"%s\"", t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      OStream_Print();
   }

}

/* --------------------------------- test_Decimal32_Signed -------------------------------------- */

void test_Decimal32_Signed(void)
{
   typedef struct { C8 const *fmt; C8 const *out; S32 n; } S_Tst;

   S_Tst const tsts[] = {
      // Basic zero and range
      { .fmt = "%ld",    .n = 0,          .out = "0" },
      { .fmt = "%ld",    .n = MAX_S32,    .out = "2147483647" },
      { .fmt = "%ld",    .n = MIN_S32,    .out = "-2147483648" },
      { .fmt = "%ld",    .n = MAX_S32-1,  .out = "2147483646" },
      { .fmt = "%ld",    .n = MIN_S32+1,  .out = "-2147483647" },

      // Zeros with width
      { .fmt = "abc%1ld",   .n = 0,        .out = "abc0" },
      { .fmt = "abc%01ld",  .n = 0,        .out = "abc0" },

      { .fmt = "abc%3ld",   .n = 0,        .out = "abc  0" },
      { .fmt = "abc%03ld",  .n = 0,        .out = "abc000" },

      { .fmt = "abc%5ld",   .n = 0,        .out = "abc    0" },             // For an int, print up to 5 leading zeros.
      { .fmt = "abc%05ld",  .n = 0,        .out = "abc00000" },

      { .fmt = "abc%12ld",   .n = 0,        .out = "abc           0" },     // Specifying more than 5 leading zeros results in spaces.
      { .fmt = "abc%012ld",  .n = 0,        .out = "abc  0000000000" },

      // Numbers
      { .fmt = "...%ld",    .n = 1,        .out = "...1" },
      { .fmt = "...%+ld",   .n = 1,        .out = "...+1" },
      { .fmt = "...%ld",    .n = 345,      .out = "...345" },
      { .fmt = "...%4ld",   .n = 73,       .out = "...  73" },
      { .fmt = "...%+4ld",  .n = 73,       .out = "... +73" },
      { .fmt = "...%+4ld",  .n = -73,      .out = "... -73" },

      { .fmt = "...%+08ld", .n = 81,      .out = "...+0000081" },
      { .fmt = "...%+06ld", .n = -81,     .out = "...-00081" },
      { .fmt = "...%04ld",  .n = -81,     .out = "...-081" },

      // All these 10-digit corners should print the same
      { .fmt = "%+ld",      .n = 1234567890,    .out = "+1234567890" },
      { .fmt = "%+10ld",    .n = 1234567890,    .out = "+1234567890" },
      { .fmt = "%+010ld",   .n = 1234567890,    .out = "+1234567890" },

      // Just-under and just over field/significant digits.
      { .fmt = "a%+9ld",     .n = 1234567890,    .out = "a+1234567890" },
      { .fmt = "a%+09ld",    .n = 1234567890,    .out = "a+1234567890" },

      { .fmt = "a%+11ld",    .n = 1234567890,    .out = "a+1234567890" },
      { .fmt = "a%+011ld",   .n = 1234567890,    .out = "a+1234567890" },

      { .fmt = "a%+12ld",    .n = 1234567890,    .out = "a +1234567890" },
      { .fmt = "a%+012ld",   .n = 1234567890,    .out = "a +1234567890" },

      // Same under/over cases but with negative 'n'. Also test mid and trailing digits.
      { .fmt = "a%+9ld",     .n = -1234067800,    .out = "a-1234067800" },
      { .fmt = "a%+09ld",    .n = -1000567800,    .out = "a-1000567800" },

      { .fmt = "a%+11ld",     .n = -1000567890,    .out = "a-1000567890" },
      { .fmt = "a%+011ld",    .n = -1000067890,    .out = "a-1000067890" },

      { .fmt = "a%+12ld",    .n = -1234567890,    .out = "a -1234567890" },
      { .fmt = "a%+012ld",   .n = -1234567890,    .out = "a -1234567890" },

      { .fmt = "abc%3lddef",.n = 79,        .out = "abc 79def" },     // Continue with rest of formatter.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Wrong output string: fmt = \"%s\",%ld", t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Wrong output length: \"%s\",%ld -> \"%s\"", t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_INT32_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      OStream_Print();
   }
}

/* --------------------------------- test_Decimal32_Unsigned -------------------------------------- */

void test_Decimal32_Unsigned(void)
{
   typedef struct { C8 const *fmt; C8 const *out; U32 n; } S_Tst;

   S_Tst const tsts[] = {
      // Basic zero and range
      { .fmt = "%lu",    .n = 0,          .out = "0" },
      { .fmt = "%lu",    .n = MAX_U32,    .out = "4294967295" },
      { .fmt = "%lu",    .n = MAX_U32-1,  .out = "4294967294" },

      // Zeros with width
      { .fmt = "abc%1lu",   .n = 0,        .out = "abc0" },
      { .fmt = "abc%01lu",  .n = 0,        .out = "abc0" },

      { .fmt = "abc%3lu",   .n = 0,        .out = "abc  0" },
      { .fmt = "abc%03lu",  .n = 0,        .out = "abc000" },

      { .fmt = "abc%5lu",   .n = 0,        .out = "abc    0" },             // For an int, print up to 5 leading zeros.
      { .fmt = "abc%05lu",  .n = 0,        .out = "abc00000" },

      { .fmt = "abc%12lu",   .n = 0,        .out = "abc           0" },     // Specifying more than 5 leading zeros results in spaces.
      { .fmt = "abc%012lu",  .n = 0,        .out = "abc  0000000000" },

      // Numbers
      { .fmt = "...%lu",    .n = 1,        .out = "...1" },
      { .fmt = "...%+lu",   .n = 1,        .out = "...+1" },
      { .fmt = "...%lu",    .n = 345,      .out = "...345" },
      { .fmt = "...%4lu",   .n = 73,       .out = "...  73" },
      { .fmt = "...%+4lu",  .n = 73,       .out = "... +73" },

      { .fmt = "...%+08lu", .n = 81,      .out = "...+0000081" },

      // All these 10-digit corners should print the same
      { .fmt = "%+lu",      .n = 1234567890,    .out = "+1234567890" },
      { .fmt = "%+10lu",    .n = 1234567890,    .out = "+1234567890" },
      { .fmt = "%+010lu",   .n = 1234567890,    .out = "+1234567890" },

      // Just-under and just over field/significant digits.
      { .fmt = "a%+9lu",     .n = 1234567890,    .out = "a+1234567890" },
      { .fmt = "a%+09lu",    .n = 1234567890,    .out = "a+1234567890" },

      { .fmt = "a%+11lu",    .n = 1234567890,    .out = "a+1234567890" },
      { .fmt = "a%+011lu",   .n = 1234567890,    .out = "a+1234567890" },

      { .fmt = "a%+12lu",    .n = 1234567890,    .out = "a +1234567890" },
      { .fmt = "a%+012lu",   .n = 1234567890,    .out = "a +1234567890" },

      { .fmt = "abc%3ludef",.n = 79,        .out = "abc 79def" },     // Continue with rest of formatter.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Wrong output string: fmt = \"%s\",%lu", t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Wrong output length: \"%s\",%lu -> \"%s\"", t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_UINT32_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      OStream_Print();
   }
}

// ----------------------------------------- eof --------------------------------------------
