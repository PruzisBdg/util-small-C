#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  ReverseInPlace()
|
|  This reverse doesn't have to check for source-destination overlaps.
|
------------------------------------------------------------------------------------------*/

PUBLIC void ReverseBytesInPlace(U8 *io, U16 nBytes)
{
   #define _swap(a, b) \
      { U8 n = a; a = b; b = n; }

   if(nBytes > 1) {
      U16 i, j;
      for(i = 0, j = nBytes-1; j > i; i++, j--) {
         _swap(io[i], io[j]); }}
}

// --------------------- eof --------------------------------  -
