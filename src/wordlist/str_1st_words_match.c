/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  Str_1stWordsMatch()
|
|  Return 1 if 1st word in word list 'w1' matches the 1st word in list 'w2'. A word is one
|  or more non-space chars, terminated by any non-word char.
|
|  Bypasses leading whitespace in 'w1' and 'w2'.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT Str_1stWordsMatch( U8 GENERIC const *w1, U8 GENERIC const *w2 )
{
   w1 = Str_LTrim(w1);
   w2 = Str_LTrim(w2);

   U8 i;

   for(i = 0;;i++)
   {
      if( !Str_WordChar(w1[i]) && !Str_WordChar(w2[i]) )   // Both strings or words ended (together)?
      {
         if(i == 0)                                // but didn't have word yet?
            { return 0; }                          // then no match
         else                                      // else at least 1 char matched
            { return 1; }                          // so success!
      }
      else if( w1[i] != w2[i] )                    // else these 2 chars didn't match?
         { return 0; }                             // so fail.
   }
}

// ------------------------------- eof -----------------------------------
