/* -------------------------------------------------------------------------
|
|                             Utility Library
|
|  All these functions are in the 'util' library.
|
----------------------------------------------------------------------------- */

#ifndef UTIL_H
#define UTIL_H

#include "libs_support.h"
#include "bit_field.h"

/* ------------------------- On Ints ------------------------------- */

PUBLIC void FillIntsU8( S16 RAM_IS *a, S16 n, U8 cnt );
PUBLIC void CopyIntsU8( S16 RAM_IS *dest, S16 const  *src, U8 cnt );
PUBLIC void CopyConstIntsU8( S16 RAM_IS *dest, S16 CONST *src, U8 cnt );
PUBLIC S32 SumIntsU8( S16 RAM_IS *a, U8 cnt );

/* ------------------------- On bytes -------------------------------- */

PUBLIC void CopyBytesU8( U8 RAM_IS *src, U8 RAM_IS *dest, U8 cnt );
#define CopyRBytesU8(s,d,c)  CopyBytesU8((s),(d),(c))
PUBLIC void CopyLBytesU8( U8 RAM_IS *dest, U8 RAM_IS *src, U8 cnt );
PUBLIC void CopyConstBytesU8( U8 CONST *src, U8 RAM_IS *dest, U8 cnt );
#define CopyRConstBytesU8(s,d,c)  CopyConstBytesU8((s),(d),(c))
PUBLIC void CopyLConstBytesU8(U8 RAM_IS *dest, U8 CONST *src, U8 cnt );

PUBLIC void CopyBytesU8_Reversed( U8 RAM_IS *dest, U8 RAM_IS *src, U8 cnt );
PUBLIC void CopyConstBytesU8_Reversed( U8 RAM_IS *dest, U8 CONST *src, U8 cnt );

PUBLIC void FillBytesU8( U8  RAM_IS *a, U8 n, U8 cnt );
PUBLIC void ZeroBytesU8(void RAM_IS *p, U8 cnt);
PUBLIC U16 SumBytesU8( U8 RAM_IS *a, U8 cnt );
PUBLIC void SwapBytesU8(void RAM_IS *a, void RAM_IS *b, U8 cnt);

/* ------------------------- On floats -------------------------------- */

PUBLIC void FillFloatsU8( float RAM_IS *a, float n, U8 cnt );

/* ------------------------- Hex ASCII -------------------------------- */

PUBLIC BIT  IsHexASCII(U8 ch);
PUBLIC U8   HexToNibble(U8 ch);

PUBLIC U8   LowNibbleToHexASCII(U8 n);
PUBLIC U8   HighNibbleToHexASCII(U8 n);
PUBLIC U8   NibbleToHexASCII(U8 n);

// Macro versions of the above, for constants
#define _NibbleToHexASCII(n)  ((n) < 10 ? (n) + '0' : ((n) <= 0x0F ? (n)-10+'A' : 0))
#define _HighNibbleToHexASCII(n)  _NibbleToHexASCII( HIGH_NIBBLE((n)) )
#define _LowNibbleToHexASCII(n)  _NibbleToHexASCII( LOW_NIBBLE((n)) )

// To construct HexASCII strings
PUBLIC void ByteToHexASCII(C8 *hex, U8 n);
PUBLIC U8   HexASCIItoByte(C8 const *hex);
PUBLIC void IntToHexASCII_Space(C8 *out, S16 n);
PUBLIC void IntToHexASCII_NoSpace(C8 *out, S16 n);

// To parse HexASCII strings
PUBLIC C8 const* GetNextHexASCIIByte(C8 const *hexStr, U8 *out);
PUBLIC C8 const* GetNextHexASCIIInt(C8 const *hexStr, S16 *out);
PUBLIC C8 const* GetNextHexASCII_U16(C8 const *hexStr, U16 *out);
PUBLIC C8 const* GetNextHexASCII_U24(C8 const *hexStr, U32 *out);
PUBLIC C8 const* GetNextHexASCII_U32(C8 const *hexStr, U32 *out);
PUBLIC C8 const* GetNextHexASCII_U8toU32(C8 const *hexStr, U32 *out, U8 *bytesGot);
PUBLIC C8 const* GetNextHexASCIIByte_Lenient(C8 const *hexStr, C8 *out);

PUBLIC U16 HexASCII_ToU16(U8 const *hexStr);
PUBLIC U32 HexASCII_ToU32(U8 const *hexStr);

