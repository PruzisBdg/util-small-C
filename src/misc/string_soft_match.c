/*---------------------------------------------------------------------------
|
| Bounded strings
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

PRIVATE U8 matchCh(C8 a, C8 b)
{
    return
        a == b                   ? 10 : (
        toupper(a) == toupper(b) ? 0 : (
        isalpha(a) && isalpha(b) ? 0 : (
        isdigit(a) && isdigit(b) ? 0 : (
        isspace(a) && isspace(b) ? 0 : (
        0 )))));
}


PRIVATE tSoftMatchStr match1(C8 const *a, C8 const *b)
{
    tSoftMatchStr match;
    for(match = 0; *a != '\0' && *b != '\0'; a++, b++)
        { match += matchCh(*a, *b); }
    return match;
}

typedef struct { tSoftMatchStr *buf; tSoftMatchStr const *filt; U8 put, size; } S_FirU16;

PRIVATE U8 bump(S_FirU16 *f, U8 i) {
    if(++i >= f->size) { i = 0; }
    return i; }

PRIVATE U8 atPut(S_FirU16 *f, U8 ofs)
    { return (f->put + ofs) % f->size; }

PRIVATE tSoftMatchStr FirU16_Run(S_FirU16 *f, tSoftMatchStr n)
{
    f->buf[f->put] = n;
    f->put = bump(f, f->put);

    U8 i, j; tSoftMatchStr sum;
    for(i = 0, j = f->put, sum = 0; i < f->size; i++) {
        sum += (f->filt[i] * f->buf[j]);
        j = bump(f, j); }
    return sum;
}


PUBLIC tSoftMatchStr SoftMatchStr(C8 const *ref, C8 const *str, C8 const **matchAt)
{
    tSoftMatchStr maxM, m;

    #define _FiltSize 3

    tSoftMatchStr fb[_FiltSize] = {0};
    tSoftMatchStr const filt[_FiltSize] = {1,2,1};
    S_FirU16 f;
    f.buf = fb; f.filt = filt; f.put = 0; f.size = _FiltSize;

    for(maxM = 0, *matchAt = ref; *ref != '\0'; ref++) {
        if( (m = FirU16_Run(&f, match1(ref, str))) > maxM )
            { maxM = m; *matchAt = ref; }
        printf("m1 %d <- [%d %d %d] <- %d %s\r\n", m, f.buf[atPut(&f,2)],f.buf[atPut(&f,1)],f.buf[atPut(&f,0)], match1(ref, str), ref); }
    return maxM;
}

// --------------------- eof ------------------------------------------------
