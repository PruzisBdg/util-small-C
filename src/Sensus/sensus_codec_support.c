/* -------------------------------- Sensus Codec (Test Harness) Support --------------------------------- */

#include "libs_support.h"
#include "sensus_codec.h"
#include <stdio.h>
#include <string.h>
#include "util.h"

// -------------------------------------------------------------------------------------------
static C8 const * showEncoders(C8 *out, enc_M_EncType t)
{
   typedef struct {enc_M_EncType typ; C8 const *name; } S_Name;

   S_Name const names[] = {
      {.typ = mADE,     .name = "ADE" },
      {       mGen1,            "Gen1" },
      {       mGen2,            "Gen2" },
      {       mHRE,             "HRE" },
      {       mHRE_LCD,         "HRE-LCD" },
      {       mMag,             "Mag" }};

   if(t == mAnyEncoder) {
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
   if(*(U16*)n == 0) {
      strcpy(out, "(No parsed data)"); }
   else {
      out[0] = '(';

      #define _MayCat(_name)            \
         if(n->_name == 1) { strcat(out, #_name ", " ); }

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
static C8 const *showAlerts(C8 *out, enc_S_Alerts const *a)
{
   if(*(U16*)a == 0) {
      strcpy(out, "(No alerts)"); }
   else {
      out[0] = '(';

      #define _MayCat(_name)            \
         if(a->_name == 1) { strcat(out, #_name ", " ); }

      _MayCat(overflow)
      _MayCat(pressure)
      _MayCat(reverseFlow)
      _MayCat(tamper)
      _MayCat(leak)
      _MayCat(program)
      _MayCat(temperature)
      _MayCat(endOfLife)
      _MayCat(emptyPipe)
      _MayCat(noFlow)

      #undef _MayCat
      strcat(out, ")");
   }
   return out;
}

// -----------------------------------------------------------------------------------------------
PUBLIC U16 Sensus_PrintMsgData(C8 *out, enc_S_MsgData const *ed)
{
   // Print the number of dials
   C8 b0[100];
   strcpy(b0, "%s got=%s alerts=%s sn=\"%s\" k=\"%s\" " );
   C8 *p = b0 + strlen(b0);

   if(ed->dials >= 6 && ed->dials <= 9) {
      sprintf(p, "Tot %%0%dd(%d)", ed->dials, ed->dials); }
   else {
      sprintf(p, "Tot %%d(%d)", ed->dials); }

   strcat(b0, " flow %d%%\r\n");

   C8 b1[100]; C8 b2[100]; C8 b3[100];

   return sprintf(out, b0,
               showEncoders(b1, ed->encoderType),
               showDataFlags(b2, &ed->weGot),
               showAlerts(b3, &ed->alerts),
               ed->serialWord,
               ed->kStr,
               ed->rawTot,
               ed->flowPcent
               );
}


// --------------------------------------------- eof -----------------------------------------------------
