#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include "wordlist.h"
#include <string.h>

// Shorthand for S_YMD inside S_DateTime
#define _yr    ymd.yr
#define _mnth  ymd.mnth
#define _day   ymd.day

// These are by-element compares; which will handle differing alignments and packing.
PRIVATE bool dtsEqual(S_DateTime const *a, S_DateTime const *b) {
   return
      (a->_yr == b->_yr && a->_mnth == b->_mnth && a->_day == b->_day && a->hr == b->hr && a->min == b->min && a->sec == b->sec)
         ? true
         : false; }

//
PRIVATE bool dtsA_GTE_B(S_DateTime const *a, S_DateTime const *b) {
   return
      (a->_yr > b->_yr) ||
      (a->_yr == b->_yr && a->_mnth > b->_mnth) ||
      (a->_yr == b->_yr && a->_mnth == b->_mnth && a->_day > b->_day) ||
      (a->_yr == b->_yr && a->_mnth == b->_mnth && a->_day == b->_day && a->hr > b->hr) ||
      (a->_yr == b->_yr && a->_mnth == b->_mnth && a->_day == b->_day && a->hr == b->hr && a->min > b->min) ||
      (a->_yr == b->_yr && a->_mnth == b->_mnth && a->_day == b->_day && a->hr == b->hr && a->min == b->min && a->sec >= b->sec)
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
      { .dt = {._yr =0, ._mnth = 0, ._day = 0, .hr = 0, .min = 0, .sec = 0 }, .res = false },

      // Legal spans 2000 - 2136.
      { .dt = {._yr =2000, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .res = true },
      { .dt = {._yr =2136, ._mnth = 2,  ._day = 7,  .hr = 6,  .min = 28, .sec = 15 }, .res = true },

      // Illegal thisn'that.
      { .dt = {._yr =1999, ._mnth = 12, ._day = 31, .hr = 23, .min = 59, .sec = 59 }, .res = false },
      { .dt = {._yr =2137, ._mnth = 12, ._day = 31, .hr = 23, .min = 59, .sec = 59 }, .res = false },
      { .dt = {._yr =1999, ._mnth = 12, ._day = 31, .hr = 23, .min = 59, .sec = 59 }, .res = false },
      { .dt = {._yr =2020, ._mnth = 13, ._day = 1,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      // 30 days hath December, etc.
      { .dt = {._yr =2020, ._mnth = 1, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 1, ._day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2019, ._mnth = 2, ._day = 28,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2019, ._mnth = 2, ._day = 29,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },
      // Leap year
      { .dt = {._yr =2020, ._mnth = 2, ._day = 29,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 2, ._day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 3, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 3, ._day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 4, ._day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 4, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 5, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 5, ._day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 6, ._day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 6, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 7, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 7, ._day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 8, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 8, ._day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 9, ._day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 9, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 10, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 10, ._day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 11, ._day = 30,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 11, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      { .dt = {._yr =2020, ._mnth = 12, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0  }, .res = true },
      { .dt = {._yr =2020, ._mnth = 12, ._day = 32,  .hr = 0,  .min = 0,  .sec = 0  }, .res = false },

      // Bad HMS
      { .dt = {._yr =2020, ._mnth = 12, ._day = 32,  .hr = 13,  .min = 0,  .sec = 0  }, .res = false },
      { .dt = {._yr =2020, ._mnth = 12, ._day = 32,  .hr = 0,   .min = 60, .sec = 0  }, .res = false },
      { .dt = {._yr =2020, ._mnth = 12, ._day = 32,  .hr = 0,   .min = 0,  .sec = 60 }, .res = false },
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


/* -------------------------------------- test_SecsToYMDHMS -------------------------------------- */

void test_SecsToYMDHMS(void)
{
   typedef enum { eEq=0, eGTE, eLTE } E_Compare;
   typedef struct {  T_Seconds32 utc; S_DateTime dt; E_Compare compare; } S_Tst;

   S_Tst const tsts[] = {
      // Zero -> midnite of millennium; Max time must be > 2135.
      { .utc = 0,          .dt = {._yr =2000, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 0,  .sec = 0 } },
      { .utc = MAX_U32,    .dt = {._yr =2135, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .compare = eGTE },
      { .utc = MAX_U32,    .dt = {._yr =2136, ._mnth = 2,  ._day = 7,  .hr = 6,  .min = 28,  .sec = 15 }},

//      { .utc = MAX_U32-(365*24*3600),    .dt = {._yr =2135, ._mnth = 2,  ._day = 7,  .hr = 6,  .min = 28,  .sec = 15 }},

      // Random times from UTC calculator. Enough span to capture all the internal arithmetic, leap years etc.
      // Some allowance for leap-seconds
      { .utc = 0x25BEC970, .dt = {._yr =2020, ._mnth = 1,  ._day = 25,  .hr = 9,  .min = 40,  .sec = 0 } },
      { .utc = 0x49B3C970, .dt = {._yr =2039, ._mnth = 3,  ._day = 8,   .hr = 13, .min = 34,  .sec = 40 } },
      { .utc = 0xABCD1234, .dt = {._yr =2091, ._mnth = 5,  ._day = 3,   .hr = 10, .min = 57,  .sec = 56 } },
      { .utc = 0xFE9623B1, .dt = {._yr =2135, ._mnth = 5,  ._day = 8,   .hr = 19, .min = 00,  .sec = 01 } },

      // SecsToYMDHMS(0x376ADD7E) -> 23:59:58  UTC -> 23:59:59
      // SecsToYMDHMS() does not do leap-seconds; so allow a window.
      { .utc = 0x376ADD7E, .dt = {._yr =2029, ._mnth = 6,  ._day = 17,  .hr = 23, .min = 59,  .sec = 58 }, .compare = eGTE },
      { .utc = 0x376ADD7E, .dt = {._yr =2029, ._mnth = 6,  ._day = 18,  .hr = 0,  .min = 0,   .sec = 1  }, .compare = eLTE },
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
         sprintf(b0, "expected %lu -> %s, got %s", t->utc, expected, got);
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
      { .dt = {._yr =2000, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      //{ .dt = {._yr =2135, ._mnth = 12,  ._day = 31,  .hr = 23,  .min = 59, .sec = 59}, .utcLo = 0xFFFFFFFF, .utcHi = 0xFFFFFFFF },
      { .dt = {._yr =2136, ._mnth = 2,  ._day = 7,  .hr = 6,  .min = 28, .sec = 15}, .utcLo = 0xFFFFFFFF, .utcHi = 0xFFFFFFFF },

      { .dt = {._yr =2000, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 1,  .sec = 0  }, .utcLo = 0x3C,       .utcHi = 0x3C       },
      { .dt = {._yr =2000, ._mnth = 1,  ._day = 1,  .hr = 1,  .min = 0,  .sec = 0  }, .utcLo = 0xE10,      .utcHi = 0xE10      },
      { .dt = {._yr =2000, ._mnth = 1,  ._day = 1,  .hr = 8,  .min = 1,  .sec = 0  }, .utcLo = 0x000070BC, .utcHi = 0x000070BC },
      { .dt = {._yr =2001, ._mnth = 3,  ._day = 5,  .hr = 23, .min = 1,  .sec = 59 }, .utcLo = 0x0236D767, .utcHi = 0x0236D767 },
      { .dt = {._yr =2011, ._mnth = 5,  ._day = 31, .hr = 23, .min = 59, .sec = 59 }, .utcLo = 0x15783D7E, .utcHi = 0x15783D7F },

      // Feb 29th leap year
      { .dt = {._yr =2024, ._mnth = 2,  ._day = 29, .hr = 8,  .min = 1,  .sec = 0 }, .utcLo = 0x2D72F63B, .utcHi = 0x2D72F63C },

      // Illegal time dates return 0. Bogus year, month, day, hour, minute second.
      { .dt = {._yr =1999, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },   // Party like it's 1999!
      { .dt = {._yr =2199, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 13, ._day = 1,  .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 1,  ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 1,  ._day = 1,  .hr = 24, .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 60, .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 1,  ._day = 1,  .hr = 0,  .min = 0,  .sec = 60}, .utcLo = 0, .utcHi = 0 },


      // Jan -> Dec, but 1 day too many. Should all fail -> 0
      { .dt = {._yr =2010, ._mnth = 1,  ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2009, ._mnth = 2,  ._day = 29, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },   // Feb 29th but not a leap year.
      { .dt = {._yr =2012, ._mnth = 2,  ._day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 3,  ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 4,  ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 5,  ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 6,  ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 7,  ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 8,  ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 9,  ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 10, ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 11, ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },
      { .dt = {._yr =2010, ._mnth = 12, ._day = 32, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 0, .utcHi = 0 },

      // Last day of each month; should pass i.e non-zero.
      { .dt = {._yr =2010, ._mnth = 1,  ._day = 31, .hr = 23, .min = 59, .sec = 59}, .utcLo = 0x12F8D5FF, .utcHi = 0x12F8D5FF },
      { .dt = {._yr =2009, ._mnth = 2,  ._day = 28, .hr = 0,  .min = 12, .sec = 40}, .utcLo = 0x113B3DF8, .utcHi = 0x113B3DF8 },
      { .dt = {._yr =2012, ._mnth = 2,  ._day = 29, .hr = 14, .min = 9,  .sec = 21}, .utcLo = 0x16E0EE11, .utcHi = 0x16E0EE11 },
      { .dt = {._yr =2084, ._mnth = 3,  ._day = 31, .hr = 7,  .min = 2,  .sec = 48}, .utcLo = 0x9E779F98, .utcHi = 0x9E779F98 },
      { .dt = {._yr =2010, ._mnth = 4,  ._day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 5,  ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 6,  ._day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 7,  ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 8,  ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 9,  ._day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 10, ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 11, ._day = 30, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
      { .dt = {._yr =2010, ._mnth = 12, ._day = 31, .hr = 0,  .min = 0,  .sec = 0 }, .utcLo = 1, .utcHi = MAX_U32 },
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

/* ----------------------------------- test_LegalYMD ------------------------------------------- */

void test_LegalYMD(void)
{
   typedef struct { S_YMD ymd; bool res; } S_Tst;

   S_Tst const tsts[] = {
      { .ymd = {.yr =0, .mnth = 0, .day = 0 }, .res = false },

      // Legal spans 2000 - 2136.
      { .ymd = {.yr =2000, .mnth = 1,  .day = 1  }, .res = true },
      { .ymd = {.yr =2136, .mnth = 2,  .day = 7 }, .res = true },

      // Illegal thisn'that.
      { .ymd = {.yr =1999, .mnth = 12, .day = 31 }, .res = false },
      { .ymd = {.yr =2136, .mnth = 2,  .day = 8  }, .res = false },
      { .ymd = {.yr =1999, .mnth = 12, .day = 31 }, .res = false },
      { .ymd = {.yr =2020, .mnth = 13, .day = 1  }, .res = false },

      // 30 days hath December, etc.
      { .ymd = {.yr =2020, .mnth = 1, .day = 31  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 1, .day = 32  }, .res = false },

      { .ymd = {.yr =2019, .mnth = 2, .day = 28  }, .res = true },
      { .ymd = {.yr =2019, .mnth = 2, .day = 29  }, .res = false },
      // Leap year
      { .ymd = {.yr =2020, .mnth = 2, .day = 29  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 2, .day = 30  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 3, .day = 31  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 3, .day = 32  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 4, .day = 30  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 4, .day = 31  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 5, .day = 31  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 5, .day = 32  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 6, .day = 30  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 6, .day = 31  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 7, .day = 31  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 7, .day = 32  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 8, .day = 31  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 8, .day = 32  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 9, .day = 30  }, .res = true },
      { .ymd = {.yr =2020, .mnth = 9, .day = 31  }, .res = false },

      { .ymd = {.yr =2020, .mnth = 10, .day = 31 }, .res = true },
      { .ymd = {.yr =2020, .mnth = 10, .day = 32 }, .res = false },

      { .ymd = {.yr =2020, .mnth = 11, .day = 30 }, .res = true },
      { .ymd = {.yr =2020, .mnth = 11, .day = 31 }, .res = false },

      { .ymd = {.yr =2020, .mnth = 12, .day = 31 }, .res = true },
      { .ymd = {.yr =2020, .mnth = 12, .day = 32 }, .res = false },

      // Wildcards. _YMD_AnyYear = 0xFEFE; _YMD_AnyMnth/Day... = 0xFE.
      { .ymd = {.yr =0xFEFE, .mnth = 1,    .day = 1  }, .res = true },
      { .ymd = {.yr =2001, .mnth = 0xFE, .day = 31  }, .res = true },     // 31st is legal for a wildcard month
      { .ymd = {.yr =2001, .mnth = 0xFE, .day = 0   }, .res = false },     // But not these...
      { .ymd = {.yr =2001, .mnth = 0xFE, .day = 32  }, .res = false },
      { .ymd = {.yr =2001, .mnth = 0xFE, .day = 0xFE}, .res = true },
      { .ymd = {.yr =2099, .mnth = 1,    .day = 0xFE}, .res = true },
      // Matches anything -> always true.
      { .ymd = {.yr =0xFEFE, .mnth = 0xFE, .day = 0xFE}, .res = true },

      // Wildcards. _YMD_LastDay = 0xFD
      { .ymd = {.yr =2099, .mnth = 1,    .day = 0xFD}, .res = true },

      { .ymd = {.yr =0xFEFE, .mnth = 4,    .day = 13}, .res = true },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = Legal_YMD(&t->ymd);

      U8 dtStr[_ISO8601_YMD_MaxStr+1];
      YMD_ToStr(&t->ymd, dtStr);
      C8 b0[100];
      sprintf(b0, "%s -> %d", dtStr, rtn);
      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->res, rtn, b0);
   }
}

/* -------------------------------- test_YMD_ToStr ----------------------------------------- */

void test_YMD_ToStr(void)
{
   typedef struct { S_YMD ymd; C8 const *out; U8 rtn; } S_Tst;

   S_Tst const tsts[] = {
      { .ymd = {.yr =0,    .mnth = 0, .day = 0 },          .rtn = 10, .out = "0000-00-00" },
      { .ymd = {.yr =2135, .mnth = 12, .day = 31 },        .rtn = 10, .out = "2135-12-31" },

      // Wildcards
      { .ymd = {.yr =0xFEFE, .mnth = 12,   .day = 31   },    .rtn = 10, .out = "****-12-31" },
      { .ymd = {.yr =2135,   .mnth = 0xFE, .day = 31   },    .rtn = 10, .out = "2135-**-31" },
      { .ymd = {.yr =2135,   .mnth = 12,   .day = 0xFE },    .rtn = 10, .out = "2135-12-**" },
      { .ymd = {.yr =0xFEFE, .mnth = 0xFE, .day = 0xFE },    .rtn = 10, .out = "****-**-**" },

      // Cannot be printed as 0000-00-00.
      { .ymd = {.yr =10000, .mnth = 12, .day = 31 },       .rtn = 10, .out = "^^^^-12-31" },
      { .ymd = {.yr =2135,  .mnth = 100, .day = 31 },      .rtn = 10, .out = "2135-^^-31" },
      { .ymd = {.yr =2135,  .mnth = 12, .day = 100 },      .rtn = 10, .out = "2135-12-^^" },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      #define _OutSize (_ISO8601_YMD_MaxStr+10)
      U8 dtStr[_OutSize];
      memset(dtStr, 0x5A, _OutSize);
      dtStr[_OutSize-1] = '\0';
      #undef _OutSize

      U8 rtn = YMD_ToStr(&t->ymd, dtStr);

      if(rtn != t->rtn) {
         printf("tst #%d: in <- (%u %u %u) Bad return; expected %u, got %u", i, t->_yr, t->_mnth, t->_day, t->rtn, rtn);
         TEST_FAIL();
      }
      else if( strcmp(t->out, dtStr) != 0 ) {
         printf("tst #%d: in <- (%u %u %u) bad string; expected \"%s\", got \"%s\"", i, t->_yr, t->_mnth, t->_day, t->out, dtStr);
         TEST_FAIL();
      }
   }
}

/* -------------------------------- test_YMDHMS_ToStr ----------------------------------------- */

void test_YMDHMS_ToStr(void)
{
   typedef struct { S_DateTime dt; C8 const *out; U8 rtn; } S_Tst;

   S_Tst const tsts[] = {
      { .dt = {._yr =0,    ._mnth = 0,  ._day = 0,  .hr = 0,  .min = 0,  .sec = 0},            .rtn = 19, .out = "0000-00-00T00:00:00" },
      { .dt = {._yr =2001, ._mnth = 1,  ._day = 15, .hr = 4,  .min = 33, .sec = 25},           .rtn = 19, .out = "2001-01-15T04:33:25" },
      { .dt = {._yr =2135, ._mnth = 12, ._day = 31, .hr = 23, .min = 59, .sec = 59 },          .rtn = 19, .out = "2135-12-31T23:59:59" },

      // Wildcards
      { .dt = {._yr =0xFEFE, ._mnth = 12,   ._day = 31,   .hr = 0,     .min = 0,   .sec = 0   },   .rtn = 19, .out = "****-12-31T00:00:00" },
      { .dt = {._yr =2135,   ._mnth = 0xFE, ._day = 31,   .hr = 0,     .min = 0,   .sec = 0   },   .rtn = 19, .out = "2135-**-31T00:00:00" },
      { .dt = {._yr =2135,   ._mnth = 12,   ._day = 0xFE, .hr = 0,     .min = 0,   .sec = 0 },     .rtn = 19, .out = "2135-12-**T00:00:00" },
      { .dt = {._yr =0xFEFE, ._mnth = 0xFE, ._day = 0xFE, .hr = 0,     .min = 0,   .sec = 0 },     .rtn = 19, .out = "****-**-**T00:00:00" },
      { .dt = {._yr =0xFEFE, ._mnth = 0xFE, ._day = 0xFE, .hr = 0xFE,  .min = 0,   .sec = 0 },     .rtn = 19, .out = "****-**-**T**:00:00" },
      { .dt = {._yr =0xFEFE, ._mnth = 0xFE, ._day = 0xFE, .hr = 0xFE,  .min = 0xFE,.sec = 0 },     .rtn = 19, .out = "****-**-**T**:**:00" },
      { .dt = {._yr =0xFEFE, ._mnth = 0xFE, ._day = 0xFE, .hr = 0xFE,  .min = 0xFE,.sec = 0xFE },  .rtn = 19, .out = "****-**-**T**:**:**" },

      // If too many digits will print '?'s.
      { .dt = {._yr =10000, ._mnth = 12,  ._day = 31,  .hr = 0,  .min = 0,  .sec = 0 },        .rtn = 19, .out = "^^^^-12-31T00:00:00" },
      { .dt = {._yr =2135,  ._mnth = 100, ._day = 31,  .hr = 0,  .min = 0,  .sec = 0 },        .rtn = 19, .out = "2135-^^-31T00:00:00" },
      { .dt = {._yr =1642,  ._mnth = 12,  ._day = 100, .hr = 0,  .min = 0,  .sec = 0 },        .rtn = 19, .out = "1642-12-^^T00:00:00" },
      { .dt = {._yr =2204,  ._mnth = 12,  ._day = 9,   .hr = 100,.min = 5,  .sec = 55 },       .rtn = 19, .out = "2204-12-09T^^:05:55" },
      { .dt = {._yr =2135,  ._mnth = 12,  ._day = 9,   .hr = 100,.min = 5,  .sec = 55 },       .rtn = 19, .out = "2135-12-09T^^:05:55" },
      { .dt = {._yr =9999,  ._mnth = 99,  ._day = 99,  .hr = 99, .min = 100,.sec = 2  },       .rtn = 19, .out = "9999-99-99T99:^^:02" },
      { .dt = {._yr =9999,  ._mnth = 99,  ._day = 99,  .hr = 99, .min = 14, .sec = 100},       .rtn = 19, .out = "9999-99-99T99:14:^^" },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      #define _OutSize (_ISO8601_YMDHMS_MaxStr+10)
      U8 dtStr[_OutSize];
      memset(dtStr, 0x5A, _OutSize);
      dtStr[_OutSize-1] = '\0';

      U8 rtn = YMDHMS_ToStr(&t->dt, dtStr);

      if(rtn != t->rtn) {
         printf("tst #%d: in <- (%u %u %u) Bad return; expected %u, got %u", i, t->dt._yr, t->dt._mnth, t->dt._day, t->rtn, rtn);
         TEST_FAIL();
      }
      else if( strcmp(t->out, dtStr) != 0 ) {
         printf("tst #%d: in <- (%u %u %u) bad string; expected \"%s\", got \"%s\"", i, t->dt._yr, t->dt._mnth, t->dt._day, t->out, dtStr);
         TEST_FAIL();
      }
   }
}

/* ------------------------------------ test_YMDHMS_Equal ---------------------------------------- */

void chkEq(U16 i, S_DateTime const *a, S_DateTime const *b, bool rtn) {
   bool got;
   C8 b0[50], b1[50];

   if(rtn != (got = YMDHMS_Equal(a,b))) {
      printf("tst #%d. Wrong return a = %s b = %s. expected %d, got %d\r\n", i, YMDHStoStr_Raw(b0,a), YMDHStoStr_Raw(b1,b), rtn, got);
      TEST_FAIL(); }}

static U16 nextYr(U16 y) {
   return y == _YMD_AnyYear-1 ? _YMD_AnyYear+1 : y+1; }

static U16 nextMDHMS(U8 n) {
   return n == _DateTime_AnyMDHMS-1 ? _DateTime_AnyMDHMS+1 : n+1; }

void test_YMDHMS_Equal(void)
{
   typedef struct { S_DateTime const a; S_DateTime const b; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      { .a = {0}, .b = {0},                           .rtn = true  },

      { .a = {0}, .b = {._yr =1},                     .rtn = false },
      { .a = {0}, .b = {._yr =_YMD_AnyYear},         .rtn = true  },
      { .a = {._yr =_YMD_AnyYear}, .b = {0},         .rtn = true  },

      { .a = {0}, .b = {._mnth = 11},                  .rtn = false },
      { .a = {0}, .b = {._mnth = _DateTime_AnyMDHMS},     .rtn = true  },
      { .a = {._mnth = _DateTime_AnyMDHMS}, .b = {0},     .rtn = true  },

      { .a = {0}, .b = {._day = 23},                   .rtn = false },
      { .a = {0}, .b = {._day = _DateTime_AnyMDHMS},      .rtn = true  },
      { .a = {._day = _DateTime_AnyMDHMS}, .b = {0},      .rtn = true  },

      { .a = {0}, .b = {.hr = 4},                     .rtn = false },
      { .a = {0}, .b = {.hr = _DateTime_AnyMDHMS},       .rtn = true  },
      { .a = {.hr = _DateTime_AnyMDHMS}, .b = {0},       .rtn = true  },

      { .a = {0}, .b = {.min = 57},                   .rtn = false },
      { .a = {0}, .b = {.min = _DateTime_AnyMDHMS},      .rtn = true  },
      { .a = {.min = _DateTime_AnyMDHMS}, .b = {0},      .rtn = true  },

      { .a = {0}, .b = {.sec = 19},                   .rtn = false },
      { .a = {0}, .b = {.sec = _DateTime_AnyMDHMS},      .rtn = true  },
      { .a = {.sec = _DateTime_AnyMDHMS}, .b = {0},      .rtn = true  },

   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = YMDHMS_Equal(&t->a, &t->b);

      if(rtn != t->rtn) {
         printf("tst #%d: Bad return; expected %u, got %u", i, t->rtn, rtn);
         TEST_FAIL();
      }
   }

   // Check (100)  random Date/times.
   for(U8 i = 0; i < 100; i++)
   {
      // First check random equal YMDHMS are seen as equal.
      S_DateTime dt0 = {._yr =randU16(), ._mnth = randU8(), ._day = randU8(), .hr = randU8(), .min = randU8(), .sec = randU8() };
      S_DateTime dt1 = dt0;
      chkEq(i, &dt0, &dt1, true);

      // Bump all dt0 to avoid any wildcards
      dt0._yr = nextYr(dt0._yr);
      dt0._mnth = nextMDHMS(dt0._mnth);
      dt0._day = nextMDHMS(dt0._day);
      dt0.hr = nextMDHMS(dt0.hr);
      dt0.min = nextMDHMS(dt0.min);
      dt0.sec = nextMDHMS(dt0.sec);

      // Start with dt1 <- dt0. Change each filed in dt1, one at a time. Check for NOT equal.
      dt1 = dt0;

      dt1._yr = nextYr(dt1._yr);
      chkEq(i, &dt0, &dt1, false);

      dt1 = dt0;
      dt1._mnth = nextMDHMS(dt1._mnth);
      chkEq(i, &dt0, &dt1, false);

      dt1 = dt0;
      dt1._day = nextMDHMS(dt1._day);
      chkEq(i, &dt0, &dt1, false);

      dt1 = dt0;
      dt1.hr = nextMDHMS(dt1.hr);
      chkEq(i, &dt0, &dt1, false);

      dt1 = dt0;
      dt1.min = nextMDHMS(dt1.min);
      chkEq(i, &dt0, &dt1, false);

      dt1 = dt0;
      dt1.sec = nextMDHMS(dt1.sec);
      chkEq(i, &dt0, &dt1, false);
   }
}

/* ------------------------------------ test_YMD_Equal ---------------------------------------- */

void chkYmdEq(U16 i, S_YMD const *a, S_YMD const *b, bool rtn) {
   bool got;
   C8 b0[50], b1[50];

   if(rtn != (got = YMD_Equal(a,b))) {
      printf("tst #%d. Wrong return a = %s b = %s. expected %d, got %d\r\n", i, YMD_ToStr(a,b0), YMD_ToStr(b,b1), rtn, got);
      TEST_FAIL(); }}

void test_YMD_Equal(void)
{
   typedef struct { S_YMD const a; S_YMD const b; bool rtn; } S_Tst;

   S_Tst const tsts[] = {
      { .a = {0}, .b = {0},                           .rtn = true  },

      { .a = {.yr = 2004, .mnth = 8, .day = 20}, .b = {.yr = 2004, .mnth = 8, .day = 20},  .rtn = true },
      { .a = {.yr = 2004, .mnth = 8, .day = 20}, .b = {.yr = 2004, .mnth = 8, .day = 9},   .rtn = false },
      { .a = {.yr = 2004, .mnth = 3, .day = 20}, .b = {.yr = 2004, .mnth = 8, .day = 20},  .rtn = false },
      { .a = {.yr = 2007, .mnth = 8, .day = 20}, .b = {.yr = 2004, .mnth = 8, .day = 20},  .rtn = false },

      { .a = {0}, .b = {.yr =1},                     .rtn = false },
      { .a = {0}, .b = {.yr =_YMD_AnyYear},          .rtn = true  },
      { .a = {.yr =_YMD_AnyYear}, .b = {0},          .rtn = true  },

      { .a = {0}, .b = {.mnth = 11},                  .rtn = false },
      { .a = {0}, .b = {.mnth = _YMD_AnyMnth},        .rtn = true  },
      { .a = {.mnth = _YMD_AnyMnth}, .b = {0},        .rtn = true  },

      { .a = {0}, .b = {.day = 23},                   .rtn = false },
      { .a = {0}, .b = {.day = _YMD_AnyDay},          .rtn = true  },
      { .a = {.day = _YMD_AnyDay}, .b = {0},          .rtn = true  },

      // ---- Last day of the given month.
      { .a = {.yr = 2004, .mnth = 1, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 1, .day = 31},            .rtn = true },
      { .a = {.yr = 2004, .mnth = 1, .day = _YMD_LastDay},  .b = {.yr = 2004, .mnth = 1, .day = 10 },          .rtn = false },

      // Check a few months.
      { .a = {.yr = 2004, .mnth = 3, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 3, .day = 31},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 4, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 4, .day = 30},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 5, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 5, .day = 31},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 6, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 6, .day = 30},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 7, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 7, .day = 31},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 8, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 8, .day = 31},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 9, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 9, .day = 30},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 10,.day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 10,.day = 31},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 11,.day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 11,.day = 30},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 12,.day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 12,.day = 31},           .rtn = true },

      // Also 28/29th Feb
      { .a = {.yr = 2003, .mnth = 2, .day = _YMD_LastDay}, .b = {.yr = 2003, .mnth = 2, .day = 28},           .rtn = true },
      { .a = {.yr = 2004, .mnth = 2, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 2, .day = 29},           .rtn = true },

      { .a = {.yr = 2004, .mnth = 2, .day = _YMD_LastDay}, .b = {.yr = 2004, .mnth = 2, .day = 28},           .rtn = false },
      { .a = {.yr = 2003, .mnth = 2, .day = _YMD_LastDay}, .b = {.yr = 2003, .mnth = 2, .day = 29},           .rtn = false },
   };

   // ----- Test the corner/special cases above

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      // YMD_Equal() is symmetric. Check both ways.
      bool rtnA = YMD_Equal(&t->a, &t->b);
      bool rtnB = YMD_Equal(&t->a, &t->b);

      if(rtnA != t->rtn ) {
         printf("tst #%d: YMD_Equal(a,b) Bad return; expected %u, got %u\r\n", i, t->rtn, rtnA);
         TEST_FAIL(); }
      else if(rtnB != t->rtn ) {
         printf("tst #%d: YMD_Equal(b,a) Bad return; expected %u, got %u\r\n", i, t->rtn, rtnB);
         TEST_FAIL(); }
   }

   // ----- Check (100) random Date/times.

   for(U8 i = 0; i < 100; i++)
   {
      // First check random equal YMD are seen as equal.
      S_YMD dt0 = {.yr =randU16(), .mnth = randU8(), .day = randU8() };
      S_YMD dt1 = dt0;
      chkYmdEq(i, &dt0, &dt1, true);

      // Bump all dt0 to avoid any wildcards
      dt0.yr = nextYr(dt0.yr);
      dt0.mnth = nextMDHMS(dt0.mnth);
      dt0.day = nextMDHMS(dt0.day);

      // Start with dt1 <- dt0. Change each filed in dt1, one at a time. Check for NOT equal.
      dt1 = dt0;

      dt1.yr = nextYr(dt1.yr);
      chkYmdEq(i, &dt0, &dt1, false);

      dt1 = dt0;
      dt1.mnth = nextMDHMS(dt1.mnth);
      chkYmdEq(i, &dt0, &dt1, false);

      dt1 = dt0;
      dt1.day = nextMDHMS(dt1.day);
      chkYmdEq(i, &dt0, &dt1, false);
   }
}

/* --------------------------------- test_YMDHMS_aGTEb ----------------------------------------- */

void test_YMDHMS_aGTEb(void)
{
   typedef struct {S_DateTime const * a; S_DateTime const * b; bool rtn;} S_Tst;

   #define _dt(_yr, _mnth, _day, _hr, _min, _sec) \
      &(S_DateTime){.ymd.yr =_yr, .ymd.mnth = _mnth, .ymd.day = _day, .hr = _hr, .min = _min, .sec = _sec}

   S_Tst const tsts[] = {
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,23,5,17,44),    .rtn = true },        // Equal
      { .a = _dt(2014,11,23,5,17,42),           .b = _dt(2013,11,23,5,17,44),    .rtn = true },        // GT
      { .a = _dt(2012,11,23,5,17,42),           .b = _dt(2013,11,23,5,17,44),    .rtn = false },       // LT

      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2014,14,23,5,17,44),    .rtn = false },       // a.yr < b.yr -> false
      { .a = _dt(2013,3,23,5,17,44),            .b = _dt(2013,4,23,5,17,44),     .rtn = false },       // a.mnth < b.mnth -> false
      { .a = _dt(2013,11,7,5,17,44),            .b = _dt(2013,11,8,5,17,44),     .rtn = false },       // a.day < b.day -> false
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,23,6,17,44),    .rtn = false },       // a.hr < b.hr -> false
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,23,5,20,44),    .rtn = false },       // a.min < b.min -> false
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,23,5,17,50),    .rtn = false },       // a.sec < b.sec -> false

      // Wildcards for any field give true for comparison of that field.
      { .a = _dt(_YMD_AnyYear,11,23,5,17,44),   .b = _dt(2013,2,23,5,17,44),              .rtn = true },
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(_YMD_AnyYear,11,9,5,17,44),      .rtn = true },
      { .a = _dt(2013,_YMD_AnyMnth,23,5,17,44), .b = _dt(2013,11,23,5,17,44),             .rtn = true },
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,_YMD_AnyMnth,23,5,17,44),   .rtn = true },
      { .a = _dt(2013,11,_YMD_AnyDay,5,17,44),  .b = _dt(2013,11,23,5,17,44),             .rtn = true },
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,_YMD_AnyDay,5,17,44),    .rtn = true },
      { .a = _dt(2013,11,23,_YMD_AnyHr,17,44),  .b = _dt(2013,11,23,5,17,44),             .rtn = true },
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,23,_YMD_AnyHr,17,44),    .rtn = true },
      { .a = _dt(2013,11,23,5,_YMD_AnyMinute,44), .b = _dt(2013,11,23,5,17,44),           .rtn = true },
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,23,5,_YMD_AnyMinute,44), .rtn = true },

      // Wildcard days; there's nothing else to compare, so result is 'false'.
      { .a = _dt(2013,11,23,5,17,_YMD_AnySec),  .b = _dt(2013,11,23,5,17,44),             .rtn = false },
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,23,5,17,_YMD_AnySec),    .rtn = false },

      // Other field are still compared, largest units to smallest.
      { .a = _dt(_YMD_AnyYear,2,23,5,17,44),    .b = _dt(2013,11,23,5,17,44),             .rtn = false },
      { .a = _dt(2013,2,9,5,17,44),             .b = _dt(_YMD_AnyYear,11,9,5,17,44),      .rtn = false },
      { .a = _dt(2013,_YMD_AnyMnth,1,5,17,44),  .b = _dt(2013,11,23,5,17,44),             .rtn = false },
      { .a = _dt(2013,11,1,5,17,44),            .b = _dt(2013,_YMD_AnyMnth,23,5,17,44),   .rtn = false },
      { .a = _dt(2013,11,_YMD_AnyDay,5,17,44),  .b = _dt(2013,11,23,7,17,44),             .rtn = false },
      { .a = _dt(2013,11,23,5,17,44),           .b = _dt(2013,11,_YMD_AnyDay,7,17,44),    .rtn = false },
      { .a = _dt(2013,11,23,_YMD_AnyHr,1,44),   .b = _dt(2013,11,23,5,17,44),             .rtn = false },
      { .a = _dt(2013,11,23,5,1,44),            .b = _dt(2013,11,23,_YMD_AnyHr,17,44),    .rtn = false },
      { .a = _dt(2013,11,23,5,_YMD_AnyMinute,22), .b = _dt(2013,11,23,5,17,44),           .rtn = false },
      { .a = _dt(2013,11,23,5,17,22),           .b = _dt(2013,11,23,5,_YMD_AnyMinute,44), .rtn = false },
      { .a = _dt(2013,1,23,5,17,_YMD_AnySec),   .b = _dt(2013,3,23,5,17,44),              .rtn = false },
      { .a = _dt(2013,1,23,5,17,44),            .b = _dt(2013,3,23,5,17,_YMD_AnySec),     .rtn = false },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = YMDHMS_aGTEb(t->a, t->b);

      if(rtn != t->rtn) {
         printf("tst #%d: Bad return; expected %u, got %u\r\n", i, t->rtn, rtn);
         TEST_FAIL();
      }
   }
}

