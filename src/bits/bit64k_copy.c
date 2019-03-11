/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

#define _CLRB(reg,mask) ((reg) &= ~(mask))


/*-----------------------------------------------------------------------------------------
|
|  memcpy_bits()
|
------------------------------------------------------------------------------------------*/

static U8 lss(U8 n, S8 ls) {
   return
      ls == 0
         ? n
         : (ls > 0
            ? n << ls
            : n >> (-ls)); }

typedef S_Bit64K T_BitAddr;

#define _byte(bitAddr) Bit64K_Byte(bitAddr)
#define _bit(bitAddr)  Bit64K_BitBE(bitAddr)

static U8 mask(U8 msb, U8 width) {
   return MakeAtoBSet_U8(msb, width >= msb ? 0 : msb-width); }

static U8 orInto(U8 dest, U8 src, U8 srcMask) {
   return dest | (src & srcMask); }

PUBLIC void Bit64K_Copy(S_Bit64KPort const *port, S_Bit64K dest, S_Bit64K src, U16 numBits)
{
   U8 open;
   for(U8 rem = numBits; rem > 0; rem -= open, src += open, dest += open)
   {
      // rdDest() byte containing 'dest'. Clear those bits which will be updated.
      U8 t;
      port->rdDest(&t, _byte(dest), 1);

      /* Will clear then fill 'open' bits in 't' from 'msb' rightward. If 'rem' > 'msb' this was
         fully 'msb' to b0.
      */
      U8 msb = _bit(dest);                            // This is the msbit of the 'dest' field..
      open = rem > msb+1 ? msb+1 : rem;               // Will clear these many bits.
      CLRB(t, mask(msb, open));                       // Clear 'open' bits from 'msb'.

      /* Copy 'open' bits from 'src' into the open field in 't'. This needs either 1 or 2
         byte-reads from 'src', depending on whether the field to be copied crosses a byte
         boundary.
      */
      U8 s;
      port->getSrc(&s, _byte(src), 1);                // getSrc() into 's' the byte containing 'src'.

      /* 's' has _bit(src)+1 bits from 'src'. Take some or all of these, depending on how
         much more we 'ask' for.
      */
      U8 got = MinU8(open, _bit(src)+1);              // Take up to 'open' bits from 'src' forward to b0 of byte 't'

      /* Align the bits from 'src' (in 's')
      */
      s = lss(s, msb-_bit(src));                      // Align the field from 'src' (in 's')
      t = orInto(t, s, mask(msb, got));               // Mask to select just that field and OR into dest byte.

      if(open > got)                                  // Got a partial destination field from 1st byte?
      {
         port->getSrc(&s, _byte(src)+1, 1);           // then getSrc() next byte into 's'

         msb -= got;                                  // Move the 'dest' msb marker past the partial source field we already wrote.
         s = lss(s, msb);
         t = orInto(t, s, mask(msb, open-got));
      }
      port->wrDest(_byte(dest), &t, 1);               // Destination byte updated; put it back.
   }
}


// ---------------------------------- eof --------------------------------  -
