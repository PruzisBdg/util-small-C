/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  GetNextHexASCIIInt()
|
|  Return the next HexASCII (16bit) int from 'hexStr' into 'out', skipping any spaces.
|
|  Returns pointer to the first char after the byte read, else 0 if reached something
|  other than a HexASCII char or a space. If didn't get a legal number, then 'out'
|  will be unchanged.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const* GetNextHexASCIIInt(C8 const *hexStr, S16 *out)
{
   U8  c1, c2;

   if( (hexStr = GetNextHexASCIIByte(hexStr, &c1)) )     // Got high byte?
   {
      if( (hexStr = GetNextHexASCIIByte(hexStr, &c2)) )  // Got low byte?
      {
         *out = ((S16)c1 << 8) + c2;                     // then output an int
         return hexStr;                                  // and return ptr to next char
      }
   }
   return 0;                                             // else failed somewhere, so return 0
}



// --------------------- eof --------------------------------  -
