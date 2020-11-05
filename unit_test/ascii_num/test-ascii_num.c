#include "libs_support.h"
   #if _TARGET_IS == _TARGET_UNITY_TDD
#include "unity.h"
   #endif
#include "arith.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include <string.h>
#include "wordlist.h"

   #if _TARGET_IS != _TARGET_UNITY_TDD
#define TEST_FAIL()
#define TEST_ASSERT_TRUE_MESSAGE(...)
#define TEST_ASSERT_FALSE_MESSAGE(...)
#define TEST_ASSERT_EQUAL_UINT8_MESSAGE(...)
#define TEST_ASSERT_EQUAL_INT16_MESSAGE(...)
#define TEST_ASSERT_EQUAL_INT32_MESSAGE(...)
#define TEST_ASSERT_EQUAL_UINT32_MESSAGE(...)
#define TEST_ASSERT_EQUAL_FLOAT_MESSAGE(...)
#define TEST_ASSERT_EQUAL_STRING_MESSAGE(...)
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

/* ------------------------------------------------------------------------------------------- */
PRIVATE C8 const *catRtn(C8 *out, U8 testNum, C8 const *preamble, C8 const *inStr, C8 const *tail) {
    sprintf(out, "tst #%d. %s. testStr = \"%s\" tail = \"%s\"",
                testNum, preamble, inStr == NULL ? "NULL" : inStr, tail == NULL ? "NULL" : tail);
    return out; }

/* ---------------------------- test_ReadDirtyASCIIInt ------------------------------------------- */

void test_ReadDirtyASCIIInt(void)
{
    typedef struct { C8 const *inStr; S16 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        { .inStr = "",              .result = 0,    .tail = NULL,       .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc dr()",      .result = 0,    .tail = NULL,       .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",          .result = 0,    .tail = NULL,       .outModified = false },    // A lone '-' is not a number.

        { .inStr = "0",             .result = 0,    .tail = "",         .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "23",            .result = 23,   .tail = "",         .outModified = true },
        { .inStr = "234",           .result = 234,  .tail = "",         .outModified = true },
        { .inStr = "0xyz",          .result = 0,    .tail = "xyz",      .outModified = true },

        { .inStr = "abc123xyz",     .result = 123,  .tail = "xyz",      .outModified = true },      // Ignore leading non-digits.
        { .inStr = "abc\r\n\t 123pq",.result = 123, .tail = "pq",       .outModified = true },      // Boof thru leading whitespace

        { .inStr = "-0xyz",         .result = 0,    .tail = "xyz",      .outModified = true },
        { .inStr = "abc-123xyz",    .result = -123, .tail = "xyz",      .outModified = true },
        { .inStr = "abc123 456xyz", .result = 123,  .tail = " 456xyz",  .outModified = true },

        { .inStr = "32767xyz",      .result = 32767, .tail = "xyz",     .outModified = true },      // Min and max S16.
        { .inStr = "-32768xyz",     .result = -32768, .tail = "xyz",    .outModified = true },

        { .inStr = "0450xyz",       .result = 450,  .tail = "xyz",      .outModified = true },
        { .inStr = "000000450xyz",  .result = 450,  .tail = "xyz",      .outModified = true },      // Bazillion leading zeros are OK.
        { .inStr = "-000000450xyz", .result = -450, .tail = "xyz",      .outModified = true },      // ...and with leading '-'.

        // If 5 or more digits puts the number over/under S16, then returns with the 4 digit number
        // and with tail at the remaining digits.
        { .inStr = "000456789xyz",  .result = 4567, .tail = "89xyz",     .outModified = true },
        { .inStr = "-000456789xyz", .result = -4567,.tail = "89xyz",     .outModified = true },

        { .inStr = "- 123pqr",      .result = 123,  .tail = "pqr",      .outModified = true },      // '-' must be next to leading digit.
        { .inStr = "+123pqr",       .result = 123,  .tail = "pqr",      .outModified = true },      // "+" is ignored.

        { .inStr = "123.456",       .result = 123,  .tail = ".456",      .outModified = true },     // DP is ignored. The number is what precedes the DP.
        { .inStr = "123 456",       .result = 123,  .tail = " 456",      .outModified = true },     // Space breaks a number; like any other non-digit.
        { .inStr = "000 456",       .result = 0,    .tail = " 456",      .outModified = true },     // Leading zero is read as just that.
        { .inStr = "-+456xyz",      .result = 456,    .tail = "xyz",      .outModified = true },     // Leading zero is read as just that.
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];

        // Prime 'out' with some value other than the correct result. So can tell if was modified or no.
        S16 was = t->result + 1;
        S16 out = was;
        C8 const * ret = ReadDirtyASCIIInt(t->inStr, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr, ret)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_INT16_MESSAGE(was, out, _msg("Output should not have been modified", t)); }
        else {
            if(out == was) {
               TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
            else {
               TEST_ASSERT_EQUAL_FLOAT_MESSAGE(t->result, out,  _msg("Incorrect output", t)); }}

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}
}

