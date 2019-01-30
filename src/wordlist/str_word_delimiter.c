/*---------------------------------------------------------------------------
|
|        The inter-word delimiter for space-delimited word lists.
|
|  If Str_Delimiters[] is left NULL OR if Str_Delimiters[] == "" then SPC is
|  the only delimiter.
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
    if( Str_Delimiters != NULL )                // There's a delimiters string? AND...
    {
       if(Str_Delimiters[0] != '\0')            // ...that string is not empty?
       {
         U8 i;
         for(i = 0; i < _MaxDelimiters; i++)    // For no more than '_MaxDelimiters'
         {
            U8 dl = Str_Delimiters[i];          // For each delimiter specified in that string...

            if(dl == '\0')                      // End of list
            {
               return 0;                        // then 'ch' was not found, is not a delimiter.
            }
            else if(ch == dl)                   // 'ch' is in this delimiters list?
            {
               return 1;                        // then yes, 'ch' is delimiter.
            }
         }
         return 0;                              // Found no match for 'ch' and Exceeded length-limit.
       }
    }
    // If no list or empty list then SPC is the default delimiter.
    return ch == ' ' ? 1 : 0;
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


/*-----------------------------------------------------------------------------------------
|
|  Str_StripLeadDelimiters
|
|  Boof past any leading delimiters in 'w'. Return one 1st char of 1st word or end-of-string
|  whichever is first.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 GENERIC * Str_StripLeadDelimiters(U8 GENERIC const *w)
{
   while( Str_Delimiter(*w) ) {w++;}
    // Return 'w' as non-const; what the caller gave may have been non-const or no;
    // the type-modifier will be enforced on the caller's scope.
   return (U8 *)w;
}



// ------------------------------ eof -----------------------------------------
