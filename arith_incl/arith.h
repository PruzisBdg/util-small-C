/* -----------------------------------------------------------------
|
|   Arithmetic Support
|
|   File: arith.h
|
--------------------------------------------------------------------- */

#ifndef ARITH_H
#define	ARITH_H

#include "GenericTypeDefs.h"

PUBLIC S16  ClipS32toS16(S32 n);
PUBLIC U16  ClipS32toU16(S32 n);
PUBLIC S32  ClipU32toS32(U32 n);
PUBLIC U16  ClipU32toU16(U32 n);
PUBLIC U8   ClipS16toU8(S16 n);
PUBLIC U8   ClipU16toU8(U16 n);

PUBLIC S8   MinS8(S8 a, S8 b);
PUBLIC S8   MaxS8(S8 a, S8 b);
PUBLIC S8   MaxS8(S8 a, S8 b);
PUBLIC S16  MinS16(S16 a, S16 b);
PUBLIC S16  MaxS16(S16 a, S16 b);
PUBLIC S8   ClipS8(S8 n, S8 min, S8 max);
PUBLIC U8   ClipU8(U8 n, U8 min, U8 max);
PUBLIC U16  ClipU16(U16 n, U16 min, U16 max);
PUBLIC S16  ClipS16(S16 n, S16 min, S16 max);
PUBLIC S32  ClipS32(S32 n, S32 min, S32 max);
PUBLIC U8   MinU8(U8 a, U8 b);
PUBLIC U8   MaxU8(U8 a, U8 b);
PUBLIC U16  MinU16(U16 a, U16 b);
PUBLIC U16  MaxU16(U16 a, U16 b);
PUBLIC U32  MinU32(U32 a, U32 b);
PUBLIC U32  MaxU32(U32 a, U32 b);

PUBLIC BIT  Inside_U8(U8 a, U8 min, U8 max);
PUBLIC BIT  Inside_U16(U16 a, U16 min, U16 max);
PUBLIC BIT  Inside_S16(S16 a, S16 min, S16 max);
PUBLIC BIT  InsideEq_U32(U32 a, U32 min, U32 max);

PUBLIC U8   AplusBU8(U8 a, U8 b);
PUBLIC S16  AplusBS16(S16 a, S16 b);
PUBLIC U32  AplusBU32(U32 a, U32 b);
PUBLIC U16  AplusB_U16(U16 a, U16 b);
PUBLIC S32  AplusBS32(S32 a, S32 b);
PUBLIC U16  U16plusS16_toU16(U16 a, S16 b);
PUBLIC U32  U32plusS16(U32 a, S16 b);
PUBLIC U32  U32plusS32_toU32(U32 a, S32 b);

#define AplusBU16(a,b)  AplusB_U16((a),(b))

#define _Inv_AplusB_Inv(a,b)  (1.0 / ((1.0/(a)) + (1.0/(b))) )

PUBLIC S32 MinusS32(S32 n);		// Safe unary minus

PUBLIC U8   AminusBU8(U8 a, U8 b);
PUBLIC U16  AminusBU16(U16 a, U16 b);
PUBLIC U32  AminusBU32(U32 a, U32 b);
PUBLIC S32  AminusBU32toS32(U32 a, U32 b);
PUBLIC S16  AminusBS16(S16 a, S16 b);
PUBLIC U32  AminusBU32(U32 a, U32 b);
PUBLIC S16  AminusBU16ToS16(U16 a, U16 b);
PUBLIC S32 AminusBU32toS32(U32 a, U32 b);
#define _AminusB_unsigned(a,b)  (a > b ? a - b : 0)

PUBLIC BIT DecrU8_NowZero(U8 * a);
PUBLIC BIT DecrU8_Was1_NowZero(U8 * a);

#define _SignEqualsS16(a,b) ((((a) ^ (b)) & 0x8000) == 0)
PUBLIC BOOL SignEqualsS16(S16 a, S16 b);
PUBLIC S16  AbsS16(S16 n);

