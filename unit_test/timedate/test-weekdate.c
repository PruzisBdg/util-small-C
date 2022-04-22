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

PRIVATE C8 const *tf(bool b) { return b != false ? "true" : "false"; }

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* -------------------------------- test_Legal_WeekDate ------------------------------------- */

void test_Legal_WeekDate(void)
{
   S_WeekDate const legals[] = {
      {.yr = 1999, .week = 52, .day = 6},    // 2000-1-1 Gregorian; the start of T_Seconds32.
      {.yr = 1999, .week = 52, .day = 7},
      {.yr = 2000, .week = 1,  .day = 1},    // 2000-1-3 Gregorian.

      {.yr = 2015, .week = 53, .day = 7},

      {.yr = 2015, .week = 53, .day = 7},

      // Feb 7th 2136. The last day which contains some part of [0 ... _Max_T_Seconds32]
      {.yr = 2136, .week = 6, .day = 1},
   };

   S_WeekDate const illegals[] = {
      {.yr = 1999, .week = 52, .day = 5},    // Day before 2000-1-1 Gregorian; the start of T_Seconds32.

      // Illegal weeks & days
      {.yr = 2000, .week = 0,  .day = 1},
      {.yr = 2000, .week = 1,  .day = 0},
      {.yr = 2000, .week = 54, .day = 1},
      {.yr = 2000, .week = 1,  .day = 8},

      {.yr = 2000, .week = 1,  .day = 8},

      // Feb 8th 2136. Is past [0 ... _Max_T_Seconds32]
      {.yr = 2136, .week = 6, .day = 2},

      {.yr = 2137, .week = 1, .day = 1},
   };


   for(U8 i = 0; i < RECORDS_IN(legals); i++)
   {
      S_WeekDate const *t = &legals[i];

      if( Legal_WeekDate(t) == FALSE ) {
         printf("Legal_WeekDate() legals[%d]:  %u-W%02u-%u->false, expected true\r\n",
               i, t->yr, t->week, t->day);
         TEST_FAIL();
      }
   }

   for(U8 i = 0; i < RECORDS_IN(illegals); i++)
   {
      S_WeekDate const *t = &illegals[i];

      if( Legal_WeekDate(t) == TRUE ) {
         printf("Legal_WeekDate() illegals[%d]:  %u-W%02u-%u->false, expected true\r\n",
               i, t->yr, t->week, t->day);
         TEST_FAIL();
      }
   }
}


/* --------------------------------- test_WeekDate_A_LT_B --------------------------------- */

void test_WeekDate_A_LT_B(void)
{
   typedef struct {S_WeekDate a, b; bool rtn;} S_Tst;

   S_Tst const tsts[] = {

      {.a = (S_WeekDate){.yr=2001, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=53, .day=7 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=2,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=2 }, .rtn = false },

      // Equal => false.
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },

      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=2 }, .rtn = true  },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=2,  .day=1 }, .rtn = true  },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=7 },  .b = (S_WeekDate){.yr=2000, .week=2,  .day=1 }, .rtn = true  },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2001, .week=1,  .day=1 }, .rtn = true },
      {.a = (S_WeekDate){.yr=2000, .week=2,  .day=2 },  .b = (S_WeekDate){.yr=2001, .week=1,  .day=1 }, .rtn = true },
      {.a = (S_WeekDate){.yr=2000, .week=53, .day=7 },  .b = (S_WeekDate){.yr=2001, .week=1,  .day=1 }, .rtn = true },

      // Malformed Week-Dates always return false; even if arithmetic comparision imples true.
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=0 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=0,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=8 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=54,  .day=1 }, .rtn = false },

      // May go outside range of 'T_Seconds32' i.e [2000AD..Feb 7th 2136].
      {.a = (S_WeekDate){.yr=2136, .week=53,  .day=6 }, .b = (S_WeekDate){.yr=2136, .week=53, .day=7 }, .rtn = true },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = WeekDate_A_LT_B(&t->a, &t->b);

      if(rtn != t->rtn) {
         printf("WeekDate_A_LT_B() fail #%d:  expected %u-W%02u-%u LT %u-W%02u-%u -> %s got %s\r\n",
               i,
               t->a.yr, t->a.week, t->a.day,
               t->b.yr, t->b.week, t->b.day,
               tf(t->rtn), tf(rtn));
         TEST_FAIL();
      }
   }
}


