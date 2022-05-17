/*---------------------------------------------------------------------------
|
| 3-of-6 encode & decode.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PRIVATE U8 const en13757_3of6[] = {
   0b010110, 0b001101, 0b001110, 0b001011,
   0b011100, 0b011001, 0b011010, 0b010011,
   0b101100, 0b100101, 0b100110, 0b100011,
   0b110100, 0b110001, 0b110010, 0b101001
};

typedef union __attribute__((packed)) { U8 _u8[4]; U16 _u16[2]; U32 _u32; } U_4;

PUBLIC S_BufU8 * _3of6_encode(U8 const *tbl, S_BufU8 * out, S_BufU8 const *src) {

   U16 one(U8 n) {
      return ((U16)tbl[HIGH_NIBBLE(n)] << 6) + tbl[LOW_NIBBLE(n)]; }

   U32 two(U16 n) {
      return ((U32)one(HIGH_BYTE(n)) << 12) + one(LOW_BYTE(n)); }

   typedef {U8 *bs; U8 nib} S_Nib;

   S_Nib * eat1Byte(S_Nib *dest, U8 const *src) {
      U8 bs[2];
      u16ToLE(one(*src));

       }

   U8 * eat2Bytes(U8 *dest, U8 const *src) {
      return u32ToLE(dest, two(leToU16(src))); }

}

// --------------------------------- eof --------------------------------------