/* ---------------------------- test_ReadThruToASCIIInt ------------------------------------------- */

void test_ReadThruToASCIIInt(void)
{
    BIT strip_abcs(C8 ch) { return ch == 'a' || ch == 'b' || ch == 'c'; }

    BIT strip_abcs_minusToo(C8 ch) { return ch == 'a' || ch == 'b' || ch == 'c' || ch == '-'; }

    typedef struct { C8 const *inStr; BIT(*stripWith)(C8 ch); S16 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        // ---- No custom stripper. Eat just whitespace, like atoi()

        { .inStr = "",              .result = 0,    .tail = NULL,       .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc dr()",      .result = 0,    .tail = NULL,       .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",          .result = 0,    .tail = NULL,       .outModified = false },    // A lone '-' is not a number.

        { .inStr = "0",             .result = 0,    .tail = "",         .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "23",            .result = 23,   .tail = "",         .outModified = true },
        { .inStr = "234",           .result = 234,  .tail = "",         .outModified = true },
        { .inStr = "0xyz",          .result = 0,    .tail = "xyz",      .outModified = true },

        { .inStr = "\r\n\t 123pq",  .result = 123,  .tail = "pq",       .outModified = true },      // Boof thru leading whitespace

        { .inStr = "-0xyz",         .result = 0,    .tail = "xyz",      .outModified = true },

        { .inStr = " -123xyz",      .result = -123, .tail = "xyz",      .outModified = true },
        { .inStr = "123 456xyz",    .result = 123,  .tail = " 456xyz",  .outModified = true },

        { .inStr = "32767xyz",      .result = 32767, .tail = "xyz",     .outModified = true },      // Min and max S16.
        { .inStr = "-32768xyz",     .result = -32768, .tail = "xyz",    .outModified = true },

        { .inStr = "0450xyz",       .result = 450,  .tail = "xyz",      .outModified = true },
        { .inStr = "000000450xyz",  .result = 450,  .tail = "xyz",      .outModified = true },      // Bazillion leading zeros are OK.
        { .inStr = "-000000450xyz", .result = -450, .tail = "xyz",      .outModified = true },      // ...and with leading '-'.

        // If 5 or more digits puts the number over/under S16, then returns with the 4 digit number
        // and with tail at the remaining digits.
        { .inStr = "000456789xyz",  .result = 4567, .tail = "89xyz",    .outModified = true },
        { .inStr = "-000456789xyz", .result = -4567,.tail = "89xyz",    .outModified = true },

        { .inStr = "- 123pqr",      .result = 0,    .tail = NULL,       .outModified = false },     // '-' must be next to leading digit.
        { .inStr = "+123pqr",       .result = 123,  .tail = "pqr",      .outModified = true },      // "+" is ignored.

        { .inStr = "123.456",       .result = 123,  .tail = ".456",     .outModified = true },      // DP is ignored. The number is what precedes the DP.
        { .inStr = "123 456",       .result = 123,  .tail = " 456",     .outModified = true },      // Space breaks a number; like any other non-digit.
        { .inStr = "000 456",       .result = 0,    .tail = " 456",     .outModified = true },      // Leading zero is read as just that.
        { .inStr = "-+456xyz",      .result = -456, .tail = "xyz",      .outModified = true },      // '-+' prepended is a legal number, yes.

        // ---- Add strippers.

        { .inStr = "abc123 xyz",    .result = 123,  .tail = " xyz",     .outModified = true,    .stripWith = strip_abcs },
        // Accept '-' and '+'
        { .inStr = "abc-123 xyz",   .result = -123, .tail = " xyz",     .outModified = true,    .stripWith = strip_abcs },
        { .inStr = "abc+123 xyz",   .result = 123,  .tail = " xyz",     .outModified = true,    .stripWith = strip_abcs },
        // '+', '-' must be adjacent to digits.
        { .inStr = "ab-c123 xyz",   .result = 0,    .tail = NULL,       .outModified = false,   .stripWith = strip_abcs },
        { .inStr = "ab+c123 xyz",   .result = 0,    .tail = NULL,       .outModified = false,   .stripWith = strip_abcs },

        // Stripper also takes '-', even if in front of the number.
        { .inStr = "ab-c123 xyz",   .result = 123,  .tail = " xyz",     .outModified = true,    .stripWith = strip_abcs_minusToo },
        { .inStr = "abc-123 xyz",   .result = 123,  .tail = " xyz",     .outModified = true,    .stripWith = strip_abcs_minusToo },
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];

        // Prime 'out' with some value other than the correct result. So can tell if was modified or no.
        S16 was = t->result + 1;
        S16 out = was;
        C8 const * ret = ReadThruToASCIIInt(t->inStr, &out, t->stripWith );

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr, ret)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_INT16_MESSAGE(was, out, _msg("Output should not have been modified", t)); }
        else {
            if(out == was) {
               TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
            else {
               TEST_ASSERT_EQUAL_FLOAT_MESSAGE(t->result, out,  _msg("Incorrect output", t)); }}

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}

}

