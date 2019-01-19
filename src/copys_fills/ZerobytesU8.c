/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC void ZeroBytesU8(void RAM_IS *p, U8 cnt) 
{ 
   FillBytesU8( (U8 RAM_IS *)p, 0, cnt); 
}
 
