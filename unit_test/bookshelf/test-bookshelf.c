#include "unity.h"
#include <stdlib.h>
#include "tdd_common.h"
#include "util.h"
#include <string.h>
#include "arith.h"

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

// A digest for a book where byte[0] is length and always keep.
static bookPack_S_Digest const * alwaysKeep(U8 const *bk, bookPack_S_Digest *dig) {
   dig->len = bk[0];
   dig->keep = true;
   return dig; }

/* ------------------------------ maybeKeep ------------------------------------

   A digest for a book where book[0] is length and keep if book[1] > 0.
*/
static bookPack_S_Digest const * maybeKeep(U8 const *bk, bookPack_S_Digest *dig) {
   dig->len = bk[0];
   dig->keep = bk[1] > 0 ? true : false;
   return dig; }


bookPack_S_Packer scanner;
bookPack_S_Stats stats;

/* ------------------------------ initScan -------------------------------------

*/
static void initScan(bookPack_S_Packer *s, bookPack_F_GetsDigest d)
{
   s->minLen = 2;          // A book is a least 2bytes
   s->digest = d;
}

/* --------------------------------- prefillStats --------------------------------------

   Prefill with 0x5A5A so we can see which fields were updated.
*/
#define _StatsInit 0x5A5A
static void prefillStats(bookPack_S_Stats *s)
{
   s->errIdx = s->nBooks = s->nKept = _StatsInit;
}

// ============================ Null Books tests =====================================

/* --------------------------------- test_NoBooks ---------------------------------------

   Has not even 1 book
*/
void test_NoBooks(void) {

   // A 0-byte bookshelf ('cnt' = 0)
   U8 b0[4] = {0,1,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 1};

   initScan(&scanner, alwaysKeep);
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns NULL, bs0 <- 0.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   U8 bout[4] = {0,1,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
   // No books processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nKept);
}

/* ============================ ends: Null Books tests =============================





// =========================== Legal Books tests ===========================


/* ---------------------------------- test_keepOne --------------------------------- */

void test_keepOne(void)
{
   // Single 4-byte book which we keep
   U8 b0[4] = {4,1,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns bs0 unchanged.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(4, bs0->cnt);
   U8 bout[4] = {4,1,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
   // 1 book processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nKept);
}

/* ---------------------------------- test_RemoveOne --------------------------------- */

void test_RemoveOne(void)
{
   // Single 4-byte book which we remove
   U8 b0[4] = {4,0,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns bs0 with count <- 0.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   U8 bout[4] = {4,0,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);

   // 1 book processed and removed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit,   stats.nKept);
}

/* ----------------------------- test_OneUndersized --------------------------------- */

void test_OneUndersized(void)
{
   // Length = 1-byte. Illegal
   U8 b0[4] = {1,10,11,12};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns NULL, bs0 set to zero bytes.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   U8 bout[4] = {1,10,11,12};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);

   // No book processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nKept);

   // The bad book was the 1st one, at 'src[0]'
   TEST_ASSERT_EQUAL_UINT16(0, stats.errIdx);
}

/* ----------------------------- test_OneOversized --------------------------------- */

void test_OneOversized(void)
{
   // Length = 6, larger than bs0.
   U8 b0[4] = {6,1,11,12};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns NULL, bs0 set to zero bytes.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   U8 bout[4] = {6,1,11,12};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);

   // No book processed.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.nKept);

   // The bad book was the 1st one, at 'src[0]'
   TEST_ASSERT_EQUAL_UINT16(0, stats.errIdx);
}

/* ----------------------------- test_Keep1st_2ndUndersized ----------------------------- */

void test_Keep1st_2ndUndersized(void)
{
   // 2nd book has illegal length.
   U8 b0[6] = {3,1,8,  1,9,10};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 6};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns NULL, bs0 unchanged, but minus the illegal latter book.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
   U8 bout[6] = {3,1,8,  1,9,10};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 6);

   // One book processed and kept.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nKept);

   // The bad book was the 2nd one, at 'src[3]' (above)
   TEST_ASSERT_EQUAL_UINT16(3, stats.errIdx);
}

/* ----------------------------- test_Keep1st_2ndOversized ----------------------------- */

