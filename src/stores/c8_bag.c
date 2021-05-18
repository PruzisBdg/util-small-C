/* ------------------------------------------------------------------------------
|
| Bag holding unique unsigned chars. The store is 4 x 32bit words.
|
| Public:
|     C8bag_Clear()
|     C8bag_AddRange()
|     C8bag_RemoveRange()
|     C8bag_AddOne()
|     C8bag_RemoveOne()
|     C8bag_Invert()
|     C8bag_Contains()
|     C8bag_Print()
|     C8bag_List()
|
--------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "libs_support.h"
#include "util.h"
#include "arith.h"

#define _LineAllOnes    (MAX_U32)
#define _LineAllZeros   (0)
#define _BitsPerLine    (8*sizeof(C8bag_T_Line))

/* ------------------------------ C8bag_Clear ------------------------------- */

PUBLIC void C8bag_Clear(S_C8bag *m)
   { memset(m, 0, sizeof(S_C8bag)); }

/* --------------------------------- C8bag_Add/RemoveRange ------------------------------- */

PRIVATE BOOL addRemove(S_C8bag *m, C8 from, C8 to, BOOL add)
{
   if(from >= 0 && to >= 0 && to >= from)  // C8 is signed; 'from' and 'to' are 0..7F?
   {
      U8 ln, lsb;
      ln = from / _BitsPerLine;
      lsb = from - (ln * _BitsPerLine);

      while(to >= from)
      {
         U8 bits = MinU8(to-from+1, _BitsPerLine-lsb);

         if(bits >= _BitsPerLine)
            { m->lines[ln] = (add == TRUE) ? _LineAllOnes : _LineAllZeros; }
         else
         {
            if(add == TRUE)
               { m->lines[ln] |= ( ((1L << bits)-1) << lsb); }
            else
               { m->lines[ln] &= ~( ((1L << bits)-1) << lsb); }
         }
         lsb = 0;

         if(++ln >= _C8bag_NumLines)
            break;
         else
            from = _BitsPerLine * (U16)ln;
      }
      return TRUE;
   }
   return FALSE;
}

PUBLIC BOOL C8bag_AddRange(S_C8bag *m, C8 from, C8 to)
   { return addRemove(m, from, to, TRUE); }

PUBLIC BOOL C8bag_RemoveRange(S_C8bag *m, C8 from, C8 to)
   { return addRemove(m, from, to, FALSE); }

/* --------------------------------- C8bag_Add/RemoveCh ------------------------------- */

PRIVATE void addRemoveOne(S_C8bag *m, C8 n, BOOL add)
{
   if(n >= 0)          // C8 is signed; 'n' is 0..7F?
   {
      U8 ln = n / _BitsPerLine;
      U8 bt = n - (ln * _BitsPerLine);

      if(add == TRUE)
         { m->lines[ln] |= (U32)(1L << bt); }
      else
         { m->lines[ln] &= ~(U32)(1L << bt); }
   }
}

PUBLIC void C8bag_AddOne(S_C8bag *m, C8 n)
   { addRemoveOne(m, n, TRUE); }

PUBLIC void C8bag_RemoveOne(S_C8bag *m, C8 n)
   { addRemoveOne(m, n, FALSE); }

/* --------------------------------- C8bag_Invert ----------------------------------- */

PUBLIC void C8bag_Invert(S_C8bag *m)
{
   U8 c;
   for(c = 0; c < _C8bag_NumLines; c++)
      { m->lines[c] = ~m->lines[c]; }
}

/* ------------------------------ C8bag_Contains ------------------------------- */

PUBLIC BOOL C8bag_Contains(S_C8bag const *m, C8 n)
{
   if(n < 0)          // C8 is signed; 'n' is 0..7F?
   {
      return FALSE;
   }
   else
   {
      U8 ln, lsb;

      ln = n / _BitsPerLine;
      lsb = n - (ln * _BitsPerLine);
      return (m->lines[ln] & (1UL << lsb)) > 0 ? TRUE : FALSE;
   }
}

/* ------------------------------ C8bag_Print ------------------------------- */

PUBLIC void C8bag_Print(S_C8bag const *m)
{
   if(sizeof(m->lines[0]) == sizeof(U32)) {
      U8 i;
      for(i = 0; i < _C8bag_NumLines; i++) {
         printf("  %d: 0x08%lX\r\n", i, m->lines[i]); }}

}

/* ------------------------------ C8bag_PrintLine -------------------------------

   Prints '[0x12345678 087654321 ...]'
*/
PUBLIC C8 const * C8bag_PrintLine(C8 *out, S_C8bag const *m)
{
   if(sizeof(m->lines[0]) == sizeof(U32)) {
      U8 i; C8 *prntAt;
      for(i = 0, prntAt = out; i < _C8bag_NumLines; i++) {
         prntAt += sprintf(prntAt,
            i == 0
               ? "[0x%08lX"                  // Start with left-bracket
               : (i == _C8bag_NumLines-1
                  ? " %08lX]"                 // End with right-bracket.
                  : " %08lX"),
            m->lines[i]); }
      return out; }
   return strcpy(out, "[C8Bag print error]");
}

/* ------------------------------ C8bag_Count ------------------------------- */

PUBLIC U16 C8bag_Count(S_C8bag const *m)
{
   U8 i; U16 cnt;
   for(i = 0, cnt = 0; i < _C8bag_NumLines; i++) {
      cnt += NumBitsSet_U32(m->lines[i]); }
   return cnt;
}

#define TAB 0x09
#define CR  0x0D
#define LF  0x0A

/* ---------------------------------- C8bag_List ----------------------------- */

PUBLIC C8 * C8bag_List(C8 *buf, S_C8bag const *m)
{
   U16 i, c;
   for(c = 0, i = 0; c <= MAX_C8; c++)
   {
      if(C8bag_Contains(m, c))
      {
         switch(c)
         {
            case CR:  buf[i++] = '\\'; buf[i++] = 'r'; break;
            case LF:  buf[i++] = '\\'; buf[i++] = 'n'; break;
            case TAB: buf[i++] = '\\'; buf[i++] = 't'; break;
            default: buf[i++] = c;
         }
      }
   }
   buf[i] = '\0';
   return buf;
}

/* ---------------------------------- C8bag_Equal ----------------------------- */

PUBLIC BOOL C8bag_Equal(S_C8bag const *a, S_C8bag const *b)
{
   return memcmp(a, b, sizeof(S_C8bag)) == 0 ? TRUE : FALSE;
}

/* ---------------------------------- C8bag_ListInv ----------------------------- */

PUBLIC C8 * C8bag_ListInv(C8 *buf, S_C8bag const *m)
{
   U16 i, c;
   for(c = 0, i = 0; c <= MAX_C8; c++)
   {
      if(C8bag_Contains(m, c) == FALSE)
      {
         switch(c)
         {
            case CR:  buf[i++] = '\\'; buf[i++] = 'r'; break;
            case LF:  buf[i++] = '\\'; buf[i++] = 'n'; break;
            case TAB: buf[i++] = '\\'; buf[i++] = 't'; break;
            default: buf[i++] = c;
         }
      }
   }
   buf[i] = '\0';
   return buf;
}

// -------------------------------- end ----------------------------------------
