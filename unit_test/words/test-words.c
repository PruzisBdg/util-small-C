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

typedef struct { C8 const *in; C8 const *rtn; C8 const * delimiters; } S_Tst;

/* Loop through 'numTests' 't', applying cvts(). Stop and message on 1st fail.
*/
#define _CopyOut false
#define _InPlace true
PRIVATE void runTests(S_Tst const *t, U8 numTests, C8 const *(*cvts)(C8*, C8 const *), C8 const * funcName, bool inPlace)
{
   for(U8 i = 0; i < numTests; i++, t++)
   {
      if(t->delimiters != NULL) { Str_Delimiters = t->delimiters; }

      C8 out[100];
      C8 const *p;

      if(inPlace == true)
      {
         strcpy(out, t->in);
         p = cvts(out, out);
      }
      else
      {
         strcpy(out, "unchanged");
         p = cvts(out, t->in);
      }

      C8 b0[100];
      sprintf(b0, "Output str mismatch: %s()[%d] ", funcName, i);
      C8 b1[100];
      sprintf(b1, "Return str mismatch: %s()[%d] ", funcName, i);

      TEST_ASSERT_EQUAL_STRING_MESSAGE(t->rtn, out, b0);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(p, out, b1);
   }
}

/* ------------------------------------- test_Word_1stToUpper ------------------------------------------ */

void test_Word_1stToUpper(void)
{
   S_Tst const tsts[] = {
      { .in = "",            .rtn = "unchanged" },                               // Empty str -> unchanged
      { .in = "   ",         .rtn = "unchanged" },                               // Just default delimiter -> unchanged
      { .in = " .;",         .rtn = "unchanged",      .delimiters = "; ." },

      { .in = "ABC",         .rtn = "ABC",            .delimiters = ". " },      // Single word, already uppercase.
      { .in = "a_123bc",     .rtn = "A_123BC",        .delimiters = ". " },      // Single word
      { .in = "..abc..",     .rtn = "ABC",            .delimiters = ". " },      // With leading trailing delimiterss
      { .in = "abc def",     .rtn = "ABC",            .delimiters = ". " },      // Multiple word. Just the first is copied over, uppercase
   };

   runTests(tsts, RECORDS_IN(tsts), Word_1stToUpper, "Word_1stToUpper", _CopyOut);
}

/* ------------------------------------- test_Word_1stToLower ------------------------------------------ */

void test_Word_1stToLower(void)
{
   S_Tst const tsts[] = {
      { .in = "",            .rtn = "unchanged" },                               // Empty str -> unchanged
      { .in = "   ",         .rtn = "unchanged" },                               // Just default delimiter -> unchanged
      { .in = " .;",         .rtn = "unchanged",      .delimiters = "; ." },

      { .in = "abc",         .rtn = "abc",            .delimiters = ". " },      // Single word, already lowercase -> copied out unchanged
      { .in = "ABC123D",     .rtn = "abc123d",        .delimiters = ". " },      // Single word -> all letters lowercased.
      { .in = "..ABC..",     .rtn = "abc",            .delimiters = ". " },      // With leading trailing delimiterss
      { .in = "ABC DEF",     .rtn = "abc",            .delimiters = ". " },      // Multiple word. Just the first is copied over, uppercase
   };

   runTests(tsts, RECORDS_IN(tsts), Word_1stToLower, "Word_1stToLower", _CopyOut);
}

/* ------------------------------------- test_Word_1stToCamel ------------------------------------------ */

void test_Word_1stToCamel(void)
{
   S_Tst const tsts[] = {
      { .in = "",            .rtn = "unchanged" },                               // Empty str -> unchanged
      { .in = "   ",         .rtn = "unchanged" },                               // Just default delimiter -> unchanged
      { .in = " .;",         .rtn = "unchanged",      .delimiters = "; ." },

      { .in = "abc",         .rtn = "Abc",            .delimiters = ". " },      // Single lowercased word -> cameled
      { .in = "abc123d",     .rtn = "Abc123d",        .delimiters = ". " },      // Single word, already lowercase
      { .in = "..abc..",     .rtn = "Abc",            .delimiters = ". " },      // With leading trailing delimiters
      { .in = "abc def",     .rtn = "Abc",            .delimiters = ". " },      // Multiple word. Just the first is copied over, cameled.

      { .in = "ALLCAPS",      .rtn = "Allcaps",       .delimiters = ". " },      // All uppercase -> cameled...

      // ... But if already cameled, it's left alone.
      { .in = "AlreadyCamelCase",   .rtn = "AlreadyCamelCase",      .delimiters = ". " },
   };

   runTests(tsts, RECORDS_IN(tsts), Word_1stToCamel, "Word_1stToCamel", _CopyOut);
}

