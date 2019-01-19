#include "libs_support.h"
#include "util.h"
#include <string.h>
#include <ctype.h>

typedef struct {
    S16         num;
    C8 const    *msg;
} S_HttpStatusCode;

PRIVATE S_HttpStatusCode const codes[] =
{
    {100, "Continue"},
    {101, "Switching Protocols"},
    {102, "Processing"},
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {207, "Multi-Status"},
    {208, "Already Reported"},
    {226, "IM Used"},
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {307, "Temporary Redirect"},
    {308, "Permanent Redirect"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Payload Too Large"},
    {414, "URI Too Long"},
    {415, "Unsupported Media Type"},
    {416, "Range Not Satisfiable"},
    {417, "Expectation Failed"},
    {421, "Misdirected Request"},
    {422, "Unprocessable Entity"},
    {423, "Locked"},
    {424, "Failed Dependency"},
    {426, "Upgrade Required"},
    {428, "Precondition Required"},
    {429, "Too Many Requests"},
    {431, "Request Header Fields Too Large"},
    {451, "Unavailable For Legal Reasons"},
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"},
    {505, "HTTP Version Not Supported"},
    {506, "Variant Also Negotiates"},
    {507, "Insufficient Storage"},
    {508, "Loop Detected"},
    {510, "Not Extended"},
    {511, "Network Authentication Required"},
};

PRIVATE C8 const * matchStatusCode(S16 codeToMatch) {

    U8 c;
    for(c = 0; c < RECORDS_IN(codes); c++) {
        if(codes[c].num == codeToMatch) {
            return codes[c].msg; }}
    return NULL;
}



PRIVATE BOOL matchesStr(C8 const *p, C8 const *ref)
{
    for(; *ref != '\0'; p++, ref++) {
        if(*p != *ref) {
            return FALSE; }}
    return TRUE;
}



/* ----------------------------------- FindHTTPstatusMsg ----------------------------------------*/

#define _HttpStatusCode_OK 200

PUBLIC BOOL FindHTTPstatusMsg(C8 const *str, S_MatchedinStr *match)
{
    C8 const *p;
    C8 const *statusText;
    S16 nnn;

    for(p = str; *p != '\0'; p++) {                                     // Until the end of 'str'
        if(isdigit(*p)) {                                               // Got (1st) digit?
            if(p > str) {                                               // If past start of string?...
                if(isdigit(*(p-1))) {                                   // ...is previous char a digit?
                    continue; }}                                        // if yes, this is NOT start of a status code; move on.
                                                // else got <not-digit> <digit>; continue checking
            if(isspace(*(p+3))) {                                       // 4th char is space? AND
                if(isdigit(*(p+1))) {                                   // ...2nd AND...
                    if(isdigit(*(p+2)))                                 // ...3rd chars are digits?
                    {                                                   // then we have [no-digit,3digits,space]
                        ReadDirtyASCIIInt(p, &nnn);                     // Read the 'nnn'.

                        // Search for 'nnn' status codes tables.
                        if( (statusText = matchStatusCode(nnn)) != NULL)  // Match?
                        {                                                   // yes; 'nnn' is a status code?

                           // Yes, then check that text following 'nnn' in 'str' is the correct 'statusText'.
                            if(matchesStr(p+4, statusText))                 // payload text matches 'statusText'?
                            {                                               // then we fond a HTTP status message
                                match->at = (C8*)p;                         // Ths status message 'nnn <spc> <statusText> \r\n' starts here
                                match->numChars = strlen(statusText) + 4;   // Length is <3 digits> + <spc> + <statusText>
                                match->result = nnn;                        // Our status code is this.
                                return TRUE;                                // And we succeeded.
                                }}}}}}}

    match->at = NULL;       // No match
    match->numChars = 0;
    match->result = 0;
    return FALSE;           // And say we failed.
}

// ------------------------------------------------- eof ------------------------------------------------------