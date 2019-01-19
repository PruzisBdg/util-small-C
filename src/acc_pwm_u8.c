#include "GenericTypeDefs.h"
#include "small_utils.h"

/* ---------------------------------- AccPwmU8_Init ---------------------------------- */

PUBLIC void AccPwmU8_Init(S_AccPwmU8 *a, AccPwmU8_T incr, AccPwmU8_T fs)
    { a->acc = 0; a->incr = incr; a->fs = fs; }

/* ---------------------------------- AccPwmU8_WrIncr ---------------------------------- */

PUBLIC void AccPwmU8_WrIncr(S_AccPwmU8 *a, AccPwmU8_T incr)
    { a->incr = incr; }

/* ---------------------------------- AccPwmU8_Run ---------------------------------- */

PUBLIC BOOL AccPwmU8_Run(S_AccPwmU8 *a)
{
    a->acc += a->incr;

    if(a->acc >= a->fs)
    {
        a->acc -= a->fs;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// ----------------------------------- eof ------------------------------------------------
