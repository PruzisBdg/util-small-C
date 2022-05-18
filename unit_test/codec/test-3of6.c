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

   S_BufU8 *rtn = en13757_3of6_Encode(dest, src);

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

   S_BufU8 *rtn = en13757_3of6_Encode(dest, src);

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

   S_BufU8 *rtn = en13757_3of6_Encode(dest, src);

   TEST_ASSERT_EQUAL_PTR(NULL, rtn);                  // Return NULL
   TEST_ASSERT_EQUAL_UINT16(MAX_U16, dest->cnt);            // 'dest' unmodified.
   TEST_ASSERT_EACH_EQUAL_UINT8(0x5A, dest->bs, 2);   // 'dest' unmodified.
}

// 3 of 6, by nibble 0x0 -> 0xF.
#define _s0 0b010110
#define _s1 0b001101
#define _s2 0b001110
#define _s3 0b001011
#define _s4 0b011100
#define _s5 0b011001
#define _s6 0b011010
#define _s7 0b010011
#define _s8 0b101100
#define _s9 0b100101
#define _sA 0b100110
#define _sB 0b100011
#define _sC 0b110100
#define _sD 0b110001
#define _sE 0b110010
#define _sF 0b101001


/* -------------------- test_en13757_encode_1_byte ---------------------

   Encoding of 1 byte.
*/
void test_en13757_encode_1_byte(void)
{
   typedef struct { U8 raw, enc[2]; } S_Tst;

   // Encode 2 nibbles in 12 bits in 1 byte + nibble  '_s0' -> '_sF' above.
   #define _6s(n) _s ## n
   #define _2_6s(a,b)  { ((_6s(a) & 0x3) << 6) + _6s(b), _6s(a)>>2 }

   S_Tst const tsts[] = {
      {.raw = 0x01, .enc = _2_6s(0,1) },
      {.raw = 0x23, .enc = _2_6s(2,3) },
      {.raw = 0x45, .enc = _2_6s(4,5) },
      {.raw = 0x67, .enc = _2_6s(6,7) },
      {.raw = 0x89, .enc = _2_6s(8,9) },
      {.raw = 0xAB, .enc = _2_6s(A,B) },
      {.raw = 0xCD, .enc = _2_6s(C,D) },
      {.raw = 0xEF, .enc = _2_6s(E,F) }};

   #define _rawBytes 1
   #define _encBytes 2
   #define _destBufSize 4

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_BufU8 *src  = &(S_BufU8){.bs = (U8[_rawBytes]){t->raw}, .cnt = _rawBytes};

      // Prefill 'dest'; will check for overrun.
      #define _Fill 0x5A
      S_BufU8 *dest = &(S_BufU8){.bs = (U8[_destBufSize]){[0 ... _destBufSize-1] = _Fill}, .cnt = _destBufSize};

      S_BufU8 *rtn = en13757_3of6_Encode(dest, src);

      U8 expected[_destBufSize] = {t->enc[0], t->enc[1], _Fill, _Fill};

      // en13757_3of6_Encode() must return 'dest' unmangled.
      if(rtn != dest || rtn->bs != dest->bs || rtn->cnt != dest->cnt) {
         printf("tst #%u: rtn did not match dest\r\n", i);
         TEST_FAIL();}

      // Correct encoded byte count? Correct encoded bytes?
      else if(rtn->cnt != _encBytes || memcmp(expected, rtn->bs, _destBufSize) != 0 ) {
         printf("tst #%u: expected <%02x %02x 5A 5A>[2], got <%02x %02x %02x %02x>[%u]\r\n",
                i,
                t->enc[0], t->enc[1],
                rtn->bs[0], rtn->bs[1], rtn->bs[2], rtn->bs[3], rtn->cnt);
            TEST_FAIL(); }

   } // for()

   #undef _rawBytes
   #undef _encBytes
   #undef _destBufSize
}

