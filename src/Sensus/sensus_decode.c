/* ----------------------------------------------------------------------------------------------

  Decoder for 'Sensus' format messages from (Badger) Water Meters.

-------------------------------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>
#include "arith.h"
#include <string.h>

#define _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT

typedef U32 enc_T_Tot;          // Raw Meter readings; up to 9 digits
typedef U8  enc_T_FlowPcent;    // Percent of max flow
typedef U8  enc_T_Pressure;     // In 0.1bar
typedef U8  enc_T_MeterSize;    // Size in b[5:0] of the 32 bit M-Field
typedef U8  enc_T_MeterType;    // Size/type in the top byte of the 24bit M-Field
typedef U8  enc_T_Resolution;
typedef U8  enc_T_ProdCode;     // b[15:12] of 32bit M-field
typedef U8  enc_T_UOM;
typedef U8  enc_T_MeasMode;
typedef U8  enc_T_FlowDir;
typedef S16 enc_T_degC;         // In degC.

// The kind of Encoder message.'m' <-> masks so decoder can filter for multiple message-types.
typedef enum {
   mADE        = 0x01,
   mGen1       = 0x02,
   mGen2       = 0x04,
   mHRE        = 0x08,
   mHRE_LCD    = 0x10,
   mMag        = 0x20,
   mAnyEncoder = 0xFF
} enc_M_EncType;

// Will be set if this alert occurs in the message; otherwise clear.
typedef struct {
   U32   overflow    :1,      // of the totaliser
         pressure    :1,      // Any of the min.max or average pressure tagged as bad.
         reverseFlow :1,
         tamper      :1,
         leak        :1,
         program     :1,      // 'Program' error in basic status.
         temperature :1,
         endOfLife   :1,      // Usually battery is dead.
         emptyPipe   :1,
         noFlow      :1;      // i.e no usage for some time.

         #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
   struct {    // Mag-specific alerts.
      U32
         adcError    :1,
         lowBatt     :1,
         badCoilDrive :1,
         measTimeout :1,
         flowStim    :1,
         ovStatus    :1,
         maxFlow     :1,
         badSensor   :1;
      } mag;
         #endif // _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
} enc_S_Alerts;

// Will be set if the corresponding field in enc_S_MsgData was updated from the Encoder message.
typedef struct {
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
} enc_S_WotWeGot;

#define _enc_MaxSerNumChars 10

typedef struct { enc_T_Pressure _min, _max, _avg;  } S_Pres;

typedef struct {
   enc_M_EncType     encoderType;                        // From the message format i.e HRE, Gen2 etc.
   enc_S_WotWeGot    weGot;                              // Which of the following variables were read form the message
   enc_S_Alerts      alerts;
   C8                serialWord[_enc_MaxSerNumChars+1];  // serial-word 'RBrrrrrrrrr;'. Up to 10 digits & letters.
   C8                kStr[_enc_MaxSerNumChars+1];        // The 'ownership number' ';Knnnnnnnnn' up to 10 letters & digits..
   enc_T_Tot         rawTot,                             // RBrrrrrrrrr;IBssssssssss
                     revTot;
   U8                dials;                              // Digits in ';RBnnnnnnnn'. Usually >= 6; no more than 9.
   enc_T_FlowPcent   flowPcent;

   struct {
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


/* --------------------------------- getSerialWord ---------------------------------------------------

   Given serialisation word "ssssssssss(:|'\0'|<CR>)", get "ssssssssss" into 'out'.

   "ssssssssss" must be 1-10 alphanumeric chars ('_enc_MaxSerNumChars') immediately followed by one
   ';', '\0' or <CR>.

   If success then return on the trailing ';'; else return NULL.
*/
_EXPORT_FOR_TEST C8 const * getSerialWord(C8 const *src, C8 *out)
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
static U8 bitU16(U16 n, U8 bit) {
   return (n & (1U << bit)) != 0 ? 1 : 0; }

// -----------------------------------------------------------------------------------------------
static U8 bitU32(U32 n, U8 bit) {
   return (n & (1UL << bit)) != 0 ? 1 : 0; }

// -----------------------------------------------------------------------------------------------
static U8 fieldU32(U32 n, U8 msb, U8 lsb) {
   return (n && MakeAtoBSet_U32(msb, lsb)) >> lsb; }


