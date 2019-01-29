#include <stdio.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"
#include "wordlist.h"
#include <string.h>

int main(void)
{
#if 0

   S16 n;
   C8 const inStr[] = "0";
   ReadDirtyASCIIInt(inStr, &n);
   printf("ReadDirtyASCIIInt \"%s\" -> %d\r\n", inStr, n);

    T_IPAddrNum n = 0x55AA55AA;
    C8 const str[] = "0.0.0.0";
    //C8 const str[] = "0.1.2.3";
    //C8 const str[] =  "16 .32. 48 . 64 ";
    //C8 const str[] =  "just some text";
    //C8 const str[]  = "7.89.22  1.2.3.4tail";

    C8 const *p = ReadIPAddr(str, &n);
    C8 b1[_MaxIPAddrChars+1];

    printf("IP \"%s\" -> 0x%08lx -> %s, tail = \"%s\"\r\n", str, n, PrintIPAddr(b1, n), p == NULL ? "NULL" : (*p == '\0' ? "\\0" : p));
#endif
   C8 b0[100];
   strcpy(b0, "");
    printf("ret = \"%s\"\r\n", Str_Insert(b0, "abc def", 3, 2));
    return 0;
}
