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

// ----------------------------------------- eof --------------------------------------------
