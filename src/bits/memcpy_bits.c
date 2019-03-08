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

typedef S_BitAddr16 T_BitAddr;

//#define _bitAddr(_byte, _bit)    (((U16)(_byte) << 8) + (_bit))
//#define _byteIs(ba)              ((ba) >> 8)
//#define _bitIs(ba)               ((ba) & 0x00FF)

typedef struct {
   bool (*rd)(U8 *to, U8 from, U8 cnt );
   bool (*wr)(U8 to, U8 const *from, U8 cnt);
   bool (*get)(U8 *to, U8 from, U8 cnt);
} S_Intfc;

#define _byte(bitAddr) bitAddr16_Byte(bitAddr)
#define _bit(bitAddr)  bitAddr16_Bit(bitAddr)

static U8 mask(U8 msb, U8 width) {
   return MakeAtoBSet_U8(msb, width >= msb ? 0 : msb-width); }

static U8 orInto(U8 dest, U8 src, U8 srcMask) {
   return dest | (src & srcMask); }

PUBLIC void memcpy_bits(S_Intfc const *port, T_BitAddr dest, T_BitAddr src, U16 numBits)
{
   U8 open;
   for(U8 rem = numBits; rem > 0; rem -= open, src += open, dest += open)
   {
      // rd() byte containing 'dest'. Clear those bits which will be updated.
      U8 t;
      port->rd(&t, _byte(dest), 1);

      /* Will clear then fill 'open' bits in 't' from 'msb' rightward. If 'rem' > 'msb' this was
         fully 'msb' to b0.
      */
      U8 msb = _bit(dest);                         // This is the msbit of the 'dest' field..
      open = rem > msb ? msb : rem;                // Will clear these many bits.
      CLRB(t, mask(msb, open));                    // Clear'open' bits from 'msb'.

      // Copy 'open' bits from 'src' into the open field in 't'.
      U8 take;
      for(U8 ask = open; ask > 0; ask -= take) {
         U8 s;
         port->get(&s, _byte(src), 1);             // get() into 's' the byte containing 'src'.

         /* 's' has _bit(src)+1 bits from 'src'. Take some or all of these, depending on how
            much more we 'ask' for.
         */
         take = MinU8(ask, _bit(src)+1);

         /* Align the bits from 'src' (in 's')
         */
         s = lss(s, msb-_bit(src));

         t = orInto(t, src, mask(msb, take));
      }
   }
}


// ---------------------------------- eof --------------------------------  -
