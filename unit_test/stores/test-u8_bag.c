#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
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

// ----------- Randomise internal structure of 'b'
void initRandom(S_U8bag const *b) {
   U8 i; U8 *p;
   for(i = 0, p = (U8*)b; i < sizeof(S_U8bag); i++, p++) {
      *p = randU8(); }}

// ------------ Random contents (not the same as above)
S_U8bag const *someRandomBag(S_U8bag *b) {
   U8bag_Clear(b);
   for(U16 i = 0; i < 0x100; i++) {
      U8 n = randU8();
      if( n > 0x7F ) {
         U8bag_AddOne(b, i); }}}

// --------------------------------------------------------------------------------------------
static void testEmpty(S_U8bag const *b) {
   for(U16 i = 0; i < 0x100; i++) {
      if( U8bag_Contains(b, i) == true) {
         TEST_FAIL(); }}}

// --------------------------------------------------------------------------------------------
static void testFull(S_U8bag const *b) {
   for(U16 i = 0; i < 0x100; i++) {
      if( U8bag_Contains(b, i) == false) {
         TEST_FAIL(); }}}

// --------------------------------------------------------------------------------------------
static void fill(S_U8bag *b) {
   for(U16 i = 0; i < 0x100; i++) {
      U8bag_AddOne(b, i); }}

// --------------------------------------------------------------------------------------------
typedef struct {U8 from, to;} S_RangeU8;

static S_RangeU8 const *randomRangeU8(S_RangeU8 *r) {
   U8 a = randU8();
   U8 b = randU8();
   if(a > b)
      { r->from = b; r->to = a; }
   else
      { r->from = a; r->to = b; }
}

// --------------------------------------------------------------------------------------------
void failIfAbsent(S_U8bag const *b, U8 i) {
   if( U8bag_Contains(b, i) == false) {
      TEST_FAIL(); }}

// --------------------------------------------------------------------------------------------
void failIfThere(S_U8bag const *b, U8 i) {
   if( U8bag_Contains(b, i) == true) {
      TEST_FAIL(); }}

// --------------------------------------------------------------------------------------------
static void failIfNotEqual(S_U8bag const *a, S_U8bag const *b) {
   if( U8bag_sEqual(a, b) == false) {
      TEST_FAIL(); }}

// --------------------------------------------------------------------------------------------
static void failIfEqual(S_U8bag const *a, S_U8bag const *b) {
   if( U8bag_sEqual(a, b) == true) {
      TEST_FAIL(); }}

static void checkExactlytRangePresent(S_U8bag const *b, S_RangeU8 const *r) {
   // Items added should all be there; all others should be absent.
   for(U16 i = 0; i < 0x100; i++) {
      if(i >= r->from && i <= r->to) {
         failIfAbsent(b,i); }
      else {
         failIfThere(b,i); }}}

static void checkExactlytRangeAbsent(S_U8bag const *b, S_RangeU8 const *r) {
   // Items added should all be there; all others should be absent.
   for(U16 i = 0; i < 0x100; i++) {
      if(i >= r->from && i <= r->to) {
         failIfThere(b,i); }
      else {
         failIfAbsent(b,i); }}}

// ============================== TESTS START HERE ==============================================

/* ----------------------------------- test_NewEmptyBag ---------------------------------------- */

void test_NewEmptyBag(void)
{
   S_U8bag b;
   // Clear() should empty a bag filled with random stuff
   initRandom(&b);
   U8bag_Clear(&b);
   testEmpty(&b);
   // Clear should empty a completely filled bag.
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
            failIfAbsent(&b,j); }
         else {
            failIfThere(&b,j); }}

      U8bag_RemoveOne(&b, i);
      testEmpty(&b);
   }
}

/* --------------------------------- test_Fill_OneByOne ---------------------------------------------- */

void test_Fill_OneByOne(void)
{
   S_U8bag b;
   U8bag_Clear(&b);     // Starting with an empty bag.
   fill(&b);            // Fills one-by-one.
   testFull(&b);
}

/* -------------------------------- test_DuplicateFill --------------------------------------------- */

