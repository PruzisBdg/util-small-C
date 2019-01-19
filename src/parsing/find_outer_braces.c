#include "libs_support.h"
#include "util.h"
#include <string.h>

/* ------------------------------- FindOuterBraces --------------------------------------*/

PUBLIC BOOL FindOuterBraces(C8 const *str, S_Braces const *b, U16 maxChars, S_BracketedAt *found)
{
    C8 *p;

    found->open = found->payload = found->close = 0;

    if( (p = strstr(str, b->toOpen)) == NULL)                   // No opening brace?
    {
        return FALSE;                                           // then fail.
    }
    else                                                        // else found open'.
    {
        found->open = p - str;                                  // index to open is...
        found->payload = found->open + strlen(b->toOpen);       // payload starts after 'open'.

        if(found->payload > maxChars)                           // But payload is too far down string?
        {
            return FALSE;                                       // then fail.
        }
        else                                                    // else search for last 'close'
        {
            while(1)
            {
                if( (p = strstr(p, b->toClose)) == NULL)        // (No more) 'close' sequences.
                {                                               // then either we snagged the last one, or there were none.
                    return                                      // success if
                        found->close <= maxChars &&             // 'close is not too far down string? AND
                        found->close > found->open;             // 'close' is past open, menaing we actually found one.
                }
                else                                            // else found a 'close'. Look for more
                {
                    found->close = p-str;                       // Mark the latest 'close'.

                    if(*(++p) == '\0')                          // Ahead 1 char (this is a multi-char match); Hit end-of-string?
                    {
                        return                                  // then return if 'close' is legal, same as above.
                            found->close <= maxChars &&
                            found->close > found->open;
                    }
                }
            }
        }
    }
}


// ----------------------------------- eof -------------------------------------
