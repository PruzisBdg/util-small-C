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
|  Str_Delete()
|
|  Delete 'cnt' words from 'lst' from 'start' onwards.
|
------------------------------------------------------------------------------*/

PUBLIC U8 GENERIC * Str_Delete( U8 GENERIC *lst, U8 start, U8 cnt )
{
   // Note: We can case return from Str_GetNthWord() to non-const because we know that
   // return is 'lst' (which is non-const).
   return strcpy( (C8*)Str_GetNthWord(lst, start), (C8 const *)Str_GetNthWord(lst, start+cnt) );
}

// --------------------------- eof -------------------------------
