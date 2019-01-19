#include "unity.h"
#include "arith.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include "util.h"
#include <string.h>

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* --------------------------------- test_PrintIPAddr ------------------------------------------ */

typedef struct { T_IPAddrNum num; C8 const *str; } S_ChkPrintIPAddr;

#define _toIPNum(d3,d2,d1,d0)    (((U32)(d3) << 24) + ((U32)(d2) << 16) + ((U32)(d1) << 8) + (d0))
#define _toIPStr(d3,d2,d1,d0)    #d3"."#d2"."#d1"."#d0
#define _toS_IPAddr(d3,d2,d1,d0)  { _toIPNum(d3,d2,d1,d0), _toIPStr(d3,d2,d1,d0) }

S_ChkPrintIPAddr const ipaddrPrintChks[] = {
    _toS_IPAddr(0,0,0,0),
    _toS_IPAddr(1,2,3,4),
    _toS_IPAddr(10,20,30,40),
    _toS_IPAddr(255,255,255,255),
};

void test_PrintIPAddr(void)
{
    U8 i;
    C8 b1[_MaxIPAddrChars+1];
    for(i = 0; i < RECORDS_IN(ipaddrPrintChks); i++) {
            TEST_ASSERT_EQUAL_STRING( ipaddrPrintChks[i].str,  PrintIPAddr(b1, ipaddrPrintChks[i].num)); }
}

/* --------------------------------- test_ReadIPAddr ------------------------------------------ */

typedef struct {
    C8 const *inStr;        // Test string
    T_IPAddrNum res;        // expected 32bit IP
    C8 const *rtn;       // remaining string, which may be NULL. meaning we failed to decode an IP.
    } S_ChkRdIPAddr;

// Some number we can us to check that  ReadIPAddr(*result) was unchanged on fail.
#define _IPOutUnchanged 0xABCDEF12

S_ChkRdIPAddr const ipAddrRdChecks[] = {
    // Legal numbers, correct formatting.
    { "0.1.2.3",                            0x00010203, "" },
    { "16.32.48.64",                        0x10203040, "" },
    { "252.253.254.255",                    0xFCFDFEFF, "" },

    // Legal numbers but/w leading zeros - are OK
    { "000.001.002.003",                    0x00010203, "" },

    // Illegal numbers
    { "300.1.2.3",                          _IPOutUnchanged, NULL },
    { "0.400.2.3",                          _IPOutUnchanged, NULL },
    { "0.1.500.3",                          _IPOutUnchanged, NULL },
    { "0.1.2.600",                          _IPOutUnchanged, NULL },

    // Legal numbers but >3 (printed) digits -> fail
    { "0252.253.254.255",                    _IPOutUnchanged, NULL },
    { "252.0253.254.255",                    _IPOutUnchanged, NULL },
    { "252.253.0254.255",                    _IPOutUnchanged, NULL },
    { "252.253.254.0255",                    _IPOutUnchanged, NULL },

    // Eats preamble and return @ suffix.
    { "some preamble 1.2.3.4suffix",        0x01020304, "suffix" },

    // Up to 2 between digit and dot
    { "16 .32. 48 . 64 ",                   0x10203040, " " },
    { "01. 02.003.  04",                    0x01020304, "" },
    { "01  . 02  .  003.  04",              0x01020304, "" },

    // But no more than 2 spaces. These have 3.
    { "0   .1.2.3",                         _IPOutUnchanged, NULL },
    { "0.   1.2.3",                         _IPOutUnchanged, NULL },
    { "0.1.2   .3",                         _IPOutUnchanged, NULL },
    { "0.1.2.   3",                         _IPOutUnchanged, NULL },

    // No letters or double-dots.
    { "0a.1.2.3",                         _IPOutUnchanged, NULL },
    { "0.1.2z.3",                         _IPOutUnchanged, NULL },
    { "0.1..2.3",                         _IPOutUnchanged, NULL },
    { "0.1.2..3",                         _IPOutUnchanged, NULL },

    // Eats leading numbers which don't themselves start an IP addr.
    { "56 9999 1.2.3.4abc",          0x01020304, "abc" },

    // No octets or no numbers at all.
    { "",                                   _IPOutUnchanged, NULL },
    { "just some text",                    _IPOutUnchanged, NULL },

    // Must ignore / bypass partial matches.
    { "7.89.22  1.2.3.4tail",           0x01020304, "tail" },       // Bypass partial, find the real IP
    { "1.2.3.tail",                     _IPOutUnchanged, NULL },
};