/* ------------------------- test_en13757_encode_2_bytes -----------------------------

*/
void test_en13757_encode_2_bytes(void)
{
   #define _rawBytes    2
   #define _encBytes    3
   #define _destBufSize 5

   typedef struct { U8 raw[_rawBytes], enc[_encBytes]; } S_Tst;

   // Encode 4 nibbles in 24 bits, wholly occupying 3 bytes  '_s0' -> '_sF' above.
   #define _6s(n) _s ## n

   #define _4_6s(a,b,c,d) {                     \
      ((_6s(a) & 0x3) << 6)  + _6s(b),          \
      ((_6s(d) & 0xF) << 4)  + (_6s(a) >> 2),   \
      ( _6s(c) << 2)         + (_6s(d) >> 4) }

   S_Tst const tsts[] = {
      {.raw = {0x01, 0x23}, .enc = _4_6s(0,1,2,3) } ,
      {.raw = {0x23, 0x45}, .enc = _4_6s(2,3,4,5) } ,
      {.raw = {0x45, 0x67}, .enc = _4_6s(4,5,6,7) } ,
      {.raw = {0x67, 0x89}, .enc = _4_6s(6,7,8,9) } ,
      {.raw = {0x89, 0xAB}, .enc = _4_6s(8,9,A,B) } ,
      {.raw = {0xAB, 0xCD}, .enc = _4_6s(A,B,C,D) } ,
      {.raw = {0xCD, 0xEF}, .enc = _4_6s(C,D,E,F) } ,
      };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_BufU8 *src  = &(S_BufU8){.bs = (U8[_rawBytes]){t->raw[0], t->raw[1]}, .cnt = _rawBytes};

      // Prefill 'dest'; will check for overrun.
      #define _Fill 0x5A
      S_BufU8 *dest = &(S_BufU8){.bs = (U8[_destBufSize]){[0 ... _destBufSize-1] = _Fill}, .cnt = _destBufSize};

      S_BufU8 *rtn = en13757_3of6_Encode(dest, src);

      U8 expected[_destBufSize] = {t->enc[0], t->enc[1], t->enc[2], _Fill, _Fill};

      // en13757_3of6_Encode() must return 'dest' unmangled.
      if(rtn != dest || rtn->bs != dest->bs || rtn->cnt != dest->cnt) {
         printf("tst #%u: rtn did not match dest\r\n", i);
         TEST_FAIL();}

      // Correct encoded byte count? Correct encoded bytes?
      else if(rtn->cnt != _encBytes || memcmp(expected, rtn->bs, _destBufSize) != 0 ) {
         printf("tst #%u: expected <%02x %02x %02x 5A 5A>[5], got <%02x %02x %02x %02x %02x>[%u]\r\n",
                i,
                t->enc[0], t->enc[1], t->enc[2],
                rtn->bs[0], rtn->bs[1], rtn->bs[2], rtn->bs[3], rtn->bs[4], rtn->cnt);
            TEST_FAIL(); }
   } // for()

   #undef _rawBytes
   #undef _encBytes
   #undef _destBufSize
}


/* ------------------------- test_en13757_encode_3_bytes -----------------------------

*/
void test_en13757_encode_3_bytes(void)
{
   #define _rawBytes    3
   #define _encBytes    5
   #define _destBufSize 7

   typedef struct { U8 raw[_rawBytes], enc[_encBytes]; } S_Tst;

   // Encode 6 nibbles in 36 bits, wholly occupying 4 bytes + nibble  '_s0' -> '_sF' above.
   #define _6s(n) _s ## n

   #define _6_6s(a,b,c,d,e,f) {                 \
      ((_6s(a) & 0x3) << 6)  + _6s(b),          \
      ((_6s(d) & 0xF) << 4)  + (_6s(a) >> 2),   \
      ( _6s(c) << 2)         + (_6s(d) >> 4),   \
      ((_6s(e) & 0x3) << 6)  + _6s(f),          \
       (_6s(e) >> 2) }                          \

   S_Tst const tsts[] = {
      {.raw = {0x01,0x23,0x45}, .enc = _6_6s(0,1,2,3,4,5) },
      {.raw = {0x23,0x45,0x67}, .enc = _6_6s(2,3,4,5,6,7) },
      {.raw = {0x45,0x67,0x89}, .enc = _6_6s(4,5,6,7,8,9) },
      {.raw = {0x67,0x89,0xAB}, .enc = _6_6s(6,7,8,9,A,B) }};

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_BufU8 *src  = &(S_BufU8){.bs = (U8[_rawBytes]){t->raw[0], t->raw[1], t->raw[2]}, .cnt = _rawBytes};

      // Prefill 'dest'; will check for overrun.
      #define _Fill 0x5A
      S_BufU8 *dest = &(S_BufU8){.bs = (U8[_destBufSize]){[0 ... _destBufSize-1] = _Fill},.cnt = _destBufSize};

      S_BufU8 *rtn = en13757_3of6_Encode(dest, src);

      U8 expected[_destBufSize] = {t->enc[0], t->enc[1], t->enc[2], t->enc[3], t->enc[4], _Fill, _Fill};

      // en13757_3of6_Encode() must return 'dest' unmangled.
      if(rtn != dest || rtn->bs != dest->bs || rtn->cnt != dest->cnt) {
         printf("tst #%u: rtn did not match dest\r\n", i);
         TEST_FAIL();}

      // Correct encoded byte count? Correct encoded bytes?
      else if(rtn->cnt != _encBytes || memcmp(expected, rtn->bs, _destBufSize) != 0 ) {
         printf("tst #%u: expected <%02x %02x %02x %02x %02x 5A 5A>[7], got <%02x %02x %02x %02x %02x %02x %02x>[%u]\r\n",
                i,
                t->enc[0], t->enc[1], t->enc[2], t->enc[3], t->enc[4],
                rtn->bs[0], rtn->bs[1], rtn->bs[2], rtn->bs[3], rtn->bs[4], rtn->bs[5], rtn->bs[6], rtn->cnt);
            TEST_FAIL(); }
   } // for()

   #undef _rawBytes
   #undef _encBytes
   #undef _destBufSize
}


