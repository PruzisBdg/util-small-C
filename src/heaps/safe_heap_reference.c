/* Protected calloc() and free().

   Avoids hard faults from double calls on malloc() or free(p) OR calls on free(p)
   where 'p' is not actually on the heap.
*/
#include "libs_support.h"
#include "string.h"
#include "util.h"
#include "arith.h"

   #if _TARGET_IS == _TARGET_UNITY_TDD
extern void * heapStr_TDDMalloc(size_t n);
extern void heapStr_TDDFree(void *p);

#define _malloc(n) heapStr_TDDMalloc((n))
#define _free(p)   heapStr_TDDFree((p))
   #else
#define _malloc malloc
#define _free free
   #endif // _TARGET_IS

// A safe landing spot, as far a possible, for when we can't get from heap.
PRIVATE U32 aSafeNothing = 0;
PRIVATE void *toSafeNothing = &aSafeNothing;
PRIVATE S_SafeRef nullSafeRef = (S_SafeRef){.ref = NULL, .size = 0, .used = 0, .tag = 0};

#define _MagicTag 0x67F2


/* ------------------------------ safeRef_New ----------------------------------------

   Note: It's OK to make a zero-sized Ref. Can't Store anything in it.
*/
PUBLIC S_SafeRef * safeRef_New(U16 nBytes) {

   S_SafeRef *s = (S_SafeRef*)_malloc(sizeof(S_SafeRef));      // Get a reference

   if(s != NULL) {                           // Got reference?
      void *p = _malloc(nBytes);             // Get storage
      if(p == NULL) {                        // Got storage? NO
         _free(s); }                         // then free up the reference; won't be using it.
      else {
         s->ref = p;                         // else attach storage to reference
         s->size = nBytes;                   // Store size is this.
         s->used = 0;                        // and starts empty.
         memset(p, 0, nBytes);               // Store starts cleaed (like a calloc())
         s->tag = _MagicTag; }               // and give it the secret tag.
      return s; }                            // Success; return the new reference (and it's storage).

   // Else a _malloc() fail above; return a safe landing spot; best we can do.
   nullSafeRef.ref = toSafeNothing;
   return &nullSafeRef;
}

/* ------------------------------ safeRef_Renew ----------------------------------------

   Make 'sf' new/anew if necessary.
*/
PUBLIC S_SafeRef * safeRef_Renew(S_SafeRef **h, U16 nBytes) {
   S_SafeRef *rtn;
   if(h == NULL || *h == NULL || (*h)->tag != _MagicTag) {  // NULL handle, NULL reference OR No Tag?
      rtn = safeRef_New(nBytes);                            // then mint new.
      *h = rtn; }                                           // and give 'h' the new ref.
   else if((*h)->size != nBytes) {                          // Size changed?
      safeRef_Delete(h);                                    // then delete...
      rtn = safeRef_New(nBytes);                            // ...make new...
      *h = rtn; }                                           // ...and give 'h' the nre ref.
   else {
      return *h; }                                          // else already exists; return as-is.
   return rtn;
}

/* --------------------------------- safeRef_Delete ---------------------------------------

   Remove 'sf' from heap. Leave it pointing to a safe spot.
*/
PUBLIC bool safeRef_Delete(S_SafeRef **h) {
   if(h != NULL && *h != NULL) {          // Both 'h' (handle) and it's destination exist? (&& is a Sequence Point, so can check both here).
      S_SafeRef *s = *h;                  // Safe to use '*h'
      if(s->tag == _MagicTag) {           // Tag is visible? so 's' exists on heap.
         _free(s->ref);                   // then first remove storage...
         _free(s); }}                     // ...and remove 's' itsef.

   // Original ref is gone; switch '*h' to safe landing spot.
   nullSafeRef.ref = toSafeNothing;
   *h = &nullSafeRef;
   return true;            // Always succeeds, for now.
}

/* --------------------------------- safeRef_Write ---------------------------------------

   Write 'nBytes' from 'src' to 'sf'.

   Return false if 'sf' doesn't exist on the heap OR if 'sf' will not hold 'nBytes'.
*/
PUBLIC bool safeRef_Write(S_SafeRef *sf, U8 const *src, U16 srcbytes) {
   if(sf->tag == _MagicTag) {                // 'sf' exists?
      if(sf->size >= srcbytes) {             // 'sf' will hold all of 'src'?
         memcpy(sf->ref, src, srcbytes);     // Copy-in 'src'
         sf->used = srcbytes;                // 'sf' now has this much data.
         return true; }}                     // and Success!
   return false;     // No 'sf' OR 'sf' was too small for 'src'.
}

/* --------------------------------- safeRef_WriteAt ---------------------------------------

   Write 'srcBytes' from 'src' to 'sf' at 'ofs'. If 'nBytes' won't fit in 'df' then write as
   many as will fit.

   'ofs' may overlap existing data in 'sf'. If so then that data is is overwritten.

   Return false if 'sf' doesn't exist on the heap.
*/
PUBLIC bool safeRef_WriteAt(S_SafeRef *sf, U16 ofs, U8 const *src, U16 srcbytes) {
   if(sf->tag == _MagicTag) {                                        // 'sf' exists?
      U16 toWrite = MinU16(srcbytes, AminusBU16(sf->size, ofs));     // Write as many bytes will fit; up to 'srcBytes'.
      memcpy( &((U8*)(sf->ref))[ofs], src, toWrite);                 // Copy-in these at [ofs]

      /* Update tally of bytes in store. If 'ofs'/'toWrite' lies within existing data ('used')
         the the count doesn't change.
      */
      sf->used = MaxU16(sf->used, ofs + toWrite);                    // Update tally of bytes in Store
      return true; }                                                 // Success.
   return false;        // 'sf' was not a Store.
}

/* --------------------------------- safeRef_Append ---------------------------------------

   Appand 'srcBytes' from 'src' to 'sf'.

   Return false if 'sf' doesn't exist or not enough room to add 'srcBytes'.
*/
PUBLIC bool safeRef_Append(S_SafeRef *sf, U8 const *src, U16 srcbytes) {
   return safeRef_WriteAt(sf, sf->used, src, srcbytes); }

/* --------------------------------- safeRef_Read ---------------------------------------

   Read into 'dest' 'nBytes', or up to the size of 'sf', from 'sf'.

   Return false if 'sf' doesn't exist (on the heap).
*/
PUBLIC bool safeRef_Read(S_SafeRef *sf, U8 *dest, U16 nBytes) {
   if(sf->tag == _MagicTag) {
      memcpy(dest, sf->ref, MinU16(nBytes, sf->size));
      return true; }
   return false;
}

// -----------------------------------------------------------------------------------------
PUBLIC U16 safeRef_Size(S_SafeRef *sf) {
   return sf->size; }

// -----------------------------------------------------------------------------------------
PUBLIC U16 safeRef_Used(S_SafeRef *sf) {
   return sf->used; }

// -----------------------------------------------------------------------------------------
PUBLIC U16 safeRef_Free(S_SafeRef *sf) {
   return AminusBU16(sf->size, sf->used); }

// -------------------------------------------- eof ---------------------------------------------------
