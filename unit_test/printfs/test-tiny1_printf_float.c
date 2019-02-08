#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include "tiny1_stdio.h"
#include <string.h>
#include "tiny1_printf_test_support.h"

PRIVATE void chkStr(C8 const *str, T_PrintCnt cnt)
{
   OStream_Reset();
   T_PrintCnt rtn = tiny1_printf(str);
   TEST_ASSERT_EQUAL_STRING(str, OStream_Get());
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
   typedef struct { C8 const *str; T_PrintCnt cnt; } S_Tst;

   S_Tst const tsts[] = {
      { .str = "", .cnt = 0 },                                                // Empty
      { .str = "My 1st string\r\n", .cnt = sizeof("My 1st string\r\n")-1 },   // Return is number of chars written
      //{ .str = "\\a\r\n", .cnt = sizeof("\\a\r\n")-1 },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++) {
      S_Tst const *t = &tsts[i];
      chkStr(t->str, t->cnt);
   }
}

// ----------------------------------------- eof --------------------------------------------