/* --------------------------------- test_WeekDate_A_GT_B --------------------------------- */

void test_WeekDate_A_GT_B(void)
{
   typedef struct {S_WeekDate a, b; bool rtn;} S_Tst;

   S_Tst const tsts[] = {

      {.a = (S_WeekDate){.yr=2000, .week=53, .day=7 },  .b = (S_WeekDate){.yr=2001, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=2 },  .b = (S_WeekDate){.yr=2000, .week=2,  .day=1 }, .rtn = false },

      // Equal => false.
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },

      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=2 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = true  },
      {.a = (S_WeekDate){.yr=2000, .week=2,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = true  },
      {.a = (S_WeekDate){.yr=2000, .week=2,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=7 }, .rtn = true  },
      {.a = (S_WeekDate){.yr=2001, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = true },
      {.a = (S_WeekDate){.yr=2001, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=2,  .day=2 }, .rtn = true },
      {.a = (S_WeekDate){.yr=2001, .week=1 , .day=1 },  .b = (S_WeekDate){.yr=2000, .week=53, .day=7 }, .rtn = true },

      // Malformed Week-Dates always return false; even if arithmetic comparision imples true.
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=0 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=0,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=8 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=54,  .day=1 }, .rtn = false },

      // May go outside range of 'T_Seconds32' i.e [2000AD..Feb 7th 2136].
      {.a = (S_WeekDate){.yr=2136, .week=53,  .day=7 }, .b = (S_WeekDate){.yr=2136, .week=53, .day=6 }, .rtn = true },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = WeekDate_A_GT_B(&t->a, &t->b);

      if(rtn != t->rtn) {
         printf("WeekDate_A_GT_B() fail #%d:  expected %u-W%02u-%u GT %u-W%02u-%u -> %s got %s\r\n",
               i,
               t->a.yr, t->a.week, t->a.day,
               t->b.yr, t->b.week, t->b.day,
               tf(t->rtn), tf(rtn));
         TEST_FAIL();
      }
   }
}

/* --------------------------------- test_WeekDate_Equal --------------------------------- */

void test_WeekDate_Equal(void)
{
   typedef struct {S_WeekDate a, b; bool rtn;} S_Tst;

   S_Tst const tsts[] = {

      {.a = (S_WeekDate){.yr=2000, .week=53, .day=7 },  .b = (S_WeekDate){.yr=2001, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=2 },  .b = (S_WeekDate){.yr=2000, .week=2,  .day=1 }, .rtn = false },

      // Equal => true.
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = true },
      {.a = (S_WeekDate){.yr=2092, .week=53, .day=7 },  .b = (S_WeekDate){.yr=2092, .week=53, .day=7 }, .rtn = true },

      // Malformed Week-Dates always return false; even if arithmetic comparision imples true.
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=0 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=0,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=1 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=1,  .day=8 }, .rtn = false },
      {.a = (S_WeekDate){.yr=2000, .week=1,  .day=1 },  .b = (S_WeekDate){.yr=2000, .week=54,  .day=1 }, .rtn = false },

      // May go outside range of 'T_Seconds32' i.e [2000AD..Feb 7th 2136].
      {.a = (S_WeekDate){.yr=2136, .week=53,  .day=7 }, .b = (S_WeekDate){.yr=2136, .week=53, .day=7 }, .rtn = true },
      {.a = (S_WeekDate){.yr=1990, .week=14,  .day=4 }, .b = (S_WeekDate){.yr=1990, .week=14, .day=4 }, .rtn = true },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      bool rtn = WeekDate_Equal(&t->a, &t->b);

      if(rtn != t->rtn) {
         printf("test_WeekDate_Equal() fail #%d:  expected %u-W%02u-%u == %u-W%02u-%u -> %s got %s\r\n",
               i,
               t->a.yr, t->a.week, t->a.day,
               t->b.yr, t->b.week, t->b.day,
               tf(t->rtn), tf(rtn));
         TEST_FAIL();
      }
   }
}


