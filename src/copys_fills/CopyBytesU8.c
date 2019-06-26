/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "util.h"

// This is the original function with the copy-right.
PUBLIC void CopyBytesU8( U8 RAM_IS *src, U8 RAM_IS *dest, U8 cnt )
   { for( ; cnt; cnt--, src++, dest++ ) *dest = *src; }

// i.e to left
PUBLIC void CopyLBytesU8( U8 RAM_IS *dest, U8 RAM_IS *src, U8 cnt )
   { for( ; cnt; cnt--, src++, dest++ ) *dest = *src; }

// --------------------- eof --------------------------------