/* ---------------------------- test_ReadDirtyASCII_S32 ------------------------------------------- */

void test_ReadDirtyASCII_S32(void)
{
    typedef struct { C8 const *inStr; S32 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        { .inStr = "",              .result = 0,    .tail = NULL,             .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc dr()",      .result = 0,    .tail = NULL,             .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",          .result = 0,    .tail = NULL,             .outModified = false },    // A lone '-' is not a number.

        { .inStr = "0",             .result = 0,         .tail = "",          .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "23",            .result = 23,        .tail = "",          .outModified = true },
        { .inStr = "23456789",      .result = 23456789,  .tail = "",          .outModified = true },
        { .inStr = "0xyz",          .result = 0,         .tail = "xyz",       .outModified = true },

        { .inStr = "abc123xyz",     .result = 123,       .tail = "xyz",       .outModified = true },      // Ignore leading non-digits.
        { .inStr = "abc\r\n\t 123pq",.result = 123,      .tail = "pq",        .outModified = true },      // Boof thru leading whitespace

        { .inStr = "-0xyz",         .result = 0,         .tail = "xyz",       .outModified = true },
        { .inStr = "abc-1234567xyz",.result = -1234567,      .tail = "xyz",       .outModified = true },
        { .inStr = "abc123 456xyz", .result = 123,       .tail = " 456xyz",   .outModified = true },

        { .inStr = "32767xyz",      .result = 32767,     .tail = "xyz",       .outModified = true },      // Min and max S16.
        { .inStr = "-32768xyz",     .result = -32768,    .tail = "xyz",       .outModified = true },

        { .inStr = "2147483647xyz", .result = 2147483647, .tail = "xyz",      .outModified = true },      // Min and max S32.
        { .inStr = "-2147483648xyz",.result = -2147483648, .tail = "xyz",     .outModified = true },

        { .inStr = "0450xyz",       .result = 450,       .tail = "xyz",       .outModified = true },
        { .inStr = "000000450328xyz",.result = 450328,   .tail = "xyz",       .outModified = true },      // Bazillion leading zeros are OK.
        { .inStr = "-000000450xyz", .result = -450,      .tail = "xyz",       .outModified = true },      // ...and with leading '-'.

        // If 10 or more digits puts the number over/under S32, (2147483647, -2147483648) then returns with the 9 digit number
        // and with tail at the remaining digits.
        { .inStr = "0003147483647xyz",  .result = 314748364, .tail = "7xyz",  .outModified = true },
        { .inStr = "-0003147483648xyz", .result = -314748364,.tail = "8xyz",  .outModified = true },

        { .inStr = "- 123pqr",      .result = 123,       .tail = "pqr",       .outModified = true },      // '-' must be next to leading digit.
        { .inStr = "+123pqr",       .result = 123,       .tail = "pqr",       .outModified = true },      // "+" is ignored.

        { .inStr = "123.456",       .result = 123,       .tail = ".456",      .outModified = true },     // DP is ignored. The number is what precedes the DP.
        { .inStr = "123 456",       .result = 123,       .tail = " 456",      .outModified = true },     // Space breaks a number; like any other non-digit.
        { .inStr = "000 456",       .result = 0,         .tail = " 456",      .outModified = true },     // Leading zero is read as just that.
        { .inStr = "-+456xyz",      .result = 456,       .tail = "xyz",       .outModified = true },     // Leading zero is read as just that.
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];

        // Prime 'out' with some value other than the correct result. So can tell if was modified or no.
        S32 was = t->result + 1;
        S32 out = was;
        C8 const * ret = ReadDirtyASCII_S32(t->inStr, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr, ret)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_INT32_MESSAGE(was, out, _msg("Output should not have been modified", t)); }
        else {
            if(out == was) {
               TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
            else {
               if(t->result != out) {
                  printf("Incorrect output: expected %ld, got %ld ", t->result, out);
                  TEST_FAIL();}}}

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}
}

/* ---------------------------- test_ReadAsciiS32 ------------------------------------------- */

