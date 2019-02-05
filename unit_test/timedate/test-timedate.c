#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include "wordlist.h"
#include <string.h>

// These are by-element compares; which will handle differing alignments and packing.
PRIVATE bool dtsEqual(S_DateTime const *a, S_DateTime const *b) {
   return
      (a->yr == b->yr && a->mnth == b->mnth && a->day == b->day && a->hr == b->hr && a->min == b->min && a->sec == b->sec)
         ? true
         : false; }

//
PRIVATE bool dtsA_GTE_B(S_DateTime const *a, S_DateTime const *b) {
   return
      (a->yr > b->yr) ||
      (a->yr == b->yr && a->mnth > b->mnth) ||
      (a->yr == b->yr && a->mnth == b->mnth && a->day > b->day) ||
      (a->yr == b->yr && a->mnth == b->mnth && a->day == b->day && a->hr > b->hr) ||
      (a->yr == b->yr && a->mnth == b->mnth && a->day == b->day && a->hr == b->hr && a->min > b->min) ||
      (a->yr == b->yr && a->mnth == b->mnth && a->day == b->day && a->hr == b->hr && a->min == b->min && a->sec >= b->sec)
         ? true
         : false; }

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
      { .dt = {.yr = 1999, .mnth = 12, .day = 31, .hr = 23, .min = 59, .sec = 59 }, .res = false },
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
      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->res, rtn, b0);// This is a by-element compare; which will handle differing alignments and packing.
   }
}


/* -------------------------------------- test_SecsToYMDHMS -------------------------------------- */

