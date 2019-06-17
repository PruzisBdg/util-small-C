/* BCD little-endian
*/

#include "libs_support.h"
#include "util.h"

// ---- unprotected --------------------------------------------
static U8 bcdU8(U8 n) {
   return ((n / 10) << 4) + (n % 10); }

/* ----------------- bcdLE_U8..U64 ------------------------------

   'n' -> 'out' as BCD little-endian.

   Return false if 'n' is too large.
   If return false then 'out' will be unchanged.
*/

// --------------------------------------------------------------
PUBLIC bool bcdLE_U8(U8 *out, U8 n)
{
   if(n > 99)
      { return false; }
   else {
      out[0] = bcdU8(n);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool bcdLE_U16(U8 *out, U16 n)
{
   if(n > 9999)
      { return false; }
   else {
      out[0] = bcdU8(n % 100);
      out[1] = bcdU8(n / 100);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool bcdLE_U32(U8 *out, U32 n)
{
   if(n > 99999999)
      { return false; }
   else {
      bcdLE_U16(&out[0], n % 10000);
      bcdLE_U16(&out[2], n / 10000);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool bcdLE_U48(U8 *out, U64 n)
{
   if(n > 999999999999ULL)
      { return false; }
   else {
      bcdLE_U32(&out[0], n % 100000000);
      bcdLE_U16(&out[4], n / 100000000);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool bcdLE_U64(U8 *out, U64 n)
{
   if(n > 9999999999999999ULL)
      { return false; }
   else {
      bcdLE_U32(&out[0], n % 100000000);
      bcdLE_U32(&out[4], n / 100000000);
      return true; }
}

// ----------------------------------- eof -------------------------------------------------