void test_ReadAsciiS32(void)
{
    typedef struct { C8 const *inStr; S32 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        { .inStr = "",              .result = 0,    .tail = NULL,             .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc dr()",      .result = 0,    .tail = NULL,             .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",          .result = 0,    .tail = NULL,             .outModified = false },    // A lone '-' is not a number.

        { .inStr = "0",             .result = 0,         .tail = "",          .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "23",            .result = 23,        .tail = "",          .outModified = true },
        { .inStr = "23456789",      .result = 23456789,  .tail = "",          .outModified = true },
        { .inStr = "0xyz",          .result = 0,         .tail = "xyz",       .outModified = true },

        //{ .inStr = "abc123xyz",     .result = 123,       .tail = "xyz",       .outModified = true },      // Ignore leading non-digits.
        //{ .inStr = "abc\r\n\t 123pq",.result = 123,      .tail = "pq",        .outModified = true },      // Boof thru leading whitespace

        { .inStr = "-0xyz",         .result = 0,         .tail = "xyz",       .outModified = true },
        //{ .inStr = "abc-1234567xyz",.result = -1234567,      .tail = "xyz",       .outModified = true },
        //{ .inStr = "abc123 456xyz", .result = 123,       .tail = " 456xyz",   .outModified = true },

        { .inStr = "32767xyz",      .result = 32767,     .tail = "xyz",       .outModified = true },      // Min and max S16.
        { .inStr = "-32768xyz",     .result = -32768,    .tail = "xyz",       .outModified = true },

        { .inStr = "2147483647xyz", .result = 2147483647, .tail = "xyz",      .outModified = true },      // Min and max S32.
        { .inStr = "-2147483648xyz",.result = -2147483648, .tail = "xyz",     .outModified = true },

        { .inStr = "0450xyz",       .result = 450,       .tail = "xyz",       .outModified = true },
        { .inStr = "000000450328xyz",.result = 450328,   .tail = "xyz",       .outModified = true },      // Bazillion leading zeros are OK.
        { .inStr = "-000000450xyz", .result = -450,      .tail = "xyz",       .outModified = true },      // ...and with leading '-'.

        // If 10 or more digits puts the number over/under S32, (2147483647, -2147483648) then returns with the 9 digit number
        // and with tail at the remaining digits.
        { .inStr = "0003147483647xyz",  .result = 314748364, .tail = "7xyz",  .outModified = true },
        { .inStr = "-0003147483648xyz", .result = -314748364,.tail = "8xyz",  .outModified = true },

        { .inStr = "+123pqr",       .result = 123,       .tail = "pqr",       .outModified = true },      // "+" is ignored.

        { .inStr = "123.456",       .result = 123,       .tail = ".456",      .outModified = true },     // DP is ignored. The number is what precedes the DP.
        { .inStr = "123 456",       .result = 123,       .tail = " 456",      .outModified = true },     // Space breaks a number; like any other non-digit.
        { .inStr = "000 456",       .result = 0,         .tail = " 456",      .outModified = true },     // Leading zero is read as just that.

        { .inStr = "- 123pqr",      .result = 0,         .tail = NULL,       .outModified = false },      // '-' must be next to leading digit.
        { .inStr = "-+456xyz",      .result = 0,         .tail = NULL,       .outModified = false },     // Just 1 sign allowed, directly before 1st digit.
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];

        // Prime 'out' with some value other than the correct result. So can tell if was modified or no.
        S32 was = t->result + 1;
        S32 out = was;
        C8 const * ret = ReadAsciiS32(t->inStr, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr, ret)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_INT32_MESSAGE(was, out, _msg("Output should not have been modified", t)); }
        else {
            if(out == was) {
               TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
            else {
               if(t->result != out) {
                  printf("Incorrect output: expected %ld, got %ld ", t->result, out);
                  TEST_FAIL();}}}

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}
}

/* --------------------------------- test_ReadASCIIToFloat ----------------------------------------- */

