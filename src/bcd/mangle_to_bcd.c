#include "libs_support.h"
#include "util.h"

PUBLIC U32 MangleU32toBCD(U32 n) {
   union u {U32 _u32; U8 bs[4]; } u;
   u._u32 = n;
   for(U8 i = 0; i < 4; i++) {
      u.bs[i] = U8toBCD(u.bs[i] % 99); }
   return u._u32; }

// ------------------------------- eof -------------------------------------------------