PRIVATE bool ipRdCk(S_ChkRdIPAddr const *chk) {
    T_IPAddrNum res = _IPOutUnchanged;                  // Prime output with this number; to see if it gets written.

    // If there's a fail, will show the test string
    C8 failStr[100];
    sprintf(failStr, "input -> \"%s\"", chk->inStr);

    C8 const *rtn = ReadIPAddr(chk->inStr, &res);

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(chk->res, res, failStr);

    if(chk->rtn == NULL) {
        TEST_ASSERT_TRUE_MESSAGE(rtn == NULL, failStr); }
    else {
        TEST_ASSERT_EQUAL_STRING_MESSAGE(chk->rtn, rtn, failStr); }
}

void test_ReadIPAddr(void)
{
    T_IPAddrNum n;
    ReadIPAddr("0.0.0.0", &n);
    TEST_ASSERT_EQUAL_HEX32(_toIPNum(0,0,0,0) , n);

    U8 i;
    for(i = 0; i < RECORDS_IN(ipAddrRdChecks); i++) {
        ipRdCk(&ipAddrRdChecks[i]); }
}

// ==================================== MAC addresses ============================================

/* --------------------------------- test_macAddr_LegalStr ------------------------------------- */

/* ------------------------------------------------------------------------------------------ */
PRIVATE U_MACasNum const * makeRandomMAC(U_MACasNum *mac) {
    U8 i; for(i = 0; i < _MacAddrAsBinary_bytes; i++) {
        mac->asBytes[i] = randU8(); }
    return mac; }

void test_macAddr_LegalStr(void)
{
    C8 b1[_MacAddr_Chars+1];

    // 100 random printed MACs.
    U8 i; for(i = 0; i < 100; i++) {
        U_MACasNum m;
        macAddr_NumToStr(b1, makeRandomMAC(&m)) ;
        TEST_ASSERT_TRUE_MESSAGE(macAddr_LegalStr(b1) == true, b1); }

    #define _legalStr(s) \
        TEST_ASSERT_TRUE_MESSAGE(macAddr_LegalStr(s) == true, "Str = " s)

    #define _badStr(s) \
        TEST_ASSERT_TRUE_MESSAGE(macAddr_LegalStr(s) == false, "Str = " s)

    _legalStr("00:00:00:00:00:00");
    _legalStr("01:00:00:9A:EF:FF");
    _legalStr("aA:bB:cC:dD:eE:fF");        // Both uppercase and lowercase Hex OK.

    _badStr  ("");
    _badStr  ("12:34:56:78:9A:BC2");       // Trailng digit not allowed
    _legalStr("12:34:56:78:9A:BCA");       // But... Trailng letter is allowed, even if it's a HexASCI digit.

    _badStr  ("12:34:56:78:9A:B");          // Incomplete
    _badStr  ("12:34:56:78:9A:B ");         // Incomplete.

    // Spaces
    _badStr  ("12:34:56:78:9A:B C");
    _badStr  ("1 2:34:56:78:9A:BC");
    _badStr  ("12:34:5 6:78:9A:BC");
    _badStr  ("12:34:56: 78:9A:BC");
    _badStr  ("12:34 :56:78:9A:BC");
    _badStr  ("12:34:56: 78:9A:BC");

    // Spaces
    _badStr  ("12 34:56:78:9A:BC");
    _badStr  ("12:34 56:78:9A:BC");
    _badStr  ("12:34:56 78:9A:BC");
    _badStr  ("12:34:56:78 9A:BC");
    _badStr  ("12:34:56:78:9A BC");

    // Stops, commas and semi-colons
    _badStr  ("12,34:56:78:9A:BC");
    _badStr  ("12:34,56:78:9A:BC");
    _badStr  ("12:34:56.78:9A:BC");
    _badStr  ("12:34:56:78.9A:BC");
    _badStr  ("12.34.56.78.9A.BC");
    _badStr  ("12,34:56:78,9A:BC");
    _badStr  ("12:34:56;78:9A:BC");
    _badStr  ("12;34:56:78:9A:BC");
    _badStr  ("12:34:56:78;9A:BC");
    _badStr  ("12:34:56:78:9A;BC");

    _legalStr("12:34:56:78:9A:BC");
}