void test_DuplicateFill(void)
{
   S_U8bag b;
   U8bag_Clear(&b);

   U8 wereFilled[0x100] = { [0 ... 0xFF] = 0 };

   // Add (150) randomly chosen items. Because there are just 256 slots in the bag there will be duplicates.
   #define _NTst 150
   for(U8 i = 0; i < _NTst; i++) {
      U8 fi = randU8();
      U8bag_AddOne(&b, fi);
      wereFilled[fi] = 1; }      // Record that this item was added (at least once).

   // Check that any item added at least once is there AND that any items which were
   // never added are NOT there.
   for(U16 i = 0; i < 0x100; i++) {
      if( wereFilled[i] == 1) {
         failIfAbsent(&b,i); }
      else {
         failIfThere(&b,i); }}

   #undef _NTst
}

/* ----------------------------------- test_AddRange --------------------------------------------- */

void test_AddRange(void)
{
   S_U8bag b;

   // Check 20 randomly chosen range-adds
   for(U8 j = 0; j < 20; j++) {
      U8bag_Clear(&b);                    // Start empty

      S_RangeU8 r;
      randomRangeU8(&r);                  // Choose random (legal) range
      U8bag_AddRange(&b, r.from, r.to);   // Add it.
      checkExactlytRangePresent(&b, &r); }
}

/* ----------------------------------- test_RemoveRange --------------------------------------------- */

void test_RemoveRange(void)
{
   S_U8bag b;
   U8bag_Clear(&b);                       // Start empty

   // Check 20 randomly chosen range-removes
   for(U8 j = 0; j < 20; j++) {
      fill(&b);

      S_RangeU8 r;
      randomRangeU8(&r);                     // Choose random (legal) range
      U8bag_RemoveRange(&b, r.from, r.to);   // Remove it.
      checkExactlytRangeAbsent(&b, &r); }
}

/* ---------------------------------- test_Equal ----------------------------------------------- */

void test_Equal(void)
{
   S_U8bag a, b;

   // Two empty bags should be equal.
   U8bag_Clear(&a);
   U8bag_Clear(&b);
   failIfNotEqual(&a, &b);
   failIfNotEqual(&a, &a);                   // Bag should always be equal to itself.

   // Two full bags should be equal.
   fill(&a);
   fill(&b);
   failIfNotEqual(&a, &b);
   failIfNotEqual(&a, &a);                   // Bag should always be equal to itself.

   // Add the same random ranges to two bags. Check they are equal after the adds.
   for(U8 i = 0; i < 20; i++) {
      S_RangeU8 r;

      randomRangeU8(&r);
      U8bag_Clear(&a);
      U8bag_Clear(&b);

      U8bag_AddRange(&a, r.from, r.to);      // Add range 1st bag.
      failIfEqual(&a, &b);                   // Should now be different from 2nd.
      failIfNotEqual(&a, &a);                // Bag should always be equal to itself.
      U8bag_AddRange(&b, r.from, r.to);      // Add same range to 1st bag
      failIfNotEqual(&a, &b);                // 1st and 2nd bags should be equal.
   }
}

/* ------------------------------------ test_Copy --------------------------------------------- */

void test_Copy(void)
{
   S_U8bag a, b;

   // Do some random copies, Check for equality after each copy.
   for(U8 i = 0; i < 20; i++) {
      U8bag_Clear(&a);
      U8bag_Clear(&b);

      S_RangeU8 r;
      randomRangeU8(&r);
      U8bag_AddRange(&a, r.from, r.to);      // Add range 1st bag.
      U8bag_Copy(&b, &a);                    // Copy to 2nd bag.
      failIfNotEqual(&a, &b);                // 1st and 2nd bags should be equal.
   }
}

/* ------------------------------------ test_SelfCopy --------------------------------------------- */

