#include "unity.h"
#include "arith.h"
#include <stdlib.h>
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

/* ------------------------------------------------------------------------------------------- */
PRIVATE C8 const *catRtn(C8 *out, U8 testNum, C8 const *preamble, C8 const *inStr) {
    sprintf(out, "tst #%d. %s. testStr = \"%s\"", testNum, preamble, inStr == NULL ? "NULL" : inStr);
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
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr)

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

/* --------------------------------- test_ReadASCIIToFloat ----------------------------------------- */

void test_ReadASCIIToFloat(void)
{
    typedef struct { C8 const *inStr; float result; C8    const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        // No number
        { .inStr = "",              .result = 0,      .tail = NULL,       .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc",           .result = 0,      .tail = NULL,       .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",          .result = 0,      .tail = NULL,       .outModified = false },    // A lone '-' is not a number.

        // Integers
        { .inStr = "0",             .result = 0.0,      .tail = "",         .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "-0",            .result = 0.0,      .tail = "",         .outModified = true },      // "0" -> 0, output at end of string.
        { .inStr = "23",            .result = 23.0,     .tail = "",         .outModified = true },
        { .inStr = "234",           .result = 234,      .tail = "",         .outModified = true },
        { .inStr = "23pqr",         .result = 23,       .tail = "pqr",      .outModified = true },
        { .inStr = "23xyz",         .result = 23,       .tail = "xyz",      .outModified = true },

        { .inStr = "000000450xyz",  .result = 450,       .tail = "xyz",      .outModified = true },      // Bazillion leading zeros are OK.
        { .inStr = "-+456xyz",      .result = 456,       .tail = "xyz",      .outModified = true },     // The '-' isn't next to number is is ignored.

        { .inStr = "  123xyz",     .result = 123,        .tail = "xyz",      .outModified = true },      // Ignore leading spaces.

        { .inStr = " -234",         .result = -234,      .tail = "",         .outModified = true },
        { .inStr = " +234",         .result =  234,      .tail = "",         .outModified = true },

        { .inStr = "- 234",         .result = 0,         .tail = NULL,         .outModified = false },   // '-' must be connected to number.

        // Fixed point
        { .inStr = " 12.345abc",    .result =  12.345,   .tail = "abc",         .outModified = true },
        { .inStr = "-12.345abc",    .result =  -12.345,  .tail = "abc",         .outModified = true },

        { .inStr = " 0.345abc",    .result =  0.345,   .tail = "abc",         .outModified = true },
        { .inStr = "  .345abc",    .result =  0.345,   .tail = "abc",         .outModified = true },

        // Incomplete fixed point; the trailing '.' is considered part of the number.
        { .inStr = " 12.abc",       .result =  12.0,      .tail = "abc",         .outModified = true },

        // Float
        { .inStr = " 1.23E4abc",    .result =  12300.0,  .tail = "abc",         .outModified = true },
        { .inStr = " -5.67E1abc",   .result =  -56.7,    .tail = "abc",         .outModified = true },
        { .inStr = " 0.123E2abc",   .result =  12.3,     .tail = "abc",         .outModified = true },
        { .inStr = " 4.567E-5abc",  .result =  4.567E-5, .tail = "abc",         .outModified = true },
        // Also recognises 'e'
        { .inStr = " 1.23e4abc",    .result =  12300.0,  .tail = "abc",         .outModified = true },

        // Incomplete float - just mantissa as int.
        { .inStr = " 1.23Eabc",     .result =  1.23,     .tail = "Eabc",         .outModified = true },

        // Hex
        { .inStr = "0x00 abc",      .result = 0.0,       .tail = " abc",         .outModified = true },
        { .inStr = "0xFE abc",      .result = 254.0,     .tail = " abc",         .outModified = true },
        { .inStr = "0x12ACxyz",     .result = 4780.0,    .tail = "xyz",         .outModified = true },
        { .inStr = "0xFFFF",        .result = 65535.0,   .tail = "",         .outModified = true },
        // 'X' is also hex.
        { .inStr = "0XFE abc",      .result = 254.0,     .tail = " abc",         .outModified = true },      // "0" -> 0, output at end of string.
        // Must be '0x..' or '0X..'
        { .inStr = "xFE abc",       .result = 0,          .tail = NULL,       .outModified = false },     // No number -> return NULL, output unchanged.
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];

        // Prime 'out' with some value other than the correct result. So can tell if was modified or no.
        float was = t->result + 1;
        float out = was;
        C8 const * ret = ReadASCIIToFloat(t->inStr, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr)

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

/* ---------------------------- test_ReadDirtyASCIIInt_ByCh ------------------------------------------- */

void test_ReadDirtyASCIIInt_ByCh(void)
{

}

/* ---------------------------- test_ReadTaggedASCIIInt ------------------------------------------- */

void test_ReadTaggedASCIIInt(void)
{
    typedef struct { C8 const *inStr; C8 const *tag; S16 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        // Match a tag.
        { .inStr = "ABC 123 def",       .tag = "ABC",   .result = 123,  .tail = " def",     .outModified = true },
        // Get 1st number after, bypassing intervening words.
        { .inStr = "ABC def 123 ghi",   .tag = "ABC",   .result = 123,  .tail = " ghi",     .outModified = true },
        // Tag is e.g the 2nd word, rather than the 1st.
        { .inStr = "ABC DEF 123 ghi",   .tag = "DEF",   .result = 123,  .tail = " ghi",     .outModified = true },
        // Tail may be glued to number.
        { .inStr = "ABC 123def",        .tag = "ABC",   .result = 123,  .tail = "def",     .outModified = true },

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
        S16 was = t->result + 1;
        S16 out = was;
        C8 const * ret = ReadTaggedASCIIInt(t->inStr, t->tag, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, i, (msg), (tst)->inStr)

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_INT16_MESSAGE(was, out, _msg("Output should not have been modified", t)); }
        else
            { TEST_ASSERT_EQUAL_INT16_MESSAGE(t->result, out,  _msg("Incorrect output", t)); }

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE( _msg("Return should been NULL but wasn't",t)); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE(_msg("NULL return (wrong)",t)); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, _msg("Wrong tail", t)); }}
}


// ----------------------------------------- eof --------------------------------------------