// -------------------------- ACSII to Number Parsers ------------------------

// A multi-type 32-bit number;
typedef struct {
    union { float asFloat; S32 asS32; U32 asU32; } num;
    bool gotInt;
    bool gotUnsigned;
    bool reqFloat;
} T_FloatOrInt;

PUBLIC U8 const * ReadASCIIToNum(U8 const *inTxt, T_FloatOrInt *out);
PUBLIC U8 const * ReadASCIIToFloat(U8 const *inTxt, float *out);
PUBLIC BIT        GotFloatFromASCII(U8 const *inTxt, float *out);
PUBLIC C8 const * ReadDirtyASCIIInt(C8 const *inTxt, S16 *out);
PUBLIC C8 const * ReadDirtyASCII_S32(C8 const *inTxt, S32 *out);
PUBLIC C8 const * ReadAsciiS32(C8 const *inTxt, S32 *out);
PUBLIC U8 const * ReadDirtyBinaryWord(U8 const *inTxt, U16 *out);
PUBLIC C8 const * ReadTaggedASCIIInt(C8 const *in, C8 const *tag, S32 *out, C8 const *delimiters);


// Is fed one char at a time.
PUBLIC void ReadDirtyASCIIInt_ByCh_Init(void);
PUBLIC BIT  ReadDirtyASCIIInt_ByCh(U8 ch, S16 *out);

/* ------------------------------- Endian/Alignment conversions ---------------------- */

#define _ToU16_BigEnd(b)  (((U16)(*(b)) << 8) + *((b)+1))
#define _ToU32_BigEnd(b)  (((U32)_ToU16_BigEnd(b) << 16) + _ToU16_BigEnd((b)+2))

#define _ToU16_WeeEnd(b)  (((U16)(*((b)+1)) << 8) + *(b))
#define _ToU32_WeeEnd(b)  (((U32)_ToU16_WeeEnd((b)+2) << 16) + _ToU16_WeeEnd(b))

typedef enum { eNoEndian = 0, eLittleEndian, eBigEndian } E_EndianIs;
static inline bool isEndian(E_EndianIs e) { return e != eNoEndian ? true : false; }

// To LE, non-aligned.
PUBLIC void u16ToLE(U8 *out, U16 n);
PUBLIC void s16ToLE(U8 *out, S16 n);
PUBLIC U8 * u32ToLE(U8 *out, U32 n);
PUBLIC void s32ToLE(U8 *out, S32 n);
PUBLIC void u64ToLE(U8 *out, U64 n);
PUBLIC void s64ToLE(U8 *out, S64 n);
PUBLIC U16 leToU16(U8 const * src);
PUBLIC U32 leToU32(U8 const *src);
PUBLIC U64 leToU64(U8 const *src);

// To BE, non-aligned.
PUBLIC void u16ToBE(U8 *out, U16 n);
PUBLIC void s16ToBE(U8 *out, S16 n);
PUBLIC void u32ToBE(U8 *out, U32 n);
PUBLIC void s32ToBE(U8 *out, S32 n);
PUBLIC void u64ToBE(U8 *out, U64 n);
PUBLIC void s64ToBE(U8 *out, S64 n);
PUBLIC U16 beToU16(U8 const * src);
PUBLIC U32 beToU32(U8 const *src);
PUBLIC U64 beToU64(U8 const *src);

PUBLIC U16 ReverseU16(U16 n);
PUBLIC U32 ReverseU32(U32 n);

// Convert to system endian.
PUBLIC U16 ToSysEndian_U16(U16 n, E_EndianIs e);
PUBLIC U32 ToSysEndian_U32(U32 n, E_EndianIs e);

// Endian-aware iterator.
typedef struct T_EndianPtr T_EndianPtr;

struct T_EndianPtr {
   U8* (*next)(T_EndianPtr*);    // returns '_at', iterated.
   U8 const * _at;               // Current ptr.
   };

PUBLIC U8* EndianPtr_New(T_EndianPtr *ep, U8 const *bufStart, U16 nbytes, E_EndianIs bitFieldEndian);
PUBLIC C8 const * PrintEndian(E_EndianIs e);

/* ----------------------- Basic byte queue ------------------------------- */

typedef struct {
   U8 *buf,
       size,
       put,
       get,
       cnt,
       locked;
} S_byteQ;