void test_SecsToYMDHMS(void)
{
   typedef enum { eEq=0, eGTE, eLTE } E_Compare;
   typedef struct {  T_Seconds32 utc; S_DateTime dt; E_Compare compare; } S_Tst;

   S_Tst const tsts[] = {
      // Zero -> midnite of millennium; Max time must be > 2135.
      { .utc = 0,          .dt = {.yr = 2000, .mnth = 1,  .day = 1,  .hr = 0,  .min = 0,  .sec = 0 } },
      { .utc = MAX_U32,    .dt = {.yr = 2135, .mnth = 1,  .day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .compare = eGTE },

      // Random times from UTC calculator. Enough span to capture all the internal arithmetic, leap years etc.
      // Some allowance for leap-seconds
      { .utc = 0x25BEC970, .dt = {.yr = 2020, .mnth = 1,  .day = 25,  .hr = 9,  .min = 40,  .sec = 0 } },

      // SecsToYMDHMS(0x376ADD7E) -> 23:59:58  UTC -> 23:59:59
      // SecsToYMDHMS() does not do leap-seconds; so allow a window.
      { .utc = 0x376ADD7E, .dt = {.yr = 2029, .mnth = 6,  .day = 17,  .hr = 23, .min = 59,  .sec = 58 }, .compare = eGTE },
      { .utc = 0x376ADD7E, .dt = {.yr = 2029, .mnth = 6,  .day = 18,  .hr = 0,  .min = 0,   .sec = 1  }, .compare = eLTE },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_DateTime out;
      SecsToYMDHMS(t->utc, &out);

      if( (t->compare == eEq  && dtsEqual(&t->dt, &out) == false) ||
          (t->compare == eGTE && dtsA_GTE_B(&out, &t->dt) == false) ||
          (t->compare == eLTE && dtsA_GTE_B(&t->dt, &out) == false))
      {
         U8 expected[_ISO8601_YMDHMS_MaxStr+1];
         U8 got[_ISO8601_YMDHMS_MaxStr+1];
         YMDHMS_ToStr(&t->dt, expected);
         YMDHMS_ToStr(&out, got);

         C8 b0[100];
         sprintf(b0, "%s -> %s", expected, got);
         TEST_FAIL_MESSAGE(b0);
      }
   }
}

/* ------------------------------------- test_YMDHMS_To_Secs ------------------------------------------ */

void test_YMDHMS_To_Secs(void)
{
   typedef struct { S_DateTime dt; T_Seconds32 utcLo, utcHi;  } S_Tst;

   S_Tst const tsts[] = {
      // Zero -> midnite of millennium; Max time must be > 2135.
      { .dt = {.yr = 2000, .mnth = 1,  .day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },

      { .dt = {.yr = 2000, .mnth = 1,  .day = 1,  .hr = 0,  .min = 1,  .sec = 0  }, .utcLo = 0x3C,       .utcHi = 0x3C       },
      { .dt = {.yr = 2000, .mnth = 1,  .day = 1,  .hr = 1,  .min = 0,  .sec = 0  }, .utcLo = 0xE10,      .utcHi = 0xE10      },
      { .dt = {.yr = 2000, .mnth = 1,  .day = 1,  .hr = 8,  .min = 1,  .sec = 0  }, .utcLo = 0x000070BC, .utcHi = 0x000070BC },
      { .dt = {.yr = 2001, .mnth = 3,  .day = 5,  .hr = 23, .min = 1,  .sec = 59 }, .utcLo = 0x0236D767, .utcHi = 0x0236D767 },
      { .dt = {.yr = 2011, .mnth = 5,  .day = 31, .hr = 23, .min = 59, .sec = 59 }, .utcLo = 0x15783D7E, .utcHi = 0x15783D7F },

      // Feb 29th leap year
      { .dt = {.yr = 2024, .mnth = 2,  .day = 29, .hr = 8,  .min = 1,  .sec = 0 }, .utcLo = 0x2D72F63B, .utcHi = 0x2D72F63C },

      // Illegal time dates return 0. Bogus year, month, day, hour, minute second.
      { .dt = {.yr = 1999, .mnth = 1,  .day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },   // Party like it's 1999!
      { .dt = {.yr = 2199, .mnth = 1,  .day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 13, .day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 1,  .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 1,  .day = 1,  .hr = 24, .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 1,  .day = 1,  .hr = 0,  .min = 60, .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 1,  .day = 1,  .hr = 0,  .min = 0,  .sec = 60}, .utcLo = 0, .utcHi = 0 },


      // Jan -> Dec, but 1 day too many. Should all fail -> 0
      { .dt = {.yr = 2010, .mnth = 1,  .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2009, .mnth = 2,  .day = 29, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },   // Feb 29th but not a leap year.
      { .dt = {.yr = 2012, .mnth = 2,  .day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 3,  .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 4,  .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 5,  .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 6,  .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 7,  .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 8,  .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 9,  .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 10, .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 11, .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {.yr = 2010, .mnth = 12, .day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },

      // Last day of each month; should pass i.e non-zero.
      { .dt = {.yr = 2010, .mnth = 1,  .day = 31, .hr = 23, .min = 59, .sec = 59}, .utcLo = 0x12F8D5FF, .utcHi = 0x12F8D5FF },
      { .dt = {.yr = 2009, .mnth = 2,  .day = 28, .hr = 0,  .min = 12, .sec = 40}, .utcLo = 0x113B3DF8, .utcHi = 0x113B3DF8 },
      { .dt = {.yr = 2012, .mnth = 2,  .day = 29, .hr = 14, .min = 9,  .sec = 21}, .utcLo = 0x16E0EE11, .utcHi = 0x16E0EE11 },
      { .dt = {.yr = 2084, .mnth = 3,  .day = 31, .hr = 7,  .min = 2,  .sec = 48}, .utcLo = 0x9E779F98, .utcHi = 0x9E779F98 },
      { .dt = {.yr = 2010, .mnth = 4,  .day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 5,  .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 6,  .day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 7,  .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 8,  .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 9,  .day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 10, .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 11, .day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {.yr = 2010, .mnth = 12, .day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      T_Seconds32 res = YMDHMS_To_Secs(&t->dt);

      if( res < t->utcLo || res > t->utcHi )
      {
         U8 expected[_ISO8601_YMDHMS_MaxStr+1];
         YMDHMS_ToStr(&t->dt, expected);

         U8 got[_ISO8601_YMDHMS_MaxStr+1];
         SecsTo_YMDHMS_Str(got, res);

         C8 b0[100];
         sprintf(b0, "%s -> 0x%lx/%lud (%s) [lims 0x%lx -> 0x%lx]", expected, res, res, got, t->utcLo, t->utcHi);
         TEST_FAIL_MESSAGE(b0);
      }
   }
}

// ----------------------------------------- eof --------------------------------------------
