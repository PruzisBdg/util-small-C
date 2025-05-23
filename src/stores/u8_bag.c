/* ------------------------------------------------------------------------------
|
| Bag holding unique unsigned chars. The store is 8 x 32bit words.
|
| Public:
|     U8bag_Clear()
|     U8bag_AddRange()
|     U8bag_RemoveRange()
|     U8bag_AddOne()
|     U8bag_RemoveOne()
|     U8bag_Invert()
|
|
|
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

#define _U8bag_NumLines (((U16)MAX_U8+1)/(8 *sizeof(U8bag_T_Line)))

#define _LineAllOnes    (MAX_U32)
#define _LineAllZeros   (0)
#define _BitsPerLine    (8*sizeof(U8bag_T_Line))

/* ------------------------------ U8bag_Clear ------------------------------- */

PUBLIC void U8bag_Clear(S_U8bag *m)
   { memset(m, 0, sizeof(S_U8bag)); }

/* ------------------------------ U8bag_Copy ------------------------------- */

PUBLIC void U8bag_Copy(S_U8bag *dest, S_U8bag const *src)
   { memcpy(dest, src, sizeof(S_U8bag)); }

/* --------------------------------- U8bag_Add/RemoveRange ------------------------------- */

PRIVATE void addRemove(S_U8bag *m, U8 from, U8 to, BOOL add)
{
   U8 ln, lsb;
   ln = from / _BitsPerLine;
   lsb = from - (ln * _BitsPerLine);

   while(to >= from)
   {
      U8 bits = MinU8(to-from+1, _BitsPerLine-lsb);

      if(bits >= _BitsPerLine)
         { m->lines[ln] = add == TRUE ? _LineAllOnes : _LineAllZeros; }
      else
      {
         if(add == TRUE)
            { m->lines[ln] |= ( ((1L << bits)-1) << lsb); }
         else
            { m->lines[ln] &= ~( ((1L << bits)-1) << lsb); }
      }
      lsb = 0;

      if(++ln >= _U8bag_NumLines)
         break;
      else
         from = _BitsPerLine * (U16)ln;
   }
}

PUBLIC void U8bag_AddRange(S_U8bag *m, U8 from, U8 to)
   { addRemove(m, from, to, TRUE); }

PUBLIC void U8bag_RemoveRange(S_U8bag *m, U8 from, U8 to)
   { addRemove(m, from, to, FALSE); }

/* --------------------------------- U8bag_Add/RemoveOne ------------------------------- */

PRIVATE void addRemoveOne(S_U8bag *m, U8 n, BOOL add)
{
   U8 ln = n / _BitsPerLine;
   U8 bt = n - (ln * _BitsPerLine);

   if(add == TRUE)
      { m->lines[ln] |= (U32)(1L << bt); }
   else
      { m->lines[ln] &= ~(U32)(1L << bt); }
}

PUBLIC void U8bag_AddOne(S_U8bag *m, U8 n)
   { addRemoveOne(m, n, TRUE); }

PUBLIC void U8bag_RemoveOne(S_U8bag *m, U8 n)
   { addRemoveOne(m, n, FALSE); }

/* --------------------------------- U8bag_Invert ----------------------------------- */

PUBLIC void U8bag_Invert(S_U8bag *m)
{
   U8 c;
   for(c = 0; c < _U8bag_NumLines; c++)
      { m->lines[c] = ~m->lines[c]; }
}

/* ------------------------------ U8bag_Contains ------------------------------- */

PUBLIC BOOL U8bag_Contains(S_U8bag const *m, U8 n)
{
   U8 ln, lsb;

   ln = n / _BitsPerLine;
   lsb = n - (ln * _BitsPerLine);
   return (m->lines[ln] & (1UL << lsb)) > 0 ? TRUE : FALSE;
}

/* ------------------------------ U8bag_sEqual ------------------------------- */

PUBLIC bool U8bag_sEqual(S_U8bag const *a, S_U8bag const *b)
{
   for(U8 i = 0; i < _U8bag_NumLines; i++) {
      if(a->lines[i] != b->lines[i]) {
         return false; }}
   return true;
}


/* ------------------------------ U8bag_Print ------------------------------- */

PUBLIC void U8bag_Print(S_U8bag const *m)
{
   if(sizeof(m->lines[0]) == sizeof(U32)) {
      U8 i;
      for(i = 0; i < _U8bag_NumLines; i++) {
         printf("  %d: 0x%lx\r\n", i, (unsigned long)m->lines[i]); }}

}

#define TAB 0x09
#define CR  0x0D
#define LF  0x0A

/* ---------------------------------- U8bag_ListCh -----------------------------

   List (printables) as a string. Note that 0 (zero) cannot be in the bag; otherwise the
   list will be the empty string.
*/
PUBLIC C8 * U8bag_ListCh(C8 *out, S_U8bag const *m)
{
   U16 i, c;
   for(c = 0, i = 0; c < 256; c++)
   {
      if(U8bag_Contains(m, c))
      {
         switch(c)
         {
            case CR:  out[i++] = '\\'; out[i++] = 'r'; break;
            case LF:  out[i++] = '\\'; out[i++] = 'n'; break;
            case TAB: out[i++] = '\\'; out[i++] = 't'; break;
            default: out[i++] = c;
         }
      }
   }
   out[i] = '\0';
   return out;
}

/* ----------------------------------- U8bag_List ---------------------------------- */

PUBLIC C8 * U8bag_List(C8 *out, S_U8bag const *m, U8 *cnt)
{
   U16 i, c;
   for(c = 0, i = 0; c < 256; c++)
   {
      if(U8bag_Contains(m, c))
         { out[i++] = c; }
   }
   *cnt = i;         // Return number of items which were in 'm' and are now in 'out'.
   return out;
}

// -------------------------------- end ----------------------------------------
