/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  GetNextHexASCIIByte_Lenient()
|
|  Return the next HexASCII byte from 'hexStr' into 'out', skipping any spaces.
|
|  This version (of GetNextHexASCIIByte()) also parse a single HEXASCII char if it is
|  followed by a non-HEXASCII char i.e parses e.g:
|      ' 3F', but also ' A:' -> 0x0A or '3n' -> 0x03
|
|  So will parse IP addresses where each field may be 1 or 2 digits.
|
|  Returns pointer to the first char after the byte read, else 0 if reached something
|  other than a HexASCII char or a space.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const* GetNextHexASCIIByte_Lenient(C8 const *hexStr, C8 *out)
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
