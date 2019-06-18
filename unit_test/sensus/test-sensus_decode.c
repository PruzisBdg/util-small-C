#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

/* Harnesses below assume little-endian; they are for X86 so should be true (for now).

*/
#ifdef __BYTE_ORDER__
   #if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
      #error "__BYTE_ORDER__ must be __ORDER_LITTLE_ENDIAN__."
   #endif
#else
   #error "__BYTE_ORDER__ must be defined for this harness."
#endif


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}


// ----------------------------------------- eof --------------------------------------------