/* --------------------------------------- test_YMDHMS_AddSecs ------------------------------------ */

void test_YMDHMS_AddSecs(void)
{
   typedef struct {S_DateTime const * out; S_DateTime const * in; S32 secs;} S_Tst;

   S_Tst const tsts[] = {
      // Add nothing to a Time/Date. Get it back unchanged.
      { .out = _dt(2013,11,23,5,17,44),    .in = _dt(2013,11,23,5,17,44),     .secs = 0               },

      // Adds and subtracts
      { .out = _dt(2013,11,23,5,17,59),    .in = _dt(2013,11,23,5,17,00),     .secs = 59,             },    // + 1sec
      { .out = _dt(2014,1,1,00,00,00),     .in = _dt(2013,12,31,23,59,59),    .secs = 1,              },    // Rollover to the new year
      { .out = _dt(2013,12,31,23,59,59),   .in = _dt(2014,1,1,00,00,00),      .secs = -1,             },    // Rollunder to the previous year
      { .out = _dt(2013,11,23,5,18,1),     .in = _dt(2013,11,23,5,17,00),     .secs = 61,             },    // + 1min
      { .out = _dt(2013,11,23,6,17,00),    .in = _dt(2013,11,23,5,17,00),     .secs = 3600,           },    // +1hr
      { .out = _dt(2013,11,24,5,17,00),    .in = _dt(2013,11,23,5,17,00),     .secs = 24*3600L,       },    // +1day
      { .out = _dt(2013,2,23,5,17,00),     .in = _dt(2013,1,23,5,17,00),      .secs = 31*24*3600L,    },    // Jan -> Feb
      { .out = _dt(2013,1,23,5,17,00),     .in = _dt(2013,2,23,5,17,00),      .secs = -31*24*3600L,   },    // Feb -> Jan
      { .out = _dt(2014,11,23,5,17,00),    .in = _dt(2013,11,23,5,17,00),     .secs = 365*24*3600L,   },    // 2013 -> 2014
      { .out = _dt(2013,11,23,5,17,00),    .in = _dt(2014,11,23,5,17,00),     .secs = -365*24*3600L,  },    // 2014-> 2013

      //{ .out = _dt(1910,1,1,0,5,0),        .in = _dt(1910,1,1,0,0,0),         .secs = 300,  },

      // Clip at upper and lower bounds of S_DateTime.
      { .out = _dt(2000,1,1,0,0,0),        .in = _dt(2014,1,1,00,00,00),      .secs = MIN_S32,        },
      { .out = _dt(2136,2,7,6,28,15),      .in = _dt(2081,4,7,21,42,15),      .secs = MAX_S32,        },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_DateTime out;

      S_DateTime const * rtn = YMDHMS_AddSecs(&out, t->in, t->secs);

      if(rtn != &out) {
         printf("tst #d function return did not equal out ptr\r\n");
         TEST_FAIL(); }

      else if( YMDHMS_Equal(&out, t->out) == false) {
         C8 b0[_ISO8601_YMDHMS_MaxStr], b1[_ISO8601_YMDHMS_MaxStr], b2[_ISO8601_YMDHMS_MaxStr];

         YMDHMS_ToStr(t->in, b0);
         YMDHMS_ToStr(t->out, b1);
         YMDHMS_ToStr(&out, b2);

         printf("tst #%d: %s + %ld secs -> expected %s, got %s\r\n", i, b0, t->secs, b1, b2);
         TEST_FAIL();
      }
   }

   /* 'in' and 'out' in YMDHMS_AddSecs() may be the same object. Check this by running the same tests
       returning the input onto itself.
   */
   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_DateTime out = *(t->in);
      S_DateTime const * rtn = YMDHMS_AddSecs(&out, &out, t->secs);     // 'out' <- 'out'.

      if( YMDHMS_Equal(&out, t->out) == false) {
         C8 b0[_ISO8601_YMDHMS_MaxStr], b1[_ISO8601_YMDHMS_MaxStr], b2[_ISO8601_YMDHMS_MaxStr];

         YMDHMS_ToStr(t->in, b0);
         YMDHMS_ToStr(t->out, b1);
         YMDHMS_ToStr(&out, b2);

         printf("YMDHMS_AddSecs() tst #%d: %s + %ld secs -> expected %s, got %s\r\n", i, b0, t->secs, b1, b2);
         TEST_FAIL();
      }
   }
}