/* ---------------------------- decodeBasicStatus ---------------------------------------------------

   Decode into 'ed' the basic 16bit status in 'mf' e.g ':Mbbbb?!' or the lower 16 bits of ';Mbbbbbb'.
*/
_EXPORT_FOR_TEST bool decodeBasicStatus(U16 mf, enc_S_MsgData *ed) {

   enc_S_Alerts a;
   a.overflow     = bitU16(mf, 15);
   a.pressure     = bitU16(mf, 14);
   a.tamper       = bitU16(mf, 7);
   a.program      = bitU16(mf, 6);
   a.leak         = bitU16(mf, 5);
   a.reverseFlow  = bitU16(mf, 4);
   a.noFlow       = bitU16(mf, 3);
   a.endOfLife    = bitU16(mf, 2);
   a.temperature  = bitU16(mf, 1);
   a.emptyPipe    = bitU16(mf, 0);
   ed->alerts = a;

   ed->weGot.alerts = 1;
   return true;   // Always succeeds, for now.
}

/* ------------------------------ decodeExtended_MField -----------------------------------------------

   Decode into 'ed' 24bit M-field in the 3 lower bytes of 'mf'.
*/
_EXPORT_FOR_TEST bool decodeExtended_MField(U32 mf, enc_S_MsgData *ed) {

   // Extended content in msb (of the 3 bytes).
   ed->meterType = LOW_BYTE(HIGH_WORD(mf));
   ed->weGot.meterType = 1;
   ed->uom = fieldU32(mf, 11, 9);
   ed->weGot.uom = 1;

   // Legacy status in the lower 2 bytes.
   return decodeBasicStatus(LOW_WORD(mf), ed);
}

/* ------------------------------ decodeMag_MField -----------------------------------------------

   Decode into 'ed' 32bit M-field in 'mf'.
*/
   #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT

typedef enum {
   eMag_Unknown = 0, eMag_M2000 = 1, eMag_M5000 = 2, eMag_M1500 = 3, eMag_M1000 = 4,
   eMag_M5000B = 5, eMag_Utility = 6, eMag_Isonic4000 = 7, eMag_TFX_500W = 8
} E_MagProdCode;

_EXPORT_FOR_TEST bool decodeMag_MField(U32 mf, enc_S_MsgData *ed) {

   //How some Mag-Meter M-Fields depends on the type of Mag_meter. So get that first.
   ed->mag.prodCode = fieldU32(mf, 15, 12);
   ed->weGot.prodCode = 1;

   enc_S_Alerts *a = &ed->alerts;

   #define _mpc (ed->mag.prodCode)

   if(_mpc == eMag_M2000 || _mpc == eMag_M1000 || _mpc == eMag_M5000 || _mpc == eMag_Utility) {
      a->mag.maxFlow   = bitU32(mf, 27);
      a->mag.adcError  = bitU32(mf, 26);
      a->emptyPipe     = bitU32(mf, 25);
      a->mag.badSensor = bitU32(mf, 24); }

   if(_mpc == eMag_M2000) {
      a->mag.flowStim = bitU32(mf, 30);
      a->overflow     = bitU32(mf, 29);
      a->mag.ovStatus = bitU32(mf, 28);
      }
   else if(_mpc == eMag_M1000 || _mpc == eMag_M5000 || _mpc == eMag_M5000B ) {
      a->mag.badCoilDrive = bitU32(mf,30);
      a->mag.measTimeout  = bitU32(mf,29);
      }
   else if(_mpc == eMag_Utility) {
      a->endOfLife       = bitU32(mf, 30);
      a->mag.measTimeout = bitU32(mf, 29);
      a->leak            = bitU32(mf, 23);
      a->reverseFlow     = bitU32(mf, 22); }

   ed->mag.res = fieldU32(mf, 19, 16);
   ed->weGot.res = 1;

   ed->uom = fieldU32(mf, 11, 8);
   ed->weGot.uom = 1;

   ed->alerts.reverseFlow = bitU32(mf, 6);

   ed->mag.meterSize = fieldU32(mf, 5, 0);
   ed->weGot.meterSize = 1;

   ed->mag.biDirectional = bitU32(mf,7);

   ed->weGot.alerts = 1;_SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
   return true;      // Always succeeds, for now.
}
   #endif // _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT


