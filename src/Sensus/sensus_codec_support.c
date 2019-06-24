/* -------------------------------- Sensus Codec (Test Harness) Support --------------------------------- */

#include "libs_support.h"
#include "sensus_codec.h"
#include <stdio.h>
#include <string.h>
#include "util.h"
#include <ctype.h>

// -------------------------------------------------------------------------------------------
PUBLIC C8 const * sens_ShowEncoders(C8 *out, enc_M_EncType t)
{
   typedef struct {enc_M_EncType typ; C8 const *name; } S_Name;

   S_Name const names[] = {
      {.typ = mADE,     .name = "ADE" },
      {       mGen1,            "Gen1" },
      {       mGen2,            "Gen2" },
      {       mHRE,             "HRE" },
      {       mHRE_LCD,         "HRE-LCD" },
      {       mMag,             "Mag" }};

   if(t == 0) {
      strcpy(out, "(none)"); }
   else if(t == mAnyEncoder) {
      strcpy(out, "(Any)"); }
   else
   {
      strcpy(out, "(");

      for(U8 i = 0; i < RECORDS_IN(names); i++) {
         if(BSET(t, names[i].typ)) {
            if(i > 0)
               { strcat(out, "|"); }       // Insert '|' (OR) bar between names.
            strcat(out, names[i].name); }}

      strcat(out, ")");
   }
   return out;
}

