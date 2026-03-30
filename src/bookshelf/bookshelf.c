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
      return NULL; }                                           // then bail now.
   else {
      U8 *tail = src->bs;                                            // 'tail' starts at 1st book
      T_ReBook const *t = pk->digest(tail, &(T_ReBook){});

      if(t->len + pk->minLen > src->cnt) {
         return NULL; }

      U8 *head = src->bs + t->len;                                   // 'head' starts at 2nd book.
      T_ReBook const *h = pk->digest(head, &(T_ReBook){});           // Get digest from 'head' book.

      if(h->len + pk->minLen > src->cnt) {
         return NULL; }

      /* Process books left-to-right (up 'src'), removing any that are flagged for removal by
         copying the next book down over them. Otherwise move the next book left to close any
         gap.

         The gap between the the last moved book and the next grows as books are removed. When all
         books have been addressed they will be packed to left, with gap at right.
      */
      //while(head + h->len <= src->bs + src->cnt)                    // While 'head' book does NOT extend past 'src[cnt]'
      while(1)
      {
         if(t->keep == true) {                                       // Keep the tail book?
            tail += t->len;                                          // then advance 'tail' past that book.

            if(head > tail) {                                        // A gap from prev (tail) book to next (head) one?
               memcpy(tail, head, h->len);}}                         // then copy-down head book over the gap.
         else {                                                      // else overwrite the tail book...
            memcpy(tail, head, h->len);                              // ... copy down the head book on top of it.

            // Reduce total books-length by the size of the tail book which was just copied over.
            src->cnt -= t->len;

            t = pk->digest(tail, &(T_ReBook){});        // New 'tail' book is...

            if(tail + t->len == src->bs + src->cnt) {
               return src; }
            }

         /* Whether we copied over the existing tail book or advanced 'tail' to the next book
            there is a new tail book. Update the tail-digest.
         */
         t = pk->digest(tail, &(T_ReBook){});        // New 'tail' book is...

         /* Whatever we did above, advance 'head' to the next book.
         */
         if(head + h->len == src->bs + src->cnt) {
            if(h->keep == false) {
               src->cnt -= h->len; }
            return src; }
         else if(head + h->len > src->bs + src->cnt) {
printf("-------- here\r\n");
            return NULL; }
         else {
            head += h->len;
            h = pk->digest(head, &(T_ReBook){}); }       // New 'head' book is...

         // If this book is too small then bail now.
         if(h->len < pk->minLen) {
            return NULL; }
      } // while( ...)

      return NULL; }

      /* If the last book went past the total books width then then somewhere there was
         a bogus length. The return NULL indication failure. Else all-good; return 'src'
      */
//      return head + h->len != src->bs + src->cnt
//               ? NULL : src; }
} // CullPackedBooks()

// ---------------------------------------- eof ----------------------------------------------------
