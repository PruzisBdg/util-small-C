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

/* --------------------------------- test_JustFormatters -------------------------------------- */

void test_JustFormatters(void) {
   typedef struct { C8 const *fmt; C8 const *out; } S_Tst;

   S_Tst const tsts[] = {
      { .fmt = "", .out = "" },                                                // Empty
      { .fmt = "My 1st formatter\r\n", .out = "My 1st formatter\r\n" },   // Return is number of chars written

      // This printf condenses escape sequences.
      { .fmt = "\\\\a\r\n",         .out = "\\a\r\n"   },
      { .fmt = "\\\\tZ\r\n",        .out = "\\tZ\r\n"  },
      { .fmt = "Ten%%s",             .out = "Ten%s"      },         // Escapes '%'
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++) {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt);

      C8 b0[100];
      sprintf(b0, "Wrong output string: fmt = \"%s\"", t->fmt);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Wrong output length: \"%s\"", t->fmt);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      //OStream_Print();
   }
}

/* --------------------------------- test_JustStrings -------------------------------------- */

void test_JustStrings(void) {
   typedef struct { C8 const *fmt; C8 const *str; C8 const *out; } S_Tst;

   S_Tst const tsts[] = {
      { .fmt = "%s", .str = "", .out = ""},                                                // Empty
      { .fmt = "..%s..", .str = "123", .out = "..123.." },
      { .fmt = "..%s..", .str = "123", .out = "..123.." },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++) {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->str);

      C8 b0[100];
      sprintf(b0, "Wrong output string: fmt = \"%s\"(\"%s\") -> \"%s\"", t->fmt, t->str, OStream_Get());
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Wrong output length: fmt = \"%s\"(\"%s\") -> \"%s\"", t->fmt, t->str, OStream_Get());
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      //OStream_Print();
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
      //OStream_Print();
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
      //OStream_Print();
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
      //OStream_Print();
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
      //OStream_Print();
   }
}

/* --------------------------------- test_Hex16 -------------------------------------- */

void test_Hex16(void)
{
   typedef struct { C8 const *fmt; C8 const *out; U16 n; } S_Tst;

   S_Tst const tsts[] = {
      // Basic zero and range
      { .fmt = "%x",    .n = 0,          .out = "0" },                     // Zero prints value of the byte i.e 0x00
      { .fmt = "%x",    .n = MAX_U16,    .out = "ffff" },                  // Lowercase
      { .fmt = "%X",    .n = MAX_U16-1,  .out = "FFFE" },                  // Uppercase
      // Zeros with width
      { .fmt = "abc%1x",   .n = 0,        .out = "abc0" },
      { .fmt = "abc%01x",  .n = 0,        .out = "abc0" },

      { .fmt = "abc%3x",   .n = 0,        .out = "abc  0" },
      { .fmt = "abc%03x",  .n = 0,        .out = "abc000" },

      { .fmt = "abc%4x",   .n = 0,        .out = "abc   0" },             // For an Hex16, print up to 4 leading zeros.
      { .fmt = "abc%04x",  .n = 0,        .out = "abc0000" },

      { .fmt = "abc%12x",   .n = 0,        .out = "abc           0" },     // Specifying more than 4 leading zeros results in spaces.
      { .fmt = "abc%012x",  .n = 0,        .out = "abc        0000" },

      // Numbers
      { .fmt = "...%x",    .n = 1,        .out = "...1" },
      { .fmt = "...%x",    .n = 0xABCD,   .out = "...abcd" },
      { .fmt = "...%X",    .n = 0xABCD,   .out = "...ABCD" },
      { .fmt = "...%+x",   .n = 1,        .out = "...1" },                 // For Hex, the '+' modifier is ignored.
      { .fmt = "...%x",    .n = 0x345,      .out = "...345" },
      { .fmt = "...%4x",   .n = 0x73,       .out = "...  73" },

      { .fmt = "...%04x", .n = 0x81,      .out = "...0081" },

      // All these 4-digit corners should print the same
      { .fmt = "%x",      .n = 0x1234,    .out = "1234" },
      { .fmt = "%4x",     .n = 0x1234,    .out = "1234" },
      { .fmt = "%04x",    .n = 0x1234,    .out = "1234" },

      // Just-under and just over field/significant digits.
      { .fmt = "%3x",     .n = 0x1234,    .out = "1234" },
      { .fmt = "%03x",    .n = 0x1234,    .out = "1234" },

      { .fmt = "%5x",     .n = 0x1234,    .out = " 1234" },
      { .fmt = "%05x",    .n = 0x1234,    .out = " 1234" },

      { .fmt = "a%6x",    .n = 0x1234,    .out = "a  1234" },
      { .fmt = "a%06x",   .n = 0x1234,    .out = "a  1234" },

      { .fmt = "abc%3xdef",.n = 0x79,        .out = "abc 79def" },     // Continue with rest of formatter.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Wrong output string: fmt = \"%s\",0x%x", t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Wrong output length: \"%s\",0x%x -> \"%s\"", t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      //OStream_Print();
   }
}