/* ----------------------------------- getXT -----------------------------------------------------

   Get either of the 2 temperature fields (below) from 'src' into 'ed'

      - 'XTddd',     (type 1) where ddd is a single signed decimal temperature
      - 'XTffss',    (type 2) where 'ff' and 'aa' are fluid and ambient temperatures,
                              (each in signed 8-bit HexASCII).

   This given 'src' is on the char after 'XT' i.e at 'ddd' or 'ffaa'. 'xt.isType2' says which we got.

   Return on the 1st char after the field; NULL if fail.
*/
_EXPORT_FOR_TEST C8 const * getXT(C8 const *src, enc_S_MsgData *ed ) {

   /* Check dual-temperature 'XTffaa', where  'ff' is fluid degC and 'aa' is ambient degC.
      in signed 8-bit HexASCII.

      Since GetNextHexASCII_U16() eats leading spaces check that the first char is NOT a
      space and then that GetNextHexASCII_U16() can read 2 HexASCII bytes from there.
   */
   if(isalnum(src[0])) {                              // First byte is NOT a space? AND...
      U16 n;
      if(NULL != GetNextHexASCII_U16(src, &n)) {      // ... read 2 HexASCII bytes from 'src'

         /* then this is a dual-temperature reading. Parse each of fluid and ambient,
            checking for any exception codes.
         */
         U8 fluid = HIGH_BYTE(n);
         if(fluid == 0x7E) {                          // Error reading temperature?
            ed->alerts.temperature = 1; }             // then post alert.
         else if(fluid == 0x7D || fluid == 0x7C) {    // No sensor? OR Meter is in storage mode?
            }                                         // then do nothing.
         else {                                       // else byte is a valid degC
            ed->noMag.fluidDegC = (S8)fluid;          // read it as signed 8-bit
            ed->weGot.fluidTmpr = 1; }                // and say we got a degC

         // Repeat above for ambient temperature.
         U8 amb = LOW_BYTE(n);
         if(amb == 0x7E) {
            ed->alerts.temperature = 1; }
         else if(amb == 0x7D || amb == 0x7C) {
         }
         else {
            ed->noMag.ambientDegC = (S8)amb;
            ed->weGot.ambientTmpr = 1; }
         return src + 4; }}                           // Advance past 'ffaa' and done.

   // else check for ';XTddd', 2nd and 3rd of 'ddd' must be digits...
   else if( isdigit(src[1]) && isdigit(src[2])) {
      // ... and 'ddd' must read as positive or negative integer.
      S16 n;
      if( NULL != ReadDirtyASCIIInt(src, &n) ) {      // Read a signed int?

         // Check special numbers which mean error or exception.
         if(n == 125 )                                // 125?, Meter is in storage mode
            {}                                        // so no temperature read, do nothing.
         else if(n == 126 ) {                         // 126?, fault reading temperature.
            ed->alerts.temperature = 1; }             // post alert
         else {
            ed->noMag.fluidDegC = n;                  // else legal fluid temperature reading. use it
            ed->weGot.fluidTmpr = 1; }                // and say we have a that reading.
         return src + 3; }                            // Advance past 'ddd' and done.
   }
   return NULL;      // Some fail above.
}

#define _LongestEncoderMsg_chars \
   sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTffaa;Kyyyyyyyyyy;XPiijjkk\r")

typedef struct {U8 _min, _max; } S_MsgLimits;

