#include "unity.h"
#include <stdlib.h>
#include "tdd_common.h"
#include "util.h"
#include <string.h>

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

// A digest for a book where byte[0] is length and always keep.
static T_ReBook const * alwaysKeep(U8 const *bk, T_ReBook *dig) {
   dig->len = bk[0];
   dig->keep = true;
   return dig; }

/* ------------------------------ maybeKeep ------------------------------------

   A digest for a book where book[0] is length and keep if book[1] > 0.
*/
static T_ReBook const * maybeKeep(U8 const *bk, T_ReBook *dig) {
   dig->len = bk[0];
   dig->keep = bk[1] > 0 ? true : false;
   return dig; }


S_BookScanner scanner;
S_ScanStats stats;

/* ------------------------------ initScan -------------------------------------

*/
static void initScan(S_BookScanner *s, F_GetsDigest d)
{
   s->minLen = 2;          // A book is a least 2bytes
   s->digest = d;
}

/* --------------------------------- prefillStats --------------------------------------

   Prefill with 0x5A5A so we can see which fields were updated.
*/
#define _StatsInit 0x5A5A
static void prefillStats(S_ScanStats *s)
{
   s->badIdx = s->nBooks = s->nCulled = _StatsInit;
}

// ============================ Null Books tests =====================================

/* --------------------------------- test_NoBooks ---------------------------------------

   Has not even 1 book
*/
void test_NoBooks(void) {

   // A 1-byte bookshelf ('cnt' = 1)
   U8 b0[4] = {0,1,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 1};

   initScan(&scanner, alwaysKeep);
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(1, bs0->cnt);
   U8 bout[4] = {0,1,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
   // No books processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nCulled);
}

/* ============================ ends: Null Books tests =============================





// =========================== Legal Books tests ===========================


/* ---------------------------------- test_keepOne --------------------------------- */

void test_keepOne(void)
{
   // Single 4-byte book which we keep
   U8 b0[4] = {4,1,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns bs0 unchanged.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(4, bs0->cnt);
   U8 bout[4] = {4,1,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
   // 1 book processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nCulled);
}

/* ---------------------------------- test_RemoveOne --------------------------------- */

void test_RemoveOne(void)
{
   // Single 4-byte book which we remove
   U8 b0[4] = {4,0,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns bs0 with count <- 0.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   U8 bout[4] = {4,0,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);

   // 1 book processed and removed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nCulled);
}

/* ----------------------------- test_OneUndersized --------------------------------- */

void test_OneUndersized(void)
{
   // Length = 1-byte. Illegal
   U8 b0[4] = {1,10,11,12};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(4, bs0->cnt);
   U8 bout[4] = {1,10,11,12};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);

   // No book processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nCulled);

   // The bad book was the 1st one, at 'src[0]'
   TEST_ASSERT_EQUAL_UINT16(0, stats.badIdx);
}

/* ----------------------------- test_OneOversized --------------------------------- */

void test_OneOversized(void)
{
   // Length = 6, larger than bs0.
   U8 b0[4] = {6,1,11,12};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(4, bs0->cnt);
   U8 bout[4] = {6,1,11,12};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);

   // No book processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nCulled);

   // The bad book was the 1st one, at 'src[0]'
   TEST_ASSERT_EQUAL_UINT16(0, stats.badIdx);
}

/* ----------------------------- test_Keep1st_2ndUndersized ----------------------------- */

void test_Keep1st_2ndUndersized(void)
{
   // 2nd book has illegal length.
   U8 b0[6] = {3,1,8,  1,9,10};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 6};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns NULL, bs0 unchanged, but minus the illegal latter book.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
   U8 bout[6] = {3,1,8,  1,9,10};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 6);

   // No book processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit,  stats.nCulled);

   // The bad book was the 2nd one, at 'src[3]' (above)
   TEST_ASSERT_EQUAL_UINT16(1, stats.badIdx);
}

/* ----------------------------- test_Keep1st_2ndOversized ----------------------------- */

