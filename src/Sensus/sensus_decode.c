/* ----------------------------------------------------------------------------------------------

  Decoder for 'Sensus' format messages from (Badger) Water Meters.


-------------------------------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>

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

// The kind of Encoder message.
typedef enum {
   eADE        = 0x01,
   eGen1       = 0x02,
   eGen2       = 0x04,
   eHRE        = 0x08,
   eHRE_LCD    = 0x10,
   eMag        = 0x20,
   eAnyEncoder = 0xFF
} Sens_E_EncType;

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
   Sens_E_EncType    encoderType;
   Sens_S_WotWeGot   wotWeGot;
   C8                serNumStr[_enc_MaxSerNumChars+1];
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

/* --------------------------------- endField ----------------------------------------------------

   Return true if 'src' is ';' which ends/start  a field or <CR> or '\0' which ends and encoder
   message.
*/
_EXPORT_FOR_TEST bool endField(C8 const *src) {
   #define CR 0x13
   return src[0] == ';' || src[0] == '\0' || src[0] == CR; }


/* --------------------------------- copySerNum ---------------------------------------------------

   Given serial number "ssssssssss(:|'\0'|<CR>)", get "ssssssssss" into 'out'.

   "ssssssssss" must be 1-10 alphanumeric chars ('_enc_MaxSerNumChars') immediately followed by one
   ';', '\0' or <CR>.

   If success then return on the trailing ';'; else return NULL.
*/
_EXPORT_FOR_TEST C8 const * copySerNum(C8 *out, C8 const *src)
{
   for(U8 i = 0; i < _enc_MaxSerNumChars; i++, src++, out++) {    // From 'src', up to 10 (alphanumeric) chars
      if( endField(*src) == true ) {                              // Field delimiter ie ';', <CR> or '\0'?
         *out = '\0';                                             // Terminate whatever we built in 'out'.
         return i == 0                                            // Didn't even get 1 char?
            ? NULL                                                // then fail, serial number cannot be empty.
            : src; }                                              // else return on end of the field we read.
      else if(isalnum(*src)) {                                    // else another char of the serial number?...
         *out = *src; }}                                           // ...copy it to 'out'.

   return NULL;      // Got a non-alphanumeric before filed delimiter.
}

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

   'out->wotWeGot tabulates which fields were received. It includes 'rawTot', 'serNum' though
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

   where:
      RBrrrrrrrrr          - totaliser, 6 to 9 digits
      IBssssssssss         - serial number; 1-10 alphanumeric
      GCaa                 - flow pcent 0-99
      Mbbbbbb,xxxxxx       - 24 bit M-field; see Badger Wiki for contents.
      Mbbbbbbbb,xxxxxxxx   - 32 bit M-field     "     "     "        "
      XTddd                - temperature; 3-digits/min e.g 024 or -16
      XTffaa               - fluid and ambient degC, 2 + 2 Hex chars.
      XPiijjkk             -
*/

PUBLIC bool Sensus_BlockDecode(S_EncoderMsgData *out, C8 const *src, Sens_E_EncType lookFor)
{
   if(*src == 'V') {
      if(NULL != (src = startField(src, "RB"))) {
         T_FloatOrInt fi = {0};
         if(NULL != (src = ReadASCIIToNum(src, &fi))) {
            if(fi.gotUnsigned == true && fi.gotInt == true) {
            }
         }
      }
   }
}

// ------------------------------------------ eof ------------------------------------------------