void test_SelfCopy(void)
{
   S_U8bag a, b;

   // Do some random self-copies. Check for equality after each copy.
   for(U8 i = 0; i < 20; i++) {
      U8bag_Clear(&a);
      U8bag_Clear(&b);

      S_RangeU8 r;
      randomRangeU8(&r);
      U8bag_AddRange(&a, r.from, r.to);      // Add range 1st bag.
      U8bag_AddRange(&b, r.from, r.to);      // Add same range 2nd bag.
      U8bag_Copy(&a, &a);                    // Copy 1st bag to itself
      failIfNotEqual(&a, &b);                // 1st and 2nd bags should still be equal.
   }
}


/* ---------------------------------- test_Invert ----------------------------------------------- */

void test_Invert(void)
{
   S_U8bag a, b, c;
   U8bag_Clear(&a);
   U8bag_Clear(&b);
   U8bag_Clear(&c);

   // Inverting an empty bag should give a full bag.
   fill(&b);
   U8bag_Invert(&a);
   failIfNotEqual(&a, &b);

   // Inverting a full bag should give an empty one.
   U8bag_Invert(&b);
   failIfNotEqual(&b, &c);

   // Inverting a bag twice should return it to how it was.
   for(U8 i = 0; i < 20; i++) {
      U8bag_Clear(&a);
      U8bag_Clear(&b);

      S_RangeU8 r;
      randomRangeU8(&r);
      U8bag_AddRange(&a, r.from, r.to);      // Add range 1st bag.
      U8bag_AddRange(&b, r.from, r.to);      // Add range 2nd (reference) bag.
      U8bag_Invert(&a);                      // Invert 1st bag twice.
      U8bag_Invert(&a);
      failIfNotEqual(&a, &b);                // 1st bag should return to what it was.
   }
}

/* ---------------------------------- test_ListCh --------------------------------------------- */

void test_ListCh(void)
{
   S_U8bag a;
   U8bag_Clear(&a);

   U8 b0[256];

   U8bag_ListCh(b0, &a);
   TEST_ASSERT_EQUAL_STRING("", b0);

   S_RangeU8 r;
   r.from = 'a';
   r.to = 'g';
   U8bag_AddRange(&a, r.from, r.to);
   U8bag_RemoveOne(&a, 'd');
   U8bag_ListCh(b0, &a);
   TEST_ASSERT_EQUAL_STRING("abcefg", b0);

   // These control chars are printed as escapes i.e "\r", "\n" etc.
   U8bag_AddOne(&a, 0x0A);
   U8bag_AddOne(&a, 0x0D);
   U8bag_AddOne(&a, 0x09);    // Tab

   // Note chars are listed in ascending order of numeric value.
   U8bag_ListCh(b0, &a);
   TEST_ASSERT_EQUAL_STRING("\\t\\n\\rabcefg", b0);
}

/* ------------------------------------ test_List ----------------------------------------- */

void test_List(void)
{
   S_U8bag a;
   U8bag_Clear(&a);

   #define _BSize ((U16)MAX_U8+1)
   U8 out[_BSize];
   memset(out, 0x5A, _BSize);
   U8 ref[_BSize];
   memcpy(ref, out, _BSize);

   U8 cnt;

   // Listing an empty bag should give zero 'cnt' and leave out[] unchanged.
   U8bag_List(out, &a, &cnt);
   TEST_ASSERT_EQUAL_UINT8(0, cnt);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(ref, out, _BSize);

   /* Add some numbers. List-count should be correct. out[] should have exactly the number in U8bag 'a'
      in ascending order. Reset of out[] should be unmodified.
   */
   S_RangeU8 r;
   r.from = 1;
   r.to = 9;
   U8bag_AddRange(&a, r.from, r.to);
   U8bag_AddOne(&a, 55);
   U8bag_AddOne(&a, 56);
   U8bag_List(out, &a, &cnt);
   TEST_ASSERT_EQUAL_UINT8(11, cnt);
   U8 r2[_BSize] = {1,2,3,4,5,6,7,8,9,55,56, [11 ... 0xFF] = 0x5A};
   TEST_ASSERT_EQUAL_UINT8_ARRAY( r2, out, _BSize );

   #undef _BSize

}

// ----------------------------------------- eof --------------------------------------------