/* -------------------------------- test_macAddr_NumToStr ------------------------------------- */
#define _MacAddr_Str(a,b,c,d,e,f) #a":"#b":"#c":"#d":"#e":"#f

void test_macAddr_NumToStr(void)
{
    C8 b1[_MacAddr_Chars+1];
    // Make a MAC number, print it, and compare printout with string of original number.
    #define _printTest(mac, a,b,c,d,e,f) \
        U_MACasNum mac =  _MacAddr_DeclConst(a,b,c,d,e,f); \
        TEST_ASSERT_EQUAL_STRING_MESSAGE(_MacAddr_Str(a,b,c,d,e,f), macAddr_NumToStr(b1, &mac), "Str = " _MacAddr_Str(a,b,c,d,e,f));

    // Try the full range of hex HexASCII.
    _printTest(m1, 00,01,03,04,05,06)
    _printTest(m2, AB,CD,EF,FF,23,45)
}

/* -------------------------------- test_macAddr_NumsEqual ------------------------------------- */

static U_MACasNum const * randByteN(U_MACasNum *m, U8 idx)
{
    U8 nu;
    while( (nu = randU8()) == m->asBytes[idx] ) {}  // Make sure we change the number.
    m->asBytes[idx] = nu;
    return m;
}

void test_macAddr_NumsEqual(void)
{
    C8 mStr[_MacAddr_Chars+1];

    U8 i; for(i = 0; i < 50; i++) {
        U_MACasNum mac0;
        U_MACasNum const * m0 = makeRandomMAC(&mac0);

        TEST_ASSERT_TRUE_MESSAGE(macAddr_NumsEqual(m0, m0), macAddr_NumToStr(mStr, m0) );

        U_MACasNum mac1 = mac0;
        TEST_ASSERT_FALSE_MESSAGE( macAddr_NumsEqual(m0, randByteN(&mac1, i % _MacAddrAsBinary_bytes)), macAddr_NumToStr(mStr, m0) );
        }
}

/* -------------------------------- test_macAddr_StrToNum ------------------------------------- */

typedef struct {
    C8 const    *testStr;           // Test string (which may be empty or bad)
    U_MACasNum  reqdNum;            // If 'mustMatch' then it must match this MAC.
    bool        mustMatch,          // Match required
                rtn,                // function return must be...
                noOutput;           // If true, then, the U_MACasNum * passed to macAddr_StrToNum() must be unchanged.
} S_MaxRdTest;

PRIVATE C8 const *tf(bool b) { return b != false ? "true" : "false"; }

PRIVATE bool testOneRead(S_MaxRdTest const *t) {
    U_MACasNum fromStr;
    makeRandomMAC(&fromStr);                                // Fill output MAC with a random.
    U_MACasNum was = fromStr;                               // Remember what that random was.
    bool rtn = macAddr_StrToNum(&fromStr, t->testStr);      // Convert test string to MAC in 'fromStr'.

    C8 b0[_MacAddr_Chars+1];
    C8 b1[150];
    b1[0] = '\0';

    if(t->noOutput == true) {
        if( macAddr_NumsEqual(&was, &fromStr) == false) {
            sprintf(b1, "Should have been no-output but ...\"%s\" -> %s", t->testStr, macAddr_NumToStr(b0, &fromStr) ); }}
    else if( !macAddr_NumsEqual(&t->reqdNum, &fromStr) && macAddr_NumsEqual(&was, &fromStr) && t->mustMatch == true) {
        sprintf(b1, "No parse \"%s\" -> \"unchanged\"", t->testStr ); }
    else if( macAddr_NumsEqual(&t->reqdNum, &fromStr) == false && t->mustMatch == true) {
        C8 b2[_MacAddr_Chars+1];
        sprintf(b1, "Bad parse \"%s\" -> %s, expected %s", t->testStr, macAddr_NumToStr(b0, &fromStr), macAddr_NumToStr(b2, &t->reqdNum) ); }

    // If return boolean was mismatecd print that (also)
    if( rtn != t->rtn) {
        C8 b3[30];
        // If number WAS converted correctly then there will be no error printout above. Must print it now
        // to label the boolean-return mismatch.
        if(b1[0] == '\0') {
            sprintf(b1, "\"%s\" -> %s, but...", t->testStr, macAddr_NumToStr(b0, &fromStr) ); }

        sprintf(b3, " rtn <- %s, expected %s", tf(rtn), tf(t->rtn));
        strcat(b1, b3); }

    TEST_ASSERT_TRUE_MESSAGE(b1[0] == '\0', b1 );
}