void test_Keep1st_2ndOversized(void)
{
   // 2nd book length extends past bookshelf.
   U8 b0[6] = {3,1,8,  4,9,10};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 6};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns NULL, count omits 2nd illegal book.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
   U8 bout[6] = {3,1,8,  4,9,10};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 6);

   // One book processed and kept.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nKept);

   // The bad book was the 2nd one, at 'src[3]' (above)
   TEST_ASSERT_EQUAL_UINT16(3, stats.errIdx);
}

/* ----------------------------- test_KeepBoth -----------------------------------
*/
void test_KeepBoth(void)
{
   // 2 Books, different lengths, keep both.
   U8 b0[] = {3,1,10,  2,1};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns bs0, which is unchanged.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(5, bs0->cnt);
   U8 bout[5] = {3,1,10,  2,1};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

   // 2 books processed; none culled.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nKept);

   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);
}

/* -------------------------------- test_CullLast --------------------------------

   Remove the last (of 2 books).
*/
void test_CullLast(void)
{
   // 2 Books, different lengths, remove 2nd book.
   U8 b0[] = {3,1,10,  2,0};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns bs0 with just the 1st book.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
   // Bytes for 2nd book was left in-place, even though its gone.
   U8 bout[5] = {3,1,10,  2,0};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

   // 2 books processed; 1 kept.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nKept);

   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);
}

/* -------------------------------- test_Cull1stOfTwo ------------------------------------

   Remove 1st of 2 books.
*/
void test_Cull1stOfTwo(void)
{
   // To test that stats are cumulative; reset them just here, before running 2 bookPack_CullRepack() below.
   prefillStats(&stats);

   {
      // 2 Books, different lengths, remove 1st book.
      U8 b0[] = {3,0,10,  2,1};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(2, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {2,1,10,  2,1};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // 2 books processed; 1 culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nKept);
   }

   {
      // Same as above, but put larger book 1st
      U8 b0[] = {2,0,  3,1,10};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {3,1,10,  1,10};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // Another 2 books processed; another 1 culled.
      // Add +2, +1 to exising totals.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+4, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2,  stats.nKept);
   }
   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);
}

/* -------------------------------- test_CullBoth -------------------------------------
*/
void test_CullBoth(void)
{
   // To test that stats are cumulative; reset them just here, before running 2 bookPack_CullRepack() below.
   prefillStats(&stats);
   {
      // 2 Books, different lengths, remove both books.
      U8 b0[] = {3,0,10,  2,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
      // Both books culled; neither moved.
      U8 bout[5] = {3,0,10,  2,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // 2 books processed; 2 culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit,   stats.nKept);
   }

   {
      // 2 Books, different lengths, remove both books.
      U8 b0[] = {2,0,  3,0,10};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
      // Both books culled; neither moved.
      U8 bout[5] = {2,0,  3,0,10};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // Another 2 books processed; another 2 culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+4, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit,   stats.nKept);
   }
   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);
}

/* --------------------------- test_KeepAll3 -------------------------------- */

void test_KeepAll3(void)
{
   // 3 Books, different lengths, keep all.
   U8 b0[] = {3,1,10,  2,1,  4,1,5,6};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns bs0 with 2nd & 3rd books at left.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(9, bs0->cnt);
   // Part of the orignal 3rd book is left in-place.
   U8 bout[9] = {3,1,10,  2,1,  4,1,5,6};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);

   // 3 books process; none culled.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nKept);

   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);
}

/* ------------------------------ test_CullAll3 --------------------------------- */

void test_CullAll3(void)
{
   // 3 Books, different lengths, cull all.
   U8 b0[] = {3,0,10,  2,0,  4,0,5,6};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

   initScan(&scanner, maybeKeep);   // Set scanner
   prefillStats(&stats);

   S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

   // Returns bs0 with length zero
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   // Not reqd. for test but see 3rd book now at left because 2 preceding ones were deleted.
   U8 bout[9] = {3,0,10,  2,0,  4,0,5,6};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);

   // 3 books process; 3 culled.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nBooks);
   TEST_ASSERT_EQUAL_UINT16(_StatsInit,   stats.nKept);

   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);
}