/* ------------------------- test_en13757_encode_4_bytes -----------------------------

*/
void test_en13757_encode_4_bytes(void)
{
   #define _rawBytes    4
   #define _encBytes    6
   #define _destBufSize 8

   typedef struct { U8 raw[_rawBytes], enc[_encBytes]; } S_Tst;

   // Encode 8 nibbles in 48 bits, wholly occupying 6 bytes  '_s0' -> '_sF' above.
   #define _6s(n) _s ## n

   #define _8_6s(a,b,c,d,e,f,g,h) {             \
      ((_6s(a) & 0x3) << 6)  + _6s(b),          \
      ((_6s(d) & 0xF) << 4)  + (_6s(a) >> 2),   \
      ( _6s(c) << 2)         + (_6s(d) >> 4),   \
      ((_6s(e) & 0x3) << 6)  + _6s(f),          \
      ((_6s(h) & 0xF) << 4)  + (_6s(e) >> 2),   \
      ( _6s(g) << 2)         + (_6s(h) >> 4) }  \


   S_Tst const tsts[] = {
      {.raw = {0x01,0x23,0x45,0x67}, .enc = _8_6s(0,1,2,3,4,5,6,7) },
      {.raw = {0x89,0xAB,0xCD,0xEF}, .enc = _8_6s(8,9,A,B,C,D,E,F) },
      };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      S_BufU8 *src  = &(S_BufU8){.bs = (U8[_rawBytes]){t->raw[0], t->raw[1], t->raw[2], t->raw[3]}, .cnt = _rawBytes};

      // Prefill 'dest'; will check for overrun.
      #define _Fill 0x5A
      S_BufU8 *dest = &(S_BufU8){.bs = (U8[_destBufSize]){[0 ... _destBufSize-1] = _Fill},.cnt = _destBufSize};

      S_BufU8 *rtn = en13757_3of6_Encode(dest, src);

      U8 expected[_destBufSize] = {t->enc[0], t->enc[1], t->enc[2], t->enc[3], t->enc[4], t->enc[5], _Fill, _Fill};

      // en13757_3of6_Encode() must return 'dest' unmangled.
      if(rtn != dest || rtn->bs != dest->bs || rtn->cnt != dest->cnt) {
         printf("tst #%u: rtn did not match dest\r\n", i);
         TEST_FAIL();}

      // Correct encoded byte count? Correct encoded bytes?
      else if(rtn->cnt != _encBytes || memcmp(expected, rtn->bs, _destBufSize) != 0 ) {
         printf("tst #%u: expected <%02x %02x %02x %02x %02x %02x 5A 5A>[7], got <%02x %02x %02x %02x %02x %02x %02x %02x>[%u]\r\n",
                i,
                t->enc[0], t->enc[1], t->enc[2], t->enc[3], t->enc[4], t->enc[5],
                rtn->bs[0], rtn->bs[1], rtn->bs[2], rtn->bs[3], rtn->bs[4], rtn->bs[5], rtn->bs[6], rtn->bs[7], rtn->cnt);
            TEST_FAIL(); }
   } // for()

   #undef _rawBytes
   #undef _encBytes
   #undef _destBufSize
}



// ----------------------------------------- eof --------------------------------------------
