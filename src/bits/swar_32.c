
#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  Number of bits set. SWAR = Sideways addition - registered.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 SWAR32(U32 n)
{
   // Count the number of bits 0,1, or 2, in each of 16 2 bit blocks of n.
   n = n - ((n >> 1) & 0x55555555);

   // Count he number of bits 0..4, in each of 8 of 2 x 2-bit blocks
   n = (n & 0x33333333) + ((n >> 2) & 0x33333333);

   /* Count he number of bits 0..8, in each of 4 of 2 x 4-bit blocks. Result
      is 4 adjacent bytes each a bits-count of the original byte they supplant e.g

         0xA301927F -> 0x04010307

      Then since:
         k * 0x01010101 = (k << 24) + (k << 16) + (k << 8) + k.

      multiplying the count above by 0x01010101 and >> 24 gives e.g

         0x04010307 * 0x01010101 -> 0x0F000000 >> 24 -> 0x0F
   */
   return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


// --------------------- eof --------------------------------  -
