#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include "tiny1_stdio.h"
#include <string.h>

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

// tiny1_printc.c imports these.
PUBLIC void TPrint_PutCh(U8 ch) {}
PUBLIC C8 *TPrint_BufPtr;
PUBLIC tiny1_S_SafeSprintf TPrint_Safe;
PUBLIC void TPrint_ChIntoBuf(U8 ch) {}
PUBLIC void TPrint_ChIntoBuf_Safe(U8 ch) {}

// ----------------------------------------- eof --------------------------------------------
