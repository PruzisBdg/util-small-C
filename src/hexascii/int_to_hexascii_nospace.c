/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  IntToHexASCII_NoSpace()
|
|  Print 'n' to 'out' as 4-character hexASCII e.g   0x1204 -> "1204"
|
------------------------------------------------------------------------------------------*/

PUBLIC void IntToHexASCII_NoSpace(C8 *out, S16 n)
{
   ByteToHexASCII(out, HIGH_BYTE(n));
   ByteToHexASCII(out+2, LOW_BYTE(n));  // N.B 'out+1' gives compiler error with Raisonance RC51
}



// --------------------- eof --------------------------------  -