PUBLIC void byteQ_Init  ( S_byteQ *q, U8 *buf, U8 size);
PUBLIC BOOL byteQ_Exists(S_byteQ *q);
PUBLIC void byteQ_Flush ( S_byteQ *q);
PUBLIC BIT  byteQ_Write ( S_byteQ *q, U8 const *src, U8 bytesToWrite);
PUBLIC BIT  byteQ_Read  ( S_byteQ *q, U8 *dest,      U8 bytesToRead);
PUBLIC BIT  byteQ_Locked( S_byteQ *q);
PUBLIC U8   byteQ_Count ( S_byteQ *q);
PUBLIC U8   byteQ_Size  ( S_byteQ *q);
PUBLIC U8 * byteQ_ToFill( S_byteQ *q, U8 cnt);
PUBLIC void byteQ_Unlock(S_byteQ *q);

/* ----------------------- Linear byte buffer ------------------------------------

   Unlike byteQ_, Read() does NOT remove from buffer. So no wrap.
*/
typedef struct {
   U8 *buf,
       size,
       get,
       cnt,
       locked;
} S_byteBuf;

PUBLIC void byteBuf_Init  ( S_byteBuf *b, U8 *buf, U8 size);
PUBLIC U8 * byteBuf_Start (S_byteBuf *b);
PUBLIC BOOL byteBuf_Exists( S_byteBuf *b);
PUBLIC void byteBuf_Flush ( S_byteBuf *b);
PUBLIC BIT  byteBuf_Write ( S_byteBuf *b, U8 const *src, U8 bytesToWrite);
PUBLIC BIT  byteBuf_Read  ( S_byteBuf *b, U8 *dest, U8 bytesToRead);
PUBLIC BIT  byteBuf_ReadAt( S_byteBuf *b, U8 *dest, U8 from, U8 bytesToRead );
PUBLIC BIT  byteBuf_Locked( S_byteBuf *b);
PUBLIC U8   byteBuf_Count ( S_byteBuf *b);
PUBLIC U8   byteBuf_Size  ( S_byteBuf *b);
PUBLIC U8   byteBuf_Free  (S_byteBuf *b);
PUBLIC void byteBuf_Unlock(S_byteBuf *b);

// Direct acces to buffer.
PUBLIC U8 * byteBuf_PutAt(S_byteBuf *b);
PUBLIC U8 * byteBuf_Reserve(S_byteBuf *b, U8 cnt);
PUBLIC U8 * byteBuf_ToFill( S_byteBuf *b, U8 cnt);
PUBLIC U8 * byteBuf_Start(S_byteBuf *b);


// Wrapper for a for() loop.
PUBLIC void RepeatIdxFunc( void( *func)(U8), U8 repeatCnt);

/* ---------------------------- void* Stack ------------------------------ */

typedef struct { void **base; U8 size, put; } S_PtrStack;

PUBLIC bool  PtrStack_Init(S_PtrStack *stk, void **voidsBuf, U8 size);
PUBLIC bool  PtrStack_Push(S_PtrStack *stk, void *p);
PUBLIC void* PtrStack_Pop(S_PtrStack *stk);
PUBLIC void* PtrStack_Top(S_PtrStack *stk);
PUBLIC void  PtrStack_Flush(S_PtrStack *stk);
PUBLIC U8    PtrStack_Cnt(S_PtrStack *stk);
PUBLIC U8    PtrStack_Free(S_PtrStack *stk);

/* ---------------------------- Bits ---------------------------------- */
PUBLIC U8 IdxToLsbSet_U8(U8 n);

PUBLIC U8 NumBitsSet_U16(U16 n);
PUBLIC U8 IdxToLsbSet_U16(U16 n);

PUBLIC U8 NumBitsSet_U32(U32 n);
PUBLIC U8 IdxToLsbSet_U32(U32 n);
PUBLIC U8 SWAR32(U32 n);            // SWAR = Sideways addition. Faster than NumBitsSet_U32().

PUBLIC U8  MakeAtoBSet_U8 (U8 msb, U8 lsb);
PUBLIC U16 MakeAtoBSet_U16(U8 msb, U8 lsb);
PUBLIC U32 MakeAtoBSet_U32(U8 msb, U8 lsb);

