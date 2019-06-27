#include "unity.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "tdd_common.h"
#include "sensus_codec.h"
#include <string.h>

/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* For:
      bool Sensus_DecodeMsg(C8 const *src, enc_S_MsgData *ed, enc_M_EncType filterFor);
*/
typedef struct { C8 const *msg; enc_M_EncType filt; bool rtn; enc_S_MsgData *out; } S_DecodeTst;

/* ------------------------------- legalEmptyEncoder --------------------------------

   Zeros, empty strings, <null> fields for pressure and temperature. This is what the
   Decoder must return if it can't complete a parse.
*/
PRIVATE enc_S_MsgData legalEmptyEncoder = {
   .serialWord = "",
   .kStr = "",
   .noMag = {
      .pres = {._min = 0xFF, ._max = 0xFF, ._avg = 0xFF},
      .fluidDegC = 0xFF, .ambientDegC = 0xFF}
};

/* --------------------------------- zeroEncoder ----------------------------------------

   which is NOT legal. For one, 'serialStr' and 'KStr' are NULL; Host will trap trying to
   reading them (the strings)
*/
PRIVATE enc_S_MsgData zeroEncoder = {0};

/* ----------------------------- aGarbageEncoder -----------------------------------------

   In tests, to make sure all fields are correctly updated.
*/
PRIVATE enc_S_MsgData const *aGarbageEncoder(void) {
   static enc_S_MsgData ed;
   randFill((void*)&ed, sizeof(enc_S_MsgData));
   return &ed; }

// --------------------------------------------------------------------------------------
static C8 const *testMessagePremable(C8 *out, U8 testNum, S_DecodeTst const *t, bool rtn) {
   C8 b0[100];
   sprintf(out, "\r\ntst #%d  msg = \"%s\n\", filt = %s -> %s",
      testNum, t->msg, sens_ShowEncoders(b0, t->filt), rtn == false ? "false" : "true");
   return out;
}

/* ----------------------------------- test_decodeBasicStatus ---------------------------------- */

void test_decodeBasicStatus(void) {
   typedef struct {U16 mf; enc_S_Alerts alerts; } S_Tst;

   S_Tst const tsts[] = {
      { .mf = 0x0000, .alerts.noMag.asU16 = 0 },

      { .mf = 0x8000, .alerts.noMag.bs.overflow = 1   },
      { .mf = 0x4000, .alerts.noMag.bs.pressure = 1   },
      { .mf = 0x0100, .alerts.noMag.bs.negFlowRate = 1},
      { .mf = 0x0080, .alerts.noMag.bs.tamper = 1     },
      { .mf = 0x0040, .alerts.noMag.bs.program = 1    },
      { .mf = 0x0020, .alerts.noMag.bs.leak = 1       },
      { .mf = 0x0010, .alerts.noMag.bs.reverseFlow = 1},
      { .mf = 0x0008, .alerts.noMag.bs.noFlow = 1     },
      { .mf = 0x0004, .alerts.noMag.bs.endOfLife = 1  },
      { .mf = 0x0002, .alerts.noMag.bs.temperature = 1},
      { .mf = 0x0001, .alerts.noMag.bs.emptyPipe = 1  },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      enc_S_MsgData out;
      bool rtn = decodeBasicStatus(t->mf, &out);

      if(rtn != true) {
         printf("tst #%d: Expected true, got %d\r\n", i, rtn);
         TEST_FAIL(); }
      else if(t->alerts.noMag.asU16 != out.alerts.noMag.asU16) {
         C8 b1[150], b2[150];
         printf("tst #%d: expected %s,  got %s\r\n", i, sens_ShowAlerts(b1, &t->alerts), sens_ShowAlerts(b2, &out.alerts) );
         TEST_FAIL(); }
   }
}

/* -------------------------------------- test_getXT -------------------------------------------- */

//_EXPORT_FOR_TEST C8 const * getXT(C8 const *src, enc_S_MsgData *ed );

