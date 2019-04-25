/* -----------------------------------------------------------------------------------------

   64K bit-addressing:

   A bit address is U16, 2^13 bytes * 2^3 bits per byte.  This internal format allows addresses
   to be added directly.

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

#define _MaxUcnt(typ)  (sizeof(type) == 1 ? MAX_U8 ? (sizeof(type) == 2 ? MAX_U16 : MAX_U32) )

#define _BitRS 3
#define _MaxBit ((1 << _BitRS)-1)
#define _BitMask _MaxBit

#define _MaxByte ((1U << ((8*sizeof(S_Bit64K)) - 1 - _BitRS)) - 1)

#define _revBit(b)  (7-(b))

static inline S_Bit64K makeAddrBE(U16 _byte, U8 _bit) {
   return ( (S_Bit64K)MinU16(_byte, _MaxByte) << _BitRS) + _revBit(MinU8(_bit, _MaxBit)); }

static inline S_Bit64K makeAddrLE(U16 _byte, U8 _bit) {
   return ( (S_Bit64K)MinU16(_byte, _MaxByte) << _BitRS) + MinU8(_bit, _MaxBit); }

static inline U8           bitsLE(S_Bit64K bf)  { return bf & _BitMask; }
static inline U8           bitsBE(S_Bit64K bf)  { return _revBit(bf & _BitMask); }
static inline U16          bytes(S_Bit64K bf)   { return bf >> _BitRS; }

// The internal representation of the bit-address is contiguous; so just return the
// value of that.
static inline U16       getAddr(S_Bit64K bf) { return (U16)bf; }
static inline S_Bit64K  putAddr(U16 n)       { return (S_Bit64K)n; }

// ------------------------------------------------------------------------------
PUBLIC S_Bit64K bit64K_MakeBE(U16 _byte, U8 _bit)
   { return makeAddrBE(_byte, _bit); }

PUBLIC S_Bit64K bit64K_MakeLE(U16 _byte, U8 _bit)
   { return makeAddrLE(_byte, _bit); }

// ------------------------------------------------------------------------------
PUBLIC U8 bit64K_BitBE(S_Bit64K bf)
   { return bitsBE(bf); }

// ------------------------------------------------------------------------------
PUBLIC U8 bit64K_BitLE(S_Bit64K bf)
   { return bitsLE(bf); }

// ------------------------------------------------------------------------------
PUBLIC U16 bit64K_Byte(S_Bit64K bf)
   { return bytes(bf); }

// ------------------------------------------------------------------------------
PUBLIC S_Bit64K bit64K_AddBits(S_Bit64K src, S16 nbits) {
   return ClipS32toU16((S32)getAddr(src) + nbits); }

// ------------------------------------------------------------------------------
PUBLIC S_Bit64K bit64K_AddBytes(S_Bit64K src, S16 bytes) {
   return bit64K_AddBits(src, 8*bytes); }

// ------------------------------------------------------------------------------
PUBLIC S_Bit64K bit64K_Add(S_Bit64K a, S_Bit64K b) {
   return AplusB_U16(getAddr(a), getAddr(b)); }

// ------------------------------------------ eof -------------------------------------------
