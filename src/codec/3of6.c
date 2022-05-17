/*---------------------------------------------------------------------------
|
| 3-of-6 encode & decode.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* ----------------------------------- _3of6_encode ------------------------------------------

   3-of6 encode 'src' into 'dest' using 'tbl'.

   'src.cnt' is the number of bytes to encode (in 'src.bs').

   'dest.cnt' is the number of bytes available for the encoded content in 'dest.bs'.

   'tbl' is 16 entries mapping nibbles 0x0-0xF to their sextets.

   Return 'dest' with 'dest.cnt' equal to the number of bytes occupied by the encoded content.

   'src' and 'dest' AND 'src.bs' and 'dest.bs' must be non-NULL. 'src' is not modified (const).

   Fail and return NULL if:
      - 'dest' does not have enough room for the encoded content;
            in whch case 'dest' is unmodified.
*/
PUBLIC S_BufU8 * _3of6_encode(U8 const *tbl, S_BufU8 * dest, S_BufU8 const *src) {

   // One byte to 2 sextets (12 bits), right-justified in U16
   U16 one(U8 n) {
      return ((U16)tbl[HIGH_NIBBLE(n)] << 6) + tbl[LOW_NIBBLE(n)]; }

   // Two bytes to 4 sextets (24 bits), right-justified in U32.
   U32 two(U16 n) {
      return ((U32)one(HIGH_BYTE(n)) << 12) + one(LOW_BYTE(n)); }

   // 1 src -> 2 encoded: 2 src -> 3 enc; 3 -> 5; 4->6 etc.
   U16 encodedCnt(U16 srcCnt) { return ((3 * (U32)src->cnt) + 1) / 2;}

   // ---------------------------------------------------------


   // Fail rightaway if 'src' is too large or 'dest' is too small for encoded content.
   if(dest->cnt < encodedCnt(src->cnt) ||
      src->cnt > 2 * (U32)MAX_U16 / 3 ) {
      return NULL; }

   // Nothing to encode? => Success doing nothing. Return empty 'dest'.
   else if(src->cnt == 0) {
      dest->cnt = 0;
      return dest; }

   /* else eat 2 bytes at a time, (producing 3 encoded bytes at a time), until there
      are one or zero bytes left. If one byte left then encode that into a final 2 bytes.

      Note that the endianess in reading in raw bytes and writing out encoded bytes does
      not matter, as long at the same is used for both in and out.
   */
   else {
      U8 const *s = src->bs;  U8 *d = dest->bs;             // 'from' and 'to'

      for(U16 rem = src->cnt; rem > 0; s+=2, d+=3)          // Until 'src' consumed...
      {                                                     // ...eat 2 bytes; make 3.
         if(rem == 1) {                                     // One left?
            u16ToLE(d, one(*s));                            // then finish off
            rem = 0; }
         else {                                             // else convert another 2 bytes.
            u32ToLE(d, two(leToU16(s)));
            rem -= 2; }
      } // for().
      dest->cnt = encodedCnt(src->cnt);      // Write encoded bytes produced.
      return dest; }                         // Success!
}

/* -------------------------- en13757_3of6 ----------------------------------------

   3-of-6 encoding for MBus Wireless Mode T, Meter-to-Other.
*/
PUBLIC S_BufU8 * en13757_3of6(S_BufU8 * dest, S_BufU8 const * src) {

   // See EN13757-4:2019 7.4.2.1 Table 10.
   U8 const en13757_3of6[] = {
      0b010110, 0b001101, 0b001110, 0b001011,
      0b011100, 0b011001, 0b011010, 0b010011,
      0b101100, 0b100101, 0b100110, 0b100011,
      0b110100, 0b110001, 0b110010, 0b101001 };

   return _3of6_encode(en13757_3of6, dest, src); }



// --------------------------------- eof --------------------------------------
