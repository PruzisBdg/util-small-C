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
    Str_Delimiters = NULL;       // Default to SPC as the only delimiter.
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* --------------------------------- test_Str_FindWord_DfltDelimiter ------------------------------------ */

void test_Str_FindWord_DfltDelimiter(void)
{
    typedef struct { C8 const *lst; C8 const *str; U8 foundAt; } S_Tst;

    S_Tst const tsts[] = {                                                          // Looking for...
        { .lst = "", .str = "", .foundAt = 0 },                                             //    nothing in nothing -> answer is the 1st [0] of nothing.

        { .lst = "",                        .str = "aaa bbb", .foundAt = _Str_NoMatch },    //    something in nothing -> Fail.
        { .lst = "abc def ghi",             .str = "aaa bbb", .foundAt = _Str_NoMatch },    //    something in nothing -> Fail.

        { .lst = "abc def ghi",             .str = "abc",   .foundAt = 0 },                 // Matched 1st word
        { .lst = "abc def ghi",             .str = "def",   .foundAt = 1 },                 //   "     2nd  "
        { .lst = "abc def ghi",             .str = "ghi",   .foundAt = 2 },                 //   "     3rd  "
        { .lst = "ab abc abcd abcd",        .str = "abcd",  .foundAt = 2 },                 // Ignore partials in the target 'lst'
        { .lst = "abcd bcde abcde bcd bcd", .str = "bcd",   .foundAt = 3 },                 // Ignore sub-matches in the target 'lst'.

        { .lst = "   abc def ghi",          .str = "abc",   .foundAt = 0 },                 // Leading spaces in the target OK
        { .lst = "   abc    def    ghi",    .str = "ghi",   .foundAt = 2 },                 // Multiple spaces in the target OK

        { .lst = "   abc def ghi",          .str = "   abc ddd",   .foundAt = 0 },          // Leading spaces in search spec OK
     };

    for(U8 i = 0; i < RECORDS_IN(tsts); i++)
    {
        S_Tst const *t = &tsts[i];
        U8 res = Str_FindWord(t->lst, t->str);
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->foundAt, res, t->lst);
    }
}

/* --------------------------------- test_Str_FindWord_Delimiters ------------------------------------ */

void test_Str_FindWord_Delimiters(void)
{
    typedef struct { C8 const *lst; C8 const *str; U8 foundAt; } S_Tst;

    S_Tst const tsts[] = {                                                      // Looking for...
        { .lst = "abc,def,ghi",             .str = "abc",   .foundAt = 0 },         // Matched 1st word
        { .lst = "abc;def;ghi",             .str = "def",   .foundAt = 1 },         //   "     2nd  "
        { .lst = "abc def:ghi",             .str = "ghi",   .foundAt = 2 },         //   "     3rd  "

        { .lst = ", ;abc, def ghi",         .str = "abc",   .foundAt = 0 },         // Leading delimiters in the target OK
     };

     Str_Delimiters = " ,;:";

    for(U8 i = 0; i < RECORDS_IN(tsts); i++)
    {
        S_Tst const *t = &tsts[i];
        U8 res = Str_FindWord(t->lst, t->str);
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->foundAt, res, t->lst);
    }
}

/* --------------------------------- test_Str_1stWordHasChar ------------------------------------ */

PRIVATE C8 const * printMsg(C8 const *lst, C8 ch, U8 res) {
   static C8 b0[100];
   sprintf(b0, "'%c' in \"%s\" -> %d", ch, lst, res);
   return b0; }

