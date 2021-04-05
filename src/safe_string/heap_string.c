/* Protected strings from heap.
*/
#include "libs_support.h"
#include "string.h"
#include "util.h"
#include "arith.h"

// ----------------------------------------------------------------------------------------
PUBLIC S_HeapStr * heapStr_New(U16 len) {
   return safeRef_New(len+1); }

// ----------------------------------------------------------------------------------------
PUBLIC S_HeapStr * heapStr_Renew(S_HeapStr **h, U16 len) {
   return safeRef_Renew(h, len+1); }

// ----------------------------------------------------------------------------------------
PUBLIC bool heapStr_Delete(S_HeapStr **h) {
   return safeRef_Delete(h); }

// ----------------------------------------------------------------------------------------
PUBLIC bool heapStr_Write(S_HeapStr *s, C8 const *src) {
   U16 len = strlen(src);
   U16 maxStr = safeRef_Size(s)-1;                 // largest string which will fit is...

   if(len <= maxStr) {                             // 'src' fits in 's'
      return safeRef_Write(s, src, len+1); }       // then write all of it.
   else {                                          // else 'src' too large for 's'
      // So make a truncated copy of 'src'.
      C8 * p = malloc(maxStr+1);
      memcpy(p, src, maxStr);
      p[maxStr] = '\0';
      // and fill 's' with that truncated copy.
      bool rtn = safeRef_Write(s, p, maxStr+1);
      free(p);
      return rtn; }}

// ----------------------------------------------------------------------------------------
PUBLIC C8 const * heapStr_Read( S_HeapStr *s, C8 *out, U16 charsReq) {
   if(true == safeRef_Read(s, out, MinU16(safeRef_Used(s), charsReq))) {
      out[charsReq] = '\0'; }
   else {
      out[0] = '\0'; }     // No read; will return empty string.
   return out; }

// ----------------------------------------------------------------------------------------
PUBLIC C8 const * heapStr_Take( S_HeapStr **h, C8 *out, U16 charsReq) {
   C8 const *rtn = heapStr_Read(*h, out, charsReq);      // Copy to 'out'
   safeRef_Delete(h);                                    // then delete source.
   return rtn; }

// ----------------------------------------------------------------------------------------
PUBLIC C8 const * heapStr_Ref(S_HeapStr *s) {
   return s->ref; }

// ----------------------------------------------------------------------------------------
PUBLIC bool heapStr_Append(S_HeapStr *s, C8 const *src) {
   U16 len = strlen(src);

   if(len == 0) {                                                    // 'src' is empty string?
      return true; }                                                 // then nothing to add; succeed at that.
   else if(true == safeRef_WriteAt(s,                                // else append the string...
                        s->used == 0 ? 0 : s->used-1,                // ...if empty then it's not an append; just write at start of Store.
                           src, len+1)) {
      ((U8*)(s->ref))[s->size-1] = '\0';                             // In case there wasn't room for all of it, write '\0' at end of store.
      return true; }                                                 // Success!
   return false;           // safeRef_WriteAt() failed.

}

// ----------------------------------------- eof --------------------------------------------------