/* ---------------------------------- BCD ------------------------------------------- */
PUBLIC bool BCD2_LE( U8 *out, U8 n );
PUBLIC bool BCD4_LE(U8 *out, U16 n);
PUBLIC bool BCD6_LE(U8 *out, U32 n);
PUBLIC bool BCD8_LE(U8 *out, U32 n);
PUBLIC bool BCD12_LE(U8 *out, U64 n);
PUBLIC bool BCD16_LE(U8 *out, U64 n);

/* ------------------------ Bit field, 64kbit. LE and BE bit-orders. ------------------------- */
typedef U16 T_bit64K;

PUBLIC T_bit64K   bit64K_MakeLE(U16 _byte, U8 _bit);
PUBLIC U8         bit64K_BitLE(T_bit64K bf);

PUBLIC T_bit64K   bit64K_MakeBE(U16 _byte, U8 _bit);
PUBLIC U8         bit64K_BitBE(T_bit64K bf);

PUBLIC U16        bit64K_Byte(T_bit64K bf);
PUBLIC T_bit64K   bit64K_AddBits(T_bit64K src, S16 nbits);     // Signed, so may subtract addresses.
PUBLIC T_bit64K   bit64K_AddBytes(T_bit64K src, S16 bytes);
PUBLIC T_bit64K   bit64K_Add(T_bit64K a, T_bit64K b);

// ---- bit64K_Copy() source and destination ports.
typedef U16             bit64K_atByte;       // UP to 13 bit's for this; (3 for the bits in a byte)
typedef bit64K_atByte   bit64K_byteCnt;      // The same type which specifies a byte.
typedef U16             bit64K_T_Cnt;        // Enumerates bit addresses.

// Read / write to a logical byte-address. (Which is usually a memory-mapped peripheral over serial comms).
// These return false if an operation fails.
typedef bool bit64K_Rds(U8 *to,       bit64K_atByte from, bit64K_byteCnt cnt);
typedef bool bit64K_Wrs(bit64K_atByte to, U8 const *from, bit64K_byteCnt cnt);

typedef union {
   // Source bit-address limits for bit64K_Out(); destination bit-address limits for bit64K_In(). Ignored if both zero.
   struct { T_bit64K min, max; } bits;
   } bit64K_Range;

typedef struct {
   S_byteBuf      q;          // Use this 'S_byteBuf' for the cache.
   bit64K_atByte  atByte;     // the byte of the bit-addr of the 1st cache element (if it exists)
   } bit64K_Cache;

typedef struct {
   struct {
      bit64K_Rds     *get;       // When the source is a bit-field, this get()s from a logical byte address.
      bit64K_Range   range;      // legal bit-addr for get() OR if source is a buffer, max bytes to read.
      bit64K_atByte  maxOutBytes;// For bit64K_Out(), the max bytes to output; for bit64K_Out() the max bytes to read. Ignored if zero.
      } src;
   struct {
      bit64K_Rds  *rd;           // When the destination is a bit-field, to read bytes from it (so they can be modified and put back).
      bit64K_Wrs  *wr;           // When the destination is a bit-field, to write bytes to it.
      bit64K_Range range;
      } dest;
   bit64K_Cache *cache;
} bit64K_Ports;

PUBLIC bool bit64K_NewPort(bit64K_Ports *p, bit64K_Cache *cache, U8 *cacheBuf, U8 cacheBytes);
PUBLIC bool bit64K_ResetPort(bit64K_Ports *p);
PUBLIC bool bit64K_FlushCache(bit64K_Ports *p);

PUBLIC bool bit64K_Copy(bit64K_Ports const *port, T_bit64K dest, T_bit64K src, bit64K_T_Cnt numBits);
PUBLIC bool bit64K_Out(bit64K_Ports const *port, U8 *dest, T_bit64K src, bit64K_T_Cnt numBits, E_EndianIs srcEndian);
PUBLIC bool bit64K_In(bit64K_Ports const *port, T_bit64K dest, U8 const * src, bit64K_T_Cnt numBits, E_EndianIs destEndian, bool srcIsEndian);
PUBLIC bool bit64K_ParmFitsField(U8 const *parm, U8 parmBytes, bit64K_T_Cnt fieldBits, bool parmHasEndian);

/* --------------------- Time/Date, ISO8601 format -------------------------

   These functions convert between seconds and hours/minutes/seconds (HMS) and
   date/time (YMD/HMS). They can render HMS and YMD/HMS as ISO8601 strings.

   Zero seconds is midnight, New-years Eve 2000AD and spans forward 132 years to
   2132AD.
*/

