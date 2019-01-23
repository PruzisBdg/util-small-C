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
PRIVATE C8 const *catRtn(C8 *out, C8 const *preamble, C8 const *inStr) {
    sprintf(out, "%s. testStr = \"%s\"", preamble, inStr == NULL ? "NULL" : inStr);
    return out; }

/* ---------------------------- test_ReadDirtyASCIIInt ------------------------------------------- */

void test_ReadDirtyASCIIInt(void)
{
    typedef struct { C8 const *inStr; S16 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        { .inStr = "",              .result = 0,    .tail = NULL,       .outModified = false },     // Empty string -> return NULL, output unchanged.
        { .inStr = "abc dr()",      .result = 0,    .tail = NULL,       .outModified = false },     // No number -> return NULL, output unchanged.
        { .inStr = "-abc",             .result = 0,    .tail = NULL,     .outModified = false },    // A lone '-' is not a number.

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
        S16 was = t->result + 1;
        S16 out = was;
        C8 const * ret = ReadDirtyASCIIInt(t->inStr, &out);

        C8 b0[100];
        #define _msg(msg, tst)  catRtn(b0, (msg), (tst)->inStr)

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

/* ---------------------------- test_ReadDirtyASCIIInt_ByCh ------------------------------------------- */

void test_ReadDirtyASCIIInt_ByCh(void)
{

}


// ----------------------------------------- eof --------------------------------------------