void test_Str_1stWordHasChar(void)
{
    typedef struct { C8 const *lst; C8 ch; U8 foundAt; } S_Tst;

    S_Tst const tsts[] = {
        { .lst = "",             .ch = 'a',     .foundAt = _Str_NoMatch },           // 'ch' can't be found in an empty string.
        { .lst = " xyz  ",       .ch = 'a',     .foundAt = _Str_NoMatch },           //
        { .lst = "  ",           .ch = ' ',     .foundAt = _Str_NoMatch },           // Cannot find a delimiter.

        { .lst = "abc",          .ch = 'a',     .foundAt = 0 },
        { .lst = " abc",         .ch = 'a',     .foundAt = 1 },                     // Match is from start of word.
        { .lst = "abc",          .ch = 'c',     .foundAt = 2 },

        { .lst = "abc def",      .ch = 'd',     .foundAt = _Str_NoMatch },          // Match can only be in 1st word.
        { .lst = "abbbc",        .ch = 'b',     .foundAt = 1 },                     // Find the 1st match
        { .lst = "abc",          .ch = 'c',     .foundAt = 2 },
    };
    Str_Delimiters = NULL;

    for(U8 i = 0; i < RECORDS_IN(tsts); i++)
    {
        S_Tst const *t = &tsts[i];
        U8 res = Str_1stWordHasChar(t->lst, t->ch);
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->foundAt, res, printMsg(t->lst, t->ch, res));
    }
}

/* -------------------------------- test_Str_WordCharCnt --------------------------------------- */

void test_Str_WordCharCnt(void)
{
   typedef struct { C8 const *wd; U8 cnt;  C8 const *delimiters;} S_Tst;

   S_Tst const tsts[] = {
      { .wd = "",          .cnt = 0 },       // Empty string -> zero-count
      { .wd = "    ",      .cnt = 0 },       // Just delimiters (SPC) -> zero
      { .wd = "a",         .cnt = 1 },       //
      { .wd = "abcd",      .cnt = 4 },       //
      { .wd = "  abcd",    .cnt = 4 },       // Ignores leading delimiters
      { .wd = "  abcd efg",.cnt = 4 },       // Counts just the 1st word

      // With some addtional delimiters.
      { .wd = ";abc",    .cnt = 3,  .delimiters = " ,;" },
      { .wd = "  abcd,efg",.cnt = 4 },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      U8 res = Str_WordCharCnt(t->wd);

      C8 b0[100];
      sprintf(b0, "\"%s\" -> %d", t->wd, t->cnt);
      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->cnt, res, b0);
   }
}

/* -------------------------------------- test_Str_WordCnt --------------------------------------- */

void test_Str_WordCnt(void)
{
   typedef struct { C8 const *lst; U8 cnt;  C8 const *delimiters;} S_Tst;

   S_Tst const tsts[] = {
      { .lst = "",               .cnt = 0 },                               // Empty string -> zero-count
      { .lst = "    ",           .cnt = 0 },                               // Just delimiters (SPC) -> zero
      { .lst = ";;;",            .cnt = 0,        .delimiters = " ,;" },
      { .lst = " one",           .cnt = 1 },
      { .lst = "one,two,three",  .cnt = 3,         .delimiters = "," },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      U8 res = Str_WordCnt(t->lst);

      C8 b0[100];
      sprintf(b0, "\"%s\" -> %d", t->lst, t->cnt);
      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->cnt, res, b0);
   }

}

/* ---------------------------------- test_Str_WordInStr ----------------------------------------- */

void test_Str_WordInStr(void)
{
   // Str_WordInStr() just maps Str_FindWord() to a boolean so test just 1/0 i.e true/false.
   TEST_ASSERT_EQUAL_UINT8(1, Str_WordInStr("abc def", "abc"));
   TEST_ASSERT_EQUAL_UINT8(0, Str_WordInStr("abc def", "pqr"));
}

/* ---------------------------------- test_Str_WordsInStr ----------------------------------------- */

