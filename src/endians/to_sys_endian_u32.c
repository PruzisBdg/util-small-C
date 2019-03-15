#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  ToSysEndian_U32()
|
------------------------------------------------------------------------------------------*/

static inline U32 reverseU32(U32 n) {  return (ReverseU16(LOW_WORD(n)) << 16) + ReverseU16(HIGH_WORD(n)); }

PUBLIC U32 ToSysEndian_U32(U32 n, E_EndianIs e)
{
   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
         return e == eBigEndian ? reverseU32(n) : n;
      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
         return e == eLittleEndian ? reverseU32(n) : n;
      #else
         return n;
      #endif

   #else
      #warning "ToSysEndian_U16() Endian undefined - bytes will always be copied no-reverse."
      return n;
   #endif // __BYTE_ORDER__
}

// --------------------- eof --------------------------------  -
