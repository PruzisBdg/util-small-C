#include "util.h"

PUBLIC BOOLEAN AllBytesTestTrue_U16(U8 const *src, U16 cnt, bool(*test)(U8))
{
   for(U16 i = 0; i < cnt; i++)
   {
      if( test(src[i]) == FALSE)
      {
         return FALSE;
      }
   }
   return TRUE;
}

// -------------------------------------- eof --------------------------------------