PUBLIC S32  MulS16(S16 a, S16 b);
#define     MulS16ToS16(a,b)  ClipS32toS16((a) * (S32)(b))
PUBLIC S16  AmulBdivC_S16(S16 a, S16 b, S16 c);
PUBLIC U16  AmulBdivC_U16(U16 a, U16 b, U16 c);
PUBLIC U16  AmulBdivC_U16_rnd(U16 a, U16 b, U16 c);
PUBLIC S16  AmulBdivC_S16_U8_U8(S16 a, U8 b, U8 c);
PUBLIC U8   AmulBdivC_U8(U8 a, U8 b, U8 c);
PUBLIC U8   AmulBdivC_U8_rnd(U8 a, U8 b, U8 c);
PUBLIC S16  AmulBdivC_S16_S8_S8(S16 a, S8 b, S8 c);
PUBLIC S16  AmulBdivC_S16_S8_S8(S16 a, S8 b, S8 c);

PUBLIC U8   MixU8(U8 a, U8 b, U8 m);
PUBLIC U16  MixU16(U16 a, U16 b, U8 m);


PUBLIC U8   Mean2_U8(U8 a, U8 b);
PUBLIC U8   Span3_U8(U8 a, U8 b, U8 c);
PUBLIC U8   Span4_U8(U8 a, U8 b, U8 c, U8 d);
PUBLIC U8   Mean3_U8(U8 a, U8 b, U8 c);
PUBLIC U8   Mean4_U8(U8 a, U8 b, U8 c, U8 d);
PUBLIC U8   AbsDiffU8(U8 a, U8 b);
PUBLIC U16  AbsDiffU16(U16 a, U16 b);
PUBLIC U16  AbsDiffS16(S16 a, S16 b);
PUBLIC U32  AbsDiffU32(U32 a, U32 b);
PUBLIC U8   Median3_U8(U8 a, U8 b, U8 c);

PUBLIC U8   SqrtU16(U16 n);
PUBLIC U16  SqrtU32(U32 n);

PUBLIC S16  DeadbandS16(S16 n, S16 lo, S16 hi);

PUBLIC U16  SqU8(U8 a);
PUBLIC U32  SqU16(U16 n);
PUBLIC S32  SqS16(S16 n);

PUBLIC U8   RootSumSqU8(U8 a, U8 b);
PUBLIC U16  RootSumSqU16(U16 a, U16 b);
PUBLIC U16  RootSumSqS16(S16 a, S16 b);

PUBLIC U8 SigmaSqU8(U8 n);

typedef struct {
    U8 const *theTbl;
    U8 tblSize;
} S_ConstTblU8;

PUBLIC U8 LinterpFromU8Tbl_naked(U8 const *tbl, U8 x);
PUBLIC U8 LinterpFromU8Tbl(S_ConstTblU8 const *tbl, U8 x);
#define _LinterpTblU8_Step        16
#define _LinterpTblU8_MaxEntries  17    // 16 x 16 = 256 (U8) plus end-point

typedef struct {
    S16 const *theTbl;
    U8 tblSize;
} S_ConstTblS16;

PUBLIC S16 LinterpFromS16Tbl(S_ConstTblS16 const *tbl, S16 x);
PUBLIC S16 LinterpFromS16Tbl_naked(S16 const *tbl, S16 x);
#define _LinterpTblS16_Step  256

PUBLIC float GetPwr10Float(S16 exp);
PUBLIC S32   ClipFloatToLong(float f);
PUBLIC S16   ClipFloatToInt(float f);
PUBLIC float MinFloat(float a, float b);
PUBLIC float MaxFloat(float a, float b);
PUBLIC float Mod_Float(float n, float d);
PUBLIC BOOL  InsideEq_Float(float n, float min, float max);

PUBLIC U8 RankU8_UpTo8( U8 const *buf, U8 size, U8 rank );
PUBLIC U8 RankU8_UpTo16( U8 const *buf, U8 size, U8 rank );

