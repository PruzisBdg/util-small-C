#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  ReverseU16()
|
------------------------------------------------------------------------------------------*/

PUBLIC U16 ReverseU16(U16 n)
{
    return ((n & 0x00FF) << 8) + (n >> 8);
}

// --------------------- eof --------------------------------  -
