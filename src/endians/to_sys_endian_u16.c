#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  ToSysEndian_U16()
|
------------------------------------------------------------------------------------------*/

static inline U16 reverseU16(U16 n) { return ((LOW_BYTE(n)) << 8) + HIGH_BYTE(n); }

PUBLIC U16 ToSysEndian_U16(U16 n, E_EndianIs e)
{
   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
         return e == eBigEndian ? reverseU16(n) : n;
      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
         return e == eLittleEndian ? reverseU16(n) : n;
      #else
         return n;
      #endif

   #else
      #warning "ToSysEndian_U16() Endian undefined - bytes will always be copied no-reverse."
      return n;
   #endif // __BYTE_ORDER__
}

// --------------------- eof --------------------------------  -
