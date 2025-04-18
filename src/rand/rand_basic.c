#include "util.h"
#include <stdlib.h>

PUBLIC U8  randU8(void)  { return (U8)((float)rand() * MAX_U8 / RAND_MAX); }
PUBLIC U16 randU16(void) { return (U16)((float)rand() * MAX_U16 / RAND_MAX); }
PUBLIC U16 randU32(void) { return (U32)((float)rand() * MAX_U32 / RAND_MAX); }
PUBLIC S16 randS16(void) { return (S16)( 2.0 * (float)((S32)rand() - (RAND_MAX/2)) * MAX_S16 / RAND_MAX); }
PUBLIC S32 randS32(void) { return (S32)( 2.0F * (float)((S32)rand() - (RAND_MAX/2)) * MAX_S32 / RAND_MAX); }
PUBLIC S8  randS8(void)  { return (S8) ( 2.0 * (float)((S32)rand() - (RAND_MAX/2)) * MAX_S8  / RAND_MAX); }

PUBLIC void randFill(void *dest, size_t numBytes) {
   for(size_t i = 0; i < numBytes; i++) {
      *((U8*)dest) = randU8();
      dest = (void*)((uintptr_t)dest+1);
   }
}

// ------------------------------------------- eof -----------------------------------------------------
