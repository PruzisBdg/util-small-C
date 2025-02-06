/*---------------------------------------------------------------------------
|
|                          Word lists
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------
|
|  Str_Replace()
|
|  Replace 'numToCut' words in 'str' with 'numToInsert' words from 'toFind'.
|
|  Replacement is at the 1st SINGLE word in 'str' which matches the 1st WORD in
|  'to Find'.
|
|  If 'numToCut' exceeds the number of words in the remainder of the string
|  then the whole remainder is replaced.
|
|  If 'numToInsert' exceeds the number of words in 'toFind' then all the words
|  in 'toFind' are inserted.
|
|  If there is no match then 'str' is unchanged.
|
------------------------------------------------------------------------------*/

PUBLIC C8 GENERIC * Str_Replace( C8 GENERIC *str,
                  C8 GENERIC const *toFind,
                  C8 GENERIC const * replaceWith,
                  U8 numToCut,
                  U8 numToInsert )
{
   if(numToCut > 0)
   {
      U8 findIdx;

      if( (findIdx = Str_FindWord(str, toFind)) != _Str_NoMatch)
      {
         Str_Delete(str, findIdx, numToCut);
         Str_Insert(str, replaceWith, findIdx, numToInsert);
      }
   }
   return str;      // Return destination string, updated or no.
}

// ----------------------------- eof --------------------------------------
