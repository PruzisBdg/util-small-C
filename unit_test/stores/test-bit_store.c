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

// ----------------------------------------------------------------------------------
void test_MemSize(void)
{
   // Storage word size is U32 i.e 0,4,8,12 bytes.
   TEST_ASSERT_EQUAL_UINT8(0,   _BitStore_WordsUsed(0));
   TEST_ASSERT_EQUAL_UINT8(1,   _BitStore_WordsUsed(1));
   TEST_ASSERT_EQUAL_UINT8(1,   _BitStore_WordsUsed(32));
   TEST_ASSERT_EQUAL_UINT8(2,   _BitStore_WordsUsed(33));
   TEST_ASSERT_EQUAL_UINT8(2,   _BitStore_WordsUsed(64));
   TEST_ASSERT_EQUAL_UINT8(3,   _BitStore_WordsUsed(65));

   TEST_ASSERT_EQUAL_UINT8(0,   _BitStore_BytesUsed(0));
   TEST_ASSERT_EQUAL_UINT8(4,   _BitStore_BytesUsed(1));
   TEST_ASSERT_EQUAL_UINT8(4,   _BitStore_BytesUsed(8));
   TEST_ASSERT_EQUAL_UINT8(4,   _BitStore_BytesUsed(32));
   TEST_ASSERT_EQUAL_UINT8(8,   _BitStore_BytesUsed(33));
   TEST_ASSERT_EQUAL_UINT8(12,  _BitStore_BytesUsed(65));
}


// ----------------------------------------------------------------------------------
void test_Make(void)
{
   /* Make a 35 bit store which should use 2 words. But give the Store 3 words; check 3rd
      word is not overwritten.

      Also init S_BitStore with garbage; should be set up correctly
   */
   U32 buf[3] = {0x55555555, 0x55555555, 0x55555555};

   S_BitStore *bs = BitStore_Make(
                           &(S_BitStore){.mem = (U32*)0x55555555, .capacity = 0x5555},
                           buf,
                           35 );

   // Bit store uses supplied buffer?
   TEST_ASSERT_EQUAL_PTR(bs->mem, buf);

   // buf[0,1] should be cleared. buf[2] not used; should be untouched.
   TEST_ASSERT_EQUAL_UINT32(buf[0], 0);
   TEST_ASSERT_EQUAL_UINT32(buf[1], 0);
   TEST_ASSERT_EQUAL_UINT32(buf[2], 0x55555555);

   TEST_ASSERT_EQUAL_UINT16(35, bits_StoreCapacity(bs));
   TEST_ASSERT_EQUAL_UINT16(0, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllClr(bs));
}


// ----------------------------------------------------------------------------------
void test_Make_Macro(void)
{
   #define _StoreSize 10
   S_BitStore *bs = _BitStore_Make(_StoreSize);

   TEST_ASSERT_EQUAL_UINT16(bits_StoreCapacity(bs), 10);
   TEST_ASSERT_EQUAL_UINT16(bits_NumSet(bs), 0);
   TEST_ASSERT_EQUAL_UINT8(bits_AllClr(bs), true);

   #undef _StoreSize
}

// ----------------------------------------------------------------------------------
void test_ScanEmpty(void)
{
   // This Store will reserve 2 x U32.
   #define _StoreSize 41
   S_BitStore *bs = _BitStore_Make(_StoreSize);

   // All 41 bits should be Clear AND Not Set
   for(U8 i = 0; i < _StoreSize; i++)
   {
      TEST_ASSERT_EQUAL_UINT8(false, bits_BitSet(bs, i) );
      TEST_ASSERT_EQUAL_UINT8(true,  bits_BitClr(bs, i) );
   }

   // Queries for indices >= 41 should always return false.
   for(U8 i = _StoreSize; i < 64; i++)
   {
      TEST_ASSERT_EQUAL_UINT8(false, bits_BitSet(bs, i) );
      TEST_ASSERT_EQUAL_UINT8(false, bits_BitClr(bs, i) );
   }

   // Queries for indices outside the word-range of the Store buffer should be harmless (and return false)
   for(U8 i = 65; i < 100; i++)
   {
      TEST_ASSERT_EQUAL_UINT8(false, bits_BitSet(bs, i) );
      TEST_ASSERT_EQUAL_UINT8(false, bits_BitClr(bs, i) );
   }
   #undef _StoreSize
}

