/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "util.h"

PUBLIC  U8 RAM_IS * CopyBytesU8_Reversed( U8 RAM_IS *dest, U8 RAM_IS *src, U8 cnt )
{
   if(cnt > 0)
   {
      for( U8 RAM_IS *p = dest + (cnt-1); cnt; cnt--, src++, p-- ) *p = *src;
   }
   return dest;
}

PUBLIC U8 RAM_IS * CopyConstBytesU8_Reversed( U8 RAM_IS *dest, U8 CONST *src, U8 cnt )
{
   if(cnt > 0)
   {
      for( U8 RAM_IS *p = dest + (cnt-1); cnt; cnt--, src++, p-- ) *p = *src;
   }
   return dest;
}



// --------------------- eof --------------------------------