void test_Str_WordsInStr(void)
{
    typedef struct { C8 const *lst; C8 const *str; U8 foundAt; C8 const *delimiters; } S_Tst;

    S_Tst const tsts[] = {                                                          // Looking for...
        { .lst = "", .str = "", .foundAt = _Str_NoMatch },                                      //    nothing in nothing -> answer is the 1st [0] of nothing.
        { .lst = "",                        .str = "aaa bbb",     .foundAt = _Str_NoMatch },    //    something in nothing -> Fail.
        { .lst = "abc def ghi",             .str = "aaa bbb",     .foundAt = _Str_NoMatch },    //    something in nothing -> Fail.

        { .lst = "abc def ghi",             .str = "abc",         .foundAt = 0 },               // 1st word in 'str' is found in 'lst'.
        { .lst = "abc def ghi",             .str = "pqr abc",     .foundAt = 1 },               // 2nd word in 'str' is found in 'lst'.
        { .lst = "def abc ghi",             .str = "pqr abc",     .foundAt = 1 },               // ditto
        { .lst = "def ghi abc",             .str = "pqr abc",     .foundAt = 1 },               // ditto

        { .lst = " vuw xyz def abc ghi",    .str = " pqr abc def ",   .foundAt = 1 },           // For multiple matches, it's the 1st match in 'str'
        { .lst = " vuw xyz def abc b",      .str = " pqr mno yu b",   .foundAt = 3 },           // For multiple matches, it's the 1st match in 'str'
     };

    for(U8 i = 0; i < RECORDS_IN(tsts); i++)
    {
        S_Tst const *t = &tsts[i];
        U8 res = Str_WordsInStr(t->lst, t->str);
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->foundAt, res, t->lst);
    }
}

/* --------------------------------- test_Str_GetEndWord -------------------------------------- */

void test_Str_GetEndWord(void)
{
    typedef struct { C8 const *lst; U8 nth; C8 const * tail; C8 const *delimiters; } S_Tst;

    S_Tst const tsts[] = {                                                 // Looking for...
        { .lst = "",          .nth = 0,         .tail = "" },                    // ...end of 1st word in empty string -> EOS
        { .lst = "",          .nth = 20,        .tail = "" },                    // ...end of e.g 20th word in empty string -> EOS
        { .lst = "    ",      .nth = 11,        .tail = "" },                    // ...end of e.g 11th word in string with no words -> EOS

        { .lst = "abc def",   .nth = 0,        .tail = "c def" },                // ...end of 1st word in string with 2 words
        { .lst = "abc def ",  .nth = 1,        .tail = "f " },                   // ...end of 2nd word in string with 2 words
        { .lst = "abc def ",  .nth = 2,        .tail = "f " },                   // ...end of 3rd word, but string has just 2 words -> return end of last (2nd) word.
        { .lst = "abc def",   .nth = 2,        .tail = "f" },                    // ...end of 3rd word, but string has just 2 words -> return end of last (2nd) word.

        // Alternate delimiters
        { .lst = "abc;def;ghi",   .nth = 1,    .tail = "f;ghi", .delimiters = ",;" },
     };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      C8 const * rtn = Str_GetEndWord(t->lst, t->nth);
      C8 b0[100];

      sprintf(b0, "\"%s\"[%d] -> \"%s\"", t->lst, t->nth, rtn);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, rtn, b0);
   }
}

/* ------------------------------------- test_Str_Insert ------------------------------------- */

