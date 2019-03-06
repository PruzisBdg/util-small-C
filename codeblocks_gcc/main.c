#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "wordlist.h"
#include <string.h>
#include "tiny1_stdio.h"
#include "tiny1_printf_test_support.h"

int main(void)
{
   OStream_Reset();
   tiny1_printf("%4S", "\xCB" "abcdefg");
   OStream_Print();

    return 0;
}