/* --------------------------------- test_Hex32 -------------------------------------- */

void test_Hex32(void)
{
   typedef struct { C8 const *fmt; C8 const *out; U32 n; } S_Tst;

   S_Tst const tsts[] = {
      // Basic zero and range
      { .fmt = "%lx",    .n = 0,          .out = "0" },                    // Zero prints value of the byte i.e 0x00
      { .fmt = "%lx",    .n = MAX_U32,    .out = "ffffffff" },             // Lowercase
      { .fmt = "%lX",    .n = MAX_U32-1,  .out = "FFFFFFFE" },             // Uppercase
      // Zeros with width
      { .fmt = "abc%1lx",   .n = 0,        .out = "abc0" },
      { .fmt = "abc%01lx",  .n = 0,        .out = "abc0" },

      { .fmt = "abc%3lx",   .n = 0,        .out = "abc  0" },
      { .fmt = "abc%03lx",  .n = 0,        .out = "abc000" },

      { .fmt = "abc%8lx",   .n = 0,        .out = "abc       0" },             // For an Hex32, print up to 8 leading zeros.
      { .fmt = "abc%08lx",  .n = 0,        .out = "abc00000000" },

      { .fmt = "abc%12lx",   .n = 0,        .out = "abc           0" },     // Specifying more than 8 leading zeros results in spaces.
      { .fmt = "abc%012lx",  .n = 0,        .out = "abc    00000000" },

      // Numbers
      { .fmt = "...%lx",    .n = 1,        .out = "...1" },
      { .fmt = "...%+lx",   .n = 1,        .out = "...1" },                 // For Hex, the '+' modifier is ignored.
      { .fmt = "...%lx",    .n = 0x345ABC,      .out = "...345abc" },
      { .fmt = "...%8lx",   .n = 0x7399,       .out = "...    7399" },

      { .fmt = "...%08lx", .n = 0x81,      .out = "...00000081" },

      // All these 8-digit corners shoxld print the same
      { .fmt = "%lX",      .n = 0x1234ABCD,    .out = "1234ABCD" },
      { .fmt = "%8lx",     .n = 0x1234CDEF,    .out = "1234cdef" },
      { .fmt = "%08lx",    .n = 0x12345678,    .out = "12345678" },

      // Just-under and just over field/significant digits.
      { .fmt = "%7lx",     .n = 0x1234CDEF,    .out = "1234cdef" },
      { .fmt = "%07lx",    .n = 0x12340002,    .out = "12340002" },

      { .fmt = "%9lX",     .n = 0x10A0F234,    .out = " 10A0F234" },
      { .fmt = "%09lX",    .n = 0x10A0F234,    .out = " 10A0F234" },

      { .fmt = "a%10lx",    .n = 0x12345600,    .out = "a  12345600" },
      { .fmt = "a%010lx",   .n = 0x12345600,    .out = "a  12345600" },

      { .fmt = "abc%3lxdef",.n = 0x79,        .out = "abc 79def" },     // Continue with rest of formatter.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Test #%d: Wrong output string: fmt = \"%s\",0x%lx", i, t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Test #%d: Wrong output length: \"%s\",0x%lx -> \"%s\"", i, t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      //OStream_Print();
   }
}

/* ----------------------------------- test_Char -------------------------------------------- */

void test_Char(void)
{
   typedef struct { C8 const *fmt; C8 ch; C8 const *out; } S_Tst;

   S_Tst const tsts[] = {
      // Zero ('\0') results in no output; it's the string terminator.
      { .fmt = "%c",    .ch = '\0',    .out = "" },

      { .fmt = "%c",    .ch = 'Z',     .out = "Z" },

      // For '%c' non-printables are output as-is.
      { .fmt = "%c",    .ch = '\r',    .out = "\x0D" },
      { .fmt = "%c",    .ch = 0x02,    .out = "\x02" },

      // For '%C" \r,\n,\t are quoted as their escape chars; else as Hex number.
      // printables are output as-is.
      { .fmt = "%c",    .ch = 'M',     .out = "M" },
      { .fmt = "%C",    .ch = '\r',    .out = "\\r" },
      { .fmt = "%C",    .ch = '\t',    .out = "\\t" },
      { .fmt = "%C",    .ch = '\n',    .out = "\\n" },
      { .fmt = "%C",    .ch = 0x02,    .out = "\\x02" },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->ch);

      C8 b0[100];
      sprintf(b0, "Test #%d: Wrong output string: fmt = \"%s\",%c", i, t->fmt, t->ch);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Test #%d: Wrong output length: \"%s\"<-\'%c\' -> \"%s\"", i, t->fmt, t->ch, t->out);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      //OStream_Print();
   }
}


