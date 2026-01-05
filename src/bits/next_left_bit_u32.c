
#include "libs_support.h"
#include "util.h"

/* ----------------------------- NextLeftBitU32 ---------------------------------------------

   Given iterator 'i' with a 1-bit mask, return the next (left) bit in m->rem under m->mask;
   0 (zero) if no more bits.

   E.g start with {.mask = 1, .rem = 40200001h}, gives 1h -> 20_0000h -> 4000_0000h -> 0
*/


U32 NextLeftBitU32(S_BitMaskIterU32 *m) {
   if(m->rem == 0) {                      // No (more) bits set?
      return 0; }                         // then 0 -> Done.
   else {                                 // else at least 1 more bit to extract.
      while((m->rem & m->mask) == 0) {    // Until 'mask' is on a bit in 'rem'...
         m->mask <<= 1;}                  // ...LS 'mask'
      m->rem -= m->mask;                  // Remove that bit from 'rem'
      return m->mask; }}                  // and return just that bit.

// --------------------------------- eof ----------------------------------------------------
