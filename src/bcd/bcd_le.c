/* BCD little-endian
*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>

// ============================= To BCD ========================================

// ---- unprotected --------------------------------------------
PUBLIC U8 U8toBCD(U8 n) {
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
      out[0] = U8toBCD(n);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool BCD4_LE(U8 *out, U16 n)
{
   if(n > 9999)
      { return false; }
   else {
      out[0] = U8toBCD(n % 100);
      out[1] = U8toBCD(n / 100);
      return true; }
}

// --------------------------------------------------------------
PUBLIC bool BCD6_LE(U8 *out, U32 n)
{
   if(n > 999999)
      { return false; }
   else {
      BCD4_LE(&out[0], n % 10000);
      out[2] = U8toBCD(n / 10000);
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



/* ============================= From BCD little-endian ===================================

   1. Return false if the input is not all BCD.
   2. If return false then output is unmodified.
*/
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

/* ----------------------- BCDle_toU64 -----------------------

   Up to 8 BCD bytes into a U64.
*/
PUBLIC bool BCDle_toU64(U64 *out, U8 const *in, U8 numBytes) {
   U8 n;
   U64 _u64 = 0;
   for(U8 i = numBytes; i > 0; i--) {        // For each (BCD) byte...
      if(BCDtoU8(&n, in[i-1]) == false) {    // Not a BCD?
         return false; }                     // then bail.
      _u64 = (100 * _u64) + n; }             // else add to 'out' little-endian.
   *out = _u64;
   return true; }




// ----------------------------------- eof -------------------------------------------------