/* A 32bit count of seconds. Will count 132 years */
typedef U32 T_Seconds32;
#define _Max_T_Seconds32 MAX_U32

typedef struct {
   U16 yr;           // 0000 - 9999.  0000 = 1.BC 0001 = 1.AD
   U8  mnth,         // 1 - 12 = Jan -> Dec
       day,          // 1 - 31
       hr,           // 0 - 23
       min,sec;      // 0 - 59
   } S_DateTime;

// If you want to call 'S_DateTime' this to be consistent with below.
#define S_YMDHMS  S_DateTime

/* Used when exporting this struct (to another device). Then the contents are packed, regardless
   of the format internal to the code.
*/
#define _YMDHMS_BytesPacked 7

typedef struct {U16 yr; U8 mnth, day;} S_YMD;

// Up to 9999 hrs when rendered as an ISO8601 string i.e max is "9999:59:59".
typedef struct {
   U16 hr;           // 0 - 9999
   U8 min, sec;      // 0 - 59
   } S_TimeHMS;

/* Used when exporting this struct (to another device). Then the contents are packed, regardless
   of the format internal to the code.
*/
#define _HMS_BytesPacked 4

#define _2000AD 2000    // 32bit seconds are offset from midnight, New-years eve 2000AD.

#define _HMSStr_Maxlen 10

PUBLIC U8            DaysInMonth(U16 yr, U8 month);
PUBLIC void          SecsToHMS(T_Seconds32 secs, S_TimeHMS *hms);
PUBLIC U8            SecsToHMSStr(   T_Seconds32 secsCnt, C8 *strOut);
PUBLIC U8            SecsToHMS32_Str(T_Seconds32 secsCnt, C8 *strOut);
PUBLIC U8            SecsToHrMinStr(T_Seconds32 secsCnt, C8 *strOut);
PUBLIC C8 const *    SecsToHrMinStrRtn(T_Seconds32 secsCnt, C8 *strOut);
PUBLIC C8 const *    SecsToHMSStrRtn(   T_Seconds32 secsCnt, C8 *strOut);
PUBLIC C8 const *    SecsToHMS32_StrRtn(T_Seconds32 secsCnt, C8 *strOut);
PUBLIC BOOLEAN       YMDHMS_Equal(S_DateTime const *a, S_DateTime const *b);

PUBLIC void          SecsToYMDHMS(T_Seconds32 secsSince2000AD, S_DateTime *dt);
PUBLIC C8 *          SecsTo_YMDHMS_Str(C8 *strOut, T_Seconds32 secs);
PUBLIC BOOL          ISO8601StrToSecs( C8 const *dateStr, T_Seconds32 *absTimeOut );
PUBLIC T_Seconds32   YMDHMS_To_Secs(S_DateTime const *dt);
PUBLIC BOOL          Legal_YMDHMS(S_DateTime const *t);
PUBLIC BOOLEAN       Legal_YMD(S_YMD const *t);
PUBLIC BOOLEAN       Legal_HMS(S_TimeHMS const *t);
PUBLIC U8            YMDHMS_ToStr(S_DateTime const *t, C8 *outStr);
PUBLIC U8            YMD_ToStr(S_YMD const *t, C8 *outStr);
PUBLIC U8            HMS_ToStr(S_TimeHMS const *t, C8 *outStr);
PUBLIC T_Seconds32   EpochTo2000AD(T_Seconds32 epochSecs);
PUBLIC T_Seconds32   AD2000toEpoch(T_Seconds32 secsSince2000AD);

#define _12am_Jan_1st_2000_Epoch_secs 946684800UL


extern S16 const     DaysToMonthStartTbl[];

#define _ISO8601_YMDHMS_MaxStr (sizeof("2018-03-06T20:28:44") + 2)
#define _ISO8601_YMDHMS_AsSpaces       "                   "

#define _HMS_MaxStr   (sizeof("65535:59:59") + 2)
#define _ISO8601_HMS_MaxSpaces "           "
#define _ISO8601_HMS_AsSpaces "        "

/* --------------------------- Pretty Prints -------------------------------------- */
PUBLIC C8 const * ArrayPrettyPrint(C8 *out, U16 outBufLen, C8 const *prefix, U8 const *src, size_t len, U16 maxLine);
PUBLIC C8 const * PrintU8s_1Line(C8 *out, U16 outBufLen, C8 const *fmt, U8 const *src, size_t numBytes);
PUBLIC C8 const * PrintS16s_1Line(C8 *out, U16 outBufLen, C8 const *fmt, S16 const *src, size_t numBytes);

