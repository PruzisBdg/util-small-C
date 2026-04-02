#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <stdio.h>

S_BookScanner scanner;
S_ScanStats stats;

// A digest for a book where book[0] is length and keep if book[1] > 0.
static T_ReBook const * maybeKeep(U8 const *bk, T_ReBook *dig) {
   dig->len = bk[0];
   dig->keep = bk[1] > 0 ? true : false;
   return dig; }

/* ------------------------------ initScan -------------------------------------

*/
static void initScan(S_BookScanner *s, F_GetsDigest d)
{
   s->minLen = 2;          // A book is a least 2bytes
   s->digest = d;
}

/* --------------------------------- prefillStats --------------------------------------

   Prefill with 0x5A5A so we can see which fields were updated.
*/
#define _StatsInit 0x5A5A
static void prefillStats(S_ScanStats *s)
{
   s->errIdx = s->nBooks = s->nKept = _StatsInit;
}

int main (void)
{
   {
      // 2 Books, different lengths, remove 2nd; but 2nd has illegal length (1).
      U8 b0[] = {3,1,10,  1,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);
   }

   {
      // 2 Books, different lengths, remove both; but 2nd has illegal length (1).
      U8 b0[] = {3,0,10,  1,0};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);
   }

   {
      // 2 Books, different lengths, remove 1st book.
      U8 b0[] = {3,0,10,  2,1};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 5};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);
   }

   {
      U8 b0[6] = {3,1,8,  1,9,10};
      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 6};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
      prefillStats(&stats);

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);
   }
   {

      // 5 Books, different lengths, keep all
      U8 b0[] = {3,1,10,  2,1,  4,1,5,6,  5,1,7,8,9,  3,1,11};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 17};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
      prefillStats(&stats);

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);
   }
   {

      // 5 Books, different lengths, cull 2nd & 4th
      U8 b0[] = {3,1,10,  2,0,  4,1,5,6,  5,0,7,8,9,  3,1,11};

      S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 17};

      initScan(&scanner, maybeKeep);   // Set minimum length & digest, Zero counts.
      prefillStats(&stats);

      S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0, &stats);
   }


   printf("--------------- done\r\n");
}


// ------------------------------------- eof ---------------------------------------------
