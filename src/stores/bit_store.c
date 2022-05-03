
/* -----------------------------------------------------------------

  Stores bits in bytes / words.

  Unused bits in the byte / word array are always left at 0 (zero).

  Store 'capacity' may be zero (0); all functions will still return
  sensible results. But... a zero-capacity Store must have at least 1 (U32)
  word at 'mem'.

-------------------------------------------------------------------- */
#include "libs_support.h"
#include "util.h"
#include <string.h>

#define _Wordsize 32
#define _Mask     (_Wordsize - 1)
#define _Shift    5
#define _AllSet   MAX_U32

CASSERT(_Wordsize == _BitStore_WordSize);

// ----------------------------------------------------------------------------
PUBLIC S_BitStore * BitStore_Make(S_BitStore *bs, U32 *mem, U16 nbits) {
   *bs = (S_BitStore){
      .mem = mem,
      .capacity = nbits};
   // Explicitly zero the supplied memory.
   memset(bs->mem, 0, _BitStore_BytesUsed(nbits));
   return bs; }

// ----------------------------------------------------------------------------
PUBLIC U16 bits_StoreCapacity(S_BitStore *bs) {
   return bs->capacity; }

/* -------------------------- bits_Set/ClrBit ----------------------------------

   If 'idx' is outside the range of the Store then do nothing.
*/
PUBLIC void bits_SetBit(S_BitStore *bs, U8 idx) {
   if(idx <= bs->capacity) {
      U8 i = idx >> _Shift;
      bs->mem[i] = bs->mem[i] | (1 << (idx % _Wordsize)); }}

PUBLIC void bits_ClrBit(S_BitStore *bs, U8 idx) {
   if(idx <= bs->capacity) {
      U8 i = idx >> _Shift;
      bs->mem[i] = bs->mem[i] & ~(1 << (idx % _Wordsize)); }}

/* -------------------------- bits_Set/ClrAll ---------------------------------- */

static inline U16 numWords(S_BitStore *bs) { return (bs->capacity + _Wordsize - 1) / _Wordsize; }

PUBLIC void bits_ClrAll(S_BitStore *bs) {
   for(U16 i = 0; i < numWords(bs); i++) { bs->mem[i] = 0; }}

PUBLIC void bits_SetAll(S_BitStore *bs) {
   if(bs->capacity > 0) {
      U16 nw = numWords(bs);
      // Set all bits in all words but the last
      for(U16 i = 0; i < nw-1; i++) { bs->mem[i] = _AllSet; }

      /* In the last word 1 to all (32) bits will be set. Unused bits remain <- 0,
         even for _SetAll(). This so bits_NumSet() works correctly.
      */
      bs->mem[nw-1] = bs->capacity % _Wordsize == 0         // Last word stores (32) bits
                           ? _AllSet                        // then all are set
                           : ((1UL << (bs->capacity % _Wordsize)) - 1); }}  // else 1 to 31 bits are set.

/* -------------------------- bits_BitSet/Clr ------------------------------------

   If 'idx' is outside the Store then return false (for both _BitSet() and _BitClr() ).
*/
PRIVATE inline bool bset(S_BitStore *bs, U8 idx) {
   U8 i = idx >> _Shift;
   return (bs->mem[i] & (1 << (idx % _Wordsize))) > 0 ? true : false; }

PUBLIC bool bits_BitSet(S_BitStore *bs, U8 idx) {
   return
      idx >= bs->capacity
         ? false
         : bset(bs, idx); }

#define _not(b) ((b) == true) ? false : true

PUBLIC bool bits_BitClr(S_BitStore *bs, U8 idx) {
   return
      idx >= bs->capacity
         ? false
         : _not(bset(bs, idx)); }

/* ------------------------- bits_NumSet --------------------------------------

*/
PUBLIC U16 bits_NumSet(S_BitStore *bs) {
   U16 numSet = 0;

   for(U16 i = 0; i < numWords(bs); i++) {
         numSet += SWAR32(bs->mem[i]); }
   return numSet; }

/* --------------------------- bits_AllSet/Clr -----------------------------------

*/
PUBLIC bool bits_AllSet(S_BitStore *bs) {
   return bits_NumSet(bs) >= bs->capacity ? true : false; }

PUBLIC bool bits_AllClr(S_BitStore *bs) {
   return bits_NumSet(bs) == 0 ? true : false; }

// ------------------------------- eof ------------------------------------------
