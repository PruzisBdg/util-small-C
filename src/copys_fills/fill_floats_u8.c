/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "util.h"

PUBLIC void FillFloatsU8( float RAM_IS *a, float n, U8 cnt )
   { for( ; cnt; cnt--, a++ ) *a = n; }

// --------------------- eof --------------------------------  
