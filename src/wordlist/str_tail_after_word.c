/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_TailAfterWord()
|
|  Find the word in 'lst' which matches the FIRST OR ONLY word in 'str'. Return a pointer in
|  'lst' to the 1st char beyond the matching word (which may be end of string).
|
|  Return NULL if there's no match.
|
|  A word is one or more non-space characters.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 GENERIC const * Str_TailAfterWord( C8 GENERIC const *lst, C8 GENERIC const *str )
{
   U8 rIDATA ch,           // current char from 'lst'
            idx = 0,       // indexes 'str'
            wordCnt = 0;   // counts through words in 'lst'

   BIT      wasSpc = 1;    // if last char was a space
   BIT      neq = 0;       // when comparing words, '1' if a mismatch

   str = Str_LTrim(str);          // Advance to start of first word in 'str'. (Or end-of-string if empty).

   while(1)
   {
      ch = *lst++;                              // next char from list

      if( ch == '\0' )                          // end of list?
      {                                         // then whether we matched or no, there's nothing more...
         if( neq == 0 && !Str_WordChar(str[idx]))   // match so far AND end of 'str' OR end of 1st word in 'str'
         {
            return lst-1;                       // then found match but there nothing afterwards. Return end of 'lst'.
         }
         else
         {
            return NULL;                        // else no match in 'lst'
         }
      }
      else if( Str_Delimiter(ch)  )             // Next ch is delimiter?
      {
         if( wasSpc == 0 )                      // last char wasn't space (AND have space now)?
         {
            wasSpc = 1;                         // mark it now

            if( neq == 0 && !Str_WordChar(str[idx]))// match so far AND end of 'str' OR end of 1st word in 'str'
            {
               return lst-1;                    // then found match for 'str'; return index to matching word
            }
            else                                // else words don't match, will keep looking
            {
               wordCnt++;                       // on to the next word
               neq = 0;                         // assume a match until found otherwise
               idx = 0;                         // reset 'str' ptr
            }
         }
      }
      else                                      // else a char other than space
      {
         wasSpc = 0;

         if( neq == 0 )                         // word matches so far?
         {
            if( ch != str[idx] )                // but this char doesn't match?
            {
               neq = 1;                         // then note the mismatch
            }
            else                                // else will keep comparing
            {
               idx++;                           // so must bump 'str' ptr
            }
         }
      }
   }
}

// ------------------------------ eof ---------------------------------------
