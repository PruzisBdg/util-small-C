/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "util.h"


PUBLIC void CopyConstBytesU8( U8 CONST *src, U8 RAM_IS *dest, U8 cnt ) 
   { for( ; cnt; cnt--, src++, dest++ ) *dest = *src; }


// --------------------- eof --------------------------------  
