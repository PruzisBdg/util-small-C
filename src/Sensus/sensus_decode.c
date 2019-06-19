/* ----------------------------------------------------------------------------------------------

  Decoder for 'Sensus' format messages from (Badger) Water Meters.


-------------------------------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>
#include <string.h>

typedef U32 Sens_T_Tot;       // Raw Meter readings; up to 9 digits
typedef U8  Sens_T_FlowPcent;
typedef U8  Sens_T_MeterSize;
typedef U8  Sens_T_MeterType;
typedef U8  Sens_T_Resolution;
typedef U8  Sens_T_ProdCode;
typedef U8  Sens_T_UOM;
typedef U8  Sens_T_MeasMode;
typedef U8  Sens_T_FlowDir;
typedef S16 Sens_T_degC;
typedef S16 Sens_T_degC;

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
   U8    overflow    :1,
         pressure    :1,
         reverseFlow :1,
         tamper      :1,
         leak        :1,
         program     :1,
         temperature :1,
         endOfLife   :1,
         emptyPipe   :1,
         noFlow      :1;
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
   Sens_M_EncType    encoderType;
   Sens_S_WotWeGot   weGot;
   C8                serialStr[_enc_MaxSerNumChars+1];
   C8                kStr[_enc_MaxSerNumChars+1];
   Sens_T_Tot        rawTot;
   Sens_T_FlowPcent  flowPcent;
   Sens_T_degC       fluidDegC,
                     ambientDegC;
   Sens_T_MeterSize  meterSize;
   Sens_T_MeterType  meterType;
   Sens_T_Resolution res;
   Sens_T_ProdCode   prodCode;
   Sens_T_UOM        uom;
   Sens_T_MeasMode   measMode;
   Sens_T_FlowDir    flowDir;
   Sens_S_Alerts     alerts;
} S_EncoderMsgData;

#define _FieldDelimiter ';'

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
static bool inline endMsg(C8 const *src) {
   #define CR 0x13
   return src[0] == '\0' || src[0] == CR; }


/* --------------------------------- endField ----------------------------------------------------

   Return true if 'src' is ';' which ends/start  a field or <CR> or '\0' which ends and encoder
   message.
*/
PRIVATE bool endField(C8 const *src) {
   return src[0] == ';' || endMsg(src); }


/* --------------------------------- copySerNum ---------------------------------------------------

   Given serial number "ssssssssss(:|'\0'|<CR>)", get "ssssssssss" into 'out'.

   "ssssssssss" must be 1-10 alphanumeric chars ('_enc_MaxSerNumChars') immediately followed by one
   ';', '\0' or <CR>.

   If success then return on the trailing ';'; else return NULL.
*/
_EXPORT_FOR_TEST C8 const * copySerNum(C8 const *src, C8 *out)
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
      - 'src' had a legally formatted message for one of 'lookFor' AND
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

PUBLIC bool Sensus_BlockDecode(S_EncoderMsgData *out, C8 const *src, Sens_M_EncType lookFor)
{
   if(*src == 'V') {
      if(NULL != (src = startField(src, "RB"))) {
         S32 t;
         if( NULL != (src = ReadDirtyASCII_S32(src, &t))) {
            if(t >= 0 && t <= 999999999) {
               out->rawTot = t;
               out->weGot.rawTot = 1;
               if(NULL != (src = startField(src, "IB"))) {
                  if(NULL != (src = copySerNum(src, out->serialStr))) {
                     out->weGot.serNum = 1;

                     /* Got at least V;RBrrrrrr;IBsssssss i.e totaliser and serial-string. If here's nothing
                        more than it's a ADE. If M-Field is next then it's HRE; otherwise something else.
                     */
                     if(NULL != (src = startField(src, "M")))
                     {
                        if(BSET(lookFor, mHRE)) {
                           U16 mFld; U8 bytesGot;
                           if(NULL != (src = GetNextHexASCII_U16(src, &mFld ))) {
                              if(src[0] == '?' && src[1] == '!' && endMsg(&src[2])) {
                                 out->encoderType = mHRE;
                                 return true; }}}
                     }
                     else if(NULL != (src = startField(src, "GC")))
                     {
                        if(BSET(lookFor, mGen1 | mGen2 | mHRE_LCD | mMag)) {
                           S16 fpc;
                           if(NULL != (src = ReadDirtyASCIIInt(src, &fpc))) {
                              if(fpc >= 0 && fpc <= 99) {
                                 out->flowPcent = fpc;
                                 out->weGot.flowPcent = 1;

                                 U32 m1, m2;
                                 U8 bytesGot;
                                 if(NULL != (src = getDualMfield(src, &m1, &m2, &bytesGot))) {

                                    if(endMsg(src)) {
                                       if(bytesGot == 4 && BSET(lookFor, mMag)) {
                                          out->encoderType = mMag;
                                          return true;
                                       }
                                       else if(bytesGot == 3 && BSET(lookFor, mHRE_LCD | mGen1)) {
                                          out->encoderType = mHRE_LCD | mGen1;
                                          return true;
                                       }
                                    }
                                    else if(NULL != (src = startField(src, "XT"))) {
                                       if(BSET(lookFor, mGen1 | mGen2)) {
                                          S_XT xt;
                                          if(NULL != (src = getXT(src, &xt))) {
                                             if(NULL != (src = startField(src, "K"))) {
                                                if(NULL != (src = copySerNum(src, out->kStr))) {
                                                   if(endMsg(src) == true) {
                                                      if(BSET(lookFor, mGen1)) {
                                                         out->encoderType = mGen2;
                                                         return true; }}

                                                   else if(NULL != (src = startField(src, "XP"))) {
                                                      if(BSET(lookFor, mGen2)) {
                                                         S_XP xp;
                                                         if(NULL != (src = getXP(src, &xp))) {
                                                            out->encoderType = mGen2;
                                                            return true; }}}}}}}}}}}}
                     } // startField(src, "GC")

                     else if( endMsg(src) && BSET(lookFor, mADE) )
                     {
                        if(out->rawTot <= 999999 && strlen(out->serialStr) <= 7  )
                        out->encoderType = mADE;
                        return true;
                     }
                  }}}}}}
   return false;
} // Sensus_BlockDecode



typedef struct {
   Sens_M_EncType lookFor;
} Sensus_S_StreamDecode;

/* ---------------------------------- Sensus_StreamStart --------------------------------------------


*/
PUBLIC bool Sensus_StreamDecode(Sensus_S_StreamDecode *dc, Sens_M_EncType lookFor)
{
   dc->lookFor = lookFor;
   return true;
}

// Results of next input e.g char to a stream parser.
typedef enum {
   eParseStream_Fail    = 0,     // This (char) failed to parse; we're done
   eParseStream_Done    = 1,     // Parse is complete
   eParseStream_Continue = 2,     // More input needed.
   eParseStream_Fault   = 3      // Broke/overloaded the parser itself (which shouldn't happen unless e.g malloc() issue)
} E_ParseStreamState;


/* ---------------------------------- Sensus_DecodeStream --------------------------------------------

   Apply next 'ch' of a Sesnue message to the decoder 'dc'.If the parse completes (eParseStream_Done)
   then 'out' has what was in the message.



   Returns - see E_ParseStreamState
*/
PUBLIC E_ParseStreamState Sensus_DecodeStream(Sensus_S_StreamDecode *dc, S_EncoderMsgData *out, C8 ch)
{
   return eParseStream_Continue;
}

// ------------------------------------------ eof ------------------------------------------------
