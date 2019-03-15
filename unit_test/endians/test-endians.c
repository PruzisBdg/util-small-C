#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

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

// ----------------------------------------- eof --------------------------------------------
