/* BCD little-endian
*/

#include "libs_support.h"
#include "util.h"

// ---- unprotected --------------------------------------------
static U8 bcdU8(U8 n) {
   return ((n / 10) << 4) + (n % 10); }

/* ----------------- BCD2_LE..U64 ------------------------------

   'n' -> 'out' as BCD little-endian.

   Return false if 'n' is too large.
   If return false then 'out' will be unchanged.
*/

// --------------------------------------------------------------
PUBLIC bool BCD2_LE(U8 *out, U8 n)
{
   if(n > 99)
      { return false; }
   else {
      out[0] = bcdU8(n);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool BCD4_LE(U8 *out, U16 n)
{
   if(n > 9999)
      { return false; }
   else {
      out[0] = bcdU8(n % 100);
      out[1] = bcdU8(n / 100);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool BCD8_LE(U8 *out, U32 n)
{
   if(n > 99999999)
      { return false; }
   else {
      BCD4_LE(&out[0], n % 10000);
      BCD4_LE(&out[2], n / 10000);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool BCD12_LE(U8 *out, U64 n)
{
   if(n > 999999999999ULL)
      { return false; }
   else {
      BCD8_LE(&out[0], n % 100000000);
      BCD4_LE(&out[4], n / 100000000);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool BCD16_LE(U8 *out, U64 n)
{
   if(n > 9999999999999999ULL)
      { return false; }
   else {
      BCD8_LE(&out[0], n % 100000000);
      BCD8_LE(&out[4], n / 100000000);
      return true; }
}

// ----------------------------------- eof -------------------------------------------------
