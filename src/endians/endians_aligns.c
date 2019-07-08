#include "libs_support.h"
#include "util.h"

// ---- Little endian
PUBLIC void u16ToLE(U8 *out, U16 n)
	{ out[0] = LOW_BYTE(n); out[1] = HIGH_BYTE(n); }

PUBLIC void s16ToLE(U8 *out, S16 n)
	{ out[0] = LOW_BYTE(n); out[1] = HIGH_BYTE(n); }

PUBLIC void u32ToLE(U8 *out, U32 n)
	{ u16ToLE(&out[0], LOW_WORD(n)); u16ToLE(&out[2], HIGH_WORD(n)); }

PUBLIC void u64ToLE(U8 *out, U64 n)
	{ u32ToLE(&out[0], LOW_DWORD(n)); u32ToLE(&out[4], HIGH_DWORD(n)); }



PUBLIC U16 leToU16(U8 const * src)
	{ return ((U16)src[1] << 8) + src[0]; }

PUBLIC U32 leToU32(U8 const *src)
	{ return (U32)leToU16(&src[2]) << 16 + leToU16(&src[0]); }

PUBLIC U64 leToU64(U8 const *src)
	{ return (U64)leToU32(&src[4]) << 32 + leToU32(&src[0]); }



// ---- Big endian
PUBLIC void u16ToBE(U8 *out, U16 n)
	{ out[0] = HIGH_BYTE(n); out[1] = LOW_BYTE(n); }

PUBLIC void s16ToBE(U8 *out, S16 n)
	{ out[0] = HIGH_BYTE(n); out[1] = LOW_BYTE(n); }

PUBLIC void u32ToBE(U8 *out, U32 n)
	{ u16ToBE(&out[0], HIGH_WORD(n)); u16ToBE(&out[2], LOW_WORD(n)); }

PUBLIC void s32ToBE(U8 *out, S32 n)
	{ s16ToBE(&out[0], HIGH_WORD(n)); s16ToBE(&out[2], LOW_WORD(n)); }

PUBLIC void u64ToBE(U8 *out, U64 n)
	{ u32ToBE(&out[0], HIGH_DWORD(n)); u32ToBE(&out[4], LOW_WORD(n)); }

PUBLIC void s64ToBE(U8 *out, S64 n)
	{ s32ToBE(&out[0], HIGH_DWORD(n)); s32ToBE(&out[4], LOW_WORD(n)); }



PUBLIC U16 beToU16(U8 const * src)
	{ return ((U16)src[0] << 8) + src[1]; }

PUBLIC U32 beToU32(U8 const *src)
	{ return (U32)beToU16(&src[0]) << 16 + beToU16(&src[2]); }

PUBLIC U64 beToU64(U8 const *src)
	{ return (U64)beToU32(&src[0]) << 32 + beToU32(&src[4]); }

// =========================================== EOF ===================================================
