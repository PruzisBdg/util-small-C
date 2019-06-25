/* ------------------------------ BMI Sensus Codec  ---------------------------------------------------

   Codec for Sensus-style messages to/from Badger Water Meters. Right now it's just the decoder.
*/

#ifndef SENSUS_CODEC_H
#define SENSUS_CODEC_H

#include "libs_support.h"

#define _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT

// ======================================== Batch Decoder ========================================

typedef U32 enc_T_Tot;           // Raw Meter readings; up to 9 digits
typedef U8  enc_T_FlowPcent;     // Percent of max flow
typedef U8  enc_T_Pressure;      // In 0.1bar
typedef S16 enc_T_degC;          // In degC.
typedef U8  enc_T_MeterSize;     // Size in b[5:0] of the 32 bit M-Field
typedef U8  enc_T_MeterType;     // Size/type in the top byte of the 24bit M-Field
typedef U8  enc_T_Resolution;    // x1,x0.1,x10 etc
typedef U8  enc_T_ProdCode;      // b[15:12] of 32bit M-field
typedef U8  enc_T_UOM;           // m3, ft3, gal etc.
typedef U8  enc_T_MeasMode;      // Unidirectional or bidirectional. For Mag Meters.
typedef U8  enc_T_FlowDir;       // Forward or reverse.

// The kind of Encoder message.'m' <-> masks so decoder can filter for multiple message-types.
typedef enum {
   mNoEncoders = 0x00,
   mADE        = 0x01,
   mGen1       = 0x02,
   mGen2       = 0x04,
   mHRE        = 0x08,
   mHRE_LCD    = 0x10,
   mMag        = 0x20,
   mAnyEncoder = 0xFF
} enc_M_EncType;

// Will be set if this alert occurs in the message; otherwise clear.
            #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
typedef union {
   U16   asU16;
   struct { U16
      adcError    :1,
      lowBatt     :1,
      badCoilDrive :1,
      measTimeout :1,
      flowStim    :1,
      ovStatus    :1,
      maxFlow     :1,
      badSensor   :1; } bs;
} enc_S_MagAlerts;
         #endif // _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT

typedef union {
   U16 asU16;
   struct { U16
         overflow    :1,      // of the totaliser
         pressure    :1,      // Any of the min.max or average pressure tagged as bad.
         reverseFlow :1,
         negFlowRate :1,      // the current flow rate is negative ('GCnn') is negative.
         tamper      :1,
         leak        :1,
         program     :1,      // 'Program' error in basic status.
         temperature :1,
         endOfLife   :1,      // Usually battery is dead.
         emptyPipe   :1,
         noFlow      :1; } bs;     // i.e no usage for some time.
} enc_S_NonMagAlerts;

typedef struct __attribute__((packed)) {
   enc_S_NonMagAlerts   noMag;
         #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
   enc_S_MagAlerts      mag;
         #endif // _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
} enc_S_Alerts;

// Will be set if the corresponding field in enc_S_MsgData was updated from the Encoder message.
typedef union {
   U16   asU16;
   struct {
      U16   serWord     :1,
            rawTot      :1,
            flowPcent   :1,
            pressure    :1,
            fluidTmpr   :1,
            ambientTmpr :1,
            meterSize   :1,
            meterType   :1,
            res         :1,
            prodCode    :1,
            uom         :1,
            measMode    :1,
            alerts      :1;
   } bs;
} enc_S_WotWeGot;

#define _enc_MaxSerNumChars 10

typedef struct { enc_T_Pressure _min, _max, _avg;  } S_Pres;

