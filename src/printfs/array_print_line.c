#include "libs_support.h"
#include "util.h"
#include "arith.h"
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
            p += sprintf(p, fmt, *src); 	   		   // else print just number.

            if( ((i+1) % 8) == 0 ) {                  // For long lists, add a gap every 8
               p += sprintf(p, " "); }}

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

/* ---------------------------------- PrintU8s_MarkDiffs ----------------------------------------------

   Print 2 lines under/over. Mark any differences with '^'
*/
PUBLIC C8 const * PrintU8s_MarkDiffs(C8 *out, U16 outBufLen, C8 const *hdr, C8 const *fmt, U8 const *srcA, size_t aBytes, U8 const *srcB, size_t bBytes)
{
   // ---- Mark differences with carets '^'.
   C8 const * markDiffs(C8 *out, U16 outBufLen, U8 ofs, U8 stride, U8 const *srcA, U8 const *srcB, size_t numBytes)
   {
      // Print something if 'src' and 'out' exist and there's room for at least 'prefix'.
      if(srcA != NULL && srcB != NULL && out != NULL)
      {
         if(numBytes == 0 ||
            numBytes * stride > outBufLen) {
            *out = '\0'; }

         else {
            U16 lineLen = ofs + (numBytes * stride) + numBytes/8;
            memset(out, ' ', lineLen);

            U16 i; for(i = 0; i < numBytes; i++) {
               if(srcA[i] != srcB[i]) {
                  out[ ofs + (i * stride) + (i/8)] = '^'; }}   // 'i/8' adds a gap every 8 to match PrintU8s_1Line().

            out[lineLen] = '\0'; }
      }
      return out;
   }

   // ---- Prints and compare numbers on one line

   #define _BytesPerLine 48

   C8 const * numsAndDiffs_Line(C8 *out, U16 outBufLen, C8 const *hdr, C8 const *fmt, U8 const *srcA, size_t aBytes, U8 const *srcB, size_t bBytes)
   {
      /* Find the length of the printout of one number. They should all print the same size to
         line up and be compared.
      */
      U8 stride = sprintf( (C8[20]){}, fmt, srcA[0]);

      // Find size of leading printout of the array size.
      #define _LenFmt "[%u] "
      U8 ofs = sprintf( (C8[20]){}, _LenFmt, MaxU16(aBytes, bBytes));

      if( (3 * stride * MaxU16(aBytes, bBytes)) + (2 * sizeof("\r\n")) > outBufLen) {
         *out = '\0'; }
      else {
         U16 perLine = outBufLen/3;

         C8 *b0 = malloc( perLine + 2 );
         C8 *b1 = malloc( perLine + 2 );
         C8 *b2 = malloc( perLine + 2 );

         sprintf(out, "%sexpected " _LenFmt "{%s},\r\n%s          %s\r\n%sgot      " _LenFmt "{%s}",
                  hdr,  (U16)aBytes,  PrintU8s_1Line(b0, perLine, fmt, srcA, MinU16(aBytes, _BytesPerLine)),
                  hdr,  markDiffs(b1, perLine, ofs, stride, srcA, srcB, MinU16(MinU16(aBytes, bBytes),_BytesPerLine)  ),
                  hdr,  (U16)bBytes,  PrintU8s_1Line(b2, perLine, fmt, srcB, MinU16(bBytes, _BytesPerLine)));

         free(b0); free(b1); free(b2); }

      return out;
   }

   // ------ Print and compare, breaking into as many lines as necessary.

   U8 stride = sprintf( (C8[20]){}, fmt, srcA[0]);
   U8 blocks = MaxU16(aBytes, bBytes) / _BytesPerLine;

   if( (3 * stride * MaxU16(aBytes, bBytes)) + (((3 * blocks)-1) * sizeof("\r\n")) > outBufLen) {
      *out = '\0'; }
   else {
      while(aBytes > _BytesPerLine || bBytes > _BytesPerLine)
      {
         numsAndDiffs_Line(out, outBufLen, hdr, fmt, srcA, aBytes, srcB, bBytes);

         aBytes = AminusBU16(aBytes, _BytesPerLine);
         srcA += _BytesPerLine;
         bBytes = AminusBU16(bBytes, _BytesPerLine);
         srcB += _BytesPerLine;

         strcat(out, "\r\n");
         out += strlen(out);
      }
      numsAndDiffs_Line(out, outBufLen, hdr, fmt, srcA, aBytes, srcB, bBytes); }
   return out;
}

// ===================================== eof ==========================================================
