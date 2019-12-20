#include "libs_support.h"
#include "util.h"

PUBLIC bool U32toBCD(U32 *bcd, U32 n) {
   if(n > 99999999)
      { return false; }
   else {
      typedef union { U32 _u32; U8 bytes[sizeof(U32)]; } U_U32;

      for(U8 i = 0; i < sizeof(U32); i++) {
         ((U_U32*)bcd)->bytes[i] = U8toBCD( ((U_U32*)&n)->bytes[i] ); }
      return true; }
}

// ------------------------------------------- eof ------------------------------------------------
