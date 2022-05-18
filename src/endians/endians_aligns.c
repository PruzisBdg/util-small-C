#include "libs_support.h"
#include "util.h"

// ---- Little endian
PUBLIC U8* u16ToLE(U8 *out, U16 n)
	{ out[0] = LOW_BYTE(n); out[1] = HIGH_BYTE(n); return out; }

PUBLIC U8 * s16ToLE(U8 *out, S16 n)
	{ out[0] = LOW_BYTE(n); out[1] = HIGH_BYTE(n); return out; }

PUBLIC U8 * u32ToLE(U8 *out, U32 n)
	{ u16ToLE(&out[0], LOW_WORD(n)); u16ToLE(&out[2], HIGH_WORD(n)); return out; }

PUBLIC U8 * s32ToLE(U8 *out, S32 n)
	{ s16ToLE(&out[0], LOW_WORD(n)); s16ToLE(&out[2], HIGH_WORD(n)); return out; }

PUBLIC U8 * u64ToLE(U8 *out, U64 n)
	{ u32ToLE(&out[0], LOW_DWORD(n)); u32ToLE(&out[4], HIGH_DWORD(n)); return out; }

PUBLIC U8 * s64ToLE(U8 *out, S64 n)
	{ s32ToLE(&out[0], LOW_DWORD(n)); s32ToLE(&out[4], HIGH_DWORD(n)); return out; }


// Write the number represented by low 24 bits of 'n' to out; modifying just 3 bytes.
PUBLIC U8 * u24ToLE(U8 *out, U32 n)
	{ u16ToLE(&out[0], LOW_WORD(n)); out[2] = LOW_BYTE(HIGH_WORD(n)); return out; }



PUBLIC S16 leToS16(U8 const * src)
	{ return ((S16)src[1] << 8) + src[0]; }

PUBLIC U16 leToU16(U8 const * src)
	{ return ((U16)src[1] << 8) + src[0]; }

PUBLIC U32 leToU24(U8 const *src)
	{ return ((U32)src[2] << 16) + leToU16(&src[0]); }

PUBLIC U32 leToU32(U8 const *src)
	{ return ((U32)leToU16(&src[2]) << 16) + leToU16(&src[0]); }

PUBLIC U64 leToU48(U8 const *src)
	{ return ((U64)leToU16(&src[4]) << 32) + leToU32(&src[0]); }

PUBLIC U64 leToU64(U8 const *src)
	{ return ((U64)leToU32(&src[4]) << 32) + leToU32(&src[0]); }

PUBLIC U64 BytesLEtoU64(U8 const *src, U8 numBytes) {
   U64 out = 0;
   for(U8 i = numBytes; i > 0; i--) {
      out = (out << 8) + src[i-1]; }
   return out; }

PUBLIC float leToFloat(U8 const *src)
{
   union utag { U8 b[4]; float f; } u;

   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
         // Swap and align;
         u.b[0] = src[3]; u.b[1] = src[2]; u.b[2] = src[1]; u.b[3] = src[0];
      #else
         // else just align;
         u.b[0] = src[0]; u.b[1] = src[1]; u.b[2] = src[2]; u.b[3] = src[3];
      #endif
   #else
      #warning "ToSysEndian_U16() Endian undefined - bytes will always be copied no-reverse."
      // Just align;
      u.b[0] = src[0]; u.b[1] = src[1]; u.b[2] = src[2]; u.b[3] = src[3];
   #endif // __BYTE_ORDER__

   return u.f;    // Return as float.
}

// ---- Big endian
PUBLIC U8 * u16ToBE(U8 *out, U16 n)
	{ out[0] = HIGH_BYTE(n); out[1] = LOW_BYTE(n); return out; }

PUBLIC U8 * s16ToBE(U8 *out, S16 n)
	{ out[0] = HIGH_BYTE(n); out[1] = LOW_BYTE(n); return out; }

PUBLIC U8 * u32ToBE(U8 *out, U32 n)
	{ u16ToBE(&out[0], HIGH_WORD(n)); u16ToBE(&out[2], LOW_WORD(n)); return out; }

PUBLIC U8 * s32ToBE(U8 *out, S32 n)
	{ s16ToBE(&out[0], HIGH_WORD(n)); s16ToBE(&out[2], LOW_WORD(n)); return out; }

PUBLIC U8 * u64ToBE(U8 *out, U64 n)
	{ u32ToBE(&out[0], HIGH_DWORD(n)); u32ToBE(&out[4], LOW_DWORD(n)); return out; }

PUBLIC U8 * s64ToBE(U8 *out, S64 n)
	{ s32ToBE(&out[0], HIGH_DWORD(n)); s32ToBE(&out[4], LOW_DWORD(n)); return out; }



PUBLIC U16 beToU16(U8 const * src)
	{ return ((U16)src[0] << 8) + src[1]; }

PUBLIC U32 beToU32(U8 const *src)
	{ return ((U32)beToU16(&src[0]) << 16) + beToU16(&src[2]); }

PUBLIC U64 beToU64(U8 const *src)
	{ return ((U64)beToU32(&src[0]) << 32) + beToU32(&src[4]); }

// =========================================== EOF ===================================================
