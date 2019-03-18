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

/* ------------------------------- test_Infix --------------------------------------------- */

void test_Infix(void)
{
   #define _SeqSum(n)  (((n) * ((n) + 1)) / 2)

   // Supports 26 parms, for now (a-z).
   TEST_ASSERT_EQUAL_INT(_SeqSum(2),   _Infix(+,1,2));         // 1+2
   TEST_ASSERT_EQUAL_INT(_SeqSum(3),   _Infix(+,1,2,3));          // 1+2+3
   TEST_ASSERT_EQUAL_INT(_SeqSum(4),   _Infix(+,1,2,3,4));           // 1+2+3+4
   TEST_ASSERT_EQUAL_INT(_SeqSum(5),   _Infix(+,1,2,3,4,5));
   TEST_ASSERT_EQUAL_INT(_SeqSum(6),   _Infix(+,1,2,3,4,5,6));
   TEST_ASSERT_EQUAL_INT(_SeqSum(7),   _Infix(+,1,2,3,4,5,6,7));
   TEST_ASSERT_EQUAL_INT(_SeqSum(8),   _Infix(+,1,2,3,4,5,6,7,8));
   TEST_ASSERT_EQUAL_INT(_SeqSum(9),   _Infix(+,1,2,3,4,5,6,7,8,9));
   TEST_ASSERT_EQUAL_INT(_SeqSum(10),  _Infix(+,1,2,3,4,5,6,7,8,9,10));
   TEST_ASSERT_EQUAL_INT(_SeqSum(11),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11));
   TEST_ASSERT_EQUAL_INT(_SeqSum(12),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12));
   TEST_ASSERT_EQUAL_INT(_SeqSum(13),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13));
   TEST_ASSERT_EQUAL_INT(_SeqSum(14),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14));
   TEST_ASSERT_EQUAL_INT(_SeqSum(15),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15));
   TEST_ASSERT_EQUAL_INT(_SeqSum(16),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16));
   TEST_ASSERT_EQUAL_INT(_SeqSum(17),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17));
   TEST_ASSERT_EQUAL_INT(_SeqSum(18),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18));
   TEST_ASSERT_EQUAL_INT(_SeqSum(19),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19));
   TEST_ASSERT_EQUAL_INT(_SeqSum(20),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20));
   TEST_ASSERT_EQUAL_INT(_SeqSum(21),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21));
   TEST_ASSERT_EQUAL_INT(_SeqSum(22),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22));
   TEST_ASSERT_EQUAL_INT(_SeqSum(23),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23));
   TEST_ASSERT_EQUAL_INT(_SeqSum(24),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24));
   TEST_ASSERT_EQUAL_INT(_SeqSum(25),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25));
   TEST_ASSERT_EQUAL_INT(_SeqSum(26),  _Infix(+,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26));

   #undef _SeqSum
}

/* ------------------------------- test_Prefix --------------------------------------------- */

void test_Prefix(void)
{
   #define _SeqSum(n)  -(((n) * ((n) + 1)) / 2)

   // Supports 26 parms, for now (a-z).
   TEST_ASSERT_EQUAL_INT(_SeqSum(1),   _Prefix(-,1));          // -1
   TEST_ASSERT_EQUAL_INT(_SeqSum(2),   _Prefix(-,1,2));           // -1-2
   TEST_ASSERT_EQUAL_INT(_SeqSum(3),   _Prefix(-,1,2,3));            // -1-2-3
   TEST_ASSERT_EQUAL_INT(_SeqSum(4),   _Prefix(-,1,2,3,4));             // etc...
   TEST_ASSERT_EQUAL_INT(_SeqSum(5),   _Prefix(-,1,2,3,4,5));
   TEST_ASSERT_EQUAL_INT(_SeqSum(6),   _Prefix(-,1,2,3,4,5,6));
   TEST_ASSERT_EQUAL_INT(_SeqSum(7),   _Prefix(-,1,2,3,4,5,6,7));
   TEST_ASSERT_EQUAL_INT(_SeqSum(8),   _Prefix(-,1,2,3,4,5,6,7,8));
   TEST_ASSERT_EQUAL_INT(_SeqSum(9),   _Prefix(-,1,2,3,4,5,6,7,8,9));
   TEST_ASSERT_EQUAL_INT(_SeqSum(10),  _Prefix(-,1,2,3,4,5,6,7,8,9,10));
   TEST_ASSERT_EQUAL_INT(_SeqSum(11),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11));
   TEST_ASSERT_EQUAL_INT(_SeqSum(12),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12));
   TEST_ASSERT_EQUAL_INT(_SeqSum(13),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13));
   TEST_ASSERT_EQUAL_INT(_SeqSum(14),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14));
   TEST_ASSERT_EQUAL_INT(_SeqSum(15),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15));
   TEST_ASSERT_EQUAL_INT(_SeqSum(16),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16));
   TEST_ASSERT_EQUAL_INT(_SeqSum(17),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17));
   TEST_ASSERT_EQUAL_INT(_SeqSum(18),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18));
   TEST_ASSERT_EQUAL_INT(_SeqSum(19),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19));
   TEST_ASSERT_EQUAL_INT(_SeqSum(20),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20));
   TEST_ASSERT_EQUAL_INT(_SeqSum(21),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21));
   TEST_ASSERT_EQUAL_INT(_SeqSum(22),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22));
   TEST_ASSERT_EQUAL_INT(_SeqSum(23),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23));
   TEST_ASSERT_EQUAL_INT(_SeqSum(24),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24));
   TEST_ASSERT_EQUAL_INT(_SeqSum(25),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25));
   TEST_ASSERT_EQUAL_INT(_SeqSum(26),  _Prefix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26));

   #undef _SeqSum
}