typedef struct {
   enc_M_EncType     encoderType;                        // From the message format i.e HRE, Gen2 etc.
   enc_S_WotWeGot    weGot;                              // Which of the following variables were read form the message
   enc_S_Alerts      alerts;
   C8                serialWord[_enc_MaxSerNumChars+1];  // serial-word 'RBrrrrrrrrr;'. Up to 10 digits & letters.
   C8                kStr[_enc_MaxSerNumChars+1];        // The 'ownership number' ';Knnnnnnnnn' up to 10 letters & digits..
   enc_T_Tot         rawTot;                             // RBrrrrrrrrr;IBssssssssss
   U8                dials;                              // Digits in ';RBnnnnnnnn'. Usually >= 6; no more than 9.
   enc_T_FlowPcent   flowPcent;

   struct {
      enc_T_Tot      revTot;                             // Reverse total; from 'xxxxxx' in ';Mbbbbbb,xxxxxx'
      S_Pres         pres;
      enc_T_degC     fluidDegC,
                     ambientDegC;
      } noMag;

   enc_T_MeterType   meterType;                          // Size/type in the top byte of the 24bit M-Field

   enc_T_UOM         uom;
      #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
   struct {
      enc_T_Tot         secTot;                          // the secondary totaliser in 'Mbbbbbbbb,xxxxxxxx'.
      enc_T_ProdCode    prodCode;                        // b[15:12] of 32bit M-field
      enc_T_Resolution  res;
      enc_T_MeterSize   meterSize;                       // Size in b[5:0] of the 32 bit M-Field
      bool              biDirectional;                   // Mag Meter measurement mode.
   } mag;
      #endif
} enc_S_MsgData;

PUBLIC bool Sensus_DecodeMsg(C8 const *src, enc_S_MsgData *ed, enc_M_EncType filterFor);

// ===================================== ends: Batch Decoder ========================================'



/* ==================================== Stream Decoder ========================================

   Accepts byte stream then calls batch decoder.
*/

typedef struct {U8 _min, _max; } S_MinMaxU8;
// For the E-Series (Gen2)
#define _enc_LongestMsg_chars \
   sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTffaa;Kyyyyyyyyyy;XPiijjkk\r")

typedef struct {
   enc_M_EncType filterFor;                        // Zero of more message types to expect.
   U8             put;                             // i.e buf[put].
   S_MinMaxU8     msgLimits;                       // longest and shorted expected messages (given 'filterFor')
   C8             buf[_enc_LongestMsg_chars+1];    // Add incoming Sensus chars here.
} enc_S_StreamDecode;

// Results of next input e.g char to a stream parser.
typedef enum {
   eParseStream_Fail    = 0,     // This (char) failed to parse; we're done
   eParseStream_Done    = 1,     // Parse is complete
   eParseStream_Continue = 2,     // More input needed.
   eParseStream_Fault   = 3      // Broke/overloaded the parser itself (which shouldn't happen unless e.g malloc() issue)
} enc_E_StreamState;

PUBLIC bool Sensus_DecodeStart(enc_S_StreamDecode *dc, enc_M_EncType filterFor);
PUBLIC enc_E_StreamState Sensus_DecodeStream(enc_S_StreamDecode *dc, enc_S_MsgData *ed, C8 ch);

// ===================================== ends: Stream Decoder ========================================

// ============================ Exported just for Test Harness =========================================
_EXPORT_FOR_TEST bool decodeBasicStatus(U16 mf, enc_S_MsgData *ed);
_EXPORT_FOR_TEST C8 const * getXT(C8 const *src, enc_S_MsgData *ed );
_EXPORT_FOR_TEST C8 const * getXP(C8 const *src,  enc_S_MsgData *ed );

// =================================== Test harness support ===========================================

PUBLIC C8 const * Sensus_PrintMsgData(C8 *out, enc_S_MsgData const *ed);

#define _sens_EncodersEqual_IgnoreMag  false
#define _sens_EncodersEqual_ChkMag     true
PUBLIC bool Sensus_EncodersEqual(enc_S_MsgData const *a, enc_S_MsgData const *b, bool chkMag);
PUBLIC C8 const * sens_ShowEncoders(C8 *out, enc_M_EncType t);
PUBLIC C8 const * sens_ShowAlerts(C8 *out, enc_S_Alerts const *a);

#endif // SENSUS_CODEC_H

// ------------------------------------ eof ------------------------------------------------------------
