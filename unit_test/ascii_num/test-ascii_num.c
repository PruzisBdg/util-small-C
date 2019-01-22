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

/* ---------------------------- test_ReadDirtyASCIIInt ------------------------------------------- */

void test_ReadDirtyASCIIInt(void)
{
    typedef struct { C8 const *inStr; S16 result; C8 const *tail; bool outModified; } S_Tst;

    S_Tst const tsts[] = {
        { .inStr = "",              .result = 0,    .tail = NULL, .outModified = false },       // Empty string -> return NULL, output unchanged.
        { .inStr = "abc dr()",      .result = 0,    .tail = NULL, .outModified = false },       // No number -> return NULL, output unchanged.
    };

    U8 i; for(i = 0; i < RECORDS_IN(tsts); i++) {
        S_Tst const *t = &tsts[i];
        S16 was = t->result + 1;
        S16 out = was;
        C8 const * ret = ReadDirtyASCIIInt(t->inStr, &out);

        if(t->outModified == false)
            { TEST_ASSERT_EQUAL_INT16_MESSAGE(was, out, "Output should not have been modified"); }
        else
            { TEST_ASSERT_EQUAL_INT16_MESSAGE(t->result, out, "Incorrect output"); }

        if(t->tail == NULL && ret != NULL)
            { TEST_FAIL_MESSAGE("Return should been NULL but wasn't"); }
        else if(t->tail != NULL && ret == NULL)
            { TEST_FAIL_MESSAGE("NULL return (wrong)"); }
        else
            { TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, ret, "Wrong tail"); }}
}

/* ---------------------------- test_ReadDirtyASCIIInt_ByCh ------------------------------------------- */

void test_ReadDirtyASCIIInt_ByCh(void)
{

}


// ----------------------------------------- eof --------------------------------------------
