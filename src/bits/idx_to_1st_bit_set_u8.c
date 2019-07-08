/*---------------------------------------------------------------------------
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  IdxToLsbSet_U8()
|
|  0 is lsb set. If no bits set (n == 0) then returns 0xFF.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 IdxToLsbSet_U8(U8 n)
{
   U8 c;

   for( c = 0; c < 8 * sizeof(U8); c++ ) {
      if( n & 0x01 ) return c;
      n >>= 1;
      }
   return 0xFF;
}


// --------------------- eof --------------------------------  -