/* This is a 1-way heap i.e a FreeAll() empties the whole heap. There's also a Take()/Return()
   extension which keeps a crude account of space taken but not given back. It's for assembling
   logging text and assumes Take()/Returns() happen tree-wise; as they do with nested printf()s.

   It provides aligned blocks; the number of bytes you request does not have to be a multiple
   of the alignment size but you get back the next multiple up.

   Alignment can be 1,2,4 bytes or a multiple of 4.
*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

// Legal alignments are 1,2,4 bytes or multiple of 4.
 PRIVATE bool legalAlign(Heap1w_T_Size n) { return n == 1 || n == 2 || n % 4 == 0; }

PRIVATE bool legalHeap(Heap1w_S const *h) { return h->mem != NULL && h->size > 0 && legalAlign(h->align); }

// From 'get' to end-of-heap. 'get' should not get ahead of size but, n case it does.
PRIVATE Heap1w_T_Size freeTail(Heap1w_S const *h) { return _AminusB_unsigned(h->size, h->get); }

/* -------------------------------------------------------------------------------------------- */
PRIVATE U32 alignUp(U32 n, Heap1w_T_Size align) {
	return
		n % align == 0						// Already aligned?
			? n								// then no change
			: align * ((n/align) + 1); }	// else round up to alignment.

PRIVATE void * vpAlignUp(void *p, Heap1w_T_Size align)
	{ return (void*)alignUp((U32)p, align); }

/* -------------------------------------------------------------------------------------------- */
PRIVATE U32 alignDown(U32 n, Heap1w_T_Size align) {
	return
		n % align == 0					// Already aligned?
			? n							// then no change
			: align * (n/align); }		// else round up to alignment.

/* -------------------------------------------------------------------------------------------- */
PUBLIC bool Heap1w_Make(Heap1w_S *h, Heap1w_S_Spec const *spec)
{
	if(spec->mem != NULL && legalAlign(spec->align) && spec->size > 0 ) {	// Legal malloc(), legal alignment and non-zero size?
		h->mem = vpAlignUp(spec->mem, spec->align);							// Used memory starts at first aligned mark at or above spec->mem

		if(h->mem < spec->mem + spec->size) {								// Aligned store will hold at least 1 byte?
			h->size = spec->size - (spec->mem - h->mem);					// then mark actual store size, which may be smaller than memory supplied because of alignment.
			h->align = spec->align;											// and set alignment
			h->get = 0;														// 'get' begins at start of heap.
			h->used = 0; h->maxReq = 0;
			return true; }}													// Success!

	// Any fail above; kill the heap; say fail!
	h->mem = NULL; h->get = 0; h->size = 0;
	return false;
}

/* -------------------------------------------------------------------------------------------- */
PUBLIC Heap1w_T_Size Heap1w_BytesFree(Heap1w_S const *h)
	{ return freeTail(h); }

/* -------------------------------------------------------------------------------------------- */
PUBLIC void * Heap1w_BufAt(Heap1w_S const *h)
	{ return h->mem + h->get; }

/* -------------------------------------------------------------------------------------------- */
PUBLIC bool Heap1w_IsInHeap(Heap1w_S const *h, void const *addr)
	{ return h->mem != NULL && addr >= h->mem && addr <= h->mem+h->size; }

/* ------------------------------- Get (1-way) -------------------------------------------------------

   Allocate blocks from 'h' until it's all taken. Heap1w_FreeAll() to clear the whole heap
*/
PUBLIC void * Heap1w_Get(Heap1w_S *h, Heap1w_T_Size n) {
	if( legalHeap(h) ) {									// Legal heap?
		n = alignUp(n, h->align);							// Expand bytes-requested to fill whole number of alignment-blocks.
		if(n > 0 && freeTail(h) >= n) {						// Request > 0 bytes ? AND has at least 'n' more bytes?
			void *p = h->mem + h->get;						// Block we will allocate starts here. 'get' is already aligned, from any previous Take().
			h->get += n;									// Advance 'get' to next aligned free spot.
			h->get = MIN(h->get, h->size);					// If 'get' beyond end of block then set it to end. (being tidy)
			return p; }}									// Return start of allocated block.
	return NULL; }				// Some fail - return NULL.

/* -------------------------------------------------------------------------------------------- */
PUBLIC void Heap1w_FreeAll(Heap1w_S *h)
	{ h->get = 0; h->used = 0; h->maxReq = 0; }

/* ------------------------------- Take & Return -----------------------------------------

   Allocate blocks from 'h', but also 'Return()' space to 'h'. 'h' keeps a 'used' count of the
   total bytes which were Take()en but not Return()ed. The 'get' is allowed to wrap to the
   start of the heap and re-use memory that was previously Take()en provided 'used' shows that
   there's enough left.

   Allocated blocks are not split between the front and end of the heap. So 'used' includes any
   unused tail which happens because a new requested block doesn't exactly fill the remaining
   heap. This is a leak which, after some number of wraps, will shut the heap down. But this heap
   is for assembling string printouts more efficiently and safely than hand-crafted buf[]s, and so
   gets cleaned off when the complete string is printed. Not a problem in practice.
*/
PUBLIC void * Heap1w_Take(Heap1w_S *h, Heap1w_T_Size n) {
	if( legalHeap(h) ) {									// Legal heap?
		n = alignUp(n, h->align);							// Expand bytes-requested to fill whole number of alignment-blocks.
		if(h->used+n > h->maxReq)
			{ h->maxReq = h->used+n; }						// Record the peak space requested. For sizing heaps.
		if(n > 0 && n <= h->size &&							// Requested non-zero bytes which will fit in heap? AND
		   h->used + n < h->size) {							// requested bytes will not overwrite storage which hasn't been released?
			if(freeTail(h) < n) {							// New block won't fit after previous one (in memory)...
				h->used += (n + freeTail(h));				// We took the tail of the heap plus 'n' bytes from start.
				h->get = 0;									// ... allocated from start of heap.
				return h->mem; }
			else {											// else new block will fit after previous one.
				void *p = h->mem + h->get;					// Block we will allocate starts here. 'get' is already aligned, from any previous Take().
				h->get += n;								// Advance 'get' to next aligned free spot.
				h->used += n;								// Used an additional 'n' bytes.
				h->get = MIN(h->get, h->size);				// If 'get' beyond end of block then set it to end. (being tidy)
				return p; }}}								// Return start of allocated block.
	return NULL; }				// Some fail - return NULL.

/* -------------------------------------------------------------------------------------------- */
PUBLIC void Heap1w_Untake(Heap1w_S *h, Heap1w_T_Size n) {
	n = alignDown(n, h->align);
	h->get = _AminusB_unsigned(h->get, n);
	h->used = _AminusB_unsigned(h->used, n); }

/* -------------------------------------------------------------------------------------------- */
PUBLIC void Heap1w_Return(Heap1w_S *h, Heap1w_T_Size numBytes)
	// First bump bytes-to-free to a whole number of alignment blocks.
	{ h->used = _AminusB_unsigned(h->used, alignUp(numBytes, h->align)); }

/* -------------------------------------------------------------------------------------------- */
PUBLIC Heap1w_T_Size Heap1w_Used(Heap1w_S const *h)
	{ return h->used; }

/* -------------------------------------------------------------------------------------------- */
PUBLIC Heap1w_T_Size Heap1w_Unused(Heap1w_S const *h)
	{ return _AminusB_unsigned(h->size, h->used); }

// ======================================== EOF ===================================================
