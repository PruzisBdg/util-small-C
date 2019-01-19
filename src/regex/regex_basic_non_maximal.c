#include "libs_support.h"
#include "util.h"
#include <string.h>

typedef struct
{
    T_MatchTerm termType;
    union {
        C8 Char;
    } U_TermVal;
} S_Term;

typedef struct {
    S_Term term;
    C8 const *nexGet;
} S_TermRead;

PRIVATE S_TermRead const *nextTerm(C8 const *readAt)
{
    static S_TermRead t;

    if(*readAt == '\0')
        { return NULL; }
    else
    {
        t.term.termType = tLiteral;
        t.term.U_TermVal.Char = *readAt;
        t.nexGet = readAt+1;
        return &t;
    }
}

PUBLIC BOOL RegexNonMaximal(C8 const *str, C8 const *regExpr, Regex_S_Result *result)
{
    if(str[0] == '\0')
    {
        result->start = 0; result->end = 0;
        return FALSE;
    }
    if(regExpr[0] == '\0')
    {
        result->start = 0;
        result->end = strlen(str);
        return TRUE;
    }
}

// --------------------------------------------- eof --------------------------------------------