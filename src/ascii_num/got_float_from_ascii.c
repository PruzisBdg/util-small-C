/*---------------------------------------------------------------------------
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  GotFloatFromASCII()
|
|  Return the next number from 'inStr' into 'out', skipping any spaces. The number
|  can be an integer, hex (0xnn), or floating point as 123.45, 1.67E5.
|
|  Return 1 if a number was parsed, else zero.
|
|  This function is wrapper for ReadASCIIToFloat(), returning a boolean instead of
|  a pointer to the remainder of 'inTxt'.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT GotFloatFromASCII(U8 const *inTxt, float *out)
{
   return ReadASCIIToFloat(inTxt, out) != 0;
}

// --------------------------------- eof --------------------------------------
