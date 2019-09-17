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
         failIfAbsent(b,i); }
      else {
         failIfThere(b,i); }}}

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

      // Items removed should be absent; all others should be there.
      for(U16 i = 0; i < 0x100; i++) {
         if(i >= r.from && i <= r.to) {
            failIfThere(&b,i); }
         else {
            failIfAbsent(&b,i); }}}
}

// ----------------------------------------- eof --------------------------------------------
