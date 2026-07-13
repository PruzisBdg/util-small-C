/* --------------------------------------------------------------------------------------------

   Prints and Chain-prints using S_BufC8.
*/
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <string.h>
#include <stdarg.h>

/* ---------------------------------- Print_BufC8 -----------------------------------------------

   snprintf() 'fmt, ...' to 'out'. On entry 'out->cnt' returns the chars-space available in 'out'.

   Returns 'out' with 'out->cnt' the number of chars actually printed.
*/
PUBLIC S_BufC8 const * Print_BufC8(S_BufC8 *out, C8 const *fmt, ...) {
   va_list  ap;
   va_start(ap, fmt);

   /* snprintf() returns the number of chars in the unconstrained output string but we return the number
      of chars actually printed to 'out'.
   */
   out->cnt = MinU16(out->cnt, vsnprintf(out->cs, out->cnt, fmt, ap));
   va_end(ap);
   return out; }


/* ----------------------------------- Chain_BufC8 -------------------------------------------------

   Same as Print_BufC8() except return 'out->cs' advanced past the chars added and with 'out->cnt'
   reduced by the number of chars added.

   Use fmt == NULL to append just a '\0'. Use this to insert a separator for a fresh string after.
*/
PUBLIC S_BufC8 const * Chain_BufC8(S_BufC8 *out, C8 const *fmt, ...) {
   if(fmt == NULL) {
      if(out->cnt > 0) {                  // 'fmt' is NULL? AND there's space to add 1 char?
         out->cs++; *(out->cs) = '\0';    // then append '\0' and cap with another '\0'
         out->cnt--; }}
   else {                                 // else fmt is not NULL. Append it with any args.
      va_list  ap;
      va_start(ap, fmt);

      /* snprintf() returns the number of chars in the unconstrained output string but we return the number
         of chars actually printed to 'out'.
      */
      U16 nChars = MinU16(out->cnt, vsnprintf(out->cs, out->cnt, fmt, ap));
      va_end(ap);
      // Advance past what we just printed; update 'remaining'
      out->cs += nChars; out->cnt -= nChars; }
   return out; }


/* ------------------------------- CpyTail_BufC8 ------------------------------------------

   Copy into 'dest' from start of 'key' in 'src' until the end (of 'src').

   'src->cnt' is the number of chars in 'src->cs'. 'src' is conventionally NULL-terminated, but
   doesn't have to be. CpyTail_BufC8() will look for 'key' up to 'src->cnt' chars or '\0', whichever
   comes first.

   'dest->cnt' is the chars-space available in dest. If there's not enough room in 'dest'  for the
   copy from 'src' then copy is truncated.

   Return 'dest->cnt' the number of chars copied' into dest; '\0'/cnt = 0 if 'key' not found.
*/
PUBLIC S_BufC8 const * CpyTail_BufC8(S_BufC8 *dest, S_BufC8_ro const *src, C8 const *key) {
   C8 const *keyAt = strnstr(src->cs, key, src->cnt);    // Look for 'key' in 'src'.

   if(keyAt != NULL) {                                   // Found 'key'?
      U16 tailChars = src->cnt - (key - src->cs);        // Tail, including 'key' is these many.
      U16 toCpy = MinU16(tailChars, dest->cnt);          // Will copy tail, but no more than will fit in 'dest'.
      strncpy(dest->cs, keyAt, toCpy);                   // 'toCpy' from 'key' into 'dest'
      dest->cnt = toCpy;                                 // 'dest' now has these many chars
      dest->cs[toCpy] = '\0'; }                          // Stringify, in case it is not already.
   else {                                                // else no 'key' in 'src'
      dest->cs[0] = '\0'; dest->cnt = 0; }               // so return just "".
   return dest; }       // This function always returns some string.



/* ------------------------------------ Strcpy_BufC8 --------------------------------------------

   Copy string 'src' into 'dest', up to 'dest->cnt' chars. Return 'dest' with 'cnt' unchanged
*/
PUBLIC S_BufC8 * Strcpy_BufC8(S_BufC8 *dest, C8 const *src) {
   U16 nChars = MinU16(dest->cnt, strlen(src));    // Will copy up what will fit in 'dest'.
   memmove(dest->cs, src, nChars);
   dest->cs[nChars] = '\0';                        // '\0' terminated, whether copied all of 'src' or no.
   return dest; }


/* ------------------------------------- StrChainCpy_BufC8 --------------------------------------------

   Copy string 'src' into 'dest', up to 'dest->cnt' chars. 'dest'->cnt is unmodified.

   Return a copy of 'dest' but with 'cs' advanced to the '\0' of the 'src' copied in and with 'cnt'
   decremented by the number of chars copied in.
*/
PUBLIC S_BufC8 Strcpy_ChainBufC8(S_BufC8 const *dest, C8 const *src) {
   U16 nChars = MinU16(dest->cnt, strlen(src));    // Will copy up what will fit in 'dest'.
   memmove(dest->cs, src, nChars+1);               // Including '\0'.
   dest->cs[nChars] = '\0';                        // '\0' terminated, whether copied all of 'src' or no.

   // Return a new 'dest' starting after 'src' copied in and with chars-free remaining in 'dest'.
   return (S_BufC8){.cs = dest->cs + nChars, .cnt = dest->cnt + nChars}; }




// --------------------------------------------- eof ---------------------------------------------