/* -------------------------------- test_DaysToWeekDate --------------------------------------------- */

void test_DaysToWeekDate(void)
{
   typedef struct {U32 days; S_WeekDate wd; } S_Tst;

   S_Tst const tsts[] = {
      {.days = 0,             .wd = {.week = 52, .day = 6, .yr = 1999}},    // Sat Jan 1st 2000AD
      {.days = 1,             .wd = {.week = 52, .day = 7, .yr = 1999}},    // Sun Jan 2nd
      {.days = 2,             .wd = {.week = 1,  .day = 1, .yr = 2000}},    // Mon Jan 3rd, the 1st workday of 2000AD

      {.days = 365-7,         .wd = {.week = 51, .day = 7, .yr = 2000}},    // Sun Dec 24th 2000. 2000AD is leap year so elapsed days are 0-365.
      {.days = 365,           .wd = {.week = 52, .day = 7, .yr = 2000}},    // Sun Dec 31st 2000. 2000AD is leap year so elapsed days are 0-365.
      {.days = 365+1,         .wd = {.week = 1,  .day = 1, .yr = 2001}},    // Mon Jan 1st 2001. Jan 1st is holiday; business starts Tues Jan 2nd.
      {.days = 365+2,         .wd = {.week = 1,  .day = 2, .yr = 2001}},    // Mon Jan 2nd 2001.

      {.days = (2*365),       .wd = {.week = 1,  .day = 1, .yr = 2002}},    // Mon Dec 31st 2001. Weds Jan 2nd is 1st business day of 2002, so Mon is in Week 01
      {.days = (2*365)+1,     .wd = {.week = 1,  .day = 2, .yr = 2002}},    // Tues Jan 1st 2002. Weds Jan 2nd is 1st business day of 2002, so Tue is in Week 01
      {.days = (2*365)+2,     .wd = {.week = 1,  .day = 3, .yr = 2002}},    // Weds Jan 2nd 2002. ...
      {.days = (2*365)+6,     .wd = {.week = 1,  .day = 7, .yr = 2002}},    // Sun Jan 6th 2002. ...
      {.days = (2*365)+7,     .wd = {.week = 2,  .day = 1, .yr = 2002}},    // Mon Jan 7th 2002. -> Week 02
      {.days = (2*365)+8,     .wd = {.week = 2,  .day = 2, .yr = 2002}},    // Tues Jan 8th 2002. -> Week 02

      {.days = (3*365)-2-7,   .wd = {.week = 51, .day = 7, .yr = 2002}},   // Sun Dec 22nd 2002.
      {.days = (3*365)-2,     .wd = {.week = 52, .day = 7, .yr = 2002}},   // Sun Dec 29th 2002.
      {.days = (3*365)-1,     .wd = {.week = 1,  .day = 1, .yr = 2003}},   // Mon Dec 30th 2002. Thurs Jan 2nd is a business day so Mon is in Week 01 of 2003
      {.days = (3*365),       .wd = {.week = 1,  .day = 2, .yr = 2003}},   // Tues Dec 31st 2002. Thurs Jan 2nd is a business day so Tues is in Week 01 of 2003
      {.days = (3*365)+1,     .wd = {.week = 1,  .day = 3, .yr = 2003}},   // Wed Jan 1st 2003...
      {.days = (3*365)+5,     .wd = {.week = 1,  .day = 7, .yr = 2003}},   // Sun Jan 5th 2003... Last day of Week 1
      {.days = (3*365)+6,     .wd = {.week = 2,  .day = 1, .yr = 2003}},   // Mon Jan 6th 2003... 1st day of Week 2

      {.days = (4*365),       .wd = {.week = 1,  .day = 3, .yr = 2004}},   // Wed Dec 31st 2003... Jan 2nd is Fri of same week; so already Week 1 of 2004
      {.days = (4*365)+1,     .wd = {.week = 1,  .day = 4, .yr = 2004}},   // Thurs Jan 1st 2004...  "      "       "       "        "    "
      {.days = (4*365)+4,     .wd = {.week = 1,  .day = 7, .yr = 2004}},   // Sun Jan 4th 2004.
      {.days = (4*365)+5,     .wd = {.week = 2,  .day = 1, .yr = 2004}},   // Mon Jan 5th 2004.

      // 2004 -> 2005 (2004 is a leap yr).
      {.days = (4*365)+366-7, .wd = {.week = 52, .day = 5, .yr = 2004}},   // Fri Dec 20th 2004.
      {.days = (4*365)+366-4, .wd = {.week = 53, .day = 1, .yr = 2004}},   // Mon Dec 23rd 2004.
      {.days = (4*365)+366,   .wd = {.week = 53, .day = 5, .yr = 2004}},   // Fri Dec 31st 2004
      {.days = (4*365)+366+1, .wd = {.week = 53, .day = 6, .yr = 2004}},   // Sat Jan 1st 2005
      {.days = (4*365)+366+2, .wd = {.week = 53, .day = 7, .yr = 2004}},   // Sun Jan 2nd 2005
      {.days = (4*365)+366+3, .wd = {.week = 1,  .day = 1, .yr = 2005}},   // Mon Jan 3rd 2005

      {.days = (5*365)+366-6, .wd = {.week = 51, .day = 7, .yr = 2005}},   // Sun Dec 25th 2005
      {.days = (5*365)+366-5, .wd = {.week = 52, .day = 1, .yr = 2005}},   // Mon Dec 26th 2005
      {.days = (5*365)+366,   .wd = {.week = 52, .day = 6, .yr = 2005}},   // Sat Dec 31st 2005
      {.days = (5*365)+366+1, .wd = {.week = 52, .day = 7, .yr = 2005}},   // Sun Jan 1st 2006
      {.days = (5*365)+366+2, .wd = {.week = 1,  .day = 1, .yr = 2006}},   // Mon Jan 2nd 2006

      {.days = (6*365)+366-7, .wd = {.week = 51, .day = 7, .yr = 2006}},   // Sun Dec 24th 2006
      {.days = (6*365)+366-6, .wd = {.week = 52, .day = 1, .yr = 2006}},   // Mon Dec 25th 2006
      {.days = (6*365)+366,   .wd = {.week = 52, .day = 7, .yr = 2006}},   // Sun Dec 31st 2006
      {.days = (6*365)+366+1, .wd = {.week = 1,  .day = 1, .yr = 2007}},   // Mon Jan 1st 2007
      {.days = (6*365)+366+7, .wd = {.week = 1,  .day = 7, .yr = 2007}},   // Sun Jan 7th 2007
      {.days = (6*365)+366+8, .wd = {.week = 2,  .day = 1, .yr = 2007}},   // Mon Jan 8th 2007

      {.days = (7*365)+366-8, .wd = {.week = 51, .day = 7, .yr = 2007}},   // Sun Dec 23rd 2007.
      {.days = (7*365)+366-1, .wd = {.week = 52, .day = 7, .yr = 2007}},   // Sun Dec 30th 2007.
      {.days = (7*365)+366,   .wd = {.week = 1,  .day = 1, .yr = 2008}},   // Mon Dec 31st 2007. Weds Jan 2nd is 1st business day of 2008, so Mon is in Week 01
      {.days = (7*365)+366+1, .wd = {.week = 1,  .day = 2, .yr = 2008}},   // Tues Jan 1st 2008.
      {.days = (7*365)+366+6, .wd = {.week = 1,  .day = 7, .yr = 2008}},   // Sun Jan 6th 2008.
      {.days = (7*365)+366+7, .wd = {.week = 2,  .day = 1, .yr = 2008}},   // Mon Jan 7th 2008.

      {.days = (7*365)+(2*366)-10,  .wd = {.week = 51, .day = 7, .yr = 2008}},   // Sun Dec 21st 2008.
      {.days = (7*365)+(2*366)-3,   .wd = {.week = 52, .day = 7, .yr = 2008}},   // Sun Dec 28th 2008.
      {.days = (7*365)+(2*366)-2,   .wd = {.week = 1,  .day = 1, .yr = 2009}},   // Mon Dec 29th 2008. Fri Jan 2nd is 1st business day of 2009. so this day is in 2009.
      {.days = (7*365)+(2*366),     .wd = {.week = 1,  .day = 3, .yr = 2009}},   // Weds Dec 31st 2008. Fri Jan 2nd is 1st business day of 2009. so this day is in 2009.
      {.days = (7*365)+(2*366)+1,   .wd = {.week = 1,  .day = 4, .yr = 2009}},   // Thurs Jan 1st 2009.
      {.days = (7*365)+(2*366)+4,   .wd = {.week = 1,  .day = 7, .yr = 2009}},   // Sun Jan 4th 2009.
      {.days = (7*365)+(2*366)+5,   .wd = {.week = 2,  .day = 1, .yr = 2009}},   // Mon Jan 5th 2009.

      {.days = (8*365)+(2*366)-4,   .wd = {.week = 52, .day = 7, .yr = 2009}},   // Sun Dec 27th 2009.
      {.days = (8*365)+(2*366)-3,   .wd = {.week = 53, .day = 1, .yr = 2009}},   // Mon Dec 28th 2009.
      {.days = (8*365)+(2*366),     .wd = {.week = 53, .day = 4, .yr = 2009}},   // Thurs Dec 31st 2009.
      {.days = (8*365)+(2*366)+1,   .wd = {.week = 53, .day = 5, .yr = 2009}},   // Fri Jan 1st 2010.... Because Jan 1st is a holiday this is still last ISO week of 2009
      {.days = (8*365)+(2*366)+3,   .wd = {.week = 53, .day = 7, .yr = 2009}},   // Sun Jan 3rd 2010.... is still last ISO week of 2009
      {.days = (8*365)+(2*366)+4,   .wd = {.week = 1,  .day = 1, .yr = 2010}},   // Mon Jan 4th 2010.... starts the 2010 business year.

      // Elapsed century, days in a century - 1
      #define _CenturyE (25UL*(366+365+365+365)-1)

      // Last day of 21st century. Jan 1st 2100 is Friday, 2nd is Sat, not a business day. So this remains last ISO week of current year.
      {.days = _CenturyE,           .wd = {.week = 53,  .day = 4, .yr = 2099}},   // Thurs Dec 31st 2099.
      {.days = _CenturyE+1,         .wd = {.week = 53,  .day = 5, .yr = 2099}},   // Fri Jan 1st 2100; 1st day of 22nd century.
      {.days = _CenturyE+4,         .wd = {.week = 1,   .day = 1, .yr = 2100}},   // Mon Jan 4th 2100.

      // ------ Centurial year correction. 2100 is NOT a leap year so next 4 yesr have just 4x365 days.
      {.days = _CenturyE+31+28-7,   .wd = {.week = 7,  .day = 7, .yr = 2100}},   // Sun Feb 21st 2100

      // Sun Feb 28th 2100. Week 1 starts Jan 4th (above). 31+28 = (8x7)+3 Feb 28th is in Week 8
      {.days = _CenturyE+31+28,     .wd = {.week = 8,  .day = 7, .yr = 2100}},   // Sun Feb 28th 2100.
      {.days = _CenturyE+31+28+1,   .wd = {.week = 9,  .day = 1, .yr = 2100}},   // Sun Mar 1st 2100

      {.days = _CenturyE+365,       .wd = {.week = 52, .day = 5, .yr = 2100}},   // Fri Dec 31st 2100
      {.days = _CenturyE+365+1,     .wd = {.week = 52, .day = 6, .yr = 2100}},   // Sat Jan 1st 2101
      {.days = _CenturyE+365+3,     .wd = {.week = 1,  .day = 1, .yr = 2101}},   // Mon Jan 3rd 2101

      {.days = _CenturyE+(4*365)-8, .wd = {.week = 51, .day = 7, .yr = 2103}},   // Sun Dec 23rd 2103.
      {.days = _CenturyE+(4*365)-1, .wd = {.week = 52, .day = 7, .yr = 2103}},   // Sun Dec 30th 2103.
      {.days = _CenturyE+(4*365),   .wd = {.week = 1,  .day = 1, .yr = 2104}},   // Mon Dec 31st 2103. Weds Jan 2nd is 1st business day of 2104 so this is already Week 01 of 2104
      {.days = _CenturyE+(4*365)+1, .wd = {.week = 1,  .day = 2, .yr = 2104}},   // Tues Jan 1st 2104
      {.days = _CenturyE+(4*365)+7, .wd = {.week = 2,  .day = 1, .yr = 2104}},   // Mon Jan 7th 2104

      // 'days' counts fully elapsed. So _Max_T_Seconds32 reaches to day AFTER last day.
      {.days = _Max_T_Seconds32/(24*3600UL), .wd = {.week = 6,  .day = 2, .yr = 2136}},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_WeekDate wd;
      S_WeekDate const *rtn = DaysToWeekDate(t->days, &wd);

      if(wd.week != t->wd.week || wd.day != t->wd.day || wd.yr != t->wd.yr) {
         printf("DaysToWeekDate() fail #%d:  expected %lu -> %u-W%02u-%u  got %u-W%02u-%u\r\n",
               i, t->days,
               t->wd.yr, t->wd.week, t->wd.day,
               wd.yr,       wd.week,    wd.day);
         TEST_FAIL();
      }
   }
}