/* -------------------------- Table search ----------------------------------------- */

PUBLIC void const * TblSearchSAR_AscendKeyU16(void const *tbl, U16 numRecords, U8 bytesPerRec, U16 keyToMatch);
/* --------------------------- Safe String ------------------------------------------- */

PUBLIC U16 SafeStrCopy(C8 *out, C8 const *src, U16 maxCh);
PUBLIC U16 TestStrPrintable(C8 const *str, U16 maxCh);

PUBLIC C8 * EndStr(C8 const* str);  // Spot the end of a non-const string.
PUBLIC U8 strlenU8(C8 const* str);

/* ---------------------------- S-Records -------------------------------------------*/

typedef struct
{
    U8  type;
    U8  numDataBytes;
    U16 addr;
    U8  *payload;
} S_SRecordInfo;

PUBLIC C8 const * SRecord_Next(C8 const * str);
PUBLIC C8 const * SRecord_Parse(S_SRecordInfo *r, C8 const * src);

#define _SRecType_Data              (0)
#define _SRecType_EOF               (1)
#define _SRecType_ExtendedSegment   (2)
#define _SRecType_StartSegment      (3)
#define _SRecType_ExtendedLinear    (4)
#define _SRecType_StartLinear       (5)


/* ---------------------- Bag holding unique U8s ----------------------------------- */

typedef U32 U8bag_T_Line;
#define _U8bag_NumLines (((U16)MAX_U8+1)/(8 *sizeof(U8bag_T_Line)))

typedef struct {
   U8bag_T_Line lines[_U8bag_NumLines];
} S_U8bag;

PUBLIC void U8bag_Clear       (S_U8bag *m);
PUBLIC void U8bag_Copy        (S_U8bag *dest, S_U8bag const *src);
PUBLIC void U8bag_AddRange    (S_U8bag *m, U8 from, U8 to);
PUBLIC void U8bag_RemoveRange (S_U8bag *m, U8 from, U8 to);
PUBLIC void U8bag_AddOne      (S_U8bag *m, U8 n);
PUBLIC void U8bag_RemoveOne   (S_U8bag *m, U8 n);
PUBLIC void U8bag_Invert      (S_U8bag *m);
PUBLIC bool U8bag_sEqual      (S_U8bag const *a, S_U8bag const *b);
PUBLIC BOOL U8bag_Contains    (S_U8bag const *m, U8 n);
PUBLIC void U8bag_Print       (S_U8bag const *m);
PUBLIC C8 * U8bag_List        (C8 *out, S_U8bag const *m, U8 *cnt);
PUBLIC C8 * U8bag_ListCh      (C8 *out, S_U8bag const *m);

/* ---------------------- Bag holding unique chars (C8) -----------------------------------

   Is half the size of U8bag_
*/

typedef U32 C8bag_T_Line;
#define _C8bag_NumLines (((U16)MAX_C8+1)/(8 *sizeof(C8bag_T_Line)))

typedef struct {
   C8bag_T_Line lines[_C8bag_NumLines];
} S_C8bag;

PUBLIC void C8bag_Clear       (S_C8bag *m);
PUBLIC void C8bag_AddRange    (S_C8bag *m, C8 from, C8 to);
PUBLIC void C8bag_RemoveRange (S_C8bag *m, C8 from, C8 to);
PUBLIC void C8bag_AddOne      (S_C8bag *m, C8 n);
PUBLIC void C8bag_RemoveOne   (S_C8bag *m, C8 n);
PUBLIC void C8bag_Invert      (S_C8bag *m);
PUBLIC BOOL C8bag_Contains    (S_C8bag const *m, C8 n);
PUBLIC void C8bag_Print       (S_C8bag const *m);
PUBLIC C8 * C8bag_List(C8 *buf, S_C8bag const *m);

/* ------------------------------------- Parsing - General --------------------------------------*/

typedef U16 T_BracesIdx;
typedef struct { T_BracesIdx open, payload, close; } S_BracketedAt;

typedef struct
{
    C8 const *toOpen;
    C8 const *toClose;
} S_Braces;

