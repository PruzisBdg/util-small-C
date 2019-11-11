#include "util.h"

PUBLIC BOOLEAN AllBytesAre_U16(U8 const *src, U8 n, U16 cnt)
{
   for(U16 i = 0; i < cnt; i++, src++)
   {
      if(*src != n)
      {
         return FALSE;
      }
   }
   return TRUE;
}

// ---------------------------------------- eof ---------------------------------------

