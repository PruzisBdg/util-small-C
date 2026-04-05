/* --------------------------------------------------------------------------------------------------

   For a series of Data Records each of which encodes its own length. Like books on a shelf.
*/

#include "libs_support.h"
#include "util.h"
#include <string.h>
#include <stdio.h>

/* ----------------------------------- bookPack_CullRepack -------------------------------------------------

   Given a shelf of packed books in 'src' and 'pk->digest()' to query the size of a book and whether
   it should be culled, remove zero of more books from the 'shelf', leaving the remaining books packed
   to the 'left', low memory. Accumulate 'stats' on the cull.

   The method goes left-to-right, moving one book at a time i.e it is O(1) for shelf-size. Each book
   contains its own size 'len'. 'pk->digest()' determines whether it should be kept or culled.

   'src->len' is the size of the starting shelf.

   bookPack_CullRepack() will never read outside the bookshelf i.e it is memory-safe.

   Returns 'src->cnt' the total size of the remaining (packed) books post-cull.

   Returns NULL if:
      - digest() of a 'head' book has following book off the shelf.
      - a digest() returns a size below the minimum allowed for a book.
      - a digest() returns a zero-size book (illegal, the scan cannot progress)

   A NULL return leaves the book & shelves in an undefined state.
*/
PUBLIC S_BufU8 * bookPack_CullRepack(bookPack_S_Packer const *pk, S_BufU8 *src, bookPack_S_Stats *stats)
{
   // To update stats.
   void addBook(bookPack_S_Stats *s)           { if(s != NULL) s->nBooks++; }
   void addKept(bookPack_S_Stats *s)           { if(s != NULL) s->nKept++; }
   void wrErrIdx(bookPack_S_Stats *s, U16 n)   { s->errIdx = n; }


   /* -------------------------- getDigest ---------------------------------

      If this 'book' is legal return a digest of the book. Else NULL.
   */
   bookPack_S_Digest const * getDigest(S_BufU8 const *bk, bookPack_S_Digest *dig) {
      if(bk->cnt < pk->minLen) {                         // Book length is illegal? too short.
         return NULL; }                                  // fail <- NULL

      bookPack_S_Digest const *d = pk->digest(bk->bs, dig);       // else ask for a digest.

      if(d == NULL ||                                    // No digest? OR
         (d->len < pk->minLen ||                         // Digest returns illegal length (too short)? OR ...
          d->len > bk->cnt)) {                           // ... Digest returns length which exceeds bookshelf? (this cannot be)
         return NULL; }                                  // then fail <- NULL
      else {
         return dig; }}                                  // else return the digest.


   // --------------------------- advanceBy ----------------------------------

   void advanceBy(S_BufU8 *rd, U16 by) {
      rd->bs += by; rd->cnt -= by; }


   /* ---------------------- firstToKeep -----------------------------------

      Given 'src' at a packed list of 'books' which encode their own lengths, return

         - 'src' at the next legal 'keeper' book with 'd' to a digest of that book. Other wise...

         - if there are no more keeper books on the shelf then return 'src' at the last (legal)
           book. (with digest 'd')

         - return NULL if hit illegal data before a matching book above. Illegal is where
           encoded length is too short (0|1) or too long and is past the bookshelf.

      A keeper book has d->keep <- true.
   */
   bookPack_S_Digest const * firstToKeep(S_BufU8 *src, bookPack_S_Digest *d) {
      while(1){
         if(NULL == getDigest(src, d)) {                    // Get about book?
            return NULL; }                                  // Fail... return fail.
         else {
            addBook(stats);                                 // else add latest book to stats.
            if(d->keep == true || src->cnt == d->len) {     // Keeping this book? AND/OR it's the last book?
               return d; }                                  // then return it.
            else {
               advanceBy(src, d->len); }}}}                 // else advance to next book.


   // ------------------------- Start here ---------------------------------------

   bookPack_S_Digest *bk = &(bookPack_S_Digest){};       // A digest of the latest book.

   // Read-at and bytes-remaining. Starts at 'src->bs[]' with all of 'src->cnt'
   S_BufU8 *rd = &(S_BufU8){.bs = src->bs, .cnt = src->cnt};

   if(NULL == firstToKeep(rd, bk)) {                  // Hit error before any legal book?
      src->cnt = 0;                                   // We kept nothing.!
      wrErrIdx(stats, rd->bs - src->bs);              // Mark last (failed) parse-attempt.
      return NULL; }                                  // and say fail.
   else {
      if(bk->keep == false) {                         // else no error, but nothing to keep either.
         src->cnt = 0;                                // so, again, we got nothing.
         return src; }                                // but return 'src', says parse succeeded
      else {
         addKept(stats);                              // else a Keeper. Bump the cumulative 'stats'.

         if(rd->bs > src->bs) {                       // This 1st Keeper is NOT the 1st book?
            memmove(src->bs, rd->bs, bk->len); }      // then move this 1st keeper down to src->bs.

         U8 * packed = src->bs + bk->len;             // Start 'packed' tail right after 1st packed book.

         /* Advance 'rd' to the next book. Note! this is not necessarily 'tail'; if the
            1st book was from up the shelf, then 'rd' is beyond that.
         */
         advanceBy(rd, bk->len);

         while(rd->cnt > 0)                           // Until ate all books... or hit an error (in loop)
         {
            //printf("------- keep@ %u[%u]\r\n", (U16)(rd->bs-src->bs) - bk->len, bk->len);

            if(NULL == firstToKeep(rd, bk)) {         // Looking for next Keeper, but hit error?
               src->cnt -= rd->cnt;                   // Packed (good) are up to last 'rd'.
               wrErrIdx(stats, rd->bs - src->bs);     // Error is somewhere the failed parse.
               return NULL; }                         // and fail
            else {
               if(bk->keep == false) {                // No next Keeper, all books scanned?
                  break; }                            // Break to return with books packed.
               else {
                  addKept(stats);                     // else another Keeper, increment books-kept.
                  memmove(packed, rd->bs, bk->len);   // Append latest Keeper to 'packed'. Once we have a gap this is always a real move
                  advanceBy(rd, bk->len);             // Advance to next book after the one we just append to 'packed'
                  packed += bk->len; }}               // 'packed' advances to end of book we appended.
         } // while()

         src->cnt = packed - src->bs;        // Books processed without error! these many bytes packed.
         return src; }}                      // and return success!

} // bookPack_CullRepack()

/* -------------------------------- bookPack_InitStats ------------------------------
*/
PUBLIC void bookPack_InitStats(bookPack_S_Stats *s) {
   s->nBooks = s->nKept = s->errIdx = 0; }

/* -------------------------------- bookPack_ChainCullStats -------------------------
*/
PUBLIC S_BufC8 const * bookPack_ChainCullStats(S_BufC8 *out, bookPack_S_Stats const *s) {
   U16 nChars = snprintf(out->cs, out->cnt,
                  "nBook %u, nKept %u, errIdx %u", s->nBooks, s->nKept, s->errIdx);
   out->cs += nChars; out->cnt -= nChars;
   return out; }

// ---------------------------------------- eof ----------------------------------------------------
