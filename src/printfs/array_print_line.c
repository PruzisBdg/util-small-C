#include "libs_support.h"
#include "util.h"
#include <string.h>


/*-----------------------------------------------------------------------------------------------

	Print bytes from 'src' into one line in 'out' using 'fmt'

*/
static bool inline isSeperator(C8 ch)
   { return ch == ' ' || ch == ','; }

PUBLIC C8 const * PrintU8s_1Line(C8 *out, U16 outBufLen, C8 const *fmt, U8 const *src, size_t numBytes)
{
	// Print something if 'src' and 'out' exist and there's room for at least 'prefix'.
	if(src != NULL && out != NULL) {
      if(numBytes == 0) {                             // Nothing to print?
         *out = '\0'; }                               // then output empty string.
      else {
         C8 *p = out;

         U16 i; for(i = 0; i < numBytes; i++, src++) {
            if(p - out + 10 > outBufLen)					// Look ahead. is there room to add another digit digit?
               { break; }										// No! then add no more
            p += sprintf(p, fmt, *src); }	   		   // else print just number.

         // Bytes printed; back over any trailing separators.
         while( isSeperator(*(--p)) ) { *p = '\0'; }}
	   }
	return out;
}

/* ---------------------------------------------------------------------------------------------

   Same as above but print bytes reversed (little-endian).
*/
PUBLIC C8 const * PrintU8sReversed_1Line(C8 *out, U16 outBufLen, C8 const *fmt, U8 const *src, size_t numBytes)
{
	// Print something if 'src' and 'out' exist and there's room for at least 'prefix'.
	if(src != NULL && out != NULL) {
      if(numBytes == 0) {                             // Nothing to print?
         *out = '\0'; }                               // then output empty string.
      else {
         C8 *p = out;

         U16 i; for(i = 0, src += (numBytes-1); i < numBytes; i++, src--) {
            if(p - out + 10 > outBufLen)					// Look ahead. is there room to add another digit digit?
               { break; }										// No! then add no more
            p += sprintf(p, fmt, *src); }	   		   // else print just number.

         // Bytes printed; back over any trailing separators.
         while( isSeperator(*(--p)) ) { *p = '\0'; }}
	   }
	return out;
}


PUBLIC C8 const * PrintS16s_1Line(C8 *out, U16 outBufLen, C8 const *fmt, S16 const *src, size_t numBytes)
{
	// Print something if 'src' and 'out' exist and there's room for at least 'prefix'.
	if(src != NULL && out != NULL) {
      if(numBytes == 0) {
         *out = '\0'; }
      else {
         C8 *p = out;

         U16 i; for(i = 0; i < numBytes; i++, src++) {
            if(p - out + 10 > outBufLen)					// Look ahead. is there room to add another digit digit?
               { break; }										// No! then add no more
            p += sprintf(p, fmt, *src); }	   		   // else print just number.

         // Bytes printed; back over any trailing separators.
         while( isSeperator(*(--p)) ) { *p = '\0'; }}
	   }
	return out;
}

// ===================================== eof ==========================================================
