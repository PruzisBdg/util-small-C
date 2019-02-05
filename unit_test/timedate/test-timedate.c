#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include "wordlist.h"
#include <string.h>

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* ---------------------------------- test_Legal_YMDHMS -------------------------------------- */


void test_Legal_YMDHMS(void)
{
   typedef struct { S_DateTime dt; bool res; } S_Tst;

   S_Tst const tsts[] = {
      { .dt = {.yr = 0, .mnth = 0, .day = 0, .hr = 0, .min = 0, .sec = 0 }, .res = false },

      // Legal spans 2000 - 2132.
      { .dt = {.yr = 2000, .mnth = 1,  .day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .res = true },
      { .dt = {.yr = 2135, .mnth = 12, .day = 31, .hr = 23, .min = 59, .sec = 59 }, .res = true },

      // Illegal thisn'that.
      { .dt = {.yr = 2136, .mnth = 12, .day = 31, .hr = 23, .min = 59, .sec = 59 }, .res = false },
      { .dt = {.yr = 1999, .mnth = 12, .day = 31, .hr = 23, .min = 59, .sec = 59 }, .res = false },
      { .dt = {.yr = 2020, .mnth = 13, .day = 1,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      // 30 days hath December, etc.
      { .dt = {.yr = 2020, .mnth = 1, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 1, .day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2019, .mnth = 2, .day = 28,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2019, .mnth = 2, .day = 29,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },
      // Leap year
      { .dt = {.yr = 2020, .mnth = 2, .day = 29,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 2, .day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 3, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 3, .day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 4, .day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 4, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 5, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 5, .day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 6, .day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 6, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 7, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 7, .day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 8, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 8, .day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 9, .day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 9, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 10, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 10, .day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 11, .day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 11, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {.yr = 2020, .mnth = 12, .day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {.yr = 2020, .mnth = 12, .day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      // Bad HMS
      { .dt = {.yr = 2020, .mnth = 12, .day = 32,  .hr = 13,  .min = 0,  .sec = 0  }, .res = false },
      { .dt = {.yr = 2020, .mnth = 12, .day = 32,  .hr = 0,   .min = 60, .sec = 0  }, .res = false },
      { .dt = {.yr = 2020, .mnth = 12, .day = 32,  .hr = 0,   .min = 0,  .sec = 60 }, .res = false },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = Legal_YMDHMS(&t->dt);

      U8 dtStr[_ISO8601_YMDHMS_MaxStr+1];
      YMDHMS_ToStr(&t->dt, dtStr);
      C8 b0[100];
      sprintf(b0, "%s -> %d", dtStr, rtn);
      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->res, rtn, b0);
   }
}
// ----------------------------------------- eof --------------------------------------------
