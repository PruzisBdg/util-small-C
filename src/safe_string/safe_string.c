/*---------------------------------------------------------------------------
|
| Bounded strings
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <string.h>
#include <ctype.h>

/*-----------------------------------------------------------------------------------------
|
|
|
|
|
------------------------------------------------------------------------------------------*/

PUBLIC S_Str * str_Make(S_Str *s, C8 *buf, U16 size, BOOL writeable) {
   if(s != NULL) {
      if(buf == NULL) {                         // Did not supply buffer?
         str_Delete(s); }                       // then will return a read-only empty 's'
      else {
         s->buf = buf; s->buf[0] = '\0';        // else attach buf[] & start as ""
         s->maxBytes = size;                    // Max size is this
         s->writeable = writeable == _S_Str_Writeable ? TRUE : FALSE; }}    // Set writable?
   return s; }

PUBLIC void str_Delete(S_Str *s)
    { s->buf = ""; s->maxBytes = 0; s->writeable = FALSE; }

PUBLIC bool str_Write(S_Str *s, C8 const *src, U16 maxBytes) {
   if(s->writeable == TRUE) {
      strncpy(s->buf, src, MinU16(maxBytes, s->maxBytes));
      return true; }
   return false; }

PUBLIC C8 *str_Read(S_Str *s, C8 *out, U16 maxBytes)
    { strncpy(out, s->buf, MinU16(maxBytes, s->maxBytes)); return out; }

PUBLIC C8 str_GetCh(S_Str *s, U16 idx)
    { return s->buf[MinU16(idx, s->maxBytes)]; }

/* --------------------------------- IsPrintableStr_NoLongerThan ------------------------------ */

PUBLIC BOOL IsPrintableStr_NoLongerThan(C8 const *str, U8 maxLen)
{
    U8 c;
    for(c = 0; c <= maxLen; c++)
    {
        if(str[c] == '\0')
            { return TRUE; }
        else if(!isprint((U8)str[c]))
            { return FALSE; }
    }
    return FALSE;
}

/* Copies just printables (including whitespace) and no more than 'maxCh' from 'src' into 'out'.

   Returns number of BYTES copied into 'out', including '\0'. Always returns at least "" ('\0' = 1 byte).
*/
PUBLIC U16 SafeStrCopy(C8 *out, C8 const *src, U16 maxCh) {
	U16 chs; C8 *dest = out;
	for(chs = 0; (isprint((U8)*src) || isspace((U8)*src)) && chs < maxCh; chs++) {
		*dest++ = *src++; }
	*dest = '\0';
	return chs+1; }

/* Checks that 'str' is all printables up to 'maxCh'. Printables includes whitespace.

   Returns 0 if there's a non-printable before 'maxCh'; else number of BYTES in 'str'. (An
   empty string is 1 byte.)
*/
PUBLIC U16 TestStrPrintable(C8 const *str, U16 maxCh) {
	U16 chs;
	for(chs = 0; (isprint((U8)*str) || isspace((U8)*str)) && chs < maxCh; chs++, str++) {}
	return *str != '\0' ? 0 : chs+1; }


/* ------------------------------------ PutStr_Safe -------------------------------------------

   Copy string 'src' into 'put', but only if it will fit. 'put' is advanced to the 1st free
   byte beyond the copy.

   If 'str' is empty then put just a '\0' at dest->cs. So if 'dest->cs' was not originally
   '\0' terminated it is so now.

   Return the number of bytes put; 0 if there was no room to add.
*/
PUBLIC U16 PutStr_Safe(S_BufC8 *put, C8 const *str) {
   if(put->cnt >= 1 && put->cs != NULL) {        // 'put' has a buffer with at least 1 byte free?
      U16 len = strlen(str);
      if(put->cnt >= len + 1) {                   // 'src' fits in 'put'? - including the '\0'
         strcpy(put->cs, str);                    // then 'put' <- 'src'
         put->cs += len; put->cnt -= len;         // advance past copied; adjust free 'cnt'.
         return len;}}                            // Return number of chars put.
   return 0; }          // No put; return 0.


/* -------------------------------------- PutStrs_Safe -------------------------------------------

   Append 'cnt' 'strs' into 'put' in order, provided they will all fit. 'put' is advanced to
   the 1st free byte beyond the appends.

   Return the total number of chars added. (not including '\0').

   If all 'strs' will NOT fit then return put with at least the 1st byte <- '\0' AND with 'cnt'
   unmodified. Bytes beyond the '1st' may have been modified - i.e tried the copy.
*/
PUBLIC U16 PutStrs_Safe(S_BufC8 *put, C8 const * const *strs, U8 cnt) {
   if(put->cnt >= 1 && put->cs != NULL) {          // 'put' has a buffer with at least 1 byte free?
      S_BufC8 d0 = *put;                           // Mark initial 'put' in case we must back out.
      U16 nChars = 0;
      for(U8 i = 0; i < cnt; i++)                  // for each 'strs'
      {
         U16 len = strlen(strs[i]);
         if(put->cnt < len + 1) {                  // strs[i] will not fit in 'put'?
            *put = d0;                             // then back out of all appends i.e restore original 'put'
            put->cs[0] = '\0';                     // ... '\0' (so it is "")
            break; }                               // ... and we are done.
         else {                                    // else string fits in 'put'? - including the '\0'
            strcpy(put->cs, strs[i]);              // then 'put' <- 'src'
            put->cs += len; put->cnt -= len;       // advance past copied; adjust free 'cnt'.
            nChars += len; }                       // Update chars-count.
      }
      return nChars; }        // Return total chars added.
   return 0; }             //No chars added; return 0.

// --------------------- eof --------------------------------  -
