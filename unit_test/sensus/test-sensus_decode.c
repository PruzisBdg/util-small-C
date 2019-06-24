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
   typedef struct { C8 const *src; C8 const *tail; bool success; enc_T_degC fluidTmpr, ambTmpr; } S_Tst;

   S_Tst const tsts[] = {
      {.src = "", .tail = NULL, .success = false, .fluidTmpr = 0xFF, .ambTmpr = 0xFF },
   };

   for(U8 i = 0; i < RECORDS_IN(tsts); i++)
   {
      S_Tst const *t = &tsts[i];

      enc_S_MsgData ed;
      ed = legalEmptyEncoder;

      C8 const *rtn = getXT(t->src, &ed);

      if(t->success == true) {
         if(rtn != NULL) {
            if(strcmp(rtn, t->tail) != 0) {
               printf("tst #%d Expected rtn %s, got %s", i, t->tail, rtn);
            }
         }
         else {
            printf("tst #%d Expected success but got NULL\r\n", i);
            TEST_FAIL();
         }
      }
      else {
         if(rtn == NULL) {

         }
         else {
            printf("tst #%d Expected fail (NULL) but got %s\r\n", i, rtn);
            TEST_FAIL();

         }
      }

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
               .encoderType = mNoEncoders, .weGot.bs = {.rawTot = 1, .serWord = 1},     \
               .serialWord = _serialWord, .rawTot = _tot, .dials = _dials,            \
               .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF} }

      #define _JustTotal(_tot, _dials)                         \
            (enc_S_MsgData){                                                    \
               .encoderType = mNoEncoders, .weGot.bs = {.rawTot = 1},     \
               .serialWord = "", .rawTot = _tot, .dials = _dials,            \
               .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF} }

      // Just an ADE.
      #define _ADE(_serialWord, _tot)                                            \
            (enc_S_MsgData){                                                    \
               .encoderType = mADE, .weGot.bs = {.rawTot = 1, .serWord = 1},     \
               .serialWord = _serialWord, .rawTot = _tot, .dials = 6,            \
               .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF} }

      /* ---- No requested encoders.

         With any message, legal or no, this succeeds, returning the empty encoder.
      */
      {.msg = "",                               .filt = mNoEncoders,          .rtn = true,         .out = &legalEmptyEncoder },
      {.msg = "is garbage",                     .filt = mNoEncoders,          .rtn = true,         .out = &legalEmptyEncoder },
      {.msg = "V;RB123456;IBabc123\r",         .filt = mNoEncoders,          .rtn = true,         .out = &legalEmptyEncoder },

      {.msg = "",                               .filt = mADE,                 .rtn = false,        .out = &legalEmptyEncoder },

      /* ---- ADE   V;RBrrrrrr;IBsssssss<CR>

            is 6 digit totaliser and 1-7 character serialisation.
      */
      {.msg = "V;RB123456;IBabc123\r",         .filt = mADE,     .rtn = true,    .out = &_ADE("abc123", 123456) },                  // 6 digits OKs

      {.msg = "V;RB12345;IBabc123\r",          .filt = mADE,     .rtn = false,   .out = &_UnknownEncoder("abc123", 12345,   5) },   // 5 digits bad
      {.msg = "V;RB1234567;IBabc123\r",        .filt = mADE,     .rtn = false,   .out = &_UnknownEncoder("abc123", 1234567, 7) },   // 7 digits bad

      {.msg = "V;RB123456;IBa\r",              .filt = mADE,     .rtn = true,    .out = &_ADE("a", 123456) },                       // 1 char serial OK
      {.msg = "V;RB123456;IBabcdefg\r",        .filt = mADE,     .rtn = true,    .out = &_ADE("abcdefg", 123456) },                 // 7 chars OK

      {.msg = "V;RB123456;IB\r",               .filt = mADE,     .rtn = false,   .out = &_JustTotal(123456, 6) },                   // Zero chars bad
      {.msg = "V;RB123456;IBabcdefgh\r",       .filt = mADE,     .rtn = false,   .out = &_UnknownEncoder("abcdefgh", 123456, 6) },  // 8 chars bad

      {.msg = "V;RBc123456;IBabc123\r",         .filt = mADE,     .rtn = false,  .out = &legalEmptyEncoder },                       // Digits must start right after 'RB'
      {.msg = "V;RBc123456;IBabc123\r",         .filt = mADE,     .rtn = false,  .out = &legalEmptyEncoder },                       // Digits must start right after 'RB'
      {.msg = "V;RBc123456a;IBabc123\r",        .filt = mADE,     .rtn = false,  .out = &legalEmptyEncoder },                       // No chars in the totaliser field.
      {.msg = "V;RB123 456;IBabc123\r",         .filt = mADE,     .rtn = false,  .out = &_JustTotal(123, 3) },                      // Gap in totaliser; abort with partial number.

      {.msg = "V;RB123456;IBabc def\r",         .filt = mADE,     .rtn = false,                    // Malformed serialisation is captured as far as it goes...
            .out = &(enc_S_MsgData){ .encoderType = mNoEncoders, .weGot.bs = {.rawTot = 1},        // but not accepted.
                                     .serialWord = "abc", .rawTot = 123456, .dials = 6,
                                     .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}},

      {.msg = "V;RB123456;IBabc_def\r",         .filt = mADE,     .rtn = false,                    // Malformed serialisation is captured as far as it goes...
            .out = &(enc_S_MsgData){ .encoderType = mNoEncoders, .weGot.bs = {.rawTot = 1},        // but not accepted.
                                     .serialWord = "abc", .rawTot = 123456, .dials = 6,
                                     .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}},

      /* ---- HRE    V;RBrrrrrrrrr;IBssssssssss;Mbbbb?!<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, 16bit status in Mbbbb?!

      */
      {.msg = "V;RB123456789;IBabcdefghij;M8000?!\r",
               .filt = mHRE,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mHRE, .weGot.bs = {.rawTot = 1, .serWord = 1, .alerts = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .rawTot = 123456789, .dials = 9,
                  .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}   },


      /* ---- HRE-LCD    V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, flow% in ';GCaa' 24bit (extended) status in ';Mbbbbbb,...'
            reverse total in 'xxxxxx' of ;Mbbbbbb,xxxxxx'
      */
      {.msg = "V;RB123456789;IBabcdefghij;GC28;M008000,987654\r",
               .filt = mHRE_LCD,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mGen1|mHRE_LCD,
                  .weGot.bs = {.rawTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .rawTot = 123456789, .dials = 9,
                  .flowPcent = 28,
                  .noMag = {.revTot = 9991764UL, .pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}   },

      /* ---- E-Series Gen1   V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy<CR>

            is 6-9 digit totaliser; 1-10 char serialisation, flow% in ';GCaa' 24bit (extended) status in ';Mbbbbbb,...'
            reverse total in 'xxxxxx' of ;Mbbbbbb,xxxxxx', fluid temperature in ';XTddd', owner-number in ';Kyyyyyyyyyy'.
      */
      {.msg = "V;RB123456789;IBabcdefghij;GC28;M008000,54DC12;XT048;Kmnpqrstuvw\r",
               .filt = mGen1,  .rtn = true,  .out = &(enc_S_MsgData){
                  .encoderType = mGen1,
                  .weGot.bs = {.rawTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1, .fluidTmpr = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .rawTot = 123456789, .dials = 9, .kStr = "mnpqrstuvw",
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
                  .weGot.bs = {.rawTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1, .fluidTmpr = 1, .pressure = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .rawTot = 123456789, .dials = 9, .kStr = "mnpqrstuvw",
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
                  .weGot.bs = {.rawTot = 1, .serWord = 1, .alerts = 1, .uom = 1, .flowPcent = 1, .meterType = 1, .fluidTmpr = 1, .ambientTmpr = 1, .pressure = 1},
                  .alerts.noMag.bs.overflow = 1,
                  .serialWord = "abcdefghij", .rawTot = 123456789, .dials = 9, .kStr = "mnpqrstuvw",
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
