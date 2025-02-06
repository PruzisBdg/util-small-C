/*---------------------------------------------------------------------------
|
|                    Space-delimited Word lists
|
|  These handle strings of space-delimited words. Words are one or more of anything
|  except Spc,Tab CR,LF or '\0'. Words are sepatated by one or more spaces, CR or
|  LF (NOT tabs at present, but might add those).
|
|--------------------------------------------------------------------------*/

#ifndef WORDLIST_H
#define WORDLIST_H

#include "libs_support.h"

PUBLIC U8           Str_WordCnt(     C8 GENERIC const *str );
PUBLIC U8           Str_FindWord(    C8 GENERIC const *lst, C8 GENERIC const *str);
PUBLIC U8           Str_FindWord_CC( C8 CONST *lst, C8 CONST *str );
PUBLIC C8 GENERIC const * Str_TailAfterWord( C8 GENERIC const *lst, C8 GENERIC const *str );
PUBLIC C8 GENERIC * Str_GetNthWord(  C8 GENERIC const *lst, U8 n );
PUBLIC U8           Str_CopyNthWord( C8 GENERIC const *lst, C8 *out, U8 n, U8 maxChars );
PUBLIC BIT          Str_WordInStr(   C8 GENERIC const *str, C8 GENERIC const *theWord );
PUBLIC U8           Str_WordsInStr(  C8 GENERIC const *str, C8 GENERIC const *wordsToMatch );
PUBLIC BIT          Str_1stWordsMatch( C8 GENERIC const *w1, C8 GENERIC const *w2 );
PUBLIC U8           Str_WordCharCnt( C8 GENERIC const *w );
PUBLIC C8 GENERIC * Str_GetEndWord( C8 GENERIC const *lst, U8 n );
PUBLIC C8 GENERIC * Str_Delete( C8 GENERIC *lst, U8 start, U8 cnt );
PUBLIC C8 GENERIC * Str_Insert( C8 GENERIC *dest, C8 GENERIC const *src, U8 start, U8 cnt );
PUBLIC C8 GENERIC * Str_Replace( C8 GENERIC *str, C8 GENERIC const *toFind, C8 GENERIC const * replaceWith, U8 numToCut, U8 numToInsert );

PUBLIC BIT          Str_Delimiter(U8 ch);          // Word are divied by CRLF or ' '
PUBLIC BIT          Str_WordChar(U8 ch);
PUBLIC BIT          Str_EndOfLineOrString(U8 ch);
PUBLIC U8           Str_1stWordHasChar( C8 GENERIC const *lst, U8 ch );

#define _Str_NoMatch 0xFF      // No match found in Str_FindWord()

/* This bit is set by GetObj(). It forces Sre_FindWord and Str_Delimiter() to treat
   '.' the same as a Spc, as a word delimiter. This allows GetObj() to retrieve the
   object name from constructions like 'MyVec.min' ('MyVec' is the object, 'min' is
   the qualifier).
*/
extern BIT Str_DiceComposites;

/* This may be a string list of delimiters e.g
     " \\t\\r\\n"
   Str_Delimiters is initialised to NULL. If it's left/set NULL then SPC is the only delimiter.
*/
extern C8 const * Str_Delimiters;
PUBLIC U8 Str_1stDelimiter(void);
PUBLIC C8 GENERIC * Str_LTrim(C8 GENERIC const *w);

// Convert 1st word in a string (using Str_Delimiters to define the delimiters).
PUBLIC C8 const *Word_1stToUpper(C8 *out, C8 const *str);
PUBLIC C8 const *Word_1stToLower(C8 *out, C8 const *str);
PUBLIC C8 const *Word_1stToCamel(C8 *out, C8 const *str);

#endif // WORDLIST_H