void test_Str_Insert(void)
{
    typedef struct { C8 const *dest; C8 const *src; U8 start; U8 cnt; C8 const *res; C8 const *delimiters; } S_Tst;

   S_Tst const tsts[] = {                                                       // Insert...
       // Into an empty destination.
       { .dest = "", .src = "", .start = 0, .cnt = 0, .res = "" },                           // none of nothing ("") into start of nothing -> nothing "".
       { .dest = "", .src = "", .start = 0, .cnt = 3, .res = "" },                           // something of nothing ("") into start of nothing -> nothing "".
       { .dest = "", .src = "", .start = 3, .cnt = 3, .res = "" },                           // something of nothing ("") into beyond start of nothing -> nothing "".
       { .dest = "", .src = "", .start = 3, .cnt = 0, .res = "" },                           // none of nothing ("") into beyond start of nothing -> nothing "".

       { .dest = "", .src = "abc def", .start = 0, .cnt = 0, .res = "" },                    // none of nothing ("") into start of nothing -> nothing "".
       { .dest = "", .src = "abc def", .start = 0, .cnt = 1, .res = "abc" },                 // 1 word from "abc def" into start of nothing -> "abc".
       { .dest = "", .src = "abc def", .start = 5, .cnt = 1, .res = "abc" },                 // 1 word from "abc def" into 6th of nothing -> "abc".
       { .dest = "", .src = "abc def", .start = 3, .cnt = 2, .res = "abc def" },             // 2 words from "abc def" into 4th of nothing -> "abc def".

       { .dest = "  ", .src = "", .start = 0, .cnt = 0, .res = "  " },                       // none of nothing ("") into start of just delimiters -> unchanged "".

       { .dest = "", .src = "  ", .start = 0, .cnt = 3, .res = "" },                         // something of just delimiters ("  ") into start of nothing -> nothing "".
       { .dest = "", .src = "  ", .start = 3, .cnt = 3, .res = "" },                         // something of just delimiters ("  ") into beyond start of nothing -> nothing "".
       { .dest = "", .src = "  ", .start = 3, .cnt = 0, .res = "" },                         // none of just delimiters ("  ") into beyond start of nothing -> nothing "".

       { .dest = "abc def ghi", .src = "", .start = 0, .cnt = 2, .res = "abc def ghi" },    // something of nothing into multiple words -> unchanged.

       { .dest = "abc def ghi", .src = "pqr",         .start = 0, .cnt = 1, .res = "pqr abc def ghi" },
       { .dest = "abc def ghi", .src = "pqr tuv",     .start = 1, .cnt = 1, .res = "abc pqr def ghi" },
       { .dest = "abc def ghi", .src = "pqr tuv xyz", .start = 1, .cnt = 2, .res = "abc pqr tuv def ghi" },

       { .dest = "abc def ghi", .src = "..pqr",         .start = 0, .cnt = 1, .res = "pqr;abc def ghi", .delimiters = ";. " },

       // Request to insert more words than there are in 'src' -> All words in src are inserted
       { .dest = "abc def ghi", .src = "pqr tuv", .start = 1, .cnt = 10, .res = "abc pqr tuv def ghi", .delimiters = "" },

       { .dest = "abc def ghi", .src = "pqr", .start = 1, .cnt = 1, .res = "abc pqr def ghi" },

       // With non-default delimiter; must use the delimiter at the insertion point.
       { .dest = "abc...def.ghi", .src = "pqr", .start = 1, .cnt = 1, .res = "abc...pqr.def.ghi", .delimiters = "." },

       // With multiple delimiters; the delimiter used for insertion must be the 1st one
       { .dest = "abc;..def.ghi", .src = "pqr", .start = 1, .cnt = 1, .res = "abc;..pqr;def.ghi", .delimiters = ";." },
    };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      U8 dest[100];
      strcpy(dest, t->dest);

      C8 const * rtn = Str_Insert(dest, t->src, t->start, t->cnt);
      C8 b0[100];

      sprintf(b0, "\"%s\" + \"%s\"[%d],%d -> \"%s\"", t->dest, t->src, t->start, t->cnt, dest);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->res, dest, b0);
   }
}

/* ------------------------------------- test_Str_Delete ------------------------------------- */

//PUBLIC U8 GENERIC * Str_Delete( U8 GENERIC *lst, U8 start, U8 cnt )


void test_Str_Delete(void)
{
    typedef struct { C8 const *lst; U8 start, cnt; C8 const * res; C8 const *delimiters; } S_Tst;

    S_Tst const tsts[] = {                                                       // Insert...

    };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      U8 dest[100];
      strcpy(dest, t->lst);

      C8 const * rtn = Str_Delete(dest, t->start, t->cnt);
      C8 b0[100];

      sprintf(b0, "\"%s\" + \"%s\"[%d],%d -> \"%s\"", t->lst, t->start, t->cnt, dest);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->res, dest, b0);
   }
}

// ----------------------------------------- eof --------------------------------------------
