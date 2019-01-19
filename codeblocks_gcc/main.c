#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"


int main(void)
{
    T_IPAddrNum n = 0x55AA55AA;
    //C8 const str[] = "0.1.2.3";
    //C8 const str[] =  "16 .32. 48 . 64 ";
    //C8 const str[] =  "just some text";
    C8 const str[]  = "7.89.22  1.2.3.4tail";

    C8 const *p = ReadIPAddr(str, &n);
    C8 b1[_MaxIPAddrChars+1];

    printf("IP \"%s\" -> 0x%08lx -> %s, tail = \"%s\"\r\n", str, n, PrintIPAddr(b1, n), p == NULL ? "NULL" : (*p == '\0' ? "\\0" : p));

    return 0;
}
