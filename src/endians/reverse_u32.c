#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  ReverseU32()
|
------------------------------------------------------------------------------------------*/

PUBLIC U32 ReverseU32(U32 n)
{
    return ((U32)ReverseU16(LOW_WORD(n)) << 16) + ReverseU16(HIGH_WORD(n));
}

// --------------------- eof --------------------------------  -
