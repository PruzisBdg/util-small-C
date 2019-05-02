/* -----------------------------------------------------------------------------------------

   64K bit-addressing:

   A bit address is U16, 2^13 bytes * 2^3 bits per byte.  This internal format allows addresses
   to be added with just an arithmetic sum.

   Big-Endian (BE) and little-endian (LE) have the internal bit addresses count through the
   bits of multi-byte variables in the order of the bits in those variables.

                            Big-endian                              Little-endian
      internal addr   0 1 2 3 4 5 6 7   8 9 A B C D E F      0 1 2 3 4 5 6 7   8 9 A B C D E F
          bit         7 6 5 4 3 2 1 0   7 6 5 4 3 2 1 0      0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7
      ----------------------------------------------------------------------------------------
        e.g 0x03F0    0 0 0 0 0 0 1 1   1 1 1 1 0 0 0 0      0 0 0 0 1 1 1 1   1 1 0 0 0 0 0 0

 Public:
      bit64K_MakeBE(),  bit64K_MakeLE()      - Construct a bit address from a (byte,bit).
      bit64K_BitBE(),   bit64K_BitLE()       - Return the bit compnent of a bit address.
      bit64K_Byte()                          - Return the byte component of a bit address.
      bit64K_AddBits()                       - Add bits to a bit address.
      bit64K_AddBytes()                      - Add bytes to a bit-address
      bit64K_Add()                           - Add 2 bit addresses.

---------------------------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

// The low 3 bits of 'T_bit64K' count the bit component of the address.
#define _BitRS    3
#define _MaxBit   ((1 << _BitRS)-1)       // i.e bit-count of address is 0..7
#define _BitMask  _MaxBit                 // 0x07

#define _revBit(b)  (7-(b))      // Map [0..7] -> [7..0]

// Return the byte or byte component of a bit address
static inline U8     bitsLE(T_bit64K ba)  { return ba & _BitMask; }
static inline U8     bitsBE(T_bit64K ba)  { return _revBit(ba & _BitMask); }
static inline U16    bytes(T_bit64K ba)   { return ba >> _BitRS; }

// The internal representation of the bit-address is contiguous; so just return the
// value of that.
static inline U16       getAddr(T_bit64K ba) { return (U16)ba; }
static inline T_bit64K  putAddr(U16 n)       { return (T_bit64K)n; }

/* ----------------------------- bit64K_MakeBE ----------------------------------------------

   Return the bit-address for ('_byte', '_bit') big-endian. If '_byte' + '_bit' exceeds 0xFFFF
   (16bit) clip to 0xFFFF.
*/
PUBLIC T_bit64K bit64K_MakeBE(U16 _byte, U8 _bit) {
   return
      AplusB_U16(                                        // Add, clipping to 0xFFFF...
         (U16)_byte << _BitRS,                           // the byte-part, plus
         (_bit & ~_BitMask) +                            // any of _bit to add to the byte part, plus
               _revBit(_bit &_BitMask) ); }              // the bit-part, with count reversed.

/* ----------------------------- bit64K_MakeLE ----------------------------------------------

   Return the bit-address for ('_byte', '_bit') little-endian. If '_byte' + '_bit' exceeds
   0xFFFF (16bit) clip to 0xFFFF.
*/
PUBLIC T_bit64K bit64K_MakeLE(U16 _byte, U8 _bit) {
   return
      AplusB_U16(                   // Add, clipping to 0xFFFF...
         (U16)_byte << _BitRS,      // the byte-part, plus...
         _bit); }                   // ...the bit part.

// ---- Extract the bit and byte parts of a bit-address. Bit-extraction is endian-dependent;
// same as on the way in.

// ------------------------------------------------------------------------------
PUBLIC U8 bit64K_BitBE(T_bit64K bf)
   { return bitsBE(bf); }

// ------------------------------------------------------------------------------
PUBLIC U8 bit64K_BitLE(T_bit64K bf)
   { return bitsLE(bf); }

// ------------------------------------------------------------------------------
PUBLIC U16 bit64K_Byte(T_bit64K bf)
   { return bytes(bf); }

/* ---- Add bits and bytes to a bit-address. Add two bit-addresses.

   Overrange after addition or subtraction is clipped to limits of T_bit64K (0 - 0xFFFF).
*/

// ------ Note: 'nbits' signed so may subtract ---------------------------------
PUBLIC T_bit64K bit64K_AddBits(T_bit64K src, S16 nbits) {
   return ClipS32toU16((S32)getAddr(src) + nbits); }

// ------ Note: 'bytes' signed so may subtract ---------------------------------
PUBLIC T_bit64K bit64K_AddBytes(T_bit64K src, S16 bytes) {
   return ClipS32toU16((S32)getAddr(src) + (8 * (S32)bytes)); }

// ------------------------------------------------------------------------------
PUBLIC T_bit64K bit64K_Add(T_bit64K a, T_bit64K b) {
   return AplusB_U16(getAddr(a), getAddr(b)); }

// ------------------------------------------ eof -------------------------------------------
