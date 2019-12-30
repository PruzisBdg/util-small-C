#include "libs_support.h"
#include "util.h"

// ---------------------------------------------------------------------------
static U16 _u16toBCD(U16 n) {
   return ((U16)U8toBCD(n/100) << 8) + U8toBCD(n % 100); }

// ---------------------------------------------------------------------------
static U32 _u32toBCD(U32 n) {
   return ((U16)_u16toBCD(n/10000) << 16) + _u16toBCD(n % 10000); }

// ---------------------------------------------------------------------------
PUBLIC bool U16toBCD(U16 *bcd, U16 n) {
   if(n > 9999)
      { return false; }
   else {
      *bcd = _u16toBCD(n);
      return true; }
}

// ---------------------------------------------------------------------------
PUBLIC bool U32toBCD(U32 *bcd, U32 n) {
   if(n > 99999999)
      { return false; }
   else {
      *bcd = ((U32)_u16toBCD(n / 10000) << 16) + _u16toBCD(n % 10000);
      return true; }
}

// ---------------------------------------------------------------------------
PUBLIC bool U64toBCD(U64 *bcd, U64 n) {
   if(n > 9999999999999999ULL)
      { return false; }
   else {
      *bcd = ((U64)_u32toBCD(n / 100000000UL) << 32) + _u32toBCD(n % 100000000UL);
      return true; }
}


// ---------------------------------------------------------------------------
static bool inline isBCD(U8 n) {
   return LOW_NIBBLE(n) <= 9 && HIGH_NIBBLE(n) <= 9; }

// ---------------------------------------------------------------------------
PUBLIC bool U8isBCD(U8 n) {
   return isBCD(n); }

// ---------------------------------------------------------------------------
PUBLIC bool U16isBCD(U16 n) {
   return isBCD(HIGH_BYTE(n)) && isBCD(LOW_BYTE(n)); }

// ---------------------------------------------------------------------------
PUBLIC bool U32isBCD(U32 n) {
   return U16isBCD(HIGH_WORD(n)) && U16isBCD(LOW_WORD(n)); }

// ------------------------------------------- eof ------------------------------------------------
