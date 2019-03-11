/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

// ---------------------------- Shift left if ls > 0 else shift right.
static U8 shiftLR(U8 n, S8 ls) {
   return
      ls == 0
         ? n
         : (ls > 0
            ? n << ls
            : n >> (-ls)); }

// ----------------------------------------------------------------------------------------
static U8 rotL(U8 n, U8 by) {
   by &= 0x07;
   return (n<<by) | (n>>( (-by)&0x07 )); }

#define _byte(bitAddr) bit64K_Byte(bitAddr)
#define _bit(bitAddr)  bit64K_BitBE(bitAddr)

// ------------------------- e.g mask(6,2) -> 0x60;  mask(4,3) -> 0x1C.
static U8 mask(U8 msb, U8 width) {
   return MakeAtoBSet_U8(msb, width > msb ? 0 : msb-width+1); }

// ----------------------------------------------------------------------------------------
static U8 orInto(U8 dest, U8 src, U8 srcMask) {
   return dest | (src & srcMask); }


/*-----------------------------------------------------------------------------------------
|
|  bit64K_Copy()
|
|  Using 'port', copy 'numBits' from 'src' to 'dest', where these are 64K bitfield addresses.
|  'port' is a
|
------------------------------------------------------------------------------------------*/

PUBLIC void bit64K_Copy(S_Bit64KPorts const *port, S_Bit64K dest, S_Bit64K src, U16 numBits)
{
   U8 open;
   for(U8 rem = numBits; rem > 0; rem -= open, src += open, dest += open)
   {
      // rdDest() byte containing 'dest'. Clear those bits which will be updated.
      U8 db;
      port->rdDest(&db, _byte(dest), 1);

      /* Will clear then fill 'open' bits in 't' from 'msb' rightward. If 'rem' > 'msb' this was
         fully 'msb' to b0.
      */
      U8 msb = _bit(dest);                            // This is the msbit of the 'dest' field..
      open = rem > msb+1 ? msb+1 : rem;               // Will clear these many bits.
      CLRB(db, mask(msb, open));                      // Clear 'open' bits from 'msb'.

      /* Copy 'open' bits from 'src' into the open field in 't'. This needs either 1 or 2
         byte-reads from 'src', depending on whether the field to be copied crosses a byte
         boundary.
      */
      U8 sb;
      port->getSrc(&sb, _byte(src), 1);                // getSrc() into 's' the byte containing 'src'.

      /* 's' has _bit(src)+1 bits from 'src'. Take some or all of these, depending on how
         much more we 'ask' for.
      */
      U8 got = MinU8(open, _bit(src)+1);              // Take up to 'open' bits from 'src' forward to b0 of byte 't'

      /* Align the bits from 'src' (in 's')
      */
      sb = shiftLR(sb, msb-_bit(src));                // Shift left or right to align the field from 'src' with 'dest'.
      db = orInto(db, sb, mask(msb, got));            // Mask to select just that field and OR into dest byte.

      if(open > got)                                  // Got a partial destination field from 1st byte?
      {
         port->getSrc(&sb, _byte(src)+1, 1);          // then the remainder of the field into 'sb'.

         msb -= got;                                  // Move the 'dest' msb marker past the partial source field we already wrote.
         sb = rotL(sb, msb+1);                        // Rotate partial field left to align with destination slot.
         db = orInto(db, sb, mask(msb, open-got));    // Mask to select just that field and OR into dest byte.
      }
      port->wrDest(_byte(dest), &db, 1);              // Destination byte updated; put it back.
   }
}


// ---------------------------------- eof --------------------------------  -
