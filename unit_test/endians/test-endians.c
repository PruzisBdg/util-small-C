#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include <string.h>

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

static C8 const *printEndian( E_EndianIs e) {
   return e == eNoEndian ? "None" : (e == eLittleEndian ? "Little" : "Big"); }

/* ------------------------------- test_ToSysEndian_U16 --------------------------------------------------- */


void test_ToSysEndian_U16 (void)
{
   typedef struct { U16 in, rtn; E_EndianIs srcEndian; } S_Tst;

   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
   S_Tst const tsts[] = {
      { .in = 0x1234,    .rtn = 0x1234,   .srcEndian = eNoEndian },
      { .in = 0x1234,    .rtn = 0x1234,   .srcEndian = eLittleEndian },
      { .in = 0x1234,    .rtn = 0x3412,   .srcEndian = eBigEndian }};

      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
   S_Tst const tsts[] = {
      { .in = 0x1234,    .rtn = 0x1234,   .srcEndian = eNoEndian },
      { .in = 0x1234,    .rtn = 0x3412,   .srcEndian = eLittleEndian },
      { .in = 0x1234,    .rtn = 0x1234,   .srcEndian = eBigEndian }};
      #endif

   #else
      #warning "Endian not specified for this platform. Can't run this test"
   #endif // __BYTE_ORDER__

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      U16 rtn = ToSysEndian_U16(t->in, t->srcEndian);

      C8 b0[50];
      sprintf(b0, "Src endian = %s", printEndian(t->srcEndian) );
      TEST_ASSERT_EQUAL_HEX16_MESSAGE(t->rtn, rtn, b0);
   }
}

/* ------------------------------- test_ToSysEndian_U32 --------------------------------------------------- */

void test_ToSysEndian_U32 (void)
{
   typedef struct { U32 in, rtn; E_EndianIs srcEndian; } S_Tst;

   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
   S_Tst const tsts[] = {
      { .in = 0x12345678,    .rtn = 0x12345678,   .srcEndian = eNoEndian },
      { .in = 0x12345678,    .rtn = 0x12345678,   .srcEndian = eLittleEndian },
      { .in = 0x12345678,    .rtn = 0x78563412,   .srcEndian = eBigEndian }};

      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
   S_Tst const tsts[] = {
      { .in = 0x12345678,    .rtn = 0x12345678,   .srcEndian = eNoEndian },
      { .in = 0x12345678,    .rtn = 0x78563412,   .srcEndian = eLittleEndian },
      { .in = 0x12345678,    .rtn = 0x12345678,   .srcEndian = eBigEndian }};
      #endif

   #else
      #warning "Endian not specified for this platform. Can't run this test"
   #endif // __BYTE_ORDER__

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];
      U32 rtn = ToSysEndian_U32(t->in, t->srcEndian);

      C8 b0[50];
      sprintf(b0, "Src endian = %s", printEndian(t->srcEndian) );
      TEST_ASSERT_EQUAL_HEX32_MESSAGE(t->rtn, rtn, b0);
   }
}

/* ------------------------------- test_leToU16/32/64 --------------------------------------------------- */

void test_leToU16(void)
   { TEST_ASSERT_EQUAL_HEX16( 0x1234, leToU16( (U8[2]){0x34,0x12}) ); }

void test_leToU24(void)
   { TEST_ASSERT_EQUAL_HEX32( 0x00123456, leToU24( (U8[3]){0x56,0x34,0x12}) ); }

void test_leToU32(void)
   { TEST_ASSERT_EQUAL_HEX32( 0x12345678, leToU32( (U8[4]){0x78,0x56,0x34,0x12}) ); }

void test_leToU48(void)
   { TEST_ASSERT_EQUAL_HEX64( 0x0000123456789ABCULL, leToU48( (U8[6]){0xBC,0x9A,0x78,0x56,0x34,0x12}) ); }

void test_leToU64(void)
   { TEST_ASSERT_EQUAL_HEX64( 0x123456789ABCDE55ULL, leToU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}) ); }

void test_leToFloat(void) {
   union u { float f; U8 bs[4]; U32 _u32; } u;
   u.f = 1234E5;

   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
   u._u32 = ReverseU32(u._u32);
      #endif
   TEST_ASSERT_EQUAL_FLOAT(1234E5, leToFloat( u.bs ));
   #else
      #warning "Cannot test leToFloat(); don't know system endian"
   #endif // __BYTE_ORDER__
}

void test_BytesLEtoU64(void)
{
   TEST_ASSERT_EQUAL_HEX64(0x00,                      BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 0));
   TEST_ASSERT_EQUAL_HEX64(0x55,                      BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 1));
   TEST_ASSERT_EQUAL_HEX64(0xDE55,                    BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 2));
   TEST_ASSERT_EQUAL_HEX64(0xBCDE55,                  BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 3));
   TEST_ASSERT_EQUAL_HEX64(0x9ABCDE55,                BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 4));
   TEST_ASSERT_EQUAL_HEX64(0x789ABCDE55ULL,           BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 5));
   TEST_ASSERT_EQUAL_HEX64(0x56789ABCDE55ULL,         BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 6));
   TEST_ASSERT_EQUAL_HEX64(0x3456789ABCDE55ULL,       BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 7));
   TEST_ASSERT_EQUAL_HEX64(0x123456789ABCDE55ULL,     BytesLEtoU64( (U8[8]){0x55,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}, 8));
}

/* ------------------------------- test_beToU16/32/64 --------------------------------------------------- */