// -------------------------------------------------------------------------------------------
static C8 const *showDataFlags(C8 *out, enc_S_WotWeGot const *n)
{
   if(n->asU16 == 0) {
      strcpy(out, "(none)"); }
   else {
      strcpy(out, "(");

      #define _MayCat(_name)            \
         if(n->bs._name == 1) { strcat(out, #_name "," ); }

      _MayCat(serWord)
      _MayCat(rawTot)
      _MayCat(flowPcent)
      _MayCat(pressure)
      _MayCat(fluidTmpr)
      _MayCat(ambientTmpr)
      _MayCat(meterSize)
      _MayCat(meterType)
      _MayCat(res)
      _MayCat(prodCode)
      _MayCat(uom)
      _MayCat(measMode)
      _MayCat(alerts)

      #undef _MayCat
      strcat(out, ")");
   }
   return out;
}

// -------------------------------------------------------------------------------------------
PUBLIC C8 const *sens_ShowAlerts(C8 *out, enc_S_Alerts const *a)
{
   if(a->noMag.asU16 == 0
            #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
      && a->mag.asU16 == 0
            #endif
      ) {
      strcpy(out, "(none)"); }
   else {
      // Show the non-Mag alerts.
      strcpy(out, "(");

      #define _MayCat(_name)            \
         if(a->noMag.bs._name == 1) { strcat(out, #_name "," ); }

      _MayCat(overflow)
      _MayCat(pressure)
      _MayCat(reverseFlow)
      _MayCat(negFlowRate)
      _MayCat(tamper)
      _MayCat(leak)
      _MayCat(program)
      _MayCat(temperature)
      _MayCat(endOfLife)
      _MayCat(emptyPipe)
      _MayCat(noFlow)

      #undef _MayCat

      // Mat also show the Mag alerts.
            #ifdef _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT
      if(a->mag.asU16 != 0) {
         strcat(out, "[Mag: ");

         #define _MayCat(_name)            \
            if(a->mag.bs._name == 1) { strcat(out, #_name ", " ); }

         _MayCat(adcError)
         _MayCat(lowBatt)
         _MayCat(badCoilDrive)
         _MayCat(measTimeout)
         _MayCat(flowStim)
         _MayCat(ovStatus)
         _MayCat(maxFlow)
         _MayCat(badSensor)

         #undef _MayCat

         strcat(out, "]");
      }
            #endif // _SENSUS_MSG_DECODER_INCLUDE_MAG_SUPPORT

      strcat(out, ")");
   }
   return out;
}

// ------------------------------------------------------------------------------------------
static C8 const *showOnePres(C8 *out, enc_T_Pressure p) {
   if(p == 0xFE) {
      sprintf(out, "error"); }
   else if(p == 0xFD) {
      sprintf(out, "storage"); }
   else if(p == 0xFF) {
      sprintf(out, "null"); }
   else {
      sprintf(out, "%2.1f", (float)p/10); }
   return out;
}

// ------------------------------------------------------------------------------------------
static C8 const * showPressures(C8 *out, S_Pres const *ps)
{
   C8 b0[20], b1[20], b2[20];
   sprintf(out, "pres(0.1 bar)(min/max/avg %s/%s/%s)",
           showOnePres(b0, ps->_min), showOnePres(b1, ps->_max), showOnePres(b2, ps->_avg) );
   return out;
}

// ------------------------------------------------------------------------------------------
static C8 const *showOneTmpr(C8 *out, enc_T_degC t) {
   if(t == 0xFE) {
      sprintf(out, "<error>"); }
   else if(t == 0xFD) {
      sprintf(out, "<storage>"); }
   else if(t == 0xFF) {
      sprintf(out, "<null>"); }
   else {
      sprintf(out, "%ddegC", t); }
   return out;
}

// ------------------------------------------------------------------------------------------
static C8 const * showTemperatures(C8 *out, enc_T_degC fluid, enc_T_degC ambient)
{
   C8 b0[20], b1[20];
   sprintf(out, "fluid %s ambient %s", showOneTmpr(b0, fluid), showOneTmpr(b1, ambient));
   return out;
}

// -----------------------------------------------------------------------------------------------
static C8 const *safeKStr(C8 const *str) {
   for(U8 i = 0; i < _enc_MaxSerNumChars+1; i++) {
      if(str[i] == '\0') {
         break; }
      else if(isprint(str[i]) == false) {
         return "bogus KStr"; }
   }
   return str;
}

// -----------------------------------------------------------------------------------------------
PUBLIC C8 const * Sensus_PrintMsgData(C8 *out, enc_S_MsgData const *ed)
{
   #define _indent "      "

   // Print the number of dials
   C8 b0[100];
   strcpy(b0, "encoders=%s weRead=%s alertFlags=%s\r\n" _indent "sn=\"%s\" k=\"%s\" " );
   C8 *p = b0 + strlen(b0);

   if(ed->dials >= 6 && ed->dials <= 9) {
      sprintf(p, "Tot %%0%dd(%d)", ed->dials, ed->dials); }
   else {
      sprintf(p, "Tot %%d(%d)", ed->dials); }

   strcat(b0, " rev %u uom 0x%02X flow %d%%\r\n" _indent "%s %s\r\n");

   C8 b1[100], b2[200], b3[100], b4[100], b5[500];

   sprintf(out, b0,
               sens_ShowEncoders(b1, ed->encoderType),
               showDataFlags(b2, &ed->weGot),
               sens_ShowAlerts(b3, &ed->alerts),
               safeKStr(ed->serialWord),
               safeKStr(ed->kStr),
               ed->rawTot,
               ed->noMag.revTot,
               ed->uom,
               ed->flowPcent,
               showPressures(b4, &ed->noMag.pres),
               showTemperatures(b5, ed->noMag.fluidDegC, ed->noMag.ambientDegC));
   return out;
}

// -----------------------------------------------------------------------------------------------
PUBLIC bool Sensus_EncodersEqual(enc_S_MsgData const *a, enc_S_MsgData const *b, bool chkMag)
{
   return
      a->encoderType == b->encoderType &&
      a->weGot.asU16 == b->weGot.asU16 &&
      a->alerts.noMag.asU16 == b->alerts.noMag.asU16 &&
      a->alerts.mag.asU16 == b->alerts.mag.asU16 &&
      a->rawTot == b->rawTot &&
      a->dials == b->dials &&
      a->flowPcent == b->flowPcent &&

      strncmp(a->kStr, b->kStr, _enc_MaxSerNumChars) == 0 &&
      strncmp(a->serialWord, b->serialWord, _enc_MaxSerNumChars) == 0 &&

      a->noMag.revTot == b->noMag.revTot &&
      a->noMag.pres._min == b->noMag.pres._min &&
      a->noMag.pres._max == b->noMag.pres._max &&
      a->noMag.pres._avg == b->noMag.pres._avg &&
      a->noMag.fluidDegC == b->noMag.fluidDegC &&
      a->noMag.ambientDegC == b->noMag.ambientDegC &&
      a->meterType == b->meterType &&

      (chkMag == false ||
         ( a->mag.secTot == b->mag.secTot &&
           a->mag.meterSize == b->mag.meterSize &&
           a->mag.prodCode == b->mag.prodCode &&
           a->mag.res == b->mag.res &&
           a->mag.biDirectional == b->mag.biDirectional
         )
      );
}

// --------------------------------------------- eof -----------------------------------------------------