/* ----------------------------------- test_Float -------------------------------------------- */

void test_Float(void)
{
   typedef struct { C8 const *fmt; float n; C8 const *out; } S_Tst;

   S_Tst const tsts[] = {
      // Zero ('\0') results in no output; it's the string terminator.
      { .fmt = "%f",       .n = 0.0,         .out = "0.000000" },       // Default precision is 6.
      { .fmt = "%f",       .n = 0.12,        .out = "0.120000" },
      { .fmt = "%f",       .n = 12.345,      .out = "12.345000" },
      { .fmt = "%F",       .n = 12.345,      .out = "12.345000" },      // %F and %f are same.

      { .fmt = "%4.2f",    .n = 12.3456,     .out = "12.34" },

      { .fmt = "%07.2f",   .n = 12.3456,     .out = "0012.34" },
      { .fmt = "%7.2f",    .n = 12.3456,     .out = "  12.34" },
      { .fmt = "%+07.2f",   .n = 12.3456,     .out = "+012.34" },
      { .fmt = "%+7.2f",    .n = 12.3456,     .out = " +12.34" },
      { .fmt = "%+07.2f",   .n = -12.3456,    .out = "-012.34" },
      { .fmt = "%+7.2f",    .n = -12.3456,    .out = " -12.34" },

      // If number is too large or small, fall back to exponent printout.
      { .fmt = "%+12.2f",    .n = -1.234E12,    .out = "   -1.23e+12" },   // '%f' -> 'e'.
      { .fmt = "%+12.2F",    .n = 1.234E-12,    .out = "   +1.23E-12" },   // '%F' -> 'e'.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Test #%d: Wrong output string: fmt = \"%s\",%f", i, t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Test #%d: Wrong output length: \"%s\"<-\'%f\' -> \"%s\"", i, t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      //OStream_Print();
   }
}

/* ----------------------------------- test_FloatExp -------------------------------------------- */

void test_FloatExp(void)
{
   typedef struct { C8 const *fmt; float n; C8 const *out; } S_Tst;

   S_Tst const tsts[] = {
      { .fmt = "%e",          .n = 0.0,         .out = "0.000000e0" },
      { .fmt = "%+E",         .n = 0.0,         .out = "+0.000000E+0" },
      { .fmt = "%e",          .n = 0.12000001,  .out = "1.200000e-1" },
      { .fmt = "%+e",         .n = 12.0,        .out = "+1.200000e+1" },
      { .fmt = "%e",          .n = 12.345679,   .out = "1.234567e1" },
      { .fmt = "%E",          .n = 12.345679,   .out = "1.234567E1" },

      { .fmt = "%4.2E",       .n = 12.3456,     .out = "1.23E1" },

      { .fmt = "%09.3e",      .n = 12.3456,     .out = "001.234e1" },
      { .fmt = "%9.3e",       .n = 12.3456,     .out = "  1.234e1" },
      { .fmt = "%+012.2E",    .n = 12.3456,     .out = "+00001.23E+1" },
      { .fmt = "%+12.2e",     .n = 12.3456,     .out = "    +1.23e+1" },
      { .fmt = "%+012.2E",    .n = -12.3456,    .out = "-00001.23E+1" },
      { .fmt = "%+12.2e",     .n = -12.3456,    .out = "    -1.23e+1" },

      { .fmt = "%+12.2E",    .n = -1.234E18,    .out = "   -1.23E+18" },
      { .fmt = "%+12.2e",    .n = 1.234E-18,    .out = "   +1.23e-18" },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      OStream_Reset();
      T_PrintCnt rtn = tiny1_printf(t->fmt, t->n);

      C8 b0[100];
      sprintf(b0, "Test #%d: Wrong output string: fmt = \"%s\",%f", i, t->fmt, t->n);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, OStream_Get(), b0);                  // Correct output.
      sprintf(b0, "Test #%d: Wrong output length: \"%s\"<-\'%f\' -> \"%s\"", i, t->fmt, t->n, t->out);
      TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(t->out), rtn, b0);                       // tiny1_printf() should return length of output string.
      //OStream_Print();
   }
}



// ----------------------------------------- eof --------------------------------------------
