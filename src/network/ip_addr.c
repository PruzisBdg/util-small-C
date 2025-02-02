#include "libs_support.h"
#include "util.h"
#include <string.h>
#include <ctype.h>

/* ------------------------------------------------------------------------------------------------- */
PUBLIC C8 const *PrintIPAddr(C8 *out, T_IPAddrNum ip) {
   sprintf(out, "%u.%u.%u.%u", HIGH_BYTE(HIGH_WORD(ip)), LOW_BYTE(HIGH_WORD(ip)), HIGH_BYTE(LOW_WORD(ip)), LOW_BYTE(LOW_WORD(ip)) );
   return out; }

/* --------------------------------- ReadIPAddr -------------------------------------------------------

   The eats text until the 1st digit. After that attempts to read and IP address. Each octet may be 1-3
   chars. There may be up to 2 spaces between the octets and the '.'s. E.g
      'lead text 192.168.1.20', '192.168.001.020', '192 . 168. 1 . 20', '192.168. 01. 20'
   are all OK

   Returns the 1st char after the IP address text, NULL if fail. If did fail then 'ip' will be
   unmodified.
*/
#define _NumOctets          4
#define _MaxOctectDigits    3
#define _MaxSpaces          2   // Up to 2 spaces then '.'. Return after '.', provided that ch is printable.

#define _IsAnOctet(n) ((n) >= 0 && (n) <= 255)

// Assuming we got a number with @lastDigit, that number has 1-3 digits.
PRIVATE bool was1to3Digits(C8 const *start, C8 const *lastDigit) {
   return                                                      // Either...
        lastDigit-start < _MaxOctectDigits ||                   // No more than 3 chars 'start' to 'lastDigit' (so number cannot have > 3 digits)? OR
        isdigit(*(lastDigit-_MaxOctectDigits)) == false; }      // 3rd char back from 'lastDigit' is NOT a digit; so number cannot have > 3?

PRIVATE bool endsOnAnOctet(S16 n, C8 const *str, C8 const *lastCh) {
    return _IsAnOctet(n) && was1to3Digits(str, lastCh); }

PRIVATE bool toAfterDot(C8 const **src) { U8 i;
    for(i = 0;
        (**src == ' ' || **src == '.') && **src != '\0' && i < _MaxSpaces+1;     // While space OR '.' AND 1st,2nd or 3rd char...
        i++, (*src)++) {
        if(**src == '.') {                                      // Hit a '.'?
            (*src)++;                                           // Bump past that '.'
            return isprint(**src) != 0                          // Return true if that ch is printable.
                ? true : false; }}
    return false; }

// Up to 2 spaces then digit. Return on digit.
PRIVATE bool toDigit(C8 const **src) { U8 i;
   for(i = 0;
        (**src == ' ' || isdigit(**src)) && **src != '\0' && i < _MaxSpaces+1;       // While space OR digit AND 1st,2nd or 3rd char....
        i++, (*src)++) {
       if(isdigit(**src))                                   // Hit a digit?
            { return true; }}                               // then return @digit.
    return false; }                                         // No digit within 3 chars -> fail.

// Eat leading non-numbers then get 0..255 from no more than 3 digits.
// Return NULL if no digit found. If any digits were found, move 'src' to after these digits, whether they were octet or no.
PRIVATE bool etcOctet(C8 const **src, U8 *octet) {
   S16 n;
   C8 const *p0 = *src;                                     // Mark search start.
   if( (*src = ReadDirtyASCIIInt(*src, &n)) != NULL) {      // Snagged an integer?. *src will be after last digit, or end-of-string if no digit found.
      if( endsOnAnOctet(n, p0, *src-1) ) {                  // Is 0...255, 1-3 digits?
         *octet = n;                                        // then is a legal octet. Output it.
         return true; }}                                    // Success, return 'src' after the last digit of the octet.
   return false; }             // Fail, return 'src' after last digit of the number, which was NOT and octet. OR end-of-string if no number found.

// Eat leading non-numbers then get 0..255 from no more than 3 digits then '.'.
// Return ch after '.'
PRIVATE bool etcOctetDot(C8 const **src, U8 *octet) {
   C8 const *q = *src;
   S16 n;
   if( (*src = ReadDirtyASCIIInt(q, &n)) == NULL ) {        // No integer?
        *src = EndStrC(q); }                                // then ReadDirtyASCIIInt() searched to end-of-string. Advance 'src' there.
    else {                                                  // else found at least 1 digit.
        C8 const *lastDigit = *src-1;                       // Mark last digit; if find a '.' will use this to check octet.
      if( toAfterDot(src) == true &&                        // Got a '.' within 2 spaces of the number? AND
             endsOnAnOctet(n, q, lastDigit)) {              // 0..255, is 1..3 digits
            *octet = n;                                     // then this is a legal octet.
         return true; }}                                    // Return @ch after the '.'
   *octet = 0;                         // Fail... force result to 0.
   return false; }

// Get 0..255 from no more than 3 digits then '.'. Return ch after '.', provided it is printable.
// If fail, return NULL and 'octet' <- 0.
PRIVATE bool octetDot(C8 const **src, U8 *octet) {
   if( toDigit(src) == true ) {                          // No more than (2) spaces then digit?
        if( etcOctet(src, octet) == true) {              // This digit was start of an octet?
            if( toAfterDot(src) == true) {               // '.' after the octet?
                return true; }}}                         // Success. Return @ ch after '.'
   *octet = 0;                                           // Fail; force octet result -> 0.
   return false; }

PRIVATE bool lastOctet(C8 const **src, U8 *octet) {
   if( toDigit(src) == true) {                           // No more than (2) spaces then digit.
      if( etcOctet(src, octet) == true) {                // Read octet. etcOctet() does eat leading chars but we're already at 1st char so nothing to eat.
            return true; }}                              // Success. Return @ ch after last octet digit.
   *octet = 0;                                           // Fail; force octet result -> 0.
   return false; }

PRIVATE bool nextOctet(C8 const **src, U8 *oct, U8 idx) {
    // March thru these 4 parsers to read an IP addr.
    bool (*parsers[])(C8 const**, U8*) = { etcOctetDot, octetDot, octetDot, lastOctet };
    return
        idx >= RECORDS_IN(parsers)          // Illegal idx (> 3)?
            ? false                         // shouldn't happen, but just say fail if it does.
            : parsers[idx](src, oct); }     // else run parser to get i'th octet.


PUBLIC C8 const * ReadIPAddr(C8 const *src, T_IPAddrNum *ip) {
   U32 n = 0; U8 o; S16 i = 0;

   for(i = 0; i < _NumOctets; ) {                           // Until get IP addr or fail trying.
        if( nextOctet(&src, &o, i) == false ) {             // Didn't get next octet?
            if(*src == '\0')                                // End-of-string?
                { return NULL; }                            // then done & fail
            else                                            // else there's more string to search.
                { i = 0; n = 0; }}                          // so reset the search for 4 octets
        else {                                              // else got next octet.
            n = (n << 8) + o;                               // add it to IP addr.
            i++; }}                                         // Advance parser.
   *ip = n;                      // Success, output 32bit IP address
   return src;   }               // Return 1st char after e.g '192.168.1.20'.


// ===================================== EOF =====================================================
