#include "libs_support.h"
#include "util.h"

// ---- Little endian
PUBLIC void u16ToLE(U8 *out, U16 n)
	{ out[0] = LOW_BYTE(n); out[1] = HIGH_BYTE(n); }

PUBLIC void s16ToLE(U8 *out, S16 n)
	{ out[0] = LOW_BYTE(n); out[1] = HIGH_BYTE(n); }

PUBLIC void u32ToLE(U8 *out, U32 n)
	{ u16ToLE(&out[0], LOW_WORD(n)); u16ToLE(&out[2], HIGH_WORD(n)); }

PUBLIC U16 leToU16(U8 const * src)
	{ return ((U16)src[1] << 8) + src[0]; }

PUBLIC U32 leToU32(U8 const *src)
	{ return ((U32)src[3] << 24) + ((U32)src[2] << 16) + ((U16)src[1] << 8) + src[0]; }

// ---- Big endian
PUBLIC void u16ToBE(U8 *out, U16 n)
	{ out[0] = HIGH_BYTE(n); out[1] = LOW_BYTE(n); }

PUBLIC void s16ToBE(U8 *out, S16 n)
	{ out[0] = HIGH_BYTE(n); out[1] = LOW_BYTE(n); }

PUBLIC void u32ToBE(U8 *out, U32 n)
	{ u16ToLE(&out[0], HIGH_WORD(n)); u16ToLE(&out[2], LOW_WORD(n)); }

PUBLIC U16 beToU16(U8 const * src)
	{ return ((U16)src[0] << 8) + src[1]; }

PUBLIC U32 beToU32(U8 const *src)
	{ return ((U32)src[0] << 24) + ((U32)src[1] << 16) + ((U16)src[2] << 8) + src[3]; }

// =========================================== EOF ===================================================