void test_beToU16(void)
   { TEST_ASSERT_EQUAL_HEX16( 0x1234, beToU16( (U8[2]){0x12,0x34}) ); }

void test_beToU32(void)
   { TEST_ASSERT_EQUAL_HEX32( 0x12345678, beToU32( (U8[4]){0x12,0x34,0x56,0x78}) ); }

void test_beToU64(void)
   { TEST_ASSERT_EQUAL_HEX64( 0x123456789ABCDE55ULL, beToU64( (U8[8]){0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0x55}) ); }


/* ------------------------------- test_u16/u32/u64leToLE ---------------------------------------------------

   Note: check that funcs() don't write extra bytes to 'out[]'.
*/
void test_u16ToLE(void)
{
   U8 out[3] = {0x55,0x55,0x55};
   U8 *rtn = u16ToLE(out, 0x1234);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[3] = {0x34,0x12,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 3);
}

void test_u32ToLE(void)
{
   U8 out[5] = {0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = u32ToLE(out, 0x12345678);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[5] = {0x78,0x56,0x34,0x12,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 5);
}

void test_u64ToLE(void)
{
   U8 out[9] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = u64ToLE(out, 0x12345678ABCDEF01ULL);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[9] = {0x01,0xEF,0xCD,0xAB,0x78,0x56,0x34,0x12,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 9);
}

/* ------------------------------- test_u16/u32/u64leToBE -------------------------------------------------- */

void test_u16ToBE(void)
{
   U8 out[3] = {0x55,0x55,0x55};
   U8 * rtn = u16ToBE(out, 0x1234);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[3] = {0x12,0x34,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 3);
}

void test_u32ToBE(void)
{
   U8 out[5] = {0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = u32ToBE(out, 0x12345678);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[5] = {0x12,0x34,0x56,0x78,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 5);
}

void test_u64ToBE(void)
{
   U8 out[9] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = u64ToBE(out, 0x12345678ABCDEF01ULL);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[9] = {0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x01,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 9);
}

/* ------------------------------- test_s16/s32/s64leToLE ---------------------------------------------------

   Same tests as for unsigned counterparts.
*/

void test_s16ToLE(void)
{
   U8 out[3] = {0x55,0x55,0x55};
   U8 * rtn = s16ToLE(out, 0x1234);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[3] = {0x34,0x12,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 3);
}

void test_s32ToLE(void)
{
   U8 out[5] = {0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = s32ToLE(out, 0x12345678);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[5] = {0x78,0x56,0x34,0x12,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 5);
}

void test_s64ToLE(void)
{
   U8 out[9] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = s64ToLE(out, 0x12345678ABCDEF01ULL);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[9] = {0x01,0xEF,0xCD,0xAB,0x78,0x56,0x34,0x12,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 9);
}

/* ------------------------------- test_s16/s32/s64leToBE --------------------------------------------------

   Same tests as for unsigned counterparts.
*/
void test_s16ToBE(void)
{
   U8 out[3] = {0x55,0x55,0x55};
   U8 * rtn = s16ToBE(out, 0x1234);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[3] = {0x12,0x34,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 3);
}

void test_s32ToBE(void)
{
   U8 out[5] = {0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = s32ToBE(out, 0x12345678);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[5] = {0x12,0x34,0x56,0x78,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 5);
}

void test_s64ToBE(void)
{
   U8 out[9] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
   U8 * rtn = s64ToBE(out, 0x12345678ABCDEF01ULL);
   TEST_ASSERT_EQUAL_PTR(rtn, out);
   U8 chk[9] = {0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x01,0x55};
   TEST_ASSERT_EQUAL_UINT8_ARRAY(chk, out, 9);
}


/* ---------------------------- test_ReverseBytesInPlace ------------------------------------------- */

void test_ReverseBytesInPlace(void)
{
   typedef struct { U8 const *in; U8 const *out; U16 numBytes; } S_Tst;

   S_Tst const tsts[] = {
      // No bytes or 1 bytes to swap. No swap to do; does not touch io[], which may be NULL
      {.in = NULL, .out = (U8[2]){0x5A, 0x5A}, .numBytes = 0},
      {.in = NULL, .out = (U8[2]){0x5A, 0x5A}, .numBytes = 1},


      {.in = (U8[2]){3,4},       .out = (U8[4]){4,3, 0x5A,0x5A},        .numBytes = 2},
      {.in = (U8[3]){3,4,5},     .out = (U8[5]){5,4,3, 0x5A,0x5A},        .numBytes = 3},
      {.in = (U8[4]){3,4,5,6},   .out = (U8[6]){6,5,4,3, 0x5A,0x5A},        .numBytes = 4},
      {.in = (U8[5]){3,4,5,6,7}, .out = (U8[7]){7,6,5,4,3, 0x5A,0x5A},        .numBytes = 5},
      };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      #define _BufSize 50
      U8 io[_BufSize];
      memset(io, 0x5A, _BufSize);

      if(t->in == NULL)
      {
         // Check NULL cases for 0 and 1 bytes. Should not crash.
         ReverseBytesInPlace(NULL, t->numBytes);
      }
      else
      {
         memcpy(io, t->in, t->numBytes);
      }

      ReverseBytesInPlace(io, t->numBytes);

      if(memcmp(io, t->out, t->numBytes) != 0)
      {
         printf("test #%u bad swap\r\n", i);
         TEST_FAIL();
      }
      if(io[t->numBytes] != 0x5A)
      {
         printf("test #%u Wrote outside io[]\r\n", i);
         TEST_FAIL();
      }
   }
}

// ----------------------------------------- eof --------------------------------------------
