#include "libs_support.h"
#include "util.h"
#include "arith.h"

/*-----------------------------------------------------------------------------------------
|
|  Number of bits set in bs[cnt]. SWAR = Sideways addition - registered.
|  Sum is clipped to 0xFFFF (MAX_U16)
|
------------------------------------------------------------------------------------------*/

PUBLIC U16 SWARU8s(U8 const *bs, U16 cnt)
{
   U16 i, sum;
   for(i = 0, sum = 0; i < cnt; i++) {
      sum = AplusB_U16(sum, SWARU8(bs[i]));
   }
   return sum;
}


// --------------------- eof --------------------------------  -
