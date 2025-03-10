#include "libs_support.h"
#include "util.h"
#include <stddef.h>
#include <ctype.h>

/* ---------------------------------- BypassLeadingWhitespace -------------------------------------------------*/

PUBLIC C8 * BypassLeadingWhitespace(C8 *str)
{
    while(*str != '\0' && isspace((U8)*str))
        { str++; }
    return str;
}

/* ---------------------------------- StripTrailingWhitespace -------------------------------------------------*/

PUBLIC C8 * StripTrailingWhitespace(C8 *str)
{
    C8 *p = EndStr(str);
    while( p > str )
    {
        p--;
        if(isspace((U8)*p))
            { *p = '\0'; }
        else
            { break; }
    }
    return str;
}



// ----------------------------------- eof -------------------------------------