void test_ReadASCIIToFloat(void)
{
    typedef struct { C8 const *inStr; float result; C8 const *tail; bool outModified; C8 const *delimiters; } S_Tst;

    S_Tst const tsts[] = {
        // No number
        { .inStr = "",              .result = 0,        .tail = NULL,       .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc",           .result = 0,        .tail = NULL,       .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",          .result = 0,        .tail = NULL,       .outModified = false },    // A lone '-' is not a number.

        // Integers
        { .inStr = "0",             .result = 0.0,      .tail = "",         .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "-0",            .result = 0.0,      .tail = "",         .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "23",            .result = 23.0,     .tail = "",         .outModified = true },
        { .inStr = "234",           .result = 234,      .tail = "",         .outModified = true },
        { .inStr = "23pqr",         .result = 23,       .tail = "pqr",      .outModified = true },
        { .inStr = "23xyz",         .result = 23,       .tail = "xyz",      .outModified = true },

        // To get e.g TAG = 23.4"
        { .inStr = ", = 23pqr",     .result = 23.0,     .tail = "pqr",      .outModified = true, .delimiters = " ,=" },

        { .inStr = "000000450xyz",  .result = 450,      .tail = "xyz",      .outModified = true },      // Bazillion leading zeros are OK.
        { .inStr = "-+456xyz",      .result = 456,      .tail = "xyz",      .outModified = true },     // The '-' isn't next to number is is ignored.

        { .inStr = "  123xyz",     .result = 123,       .tail = "xyz",      .outModified = true },      // Ignore leading spaces.

        { .inStr = " -234",         .result = -234,     .tail = "",         .outModified = true },
        { .inStr = " +234",         .result =  234,     .tail = "",         .outModified = true },

        { .inStr = "- 234",         .result = 0,        .tail = NULL,       .outModified = false },   // '-' must be connected to number.

        // Fixed point
        { .inStr = " 12.345abc",    .result =  12.345,   .tail = "abc",     .outModified = true },
        { .inStr = "-12.345abc",    .result =  -12.345,  .tail = "abc",     .outModified = true },

        { .inStr = " 0.345abc",    .result =  0.345,   .tail = "abc",       .outModified = true },
        { .inStr = "  .345abc",    .result =  0.345,   .tail = "abc",       .outModified = true },

        // Incomplete fixed point; the trailing '.' is considered part of the number.
        { .inStr = " 12.abc",       .result =  12.0,      .tail = "abc",    .outModified = true },

        // Float
        { .inStr = " 1.23E4abc",    .result =  12300.0,  .tail = "abc",     .outModified = true },
        { .inStr = " -5.67E1abc",   .result =  -56.7,    .tail = "abc",     .outModified = true },
        { .inStr = " 0.123E2abc",   .result =  12.3,     .tail = "abc",     .outModified = true },
        { .inStr = " 4.567E-5abc",  .result =  4.567E-5, .tail = "abc",     .outModified = true },
        // Also recognises 'e'
        { .inStr = " 1.23e4abc",    .result =  12300.0,  .tail = "abc",     .outModified = true },

        // Incomplete float - just mantissa as int.
        { .inStr = " 1.23Eabc",     .result =  1.23,     .tail = "Eabc",    .outModified = true },

        // Hex
        { .inStr = "0x00 abc",      .result = 0.0,       .tail = " abc",    .outModified = true },
        { .inStr = "0xFE abc",      .result = 254.0,     .tail = " abc",    .outModified = true },
        { .inStr = "0x12ACxyz",     .result = 4780.0,    .tail = "xyz",     .outModified = true },
        { .inStr = "0xFFFF",        .result = 65535.0,   .tail = "",        .outModified = true },
        // Lowercase OK.
        { .inStr = "0xabcdefpqr",   .result = 11259375.0,.tail = "pqr",     .outModified = true },     // Kinda suprised float does this.

        // 'X' is also hex.
        { .inStr = "0XFE abc",      .result = 254.0,     .tail = " abc",    .outModified = true },      // "0" -> 0, output at end of string.
        // Must be '0x..' or '0X..'
        { .inStr = "xFE abc",       .result = 0,          .tail = NULL,     .outModified = false },     // No number -> return NULL, output unchanged.
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];

        // If delimiters are specified for this test then use them; else will use default (SPC).
        if(t->delimiters != NULL) {
            Str_Delimiters = t->delimiters; }
        else {
            Str_Delimiters = NULL; }

        // Prime 'out' with some value other than the correct result. So can tell if was modified or no.
        float was = t->result + 1;
        float out = was;
        C8 const * ret = ReadASCIIToFloat(t->inStr, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr, ret)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_FLOAT_MESSAGE(was, out, _msg("Output should not have been modified", t)); }
        else {
            if(out == was) {
               TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
            else {
               TEST_ASSERT_EQUAL_FLOAT_MESSAGE(t->result, out,  _msg("Incorrect output", t)); }}

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}
}

/* --------------------------------- test_ReadASCIIToNum ----------------------------------------- */

