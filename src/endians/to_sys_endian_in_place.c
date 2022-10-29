#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  ToSysEndian_InPlace()
|
------------------------------------------------------------------------------------------*/

PUBLIC void ToSysEndian_InPlace(U8 *io, U16 nBytes, E_EndianIs e)
{
   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
         if(e == eBigEndian) { ReverseBytesInPlace(io, nBytes); }
      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
         if(e == eLittleEndian) { ReverseBytesInPlace(io, nBytes); }
      #else
         // No-endian; do nothing
      #endif
   #else
      #warning "ToSysEndian_InPlace() Endian undefined - bytes will left unchanged."
   #endif // __BYTE_ORDER__
}

// --------------------- eof --------------------------------  -
