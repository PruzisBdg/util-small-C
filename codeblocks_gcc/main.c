#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include <stdio.h>

S_BookScanner scanner;

// A digest for a book where book[0] is length and keep if book[1] > 0.
static T_ReBook const * maybeKeep(U8 const *bk, T_ReBook *dig) {
   dig->len = bk[0];
   dig->keep = bk[1] > 0 ? true : false;
   return dig; }

int main (void)
{
   // 3 Books, different lengths, remove 1st book.
   U8 b0[] = {3,0,10,  2,1, 4,1,5,6};

   S_BufU8 *bs0 = &(S_BufU8){.bs = b0, .cnt = 9};

   scanner.digest = maybeKeep;
   scanner.minLen = 2;           // A book is a least 2 bytes

   S_BufU8 const * rtn = CullPackedBooks(&scanner, bs0);

   printf("--------------- done\r\n");
}


// ------------------------------------- eof ---------------------------------------------
