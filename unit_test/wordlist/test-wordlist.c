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


/* ------------------------------------- test_Str_GetNthWord ------------------------------------------ */

void test_Str_GetNthWord(void)
{
   typedef struct { C8 const *lst; U8 get; C8 const *tail; C8 const *delimiters; } S_Tst;

   S_Tst const tsts[] = {                                                          // Looking for...
      { .lst = "",      .get = 0,      .tail = "",       .delimiters = "" },           // Empty list, nothing to find -> ""
      { .lst = "...",   .get = 0,      .tail = "",       .delimiters = "." },          // Just delimiters, nothing to find, also -> ""
      { .lst = "...",   .get = 5,      .tail = "",       .delimiters = "." },          // Same, but ask for nth which doesn't exist -> ""


      { .lst = ";abc tail",         .get = 0,   .tail = "abc tail",     .delimiters = "; " },         // Request 1st word
      { .lst = "abc...def.ghi",     .get = 1,   .tail = "def.ghi",      .delimiters = "." },          // 2nd word
      { .lst = "abc..def..ghi",     .get = 2,   .tail = "ghi",          .delimiters = "." },          // Last word

      { .lst = "abc..def..ghi",     .get = 3,   .tail = "",             .delimiters = "." },          // But there's no 4th word
      { .lst = "abc..def..ghi..",   .get = 3,   .tail = "",             .delimiters = "." },          // Boof past trailing delimiters looking for that 4th word.
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      C8 const * tail = Str_GetNthWord(t->lst, t->get);

      C8 b0[100];
      sprintf(b0, "\"%s\"[%d] -> \"%s\"", t->lst, t->get, tail);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, tail, b0);
   }
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

/* --------------------------------- test_Str_TailAfterWord ------------------------------------ */

void test_Str_TailAfterWord(void)
{
    typedef struct { C8 const *lst; C8 const *str; C8 const *tail; } S_Tst;

    S_Tst const tsts[] = {                                                  // Looking for...
        { .lst = "abc,def,ghi",         .str = "abc",   .tail = ",def,ghi" },   // Matched 1st word; tail is remainder of 'lst'.
        { .lst = "abc,def,ghi",         .str = "def",   .tail = ",ghi" },       // Matched 2nd word; ...
        { .lst = "abc,def,ghi",         .str = "ghi",   .tail = "" },           //     "   3nd word; only the end of  'lst' remains.

        { .lst = "",                    .str = "",      .tail = "" },           // nothing in nothing -> end of 'lst'.

        // Failure to match.
        { .lst = "",                    .str = "abc",   .tail = NULL },         // Something in nothing -> NULL (not found)
        { .lst = "abc,def,ghi",         .str = "123",   .tail = NULL },         // No match -> NULL
        { .lst = "abc,def,ghi",         .str = "ab",    .tail = NULL },         // Partial match -> NULL
        { .lst = "abc,def,ghi",         .str = "abcd",  .tail = NULL },         // Over-match -> NULL

    };

    Str_Delimiters = " ,;:";

    for(U8 i = 0; i < RECORDS_IN(tsts); i++)
    {
        S_Tst const *t = &tsts[i];
        U8 const * res = Str_TailAfterWord(t->lst, t->str);

        if(res == NULL && t->tail == NULL)
        {
            continue;
        }
        else if(t->tail != NULL && res == NULL)
        {
            C8 b0[100];
            sprintf(b0, "tst #%d expected %s, got NULL", i, t->tail);
            TEST_FAIL_MESSAGE(b0);
        }
        else if(t->tail == NULL && res != NULL)
        {
            C8 b0[100];
            sprintf(b0, "tst #%d expected NULL, got %s", i, res);
            TEST_FAIL_MESSAGE(b0);
        }
        else
        {
            TEST_ASSERT_EQUAL_STRING_MESSAGE(t->tail, res, t->lst);
        }
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

void test_Str_Delete(void)
{
    typedef struct { C8 const *lst; U8 start, cnt; C8 const * res; C8 const *delimiters; } S_Tst;

    S_Tst const tsts[] = {                                                       // Delete...
       { .lst = "", .start = 0, .cnt = 0, .res = "", .delimiters = "" },                     // none of nothing ("") from start of nothing -> nothing "".
       { .lst = "", .start = 0, .cnt = 3, .res = "", .delimiters = "" },                     // etc...
       { .lst = "", .start = 5, .cnt = 0, .res = "", .delimiters = "" },                     // etc...
       { .lst = "", .start = 2, .cnt = 3, .res = "", .delimiters = "" },                     // etc...  All the nothings.

       { .lst = "..", .start = 0, .cnt = 0, .res = "..", .delimiters = "." },                // Preserves delimiters.

       { .lst = "abc def", .start = 0, .cnt = 0, .res = "abc def", .delimiters = "" },
       { .lst = "abc.def.", .start = 0, .cnt = 1, .res = "def.", .delimiters = "." },
       { .lst = "abc.def.ghi", .start = 1, .cnt = 1, .res = "abc.ghi", .delimiters = "." },

       { .lst = "abc...def.", .start = 0, .cnt = 1, .res = "..def.", .delimiters = "." },

       { .lst = "abc;def.ghi:..jkl", .start = 1, .cnt = 2, .res = "abc;..jkl", .delimiters = ".;:" },  // The ':' at end of excised segment gets dropped.

       { .lst = "abc;def.ghi:..jkl", .start = 1, .cnt = 3, .res = "abc;", .delimiters = ".;:" },

       { .lst = "abc;.;.def.ghi:..jkl", .start = 1, .cnt = 5, .res = "abc;.;.", .delimiters = ".;:" },   // Removing tail preserves all trailing delimiters on head.
    };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      U8 dest[100];
      strcpy(dest, t->lst);

      C8 const * rtn = Str_Delete(dest, t->start, t->cnt);

      C8 b0[100];
      sprintf(b0, "\"%s\" - [%d],%d -> \"%s\"", t->lst, t->start, t->cnt, dest);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->res, dest, b0);
   }
}