void test_ReadASCIIToNum(void)
{
    typedef struct {
        C8 const *inStr;
        float outFlt; S32 outS32; U32 outU32;
        C8 const *tail;
        bool outModified; bool reqFloat, gaveHex, gotInt, gotUnsigned; C8 const *delimiters; } S_Tst;

    // To require a float and test we got that.
    #define _ReqFloat_GotFloat  .reqFloat = true,  .gaveHex = false, .gotInt = false,  .gotUnsigned = false
    #define _ReqFloat_GaveHex   .reqFloat = true,  .gaveHex = true,  .gotInt = true,   .gotUnsigned = true
    #define _GotSignedInt       .reqFloat = false, .gaveHex = false, .gotInt = true,   .gotUnsigned = false
    #define _GotUnsignedInt     .reqFloat = false, .gaveHex = false, .gotInt = true,   .gotUnsigned = true
    #define _GotFloat           .reqFloat = false, .gaveHex = false, .gotInt = false,  .gotUnsigned = true

    S_Tst const tsts[] = {
        // No number
        { .inStr = "",              .outFlt = 0,        .tail = NULL,       .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc",           .outFlt = 0,        .tail = NULL,       .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",          .outFlt = 0,        .tail = NULL,       .outModified = false },    // A lone '-' is not a number.

        // ---- Request Floating point conversion.

        // Integers
        { .inStr = "0",             .outFlt = 0.0,      .tail = "",         .outModified = true, _ReqFloat_GotFloat },      // "0" -> 0, output at end of string.
        { .inStr = "-0",            .outFlt = 0.0,      .tail = "",         .outModified = true, _ReqFloat_GotFloat },      // "0" -> 0, output at end of string.
        { .inStr = "23",            .outFlt = 23.0,     .tail = "",         .outModified = true, _ReqFloat_GotFloat },
        { .inStr = "234",           .outFlt = 234,      .tail = "",         .outModified = true, _ReqFloat_GotFloat },
        { .inStr = "23pqr",         .outFlt = 23,       .tail = "pqr",      .outModified = true, _ReqFloat_GotFloat },
        { .inStr = "23xyz",         .outFlt = 23,       .tail = "xyz",      .outModified = true, _ReqFloat_GotFloat },

        // To get e.g TAG = 23.4"
        { .inStr = ", = 23pqr",     .outFlt = 23.0,     .tail = "pqr",      .outModified = true, _ReqFloat_GotFloat, .delimiters = " ,=" },

        { .inStr = "000000450xyz",  .outFlt = 450,      .tail = "xyz",      .outModified = true, _ReqFloat_GotFloat },      // Bazillion leading zeros are OK.
        { .inStr = "-+456xyz",      .outFlt = 456,      .tail = "xyz",      .outModified = true, _ReqFloat_GotFloat },     // The '-' isn't next to number is is ignored.

        { .inStr = "  123xyz",     .outFlt = 123,       .tail = "xyz",      .outModified = true, _ReqFloat_GotFloat },      // Ignore leading spaces.

        { .inStr = " -234",         .outFlt = -234,     .tail = "",         .outModified = true, _ReqFloat_GotFloat },
        { .inStr = " +234",         .outFlt =  234,     .tail = "",         .outModified = true, _ReqFloat_GotFloat },

        { .inStr = "- 234",         .outFlt = 0,        .tail = NULL,       .outModified = false },   // '-' must be connected to number.

        // Where '+' or '-' are defined as delimiters. Must still see read the '-' directly in from of the number.
        { .inStr = "-+- -234abc",   .outFlt = -234.0,   .tail = "abc",      .outModified = true, _ReqFloat_GotFloat, .delimiters = " -+" },
        { .inStr = "+++ +234abc",   .outFlt = 234.0,    .tail = "abc",      .outModified = true, _ReqFloat_GotFloat, .delimiters = " -+" },

        // Fixed point
        { .inStr = " 12.345abc",    .outFlt =  12.345,   .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = "-12.345abc",    .outFlt =  -12.345,  .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },

        { .inStr = " 0.345abc",    .outFlt =  0.345,   .tail = "abc",       .outModified = true, _ReqFloat_GotFloat },
        { .inStr = "  .345abc",    .outFlt =  0.345,   .tail = "abc",       .outModified = true, _ReqFloat_GotFloat },

        // Incomplete fixed point; the trailing '.' is considered part of the number.
        { .inStr = " 12.abc",       .outFlt =  12.0,      .tail = "abc",    .outModified = true, _ReqFloat_GotFloat },

        // Float
        { .inStr = " 0.0abc",       .outFlt =  0.0,      .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = " 0.5abc",       .outFlt =  0.5,      .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = " 5.abc",        .outFlt =  5.0,      .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = "  .5abc",       .outFlt =  0.5,      .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = " 1.23E4abc",    .outFlt =  12300.0,  .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = " -5.67E1abc",   .outFlt =  -56.7,    .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = " 0.123E2abc",   .outFlt =  12.3,     .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        { .inStr = " 4.567E-5abc",  .outFlt =  4.567E-5, .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },
        // Also recognises 'e'
        { .inStr = " 1.23e4abc",    .outFlt =  12300.0,  .tail = "abc",     .outModified = true, _ReqFloat_GotFloat },

        // Incomplete float - just mantissa as int.
        { .inStr = " 1.23Eabc",     .outFlt =  1.23,     .tail = "Eabc",    .outModified = true, _ReqFloat_GotFloat },

        // Overrange -> INF
        { .inStr = " 1.23E999abc",     .outFlt =  1.23,     .tail = NULL,    .outModified = true, _ReqFloat_GotFloat },
        // Too many exponent digits.
        { .inStr = " 1.23E9999abc",     .outFlt =  1.23,     .tail = NULL,    .outModified = true, _ReqFloat_GotFloat },

        // Hex - are returned as integers even if float requested.
        { .inStr = "0x00 abc",      .outU32 = 0x0,       .tail = " abc",    .outModified = true, _ReqFloat_GaveHex },
        { .inStr = "0xFE abc",      .outU32 = 0xFE,      .tail = " abc",    .outModified = true, _ReqFloat_GaveHex },
        { .inStr = "0x12ACxyz",     .outU32 = 0x12AC,    .tail = "xyz",     .outModified = true, _ReqFloat_GaveHex },
        { .inStr = "0xFFFF",        .outU32 = 0xFFFF,    .tail = "",        .outModified = true, _ReqFloat_GaveHex },
        // Lowercase OK
        { .inStr = "0xabcdefpqr",   .outU32 = 0xabcdef,  .tail = "pqr",     .outModified = true, _ReqFloat_GaveHex },
        // 'X' is also hex.
        { .inStr = "0XFE abc",      .outU32 = 0xFE,      .tail = " abc",    .outModified = true, _ReqFloat_GaveHex },      // "0" -> 0, output at end of string.
        // Must be '0x..' or '0X..' Incomplete hex defaults to float if that was requested.
        { .inStr = "xFE abc",       .outFlt = 0.0,       .tail = NULL,      .outModified = false, _ReqFloat_GotFloat},

        // Just exponent symbol, no number ahead
        { .inStr = ".E5 abc",       .outFlt = 0.0,       .tail = NULL,      .outModified = false, _ReqFloat_GotFloat},
        { .inStr = "+E20 abc",      .outFlt = 0.0,       .tail = NULL,      .outModified = false, _ReqFloat_GotFloat},

        // ---- Do not request floating point conversion.

        // Give integers, get integers.
        { .inStr = "234abc",        .outS32 = 234,       .tail = "abc",    .outModified = true, _GotSignedInt },
        { .inStr = "-567abc",       .outS32 = -567,      .tail = "abc",    .outModified = true, _GotSignedInt },
        // Has the form of a float, even though it's a whole number -> is returned as a float.
        { .inStr = " 5.abc",        .outFlt =  5.0,      .tail = "abc",     .outModified = true, _GotFloat },
        { .inStr = " 5.0abc",        .outFlt =  5.0,      .tail = "abc",     .outModified = true, _GotFloat },
        // 'E' looked like a float but wasn't. Return as int.
        { .inStr = "789E",          .outS32 = 789,       .tail = "E",    .outModified = true, _GotSignedInt },

        // Give float, get float
        { .inStr = " 1.23E4abc",    .outFlt =  12300.0,  .tail = "abc",     .outModified = true, _GotFloat },

        // MAX_S32 -> return as signed (S32)...
        { .inStr = "2147483647abc", .outS32 = 0x7FFFFFFF, .tail = "abc",    .outModified = true, _GotSignedInt },
        // ... but MAX_S32+1; get it back as U32.
        { .inStr = "0xABCpqr",      .outU32 = 0xABC,      .tail = "pqr",    .outModified = true, _GotUnsignedInt },
        { .inStr = "2147483648abc", .outU32 = 0x80000000, .tail = "abc",    .outModified = true, _GotUnsignedInt },
        { .inStr = "4294967295abc", .outU32 = 0xFFFFFFFF, .tail = "abc",    .outModified = true, _GotUnsignedInt },
        { .inStr = "0xFFFFFFFFpqr", .outU32 = 0xFFFFFFFF, .tail = "pqr",    .outModified = true, _GotUnsignedInt },
        // MIN_S32
        { .inStr = "-2147483648abc", .outS32 = -2147483648, .tail = "abc",    .outModified = true, _GotSignedInt },
        // One less than min S32 -> fail
        { .inStr = "-2147483649abc", .outS32 = 0,         .tail = NULL,    .outModified = true, _GotSignedInt },

        // Too many hex digits for U32.
        { .inStr = "0x800000000pqr", .outU32 = 0,         .tail = NULL,    .outModified = true, _GotUnsignedInt },

    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];

        // If delimiters are specified for this test then use them; else will use default (SPC).
        if(t->delimiters != NULL) {
            Str_Delimiters = t->delimiters; }
        else {
            Str_Delimiters = NULL; }

        T_FloatOrInt out;

        // Prime was 'out's with some value other than the correct result. So can tell if was modified or no.
        float was    = t->outFlt + 1;
        S32   wasS32 = t->outS32 + 1;
        S32   wasU32 = t->outU32 + 1;

        out.reqFloat = t->reqFloat;
        out.num.asFloat = was;

        // Set output flags to opposite expected by test; can be sure they were written.
        #define _not(b)  (b) == false ? true : false
        out.gotInt = _not(t->gotInt);
        out.gotUnsigned = _not(t->gotUnsigned);


        C8 const * ret = ReadASCIIToNum(t->inStr, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr, ret)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_FLOAT_MESSAGE(was, out.num.asFloat, _msg("Output should not have been modified", t)); }
        else {
            // If function did NOT return NULL (fail) then proceed to check contents of 'out'. Otherwise don't
            // check because out is undefined.

            if(ret != NULL) {
               // If requested a float regardless, then ReadASCIIToNum() should not return an Int.
               if(out.reqFloat == true && t->gaveHex == false) {
                   TEST_ASSERT_FALSE_MESSAGE(out.gotInt, _msg("Requested float, but returned int", t)); }

               if(out.gotInt == true)
               {
                   // The test must agree that an int is correct.
                   TEST_ASSERT_TRUE_MESSAGE(t->gotInt, _msg("Expected float but got int", t));

                   if(out.gotUnsigned == true) {
                       TEST_ASSERT_TRUE_MESSAGE(t->gotUnsigned, _msg("Expected signed int but got unsigned", t));

                       if(out.num.asU32 == wasU32) {
                          TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
                       else {
                          TEST_ASSERT_EQUAL_UINT32_MESSAGE(t->outU32, out.num.asU32,  _msg("Incorrect output", t)); }}
                   else {
                       TEST_ASSERT_FALSE_MESSAGE(t->gotUnsigned, _msg("Expected unsigned int but got signed", t));

                       if(out.num.asS32 == wasS32) {
                          TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
                       else {
                          TEST_ASSERT_EQUAL_UINT32_MESSAGE(t->outS32, out.num.asS32,  _msg("Incorrect output", t)); }}
               }
               else {
                   TEST_ASSERT_FALSE_MESSAGE(t->gotInt, _msg("Expected int but got float", t));

                   if(out.num.asFloat == was) {
                      TEST_FAIL_MESSAGE(_msg("Expected result (in 'out') but 'out' was not modified", t)); }
                   else {
                      TEST_ASSERT_EQUAL_FLOAT_MESSAGE(t->outFlt, out.num.asFloat,  _msg("Incorrect output", t)); }
                   }}}

        // Check 'tail' is correct.
        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}
}

/* ---------------------------- test_ReadDirtyASCIIInt_ByCh ------------------------------------------- */

void test_ReadDirtyASCIIInt_ByCh(void)
{

}

/* ---------------------------- test_ReadTaggedASCIIInt ------------------------------------------- */

void test_ReadTaggedASCIIInt(void)
{
    typedef struct { C8 const *inStr; C8 const *tag; C8 const *delimiters; S32 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        // Match a tag.
        { .inStr = "ABC 123 def",       .tag = "ABC",   .result = 123,     .tail = " def",     .outModified = true },
        { .inStr = "ABC -789 def",      .tag = "ABC",   .result = -789,    .tail = " def",     .outModified = true },
        { .inStr = "TAG 0xABC def",     .tag = "TAG",   .result = 0xABC,   .tail = " def",     .outModified = true },
        // Tag is e.g the 2nd word, rather than the 1st.
        { .inStr = "ABC DEF 123 ghi",   .tag = "DEF",   .result = 123,  .tail = " ghi",     .outModified = true },
        // Tail may be glued to number.
        { .inStr = "ABC 123def",        .tag = "ABC",   .result = 123,  .tail = "def",     .outModified = true },

        // With additional delimiters.
        { .inStr = "Var := 123 def",     .tag = "Var",   .result = 123,   .tail = " def", .outModified = true, .delimiters = " =:" },
        // Fails.

        // Empty string, empty tag -> return NULL, output unchanged.
        { .inStr = "",                  .tag = "",      .result = 0,    .tail = NULL,       .outModified = false },
        // Empty string, legal tag
        { .inStr = "",                  .tag = "ABC",   .result = 0,    .tail = NULL,       .outModified = false },
        // Non-empty string, but no tag.
        { .inStr = "ABC 123 def",       .tag = "",      .result = 0,    .tail = NULL,       .outModified = false },
        // String without a number.
        { .inStr = "ABC def",           .tag = "ABC",   .result = 0,    .tail = NULL,       .outModified = false },
        // Number must be separated from tag.
        { .inStr = "ABC123 def",        .tag = "ABC",   .result = 0,    .tail = NULL,       .outModified = false },
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];
        S32 was = t->result + 2;
        S32 out = was;
        C8 const * ret = ReadTaggedASCIIInt(t->inStr, t->tag, &out, t->delimiters);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr, ret)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_INT32_MESSAGE(was, out, _msg("Output should not have been modified", t)); }
        else
            { TEST_ASSERT_EQUAL_INT32_MESSAGE(t->result, out,  _msg("Incorrect output", t)); }

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}
}

// ----------------------------------------- eof --------------------------------------------