// ----------------------------------------------------------------------------------
void test_SetandClr(void)
{
   #define _StoreSize 41
   S_BitStore *bs = _BitStore_Make(_StoreSize);

   TEST_ASSERT_EQUAL_UINT16(0, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllClr(bs));

   // Set b0. Should read as 'set' AND 'not clear'
   bits_SetBit(bs, 0);
   TEST_ASSERT_EQUAL_UINT16(1, bits_NumSet(bs));

   TEST_ASSERT_EQUAL_UINT8(true,  bits_BitSet(bs, 0) );
   TEST_ASSERT_EQUAL_UINT8(false, bits_BitClr(bs, 0) );
   TEST_ASSERT_EQUAL_UINT8(false, bits_AllClr(bs) );

   // Set b0 again. Nothing should change.
   bits_SetBit(bs, 0);
   TEST_ASSERT_EQUAL_UINT8(true,  bits_BitSet(bs, 0) );
   TEST_ASSERT_EQUAL_UINT8(false, bits_BitClr(bs, 0) );
   TEST_ASSERT_EQUAL_UINT8(false, bits_AllClr(bs) );

   // Clear b0. Should read as 'clear; AND 'not set'.
   bits_ClrBit(bs, 0);
   TEST_ASSERT_EQUAL_UINT16(0, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(false, bits_BitSet(bs, 0) );
   TEST_ASSERT_EQUAL_UINT8(true,  bits_BitClr(bs, 0) );

   // All-clear should be true.
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllClr(bs) );

   // Set all bits, one-by-one.
   for(U8 i = 0; i < _StoreSize; i++)
   {
      // Bits-set should count up
      bits_SetBit(bs, i);
      // Each bit set should now read 'set' AND 'not clear'.
      TEST_ASSERT_EQUAL_UINT16(i+1, bits_NumSet(bs));
      TEST_ASSERT_EQUAL_UINT8(true,  bits_BitSet(bs, 0) );
      TEST_ASSERT_EQUAL_UINT8(false, bits_BitClr(bs, 0) );
   }

   // All-set should now be true.
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllSet(bs) );

   // Clear all bits, one-by-one.
   for(U8 i = 0; i < _StoreSize; i++)
   {
      bits_ClrBit(bs, i);
      TEST_ASSERT_EQUAL_UINT16(_StoreSize-i-1, bits_NumSet(bs));
      TEST_ASSERT_EQUAL_UINT8(false, bits_BitSet(bs, 0) );
      TEST_ASSERT_EQUAL_UINT8(true,  bits_BitClr(bs, 0) );
   }
   #undef _StoreSize
}

// ----------------------------------------------------------------------------------
void test_SetandClrAll(void)
{
   #define _StoreSize 82
   S_BitStore *bs = _BitStore_Make(_StoreSize);

   TEST_ASSERT_EQUAL_UINT16(0, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllClr(bs));

   bits_SetAll(bs);
   TEST_ASSERT_EQUAL_UINT16(_StoreSize, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllSet(bs));

   bits_ClrAll(bs);
   TEST_ASSERT_EQUAL_UINT16(0, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllClr(bs));

   #undef _StoreSize

   /* Check also with a Store which uses exactly a Word, because we know that uses a branch
      not covered by e.g _StoreSize <- 82.
   */
   #define _StoreSize 32
   bs = _BitStore_Make(_StoreSize);

   TEST_ASSERT_EQUAL_UINT16(0, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllClr(bs));

   bits_SetAll(bs);
   TEST_ASSERT_EQUAL_UINT16(_StoreSize, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllSet(bs));

   bits_ClrAll(bs);
   TEST_ASSERT_EQUAL_UINT16(0, bits_NumSet(bs));
   TEST_ASSERT_EQUAL_UINT8(true, bits_AllClr(bs));
}

// ----------------------------------------- eof --------------------------------------------