/* ------------------------------------- test_Str_Replace ------------------------------------- */

void test_Str_Replace(void)
{
   typedef struct { C8 const *str; C8 const *find; C8 const *subst; U8 nCut, nSubst; C8 const * res; C8 const *delimiters; } S_Tst;

    S_Tst const tsts[] = {                                                       // Replace...
       // Empty source string, hence nothing to find.
       { .str = "", .find = "", .subst = "", .nCut = 0, .nSubst = 0, .res = "", .delimiters = "" },
       { .str = "", .find = "", .subst = "", .nCut = 0, .nSubst = 3, .res = "", .delimiters = "" },
       { .str = "", .find = "", .subst = "", .nCut = 10, .nSubst = 0, .res = "", .delimiters = "" },
       { .str = "", .find = "abc", .subst = "cde", .nCut = 0, .nSubst = 0, .res = "", .delimiters = "" },
       { .str = "", .find = "abc", .subst = "cde", .nCut = 1, .nSubst = 1, .res = "", .delimiters = "" },

       { .str = "abc", .find = "abc", .subst = "cde", .nCut = 1, .nSubst = 1, .res = "cde", .delimiters = "" },

       // Match & replace from 1st word
       { .str = "abc def ghi jkl", .find = "abc", .subst = "XYZ", .nCut = 1,  .nSubst = 1, .res =  "XYZ def ghi jkl", .delimiters = "" },   // Cut 1st
       { .str = "abc def ghi jkl", .find = "abc", .subst = "XYZ", .nCut = 2,  .nSubst = 1, .res =  "XYZ ghi jkl", .delimiters = "" },       // Cut 1st & 2nd
       { .str = "abc def ghi jkl", .find = "abc", .subst = "XYZ", .nCut = 4,  .nSubst = 1, .res =  "XYZ", .delimiters = "" },               // Cut all
       { .str = "abc def ghi jkl", .find = "abc", .subst = "XYZ", .nCut = 4,  .nSubst = 1, .res =  "XYZ", .delimiters = "" },               // Cut all
       { .str = "abc def ghi jkl", .find = "abc", .subst = "XYZ", .nCut = 20, .nSubst = 1, .res =  "XYZ", .delimiters = "" },               // Try to cut more word than there are -> cuts all.

       // Match and replace some middle words
       { .str = "abc def ghi jkl", .find = "def", .subst = "UVW XYZ", .nCut = 1, .nSubst = 2, .res =  "abc UVW XYZ ghi jkl", .delimiters = "" },
       { .str = "abc def ghi jkl", .find = "def", .subst = "UVW XYZ", .nCut = 2, .nSubst = 2, .res =  "abc UVW XYZ jkl", .delimiters = "" },
       { .str = "abc def ghi jkl", .find = "def", .subst = "UVW XYZ", .nCut = 3, .nSubst = 2, .res =  "abc UVW XYZ", .delimiters = "" },
       { .str = "abc def ghi jkl", .find = "def", .subst = "UVW XYZ", .nCut = 9, .nSubst = 2, .res =  "abc UVW XYZ", .delimiters = "" },    // Try to cut more words than are -> cuts all.

       // Match and replace at end
       { .str = "abc def ghi jkl", .find = "jkl", .subst = "UVW XYZ", .nCut = 1, .nSubst = 2, .res =  "abc def ghi UVW XYZ", .delimiters = "" },
       { .str = "abc def ghi jkl", .find = "jkl", .subst = "UVW XYZ", .nCut = 3, .nSubst = 2, .res =  "abc def ghi UVW XYZ", .delimiters = "" },
       { .str = "abc def ghi jkl", .find = "jkl", .subst = "UVW XYZ", .nCut = 9, .nSubst = 2, .res =  "abc def ghi UVW XYZ", .delimiters = "" },    // Try to cut more words than are -> cuts all.

       // Non-matches -> original string unchanged.
       { .str = "abc def ghi jkl", .find = "",     .subst = "UVW XYZ", .nCut = 1, .nSubst = 2, .res =  "abc def ghi jkl", .delimiters = "" },      // Empty match string.
       { .str = "abc def ghi jkl", .find = "de",   .subst = "UVW XYZ", .nCut = 1, .nSubst = 2, .res =  "abc def ghi jkl", .delimiters = "" },      // Undermatch
       { .str = "abc def ghi jkl", .find = "defg", .subst = "UVW XYZ", .nCut = 1, .nSubst = 2, .res =  "abc def ghi jkl", .delimiters = "" },      // Overmatch.

       /* Multiple delimiters.

          1. Here ';' is the 1st delimiter on the delimiters-list so it's ';' which is inserted if the tail of the
          original string is spliced on after an insertion.

          2. Leading and trailing delimiters in the insertion string are deleted.
       */
       { .str = "abc def ghi jkl", .find = "def", .subst = "UVW...XYZ",   .nCut = 1, .nSubst = 2, .res =  "abc UVW...XYZ;ghi jkl", .delimiters = "; ." },
       { .str = "abc def ghi jkl", .find = "def", .subst = "..UVW.XYZ..", .nCut = 1, .nSubst = 2, .res =  "abc UVW.XYZ;ghi jkl",   .delimiters = "; ." },
    };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      U8 dest[100];
      strcpy(dest, t->str);

      Str_Replace(dest, t->find, t->subst, t->nCut, t->nSubst);

      C8 b0[100];
      sprintf(b0, "\"%s\" ? \"%s\"[%d], \"%s\"[%d] -> \"%s\"", t->str, t->find, t->nCut, t->subst, t->nSubst, dest);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->res, dest, b0);
   }
}

