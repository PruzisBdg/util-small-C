/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "util.h"

PUBLIC void CopyBytesU8_Reversed( U8 RAM_IS *dest, U8 RAM_IS *src, U8 cnt )
{
   if(cnt > 0)
   {
      for( dest += (cnt-1); cnt; cnt--, src++, dest-- ) *dest = *src;
   }
}

PUBLIC void CopyConstBytesU8_Reversed( U8 RAM_IS *dest, U8 CONST *src, U8 cnt )
{
   if(cnt > 0)
   {
      for( dest += (cnt-1); cnt; cnt--, src++, dest-- ) *dest = *src;
   }
}



// --------------------- eof --------------------------------
