/* BCD little-endian
*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>

// ============================= To BCD ========================================

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
PUBLIC bool BCD6_LE(U8 *out, U32 n)
{
   if(n > 999999)
      { return false; }
   else {
      BCD4_LE(&out[0], n % 10000);
      out[2] = bcdU8(n / 10000);
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



// ============================= From BCD ========================================

static bool isBCD(U8 n) {
   return LOW_NIBBLE(n) <= 9 && HIGH_NIBBLE(n) <= 9; }

// --------------------------------------------------------------
PUBLIC bool BCDtoU8(U8 *out, U8 n) {
   if(isBCD(n) == false) {
      return false; }
   else {
      *out = 10 * HIGH_NIBBLE(n) + LOW_NIBBLE(n);
      return true; }}

// --------------------------------------------------------------
PUBLIC bool BCD4le_toU16(U16 *out, U8 const *in) {
   U8 n0;
   if(true == BCDtoU8(&n0, in[0])) {
      U8 n1;
      if(true == BCDtoU8(&n1, in[1])) {
         *out = n0 + (100 * (U16)n1);
         return true; }}
   return false; }

// --------------------------------------------------------------
PUBLIC bool BCD6le_toU32 ( U32 *out, U8 const *in) {
   U16 n0;
   if(true == BCD4le_toU16(&n0, &in[0])) {
      U8 n1;
      if(true == BCDtoU8(&n1, in[2])) {
         *out = n0 + (10000 * (U32)n1);
         return true; }}
   return false; }

// --------------------------------------------------------------
PUBLIC bool BCD8le_toU32 (U32 *out, U8 const *in) {
   U16 n0;
   if(true == BCD4le_toU16(&n0, &in[0])) {
      U16 n1;
      if(true == BCD4le_toU16(&n1, &in[2])) {
         *out = n0 + (10000 * (U32)n1);
         return true; }}
   return false; }

// --------------------------------------------------------------
PUBLIC bool BCD12le_toU64( U64 *out, U8 const *in) {
   U32 n0;
   if(true == BCD8le_toU32(&n0, &in[0])) {
      U16 n1;
      if(true == BCD4le_toU16(&n1, &in[4])) {
         *out = n0 + (100000000L * (U64)n1);
         return true; }}
   return false; }



// ----------------------------------- eof -------------------------------------------------
