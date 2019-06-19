/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* -------------------------------- addNextByte ------------------------------------------

   Add next HexACSII byte to 'out', if that byte is right at *hexStr.

   If success, return true and advance *hexStr past the HexASCII we parsed (+2)
*/

static bool addNextByte(C8 const **hexStr, U32 *out, U8 *bytesGot)
{
   C8 const *p = *hexStr;

   if( !IsHexASCII(*p) || !IsHexASCII(*(p+1)) )       // Not a HexASCII digit?
   {
      return false;                                   // so return, signaling junk input
   }
   else                                               // else it's a legal HexASCII digit
   {
      *out = (*out << 8) + HexASCIItoByte(p);         // so add it to 'out'
      (*bytesGot)++;                                  // And we got one more byte.
      *hexStr += 2;                                   // Advance source pointer past what we parsed.
      return true;                                    // Success.
   }
}

/*-----------------------------------------------------------------------------------------
|
|  GetNextHexASCII_U8toU32()
|
|  Return the next HexASCII (8-32bit) unsigned int from 'hexStr' into 'out', skipping
|  any spaces before the first HexASCII byte. After that the number must be whole bytes
|  packed together i.e
|
|     good are:  ' AB'  ' ABCD'  'ABCDEF'  'ABCDEF01'
|     but        ' ABC' -> 0xAB, returning on 'C'
|                and e.g ' 12EF3' -> 0x12EF, returning on '3
|
|  Return the number of bytes read in 'bytesOut'
|
|  Returns pointer to the first char after the byte read. Return 0 (NULL) if did not
|  get at least one HexASCII byte. Also 'bytesGot' will be zero (0).
|
| If didn't get a legal number, then 'out' is undefined.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const* GetNextHexASCII_U8toU32(C8 const *hexStr, U32 *out, U8 *bytesGot)
{
   U8 n;

   if( (hexStr = GetNextHexASCIIByte(hexStr, &n)) )         // Got a (first) byte?
   {
      *out = n;  *bytesGot = 1;                             // which is this.

      // Add up to another 3 bytes to 'out'.
      if(true == addNextByte(&hexStr, out, bytesGot)) {
         if(true == addNextByte(&hexStr, out, bytesGot)) {
            addNextByte(&hexStr, out, bytesGot); } }
      return hexStr;
   }
   // Did not get at least one byte. Fail, and 0 bytes got.
   *bytesGot = 0;
   return 0;
}



// --------------------- eof --------------------------------  -
