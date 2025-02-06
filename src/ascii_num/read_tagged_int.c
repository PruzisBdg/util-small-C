/*---------------------------------------------------------------------------
|
| Find and read <tag> <int>
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>      // isdigit()
#include "arith.h"
#include "wordlist.h"

/*-----------------------------------------------------------------------------------------
|
|  ReadTaggedASCIIInt()
|
|  Return from 'in' into 'out' the first number after 'tag', The number must be a signed
|  32bit integer e.g 234, -234 or it can be hex '0xABC123'. If hex, it must be <= 0x7FFFFFFF.
|
|  This parser uses ReadDirtyASCIIInt() i.e it takes the largest sequence which completes
|  an integer (maximal match). The number may be bounded by any non-numeric chars, whitespace
|  or non-printing chars. The number may not be split. If a digit is directly preceded by '-',
|  the number is read as negative.
|
|  Returns pointer to the first char after the last byte read, NULL if did not find the 'tag'
|  or a number after it.
|
|  Note: If a number is NOT parsed '*out' is undefined.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const * ReadTaggedASCIIInt(C8 const *in, C8 const *tag, S32 *out, C8 const *delimiters)
{
   if(delimiters != NULL) {                              // Supplied delimiters?
      Str_Delimiters = delimiters; }                     // then use them.

   C8 const *tail = Str_TailAfterWord(in, tag); // Goto after 'tag', if it's there.

   if( tail != NULL && *tail != '\0' ) {                 // Got 'tag' and there's content beyond it?
      T_FloatOrInt fi = { .reqFloat = false };           // Number will be returned here, as an Int.

      if(NULL != (tail = ReadASCIIToNum(tail, &fi))) {   // Got a number after tag?
         if(fi.gotInt == true) {                         // That number is an integer?
            if(fi.gotUnsigned == true) {                 // Unsigned? ...
               if(fi.num.asU32 <= MAX_S32) {             // then ensure it fits in S32?
                  *out = (S32)fi.num.asU32;              // Yes.. convert it.
                  return tail; }}
            else {                                       // else is signed.
               *out = fi.num.asS32;                      // then return the integer
               return tail; }}}}                         // and return the tail beyond the number.
   return NULL;                                          // 'tag' not found OR no integer after tag.
}

// ----------------------------------- eof ---------------------------------------
