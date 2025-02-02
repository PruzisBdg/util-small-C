/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  HexASCII_ToU32()
|
------------------------------------------------------------------------------------------*/

PUBLIC U32 HexASCII_ToU32(C8 const *hexStr)
{
   U16  n1, n2;

   if( (hexStr = GetNextHexASCII_U16(hexStr, &n1)) )     // Got high byte?
   {
      if( (hexStr = GetNextHexASCII_U16(hexStr, &n2)) )  // Got low byte?
      {
         return ((U32)n1 << 16) + n2;                    // then output an int
      }
   }
   return 0;                                             // else failed somewhere, so return 0
}



// --------------------- eof --------------------------------  -
