#include "libs_support.h"
#include "util.h"
#include "arith.h"

// A bit address is U16, 2^13 bytes * 2^3 bits per byte.
#define _MaxUcnt(typ)  (sizeof(type) == 1 ? MAX_U8 ? (sizeof(type) == 2 ? MAX_U16 : MAX_U32) )

#define _BitRS 3
#define _MaxBit ((1 << _BitRS)-1)
#define _BitMask _MaxBit

#define _MaxByte ((1U << ((8*sizeof(S_BitAddr16)) - 1 - _BitRS)) - 1)

static inline S_BitAddr16 makeAddr(U16 _byte, U8 _bit) {
   return ( (S_BitAddr16)MinU16(_byte, _MaxByte) << _BitRS) + MinU8(_bit, _MaxBit); }

static inline U8           bits(S_BitAddr16 bf)    { return bf & _BitMask; }
static inline U16          bytes(S_BitAddr16 bf)   { return bf >> _BitRS; }

// The internal representation of the bit-address is contiguous; so just return the
// value of that.
static inline U16          getAddr(S_BitAddr16 bf) { return (U16)bf; }
static inline S_BitAddr16  putAddr(U16 n)          { return (S_BitAddr16)n; }

// ------------------------------------------------------------------------------
PUBLIC S_BitAddr16 bitAddr16_Make(U16 _byte, U8 _bit)
   { return makeAddr(_byte, _bit); }

// ------------------------------------------------------------------------------
PUBLIC U8 bitAddr16_Bit(S_BitAddr16 bf)
   { return bits(bf); }

// ------------------------------------------------------------------------------
PUBLIC U16 bitAddr16_Byte(S_BitAddr16 bf)
   { return bytes(bf); }

// ------------------------------------------------------------------------------
PUBLIC S_BitAddr16 bitAddr16_AddBits(S_BitAddr16 src, S16 nbits) {
   S16 s = getAddr(src) + nbits;
   return putAddr(s < 0 ? 0 : s);
}

// ------------------------------------------------------------------------------
PUBLIC S_BitAddr16 bitAddr16_Add(S_BitAddr16 a, S_BitAddr16 b) {
   S16 s = getAddr(a) + getAddr(b);
   return putAddr(s < 0 ? 0 : s); }

// ------------------------------------------ eof -------------------------------------------
