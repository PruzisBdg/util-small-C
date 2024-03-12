/* ================================ IRQ Callbacks Stack =========================================

   A Stack of callbacks for Interrupt Handlers.

   To chain multiple callbacks to a collection of IRQ sources, usually from the one interrupt.

   Host may add callbacks to the Stack. When the Stack is run, Callbacks on the Stack are
   executed in the order they were pushed. A Callback gets the IRQ flags, clears any that it
   handles, and returns a reference to the (revised) flags.

*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* ------------------------------ cbStack_Init -------------------------------------------

   A zero-sized callback Stack is legal.
*/
PUBLIC bool cbStack_Init(cbStack_S *s, cbStack_Cfg const *cfg) {
   if(cfg->size > 0 && cfg->buf == NULL) {      // Non-zero size but no buffer supplied?
      return false; }                           // then fail.
   else {
      s->fs = cfg->buf;
      s->size = cfg->size;
      s->put = 0;
      return true; }}

/* --------------------------------- cbStack_Free -------------------------------------------
*/
PUBLIC U8 cbStack_Free(cbStack_S const *s) {
   return AminusBU8(s->size, s->put); }

/* --------------------------------- cbStack_Chain ------------------------------------------

   Chain 'cb' to existing callbacks
*/
   static bool isOnStack(cbStack_S *_s, cbStack_T_Callback _cb) {
      for(U8 i = 0; i < _s->size; i++) {
         if(_s->fs[i] == _cb) {
            return true; }}
      return false; }

   // --------------------------------------------

PUBLIC bool cbStack_Chain(cbStack_S *s, cbStack_T_Callback cb) {

   if(cb == NULL) {                       // NULL 'cb'?
      return true; }                      // don't add; just succeed in doing nothing.
   else if(isOnStack(s, cb)) {            // 'cb' already on Stack?
      return true; }                      // then no need to add it now. Just succeed.
   else if(s->put >= s->size) {           // Stack is full?
      return false; }                     // cannot chain 'cb'; fail.
   else {
      s->fs[s->put++] = cb;               // otherwise add 'cb' to chain.
      return true; }}

/* ------------------------------- cbStack_UnChain --------------------------------------------

   Remove 'cb' from existing callbacks chain, wherever it is
*/
PUBLIC bool cbStack_UnChain(cbStack_S *s, cbStack_T_Callback cb) {
   if(cb != NULL || s->put > 0) {                     // 'cb' is real? AND the Stack has at least on callback?
       for(U8 i = 0; i < s->put; i++) {               // For the Stack root, upwards...
         if(s->fs[i] == cb) {                         // Current Stack entry is 'cb'
            // If 'cb' was not the last entry then all others must move down one.
            for(U8 j = i; j < s->put-1; j++) {        // then from current entry, to Stack top...
               s->fs[j] = s->fs[j+1]; }               // Copy next-up into the entry below.
            s->put--;                                 // Now one entry less.
            s->fs[s->put] = NULL;                     // Not strictly necessary but... Be tidy; the end slot is now empty; NULL it.
            break; }}}           // The first 'cb' was removed. Assume there are no more so break.
    return true; }  // Always succeeds.

/* ---------------------------------- cbStack_DropLast -------------------------------------------

   Unchain (pop) the most recently-added callback.
*/
PUBLIC bool cbStack_DropLast(cbStack_S *s) {
   if(s->put > 0) {                       // At least 1 callback?
      s->put--;                           // then back up...
      s->fs[s->put] = NULL; }             // ... and erase it.
   return true; }

/* ------------------------ cbStack_Run  ---------------------------------------

   Run callbacks; from latest-added to first. Each callback gets 'irqs' and should
   clear the ones it handles.
*/
PUBLIC cbStack_Irqs cbStack_Run(cbStack_S const *s, cbStack_Irqs irqs) {
   if(s->put > 0) {                          // At least 1 callback?
      for(U8 i = s->put; i > 0; i--) {       // Walk back...
         if(s->fs[i-1] == NULL) {            // CB is not NULL? (it should not be... it was added with _Chain() )
            break; }
         else {                              // yes, there's a CB there.
             s->fs[i-1](irqs); }}}           // Run it!
   return irqs; }