PUBLIC BOOL FindOuterBraces(C8 const *str, S_Braces const *b, U16 maxChars, S_BracketedAt *found);
PUBLIC C8 * BypassLeadingWhitespace(C8 *str);
PUBLIC C8 * StripTrailingWhitespace(C8 *str);

/* --------------------------- MAC address, string to number -------------------------------*/

#define _MacAddrAsBinary_bytes 6

typedef union __attribute__((packed))
{
    U8 asBytes[_MacAddrAsBinary_bytes];
    U16 asU16[(_MacAddrAsBinary_bytes+1)/2];
} U_MACasNum;

#define _MacAddr_Chars (sizeof("12:34:56:78:90:12")-1)


#define _MacAddr_DeclConst(a,b,c,d,e,f) \
    { .asBytes[0] = 0x##a, .asBytes[1] = 0x##b, .asBytes[2] = 0x##c,  .asBytes[3] = 0x##d, .asBytes[4] = 0x##e, .asBytes[5] = 0x##f }

PUBLIC BOOL macAddr_LegalStr(C8 const * macStr);
PUBLIC BOOL macAddr_StrToNum(U_MACasNum *toNum, C8 const *fromStr);
PUBLIC C8 const * macAddr_NumToStr(C8 *toStr, U_MACasNum const *fromNum);
PUBLIC BOOL macAddr_StrIsZero(C8 const * str);
PUBLIC BOOL macAddr_StrIsNonZero(C8 const * str);
PUBLIC BOOL macAddr_NumsEqual(U_MACasNum const *a, U_MACasNum const *b);

/* -------------------------- IP Address ------------------------------------------- */

typedef U32 T_IPAddrNum;
#define _MaxIPAddrChars sizeof("255.255.255.255")
PUBLIC C8 const *PrintIPAddr(C8 *out, T_IPAddrNum ip);
PUBLIC C8 const * ReadIPAddr(C8 const *src, T_IPAddrNum *ip);

/* ------------------------------ String Safety ----------------------------------------------------*/

PUBLIC BOOL IsPrintableStr_NoLongerThan(C8 const *str, U8 maxLen);

typedef S16 tSoftMatchStr;
PUBLIC tSoftMatchStr SoftMatchStr(C8 const *ref, C8 const *str, C8 const **matchAt);


/* ------------------------------ Baby Regex --------------------------------------------*/

typedef U16 RegexLT_T_Idx;

typedef struct
{
    RegexLT_T_Idx start;
    RegexLT_T_Idx end;
} RegexLT_S_Result;

typedef struct {
   void* (*getMem)(size_t);
   void  (*free)(void*);
   BOOL  printEnable;
   U8    maxSubmatches;
   U16   maxRegexLen,
         maxStrLen;
} RegexLT_S_Cfg;

typedef enum {
   E_RegexRtn_NoMatch = 0,             // Search completed but no match
   E_RegexRtn_Match,                   // Success!
   E_RegexRtn_OK = E_RegexRtn_Match,   // When compiling standalone, that compile succeeded.
   E_RegexRtn_BadExpr,                 // Regex malformed, too large etc. It failed the pre-scan
   E_RegexRtn_CompileFailed,           // Regex passed pre-scan but could not be compiled
   E_RegexRtn_BadCfg,                  // Bad/missing configuration, e.g no getMem() supplied.
   E_RegexRtn_BadInput,                // Something illegal in input string
   E_RegexRtn_OutOfMemory,             // Regex could not malloc what it needed; OR finds it didn't malloc enough.
   E_RegexRtn_RanTooLong               // Thread execution blew up; more than predicted run-steps.
} T_RegexRtn;

typedef U16 RegexLT_T_MatchIdx;
#define _RegexLT_MatchIdx_Never MAX_U16
typedef U16 RegexLT_T_MatchLen;

typedef struct {
   C8 const *at;              // Start of match in the searched string.
   RegexLT_T_MatchIdx idx;    // Index to match, relative the string[0].
   RegexLT_T_MatchLen len;    // Number of bytes matched.
} RegexLT_S_Match;

typedef struct {
   RegexLT_S_Match *matches;  // The matches.
   U8 listSize,               // the size of matches[]
   put;                       // Next free slot in matches[]. (Also the number of matches)
} RegexLT_S_MatchList;


typedef U16 RegexLT_T_Flags;
#define _RegexLT_Flags_None         0
#define _RegexLT_Flags_MatchLongest _BitM(0)
#define _RegexLT_Flags_MatchLast    _BitM(1)

PUBLIC void       RegexLT_Init        (RegexLT_S_Cfg const *cfg);
PUBLIC T_RegexRtn RegexLT_Compile     (C8 const *regexStr, void **progV);
PUBLIC T_RegexRtn RegexLT_Match       (C8 const *regex, C8 const *srcStr, RegexLT_S_MatchList **ml, RegexLT_T_Flags flags);
PUBLIC T_RegexRtn RegexLT_MatchProg   (void *prog, C8 const *srcStr, RegexLT_S_MatchList **ml, RegexLT_T_Flags flags);
PUBLIC T_RegexRtn RegexLT_Replace     (C8 const *regexStr, C8 const *ien13757_MakeTypeH_FloatnStr, C8 const *replaceStr, C8 *out);
PUBLIC T_RegexRtn RegexLT_ReplaceProg (void *prog, C8 const *inStr, C8 const *replaceStr, C8 *out);
PUBLIC C8 const * RegexLT_RtnStr      (T_RegexRtn r);
PUBLIC void       RegexLT_FreeMatches (RegexLT_S_MatchList const *ml);
PUBLIC T_RegexRtn RegexLT_FreeProgram (void *prog);

PUBLIC void       RegexLT_PrintMatchList(RegexLT_S_MatchList const *ml);
PUBLIC void       RegexLT_PrintMatchList_OnOneLine(RegexLT_S_MatchList const *ml);

/* ------------------------------ Finds HHTP Status Codes & msg -------------------------------------*/

typedef struct
{
    C8       *at;
    U16      numChars;
    U16      result;
    BOOL     found;         // TRUE if found a match.
} S_MatchedinStr;

PUBLIC BOOL FindHTTPstatusMsg(C8 const *str, S_MatchedinStr *match);

// Just the common status codes
typedef enum { eHttp_OK = 200, eHttp_BadRequest = 400,  eHttp_Unauthorised = 401, eHttp_NotFound = 404, } T_HTTPStatusCode;

/* -------------------------- 1-way Heap -------------------------------------------- */

typedef U16 Heap1w_T_Size;

typedef struct {
	void	*mem;		// Heap starts here
	Heap1w_T_Size
			size,		// Size IN BYTES.
			get,		// Next free space; as offset from 'mem'.
			align,		// Alignment for new Take()s; 1,2,4 bytes or multiple of 4.
			used,		// Number of bytes taken since Make() or Empty().
			maxReq;		// For analysis only; the max free space need during a seesion on the heap.
} Heap1w_S;

// Specifies a 1-way heap for Heap1_Make()
typedef struct {
	void	*mem;		// Memory supplied for heap is this
	Heap1w_T_Size
			size,		// and this many BYTES.
			align;		// Alignment for new Take()s. Allowed are 1,2,4 bytes or multiple of 4.
} Heap1w_S_Spec;

PUBLIC bool		Heap1w_Make(Heap1w_S *h, Heap1w_S_Spec const *spec);
PUBLIC void		Heap1w_FreeAll(Heap1w_S *h);
PUBLIC Heap1w_T_Size Heap1w_BytesFree(Heap1w_S const *h);
PUBLIC void *	Heap1w_BufAt(Heap1w_S const *h);
PUBLIC bool		Heap1w_IsInHeap(Heap1w_S const *h, void const *addr);
PUBLIC void *	Heap1w_Get(Heap1w_S *h, Heap1w_T_Size n);
PUBLIC void *	Heap1w_Take(Heap1w_S *h, Heap1w_T_Size n);
PUBLIC void		Heap1w_Return(Heap1w_S *h, Heap1w_T_Size numBytes);
PUBLIC void		Heap1w_Untake(Heap1w_S *h, Heap1w_T_Size n);
PUBLIC Heap1w_T_Size Heap1w_Used(Heap1w_S const *h);
PUBLIC Heap1w_T_Size Heap1w_Unused(Heap1w_S const *h);

// ================================= Macros ======================================================

// CAT() force expansion of 'x','y' before they are pasted. Double XCAT() forces that expansion even
// if 'x', 'y' are them selves macros. (This is an old K&R trick)
#define CAT(x,y)  x##y
#define XCAT(x,y)  CAT(x,y)

/* -------To map() over parameter lists (in various ways)

   Lots of repetition, so these get their own file.
*/
#include "map_macros.h"

#endif // UTIL_H
