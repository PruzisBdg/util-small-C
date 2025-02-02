/*---------------------------------------------------------------------------
|
|                    Single-word Operations
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include <ctype.h>
#include "wordlist.h"

// Wrap the ANSI char conversions in a function type which is compatible here.
PRIVATE U8 to_upper(U8 ch) { return toupper(ch); }
PRIVATE U8 to_lower(U8 ch) { return tolower(ch); }


/*-----------------------------------------------------------------------------------------
|
|  convert1stWord()
|
|  Convert the 1st word in str' with 'cvts()' and return in 'out'. The word is delimited
|  by delimiters listed in Str_Delimiter().
|
|  If 'out' == 'str' the conversion is done in-place. Otherwise the converted word is
|  returned alone in 'out', as a string, with leading or trailing chars.
|
|  If there are no words in 'str' (empty or just delimiters) then return 'str' unchanged.
|  Otherwise return the start of the 1st word (which was upper-cased).
|
------------------------------------------------------------------------------------------*/

PRIVATE C8 const *convert1stWord(C8 *out, C8 const *str, U8(*cvts)(U8))
{
   if(out == str)                            // In-place conversion?
   {
      if( (out = (C8*)Str_LTrim((U8 GENERIC const*)str)) != NULL) // There's at least 1 word? (of 1 or more chars)
      {
         while( Str_WordChar(*out) == 1)     // Until end of that 1st word....
         {
            *out = cvts(*out);               // Apply conversion to each char, in-place
            out++;
         }
      }
      return str;
   }
   else                                      // else copy just converted word to 'out'
   {
      C8 *p;
      C8 *q = out;                           // Mark start of 1st word.
      if( (p = (C8*)Str_LTrim((U8 GENERIC const*)str)) != NULL) // There's at least 1 word? (of 1 or more chars)
      {
         while( Str_WordChar(*p) == 1)       // Until end of that 1st word....
         {
            *q = cvts(*p);                   // Apply conversion to each char.
            p++; q++;
         }
      }
      if(q > out)                            // Converted at least 1 char/word? (into 'out').
         { *q = '\0'; }                      // then terminate what we wrote to 'out'
      return out;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  Word_1stToUpper()
|
|  Convert the 1st word in str' to uppercase. See convert1stWord() above.
|
------------------------------------------------------------------------------------------*/


PUBLIC C8 const *Word_1stToUpper(C8 *out, C8 const *str)
   { return convert1stWord(out, str, to_upper); }

/*-----------------------------------------------------------------------------------------
|
|  Word_1stToLower()
|
|  Convert the 1st word in str' to lowercase. See convert1stWord() above.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const *Word_1stToLower(C8 *out, C8 const *str)
   { return convert1stWord(out, str, to_lower); }

/*-----------------------------------------------------------------------------------------
|
|  Word_1stToCamel()
|
|  Convert the 1st word in str' to camelcase. See convert1stWord() above.
|
------------------------------------------------------------------------------------------*/

PRIVATE BIT didLeadCh;
PRIVATE BIT got1Lower;

PRIVATE U8 toCamel(U8 ch)
{
   if(didLeadCh == 0) {             // 1st letter (of 1st word)?
      didLeadCh = 1;
      return toupper(ch);           // then uppercase it.
   }
   else {                           // else 2nd letter onwards.
      if(islower(ch) != 0)          // This letter is (already) lowercase?
         { got1Lower = 1; }         // then mark that we got at least one pre-lowercased letter

      if(got1Lower == 0)            // All uppercase so far?
         { return tolower(ch); }    // then keep converting to lowercase.
      else
         { return ch; }             // else whatever it is, leave it alone.
   }
}

PUBLIC C8 const *Word_1stToCamel(C8 *out, C8 const *str)
{
   didLeadCh = 0; got1Lower = 0;             // Clear camel states.
   return convert1stWord(out, str, toCamel);
}

// =================================== EOF ================================================