/* --------------------------------------- test_Str_1stWordsMatch ------------------------------------------- */

void test_Str_1stWordsMatch(void)
{
   typedef struct { U8 const *w1; U8 const *w2; BIT res; C8 const * delimiters; } S_Tst;

   S_Tst const tsts[] = {
      // Empty word(s) -> No match
      { .w1 = "",          .w2 = "",               .res = 0,      .delimiters = "" },
      { .w1 = "word1",     .w2 = "",               .res = 0, },
      { .w1 = "",          .w2 = "word2",          .res = 0, },

      // Partials -> No match
      { .w1 = "word",      .w2 = "word2",          .res = 0, },
      { .w1 = "word1",     .w2 = "word2",          .res = 0, },
      { .w1 = "ord",       .w2 = "word",           .res = 0, },

      // Exact match.
      { .w1 = "word",      .w2 = "word",           .res = 1, },

      // Ignore leading and trailing delimiters; also additional words.
      { .w1 = "...word",      .w2 = ".word",       .res = 1,  .delimiters = "." },
      { .w1 = "...word.more", .w2 = ".word.more",  .res = 1,  .delimiters = "." },

      // The 1st words are the ones which must match. Later matches don't count.
      { .w1 = "...w1.more",   .w2 = ".w2.more",    .res = 0,  .delimiters = "." },
      { .w1 = ".w1.w2",       .w2 = ".w2.w1",      .res = 0,  .delimiters = "." },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      BIT res = Str_1stWordsMatch(t->w1, t->w2);

      C8 b0[100];
      sprintf(b0, "\"%s\" vs \"%s\"-> \"%d\"", t->w1, t->w2, res);
      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->res, res, b0);
   }
}