/* ----------------------------------- getMsgLimits -----------------------------------------------

   Given zero or more encoder types 'et', return to 'l' the number of chars of the longest and
   shortest possible messages for any of these encoders.
*/
_EXPORT_FOR_TEST S_MsgLimits const * getMsgLimits(enc_M_EncType et, S_MsgLimits *l)
{
   // A list of prototype message string, by encoder type.
   typedef struct { enc_M_EncType encType; U8 msgLen; } S_MaxStr;

   // The longest each string can be.
   PRIVATE S_MaxStr const maxStrs[] = {
      {.encType = mADE,   .msgLen = sizeof("V;RBrrrrrr;IBsssssss\r") },
      {           mGen1,            sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Kyyyyyyyyyy\r") },
      {           mGen2,            sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx;XTffaa;Kyyyyyyyyyy;XPiijjkk\r") },
      {           mHRE,             sizeof("V;RBrrrrrrrrr;IBssssssssss;Mbbbb?!\r") },
      {           mHRE_LCD,         sizeof("V;RBrrrrrrrrr;IBssssssssss;GCaa;Mbbbbbb,xxxxxx\r") },
      {           mMag,             sizeof("V;RBrrrrrrrrr;IBssssssss;GCaa;Mbbbbbbbb,xxxxxxxx\r") }};
   // The shortest each might be; 6 digit totaliser, 4 character serial-number.
   PRIVATE S_MaxStr const minStrs[] = {
      {.encType = mADE,   .msgLen = sizeof("V;RBrrrrrr;IBssss\r") },
      {           mGen1,            sizeof("V;RBrrrrrr;IBssss;GCaa;Mbbbbbb,xxxxxx;XTddd;Ky\r") },
      {           mGen2,            sizeof("V;RBrrrrrr;IBssss;GCaa;Mbbbbbb,xxxxxx;XTffaa;Ky;XPiijjkk\r") },
      {           mHRE,             sizeof("V;RBrrrrrr;IBssss;Mbbbb?!\r") },
      {           mHRE_LCD,         sizeof("V;RBrrrrrr;IBssss;GCaa;Mbbbbbb,xxxxxx\r") },
      {           mMag,             sizeof("V;RBrrrrrr;IBssss;GCaa;Mbbbbbbbb,xxxxxxxx\r") }};

   // Scan the strings table; get longest message of the encoders ORed in 'et'.
   l->_min = _LongestEncoderMsg_chars;
   l->_max = 0;

   for(U8 i = 0; i < RECORDS_IN(maxStrs); i++) {
      if(BSET(et, maxStrs[i].encType)) {
         l->_max = MaxU8(l->_max, maxStrs[i].msgLen);
         l->_min = MinU8(l->_min, minStrs[i].msgLen); }}
   return l;
}


/* ----------------------------- getXP --------------------------------------------

   Get into the 'ed' the values encoded in the pressure field 'XPiijjkk' where 'ii','jj','kk'
   are min,max and average pressures in HexASCII (in 0.1bar).

   Given 'iijjkk' return on the char after the last 'k'.
   Return NULL if parse fail; if fail then 'xp' is undefined
*/

// ---- Not a fault (0xFD) AND not no-read (0xFD) because Meter is in storage.
static bool isAPres(U8 p) {
   return p != 0xFE && p != 0xFD; }

_EXPORT_FOR_TEST C8 const * getXP(C8 const *src,  enc_S_MsgData *ed ) {
   U32 n;
   if(NULL != (src = GetNextHexASCII_U24(src, &n))) {

      U8 minP = LOW_BYTE(HIGH_WORD(n));
      U8 maxP = HIGH_BYTE(LOW_WORD(n));
      U8 avgP  = LOW_BYTE(LOW_WORD(n));

      if(minP == 0xFE || maxP == 0xFE || avgP == 0xFE) {
         ed->alerts.pressure = 1; }

      if(minP != 0xFE) {
         ed->noMag.pres._min = minP; }
      if(maxP != 0xFE) {
         ed->noMag.pres._max = maxP; }
      if(avgP != 0xFE) {
         ed->noMag.pres._avg = avgP; }

      if( isAPres(minP) || isAPres(maxP) || isAPres(avgP)) {
         ed->weGot.pressure = 1; }

      return src; }
   return NULL; }



/* --------------------------------- Sensus_BlockDecode --------------------------------------------

   Given a byte sequence in 'src' which represents a legal 'Sensus' message for one of the Encoders
   in 'filterFor', return the content of that message in 'out'.

   Return true if
      - 'src' had a legally fomsgLimitsrmatted message for one of 'filterFor' AND
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
      XPiijjkk             - min, max and average pressure.
*/
PUBLIC bool Sensus_BlockDecode(C8 const *src, enc_S_MsgData *ed, enc_M_EncType filterFor)
{
   C8 const *p = src;      // Mark the start.

   if(*p == 'V') {                                                            // 'V...  '?
      if(NULL != (p = startField(p, "RB"))) {                                 // 'V;RB...  '?
         S32 t;
         if( NULL != (p = ReadDirtyASCII_S32(p, &t))) {                       // 'V;RBrr...  '? where 'r' are 0-9
            U8 digits = AminusBU8(p - src, 3);
            if(digits < 9 && digits >= 6) {                                   // 6-9 digits?

               ed->rawTot = t;                                                // then our totaliser is that number.
               ed->dials = digits;
               ed->weGot.rawTot = 1;

               if(NULL != (p = startField(p, "IB"))) {                       // 'V;RBrrrrrr[rrr];IB...   '?
                  if(NULL != (p = getSerialWord(p, ed->serialWord))) {       // Read 'ssssssss' from 'V;RBrrrrrr[rrr];IBsssssssss'?
                     ed->weGot.serWord = 1;

                     /* Got at least V;RBrrrrrr;IBsssssss i.e totaliser and serial-string. If here's nothing
                        more than it's a ADE. If M-Field is next then it's HRE; otherwise something else.
                     */
                     if(NULL != (p = startField(p, "M")))                        // 'V;RBrrrrrrrrr;IBssssssssss;M...  '?
                     {                                                           // Only HRE fits this pattern.
                        if(BSET(filterFor, mHRE)) {                              // HRE requested?
                           U16 mFld;
                           if(NULL != (p = GetNextHexASCII_U16(p, &mFld ))) {    // Got ';Mxxxx'
                              if(p[0] == '?' && p[1] == '!' && endMsg(&p[2])) {  // Got ';Mxxxx?!'
                                 ed->encoderType = mHRE;                         // then it's a HRE
                                 decodeBasicStatus(mFld, ed);                    // Decode the 16-bit status.
                                 return true; }}}
                     }
                     else if(NULL != (p = startField(p, "GC")))                     // 'V;RBrrrrrrrrr;IBssssssssss;GC...  '?
                     {                                                              // must be Gen1,Gen2,HRE-LCD or mag.
                        if(BSET(filterFor, mGen1 | mGen2 | mHRE_LCD | mMag)) {      // Is one of the above?
                           S16 pcent;
                           if(NULL != (p = ReadDirtyASCIIInt(p, &pcent))) {         // Read (flow) pcent from ';GCnn'
                              if(pcent >= 0 && pcent <= 99) {                       // Was 0-99
                                 ed->flowPcent = pcent;                             // then write result.
                                 ed->weGot.flowPcent = 1;

                                 /* ';GCaa' is always followed by a 24-bit or 32-bit dual M-Field. 32-bit is Mag-Meter; everything
                                    else is 24-bit.
                                 */
                                 U32 m1, m2;
                                 U8 bytesGot;
                                 if(NULL != (p = getDualMfield(p, &m1, &m2, &bytesGot))) {      // Got ';Mbbbbbb,xxxxxx' or ';Mbbbbbbbb,xxxxxxxx'

                                    // If M-filed ends message then it's Gen1, HRE-LCD or Mag.
                                    if(endMsg(p)) {
                                             #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
                                       if(bytesGot == 4 && BSET(filterFor, mMag)) {                   // 32-bit M-Field?
                                          ed->encoderType = mMag;                                     // then it's a Mag
                                          if(true == decodeMag_MField(m1, ed)) {                      // Decode 1st 'bbbbbbbb'
                                             ed->mag.secTot = m2;                                     // 'xxxxxxxx' is secondary total
                                             return true; }
                                       } else
                                             #endif // _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
                                       if(bytesGot == 3 && BSET(filterFor, mHRE_LCD | mGen1)) {       // else 24-bit M-field? AND requested Gen1 or HRE-LCD?
                                          ed->encoderType = mHRE_LCD | mGen1;                         // then we got one of those; they have same message format
                                          if(true == decodeExtended_MField(m1, ed)) {                 // decode 24bit status
                                             ed->revTot = m2;                                         // 2nd M-field is reverse total.
                                             return true; }
                                       }
                                    }
                                    // If M-field is followed by ';XT... ' (temperature) then it's a Gen1 or Gen2.
                                    else if(NULL != (p = startField(p, "XT"))) {                      // Got ';XT...  '?...
                                       if(BSET(filterFor, mGen1 | mGen2)) {                           // and asking for Gen1 or Gen2.
                                          if(NULL != (p = getXT(p, ed))) {                            // then read ';XTddd' (fluid degC) or ';XTffaa' (fluid and ambient)?
                                             if(NULL != (p = startField(p, "K"))) {
                                                // For Gen1, Gen2, ';XT...' is always followed by ';Kyyyyyy... ', the owenership number.
                                                if(NULL != (p = getSerialWord(p, ed->kStr))) {        // Read ';Kyyyyyyyyy'?
                                                   // If ';Kyyyyyyyyyy' then it must be Gen1.
                                                   if(endMsg(p) == true) {                            // ';Kyyyyyyyy' was last field?
                                                      if(BSET(filterFor, mGen1)) {                    // and did request Gen1
                                                         ed->encoderType = mGen1;                     // then Gen1 is what we got
                                                         return true; }}
                                                   // else 'XP...' (pressures) after ';Kyyyy...'. Must be Gen2.
                                                   else if(NULL != (p = startField(p, "XP"))) {       // Got ';XP'?
                                                      if(BSET(filterFor, mGen2)) {                    // AND requested Gen2?
                                                         if(NULL != (p = getXP(p, ed))) {             // AND read ';XPiijjkk' into pressures?
                                                            ed->encoderType = mGen2;                  // then Gen2 is what we got.
                                                            return true; }}}}}}}}}}}}
                     } // startField(p, "GC")

                     /* else no ';M...' and no ';GC... '. It must be just  V;RBrrrrrr;IBsssssss<CR> i.e and ADE.

                        Check for end of message, a 6 digit totaliser and a serial-word of no more than 7 chars.
                     */
                     else if( endMsg(p) && BSET(filterFor, mADE) )                  // End-of-message? AND did request ADE?
                     {
                        if(ed->rawTot <= 999999 && strlen(ed->serialWord) <= 7  )    // 6-digit total? AND < 7-char serial-word.
                        ed->encoderType = mADE;
                        return true;
                     }
                  }}}}}}
   return false;     // Did not finish a decode above.
} // Sensus_BlockDecode


#define _DecoderBufLen (_LongestEncoderMsg_chars + 1)

typedef struct {
   enc_M_EncType filterFor;             // Zero of more message types to expect.
   U8             put;                 // i.e buf[put].
   S_MsgLimits    msgLimits;           // longest and shorted expected messages (given 'filterFor')
   C8             buf[_DecoderBufLen]; // Add incoming Sensus chars here.
} enc_S_StreamDecode;

/* ---------------------------------- Sensus_DecodeStart --------------------------------------------

   Setup stream decoder 'dc' to scan for message from encoder types 'filterFor'

   Return true if the stream decoder is now reset and ready to use.
*/
PUBLIC bool Sensus_DecodeStart(enc_S_StreamDecode *dc, enc_M_EncType filterFor)
{
   if(dc == NULL) {                             // No stream decoder supplied?
      return false; }                           // then fail.
   else {
      dc->filterFor = filterFor;
      getMsgLimits(filterFor, &dc->msgLimits );    // Find longest and shortest message for encoder types in 'filterFor'.
      dc->put = 0;                              // Zero the message put/char-count.
      return true; }

}

// Results of next input e.g char to a stream parser.
typedef enum {
   eParseStream_Fail    = 0,     // This (char) failed to parse; we're done
   eParseStream_Done    = 1,     // Parse is complete
   eParseStream_Continue = 2,     // More input needed.
   eParseStream_Fault   = 3      // Broke/overloaded the parser itself (which shouldn't happen unless e.g malloc() issue)
} enc_E_StreamState;


/* ---------------------------------- Sensus_DecodeStream --------------------------------------------

   Apply next 'ch' of a Sensus message to the decoder 'dc'.If the parse completes (eParseStream_Done)
   then 'ed' has what was in the message.

   Returns - see enc_E_StreamState
*/
PUBLIC enc_E_StreamState Sensus_DecodeStream(enc_S_StreamDecode *dc, enc_S_MsgData *ed, C8 ch)
{
   /* If we exceeded the longest possible message of the encoder types we are looking for then fail.
   */
   if(dc->put >= dc->msgLimits._max) {
      return eParseStream_Fail; }

   /* else add 'ch' to the buffer. If it's at least as long as the shortest expected message AND 'ch' is an
      end-of-message then give the (presumably complete) message to the parser. Return whether it parsed or no.
   */
   else {
      // If this is the 1st char, then zero results, to be filled by Sensus_BlockDecode().
      if(dc->put == 0) {
         *ed = (enc_S_MsgData){0};
         // Set pressures to their null-storage values.
         ed->noMag.pres =(S_Pres){ ._min = 0xFF, ._max = 0xFF, ._avg = 0xFF };
         }

      dc->buf[dc->put++] = ch;

      if(dc->put >= dc->msgLimits._min && endsMsg(ch)) {                // End-of-message? AND it's long enough?
         return
            true == Sensus_BlockDecode(dc->buf, ed, dc->filterFor)        // then try parsing it as a message from one of 'filterFor'
               ? eParseStream_Done                                      // Success; results will be in 'ed'.
               : eParseStream_Fail; }                                   // else fail; 'ed' is undefined.
      else {
         return eParseStream_Continue; }                                // else keep adding chars.
   }
}

// ------------------------------------------ eof ------------------------------------------------
