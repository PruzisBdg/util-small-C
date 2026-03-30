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

S_BookScanner scanner;

/* --------------------------------- test_NoBooks ---------------------------------------

   Has not even 1 book
*/
void test_NoBooks(void) {

   // A 1-byte bookshelf ('cnt' = 1)
   U8 b0[4] = {0,1,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 1};

   scanner.digest = alwaysKeep;
   scanner.minLen = 2;           // A book is a least 2bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(1, bs0->cnt);
   U8 bout[4] = {0,1,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
}

/* ------------------------------ maybeKeep ------------------------------------

   A digest for a book where book[0] is length and keep if book[1] > 0.
*/
static T_ReBook const * maybeKeep(U8 const *bk, T_ReBook *dig) {
   dig->len = bk[0];
   dig->keep = bk[1] > 0 ? true : false;
   return dig; }

/* ---------------------------------- test_keepOne --------------------------------- */

void test_keepOne(void)
{
   // Single 4-byte book which we keep
   U8 b0[4] = {4,1,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns bs0 unchanged.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(4, bs0->cnt);
   U8 bout[4] = {4,1,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
}

/* ---------------------------------- test_RemoveOne --------------------------------- */

void test_RemoveOne(void)
{
   // Single 4-byte book which we keep
   U8 b0[4] = {4,0,2,3};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns bs0 with count <- 0.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(0, bs0->cnt);
   U8 bout[4] = {4,0,2,3};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
}

/* ----------------------------- test_OneUndersized --------------------------------- */

void test_OneUndersized(void)
{
   // Length = 1-byte. Illegal
   U8 b0[4] = {1,10,11,12};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(4, bs0->cnt);
   U8 bout[4] = {1,10,11,12};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
}

/* ----------------------------- test_OneOversized --------------------------------- */

void test_OneOversized(void)
{
   // Length = 6, larger than bs0.
   U8 b0[4] = {6,1,11,12};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 4};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(4, bs0->cnt);
   U8 bout[4] = {6,1,11,12};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 4);
}

/* ----------------------------- test_Keep1st_2ndUndersized ----------------------------- */

void test_Keep1st_2ndUndersized(void)
{
   // 2nd book has illegal length.
   U8 b0[6] = {3,1,8,  1,9,10};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 6};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(6, bs0->cnt);
   U8 bout[6] = {3,1,8,  1,9,10};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 6);
}

/* ----------------------------- test_Keep1st_2ndOversized ----------------------------- */

void test_Keep1st_2ndOversized(void)
{
   // 2nd book length extends past bookshelf.
   U8 b0[6] = {3,1,8,  4,9,10};
   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 6};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns NULL, bs0 unchanged.
   TEST_ASSERT_NULL(rtn);
   TEST_ASSERT_EQUAL_UINT8(6, bs0->cnt);
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

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2 bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns bs0, which is unchanged.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(5, bs0->cnt);
   U8 bout[5] = {3,1,10,  2,1};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);
}

/* -------------------------------- test_CullLast --------------------------------

   Remove the last (of 2 books).
*/
void test_CullLast(void)
{
   // 2 Books, different lengths, remove 2nd book.
   U8 b0[] = {3,1,10,  2,0};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2 bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   // Returns bs0 with just the 1st book.
   TEST_ASSERT_EQUAL_PTR(bs0, rtn);
   TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
   // Bytes for 2nd book was left in-place, even though its gone.
   U8 bout[5] = {3,1,10,  2,0};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);
}

/* -------------------------------- test_Cull1stOfTwo ------------------------------------

   Remove 1st of 2 books.
*/
void test_Cull1stOfTwo(void)
{
   {
      // 2 Books, different lengths, remove 1st book.
      U8 b0[] = {3,0,10,  2,1};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      scanner.digest = maybeKeep;
      scanner.minLen = 2;           // A book is a least 2 bytes

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(2, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {2,1,10,  2,1};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);
   }

   {
      // Same as above, but put larger book 1st
      U8 b0[] = {2,0,  3,1,10};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      scanner.digest = maybeKeep;
      scanner.minLen = 2;           // A book is a least 2 bytes

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

      // Returns bs0 with just the 2nd book at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(3, bs0->cnt);
      // Original bytes for 2nd book are left in-place, even though 2nd book was copied down over 1st.
      U8 bout[5] = {3,1,10,  1,10};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 5);
   }
}

/* ----------------------------- test_Cull_1of3 -------------------------------
*/
void test_Cull_1of3(void)
{
   {
      // 3 Books, different lengths, remove 1st book.
      U8 b0[] = {3,0,10,  2,1, 4,1,5,6};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

      scanner.digest = maybeKeep;
      scanner.minLen = 2;           // A book is a least 2 bytes

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

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

      scanner.digest = maybeKeep;
      scanner.minLen = 2;           // A book is a least 2 bytes

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

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

      scanner.digest = maybeKeep;
      scanner.minLen = 2;           // A book is a least 2 bytes

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

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

      scanner.digest = maybeKeep;
      scanner.minLen = 2;           // A book is a least 2 bytes

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

      // Returns bs0 with 2nd & 3rd books at left.
      TEST_ASSERT_EQUAL_PTR(bs0, rtn);
      TEST_ASSERT_EQUAL_UINT8(6, bs0->cnt);
      // Part of the orignal 3rd book is left in-place.
      U8 bout[9] = {2,1, 4,1,5,6,  1,5,6};
      TEST_ASSERT_EQUAL_UINT8_ARRAY(bout, b0, 9);
   }

}


// ----------------------------------------- eof --------------------------------------------
