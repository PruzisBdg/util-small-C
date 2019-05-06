/*---------------------------------------------------------------------------
|
| Find and read <tag> <int>
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>      // isdigit()
#include "arith.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  ReadTaggedASCIIInt()
|
|  Return from 'in' into 'out' the first number after 'tag', The number must be a signed
|  16bit integer.
|
|  This parser uses ReadDirtyASCIIInt() i.e it takes the largest sequence which completes
|  an integer (maximal match). The number may be bounded by any non-numeric chars, whitespace
|  or non-printing chars. The number may not be split. If a digit is directly preceded by '-',
|  the number is read as negative.
|
|  Returns pointer to the first char after the last byte read, NULL if did not find the 'tag'
|  or a number after it.
|
|  Note: If a number is NOT parsed '*out' is undefined.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const * ReadTaggedASCIIInt(C8 const *in, C8 const *tag, S16 *out)
{
    C8 const *tail = Str_TailAfterWord(in, tag);    // Goto after 'tag', if it's there.

    if( tail != NULL && *tail != '\0' ) {           // Got 'tag' and there's content beyond it?
        return ReadDirtyASCIIInt(tail, out); }      // then look for a number there.
    return NULL;        // 'tag' not found.
}

// ----------------------------------- eof ---------------------------------------
