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

      // ADE is 6 digit totaliser and 1-7 character serialisation
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

      // HRE
      {.msg = "V;RB123456789;IBabcdefghij;M8000?!\r",         .filt = mHRE,     .rtn = true,
            .out = &(enc_S_MsgData){ .encoderType = mHRE, .weGot.bs = {.rawTot = 1, .serWord = 1, .alerts = 1},
                                     .serialWord = "abcdefghij", .rawTot = 123456789, .dials = 9,
                                     .noMag = {.pres = _NullPressures, .fluidDegC = 0xFF, .ambientDegC = 0xFF}}   },                  // 6 digits OKs

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
