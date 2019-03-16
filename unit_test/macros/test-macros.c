#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* -------------------------------------- test_NumArgs ------------------------------------------- */

void test_NumArgs(void)
{
   // _NumArgs()   counts up to 32.
   TEST_ASSERT_EQUAL_INT( 1,  _NumArgs(A) );
   TEST_ASSERT_EQUAL_INT( 2,  _NumArgs(A,B) );
   TEST_ASSERT_EQUAL_INT( 3,  _NumArgs(A,B,C) );
   TEST_ASSERT_EQUAL_INT( 4,  _NumArgs(A,B,C,D) );
   TEST_ASSERT_EQUAL_INT( 5,  _NumArgs(A,B,C,D,E) );
   TEST_ASSERT_EQUAL_INT( 6,  _NumArgs(A,B,C,D,E,F) );
   TEST_ASSERT_EQUAL_INT( 7,  _NumArgs(A,B,C,D,E,F,G) );
   TEST_ASSERT_EQUAL_INT( 8,  _NumArgs(A,B,C,D,E,F,G,H) );
   TEST_ASSERT_EQUAL_INT( 9,  _NumArgs(A,B,C,D,E,F,G,H,I) );
   TEST_ASSERT_EQUAL_INT( 10,  _NumArgs(A,B,C,D,E,F,G,H,I,J) );
   TEST_ASSERT_EQUAL_INT( 11, _NumArgs(A,B,C,D,E,F,G,H,I,J,K) );
   TEST_ASSERT_EQUAL_INT( 12, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L) );
   TEST_ASSERT_EQUAL_INT( 13, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M) );
   TEST_ASSERT_EQUAL_INT( 14, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N) );
   TEST_ASSERT_EQUAL_INT( 15, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O) );
   TEST_ASSERT_EQUAL_INT( 16, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P) );
   TEST_ASSERT_EQUAL_INT( 17, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q) );
   TEST_ASSERT_EQUAL_INT( 18, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R) );
   TEST_ASSERT_EQUAL_INT( 19, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S) );
   TEST_ASSERT_EQUAL_INT( 20, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T) );
   TEST_ASSERT_EQUAL_INT( 21, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U) );
   TEST_ASSERT_EQUAL_INT( 22, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V) );
   TEST_ASSERT_EQUAL_INT( 23, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W) );
   TEST_ASSERT_EQUAL_INT( 24, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X) );
   TEST_ASSERT_EQUAL_INT( 25, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y) );
   TEST_ASSERT_EQUAL_INT( 26, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z) );
   TEST_ASSERT_EQUAL_INT( 27, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,1) );
   TEST_ASSERT_EQUAL_INT( 28, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,1,2) );
   TEST_ASSERT_EQUAL_INT( 29, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,1,2,3) );
   TEST_ASSERT_EQUAL_INT( 30, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,1,2,3,4) );
   TEST_ASSERT_EQUAL_INT( 31, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,1,2,3,4,5) );
   TEST_ASSERT_EQUAL_INT( 32, _NumArgs(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,1,2,3,4,5,6) );

   // C99 counts empty args.
   TEST_ASSERT_EQUAL_INT( 32, _NumArgs(,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,) );

   // Empty argument list returns 1, for now. (the 'trailing comma' problem)
   TEST_ASSERT_EQUAL_INT( 1,  _NumArgs() );
}


/* -------------------------------------- test_Params ------------------------------------------- */

void test_NumParms(void)
{
   TEST_ASSERT_EQUAL_INT(0, _NumParms() );
   TEST_ASSERT_EQUAL_INT(3, _NumParms(1,22,3.14) );

   int a;
   float b;
   double c;
   TEST_ASSERT_EQUAL_INT(3, _NumParms(a,b,c) );

   // Anything that can be cast to an 'int' - but must  kill those pesky conversion warnings.
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wint-conversion"
   void *d;
   C8 const e[] = "E";
   U8 *f;
   TEST_ASSERT_EQUAL_INT(3, _NumParms(d,e,f) );

   // Strings / arrays anon declarations.
   TEST_ASSERT_EQUAL_INT(5, _NumParms( (C8[]){"Paddy"}, (C8[]){"is"}, (C8[]){"my"}, (C8[]){"name"}, (U8[]){1,2,3,4} ) );
   #pragma GCC diagnostic pop
}

// ----------------------------------------- eof --------------------------------------------
