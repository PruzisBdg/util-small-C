/* --------------------------------------------------------------------------------------------------

   For a series of Data Records each of which encodes its own length. Like books on a shelf.
*/

#include "libs_support.h"
#include "util.h"
#include <string.h>
#include <stdio.h>



/* ----------------------------------- CullPackedBooks -------------------------------------------------

   Given a shelf of packed books in 'src' and 'pk->digest()' to query the size of a book and whether
   it should be culled, remove zero of more books from the 'shelf', leaving the remaining books packed
   to the 'left', low memory.

   The method goes left-to-right, moving one book at a time i.e it is O(1) for shelf-size. Each book
   contains its own size 'len'. 'pk->digest()' determines whether it should be kept or culled.

   'src->len' is the size of the starting shelf.

   CullPackedBooks() will never read outside the bookshelf i.e it is memory-safe.

   Returns 'src->cnt' the total size of the remaining (packed) books post-cull.

   Returns NULL if:
      - digest() of a 'head' book has following book off the shelf.
      - a digest() returns a size below the minimum allowed for a book.
      - a digest() returns a zero-size book (illegal, the scan cannot progress)

   A NULL return leaves the book & shelves in an undefined state.
*/
PUBLIC S_BufU8 * CullPackedBooks(S_BookScanner const *pk, S_BufU8 *src)
{
   if(pk->minLen == 0 ||                                       // Minmum book length specified as 0 (zero)? OR
      src->cnt < pk->minLen) {                                 // 'src' doesn't hold even 1 book?
      return NULL; }                                           // then Illegal, bail now.
   else {
      // ------- Loop Setup

      // Initial tail. Length must be legal and fit in 'src'
      U8 *tail = src->bs;                                      // 'tail' starts at 1st book
      T_ReBook const *t = pk->digest(tail, &(T_ReBook){});     // digest() of 1st book, because need length

      if(t->len < pk->minLen || t->len > src->cnt) {           // Too short (illegal)? ? OR longer than bookshelf?
         return NULL; }                                        // then fail

      else if(t->len == src->cnt) {                            // Just 1 book?
         if(t->keep == false) {                                // Cull it?
            src->cnt = 0; }                                    // then now 'src' is empty.
         return src; }                                         // and we are done.

      // Setup initial head
      U8 *head = src->bs + t->len;                             // (else) 'head' starts at 2nd book.
      T_ReBook const *h = pk->digest(head, &(T_ReBook){});     // Get digest from 'head' book.

      if(h->len < pk->minLen || h->len + t->len > src->cnt) {
         src->cnt = t->keep == true ? t->len : 0;
         return NULL; }

      /* -------- Loop

         Process books left-to-right (up 'src'), removing any that are flagged for removal by
         copying the next book down over them. Otherwise move the next book left to close any
         gap.

         The gap between the the last moved book and the next grows as books are removed. When all
         books have been addressed they will be packed to left, with gap at right.
      */
      while(1)
      {
         if(t->keep == true) {                                 // Keep the tail book?
            tail += t->len;                                    // then advance 'tail' past that book.

            if(head > tail) {                                  // A gap from prev (tail) book to next (head) one?
               memmove(tail, head, h->len);                    // then move-down head book over the gap.
               head = tail; }}                                 // Adjust 'head' to match the move.
         else {                                                // else overwrite the tail book...
            /* ... then move() down the head book on top of previous one (tail). This head book
               becomes the new tail; the new Head is at 'head + h->len' (below).

               Note it's a move i.e the source block can overlap its destination image.
            */
            memmove(tail, head, h->len);

            // Reduce total books-length by the size of the tail book which was just copied over.
            src->cnt -= t->len; }

         /* Whether we copied over the existing tail book or advanced 'tail' to the next book
            there is a new tail book. Update the tail-digest.
         */
         t = pk->digest(tail, &(T_ReBook){});        // New 'tail' book is...


         // ------- Loop-done checks

         /* If this tail book ends at the current books-width then its the last on
            the shelf. Done, no error! if not keeping the book the adjust src->cnt downwards.
         */
         if(tail + t->len == src->bs + src->cnt) {             // This tail book is the last one?
            if(t->keep == false) {                             // Not keeping this last book
               src->cnt -= t->len; }                           // then adjust 'src->cnt' downwards.
            return src; }                                      // return 'src' => success.

#if 0
         else if(head + h->len == src->bs + src->cnt) {
            if(h->keep == false) {
               src->cnt -= h->len; }
            return src; }
#endif
         /* If head book will go past end of current books counts then width is not legal.
            Subtract it from src->cnt and return fail.
         */
         else if(head + h->len > src->bs + src->cnt) {
            src->cnt = head - src->bs;
            return NULL; }

         // else at least one more book to examine. Advance head.
         else {
            head += h->len;                                    // To right of gap; next bok is adjacent.
            h = pk->digest(head, &(T_ReBook){}); }             // New 'head' book is...

         // If this new head is illegal length (1) then bail now.
         if(h->len < pk->minLen) {
            src->cnt = tail + t->len - src->bs;                // Return count is up to and including the tail book.
            return NULL; }
      } // while(1)

      return NULL; }       // while(1) so should never get here but some compilers complain.

} // CullPackedBooks()

// ---------------------------------------- eof ----------------------------------------------------