/* --------------------------------------- test_Full_YMDHMS_AddSecs ------------------------------------ */

void test_Full_YMDHMS_AddSecs(void)
{
   typedef struct {S_DateTime const * out; S_DateTime const * in; S32 secs;} S_Tst;

   S_Tst const tsts[] = {
      // Add nothing to a Time/Date. Get it back unchanged.
      { .out = _dt(2013,11,23,5,17,44),    .in = _dt(2013,11,23,5,17,44),     .secs = 0               },

      // Adds and subtracts
      { .out = _dt(2013,11,23,5,17,59),    .in = _dt(2013,11,23,5,17,00),     .secs = 59,             },    // + 1sec
      { .out = _dt(2014,1,1,00,00,00),     .in = _dt(2013,12,31,23,59,59),    .secs = 1,              },    // Rollover to the new year
      { .out = _dt(2013,12,31,23,59,59),   .in = _dt(2014,1,1,00,00,00),      .secs = -1,             },    // Rollunder to the previous year
      { .out = _dt(2013,11,23,5,18,1),     .in = _dt(2013,11,23,5,17,00),     .secs = 61,             },    // + 1min
      { .out = _dt(2013,11,23,6,17,00),    .in = _dt(2013,11,23,5,17,00),     .secs = 3600,           },    // +1hr
      { .out = _dt(2013,11,24,5,17,00),    .in = _dt(2013,11,23,5,17,00),     .secs = 24*3600L,       },    // +1day
      { .out = _dt(2013,2,23,5,17,00),     .in = _dt(2013,1,23,5,17,00),      .secs = 31*24*3600L,    },    // Jan -> Feb
      { .out = _dt(2013,1,23,5,17,00),     .in = _dt(2013,2,23,5,17,00),      .secs = -31*24*3600L,   },    // Feb -> Jan
      { .out = _dt(2014,11,23,5,17,00),    .in = _dt(2013,11,23,5,17,00),     .secs = 365*24*3600L,   },    // 2013 -> 2014
      { .out = _dt(2013,11,23,5,17,00),    .in = _dt(2014,11,23,5,17,00),     .secs = -365*24*3600L,  },    // 2014-> 2013

      // Back and forward through the millennium (2000AD).
      { .out = _dt(1910,1,1,0,5,0),        .in = _dt(1910,1,1,0,0,0),         .secs = 300,  },
      { .out = _dt(1999,12,31,23,59,59),   .in = _dt(2000,1,1,0,0,0),         .secs = -1,  },               // Back 1 sec, bfore millennium
      { .out = _dt(2000,1,1,0,0,0),        .in = _dt(1999,12,31,23,59,59),    .secs = +1,  },               // Fwd 1 sec, into millennium

      { .out = _dt(1999,1,1,0,0,0),        .in = _dt(2000,1,1,0,0,0),         .secs = -365*24*3600L,  },    // Back 1 year, b4 millennium
      { .out = _dt(2000,1,1,0,0,0),        .in = _dt(1999,1,1,0,0,0),         .secs = +365*24*3600L,  },    // Fwd 1 year, into millennium

      { .out = _dt(1996,1,1,0,0,0),        .in = _dt(2000,1,1,0,0,0),         .secs = -(365+365+365+366)*24*3600L,  },  // Back 4 years...
      { .out = _dt(2000,1,1,0,0,0),        .in = _dt(1996,1,1,0,0,0),         .secs = (365+365+365+366)*24*3600L,  },  // Fwd 4 years...

      { .out = _dt(1945,12,13,20,45,52),   .in = _dt(2014,1,1,00,00,00),      .secs = MIN_S32,        },    // UTC Tool says Dec 13th; but may be wrong.
      { .out = _dt(2149,4,26,00,56,22),      .in = _dt(2081,4,7,21,42,15),      .secs = MAX_S32,        },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_DateTime out;

      S_DateTime const * rtn = Full_YMDHMS_AddSecs(&out, t->in, t->secs);

      if(rtn != &out) {
         printf("tst #d function return did not equal out ptr\r\n");
         TEST_FAIL(); }

      else if( YMDHMS_Equal(&out, t->out) == false) {
         C8 b0[_ISO8601_YMDHMS_MaxStr], b1[_ISO8601_YMDHMS_MaxStr], b2[_ISO8601_YMDHMS_MaxStr];

         YMDHMS_ToStr(t->in, b0);
         YMDHMS_ToStr(t->out, b1);
         YMDHMS_ToStr(&out, b2);

         printf("tst #%d: %s + %ld secs -> expected %s, got %s\r\n", i, b0, t->secs, b1, b2);
         TEST_FAIL();
      }
   }

   /* 'in' and 'out' in YMDHMS_AddSecs() may be the same object. Check this by running the same tests
       returning the input onto itself.
   */
   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_DateTime out = *(t->in);
      S_DateTime const * rtn = Full_YMDHMS_AddSecs(&out, &out, t->secs);     // 'out' <- 'out'.

      if( YMDHMS_Equal(&out, t->out) == false) {
         C8 b0[_ISO8601_YMDHMS_MaxStr], b1[_ISO8601_YMDHMS_MaxStr], b2[_ISO8601_YMDHMS_MaxStr];

         YMDHMS_ToStr(t->in, b0);
         YMDHMS_ToStr(t->out, b1);
         YMDHMS_ToStr(&out, b2);

         printf("YMDHMS_AddSecs() tst #%d: %s + %ld secs -> expected %s, got %s\r\n", i, b0, t->secs, b1, b2);
         TEST_FAIL();
      }
   }
}