/* ----------------------------- test_Cull_1of3 -------------------------------
*/
void test_Cull_1of3(void)
{
   {
      // 3 Books, different lengths, remove 1st book.
      U8 b0[] = {3,0,10,  2,1, 4,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

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

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

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

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

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

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(6, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {2,1, 4,1,5,6,  1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }
   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);

} // test_Cull_1of3


/* --------------------------------------- test_5 ----------------------------------------------

   5 books/records; various combos. (3 above should be enough to check loop; but just be sure).
*/
void test_5(void) {
   {
      // 5 Books, different lengths, keep all
      U8 b0[] = {3,1,10,  2,1,  4,1,5,6,  5,1,7,8,9,  3,1,11};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 17};

      initScan(&scanner, maybeKeep);   // Set scanner
      prefillStats(&stats);

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with all original books.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(17, bs0->cnt);
      U8 bout[17] = {3,1,10,  2,1,  4,1,5,6,  5,1,7,8,9,  3,1,11};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 17);

      // 5 books processed; none culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+5, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+5, stats.nKept);
   }
   {
      // 5 Books, different lengths, cull 2nd & 4th
      U8 b0[] = {3,1,10,  2,0,  4,1,5,6,  5,0,7,8,9,  3,1,11};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 17};

      initScan(&scanner, maybeKeep);   // Set scanner
      prefillStats(&stats);

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with all 2nd & 4th removed.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(10, bs0->cnt);
      U8 bout[10] = {3,1,10,  4,1,5,6,  3,1,11};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 10);

      // 5 books processed; none culled.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+5, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nKept);
   }
   // No error index posted.
   TEST_ASSERT_EQUAL_UINT16(_StatsInit, stats.errIdx);
}

// =========================== ends: Legal Books tests ===========================



// =========================== Bad Books Tests ====================================

