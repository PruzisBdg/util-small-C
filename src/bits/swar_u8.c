
#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  Number of bits set. SWAR = Sideways addition - registered.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 SWARU8(U8 n)
{
   // Count the number of bits 0,1, or 2, in each of 4 2 bit blocks of n.
   n = n - ((n >> 1) & 0x55);

   // Count he number of bits 0..4, in each of 2 of 2 x 2-bit blocks
   n = (n & 0x33) + ((n >> 2) & 0x33);

   /* Count he number of bits 0..8, in 2 x 4-bit blocks (nibbles). Result in
      the lower nibble.
   */
   return (n + (n >> 4)) & 0x0F;
}


// --------------------- eof --------------------------------  -