/* ------------------------------- test_Postfix --------------------------------------------- */

void test_Postfix(void)
{
   #define _SeqSum(n)  (-(((n) * ((n) + 1)) / 2) - 98)

   // Supports 26 parms, for now (a-z).
   TEST_ASSERT_EQUAL_INT(_SeqSum(1),   _Postfix(-,1) 100 );             // 1-100
   TEST_ASSERT_EQUAL_INT(_SeqSum(2),   _Postfix(-,1,2) 100 );              // 1-2-100
   TEST_ASSERT_EQUAL_INT(_SeqSum(3),   _Postfix(-,1,2,3) 100 );               // 1-2-3-100
   TEST_ASSERT_EQUAL_INT(_SeqSum(4),   _Postfix(-,1,2,3,4) 100 );                // etc..
   TEST_ASSERT_EQUAL_INT(_SeqSum(5),   _Postfix(-,1,2,3,4,5) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(6),   _Postfix(-,1,2,3,4,5,6) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(7),   _Postfix(-,1,2,3,4,5,6,7) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(8),   _Postfix(-,1,2,3,4,5,6,7,8) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(9),   _Postfix(-,1,2,3,4,5,6,7,8,9) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(10),  _Postfix(-,1,2,3,4,5,6,7,8,9,10) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(11),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(12),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(13),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(14),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(15),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(16),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(17),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(18),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(19),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(20),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(21),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(22),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(23),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(24),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(25),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25) 100 );
   TEST_ASSERT_EQUAL_INT(_SeqSum(26),  _Postfix(-,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26) 100 );

   #undef _SeqSum
}

/* ------------------------------- test_Map --------------------------------------------- */

void test_Map(void)
{
   #define _x3(n) (3*(n))
   #define _SeqSum(n)  (3 * (((n) * ((n) + 1)) / 2))

   // Multiply each x3 then add.
   TEST_ASSERT_EQUAL_INT(_SeqSum(1),   _Map(_x3,1));                       // 1*3
   TEST_ASSERT_EQUAL_INT(_SeqSum(3),   _Infix(+,_Map(_x3,1,2,3)));            // 3*1 + 3*2
   TEST_ASSERT_EQUAL_INT(_SeqSum(4),   _Infix(+,_Map(_x3,1,2,3,4)));             // 3*1 + 3*2 + 3*3
   TEST_ASSERT_EQUAL_INT(_SeqSum(5),   _Infix(+,_Map(_x3,1,2,3,4,5)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(6),   _Infix(+,_Map(_x3,1,2,3,4,5,6)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(7),   _Infix(+,_Map(_x3,1,2,3,4,5,6,7)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(8),   _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(9),   _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(10),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(11),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(12),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(13),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(14),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(15),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(16),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(17),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(18),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(19),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(20),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(21),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(22),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(23),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(24),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(25),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25)));
   TEST_ASSERT_EQUAL_INT(_SeqSum(26),  _Infix(+,_Map(_x3,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26)));


   #undef _SeqSum
}

// ----------------------------------------- eof --------------------------------------------
