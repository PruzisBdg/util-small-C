#include "libs_support.h"
#include "util.h"

/* ---------------------------------- SRecord_Next ------------------------------------- */

PUBLIC C8 const * SRecord_Next(C8 const * str)
{
    U16 c;
    #define _MaxLineChars ((2 * 255) + 11)      // ':' + count + 2-byte addr + up to 255 HexASCII byte + checksum + \r\n

    for(c = 0; c < _MaxLineChars && *str != '\0'; c++, str++)
    {
        if(*str == ':')
            return str+1;
    }
    return 0;
}

/* ---------------------------------- SRecord_Parse ------------------------------------- */

PUBLIC C8 const * SRecord_Parse(S_SRecordInfo *r, C8 const * src)
{
    U8 c, *d;
    U16 sum;

     if( (src = GetNextHexASCIIByte(src, &r->numDataBytes)) == 0 )
        { return 0; }
    else
    {
        if((src = GetNextHexASCII_U16(src, &r->addr)) == 0)
            { return 0; }
        else
        {
            if( (src = GetNextHexASCIIByte(src, &r->type)) == 0 )
                { return 0; }
            else
            {
                for(c = 0, d = r->payload, sum = 0; c < r->numDataBytes+1; c++, d++)
                {
                    if( (src = GetNextHexASCIIByte(src, d)) == 0 )
                        { return 0; }
                    else
                        { sum += *d; }
                }
            }
            if( LOW_BYTE(sum + r->numDataBytes + HIGH_BYTE(r->addr) + LOW_BYTE(r->addr) + r->type) != 0)
                { return 0; }
            else
                { return src; }
        }
    }
}



// ----------------------------------- eof -------------------------------------
