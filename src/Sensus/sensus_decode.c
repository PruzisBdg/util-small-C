/* ----------------------------------------------------------------------------------------------

  Decoder for 'Sensus' format messages from (Badger) Water Meters.


-------------------------------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>
#include "arith.h"
#include <string.h>

typedef U32 Sens_T_Tot;          // Raw Meter readings; up to 9 digits
typedef U8  Sens_T_FlowPcent;    // Percent of max flow
typedef U8  Sens_T_Pressure;     // In 0.1bar
typedef U8  Sens_T_MeterSize;    // Size in b[5:0] of the 32 bit M-Field
typedef U8  Sens_T_MeterType;    // Size/type in the top byte of the 24bit M-Field
typedef U8  Sens_T_Resolution;
typedef U8  Sens_T_ProdCode;     // b[15:12] of 32bit M-field
typedef U8  Sens_T_UOM;
typedef U8  Sens_T_MeasMode;
typedef U8  Sens_T_FlowDir;
typedef S16 Sens_T_degC;         // In degC.

// The kind of Encoder message.'m' <-> masks
typedef enum {
   mADE        = 0x01,
   mGen1       = 0x02,
   mGen2       = 0x04,
   mHRE        = 0x08,
   mHRE_LCD    = 0x10,
   mMag        = 0x20,
   mAnyEncoder = 0xFF
} Sens_M_EncType;

typedef enum {
   eMeterType_Undefined = 0,
} Sens_E_MeterType;

// Will be set if this alert occurs in the message; otherwise clear.
typedef struct {
   U32   overflow    :1,
         pressure    :1,
         reverseFlow :1,
         tamper      :1,
         leak        :1,
         program     :1,
         temperature :1,
         endOfLife   :1,
         emptyPipe   :1,
         noFlow      :1,
         // Mag errors.
         adcError    :1,
         lowBatt     :1,
         badCoilDrive :1,
         measTimeout :1,
         flowStim    :1,
         ovStatus    :1,
         maxFlow     :1,
         badSensor   :1;
} Sens_S_Alerts;

// Will be set if the corresponding field in S_EncoderMsgData was updated from the Encoder message.
typedef struct {
   U16   serNum      :1,
         rawTot      :1,
         flowPcent   :1,
         fluidTmpr   :1,
         ambientTmpr :1,
         meterSize   :1,
         meterType   :1,
         res         :1,
         prodCode    :1,
         uom         :1,
         measMode    :1,
         flowDir     :1,
         alerts      :1;
} Sens_S_WotWeGot;

#define _enc_MaxSerNumChars 10

typedef struct {
   Sens_M_EncType    encoderType;                        // From the message format i.e HRE, Gen2 etc.
   Sens_S_WotWeGot   weGot;                              // Which of the following variables were read form the message
   C8                serialStr[_enc_MaxSerNumChars+1];   // serial-string 'RBrrrrrrrrr;'. Up to 10 digits & letters.
   C8                kStr[_enc_MaxSerNumChars+1];        // The 'ownership number' ';Knnnnnnnnn' up to 10 letters & digits..
   Sens_T_Tot        rawTot,                             // RBrrrrrrrrr;IBssssssssss
                     revTot,
                     secTot;                             // For Mag meters, the secondary totaliser in 'Mbbbbbbbb,xxxxxxxx'.
   Sens_T_FlowPcent  flowPcent;
   struct { Sens_T_Pressure _min, _max, _avg; } pres;
   Sens_T_degC       fluidDegC,
                     ambientDegC;
   Sens_T_MeterSize  meterSize;                          // Size in b[5:0] of the 32 bit M-Field
   Sens_T_MeterType  meterType;                          // Size/type in the top byte of the 24bit M-Field
   Sens_T_Resolution res;
   Sens_T_ProdCode   prodCode;                           // b[15:12] of 32bit M-field
   Sens_T_UOM        uom;
   Sens_T_MeasMode   measMode;
   Sens_T_FlowDir    flowDir;
   Sens_S_Alerts     alerts;
   bool              biDirectional;                      // Mag Meter measurement mode.
} S_EncoderMsgData;

/* --------------------------------- startField --------------------------------------------------

   Gets e.g ":IB". where 'tag' = "IB". 'tag' may be 1 or 2 letters followed by space or '\0'

   Return the char after 'tag', NULL if no match.
*/
_EXPORT_FOR_TEST C8 const *startField(C8 const *src, C8 const *tag) {
   return
      src[0] != ';' || src[1] != tag[0]
         ? NULL
         : (tag[1] == '\0' || tag[1] == ' '
            ? src+2
            : (src[2] != tag[1]
               ? NULL
               : src+3 )); }