PUBLIC U8 MSB_U16(U16 n);

#ifndef PI
    #define PI 3.14159265358979
#endif

#define SQRT2 1.41421356237

// ---------------------- For constant expressions ---------------------------------
//
// These macros solve problems particular to constant expressions. They will work
// at runtime but they're really not for that.

// Return the floor() and fractional parts of a (floating point) number
// Uses '%' to block promotion to float within the sub-expression.
#define _Floor_Const(n) ((((long)(n) - 1) % (long)(n) ) + 1)
#define _Frac_Const(n)  ((n) - _Floor_Const(n))

/* --------------------- Scalar Accumulator ------------------------------------- */

typedef struct
{
   S32 acc;
   U16 cnt;
} S_Acc;

PUBLIC void Acc_Add(S_Acc *a, S16 n);
PUBLIC void Acc_Clear(S_Acc *a);
PUBLIC S16  Acc_Read(S_Acc *a);
PUBLIC S16  Acc_ReadAndClear(S_Acc *a);
#define _Acc_Cnt(a)  ((a).cnt)

/* --------------- Scalar Accumulator which maintains deviation sum --------------- */

typedef struct
{
   S32 acc;
   U16 cnt;
   S32 devSum;
} S_AccDev;

PUBLIC void AccDev_Add(S_AccDev *a, S16 n);
PUBLIC void AccDev_Clear(S_AccDev *a);
PUBLIC S16  AccDev_Read(S_AccDev *a);
PUBLIC S16  AccDev_MeanDev(S_AccDev *a);
PUBLIC S16 AccDev_DevSum(S_AccDev *a);
PUBLIC S16  AccDev_ReadAndClear(S_AccDev *a);
#define _AccDev_Cnt(a)  ((a).cnt)


/* ------------------- 64 & 128bit, unsigned. Add and Subtract ----------------------- */

typedef struct { S32 hi; U32 lo; } S64c;
typedef struct { U32 hi, lo; } U64c;
typedef struct { U64c n; U8 cy; } U64Cy;


PUBLIC BOOL aEQb_U64(U64c a, U64c b);
PUBLIC U64c  NextU64(U64c n);
PUBLIC U64c  PrevU64(U64c n);
PUBLIC BOOL aGTb_U64(U64c a, U64c b);
PUBLIC BOOL aGTEb_U64(U64c a, U64c b);
PUBLIC U64c  AplusB_U32toU64(U32 a, U32 b);
PUBLIC U64c  AplusB_U64(U64c a, U64c b);
PUBLIC U64c  U32toU64(U32 n);
PUBLIC U64Cy AplusBCy_U64(U64c a, U64c b);
PUBLIC U64Cy AminusBCy_U64(U64c a, U64c b);
PUBLIC U64c  AminusB_U64(U64c a, U64c b);
PUBLIC U64c  DoubleToU64(double n);
PUBLIC double U64toDouble(U64c n);


typedef struct { S64c hi; U64c lo; } S128;
typedef struct { U64c hi, lo; } U128;
typedef struct { U128 n; U8 cy; } U128Cy;

PUBLIC BOOL aEQb_U128(U128 a, U128 b);
PUBLIC U128 Next_U128(U128 n);
PUBLIC U128 Prev_U128(U128 n);
PUBLIC BOOL aGTb_U128(U128 a, U128 b);
PUBLIC U128 AplusB_U64toU128(U64c a, U64c b);
PUBLIC U128 U64toU128(U64c n);
PUBLIC U128Cy AplusBCy_U128(U128 a, U128 b);
PUBLIC U128 AplusB_U128(U128 a, U128 b);
PUBLIC U128Cy AminusBCy_U128(U128 a, U128 b);
PUBLIC U128 AminusB_U128(U128 a, U128 b);
PUBLIC U128 DoubleToU128(double n);
PUBLIC double U128toDouble(U128 n);

#endif	/* ARITH_H */

// -------------------------------- eof -----------------------------------