/* -------------------------------- test_WeekDateToDays ------------------------------------ */

void test_WeekDateToDays(void)
{
   typedef struct {S_WeekDate wd; T_Days16 days; } S_Tst;

   S_Tst const tsts[] = {
      // 1st 2 days of Gregorian 2000AD last 2 days of WeekDate calendar 1999. Special case, check it.
      {.wd = {.week = 52,  .day = 6, .yr = 1999}, .days = 0},
      {.wd = {.week = 52,  .day = 7, .yr = 1999}, .days = 1},
      // But this is before Gregorian 2000AD and hence outside 'T_Seconds32'.
      {.wd = {.week = 52,  .day = 5, .yr = 1999}, .days = _Illegal_Days16},

      // WeekDate 2000AD starts on Gregorian Jan 3rd.
      {.wd = {.week = 1,   .day = 1, .yr = 2000},  .days = 3},
      {.wd = {.week = 51,  .day = 7, .yr = 2000},  .days = 366-7},
      {.wd = {.week = 52,  .day = 7, .yr = 2000},  .days = 366},
      {.wd = {.week = 1,   .day = 1, .yr = 2001},  .days = 366+1},

      {.wd = {.week = 51,  .day = 7, .yr = 2001},  .days = (2*365)-7 },          // Sun Dec 23rd 2001.
      {.wd = {.week = 52,  .day = 7, .yr = 2001},  .days = 366+365-1 },          // Sun Dec 30th 2001.
      {.wd = {.week = 1,   .day = 1, .yr = 2002},  .days = 366+365   },          // Mon Dec 31st 2001. Weds Jan 2nd is 1st business day of 2002, so Mon is in Week 01
      {.wd = {.week = 1,   .day = 2, .yr = 2002},  .days = 366+365+1 },          // Tues Jan 1st 2002. Weds Jan 2nd is 1st business day of 2002, so Tue is in Week 01
      {.wd = {.week = 1,   .day = 3, .yr = 2002},  .days = 366+365+2 },          // Weds Jan 2nd 2002. ...
      {.wd = {.week = 1,   .day = 7, .yr = 2002},  .days = 366+365+6 },          // Sun Jan 6th 2002. ...
      {.wd = {.week = 2,   .day = 1, .yr = 2002},  .days = 366+365+7 },          // Mon Jan 7th 2002. -> Week 02
      {.wd = {.week = 2,   .day = 2, .yr = 2002},  .days = 366+365+8 },          // Tues Jan 8th 2002. -> Week 02

      {.wd = {.week = 1,   .day = 2, .yr = 2003},  .days = 366+(2*365)     },    // Tues Dec 31st 2002. Thurs Jan 2nd is a business day so Tues is in Week 01 of 2003
      {.wd = {.week = 1,   .day = 3, .yr = 2003},  .days = 366+(2*365)+1   },    // Wed Jan 1st 2003...
      {.wd = {.week = 1,   .day = 7, .yr = 2003},  .days = 366+(2*365)+5   },    // Sun Jan 5th 2003... Last day of Week 1
      {.wd = {.week = 2,   .day = 1, .yr = 2003},  .days = 366+(2*365)+6   },    // Mon Jan 6th 2003... 1st day of Week 2

      {.wd = {.week = 1,   .day = 3, .yr = 2004},  .days = 366+(3*365)     },    // Wed Dec 31st 2003... Jan 2nd is Fri of same week; so already Week 1 of 2004
      {.wd = {.week = 1,   .day = 4, .yr = 2004},  .days = 366+(3*365)+1,  },    // Thurs Jan 1st 2004...  "      "       "       "        "    "
      {.wd = {.week = 1,   .day = 7, .yr = 2004},  .days = 366+(3*365)+4,  },    // Sun Jan 4th 2004.
      {.wd = {.week = 2,   .day = 1, .yr = 2004},  .days = 366+(3*365)+5,  },    // Mon Jan 5th 2004.

      // 2004 -> 2005 (2004 is a leap yr).
      {.wd = {.week = 52,  .day = 5, .yr = 2004},  .days = (2*366)+(3*365)-7, },   // Fri Dec 20th 2004.
      {.wd = {.week = 53,  .day = 1, .yr = 2004},  .days = (2*366)+(3*365)-4, },   // Mon Dec 23rd 2004.
      {.wd = {.week = 53,  .day = 5, .yr = 2004},  .days = (2*366)+(3*365),   },   // Fri Dec 31st 2004
      {.wd = {.week = 53,  .day = 6, .yr = 2004},  .days = (2*366)+(3*365)+1, },   // Sat Jan 1st 2005
      {.wd = {.week = 53,  .day = 7, .yr = 2004},  .days = (2*366)+(3*365)+2, },   // Sun Jan 2nd 2005
      {.wd = {.week = 1,   .day = 1, .yr = 2005},  .days = (2*366)+(3*365)+3, },   // Mon Jan 3rd 2005

      {.wd = {.week = 51,  .day = 7, .yr = 2005},  .days = (2*366)+(4*365)-6, },   // Sun Dec 25th 2005
      {.wd = {.week = 52,  .day = 1, .yr = 2005},  .days = (2*366)+(4*365)-5, },   // Mon Dec 26th 2005
      {.wd = {.week = 52,  .day = 6, .yr = 2005},  .days = (2*366)+(4*365),   },   // Sat Dec 31st 2005
      {.wd = {.week = 52,  .day = 7, .yr = 2005},  .days = (2*366)+(4*365)+1, },   // Sun Jan 1st 2006
      {.wd = {.week = 1,   .day = 1, .yr = 2006},  .days = (2*366)+(4*365)+2, },   // Mon Jan 2nd 2006

      {.wd = {.week = 51,  .day = 7, .yr = 2006},  .days = (2*366)+(5*365)-7, },   // Sun Dec 24th 2006
      {.wd = {.week = 52,  .day = 1, .yr = 2006},  .days = (2*366)+(5*365)-6, },   // Mon Dec 25th 2006
      {.wd = {.week = 52,  .day = 7, .yr = 2006},  .days = (2*366)+(5*365),   },   // Sun Dec 31st 2006
      {.wd = {.week = 1,   .day = 1, .yr = 2007},  .days = (2*366)+(5*365)+1, },   // Mon Jan 1st 2007
      {.wd = {.week = 1,   .day = 7, .yr = 2007},  .days = (2*366)+(5*365)+7, },   // Sun Jan 7th 2007
      {.wd = {.week = 2,   .day = 1, .yr = 2007},  .days = (2*366)+(5*365)+8, },   // Mon Jan 8th 2007

      {.wd = {.week = 51,  .day = 7, .yr = 2007},  .days = (2*366)+(6*365)-8, },   // Sun Dec 23rd 2007.
      {.wd = {.week = 52,  .day = 7, .yr = 2007},  .days = (2*366)+(6*365)-1, },   // Sun Dec 30th 2007.
      {.wd = {.week = 1,   .day = 1, .yr = 2008},  .days = (2*366)+(6*365),   },   // Mon Dec 31st 2007. Weds Jan 2nd is 1st business day of 2008, so Mon is in Week 01
      {.wd = {.week = 1,   .day = 2, .yr = 2008},  .days = (2*366)+(6*365)+1, },   // Tues Jan 1st 2008.
      {.wd = {.week = 1,   .day = 7, .yr = 2008},  .days = (2*366)+(6*365)+6, },   // Sun Jan 6th 2008.
      {.wd = {.week = 2,   .day = 1, .yr = 2008},  .days = (2*366)+(6*365)+7, },   // Mon Jan 7th 2008.

      {.wd = {.week = 51,  .day = 7, .yr = 2008},  .days = (3*366)+(6*365)-10,  },   // Sun Dec 21st 2008.
      {.wd = {.week = 52,  .day = 7, .yr = 2008},  .days = (3*366)+(6*365)-3,   },   // Sun Dec 28th 2008.
      {.wd = {.week = 1,   .day = 1, .yr = 2009},  .days = (3*366)+(6*365)-2,   },   // Mon Dec 29th 2008. Fri Jan 2nd is 1st business day of 2009. so this day is in 2009.
      {.wd = {.week = 1,   .day = 3, .yr = 2009},  .days = (3*366)+(6*365),     },   // Weds Dec 31st 2008. Fri Jan 2nd is 1st business day of 2009. so this day is in 2009.
      {.wd = {.week = 1,   .day = 4, .yr = 2009},  .days = (3*366)+(6*365)+1,   },   // Thurs Jan 1st 2009.
      {.wd = {.week = 1,   .day = 7, .yr = 2009},  .days = (3*366)+(6*365)+4,   },   // Sun Jan 4th 2009.
      {.wd = {.week = 2,   .day = 1, .yr = 2009},  .days = (3*366)+(6*365)+5,   },   // Mon Jan 5th 2009.

      {.wd = {.week = 52,  .day = 7, .yr = 2009},  .days = (3*366)+(7*365)-4,   },   // Sun Dec 27th 2009.
      {.wd = {.week = 53,  .day = 1, .yr = 2009},  .days = (3*366)+(7*365)-3,   },   // Mon Dec 28th 2009.
      {.wd = {.week = 53,  .day = 4, .yr = 2009},  .days = (3*366)+(7*365),     },   // Thurs Dec 31st 2009.
      {.wd = {.week = 53,  .day = 5, .yr = 2009},  .days = (3*366)+(7*365)+1,   },   // Fri Jan 1st 2010.... Because Jan 1st is a holiday this is still last ISO week of 2009
      {.wd = {.week = 53,  .day = 7, .yr = 2009},  .days = (3*366)+(7*365)+3,   },   // Sun Jan 3rd 2010.... is still last ISO week of 2009
      {.wd = {.week = 1,   .day = 1, .yr = 2010},  .days = (3*366)+(7*365)+4,   },   // Mon Jan 4th 2010.... starts the 2010 business year.

      // Illegal Week-Date fields are rejected.
      {.wd = {.week = 0,   .day = 1, .yr = 2000},  .days = _Illegal_Days16},
      {.wd = {.week = 54,  .day = 1, .yr = 2000},  .days = _Illegal_Days16},
      {.wd = {.week = 1,   .day = 0, .yr = 2000},  .days = _Illegal_Days16},
      {.wd = {.week = 1,   .day = 8, .yr = 2000},  .days = _Illegal_Days16},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      T_Days16 rtn = WeekDateToDays(&t->wd);

      if(rtn != t->days) {
         printf("DaysToWeekDate() fail #%d:  expected %u-W%02u-%u -> %u got %u\r\n",
               i, t->wd.yr, t->wd.week, t->wd.day, t->days, rtn );
         TEST_FAIL();
      }
   }
}

// ----------------------------------------- eof --------------------------------------------