/* ---------------------------------- endMsg -------------------------------------------------

   <CR> or '\0' finishes an encoder message.
*/
static bool inline endsMsg(C8 ch) {
   #define CR 0x13
   return ch == '\0' || ch == CR; }

static bool inline endMsg(C8 const *src) {
   return endsMsg(src[0]); }


/* --------------------------------- endField ----------------------------------------------------

   Return true if 'src' is ';' which ends/start  a field or <CR> or '\0' which ends and encoder
   message.
*/
PRIVATE bool endField(C8 const *src) {
   return src[0] == ';' || endMsg(src); }


/* --------------------------------- getSerialStr ---------------------------------------------------

   Given serial number "ssssssssss(:|'\0'|<CR>)", get "ssssssssss" into 'out'.

   "ssssssssss" must be 1-10 alphanumeric chars ('_enc_MaxSerNumChars') immediately followed by one
   ';', '\0' or <CR>.

   If success then return on the trailing ';'; else return NULL.
*/
_EXPORT_FOR_TEST C8 const * getSerialStr(C8 const *src, C8 *out)
{
   for(U8 i = 0; i < _enc_MaxSerNumChars; i++, src++, out++) {    // From 'src', up to 10 (alphanumeric) chars
      if( endField(src) == true ) {                               // Field delimiter ie ';', <CR> or '\0'?
         *out = '\0';                                             // Terminate whatever we built in 'out'.
         return i == 0                                            // Didn't even get 1 char?
            ? NULL                                                // then fail, serial number cannot be empty.
            : src; }                                              // else return on end of the field we read.
      else if(isalnum(*src)) {                                    // else another char of the serial number?...
         *out = *src; }}                                           // ...copy it to 'out'.

   return NULL;      // Got a non-alphanumeric before filed delimiter.
}

typedef struct {U8 _min, _max; } S_U8Range;

/* --------------------------------- reqHexASCIIbytes ---------------------------------------------

*/
_EXPORT_FOR_TEST C8 const * reqHexASCIIbytes(C8 const *src, U32 *out, S_U8Range const *r, U8 *bytesGot) {
   if(NULL != (src = GetNextHexASCII_U8toU32(src, out, bytesGot))) {
      if(*bytesGot >= r->_min && *bytesGot <= r->_max) {
         return src; }}
   return NULL; }

/* -------------------------------- getDualMfield --------------------------------------------------

   Get 24bit or 32bit dual M-field i.e 'Mbbbbbb,xxxxxx' OR 'Mbbbbbbbb,xxxxxxxx' from 'src' into
   'aFld' and 'bFld'

   'a' and 'b' fields must both be 24bit or both be 32bit.

   Return at the char after the 2nd field; NULL if parse failed.

   If fail then 'aFld', 'bFld', 'got32bit' are undefined.
*/
_EXPORT_FOR_TEST C8 const * getDualMfield(C8 const *src, U32 *aFld, U32 *bFld, U8 *bytesGot) {
   U8 aBytes;
   S_U8Range const _3or4 = {._min = 3, ._max = 4};

   if( NULL != (src = reqHexASCIIbytes(src, aFld, &_3or4, &aBytes))) {
      if(*src == ',') {
         U8 bBytes;
         if( NULL != (src = reqHexASCIIbytes(src, bFld, &_3or4, &bBytes))) {
            if(aBytes == bBytes) {
               *bytesGot = aBytes;
               return src; }}}}
   return NULL; }

// -----------------------------------------------------------------------------------------------
static U8 bitAtU32(U32 n, U8 bit) {
   return (n & (1UL << bit)) != 0 ? 1 : 0; }

