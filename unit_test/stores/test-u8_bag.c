#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

// ---------------------------------------------------------------------------------------------
void initRandom(S_U8bag const *b) {
   U8 i; U8 *p;
   for(i = 0, p = (U8*)b; i < sizeof(S_U8bag); i++, p++) {
      *p = randU8(); }
}

// --------------------------------------------------------------------------------------------
static void testEmpty(S_U8bag const *b) {
   for(U16 i = 0; i < 0x100; i++) {
      if( U8bag_Contains(b, i) == true) {
         TEST_FAIL(); }}}

// --------------------------------------------------------------------------------------------
static void fill(S_U8bag const *b) {
   for(U16 i = 0; i < 0x100; i++) {
      U8bag_AddOne(&b, i); }}

/* ----------------------------------- test_NewEmptyBag ---------------------------------------- */

void test_NewEmptyBag(void)
{
   S_U8bag b;
   initRandom(&b);
   U8bag_Clear(&b);
   testEmpty(&b);

   fill(&b);
   U8bag_Clear(&b);
   testEmpty(&b);
}

/* ----------------------------------- test_AddOneRemoveOne ---------------------------------------- */

void test_AddOneRemoveOne(void)
{
   S_U8bag b;
   U8bag_Clear(&b);

   for(U16 i = 0; i < 0x100; i++) {

      U8bag_AddOne(&b, i);

      for(U16 j = 0; j < 0x100; j++) {
         if(j == i) {
            if( U8bag_Contains(&b, j) == false) {
               TEST_FAIL(); }}
         else {
            if( U8bag_Contains(&b, j) == true) {
               TEST_FAIL(); }}}

      U8bag_RemoveOne(&b, i);
      testEmpty(&b);
   }
}

// ----------------------------------------- eof --------------------------------------------
