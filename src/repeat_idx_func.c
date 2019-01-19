

/*-----------------------------------------------------------------------------------------
|
|  RepeatIdxFunc()
|
|  This is a fast wrapper for a for() loop.  The index is passed to func() in ascending
|  order.
|
------------------------------------------------------------------------------------------*/

#ifdef TOOL_IS_RIDE_8051
   #pragma TINY
#endif

#include "util.h"

PUBLIC void RepeatIdxFunc( void( *func)(U8), U8 repeatCnt)
{
   U8 DATA c;
   
   for(c = 0; c < repeatCnt; c++)
   {
         #if _TOOL_IS == TOOL_RIDE_8051
      #pragma LARGE        // Force the function call to to large model (LCALL)
         #endif
      func(c);
   }
}

// -------------------------- eof -----------------------------------------

 