/* --------------------------------------- test_Str_CopyNthWord ------------------------------------------- */

void test_Str_CopyNthWord(void)
{
   typedef struct { C8 const *lst; C8 const *out; U8 get, maxChars, res; C8 const * delimiters; } S_Tst;

   S_Tst const tsts[] = {
      // Request 1st word from empty list. Allow no chars -> unmodified output.
      { .lst = "",  .out = "unmodified", .get = 0, .maxChars = 0, .res = 0, .delimiters = "" },
      { .lst = "",  .out = "unmodified", .get = 3, .maxChars = 5, .res = 0, .delimiters = "" },
      // A list with just delimiters is also empty.
      { .lst = "...",  .out = "unmodified", .get = 3, .maxChars = 5, .res = 0, .delimiters = "." },

      // Copy of 1st word
      { .lst = "abc",      .out = "abc",     .get = 0, .maxChars = 5, .res = 3, .delimiters = "." },    // 'abc' -> 3 chars
      // Copy-out limit is exactly word size -> success.
      { .lst = "abcd",     .out = "abcd",    .get = 0, .maxChars = 4, .res = 4, .delimiters = "." },
      // Not enough room to copy out. Returns '0' (fail) and copies out 1st 3 chars.
      { .lst = "abcd",     .out = "abc",     .get = 0, .maxChars = 3, .res = 0, .delimiters = "." },

      // 1st word with leading & trailing delimiters. Just the word is copied out.
      { .lst = "..abc..",  .out = "abc",     .get = 0, .maxChars = 5, .res = 3, .delimiters = "." },    // 'abc' -> 3 chars
      { .lst = "..abcd..", .out = "abcd",    .get = 0, .maxChars = 4, .res = 4, .delimiters = "." },
      { .lst = "..abcd..", .out = "abc",     .get = 0, .maxChars = 3, .res = 0, .delimiters = "." },

      // get 2nd & last words
      { .lst = "abc def ghi",    .out = "def",     .get = 1, .maxChars = 5, .res = 3, .delimiters = ". " },    // 'abc' -> 3 chars
      { .lst = "abc def ghi",    .out = "ghi",     .get = 2, .maxChars = 5, .res = 3, .delimiters = ". " },    // 'abc' -> 3 chars
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      U8 out[100];
      strcpy(out, "unmodified");

      BIT res = Str_CopyNthWord(t->lst, out, t->get, t->maxChars);

      C8 b0[100];
      sprintf(b0, "\"%s\"[%d],max = %d -> (\"%s\",%d)", t->lst, t->get, t->maxChars, out, res);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->out, out, b0);
      TEST_ASSERT_EQUAL_UINT8_MESSAGE(t->res, res, b0);
   }
}



// ----------------------------------------- eof --------------------------------------------
