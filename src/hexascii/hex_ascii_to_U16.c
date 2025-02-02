
#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  HexASCII_ToU16()
|
------------------------------------------------------------------------------------------*/

PUBLIC U16 HexASCII_ToU16(C8 const *hexStr)
{
   U8  c1, c2;

   if( (hexStr = GetNextHexASCIIByte(hexStr, &c1)) )     // Got high byte?
   {
      if( (hexStr = GetNextHexASCIIByte(hexStr, &c2)) )  // Got low byte?
      {
         return ((U16)c1 << 8) + c2;                     // then output an int
      }
   }
   return 0;                                             // else failed somewhere, so return 0
}



// --------------------- eof --------------------------------  -
