/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "util.h"

PUBLIC void FillIntsU8( S16 RAM_IS *a, S16 n, U8 cnt )
   { for( ; cnt; cnt--, a++ ) *a = n; }

// --------------------- eof -------------------------------- 
