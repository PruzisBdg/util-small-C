/* ---------------------------------------------------------------------------------------------

   A stack of void* Grows up (from 'base').

   Minor points:
      a) A zero-sized stack is legal. Just doesn't hold anything.

      b) NULL is used to signal a failure to push() and pop(). But you can push(NULL) if you wish.
         Why you would want to - but it's up to you if you wish to do and handle this.
*/

#include "libs_support.h"
#include "util.h"
#include <string.h>

// -----------------------------------------------------------------------------------------------
PUBLIC bool PtrStack_Init(S_PtrStack *stk, void **voidsBuf, U8 size)
{
   if(voidsBuf == NULL) {        // No buffer?
      return false; }            // fail
   else {
      stk->base = voidsBuf;      // else attach 'buf'
      stk->size = size;          // Set size; (A zero-size stack is legal; can't store anything on it but... hey!)
      stk->put = 0;              // Starts empty.
   return true; }
}

// -----------------------------------------------------------------------------------------------
PUBLIC bool PtrStack_Push(S_PtrStack *stk, void *p)
{
   if( stk->base == NULL || stk->size == 0 ||      // No/zero-size stack? OR
       stk->put >= stk->size) {                    // is full?
      return false; }                              // Denied!!
   else {
      stk->base[stk->put++] = p;                   // else push 'p'.
      return true; }
}

// -----------------------------------------------------------------------------------------------
PUBLIC void* PtrStack_Pop(S_PtrStack *stk)
{
   if( stk->base == NULL || stk->size == 0 ||      // No/zero-size stack? OR
       stk->put == 0) {                            // is empty?
      return NULL; }                               // then got nothing
   else {
      return stk->base[--stk->put]; }              // else pop.
}

// -----------------------------------------------------------------------------------------------
PUBLIC void PtrStack_Flush(S_PtrStack *stk)
{
   if(stk->base != NULL) {
      memset(stk->base, 0, stk->size * sizeof(void*)); }    // Be tidy (for debug at least). Clean off the actual stack.
   stk->put = 0;                                            // It's empty.
}

// -----------------------------------------------------------------------------------------------
PUBLIC void* PtrStack_Top(S_PtrStack *stk)
{
   if( stk->base == NULL || stk->size == 0 ||      // No/zero-size stack? OR
       stk->put == 0) {                            // is empty?
      return NULL; }                               // then got nothing
   else {
      return stk->base[stk->put-1]; }              // else return top.
}

// -----------------------------------------------------------------------------------------------
PUBLIC U8 PtrStack_Cnt(S_PtrStack *stk)
   { return stk->put; }

// -----------------------------------------------------------------------------------------------
PUBLIC U8 PtrStack_Free(S_PtrStack *stk)
   { return stk->size == 0 ? 0 : stk->size - stk->put; }


// ------------------------------------------------------- eof -----------------------------------------------------
