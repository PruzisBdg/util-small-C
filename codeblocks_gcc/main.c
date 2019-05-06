#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "wordlist.h"
#include <string.h>
#include "tiny1_stdio.h"
#include "tiny1_printf_test_support.h"
#include "float.h"
#include <math.h>

// Test snippets for checking bit64K copys.
#include "bit64k_copy_tests_inline.h"

bool is_infinity_is_zero_is_denormal(float f) {
  return fpclassify(f) & (FP_INFINITE | FP_ZERO | FP_SUBNORMAL);
}

int main (void)
{
   //cpyOuts();
   //cpyOutsCached();

   float f0 = FLT_MAX;
   f0 *= (1+FLT_EPSILON);

   int cc = fpclassify(f0);

   printf("0x%x FLT_MAX_EXP %d FLT_MIN_EXP %d FLT_MIN_10_EXP %d FLT_MAX_10_EXP %d FLT_MAX %e FLT_MIN%e\r\n",
          cc, FLT_MAX_EXP, FLT_MIN_EXP, FLT_MIN_10_EXP, FLT_MAX_10_EXP, f0, FLT_MIN );
}

// ------------------------------------- eof ---------------------------------------------