void test_Keep1st_2ndOversized(void)
{
   // 2nd book length extends past bookshelf.
   U8 b0[6] = {3,1,8,  4,9,10};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 6};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns NULL, count omits 2nd illegal book.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
   U8 bout[6] = {3,1,8,  4,9,10};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 6);
}

/* ----------------------------- test_KeepBoth -----------------------------------
*/
void test_KeepBoth(void)
{
   // 2 Books, different lengths, keep both.
   U8 b0[] = {3,1,10,  2,1};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns bs0, which is unchanged.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(5, bs0->cnt);
   U8 bout[5] = {3,1,10,  2,1};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

   // 2 books processed; none culled.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit,  stats.nCulled);
}

/* -------------------------------- test_CullLast --------------------------------

   Remove the last (of 2 books).
*/
void test_CullLast(void)
{
   // 2 Books, different lengths, remove 2nd book.
   U8 b0[] = {3,1,10,  2,0};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns bs0 with just the 1st book.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
   // Bytes for 2nd book was left in-place, even though its gone.
   U8 bout[5] = {3,1,10,  2,0};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

   // 2 books processed; 1 culled.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nCulled);
}

/* -------------------------------- test_Cull1stOfTwo ------------------------------------

   Remove 1st of 2 books.
*/
void test_Cull1stOfTwo(void)
{
   // To test that stats are cumulative; reset them just here, before running 2 CullPackedBooks() below.
   prefillStats(&stats);

   {
      // 2 Books, different lengths, remove 1st book.
      U8 b0[] = {3,0,10,  2,1};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(2, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {2,1,10,  2,1};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // 2 books processed; 1 culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nCulled);
   }

   {
      // Same as above, but put larger book 1st
      U8 b0[] = {2,0,  3,1,10};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {3,1,10,  1,10};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // Another 2 books processed; another 1 culled.
      // Add +2, +1 to exising totals.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+4, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2,  stats.nCulled);
   }
}

/* -------------------------------- test_CullBoth -------------------------------------
*/
void test_CullBoth(void)
{
   // To test that stats are cumulative; reset them just here, before running 2 CullPackedBooks() below.
   prefillStats(&stats);
   {
      // 2 Books, different lengths, remove both books.
      U8 b0[] = {3,0,10,  2,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {2,0,10,  2,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // 2 books processed; 2 culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2,  stats.nCulled);
   }

   {
      // 2 Books, different lengths, remove both books.
      U8 b0[] = {2,0,  3,0,10};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {3,0,10,  0,10};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // Another 2 books processed; another 2 culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+4, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+4,  stats.nCulled);
   }
}

/* --------------------------- test_KeepAll3 -------------------------------- */

void test_KeepAll3(void)
{
   // 3 Books, different lengths, keep all.
   U8 b0[] = {3,1,10,  2,1,  4,1,5,6};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns bs0 with 2nd & 3rd books at left.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(9, bs0->cnt);
   // Part of the orignal 3rd book is left in-place.
   U8 bout[9] = {3,1,10,  2,1,  4,1,5,6};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);

   // 3 books process; none culled.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit,   stats.nCulled);
}

/* ------------------------------ test_CullAll3 --------------------------------- */

void test_CullAll3(void)
{
   // 3 Books, different lengths, cull all.
   U8 b0[] = {3,0,10,  2,0,  4,0,5,6};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

   initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
   prefillStats(&stats);

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

   // Returns bs0 with length zero
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   // Not reqd. for test but see 3rd book now at left because 2 preceding ones were deleted.
   U8 bout[9] = {4,0,5,6,  0, 4,0,5,6};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);

   // 3 books process; 3 culled.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nCulled);
}

/* ----------------------------- test_Cull_1of3 -------------------------------
*/
void test_Cull_1of3(void)
{
   {
      // 3 Books, different lengths, remove 1st book.
      U8 b0[] = {3,0,10,  2,1, 4,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(6, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {2,1, 4,1,5,6,  1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }

   {
      // 3 Books, different lengths, remove 1st book.
      U8 b0[] = {2,0,  3,1,10,  4,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(7, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {3,1,10, 4,1,5,6,  5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }

   {
      // 3 Books, different lengths, remove 2nd book.
      U8 b0[] = {3,1,10,  2,0,  4,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(7, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {3,1,10, 4,1,5,6,  5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }

   {
      // 3 Books, different lengths, remove 2nd book.
      U8 b0[] = {2,1,  3,0,10,  4,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(6, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {2,1, 4,1,5,6,  1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }
} // test_Cull_1of3


/* --------------------------------------- test_5 ----------------------------------------------

   5 books/records; various combos. (3 above should be enough to check loop; but just be sure).
*/
void test_5(void) {
   {
      // 5 Books, different lengths, keep all
      U8 b0[] = {3,1,10,  2,1,  4,1,5,6,  5,1,7,8,9,  3,1,11};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 17};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
      prefillStats(&stats);

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with all original books.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(17, bs0->cnt);
      U8 bout[17] = {3,1,10,  2,1,  4,1,5,6,  5,1,7,8,9,  3,1,11};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 17);

      // 5 books processed; none culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+5, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nCulled);
   }
   {
      // 5 Books, different lengths, cull 2nd & 4th
      U8 b0[] = {3,1,10,  2,0,  4,1,5,6,  5,0,7,8,9,  3,1,11};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 17};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
      prefillStats(&stats);

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with all 2nd & 4th removed.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(10, bs0->cnt);
      U8 bout[10] = {3,1,10,  4,1,5,6,  3,1,11};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 10);

      // 5 books processed; none culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+5, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nCulled);
   }
}

// =========================== ends: Legal Books tests ===========================



// =========================== Bad Books Tests ====================================

/* --------------------------- test_IllegalLast -------------------------------

   Last /rightmost book illegal length.
*/
void test_IllegalLast(void)
{
   // Reset Stats just here, before running 3 CullPackedBooks() below.
   prefillStats(&stats);

   {
      // 2 Books, different lengths, remove both; but 2nd has illegal length (1).
      U8 b0[] = {3,0,10,  1,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns NULL / fail.
      TEST_ASSERT_NULL(rtn);
      // 1st book removed but undisturbed; 2nd illegal undisturbed too (even though not counted).
      TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
      U8 bout[5] = {3,0,10,  1,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // 1 book processed; 1 culled; 2nd was rejected.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nCulled);
   }

   {
      // 2 Books, different lengths, remove 2nd; but 2nd has illegal length (1).
      U8 b0[] = {3,1,10,  1,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns NULL / fail.
      TEST_ASSERT_NULL(rtn);
      // 1st book removed but undisturbed; 2nd illegal undisturbed too (even though not counted).
      TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
      U8 bout[5] = {3,1,10,  1,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // (another) 1 book processed; 0 (more)  culled; 2nd was rejected.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nCulled);
   }

   {
      // 2 Books, different lengths, remove both; but 2nd has illegal length (too long).
      U8 b0[] = {3,0,10,  3,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns NULL / fail.
      TEST_ASSERT_NULL(rtn);
      // 1st book removed but undisturbed; 2nd illegal undisturbed too (even though not counted).
      TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
      U8 bout[5] = {3,0,10,  3,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // (another) 1 book processed; 1 (more) culled; 2nd was rejected.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2,  stats.nCulled);
   }
}

/* --------------------------- test_IllegalLast_of3 ------------------------ */

void test_IllegalLast_of3(void)
{
   {
      // 3 Books, different lengths, but 3rd has illegal length 1 .
      U8 b0[] = {3,1,10,  2,1,  1,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_NULL(rtn);
      TEST_ASSERT_EQUAL_UINT8(5, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {3,1,10,  2,1,  1,1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }
   {
      // 3 Books, different lengths, but 3rd has illegal length 1 .
      U8 b0[] = {3,1,10,  2,1,  5,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_NULL(rtn);
      TEST_ASSERT_EQUAL_UINT8(5, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {3,1,10,  2,1,  5,1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }
}


// ----------------------------------------- eof --------------------------------------------
