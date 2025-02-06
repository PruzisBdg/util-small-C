/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "wordlist.h"
#include <string.h>

/*-----------------------------------------------------------------------------
|
|  Str_Delete()
|
|  Delete 'cnt' words from 'lst' from 'start' onwards e.g
|
|     ("abc def ghi jkl", 1, 2) ->  "abc jkl"
|
|  Deletion preserves additional delimiters between words in this way
|
|     ("abc;def.ghi:..jkl", 1, 2) ->  "abc;..jkl"
|
|  i.e the ':' at the end of excised section gets dropped so that the splice
|  preserves the number of delimiters.
|
------------------------------------------------------------------------------*/

// One past lead delimiter e.g  "...abc' -> '..abc', '..abc' -> '.abc', '.abc' -> 'abc'.
PRIVATE C8 const * trimLeadDelimiter(C8 *wd)
{
   while( Str_Delimiter(*wd) ) { wd--; }     // Starting on a delimiter; backup until last char of previous word
   return wd+2;                              // Forward 2 to skip earliest delimiter.
}

// e.g ("abc...def", 1) -> '..def' or ('abc.def', 1) -> 'def'
PRIVATE C8 const *trimLeadof_NthWord(C8 *wd, U8 idx)
{
   wd = Str_GetNthWord(wd, idx);
   return
      idx == 0 || *wd == '\0'             // 1st word (idx == 0)? OR empty "" wd?
         ? wd                             // then do NOT trim any leads; just return 'wd' as-is.
         : trimLeadDelimiter(wd-1);       // else trim lead delimiter (above)
}

PUBLIC C8 GENERIC * Str_Delete( C8 GENERIC *lst, U8 start, U8 cnt )
{
   // Note: We can case return from Str_GetNthWord() to non-const because we know that
   // return is 'lst' (which is non-const).
   return strcpy( Str_GetNthWord(lst, start), trimLeadof_NthWord(lst, start+cnt) );
}

// --------------------------- eof -------------------------------
