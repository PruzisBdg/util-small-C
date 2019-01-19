#include "libs_support.h"
#include "util.h"
#include <string.h>
#include <ctype.h>

/* ---------------------------------- macAddr_LegalStr ------------------------------------- */

PUBLIC BOOL macAddr_LegalStr(C8 const * macStr)
{
    U8 const digitsAt[] = {0,1,3,4,6,7,9,10,12,13,15,16};
    U8 const colonsAt[] = {2,5,8,11,14};
    U8 c;

    if(strlen(macStr) < _MacAddr_Chars)
        { return FALSE; }
    else
    {
        for(c = 0; c < RECORDS_IN(digitsAt); c++) {         // At each digit position...
            if( !IsHexASCII(macStr[digitsAt[c]]) )          // [0-9A-Fa-f]?
                return FALSE; }                             // No, fail.
        for(c = 0; c < RECORDS_IN(colonsAt); c++) {         // At each colon position...
            if( macStr[colonsAt[c]] != ':' )                // there's a colon?
                return FALSE; }                             // No, fail.
        if( isdigit(macStr[_MacAddr_Chars]) )               // There's a digit right after the last octect? e.g 00:11:22:33:44:556
            { return FALSE; }                               // then consider this bogus.
        return TRUE;
    }
}


/* ---------------------------------- macAddr_StrToNum ------------------------------------- */

U8 const octetsAt[] = {0,3,6,9,12,15};

PUBLIC BOOL macAddr_StrToNum(U_MACasNum *toNum, C8 const *fromStr)
{
    if(!macAddr_LegalStr(fromStr))
        {return FALSE; }
    else
    {
        U8 c;
        for(c = 0; c < _MacAddrAsBinary_bytes; c++)
            {  toNum->asBytes[c] = HexASCIItoByte(&fromStr[octetsAt[c]]); }
        return TRUE;
    }
}

/* ---------------------------------- macAddr_NumToStr ------------------------------------- */

PUBLIC C8 const * macAddr_NumToStr(C8 *toStr, U_MACasNum const *fromNum)
{
    memset(toStr, ':', _MacAddr_Chars);                                 // Prefill with colons

    U8 c;
    for(c = 0; c < _MacAddrAsBinary_bytes; c++)                         // For each digit...
        { ByteToHexASCII(&toStr[octetsAt[c]], fromNum->asBytes[c]); }  // ...write (over colons) as HexASCII.
    toStr[_MacAddr_Chars] = '\0';                                       // Terminate.
    return toStr;
}

/* ---------------------------------- macAddr_StrIsZero ------- (and legal) ---------------- */

PUBLIC BOOL macAddr_StrIsZero(C8 const * str)
    { return macAddr_LegalStr(str) && strncmp(str, "00:00:00:00:00:00", _MacAddr_Chars) == 0; }


/* ---------------------------------- macAddr_StrIsNonZero ------ (and legal) ---------------- */

PUBLIC BOOL macAddr_StrIsNonZero(C8 const * str)
    { return macAddr_LegalStr(str) && strncmp(str, "00:00:00:00:00:00", _MacAddr_Chars) != 0; }

/* ---------------------------------- macAddr_NumsEqual ------------------------------------- */

PUBLIC BOOL macAddr_NumsEqual(U_MACasNum const *a, U_MACasNum const *b)
    { return a->asU16[0] == b->asU16[0] &&
             a->asU16[1] == b->asU16[1] &&
             a->asU16[2] == b->asU16[2]; }




// ----------------------------------- eof -------------------------------------
