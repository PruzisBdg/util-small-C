/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  memcpy_bits()
|
------------------------------------------------------------------------------------------*/

typedef struct { U8 *base; U16 ofs; } S_BitAddr;

#define _bitAddr(_byte, _bit)    (((U16)(_byte) << 8) + (_bit))
#define _byteIs(ba)              ((ba) >> 8)
#define _bitIs(ba)               ((ba) & 0x00FF)

PUBLIC void memcpy_bits(S_BitAddr const *dest, S_BitAddr const *src, U16 numBits)
{
   U8 t0, m0, i;

   t0 = dest->base[_byteIs(dest->ofs) + i];

   m0 = 0x01FF << _bitIs(dest->ofs);
   m0 = ((1 << numBits) - 1) << _bitIs(dest->ofs)
   m0 = _bitIs(dest->ofs);
}

// ---------------------------------- eof --------------------------------  -
