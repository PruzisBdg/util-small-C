/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  GetNextHexASCII_U32()
|
|  Return the next HexASCII (16bit) int from 'hexStr' into 'out', skipping any spaces.
|
|  Returns pointer to the first char after the byte read, else 0 if reached something
|  other than a HexASCII char or a space. If didn't get a legal number, then 'out'
|  will be unchanged.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const* GetNextHexASCII_U32(C8 const *hexStr, U32 *out)
{
   U16  n1, n2;

   if( (hexStr = GetNextHexASCII_U16(hexStr, &n1)) )     // Got high byte?
   {
      if( (hexStr = GetNextHexASCII_U16(hexStr, &n2)) )  // Got low byte?
      {
         *out = ((U32)n1 << 16) + n2;                    // then output an int
         return hexStr;                                  // and return ptr to next char
      }
   }
   return 0;                                             // else failed somewhere, so return 0
}



// --------------------- eof --------------------------------  -