/* --------------------------- test_IllegalLast -------------------------------

   Last /rightmost book illegal length.
*/
void test_IllegalLast(void)
{
   // Reset Stats just here, before running 3 bookPack_CullRepack() below.
   prefillStats(&stats);

   {
      // 2 Books, different lengths, remove both; but 2nd has illegal length (1).
      U8 b0[] = {3,0,10,  1,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns NULL / fail.
      TEST_ASSERT_NULL(rtn);
      // 1st book removed but undisturbed; 2nd illegal undisturbed too (even though not counted).
      TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
      U8 bout[5] = {3,0,10,  1,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // 1 book processed; 1 culled; 2nd was rejected.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit,   stats.nKept);

      // 2nd book at b0[3] is bad
      TEST_ASSERT_EQUAL_UINT16(3, stats.errIdx);
   }

   {
      // 2 Books, different lengths, remove 2nd; but 2nd has illegal length (1).
      U8 b0[] = {3,1,10,  1,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set scanner
      stats.errIdx = _StatsInit;       // Reset just the error index; make sure it gets written.

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns NULL / fail.
      TEST_ASSERT_NULL(rtn);
      // 1st book removed but undisturbed; 2nd illegal undisturbed too (even though not counted).
      TEST_ASSERT_EQUAL_UINT16(3, bs0->cnt);
      U8 bout[5] = {3,1,10,  1,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // (another) 1 book processed; 0 (more)  culled; 2nd was rejected.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+2, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nKept);

      // 2nd book at b0[3] is bad
      TEST_ASSERT_EQUAL_UINT16(3, stats.errIdx);
   }

   {
      // 2 Books, different lengths, remove both; but 2nd has illegal length (too long).
      U8 b0[] = {3,0,10,  3,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set scanner
      stats.errIdx = _StatsInit;       // Reset just the error index; make sure it gets written.

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns NULL / fail.
      TEST_ASSERT_NULL(rtn);
      // 1st book removed but undisturbed; 2nd illegal undisturbed too (even though not counted).
      TEST_ASSERT_EQUAL_UINT16(0, bs0->cnt);
      U8 bout[5] = {3,0,10,  3,0};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);

      // (another) 1 book processed; 1 (more) culled; 2nd was rejected.
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+3, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(_StatsInit+1,  stats.nKept);

      // 2nd book at b0[3] is bad
      TEST_ASSERT_EQUAL_UINT16(3, stats.errIdx);
   }
}

/* --------------------------- test_IllegalLast_of3 ------------------------ */

void test_IllegalLast_of3(void)
{
   // Reset Stats just here, before running 3 bookPack_CullRepack() below.
   prefillStats(&stats);
   {
      // 3 Books, different lengths, but 3rd has illegal length 1 .
      U8 b0[] = {3,1,10,  2,1,  1,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set scanner

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_NULL(rtn);
      TEST_ASSERT_EQUAL_UINT8(5, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {3,1,10,  2,1,  1,1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);

      // 3rd book at b0[5] is bad
      TEST_ASSERT_EQUAL_UINT16(5, stats.errIdx);
   }
   {
      // 3 Books, different lengths, but 3rd has illegal length 1 .
      U8 b0[] = {3,1,10,  2,1,  5,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      initScan(&scanner, maybeKeep);   // Set scanner
      stats.errIdx = _StatsInit;       // Reset just the error index; make sure it gets written.

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, bs0, &stats);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_NULL(rtn);
      TEST_ASSERT_EQUAL_UINT16(5, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {3,1,10,  2,1,  5,1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);

      // 3rd book at b0[5] is bad
      TEST_ASSERT_EQUAL_UINT16(5, stats.errIdx);
   }
}

/* ==================================== Polar Datapoints ========================================

   bookPack_CullRepack() is used to strip empty Datapoints from a reply to a Datapoints request.
*/


/* ---------------- Exceprts of Aquarius, just for these Test ------------------------------------

   Don't #include real Aquarius; quote just what we need to check bookPack_CullRepack().
*/

// Datapoint with just UTC or UTC followed by Ambient Tmpr.
typedef S32 T_UTC;
typedef U8  T_Tmpr;
typedef struct __attribute__((packed)) {U8 subKey, len; S32 secs; } Dpt_UTC;
typedef struct __attribute__((packed)) {U8 subkey, len; T_Tmpr degC;} Dpt_Tmpr;
typedef struct __attribute__((packed)) {Dpt_UTC utc; Dpt_Tmpr tmpr; } Dpts_UTCwTmpr;

typedef U16 T_AqKey;
typedef U16 T_AqStatus;
typedef struct __attribute__((packed)) {T_AqKey theKeyAcked; T_AqStatus status; } KeyAck;

// Aquarius Block header (not the Frame header)
typedef U16 T_AqPayloadLen;
typedef struct __attribute__((packed)) {T_AqKey key; T_AqPayloadLen len; } S_AqBlockHdr;

typedef struct __attribute__((packed)) {
   S_AqBlockHdr hdr;
   union {
      Dpt_UTC        utc;        // Just UTC
      Dpts_UTCwTmpr  utcTmpr;    // UTC plus (just) Ambient tmpr, which we summarise.
      KeyAck         keyAck;};   // Reply to Datapoint request ends with KeyAck.
} S_AqBlock;


/* ------------------------------ (Reports on) Datapoints Digest --------------------------------------

   Collect / accumulate these on reading through and condensing a 'bookshelf' of Datapoints.
*/
typedef struct {
   struct {S8 min, max;} ambT;      // Collect highest and lowest Ambient degC.
   T_AqStatus  keyAckSts;           // From KeyAck at end of Datapoints reply.
   S32         latestUTC;           // Youngest timestamp in the original Frame.
} S_DataPtDigest;

S_DataPtDigest reDataPts;


static void initDataPtsDigest(S_DataPtDigest *s) {
   s->ambT.min = 100; s->ambT.max = -100;    // Init for min/max capture
   s->keyAckSts = 0x5A5A;                    // SO we can tell we got something
   s->latestUTC = 0x5A5A5A5A; }              // Likewise; should always get a legit UTC.



static S_BufC8 chainDataPtsDigest(S_BufC8 const *src, S_DataPtDigest const *s) {
   U16 nChars = snprintf(src->cs, src->cnt, "ambT %u-%u keyAck %04Xh utc %lu",
                           s->ambT.min, s->ambT.max, s->keyAckSts, s->latestUTC);
   nChars = MinU16(nChars, src->cnt);
   return (S_BufC8){.cs = src->cs + nChars, .cnt = src->cnt - nChars}; }

static C8 const * printDataPtsDigest(S_BufC8 const *src, S_DataPtDigest const *s) {
   snprintf(src->cs, src->cnt, "ambT %u-%u keyAck %04Xh utc %lu",
                           s->ambT.min, s->ambT.max, s->keyAckSts, s->latestUTC);
   return src->cs; }

// -------------------------- ends: reports on Datapoints Digest --------------------------------



/* -------------------------------- digestAqDataPt -------------------------------------------

   Return a digest of an Aquarius Block which is a part of a reply to a Datapoints request.
   The Block can be a Datapoint or, last in the reply, a KeyAck.

   All Datapoints (should) start with UTC Subkey. After that they may have more Subkeys. This
   digest discards Datapoints which are UTC-only. Datapoints which also have Ambient Temperature
   as the 2nd Subkey; the digest accumulates minimum and maximum Ambient and discards the Subkeys.

   Other Subkeys are retained as-is.
*/
static bookPack_S_Digest const * digestAqDataPt(U8 const *bk, bookPack_S_Digest *dig) {

   #define _Subkey_UTC     0x01
   #define _Subkey_AmbT    0x14
   #define _AqKey_DataPt   0x001D
   #define _AqKey_KeyAck   0x0003

   #define _leToU16(p) leToU16((U8 const*)(p))
   #define _leToU32(p) leToU32((U8 const*)(p))

   /* <U16 0x001D, nBytes, payload[nBytes]>
         payload = <U8 subkey, nBytes, value[nBytes]>

      Calendar time = <U16 0x1D, 6, <U8 1=calendar_time, 4, utc[4]>>
   */
   S_AqBlock const *aq = (S_AqBlock const *)bk;

   // Whole 'bk' is <U16 key, len; U8 payload[len]>
   dig->len = sizeof(T_AqKey) + sizeof(T_AqPayloadLen) + _leToU16(&aq->hdr.len);

   T_AqKey key = _leToU16(&aq->hdr.key);

   // Except for KeyAck at the end, all Blocks in a Datapoints reply should be Datapoints
   if(key == _AqKey_DataPt) {

      // 1st Subkey is Calendar Time (UTC). It should always be.
      if(aq->utc.subKey == _Subkey_UTC && aq->utc.len == sizeof(T_UTC)) {

         // Mark this latest UTC. Assumes UTC in successive Datapoints ascend in time.
         reDataPts.latestUTC = _leToU32(&aq->utc.secs);

         // If Datapoint has only UTC then discard.
         if(dig->len == sizeof(S_AqBlockHdr) + sizeof(Dpt_UTC)) {
            dig->keep = false; }

         /* else if Datapoint just UTC followed by just Ambient Temperature then update
            min and max Ambient for this Aquarius Frame. Then discard the Datapoint.
         */
         else if(dig->len == sizeof(S_AqBlockHdr) + sizeof(Dpt_UTC) + sizeof(Dpt_Tmpr) &&
                 aq->utcTmpr.tmpr.subkey == _Subkey_AmbT && aq->utcTmpr.tmpr.len == sizeof(T_Tmpr)) {

            S_DataPtDigest *d = &reDataPts;
            T_Tmpr degC = aq->utcTmpr.tmpr.degC;

            if(degC > d->ambT.max) {d->ambT.max = degC;}
            if(degC < d->ambT.min) {d->ambT.min = degC;}
            dig->keep = false; }
         // anythings else; keep the Datapoint.
         else {
            dig->keep = true; }}
      else {
         dig->keep = true; }}

   // else trailing KeyAck. Note reply the status.
   else if(key == _AqKey_KeyAck) {
      reDataPts.keyAckSts = _leToU16(&aq->keyAck.status);
      dig->keep = true; }

   return dig; }



/* -------------------------------------- test_PolarDatapoints_1739 -------------------------------------

   Reply to Datpoints query from a Polar with no flow. Most are empty timestamps.
*/
void test_PolarDatapoints_1739(void)
{
      U8 b0[] = {
         0xA5, 0x0F, 0xCD, 0x96,       // Frame header.
                                 0x1D, 0x00, 0x3F, 0x00, 0x01, 0x04, 0x10, 0x0E, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x38, 0x33, 0x00, 0x08, 0x04, 0x00,
         0x00, 0x00, 0x00, 0x0E, 0x01, 0x03, 0x10, 0x01, 0x7C, 0x11, 0x01, 0x00, 0x12, 0x01, 0x00, 0x13, 0x01, 0x00, 0x14, 0x01, 0x19, 0x15, 0x02, 0x09, 0x00, 0x16, 0x01, 0xFD, 0x17, 0x06, 0x00, 0x1F,
         0x1E, 0x00, 0x0B, 0x0A, 0x18, 0x01, 0x12, 0x1D, 0x00, 0x18, 0x00, 0x01, 0x04, 0x20, 0x1C, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x38, 0x31, 0x00, 0x11, 0x01, 0x8C, 0x13, 0x01, 0x46,
         0x14, 0x01, 0x18, 0x1D, 0x00, 0x1D, 0x00, 0x01, 0x04, 0x30, 0x2A, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x30, 0x31, 0x00, 0x14, 0x01, 0x17, 0x17, 0x06, 0x1F, 0x1F, 0x1F, 0x0B, 0x0B,
         0x0B, 0x18, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x40, 0x38, 0x00, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x50, 0x46, 0x00, 0x00, 0x14, 0x01, 0x16, 0x1D, 0x00, 0x09, 0x00, 0x01,
         0x04, 0x60, 0x54, 0x00, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x70, 0x62, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x80, 0x70, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00,
         0x01, 0x04, 0x90, 0x7E, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xA0, 0x8C, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xB0, 0x9A, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04,
         0xC0, 0xA8, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xD0, 0xB6, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xE0, 0xC4, 0x00, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xF0, 0xD2,
         0x00, 0x00, 0x14, 0x01, 0x16, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x00, 0xE1, 0x00, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x10, 0xEF, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00,
         0x01, 0x04, 0x20, 0xFD, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x30, 0x0B, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x40, 0x19, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04,
         0x50, 0x27, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x60, 0x35, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x70, 0x43, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x81, 0x51,
         0x01, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x90, 0x5F, 0x01, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xA0, 0x6D, 0x01, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xB0,
         0x7B, 0x01, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xC0, 0x89, 0x01, 0x00, 0x14, 0x01, 0x16, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xD0, 0x97, 0x01, 0x00, 0x1D, 0x00, 0x09,
         0x00, 0x01, 0x04, 0xE0, 0xA5, 0x01, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xF0, 0xB3, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x00, 0xC2, 0x01, 0x00, 0x1D, 0x00,
         0x06, 0x00, 0x01, 0x04, 0x10, 0xD0, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x20, 0xDE, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x30, 0xEC, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00,
         0x01, 0x04, 0x40, 0xFA, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x50, 0x08, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x60, 0x16, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04,
         0x70, 0x24, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x80, 0x32, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x90, 0x40, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xA0, 0x4E,
         0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xB0, 0x5C, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xC0, 0x6A, 0x02, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xD0, 0x78, 0x02, 0x00,
         0x14, 0x01, 0x18, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xE0, 0x86, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xF0, 0x94, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x01, 0xA3, 0x02,
         0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x10, 0xB1, 0x02, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x20, 0xBF, 0x02, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x30, 0xCD,
         0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x40, 0xDB, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x50, 0xE9, 0x02, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x60, 0xF7, 0x02, 0x00,
         0x14, 0x01, 0x16, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x70, 0x05, 0x03, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x80, 0x13, 0x03, 0x00, 0x14, 0x01, 0x16, 0x1D, 0x00, 0x06,
         0x00, 0x01, 0x04, 0x90, 0x21, 0x03, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xA0, 0x2F, 0x03, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xB0, 0x3D, 0x03, 0x00, 0x1D, 0x00,
         0x06, 0x00, 0x01, 0x04, 0xC0, 0x4B, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xD0, 0x59, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xE0, 0x67, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00,
         0x01, 0x04, 0xF0, 0x75, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x00, 0x84, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x10, 0x92, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04,
         0x20, 0xA0, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x30, 0xAE, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x40, 0xBC, 0x03, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x50, 0xCA,
         0x03, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x60, 0xD8, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x70, 0xE6, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x81,
         0xF4, 0x03, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x90, 0x02, 0x04, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xA0, 0x10, 0x04, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04,
         0xB0, 0x1E, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xC0, 0x2C, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xD0, 0x3A, 0x04, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xE0, 0x48,
         0x04, 0x00, 0x14, 0x01, 0x16, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xF0, 0x56, 0x04, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x00, 0x65, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00,
         0x01, 0x04, 0x10, 0x73, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x20, 0x81, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x30, 0x8F, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04,
         0x40, 0x9D, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x50, 0xAB, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x60, 0xB9, 0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x70, 0xC7,
         0x04, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x80, 0xD5, 0x04, 0x00, 0x1D, 0x00, 0x23, 0x00, 0x01, 0x04, 0x30, 0x2A, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x38, 0x33, 0x00, 0x11,
         0x01, 0x00, 0x13, 0x01, 0x00, 0x14, 0x01, 0x19, 0x17, 0x06, 0x00, 0x1F, 0x1F, 0x00, 0x0B, 0x0B, 0x18, 0x01, 0x32, 0x1D, 0x00, 0x23, 0x00, 0x01, 0x04, 0x40, 0x38, 0x00, 0x00, 0x04, 0x07, 0x32,
         0x39, 0x34, 0x34, 0x30, 0x31, 0x00, 0x11, 0x01, 0x8C, 0x13, 0x01, 0x46, 0x14, 0x01, 0x18, 0x17, 0x06, 0x1F, 0x1F, 0x1F, 0x0B, 0x0B, 0x0B, 0x18, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04,
         0x50, 0x46, 0x00, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x60, 0x54, 0x00, 0x00, 0x14, 0x01, 0x19, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x70, 0x62, 0x00, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01,
         0x04, 0x80, 0x70, 0x00, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x90, 0x7E, 0x00, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xA0, 0x8C, 0x00, 0x00, 0x1D,
         0x00, 0x06, 0x00, 0x01, 0x04, 0xB0, 0x9A, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xC0, 0xA8, 0x00, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xD0, 0xB6, 0x00, 0x00, 0x14, 0x01, 0x18,
         0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xE0, 0xC4, 0x00, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xF0, 0xD2, 0x00, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x00, 0xE1, 0x00,
         0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x10, 0xEF, 0x00, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x20, 0xFD, 0x00, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x30, 0x0B,
         0x01, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x40, 0x19, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x50, 0x27, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x60,
         0x35, 0x01, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x70, 0x43, 0x01, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x81, 0x51, 0x01, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x06,
         0x00, 0x01, 0x04, 0x90, 0x5F, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xA0, 0x6D, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xB0, 0x7B, 0x01, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01,
         0x04, 0xC0, 0x89, 0x01, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xD0, 0x97, 0x01, 0x00, 0x14, 0x01, 0x19, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0xE0, 0xA5, 0x01, 0x00, 0x1D,
         0x00, 0x06, 0x00, 0x01, 0x04, 0xF0, 0xB3, 0x01, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x00, 0xC2, 0x01, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x10, 0xD0, 0x01, 0x00,
         0x14, 0x01, 0x17, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x20, 0xDE, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x30, 0xEC, 0x01, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x40, 0xFA, 0x01,
         0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x50, 0x08, 0x02, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x60, 0x16, 0x02, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x70, 0x24,
         0x02, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x80, 0x32, 0x02, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x90, 0x40, 0x02, 0x00, 0x14, 0x01, 0x17, 0x1D,
         0x00, 0x06, 0x00, 0x01, 0x04, 0xA0, 0x4E, 0x02, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xB0, 0x5C, 0x02, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xC0, 0x6A, 0x02, 0x00,
         0x14, 0x01, 0x17, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xD0, 0x78, 0x02, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0xE0, 0x86, 0x02, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x09,
         0x00, 0x01, 0x04, 0xF0, 0x94, 0x02, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x01, 0xA3, 0x02, 0x00, 0x14, 0x01, 0x17, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x10, 0xB1, 0x02,
         0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x20, 0xBF, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x30, 0xCD, 0x02, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x40, 0xDB,
         0x02, 0x00, 0x14, 0x01, 0x19, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x50, 0xE9, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x60, 0xF7, 0x02, 0x00, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x70,
         0x05, 0x03, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04, 0x80, 0x13, 0x03, 0x00, 0x14, 0x01, 0x18, 0x1D, 0x00, 0x06, 0x00, 0x01, 0x04, 0x90, 0x21, 0x03, 0x00, 0x1D, 0x00, 0x09, 0x00, 0x01, 0x04,
         0xA0, 0x2F, 0x03, 0x00, 0x14, 0x01, 0x17, 0x03, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00,

         0xA2, 0xD7 };     // ends with CRC.

      // Datapoints start at [4], after the frame header. Count omits the header and CRC.
      S_BufU8 *src = &(S_BufU8){.bs = &b0[4], .cnt = RECORDS_IN(b0)-6};

      U16 startCnt = src->cnt;

      initScan(&scanner, digestAqDataPt);
      bookPack_InitStats(&stats);
      initDataPtsDigest(&reDataPts);

      S_BufU8 const * rtn = bookPack_CullRepack(&scanner, src, &stats);

      TEST_ASSERT_EQUAL_PTR(src, rtn);
      TEST_ASSERT_EQUAL_PTR(src->bs, rtn->bs);
      TEST_ASSERT_EQUAL_UINT16(214, rtn->cnt);

      // Start with 144 Datapoints plus KeyAck. Retain 5 plus KeyAck.
      TEST_ASSERT_EQUAL_UINT16(145, stats.nBooks);
      TEST_ASSERT_EQUAL_UINT16(6,   stats.nKept);

      U8 bout[] = {
         // @ 0[67]
         0x1D, 0x00, 0x3F, 0x00, 0x01, 0x04, 0x10, 0x0E, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x38, 0x33, 0x00, 0x08, 0x04, 0x00,
         0x00, 0x00, 0x00, 0x0E, 0x01, 0x03, 0x10, 0x01, 0x7C, 0x11, 0x01, 0x00, 0x12, 0x01, 0x00, 0x13, 0x01, 0x00, 0x14, 0x01, 0x19, 0x15, 0x02, 0x09, 0x00, 0x16, 0x01, 0xFD, 0x17, 0x06, 0x00, 0x1F,
         0x1E, 0x00, 0x0B, 0x0A, 0x18, 0x01, 0x12,
         // @ 67[28]
         0x1D, 0x00, 0x18, 0x00, 0x01, 0x04, 0x20, 0x1C, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x38, 0x31, 0x00, 0x11, 0x01, 0x8C, 0x13, 0x01, 0x46, 0x14, 0x01, 0x18,
         // @ 95[33]
         0x1D, 0x00, 0x1D, 0x00, 0x01, 0x04, 0x30, 0x2A, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x30, 0x31, 0x00, 0x14, 0x01, 0x17, 0x17, 0x06, 0x1F, 0x1F, 0x1F, 0x0B, 0x0B,
         0x0B, 0x18, 0x01, 0x00,
         // @ 1032[39]
         0x1D, 0x00, 0x23, 0x00, 0x01, 0x04, 0x30, 0x2A, 0x00, 0x00, 0x04, 0x07, 0x32, 0x39, 0x34, 0x34, 0x38, 0x33, 0x00, 0x11,
         0x01, 0x00, 0x13, 0x01, 0x00, 0x14, 0x01, 0x19, 0x17, 0x06, 0x00, 0x1F, 0x1F, 0x00, 0x0B, 0x0B, 0x18, 0x01, 0x32,
         // @ 1071[39]
         0x1D, 0x00, 0x23, 0x00, 0x01, 0x04, 0x40, 0x38, 0x00, 0x00, 0x04, 0x07, 0x32,
         0x39, 0x34, 0x34, 0x30, 0x31, 0x00, 0x11, 0x01, 0x8C, 0x13, 0x01, 0x46, 0x14, 0x01, 0x18, 0x17, 0x06, 0x1F, 0x1F, 0x1F, 0x0B, 0x0B, 0x0B, 0x18, 0x01, 0x00};

      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, src->bs, RECORDS_IN(bout));

      C8 c0[150+1];
      S_BufC8 *cs0 = &(S_BufC8){.cs = c0, .cnt = 150};
      bookPack_ChainCullStats(cs0, &stats);

      printf("Stats: %s cnt %u -> %u %s\r\n",
             c0, startCnt, src->cnt,
             printDataPtsDigest(&(S_BufC8){.cs = (C8[50]){}, .cnt = 49}, &reDataPts));


}

// ----------------------------------------- eof --------------------------------------------
