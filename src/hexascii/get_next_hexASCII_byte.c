/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  GetNextHexASCIIByte()
|
|  Return the next HexASCII byte from 'hexStr' into 'out', skipping any spaces.
|
|  Returns pointer to the first char after the byte read, else 0 if reached something
|  other than a HexASCII char or a space.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const* GetNextHexASCIIByte(C8 const *hexStr, U8 *out)
{
   while(1)                                     // Run on until reach something, legal or not
   {
      if( *hexStr == ' ' )                      // Space?
      {
         hexStr++;                              // then skip past
      }
      else                                      // ...else not a space. So it's either HexASCII or something illegal
      {
         if( !IsHexASCII(*hexStr) || !IsHexASCII(*(hexStr+1)) )   // Not a HexASCII digit?
         {
            return 0;                           // so return, signalling junk input
         }
         else                                   // else it's a legal HexASCII digit
         {
            *out = HexASCIItoByte(hexStr);      // so decode it into 'out'
            return hexStr + 2;                  // and return pointer advanced to next char
         }
      }
   }
}



// --------------------- eof --------------------------------  -
