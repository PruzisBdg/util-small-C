/*---------------------------------------------------------------------------
|
| Bounded strings
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
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
      if(s->buf == NULL) {                      // Did not supply buffer?
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
        else if(!isprint(str[c]))
            { return FALSE; }
    }
    return FALSE;
}

/* Copies just printables (including whitespace) and no more than 'maxCh' from 'src' into 'out'.

   Returns number of BYTES copied into 'out', including '\0'. Always returns at least "" ('\0' = 1 byte).
*/
PUBLIC U16 SafeStrCopy(C8 *out, C8 const *src, U16 maxCh) {
	U16 chs; C8 *dest = out;
	for(chs = 0; (isprint(*src) || isspace(*src)) && chs < maxCh; chs++) {
		*dest++ = *src++; }
	*dest = '\0';
	return chs+1; }

/* Checks that 'str' is all printables up to 'maxCh'. Printables includes whitespace.

   Returns 0 if there's a non-printable before 'maxCh'; else number of BYTES in 'str'. (An
   empty string is 1 byte.)
*/
PUBLIC U16 TestStrPrintable(C8 const *str, U16 maxCh) {
	U16 chs;
	for(chs = 0; (isprint(*str) || isspace(*str)) && chs < maxCh; chs++, str++) {}
	return *str != '\0' ? 0 : chs+1; }


// --------------------- eof --------------------------------  -
