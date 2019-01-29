/*---------------------------------------------------------------------------
|
|        The inter-word delimiter for space-delimited word lists.
|
|  If Str_Delimiters[] is left NULL then SPC is the only delimiter.
|
|--------------------------------------------------------------------------*/

#include "common.h"
#include "wordlist.h"

PUBLIC BIT Str_DiceComposites = 0;
PUBLIC C8 const * Str_Delimiters = NULL;

// For safety in Str_Delimiters[] allow no more than 20 different delimiters.
#define _MaxDelimiters 20

/*-----------------------------------------------------------------------------------------
|
|  chIsDelimiter
|
|  Return 1 if 'ch' is a delimiter.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT chIsDelimiter(U8 ch)
{
    if( Str_Delimiters == NULL )
    {
        return ch == ' ' ? 1 : 0;
    }
    else
    {
        U8 i;
        for(i = 0; i < _MaxDelimiters; i++)
        {
            U8 dl = Str_Delimiters[i];

            if(dl == '\0')
            {
                return 0;
            }
            else if(ch == dl)
            {
                return 1;
            }
        }
        return 0;
    }
}

/*-----------------------------------------------------------------------------------------
|
|  wordChar()
|
|  Used by Str_ routines (wordlist). Returns 1 if char is not a word delimiter or
|  end of word list.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT wordChar(U8 ch)
{
   if( ch == '\0' || Str_Delimiter(ch) ) return 0; else return 1;
}

/*-----------------------------------------------------------------------------------------
|
|  Str_Delimiter
|
|  Return 1 if 'ch' is a delimiter, else 0.
|
------------------------------------------------------------------------------------------*/
PUBLIC BIT Str_Delimiter(U8 ch)
{
   if( chIsDelimiter(ch) || (Str_DiceComposites && ch == '.') )
      { return 1; }
   else
      { return 0; }
}

/*-----------------------------------------------------------------------------------------
|
|  Str_1stDelimiter
|
|  Return the 1st delimiter in Str_Delimiters[], SPC ' ' is Str_Delimiters is NULL.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 Str_1stDelimiter(void)
{
   if( Str_Delimiters == NULL )              // Not defined?
   {
      return ' ';                            // then default to SPC.
   }
   else
   {
      if(Str_Delimiters[0] == '\0')          // else define dbut empty?
      {
         return ' ';                         // then, again, default to SPC.
      }
      else
      {
         return Str_Delimiters[0];           // else return 1st delimiter in list.
      }
   }
}


// ------------------------------ eof -----------------------------------------