void test_macAddr_StrToNum(void)
{
    // Good parses. Try all HexASCII chars
    S_MaxRdTest t1 = {.testStr = "01:02:03:04:05:06", .reqdNum = _MacAddr_DeclConst(01,02,03,04,05,06), .mustMatch = true, .rtn = true };
    testOneRead(&t1);
    S_MaxRdTest t2 = {.testStr = "AB:CD:EF:00:FF:00", .reqdNum = _MacAddr_DeclConst(AB,CD,EF,00,FF,00), .mustMatch = true, .rtn = true };
    testOneRead(&t2);

    // Incomplete MAC -> No output && false.
    S_MaxRdTest t3 = {.testStr = "01:02:03:04:05", .reqdNum = _MacAddr_DeclConst(01,02,03,04,05,06), .mustMatch = false, .rtn = false, .noOutput = true };
    testOneRead(&t3);
}

/* -------------------------------- test_macAddr_StrIsZero ------------------------------------- */

void test_macAddr_StrIsZero(void)
{
    typedef struct {C8 const *str; bool result; } S_Test;

    S_Test const tests[] = {
        {"00:00:00:00:00:00", true},
        {"00:00:00:00:00:00ABC", true},     // Trailing text is OK, even if its a HexASCII char

        {"01:00:00:00:00:00", false},
        {"00:02:00:00:00:00", false},
        {"00:00:03:00:00:00", false},
        {"00:00:00:04:00:00", false},
        {"00:00:00:00:05:00", false},
        {"00:00:00:00:00:06", false},

        {"", false},                        // Empty -> fail
        {"some text", false},               // Not a MAC -> fail
        {"00:00:00:00:00:003", false},      // Extra digit -> false
    };

    U8 i; for(i = 0; i < RECORDS_IN(tests); i++) {
        S_Test const *t = &tests[i];
        TEST_ASSERT_TRUE_MESSAGE( macAddr_StrIsZero(t->str) == t->result, t->str);
    }
}

/* -------------------------------- test_macAddr_StrIsNonZero ------------------------------------- */

void test_macAddr_StrIsNonZero(void)
{
    typedef struct {C8 const *str; bool result; } S_Test;

    S_Test const tests[] = {
        {"00:00:00:00:00:00", false},
        {"00:00:00:00:00:00ABC", false},     // Trailing text is OK, even if its a HexASCII char

        {"01:00:00:00:00:00", true},
        {"00:02:00:00:00:00", true},
        {"00:00:03:00:00:00", true},
        {"00:00:00:04:00:00", true},
        {"00:00:00:00:05:00", true},
        {"00:00:00:00:00:06", true},

        {"", false},                        // Empty -> fail
        {"some text", false},               // Not a MAC -> fail
        {"00:00:00:00:00:003", false},      // Extra digit -> false
    };

    U8 i; for(i = 0; i < RECORDS_IN(tests); i++) {
        S_Test const *t = &tests[i];
        TEST_ASSERT_TRUE_MESSAGE( macAddr_StrIsNonZero(t->str) == t->result, t->str);
    }

}

// ----------------------------------------- eof --------------------------------------------
