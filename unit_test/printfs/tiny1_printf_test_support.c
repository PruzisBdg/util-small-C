/* ---------------------------------------------------------------------------------------
|
| Support for the test harness to check the various build-variants of tiny1_printf.c
|
|
------------------------------------------------------------------------------------------ */

#include <stdlib.h>
#include <string.h>
#include "libs_support.h"
#include "tiny1_stdio.h"
#include "tiny1_printf_test_support.h"

// Prints to an out-stream go here; to be displayed or compared by a test.
#define _OStreamBufSize 500

typedef struct {
   U8 buf[_OStreamBufSize];
   U16 put, get;
} S_OStream;

PRIVATE S_OStream os;

PUBLIC void OStream_Reset(void) {
   os.put = 0;
   os.get = 0;
   memset(os.buf, 0, _OStreamBufSize); }

PUBLIC void OStream_Print(void) {
   printf("OStream: \"%s\"\r\n", os.buf); }

PUBLIC U8 const * OStream_Get(void)
   { return os.buf; }


// =============================== tiny1_printc.c needs all these ==============================

// For tiny1_printf()
PUBLIC void TPrint_PutCh(U8 ch) {
   if(os.put < _OStreamBufSize-1) {
      os.buf[os.put++] = ch; }}

// For tiny1_sprintf()
PUBLIC C8 *TPrint_BufPtr;

PUBLIC void TPrint_ChIntoBuf(U8 ch) {
   *(TPrint_BufPtr++) = ch; }

// For tiny1_sprintf_safe
PUBLIC tiny1_S_SafeSprintf TPrint_Safe;
PUBLIC void TPrint_ChIntoBuf_Safe(U8 ch) {}

// ----------------------------------------- eof --------------------------------------------