/* --------------------------------- test_YMD_aGTEb ----------------------------------------- */

void test_YMD_aGTEb(void)
{
   typedef struct {S_YMD const * a; S_YMD const * b; bool rtn;} S_Tst;

   #define _ymd(_yr, _mnth, _day) \
      &(S_YMD){.yr =_yr, .mnth = _mnth, .day = _day}

   S_Tst const tsts[] = {
      { .a = _ymd(2013,11,23),           .b = _ymd(2013,11,23),    .rtn = true },        // Equal
      { .a = _ymd(2014,11,23),           .b = _ymd(2013,11,23),    .rtn = true },        // GT
      { .a = _ymd(2012,11,23),           .b = _ymd(2013,11,23),    .rtn = false },       // LT

      { .a = _ymd(2013,11,23),           .b = _ymd(2014,14,23),    .rtn = false },       // a.yr < b.yr -> false
      { .a = _ymd(2013,3,23),            .b = _ymd(2013,4,23),     .rtn = false },       // a.mnth < b.mnth -> false
      { .a = _ymd(2013,11,7),            .b = _ymd(2013,11,8),     .rtn = false },       // a.day < b.day -> false

      // Wildcards for any field give true for comparison of that field.
      { .a = _ymd(_YMD_AnyYear,11,23),   .b = _ymd(2013,2,23),              .rtn = true },
      { .a = _ymd(2013,11,23),           .b = _ymd(_YMD_AnyYear,11,9),      .rtn = true },
      { .a = _ymd(2013,_YMD_AnyMnth,23), .b = _ymd(2013,11,23),             .rtn = true },
      { .a = _ymd(2013,11,23),           .b = _ymd(2013,_YMD_AnyMnth,23),   .rtn = true },

      { .a = _ymd(2013,11,_YMD_AnyDay),  .b = _ymd(2013,11,23),             .rtn = true },
      { .a = _ymd(2013,11,23),           .b = _ymd(2013,11,_YMD_AnyDay),    .rtn = true },

      // '_YMD_LastDay' resolves to the actual day of the YMD.
      { .a = _ymd(2013,11,_YMD_LastDay), .b = _ymd(2013,11,23),             .rtn = true },
      { .a = _ymd(2013,11,23),           .b = _ymd(2013,11,_YMD_LastDay),    .rtn = false },
      { .a = _ymd(2013,11,_YMD_LastDay), .b = _ymd(2013,11,_YMD_LastDay),    .rtn = true },

      // Other field are still compared, largest units to smallest.
      { .a = _ymd(_YMD_AnyYear,2,23),    .b = _ymd(2013,11,23),             .rtn = false },
      { .a = _ymd(2013,2,9),             .b = _ymd(_YMD_AnyYear,11,9),      .rtn = false },
      { .a = _ymd(2013,_YMD_AnyMnth,1),  .b = _ymd(2013,11,23),             .rtn = false },
      { .a = _ymd(2013,11,1),            .b = _ymd(2013,_YMD_AnyMnth,23),   .rtn = false },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = YMD_aGTEb(t->a, t->b);

      if(rtn != t->rtn) {
         printf("tst #%d: Bad return; expected %u, got %u\r\n", i, t->rtn, rtn);
         TEST_FAIL();
      }
   }
}

// ----------------------------------------- eof --------------------------------------------