// -----------------------------------------------------------------------------------------------
static U8 bitsAtU32(U32 n, U8 msb, U8 lsb) {
   return (n && MakeAtoBSet_U32(msb, lsb)) >> lsb; }


/* ------------------------------ decode24bit_MField -----------------------------------------------

   Decode into 'ed' 24bit M-field in the 3 lower bytes of 'mf'.
*/
_EXPORT_FOR_TEST bool decode24bit_MField(U32 mf, S_EncoderMsgData *ed) {

   ed->meterSize = LOW_BYTE(HIGH_WORD(mf));
   ed->uom = bitsAtU32(mf, 11, 9);

   Sens_S_Alerts a;
   a.overflow     = bitAtU32(mf, 15);
   a.pressure     = bitAtU32(mf, 14);
   a.tamper       = bitAtU32(mf, 7);
   a.program      = bitAtU32(mf,6);
   a.leak         = bitAtU32(mf,5);
   a.reverseFlow  = bitAtU32(mf,4);
   a.noFlow       = bitAtU32(mf,3);
   a.endOfLife    = bitAtU32(mf,2);
   a.temperature  = bitAtU32(mf,1);
   a.emptyPipe    = bitAtU32(mf,0);
   ed->alerts = a;

   return true;   // Always succeeds, for now.
}

/* ------------------------------ decodeMag_MField -----------------------------------------------

   Decode into 'ed' 32bit M-field in 'mf'.
*/
typedef enum {
   eMag_Unknown = 0, eMag_M2000 = 1, eMag_M5000 = 2, eMag_M1500 = 3, eMag_M1000 = 4,
   eMag_M5000B = 5, eMag_Utility = 6, eMag_Isonic4000 = 7, eMag_TFX_500W = 8
} E_MagProdCode;

_EXPORT_FOR_TEST bool decodeMag_MField(U32 mf, S_EncoderMsgData *ed) {

   ed->prodCode = bitsAtU32(mf, 15, 12);

   Sens_S_Alerts *a = &ed->alerts;

   if(ed->prodCode == eMag_M2000 || ed->prodCode == eMag_M1000 || ed->prodCode == eMag_M5000 || ed->prodCode == eMag_Utility) {
      a->maxFlow   = bitAtU32(mf, 27);
      a->adcError  = bitAtU32(mf, 26);
      a->emptyPipe = bitAtU32(mf, 25);
      a->badSensor = bitAtU32(mf, 24); }

   if(ed->prodCode == eMag_M2000) {
      a->flowStim = bitAtU32(mf, 30);
      a->overflow = bitAtU32(mf, 29);
      a->ovStatus = bitAtU32(mf, 28);
      }
   else if(ed->prodCode == eMag_M1000 || ed->prodCode == eMag_M5000 || ed->prodCode == eMag_M5000B ) {
      a->badCoilDrive = bitAtU32(mf,30);
      a->measTimeout = bitAtU32(mf,29);
      }
   else if(ed->prodCode == eMag_Utility) {
      a->endOfLife = bitAtU32(mf,30);
      a->measTimeout = bitAtU32(mf,29);
      a->leak = bitAtU32(mf,23);
      a->reverseFlow = bitAtU32(mf,22); }

   ed->uom = bitsAtU32(mf, 11, 8);
   ed->measMode = bitAtU32(mf, 7);
   ed->alerts.reverseFlow = bitAtU32(mf, 6);
   ed->meterSize = bitsAtU32(mf, 5, 0);
   ed->biDirectional = bitAtU32(mf,7);
   return true;      // Always succeeds, for now.
}

/* ----------------------------------- getXT -----------------------------------------------------

   Get either of the 2 temperature fields (below) from 'src' into 'xt'

      - 'XTddd',     (type 1) where ddd is a single signed decimal temperature
      - 'XTffss',    (type 2) where 'ff' and 'aa' are fluid and ambient temperatures,
                              (each in signed 8-bit HexASCII).

   This given 'src' is on the char after 'XT' i.e at 'ddd' or 'ffaa'. 'xt.isType2' says which we got.

   Return on the 1st char after the field; NULL if fail.
*/
typedef struct {
   bool isType2;        // i.e'XTffaa
   union {
      S16 degC;
      struct { U8 fluid, ambient; } type2;
   } payload;
} S_XT;

