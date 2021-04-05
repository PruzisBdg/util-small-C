/* Logs mallocs() and frees() and checks that they balance.

   Because mallinfo() isn't available on many platforms including Windows.
*/
#include "libs_support.h"
#include "string.h"
#include "util.h"
#include "arith.h"
#include <stdio.h>

#define _LogSize 100

// ---------------------------------------------------------------------------------------
PUBLIC void heapLog_Make(heapLog_S *l) {
   l->mallocs.bs = calloc(_LogSize, sizeof(heapLog_Item));
   l->mallocs.put = 0;
   l->frees.bs = calloc(_LogSize, sizeof(heapLog_Item));
   l->frees.put = 0;
   l->bytesMalloced = 0;
}

// ---------------------------------------------------------------------------------------
PUBLIC void heapLog_Delete(heapLog_S *l) {
   free(l->mallocs.bs);
   free(l->frees.bs); }

// ---------------------------------------------------------------------------------------
static void add(heapLog_Items *ls, void *p, size_t n) {
   ls->bs[ls->put].at = p;
   ls->bs[ls->put++].size = n; }

// ---------------------------------------------------------------------------------------
PUBLIC void *heapLog_Malloc(heapLog_S *l, size_t n) {
   void * p = malloc(n);
   add(&l->mallocs, p, n);
   l->bytesMalloced += n;
   return p; }

// ---------------------------------------------------------------------------------------
PUBLIC void heapLog_Free(heapLog_S *l, void *p) {
   free(p);
   add(&l->frees, p, 0); }

// ------------------ There's a free for every malloc ------------------------------------
PUBLIC bool heapLog_Match(heapLog_S *l) {

   if(l->mallocs.put == l->frees.put) {               // Same number of mallocs() and frees()?
      U16 cnt = l->mallocs.put;
      for(U16 i = 0; i < cnt; i++)                    // For each malloc...
      {
         void *p = l->mallocs.bs[i].at;               // heap-block assigned was this
         bool paired = false;
         for(U16 j = 0; j < cnt; j++)                 // Over the frees-list...
         {
            if(p == l->frees.bs[j].at) {              // This malloced()ed 'p' was also free()ed?
               paired = true;                         // then this 'p' was correclty paired.
               break; }
         }                                            // So match; break to check next malloc().
         if(paired == false) {                        // 'p' was malloc()ed but not free()ed?
            return false; }                           // then fail.
      }
      return true; }          // There was a free() for every malloc(). Success.
   return false;              // Some fail above.
}

// ---------------------------------------------------------------------------------------
PUBLIC C8 const * heapLog_Report(C8 *out, heapLog_S_Stats* stats, heapLog_S *l) {
   if(out != NULL) {
      sprintf(out, "....... mallocs %u frees %u bytes malloced %lu", l->mallocs.put, l->frees.put, l->bytesMalloced); }
   else {
      return "NULL out"; }

   if(stats != NULL) {
      stats->mallocs = l->mallocs.put;
      stats->frees = l->frees.put; }
   return out;
}

/* -------------------------------------- eof ------------------------------------------- */
