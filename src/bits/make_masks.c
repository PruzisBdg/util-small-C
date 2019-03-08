#include "libs_support.h"
#include "util.h"
#include "arith.h"

/*-----------------------------------------------------------------------------------------
|
|  Make bit masks e.g  f(msb = 7, lsb = 4) -> 0xF0
|
|  These functions tolerate bogus input. If 'lsb' > 'msb'. If 'msb' or lsb is bigger than
|  (U8) the return no mask.
|
|  If the mask specified is partly outside the type, e.g U8, then returns that part
|  of the mask which overlays the type.
|
|  e.g   mask(0,0) -> 0x01   (sets just b0)
|        mask(6,1) -> 0x7E
|        mask(7,7) -> 0x80
|        mask(20,1) -> 0xFE  (the 20 is clipped to 7)
|
------------------------------------------------------------------------------------------*/

static inline U8 setToU8(U8 n) { return (1U << (n+1)) - 1; }

#define setTo(n) setToU8(n)

PUBLIC U8 MakeAtoBSet_U8(U8 msb, U8 lsb)
{
   // Clip to avoid maybe rotate with msb > 7. ANSI does not define result when shift exceeds size of promoted var.
   msb = MinU8(msb, 7);
   return
      lsb > msb || lsb > 7                      // msb,lsb crossed? OR lsb too large for byte?
         ? 0                                    // then zero-mask
         : (lsb == 0                            // lsb is b0?
            ? setTo(msb)                        // then set all bits from msb down.
            : (setTo(msb) ^ setTo(lsb-1)));     // else set msb-lsb inclusive
}

// -------------------------------------------------------------------------------------------

static inline U16 setToU16(U8 n) { return n >= 15 ? 0xFFFF : (1U << (n+1)) - 1; }

#undef setTo
#define setTo(n) setToU16(n)

PUBLIC U16 MakeAtoBSet_U16(U8 msb, U8 lsb)
{
   // Clip to avoid maybe rotate with msb > 15. ANSI does not define result when shift exceeds size of promoted var.
   msb = MinU8(msb, 15);
   return
      lsb > msb || lsb > 15                     // msb,lsb crossed? OR lsb too large for byte?
         ? 0                                    // then zero-mask
         : (lsb == 0                            // lsb is b0?
            ? setTo(msb)                        // then set all bits from msb down.
            : (setTo(msb) ^ setTo(lsb-1)));     // else set msb-lsb inclusive
}

// -------------------------------------------------------------------------------------------

static inline U32 setToU32(U8 n) { return n >= 31 ? 0xFFFFFFFF : (1UL << (n+1)) - 1; }

#undef setTo
#define setTo(n) setToU32(n)

PUBLIC U32 MakeAtoBSet_U32(U8 msb, U8 lsb)
{
   // Clip to avoid maybe rotate with msb > 31. ANSI does not define result when shift exceeds size of promoted var.
   msb = MinU8(msb, 31);
   return
      lsb > msb || lsb > 31                     // msb,lsb crossed? OR lsb too large for byte?
         ? 0                                    // then zero-mask
         : (lsb == 0                            // lsb is b0?
            ? setTo(msb)                        // then set all bits from msb down.
            : (setTo(msb) ^ setTo(lsb-1)));     // else set msb-lsb inclusive
}

// ----------------------------------- EOF ----------------------------------------------
