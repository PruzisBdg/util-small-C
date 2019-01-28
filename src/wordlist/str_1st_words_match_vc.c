/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

extern BIT wordChar(U8 ch);

/*-----------------------------------------------------------------------------------------
|
|  Str_1stWordsMatch_VC()
|
|  Return 1 if 1st word in word lost 'w1' matches the 1st word in list 'w2'. A word is one
|  or more non-space chars, terminated by any non-word char.
|
|  Bypasses leading whitespace in 'w1' and 'w2'.
|
|  This is the version with 'w2' as a ROM pointer.
|
------------------------------------------------------------------------------------------*/

PRIVATE U8 GENERIC * stripSpc(U8 GENERIC *w)
{
   while( *w == ' ' )
      { w++; }
   return w;
}

PRIVATE U8 CONST * stripSpcROM(U8 CONST *w)
{
   while( *w == ' ' )
      { w++; }
   return w;
}

PUBLIC BIT Str_1stWordsMatch_VC( U8 GENERIC *w1, U8 CONST *w2 )
{
   U8 i;

   w1 = stripSpc(w1);
   w2 = stripSpcROM(w2);

   for(i = 0;;i++)
   {
      if( !wordChar(w1[i]) && !wordChar(w2[i]) )   // ended with match?
      {
         if(i == 0)                                // but didn't have word yet?
            { return 0; }                          // then no match
         else                                      // else at least 1 char matched
            { return 1; }                          // so success!
      }
      else if( w1[i] != w2[i] )                    // else mismatch?
         { return 0; }
   }
}

// ------------------------------- eof -----------------------------------
