#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include "tiny1_stdio.h"
#include <string.h>
#include "tiny1_printf_test_support.h"

/* =============================== Tests start here ==================================

   test-tiny1_printf_float.c tests all the formats. Here we check just sprintf() specific
   cases.
*/


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* --------------------------------- test_sprintf_empty --------------------------------------- */

void test_sprintf_empty(void)
{
   C8 b0[100];
   strcpy(b0, "aaaaaaaaaaaaaaaaaaa");        // To make sure the sprintf() is correctly terminated prefill b0[].
   tiny1_sprintf(b0, "");
   TEST_ASSERT_EQUAL_STRING(b0, "");
}

/* --------------------------------- test_sprintf_basic --------------------------------------- */

void test_sprintf_basic(void)
{
   C8 b0[100];
   strcpy(b0, "aaaaaaaaaaaaaaaaaaa");           // To make sure the sprintf() is correctly terminated prefill b0[].
   tiny1_sprintf(b0, "%02d:%02d:%02d", 12, 34, 56);
   TEST_ASSERT_EQUAL_STRING(b0, "12:34:56");
}


// ----------------------------------------- eof --------------------------------------------