_EXPORT_FOR_TEST C8 const * getXT(C8 const *src, S_XT *xt ) {

   /* For 'XT'
   */
   if(isalnum(src[2]) && isalnum(src[2]) && !isalnum(src[4])) {
      U16 n;
      if(NULL != GetNextHexASCII_U16(src, &n)) {
         xt->isType2 = true;
         xt->payload.type2.fluid = HIGH_BYTE(n);
         xt->payload.type2.ambient = LOW_BYTE(n);
         return src + 4; }}

   else if( isdigit(src[1]) && isdigit(src[2])) {
      if( NULL != ReadDirtyASCIIInt(src, &xt->payload.degC) ) {
         xt->isType2 = false;
         return src + 3; }
   }
   return NULL;
}

#define _LongestEncoderMsg_chars \
   sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTffaa;Kyyyyyyyyyy;XPiijjkk\r")

typedef struct {U8 _min, _max; } S_MsgLimits;

/* ----------------------------------- getMsgLimits -----------------------------------------------

   Given zero or more encoder types 'et', return the number of chars of the longest message for
   any of these encoders.
*/
_EXPORT_FOR_TEST S_MsgLimits const * getMsgLimits(Sens_M_EncType et, S_MsgLimits *l)
{
   // A list of prototype message string, by encoder type.
   typedef struct { Sens_M_EncType encType; U8 msgLen; } S_MaxStr;

   PRIVATE S_MaxStr const maxStrs[] = {
      {.encType = mADE,   .msgLen = sizeof("V;RBrrrrrr;IBsssssss\r") },
      {           mGen1,            sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy\r") },
      {           mGen2,            sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTffaa;Kyyyyyyyyyy;XPiijjkk\r") },
      {           mHRE,             sizeof("V;RBrrrrrrrrr;IBssssssssss;Mbbbb?!\r") },
      {           mHRE_LCD,         sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx\r") },
      {           mMag,             sizeof("V;RBrrrrrrrrr;IBssssssss;GCaa;Mbbbbbbbb,xxxxxxxx\r") }};

   // Scan the strings table; get longest message of the encoders ORed in 'et'.
   l->_min = _LongestEncoderMsg_chars;
   l->_max = 0;

   for(U8 i = 0; i < RECORDS_IN(maxStrs); i++) {
      if(BSET(et, maxStrs[i].encType)) {
         l->_max = MaxU8(l->_max, maxStrs[i].msgLen);
         l->_min = MinU8(l->_min, maxStrs[i].msgLen); }}
   return l;
}


/* ----------------------------- getXP --------------------------------------------

   Get into the 'xp' the values encoded in the pressure field 'XPiijjkk' where 'ii','jj','kk'
   are min,max and average pressures in HexASCII (in 0.1bar).

   Given 'iijjkk' return on the char after the last 'k'.
   Return NULL if parse fail; if fail then 'xp' is undefined
*/
typedef struct { U8 _min, _max, avg; } S_XP;

_EXPORT_FOR_TEST C8 const * getXP(C8 const *src, S_XP *xp ) {
   U32 n;
   if(NULL != (src = GetNextHexASCII_U24(src, &n))) {
      xp->_min = LOW_BYTE(HIGH_WORD(n));
      xp->_max = HIGH_BYTE(LOW_WORD(n));
      xp->avg  = LOW_BYTE(LOW_WORD(n));
      return src; }
   return NULL; }



/* --------------------------------- Sensus_BlockDecode --------------------------------------------

   Given a byte sequence in 'src' which represents a legal 'Sensus' message for one of the Encoders
   in 'lookFor', return the content of that message in 'out'.

   Return true if
      - 'src' had a legally fomsgLimitsrmatted message for one of 'lookFor' AND
      - fields in that message could all be converted to 'out'.

   So this routine requires that all fields obey their format; it doesn't necessarily check that
   values are legal or sensible, just that they can be produced in 'out'

   If decode succeeds 'rawTot' and 'serNum' in 'out' will always be populated. Other fields will
   be updated if there's content in the message for them; otherwise they will be left alone.

   'out->weGot tabulates which fields were received. It includes 'rawTot', 'serNum' though
   really, these will always be true, but we include them for completeness.

   Sensus_BlockDecode() fails at the first 'src' byte which doesn't obey any format. It stops,
   success or fail, at a <CR> or '\0'. It does need the <CR> to succeed.

   It look for one or more of:

      ADE               V;RBrrrrrr;IBsssssss<CR>

      E-series GEN1     V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx<CR>   OR...
                        V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy<CR>

      E-series GEN2     V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy;XPiijjkk<CR>
                        V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTffaa;Kyyyyyyyyyy;XPiijjkk<CR>

      HRE               V;RBrrrrrrrrr;IBssssssssss;Mbbbb?!<CR>
      HRE-LCD           V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx<CR>

      Mag               V;RBrrrrrrrrr;IBssssssss;GCaa;Mbbbbbbbb,xxxxxxxx<CR>

   where:PUBLIC C8 const* GetNextHexASCII_U24(C8 const *hexStr, U32 *out)

      RBrrrrrrrrr          - totaliser, 6 to 9 digits
      IBssssssssss         - serial number; 1-10 alphanumeric
      GCaa                 - flow pcent 0-99
      Mbbbbbb,xxxxxx       - 24 bit M-field; see Badger Wiki for contents.
      Mbbbbbbbb,xxxxxxxx   - 32 bit M-field     "     "     "        "
      XTddd                - temperature; 3-digits/min e.g 024 or -16
      XTffaa               - fluid and ambient degC, 2 + 2 Hex chars.
      XPiijjkk             -
*/

PUBLIC bool Sensus_BlockDecode(C8 const *src, S_EncoderMsgData *ed, Sens_M_EncType lookFor)
{
   if(*src == 'V') {
      if(NULL != (src = startField(src, "RB"))) {
         S32 t;
         if( NULL != (src = ReadDirtyASCII_S32(src, &t))) {
            if(t >= 0 && t <= 999999999) {
               ed->rawTot = t;
               ed->weGot.rawTot = 1;
               if(NULL != (src = startField(src, "IB"))) {
                  if(NULL != (src = getSerialStr(src, ed->serialStr))) {
                     ed->weGot.serNum = 1;

                     /* Got at least V;RBrrrrrr;IBsssssss i.e totaliser and serial-string. If here's nothing
                        more than it's a ADE. If M-Field is next then it's HRE; otherwise something else. */
                     if(NULL != (src = startField(src, "M")))
                     {
                        if(BSET(lookFor, mHRE)) {
                           U16 mFld;
                           if(NULL != (src = GetNextHexASCII_U16(src, &mFld ))) {
                              if(src[0] == '?' && src[1] == '!' && endMsg(&src[2])) {
                                 ed->encoderType = mHRE;
                                 return true; }}}
                     }
                     else if(NULL != (src = startField(src, "GC")))
                     {
                        if(BSET(lookFor, mGen1 | mGen2 | mHRE_LCD | mMag)) {
                           S16 fpc;
                           if(NULL != (src = ReadDirtyASCIIInt(src, &fpc))) {
                              if(fpc >= 0 && fpc <= 99) {
                                 ed->flowPcent = fpc;
                                 ed->weGot.flowPcent = 1;

                                 U32 m1, m2;
                                 U8 bytesGot;
                                 if(NULL != (src = getDualMfield(src, &m1, &m2, &bytesGot))) {

                                    if(endMsg(src)) {
                                       if(bytesGot == 4 && BSET(lookFor, mMag)) {
                                          ed->encoderType = mMag;
                                          if(true == decodeMag_MField(m1, ed)) {
                                             ed->secTot = m2;
                                             return true; }
                                       }
                                       else if(bytesGot == 3 && BSET(lookFor, mHRE_LCD | mGen1)) {
                                          ed->encoderType = mHRE_LCD | mGen1;
                                          if(true == decode24bit_MField(m1, ed)) {
                                             ed->revTot = m2;
                                             return true; }
                                       }
                                    }
                                    else if(NULL != (src = startField(src, "XT"))) {
                                       if(BSET(lookFor, mGen1 | mGen2)) {
                                          S_XT xt;
                                          if(NULL != (src = getXT(src, &xt))) {
                                             if(NULL != (src = startField(src, "K"))) {
                                                if(NULL != (src = getSerialStr(src, ed->kStr))) {
                                                   if(endMsg(src) == true) {
                                                      if(BSET(lookFor, mGen1)) {
                                                         ed->encoderType = mGen2;
                                                         return true; }}

                                                   else if(NULL != (src = startField(src, "XP"))) {
                                                      if(BSET(lookFor, mGen2)) {
                                                         S_XP xp;
                                                         if(NULL != (src = getXP(src, &xp))) {
                                                            ed->encoderType = mGen2;
                                                            return true; }}}}}}}}}}}}
                     } // startField(src, "GC")

                     else if( endMsg(src) && BSET(lookFor, mADE) )
                     {
                        if(ed->rawTot <= 999999 && strlen(ed->serialStr) <= 7  )
                        ed->encoderType = mADE;
                        return true;
                     }
                  }}}}}}
   return false;
} // Sensus_BlockDecode


#define _DecoderBufLen (_LongestEncoderMsg_chars + 1)

typedef struct {
   Sens_M_EncType lookFor;             // Zero of more message types to expect.
   U8             put;                 // i.e buf[put].
   S_MsgLimits    msgLimits;           // longest and shorted expected messages (given 'lookFor')
   C8             buf[_DecoderBufLen]; // Add incoming Sensus chars here.
} Sensus_S_StreamDecode;

/* ---------------------------------- Sensus_DecodeStart --------------------------------------------

   Setup stream decoder 'dc' to scan for message from encoder types 'lookFor'

   Return true if the stream decoder is now reset and ready to use.
*/
PUBLIC bool Sensus_DecodeStart(Sensus_S_StreamDecode *dc, Sens_M_EncType lookFor)
{
   if(dc == NULL) {                             // No stream decoder supplied?
      return false; }                           // then fail.
   else {
      dc->lookFor = lookFor;
      getMsgLimits(lookFor, &dc->msgLimits );    // Find longest and shortest message for encoder types in 'lookFor'.
      dc->put = 0;                              // Zero the message put/char-count.
      return true; }

}

// Results of next input e.g char to a stream parser.
typedef enum {
   eParseStream_Fail    = 0,     // This (char) failed to parse; we're done
   eParseStream_Done    = 1,     // Parse is complete
   eParseStream_Continue = 2,     // More input needed.
   eParseStream_Fault   = 3      // Broke/overloaded the parser itself (which shouldn't happen unless e.g malloc() issue)
} E_ParseStreamState;


/* ---------------------------------- Sensus_DecodeStream --------------------------------------------

   Apply next 'ch' of a Sensus message to the decoder 'dc'.If the parse completes (eParseStream_Done)
   then 'out' has what was in the message.

   Returns - see E_ParseStreamState
*/
PUBLIC E_ParseStreamState Sensus_DecodeStream(Sensus_S_StreamDecode *dc, S_EncoderMsgData *out, C8 ch)
{
   /* If we exceeded the longest possible message of the encoder types we are looking for then fail.
   */
   if(dc->put >= dc->msgLimits._max) {
      return eParseStream_Fail; }

   /* else add 'ch' to the buffer. If it's at least as long as the shortest expected message AND 'ch' is an
      end-of-message then give the (presumably complete) message to the parser. Return whether it parsed or no.
   */
   else {
      dc->buf[dc->put++] = ch;

      if(dc->put >= dc->msgLimits._min && endsMsg(ch)) {                // End-of-message? AND it's long enough?
         return
            true == Sensus_BlockDecode(dc->buf, out, dc->lookFor)    // then try parsing it as a message from one of 'lookFor'
               ? eParseStream_Done                                      // Success; results will be in 'out'.
               : eParseStream_Fail; }                                   // else fail; 'out' is undefined.
      else {
         return eParseStream_Continue; }                                // else keep adding chars.
   }
}

// ------------------------------------------ eof ------------------------------------------------
