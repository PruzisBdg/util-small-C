/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "util.h"

PUBLIC U16 SumBytesU8( U8 RAM_IS *a, U8 cnt )
{ 
   U16 sum;
   for( sum = 0; cnt; cnt--, a++ ) sum += *a;
   return sum;
}


// --------------------- eof -------------------------------- 