/* ----------------------------- test_Word_1stToUpper_InPlace ------------------------------------------ */

void test_Word_1stToUpper_InPlace(void)
{
   S_Tst const tsts[] = {
      { .in = "",            .rtn = "" },                                        // Empty str -> empty
      { .in = "   ",         .rtn = "   "  },                                    // Just default delimiter -> unchanged
      { .in = "...",         .rtn = "...",      .delimiters = "." },             // Defined delimiter -> unchanged.

      { .in = "ABC",         .rtn = "ABC",            .delimiters = ". " },      // Single word, already uppercase.
      { .in = "a_123bc",     .rtn = "A_123BC",        .delimiters = ". " },      // Single word
      { .in = "..abc..",     .rtn = "..ABC..",        .delimiters = ". " },      // With leading trailing delimiters
      { .in = "abc def",     .rtn = "ABC def",        .delimiters = ". " },      // Multiple word. Just the first is copied over, uppercase
   };

   runTests(tsts, RECORDS_IN(tsts), Word_1stToUpper, "Word_1stToUpper", _InPlace);
}

/* -----------------------------test_Word_1stToLower_InPlace ------------------------------------------ */

void test_Word_1stToLower_InPlace(void)
{
   S_Tst const tsts[] = {
      { .in = "",            .rtn = "" },                                        // Empty str -> empty
      { .in = "   ",         .rtn = "   "  },                                    // Just default delimiter -> unchanged
      { .in = "...",         .rtn = "...",      .delimiters = "." },             // Defined delimiter -> unchanged.

      { .in = "abc",         .rtn = "abc",            .delimiters = ". " },      // Single word, already lowercase -> copied out unchanged
      { .in = "ABC123D",     .rtn = "abc123d",        .delimiters = ". " },      // Single word -> all letters lowercased.
      { .in = "..ABC..",     .rtn = "..abc..",        .delimiters = ". " },      // With leading trailing delimiters
      { .in = "ABC DEF",     .rtn = "abc DEF",        .delimiters = ". " },      // Multiple word. Just the first is copied over, uppercase
   };

   runTests(tsts, RECORDS_IN(tsts), Word_1stToLower, "Word_1stToLower", _InPlace);
}

/* ----------------------------- test_Word_1stToCamel_InPlace ------------------------------------------ */

void test_Word_1stToCamel_InPlace(void)
{
   S_Tst const tsts[] = {
      { .in = "",            .rtn = "" },                                        // Empty str -> empty
      { .in = "   ",         .rtn = "   "  },                                    // Just default delimiter -> unchanged
      { .in = "...",         .rtn = "...",      .delimiters = "." },             // Defined delimiter -> unchanged.

      { .in = "abc",         .rtn = "Abc",            .delimiters = ". " },      // Single lowercased word -> cameled
      { .in = "abc123d",     .rtn = "Abc123d",        .delimiters = ". " },      // Single word, already lowercase
      { .in = "..abc..",     .rtn = "..Abc..",        .delimiters = ". " },      // With leading trailing delimiters
      { .in = "abc def",     .rtn = "Abc def",        .delimiters = ". " },      // Multiple word. Just the first is copied over, cameled.

      { .in = "ALLCAPS",      .rtn = "Allcaps",       .delimiters = ". " },      // All uppercase -> cameled...

      // ... But if already cameled, it's left alone.
      { .in = "AlreadyCamelCase",   .rtn = "AlreadyCamelCase",      .delimiters = ". " },
   };

   runTests(tsts, RECORDS_IN(tsts), Word_1stToCamel, "Word_1stToCamel", _InPlace);
}

// ----------------------------------------- eof --------------------------------------------
