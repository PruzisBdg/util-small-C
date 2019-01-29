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
|  Make a 'gap' at 'p' by moving the remainder of the string forward
|
------------------------------------------------------------------------------*/

PRIVATE void makeGap( U8 *p, U8 gap)
{
   U8 bytesToMove;

   /* Rem strlen(p) returns to number of chars to the end of 'dest' EXCLUDING the '\0',
      which we must also move; so '+1'
   */
   bytesToMove = strlen((C8*)p)+1;

   do
   {
      bytesToMove--;
      p[bytesToMove+gap] = p[bytesToMove];
   }
   while(bytesToMove);
}

/*-----------------------------------------------------------------------------
|
|  Str_Insert()
|
|  Insert  'cnt' words from 'src' into 'dest', starting in front of word 'start'
|
|  If 'start' exceeds the number of words in 'dest' then words from 'src' are
|  appended to 'dest'.
|
|  If 'cnt' exceeds the number of words in 'src' then all words in 'src' are
|  inserted into 'dest'.
|
|  If an insertion then a delimiter must be inserted after the end of the last
|  inserted word (before the next word). If multiple delimiters are defined (in
|  Str_Delimiters[]) then the 1st delimiter in that list is the one which is inserted.
|
|  Returns 'dest'.
|
------------------------------------------------------------------------------*/

PUBLIC U8 GENERIC * Str_Insert( U8 GENERIC *dest, U8 GENERIC const *src, U8 start, U8 cnt )
{
   if( cnt > 0 )                                                     // insert at least one word?
   {
      /* Will insert from start of 'src' to end of nth word ('endOfInsert'). (if start and finish are
         the same letter then there's one byte to insert).
      */
      U8 *endOfInsert = Str_GetEndWord((U8 GENERIC*)src, cnt-1);     // Get end of cnt-th word in src.

      if(*endOfInsert != '\0')                                       // NOT end-of-string?...
      {                                                              // ...meaning 'src' had at least word, so there's something to insert.
         U8 bytesToInsert = (U8)(endOfInsert - src + 1);

         if(bytesToInsert > 0)                                       // Bytes to be inserted/appended?
         {
            U8 *p = (U8*)Str_GetNthWord(dest, start);                // Get the insertion point (in 'dest').

            /* If insertion point is not end-of-string then make a gap in 'dest' to fit the
               word to be inserted plus one space.
            */
            if(*p != '\0')                                           // Insertion point is NOT end-of-string? (i.e an append)
            {
               makeGap(p, bytesToInsert+1);                          // then make a gap for words to be inserted.
               p[bytesToInsert] = Str_1stDelimiter();                // and add a delimiter after the last word.
            }
            else                                                     // else insertion point is end-of-string
            {
               p[bytesToInsert] = '\0';                              // so add string terminator to what we append (below).
            }
            memcpy(p, src, bytesToInsert);                           // Insert/append from 'src'.
         }
      }
   }
   return dest;
}

// -------------------------------- eof -----------------------------------------
