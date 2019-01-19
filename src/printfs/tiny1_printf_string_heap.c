#include "libs_support.h"
#include "tiny1_stdio.h"
#include <stdarg.h>
#include <string.h>
#include "util.h"

extern PUBLIC tiny1_S_SafeSprintf TPrint_Safe;		// If using tiny1_sprintf_safe(), to limit chars added to output buffer.
extern void TPrint_ChIntoBuf_Safe(U8 ch);			// So can be a plain buffer or a stream.

// Your process must supply the heap; which must be just for this prints by the process.
PUBLIC Heap1w_S *TPrint_Heap1W = NULL;		

/*-----------------------------------------------------------------------------------
|
|  tiny1_print_heap1w()
|
|  Prints to (persistent) 'TPrint_Heap1W' and returns the printed string. This call is to assemble a
|  printout from multiple printf()s without having to manually allocate a buffer for each one. Just
|  Heap1w_Free() once the final printout is assembled.
|
|  Why this function? Because declaring multiple and sizing and size checking multiple buffers
|  is just lots of code line, #defs etc which obscure other code. What we want are safe transparent
|  print()s, usually for logging/debug.
|
|  Like tiny1_sprintf_safe(); this is a 'safe' print which can never overrun the heap available
|  for the intermediate and final text.
|
|  Returns the printed text (on the heap). If the text could not be printed returns an error msg.
|
--------------------------------------------------------------------------------------*/

extern T_PrintCnt tprintf_PutCh(U8 ch);

#if 0
PUBLIC C8 const * tiny1_print_heap1w(C8 FMT_QUALIFIER *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	if(TPrint_Heap1W != NULL) {													// Application supplied a heap?
		U16 bytesFree = Heap1w_BytesFree(TPrint_Heap1W);
		if(bytesFree > 1) {														// Room to print at least 1 char?
			C8 *wrTo = Heap1w_BufAt(TPrint_Heap1W);                             // Print here on heap.
			TPrint_Safe.put = wrTo;                                             // TPrint_ChIntoBuf_Safe() will put from 'wrTo'
			TPrint_Safe.cnt = 0;                                                // Counts characters added.
			TPrint_Safe.maxCh = bytesFree-1;                                    // Will add no more than.
			T_PrintCnt cnt = tprintf_internal(TPrint_ChIntoBuf_Safe, fmt, ap);  // Print onto heap.
			tprintf_PutCh('\0');												// Output must be a string, so add terminating '\0'.
			Heap1w_Take(TPrint_Heap1W, cnt+1);                                 // Take from the heap the number of bytes we actually used.
			va_end(ap);
			return wrTo; }}														// Return the string we made (which is on the heap)
		
	va_end(ap);
	return "tiny1_print_heap1w() - out of heap\r\n";      // Some fail.
}
#else
PUBLIC C8 const * tiny1_print_heap1w(C8 FMT_QUALIFIER *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	if(TPrint_Heap1W != NULL) {														// Application supplied a heap?
		/* Up to everything now on the heap right may be used in this new string. Check there's room for all
		   this plus formatter.
		*/
		U16 req = Heap1w_Used(TPrint_Heap1W) + strlen(fmt);			
		if(Heap1w_Unused(TPrint_Heap1W) > req) {									// Should be room for worst-case new string.
			C8 *wrTo;
			if(NULL != (wrTo = Heap1w_Take(TPrint_Heap1W, req))) {					// Got space for worst printout?
				TPrint_Safe.put = wrTo;                                             // TPrint_ChIntoBuf_Safe() will put from 'wrTo'
				TPrint_Safe.cnt = 0;                                                // Counts characters added.
				TPrint_Safe.maxCh = req-1;											// Will add no more than.
				T_PrintCnt cnt = tprintf_internal(TPrint_ChIntoBuf_Safe, fmt, ap);  // Print onto heap.
				cnt += tprintf_PutCh('\0');
				if(cnt < req)														// Used 'cnt+1', including the '\0'. This is less than bytes we requested?...
					{ Heap1w_Untake(TPrint_Heap1W, req-cnt); }						// ...then take back what we didn't use.
				 }																	// Output must be a string, so add terminating '\0'.
			va_end(ap);
			return wrTo; }}															// Return the string we made (which is on the heap)
		
	va_end(ap);
	return "tiny1_print_heap1w() - out of heap\r\n";      // Some fail.
}
#endif

/* --------------------------------------------------------------------------------------------- 

   If TPRINT_USE_STRING_HEAP is #defed, then tprintf_internal() when processng a string (%s) to do it's
   side of any heap accounting.
   
   Note, make sure 'TPrint_Heap1W' exists before attempting to query; tprintf_internal() is called by
   many different process outside the context of any string heap. 
   
   Also, if 'TPrint_Heap1W' exists and from is inside the heap, it may be a heap which doesn't wrap
   and use Take/Return accounting. In which case 'Heap1w_S.used' and Heap1w_Return() has no effect.
*/
extern void TPrint_ReturnToHeap(U8 const *from, U16 numBytes) {
	if(TPrint_Heap1W != NULL) {									// There's any legal 'Heap1w_S' to query?
		if( Heap1w_IsInHeap(TPrint_Heap1W, from) == true)		// 'from' was inside that heap?
			{ Heap1w_Return(TPrint_Heap1W, numBytes); }}}		// then return the 'numBytes' at 'from' to that heap.


// =================================================== EOF ===============================================