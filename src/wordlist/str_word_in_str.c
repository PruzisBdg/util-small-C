/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_WordInStr()
|
|  Return 1 if 'theWord' is anywhere in 'str', else 0
|
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Str_WordInStr( C8 GENERIC const *str, C8 GENERIC const *theWord )
{
   return Str_FindWord(str, theWord) != _Str_NoMatch;
}

// ---------------------------- eof ----------------------------------------
