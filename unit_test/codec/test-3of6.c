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

// ============================ Tests start here ====================================

/* ----------------------------- test_en13757_encode_empty_src -----------------------

   If 'src.cnt' == 0 then return empty 'dest'.
*/
void test_en13757_encode_empty_src(void)
{
   // Empty 'src'. 'src.bs' <- NULL.
   S_BufU8 * src = &(S_BufU8){.bs = NULL, .cnt = 0};
   // Prefill
   S_BufU8 * dest = &(S_BufU8){.bs = (U8[2]){0x5A, 0x5A}, .cnt = 2};

   S_BufU8 *rtn = en13757_3of6(dest, src);

   TEST_ASSERT_EQUAL_PTR(dest, rtn);                  // Return 'dest'
   TEST_ASSERT_EQUAL_PTR(dest->bs, rtn->bs);          // 'rtn' has 'dest's buffer (it should)
   TEST_ASSERT_EQUAL_UINT16(0, rtn->cnt);             // Zero count.
   TEST_ASSERT_EACH_EQUAL_UINT8(0x5A, rtn->bs, 2);    // 'dest' content 'dest.bs' unmodified.
}


/* -------------------- test_en13757_encode_src_too_large_for_dest ---------------------


*/
void test_en13757_encode_src_too_large_for_dest(void)
{
   // 'src has 2 bytes
   S_BufU8 * src = &(S_BufU8){.bs = (U8[2]){1,2}, .cnt = 2};
   // 'dest has 2 bytes; not enough, needs 3.
   S_BufU8 * dest = &(S_BufU8){.bs = (U8[2]){0x5A, 0x5A}, .cnt = 2};

   S_BufU8 *rtn = en13757_3of6(dest, src);

   TEST_ASSERT_EQUAL_PTR(NULL, rtn);                  // Return NULL
   TEST_ASSERT_EQUAL_UINT16(2, dest->cnt);            // 'dest' unmodified.
   TEST_ASSERT_EACH_EQUAL_UINT8(0x5A, dest->bs, 2);   // 'dest' unmodified.

   // 'src has 2 bytes
   src = &(S_BufU8){.bs = (U8[2]){1,2}, .cnt = 1};
   // 'dest has 1 byte; not enough, needs 2.
   dest = &(S_BufU8){.bs = (U8[1]){0x5A}, .cnt = 1};

   TEST_ASSERT_EQUAL_PTR(NULL, rtn);                  // Return NULL
   TEST_ASSERT_EQUAL_UINT16(1, dest->cnt);            // 'dest' unmodified.
   TEST_ASSERT_EACH_EQUAL_UINT8(0x5A, dest->bs, 1);   // 'dest' unmodified.
}

/* -------------------- test_en13757_encode_src_too_large_period ---------------------


*/
void test_en13757_encode_src_too_large_period(void)
{
   // 'src has 2 bytes
   S_BufU8 * src = &(S_BufU8){.bs = (U8[]){1,2}, .cnt = MAX_U16};
   // 'dest has 2 bytes; not enough, needs 3.
   S_BufU8 * dest = &(S_BufU8){.bs = (U8[2]){0x5A, 0x5A}, .cnt = MAX_U16};

   S_BufU8 *rtn = en13757_3of6(dest, src);

   TEST_ASSERT_EQUAL_PTR(NULL, rtn);                  // Return NULL
   TEST_ASSERT_EQUAL_UINT16(MAX_U16, dest->cnt);            // 'dest' unmodified.
   TEST_ASSERT_EACH_EQUAL_UINT8(0x5A, dest->bs, 2);   // 'dest' unmodified.
}

/* -------------------- test_en13757_encode_1_byte ---------------------


*/
void test_en13757_encode_1_byte(void)
{
   // 'src has 1 byte
   S_BufU8 * src = &(S_BufU8){.bs = (U8[1]){0x01}, .cnt = 1};
   // 'Give 'dest' more than enough bytes for encoded output
   S_BufU8 * dest = &(S_BufU8){.bs = (U8[4]){0,0,0,0}, .cnt = 4};

   S_BufU8 *rtn = en13757_3of6(dest, src);

   TEST_ASSERT_EQUAL_PTR(dest, rtn);                                 // Returns 'dest
   TEST_ASSERT_EQUAL_UINT16(2, dest->cnt);                           // 1 src byte -> 2 dest bytes
//   TEST_ASSERT_EQUAL_UINT8_ARRAY( (U8[4]){0b01010110, 0b00000011, 0, 0}, dest->bs, 4);      // 1st 2 bytes have 0x01 encoded
   U8 *expected = (U8[4]){0b10001101, 0b0000101, 0, 0};
   TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dest->bs, 4);      // 1st 2 bytes have 0x01 encoded

   typedef struct { U8 raw, enc[2]; } S_Tst;

   S_Tst const tsts[] = {
      {.raw = 0x01, .enc = {0b10001101, 0b0000101} },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_BufU8 *src  = &(S_BufU8){.bs = (U8[1]){t->raw}, .cnt = 1};
      S_BufU8 *dest = &(S_BufU8){.bs = (U8[4]){0,0,0,0}, .cnt = 4};

      S_BufU8 *rtn = en13757_3of6(dest, src);

      U8 expected[4] = {t->enc[0], t->enc[1], 0, 0};

      if(rtn != dest || rtn->bs != dest->bs || rtn->cnt != dest->cnt) {

      }
      else if(1) {

      }

      TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, dest->bs, 4);




   }
}

//0b010110, 0b001101
//0b0000101 0b10001101
// ----------------------------------------- eof --------------------------------------------
