/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_1stWordHasChar()
|
|  If the 1st word in 'lst' contains 'ch' then return an index to 'ch', RELATIVE TO
|  THE START OF 'LST' (not relative to the start of word). Return _Str_NoMatch if
|  'ch' not found.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT charEndsWord(C8 ch)
{
   return Str_Delimiter(ch) || ch == '\0';
}

PUBLIC U8 Str_1stWordHasChar( C8 GENERIC const *lst, U8 ch )
{
   if( charEndsWord(ch))                           // 'ch' is not a word char?
   {
      return _Str_NoMatch;                         // then can't match; we answer to words only.
   }
   else
   {
      U8 c;
      for(c = 0; Str_Delimiter(lst[c]); c++ ) {}   // Advance to start of first word in 'str'. (Or end-of-string if empty).

      for(; ; c++)                                 // Walk thru 'lst'
      {
         if(lst[c] == ch)                          // Matched 'ch'?
            { return c; }                          // then return index to match
         if( c >= 0xFE || charEndsWord(lst[c]) )   // Word is too long OR got word-delimiter?
            { return _Str_NoMatch; }               // the return failed to match.
      }
   }
}

// -------------------------------- eof -------------------------------------