void test_getXT(void)
{
   typedef struct {
      C8 const    *src;
      C8 const    *tail;
      bool        success;                    // Return was not NULL.
      enc_T_degC  fluidTmpr, ambTmpr;
      bool        alert,                      // Bad temperature alert set.
                  gotFluid, gotAmbient;
      } S_Tst;

   S_Tst const tsts[] = {
      {.src = "", .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },

      /* ';XTddd' means just fluid temperature as a signed decimal.

         'XTddd' can be followed by '\0', '\r' or ';' (next field).
      */
      {.src = "025\r",  .tail = "\r",     .success = true, .fluidTmpr = 25,   .ambTmpr = 0xFF, .gotFluid = true },
      {.src = "102",    .tail = "",       .success = true, .fluidTmpr = 102,  .ambTmpr = 0xFF, .gotFluid = true },
      {.src = "003;",   .tail = ";",      .success = true, .fluidTmpr = 3,    .ambTmpr = 0xFF, .gotFluid = true },
      // Leading '+' is legal.
      {.src = "+28;",   .tail = ";",      .success = true, .fluidTmpr = 28,   .ambTmpr = 0xFF, .gotFluid = true },
      {.src = "-28;",   .tail = ";",      .success = true, .fluidTmpr = -28,  .ambTmpr = 0xFF, .gotFluid = true },

      // 125 means Meter is in storage mode. Leave 'fluidTmpr' at 0xFF, null-reading.
      {.src = "125\r",  .tail = "\r",     .success = true, .fluidTmpr = 0xFF,  .ambTmpr = 0xFF },
      // 126 means measurement error. Set error flag.
      {.src = "126\r",  .tail = "\r",     .success = true, .fluidTmpr = 0xFF,  .ambTmpr = 0xFF, .alert = true },

      /* ';XTffaa' means fluid and ambient temperatures as signed hex.

         0x7E -> error; 0x7D -> Meter in storage; 0x7C -> No sensor.
      */
      {.src = "1517\r",  .tail = "\r",     .success = true, .fluidTmpr = 21,   .ambTmpr = 23, .gotFluid = true, .gotAmbient = true },
      {.src = "83FE\r",  .tail = "\r",     .success = true, .fluidTmpr = -125,   .ambTmpr = -2, .gotFluid = true, .gotAmbient = true },
      // Hex is not case sensitive
      {.src = "abcd\r",  .tail = "\r",     .success = true, .fluidTmpr = -85,   .ambTmpr = -51, .gotFluid = true, .gotAmbient = true },
      // 0x7C, meaning "No Sensor" doesn't trigger an alert; just menas no readin to be had.
      {.src = "7CFE\r",  .tail = "\r",     .success = true, .fluidTmpr = 0xFF,   .ambTmpr = -2, .gotFluid = false, .gotAmbient = true },
      {.src = "037C\r",  .tail = "\r",     .success = true, .fluidTmpr = 3,   .ambTmpr = 0xFF, .gotFluid = true, .gotAmbient = false },
      // 0x7D (storage mode) -> no measurement.
      {.src = "7D44\r",  .tail = "\r",     .success = true, .fluidTmpr = 0xFF,   .ambTmpr = 68, .gotFluid = false, .gotAmbient = true },
      {.src = "107D\r",  .tail = "\r",     .success = true, .fluidTmpr = 16,   .ambTmpr = 0xFF, .gotFluid = true, .gotAmbient = false },
      // 0x7E (Error) -> no measurement & alert.
      {.src = "7E44\r",  .tail = "\r",     .success = true, .fluidTmpr = 0xFF,   .ambTmpr = 68, .gotFluid = false, .gotAmbient = true, .alert = true },
      {.src = "107E\r",  .tail = "\r",     .success = true, .fluidTmpr = 16,   .ambTmpr = 0xFF, .gotFluid = true, .gotAmbient = false, .alert = true },

      // ';XTddd' or ';XTffaa' must be followed by and end-of-message or a field delimiter ';'.
      {.src = "025 ", .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "1517 ", .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },

      // Number(s) must be exactly 3 decimal or 4 HexASCII.
      {.src = "4;",     .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "05;",    .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "-015;",  .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "--15;",  .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "+-15;",  .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "12345;", .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "10C;",   .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "120C3;", .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "-3C2;",  .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "4X5;",   .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
      {.src = "4 78;",  .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },

      // No leading whitespace..
      {.src = " 054;",  .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      enc_S_MsgData ed = legalEmptyEncoder;

      C8 const *rtn = getXT(t->src, &ed);

      bool fail = false;

      C8 b0[100];
      sprintf(b0, "tst #%d.", i);

      // Parse should have succeeded, meaning returned non-NULL?
      if(t->success == true) {
         /* If parse did succeed then check for correct:
               - temperature(s)
               - data flags and alerts
               - tail returned.
         */
         if(rtn != NULL) {
            if(ed.noMag.fluidDegC != t->fluidTmpr ) {
               printf("%s Expected fluid = %ddegC, got %d\r\n", b0, t->fluidTmpr, ed.noMag.fluidDegC);
               fail = true; }

            if(ed.noMag.ambientDegC != t->ambTmpr ) {
               printf("%s Expected ambient = %ddegC, got %d\r\n", b0, t->ambTmpr, ed.noMag.ambientDegC);
               fail = true; }

            if(t->gotFluid == true && ed.weGot.bs.fluidTmpr == 0 || t->gotFluid == false && ed.weGot.bs.fluidTmpr == 1) {
               printf("%s gotFluid; expected %d, got %d\r\n", b0, t->gotFluid, ed.weGot.bs.fluidTmpr);
               fail = true; }

            if(t->gotAmbient == true && ed.weGot.bs.ambientTmpr == 0 || t->gotAmbient == false && ed.weGot.bs.ambientTmpr == 1) {
               printf("%s gotAmbient; expected %d, got %d\r\n", b0, t->gotAmbient, ed.weGot.bs.ambientTmpr);
               fail = true; }

            if(strcmp(rtn, t->tail) != 0) {
               printf("%s Expected rtn \"%s\"\r\n, got \"%s\"\r\n", b0, t->tail, rtn);
               fail = true; }

            if(t->alert == true) {
               if( ed.alerts.noMag.bs.temperature == 0) {
                  printf("%s. Expected alert but was not set.\r\n",b0);
                  fail = true; }
            }
            else {
               if( ed.alerts.noMag.bs.temperature == 1) {
                  printf("%s. Unexpected alert.\r\n", b0);
                  fail = true; }
            }
         }
         // else should have parsed but did not.
         else {
            printf("%s Expected success but got NULL\r\n", b0);
            fail = true;
         }
      }
      // else parse should have failed, meaning returned NULL.
      else {
         if(rtn == NULL) {       // Expected NULL, and got it?
            // then there should be no temperatures and no alerts.
            if(ed.noMag.fluidDegC != 0xFF || ed.noMag.ambientDegC != 0xFF) {
               printf("%s. Got NULL return (correct) but fluidTmpr = %d ambientTmpr = %d; should both be 0xFF\r\n", b0, ed.noMag.fluidDegC, ed.noMag.ambientDegC); fail = true; }
            if(ed.alerts.noMag.bs.temperature == 1) {
               printf("%s. Got NULL return (correct) but also temperature alert (wrong)\r\n", b0); fail = true;}
            if(ed.weGot.bs.fluidTmpr == 1 || ed.weGot.bs.ambientTmpr == 1) {
               printf("%s. Got NULL return (correct) but data flags fluid = %d ambient %d set (wrong)\r\n", b0, ed.weGot.bs.fluidTmpr, ed.weGot.bs.ambientTmpr); fail = true; }
         }
         else {
            printf("%s Expected fail (NULL) but got \"%s\"\r\n", b0, rtn);
            fail = true; }
      }
      if(fail == true) {
         TEST_FAIL(); }
   }
}

/* -------------------------------------- test_getXP -------------------------------------------- */

void test_getXP(void)
{
   typedef struct {
      C8 const *src;
      C8 const *tail;
      bool success;
      U8 minP, maxP, avgP;
      bool gotPres, alert;
      } S_Tst;

   S_Tst const tsts[] = {
      { .src = "", .tail = NULL, .success = false, .minP = 0xFF, .maxP = 0xFF, .avgP = 0xFF, .gotPres = 0, .alert = 0},

      /* Fields are min/max/avg, in that order.
         Next field ':' and end-of-message '\r' or '\0' are legal terminators.
      */
      { .src = "010203;",  .tail = ";",  .success = true, .minP = 1, .maxP = 2, .avgP = 3, .gotPres = 1, .alert = 0},
      { .src = "010203\r", .tail = "\r", .success = true, .minP = 1, .maxP = 2, .avgP = 3, .gotPres = 1, .alert = 0},
      { .src = "010203",   .tail = "\0", .success = true, .minP = 1, .maxP = 2, .avgP = 3, .gotPres = 1, .alert = 0},

      // Reads Hex, case-insensitive.
      { .src = "ABCDEF",   .tail = "\0", .success = true, .minP = 0xAB, .maxP = 0xCD, .avgP = 0xEF, .gotPres = 1, .alert = 0},
      { .src = "abcdef",   .tail = "\0", .success = true, .minP = 0xAB, .maxP = 0xCD, .avgP = 0xEF, .gotPres = 1, .alert = 0},

      // 3 HexASCII digits must be terminated by ';', \r' or '\0' (above).
      { .src = "010203 ",  .tail = NULL, .success = false, .minP = 0xFF, .maxP = 0xFF, .avgP = 0xFF, .gotPres = 0, .alert = 0},

      // Must be exactly 6 digits.
      { .src = "01ABE;",   .tail = NULL, .success = false, .minP = 0xFF, .maxP = 0xFF, .avgP = 0xFF, .gotPres = 0, .alert = 0},
      { .src = "7788991;", .tail = NULL, .success = false, .minP = 0xFF, .maxP = 0xFF, .avgP = 0xFF, .gotPres = 0, .alert = 0},

      // Is HexASCII, no signs
      { .src = "-010203;",  .tail = NULL, .success = false, .minP = 0xFF, .maxP = 0xFF, .avgP = 0xFF, .gotPres = 0, .alert = 0},

      // An error in any position sets an alert.
      { .src = "FE0405;", .tail = ";", .success = true, .minP = 0xFF, .maxP = 4,    .avgP = 5,     .gotPres = 1, .alert = 1},
      { .src = "03FE05;", .tail = ";", .success = true, .minP = 3,    .maxP = 0xFF, .avgP = 5,     .gotPres = 1, .alert = 1},
      { .src = "0304FE;", .tail = ";", .success = true, .minP = 3,    .maxP = 4,    .avgP = 0xFF,  .gotPres = 1, .alert = 1},
      // All 3 bad -> alert and no pressure(s) acquired.
      { .src = "FEFEFE;", .tail = ";", .success = true, .minP = 0xFF, .maxP = 0xFF, .avgP = 0xFF,  .gotPres = 0, .alert = 1},
      // One good pressure sets 'gotPres'
      { .src = "65FEFE;", .tail = ";", .success = true, .minP = 0x65, .maxP = 0xFF, .avgP = 0xFF,  .gotPres = 1, .alert = 1},
      { .src = "FE81FE;", .tail = ";", .success = true, .minP = 0xFF, .maxP = 0x81, .avgP = 0xFF,  .gotPres = 1, .alert = 1},
      { .src = "FEFE12;", .tail = ";", .success = true, .minP = 0xFF, .maxP = 0xFF, .avgP = 0x12,  .gotPres = 1, .alert = 1},

      // Meter in Storage mode is NOT and alert.
      { .src = "FDFDFD;", .tail = ";", .success = true, .minP = 0xFD, .maxP = 0xFD, .avgP = 0xFD,  .gotPres = 0, .alert = 0},

   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      enc_S_MsgData ed = legalEmptyEncoder;

      C8 const *rtn = getXP(t->src, &ed);

      bool fail = false;

      C8 b0[100];
      sprintf(b0, "tst #%d.", i);

      // Parse should have succeeded, meaning returned non-NULL?
      if(t->success == true) {
         /* If parse did succeed then check for correct:
               - pressure(s)
               - data flags and alerts
               - tail returned.
         */
         if(rtn != NULL) {
            if(ed.noMag.pres._min != t->minP ) {
               printf("%s Expected minPres = %2.1fbar, got %2.1fbar\r\n", b0, t->minP/10.0, ed.noMag.pres._min/10.0);
               fail = true; }

            if(ed.noMag.pres._max != t->maxP ) {
               printf("%s Expected maxPres = %2.1fbar, got %2.1fbar\r\n", b0, t->maxP/10.0, ed.noMag.pres._max/10.0);
               fail = true; }

            if(ed.noMag.pres._avg != t->avgP ) {
               printf("%s Expected avgPres = %2.1fbar, got %2.1fbar\r\n", b0, t->avgP/10.0, ed.noMag.pres._avg/10.0);
               fail = true; }

            if(t->gotPres == true && ed.weGot.bs.pressure == 0 || t->gotPres == false && ed.weGot.bs.pressure == 1) {
               printf("%s gotPres; expected %d, got %d\r\n", b0, t->gotPres, ed.weGot.bs.pressure);
               fail = true; }

            if(strcmp(rtn, t->tail) != 0) {
               printf("%s Expected rtn \"%s\"\r\n, got \"%s\"\r\n", b0, t->tail, rtn);
               fail = true; }

            if(t->alert == true) {
               if( ed.alerts.noMag.bs.pressure == 0) {
                  printf("%s. Expected alert but was not set.\r\n",b0);
                  fail = true; }
            }
            else {
               if( ed.alerts.noMag.bs.pressure == 1) {
                  printf("%s. Unexpected alert.\r\n", b0);
                  fail = true; }
            }
         }
         // else should have parsed but did not.
         else {
            printf("%s Expected success but got NULL\r\n", b0);
            fail = true;
         }
      }
      // else parse should have failed, meaning returned NULL.
      else {
         if(rtn == NULL) {       // Expected NULL, and got it?
            // then there should be no temperatures and no alerts.
            if(ed.noMag.pres._min != 0xFF || ed.noMag.pres._max != 0xFF || ed.noMag.pres._avg != 0xFF) {
               printf("%s. Got NULL return (correct) but minPres = %2.1fbar maxPres = %2.1fbar avgPres = %2.1fbar; should all be 0xFF\r\n",
                      b0, ed.noMag.pres._min/10.0, ed.noMag.pres._max/10.0, ed.noMag.pres._avg/10.0); fail = true; }
            if(ed.alerts.noMag.bs.pressure == 1) {
               printf("%s. Got NULL return (correct) but also pressure alert (wrong)\r\n", b0); fail = true;}
            if(ed.weGot.bs.pressure == 1) {
               printf("%s. Got NULL return (correct) but data flags pres = %d set (wrong)\r\n", b0, ed.weGot.bs.pressure); fail = true; }
         }
         else {
            printf("%s Expected fail (NULL) but got \"%s\"\r\n", b0, rtn);
            fail = true; }
      }
      if(fail == true) {
         TEST_FAIL(); }
   }
}

/* ------------------------------ test_Sensus_DecodeMsg_NoMag ---------------------------------- */

void test_Sensus_DecodeMsg_NoMag(void)
{
   C8 b1[100];

   S_DecodeTst const tsts[] = {

      #define _NullPressures {._min = 0xFF, ._max = 0xFF, ._avg = 0xFF}

      #define _UnknownEncoder(_serialWord, _tot, _dials)                         \
            (enc_S_MsgData){                                                    \
               .encoderType = mNoEncoders, .weGot.bs = {.fwdTot = 1, .serWord = 1},     \
               .serialWord = _serialWord, .fwdTot = _tot, .dials = _dials,            \
               .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF} }

      #define _JustTotal(_tot, _dials)                         \
            (enc_S_MsgData){                                                    \
               .encoderType = mNoEncoders, .weGot.bs = {.fwdTot = 1},     \
               .serialWord = "", .fwdTot = _tot, .dials = _dials,            \
               .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF} }

      // Just an ADE.
      #define _ADE(_serialWord, _tot, _dials)                                    \
            (enc_S_MsgData){                                                     \
               .encoderType = mADE, .weGot.bs = {.fwdTot = 1, .serWord = 1},     \
               .serialWord = _serialWord, .fwdTot = _tot, .dials = _dials,       \
               .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF} }

      /* ---- No requested encoders.

         With any message, legal or no, this succeeds, returning the empty encoder.
      */
      {.msg = "",                               .filt = mNoEncoders,          .rtn = true,         .out = &legalEmptyEncoder },
      {.msg = "is garbage",                     .filt = mNoEncoders,          .rtn = true,         .out = &legalEmptyEncoder },
      {.msg = "V;RB123456;IBabc123\r",         .filt = mNoEncoders,          .rtn = true,         .out = &legalEmptyEncoder },

      {.msg = "",                               .filt = mADE,                 .rtn = false,        .out = &legalEmptyEncoder },

      /* ---- ADE   V;RBrrrrrr;IBsssssss<CR>

            is 4-6 digit totaliser and 1-7 character serialisation.
      */
      {.msg = "V;RB1234;IBabc123\r",           .filt = mADE,     .rtn = true,    .out = &_ADE("abc123", 1234, 4) },                    // 4 digits OK
      {.msg = "V;RB12345;IBabc123\r",          .filt = mADE,     .rtn = true,    .out = &_ADE("abc123", 12345, 5) },                   // 5 digits OK
      {.msg = "V;RB123456;IBabc123\r",         .filt = mADE,     .rtn = true,    .out = &_ADE("abc123", 123456, 6) },                  // 6 digits OK

      {.msg = "V;RB123;IBabc123\r",            .filt = mADE,     .rtn = false,   .out = &_UnknownEncoder("abc123", 123,   3) },        // 3 digits bad
      {.msg = "V;RB1234567890;IBabc123\r",     .filt = mADE,     .rtn = false,   .out = &legalEmptyEncoder },                          // 10 digits bad

      {.msg = "V;RB1234;IBabcdef\r",           .filt = mADE,     .rtn = true,    .out = &_ADE("abcdef", 123456, 6) },                  // 6 char serial OK
      {.msg = "V;RB123456;IBabcdef\r",         .filt = mADE,     .rtn = true,    .out = &_ADE("abcdef", 123456, 6) },                  // 6 char serial OK
      {.msg = "V;RB123456;IBabcdefg\r",        .filt = mADE,     .rtn = true,    .out = &_ADE("abcdefg", 123456, 6) },                 // 7 chars OK

      {.msg = "V;RB123456;IB\r",               .filt = mADE,     .rtn = false,   .out = &_JustTotal(123456, 6) },                      // Zero chars bad
      {.msg = "V;RB123456;IBabcde\r",          .filt = mADE,     .rtn = false,   .out = &_UnknownEncoder("abcde", 123456, 6) },        // 5 chars bad
      {.msg = "V;RB123456;IBabcdefgh\r",       .filt = mADE,     .rtn = true,    .out = &_ADE("abcdefgh", 123456, 6) },                // 8 chars OK.

      {.msg = "V;RBc123456;IBabc123\r",         .filt = mADE,     .rtn = false,  .out = &legalEmptyEncoder },                       // Digits must start right after 'RB'
      {.msg = "V;RBc123456;IBabc123\r",         .filt = mADE,     .rtn = false,  .out = &legalEmptyEncoder },                       // Digits must start right after 'RB'
      {.msg = "V;RBc123456a;IBabc123\r",        .filt = mADE,     .rtn = false,  .out = &legalEmptyEncoder },                       // No chars in the totaliser field.
      {.msg = "V;RB123 456;IBabc123\r",         .filt = mADE,     .rtn = false,  .out = &_JustTotal(123, 3) },                      // Gap in totaliser; abort with partial number.

      {.msg = "V;RB123456;IBabc def\r",         .filt = mADE,     .rtn = false,                    // Malformed serialisation is captured as far as it goes...
            .out = &(enc_S_MsgData){ .encoderType = mNoEncoders, .weGot.bs = {.fwdTot = 1},        // but not accepted.
                                     .serialWord = "abc", .fwdTot = 123456, .dials = 6,
                                     .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}},

      {.msg = "V;RB123456;IBabc_def\r",         .filt = mADE,     .rtn = false,                    // Malformed serialisation is captured as far as it goes...
            .out = &(enc_S_MsgData){ .encoderType = mNoEncoders, .weGot.bs = {.fwdTot = 1},        // but not accepted.
                                     .serialWord = "abc", .fwdTot = 123456, .dials = 6,
                                     .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}},

      /* ---- HRE    V;RBrrrrrrrrr;IBssssssssss;Mbbbb?!<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, 16bit status in Mbbbb?!

      */
      {.msg = "V;RB123456789;IBabcdefghij;M8000?!\r",
               .filt = mHRE,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mHRE, .weGot.bs = {.fwdTot = 1, .serWord = 1, .alerts = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .fwdTot = 123456789, .dials = 9,
                  .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}   },


      /* ---- HRE-LCD    V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, flow% in ';GCaa' 24bit (extended) status in ';Mbbbbbb,...'
            reverse total in 'xxxxxx' of ;Mbbbbbb,xxxxxx'
      */
      {.msg = "V;RB123456789;IBabcdefghij;GC28;M008000,987654\r",
               .filt = mHRE_LCD,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mGen1|mHRE_LCD,
                  .weGot.bs = {.fwdTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .fwdTot = 123456789, .dials = 9,
                  .flowPcent = 28,
                  .noMag = {.revTot = 9991764UL, .pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}   },

      /* ---- E-Series Gen1   V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, flow% in ';GCaa' 24bit (extended) status in ';Mbbbbbb,...'
            reverse total in 'xxxxxx' of ;Mbbbbbb,xxxxxx', fluid temperature in ';XTddd', owner-number in ';Kyyyyyyyyyy'.
      */
      {.msg = "V;RB123456789;IBabcdefghij;GC28;M008000,54DC12;XT048;Kmnpqrstuvw\r",
               .filt = mGen1,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mGen1,
                  .weGot.bs = {.fwdTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1, .fluidTmpr = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .fwdTot = 123456789, .dials = 9, .kStr = "mnpqrstuvw",
                  .flowPcent = 28,
                  .noMag = {
                     .revTot = 0x54DC12,
                     .pres = _NullPressures,
                     .fluidDegC = 48, .ambientDegC = 0xFF}}   },

      /* ---- E-Series Gen2, with fluid temperature   V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy;XPiijjkk<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, flow% in ';GCaa' 24bit (extended) status in ';Mbbbbbb,...'
            reverse total in 'xxxxxx' of ;Mbbbbbb,xxxxxx', fluid temperature in ';XTddd', owner-number in ';Kyyyyyyyyyy',
            min/max/avg pressures in ';XPiijjkk'.
      */
      {.msg = "V;RB123456789;IBabcdefghij;GC28;M008000,54DC12;XT048;Kmnpqrstuvw;XP123456\r",
               .filt = mGen2,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mGen2,
                  .weGot.bs = {.fwdTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1, .fluidTmpr = 1, .pressure = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .fwdTot = 123456789, .dials = 9, .kStr = "mnpqrstuvw",
                  .flowPcent = 28,
                  .noMag = {
                     .revTot = 0x54DC12,
                     .pres = _NullPressures,
                     .fluidDegC = 48, .ambientDegC = 0xFF,
                     .pres = {._min = 0x12, ._max = 0x34, ._avg = 0x56}}}},

      /* ---- E-Series Gen2, with fluid and ambient temperature
                  V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy;XPiijjkk<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, flow% in ';GCaa' 24bit (extended) status in ';Mbbbbbb,...'
            reverse total in 'xxxxxx' of ;Mbbbbbb,xxxxxx', fluid temperature in ';XTddd', owner-number in ';Kyyyyyyyyyy',
            min/max/avg pressures in ';XPiijjkk'.
      */
      {.msg = "V;RB123456789;IBabcdefghij;GC28;M008000,54DC12;XT4821;Kmnpqrstuvw;XP123456\r",
               .filt = mGen2,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mGen2,
                  .weGot.bs = {.fwdTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1, .fluidTmpr = 1, .ambientTmpr = 1, .pressure = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .fwdTot = 123456789, .dials = 9, .kStr = "mnpqrstuvw",
                  .flowPcent = 28,
                  .noMag = {
                     .revTot = 0x54DC12,
                     .pres = _NullPressures,
                     .fluidDegC = 72, .ambientDegC = 33,
                     .pres = {._min = 0x12, ._max = 0x34, ._avg = 0x56}}}},
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_DecodeTst const *t = &tsts[i];

      enc_S_MsgData ed = *aGarbageEncoder();
      bool rtn = Sensus_DecodeMsg(t->msg, &ed, t->filt);

      C8 b2[500], b3[500];

      if(t->rtn == false) {
         if(rtn == false) {
            if(false == Sensus_EncodersEqual(t->out, &ed, _sens_EncodersEqual_IgnoreMag)) {
               printf("%s; expected false But also expected:\r\n      %s   instead got:\r\n      %s",
                        testMessagePremable(b1, i, t, rtn), Sensus_PrintMsgData(b2, t->out), Sensus_PrintMsgData(b3, &ed));
               TEST_FAIL(); }
         }
         else {
            printf("%s but expected false, returned\r\n       %s", testMessagePremable(b1, i, t, rtn), Sensus_PrintMsgData(b2, &ed));
            TEST_FAIL();
         }

      }
      else {
         if(rtn == false) {
            printf("%s but expected true. returned\r\n       %s ", testMessagePremable(b1, i, t, rtn), Sensus_PrintMsgData(b2, &ed));
            TEST_FAIL();
         }
         else {
            if(false == Sensus_EncodersEqual(t->out, &ed, _sens_EncodersEqual_IgnoreMag)) {
               printf("%s.  But expected:\r\n      %s   instead got:\r\n      %s",
                  testMessagePremable(b1, i, t, rtn), Sensus_PrintMsgData(b2, t->out), Sensus_PrintMsgData(b3, &ed));
               TEST_FAIL(); }}
         }
   } // for(U8 i = 0; i < RECORDS_IN(tsts); i++)
}

// ----------------------------------------- eof --------------------------------------------
