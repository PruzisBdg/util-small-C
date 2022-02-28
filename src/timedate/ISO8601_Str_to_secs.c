/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

/* ----------------------- ISO8601StrToSecs ---------------------------------

   Convert a ISO8601 date+time string in 'dateStr' into seconds since year 2000
   in absTimeOut'. 'dateStr' must be in the extended formay, i.e:
            "YYYY-MM-DDTHH:MM:SS"
   ('-' separates date elements; 'T' separates the date from the time and
     ':' separates the time elements.)

   Return TRUE if success, FALSE if,
      - dateStr format is not ISO8601
      - any of year, month etc illegal value  e.g month = 44
      - year is beyond 2130, because U32 can count 132 years (beyond 2000)
      - year is before 2000.

   If return FALSE then 'absTimeOut'is not updated.
*/
PUBLIC BOOL ISO8601StrToSecs( C8 const *dateStr, T_Seconds32 *absTimeOut ) {

   // sscanf() needs parms to 'ISO8601Formatter' to be all 'int'; cannot output directly to
   // S_DateTime because S_DateTime has some U8 field.
   typedef struct {int yr, mnth, day, hr, min, sec, ofsHr, ofsMin; } S_DTInt;

   // --------------------------------------------------------------------------------------
   typedef enum {OfsPlus=0, OfsMinus, OfsZ, Full, Packed, DateOnly, DayWeek, WeekOnly, NoMatch = 0xFF } E_Fmts;

   E_Fmts parse(C8 const *dateStr, S_DTInt *t) {

      typedef struct {C8 const *fmt; U8 len, numFields;} S_Fmts;
      #define _formatter(fmt, str, fields) {fmt, sizeof(str)-1, fields}

      PRIVATE S_Fmts fmts[] = {
         [OfsPlus]   = _formatter("%04d-%02d-%02dT%02d:%02d:%02d+%02d:%02d", "2000-01-01T00:00:00+00:00",  8 ),
         [OfsMinus]  = _formatter("%04d-%02d-%02dT%02d:%02d:%02d-%02d:%02d", "2000-01-01T00:00:00-00:00",  8 ),
         [OfsZ]      = _formatter("%04d-%02d-%02dT%02d:%02d:%02dZ",          "2000-01-01T00:00:00Z",       6 ),
         [Full]      = _formatter("%04d-%02d-%02dT%02d:%02d:%02d",           "2000-01-01T00:00:00",        6 ),
         [Packed]    = _formatter("%04d%02d%02dT%02d%02d%02d",               "20000101T000000",            6 ),
         [DateOnly]  = _formatter("%04d-%02d-%02d",                          "2000-01-01",                 3 ),
         //[DayWeek]   = _formatter("%04d-W%02d-%01d",                         "2000-W01-7",                 3 ),
         //[WeekOnly]  = _formatter("%04d-W%02d",                              "2000-W01",                   2 ),
         };

      /* Try each of the ISO8601 parses, from longest to shortest, and use the first one that
         succeeds.
      */
      U16 len = strlen(dateStr);

      for(E_Fmts i = 0; i < RECORDS_IN(fmts); i++)
      {
         S_Fmts const *f = &fmts[i];

         if(len == f->len) {
            if(f->numFields == 8) {
               if(sscanf(dateStr, f->fmt, &t->yr, &t->mnth, &t->day, &t->hr, &t->min, &t->sec, &t->ofsHr, &t->ofsMin) == 8) {
                  return i; }}
            else if(f->numFields == 6) {
               if(sscanf(dateStr, f->fmt, &t->yr, &t->mnth, &t->day, &t->hr, &t->min, &t->sec) == 6) {
                  return i; }}
            else if(f->numFields == 3) {
               if(i == DateOnly) {
                  if(sscanf(dateStr, f->fmt, &t->yr, &t->mnth, &t->day) == 3) {
                     return i; }}
               else if(i == DayWeek) {
                  int week, weekday;
                  if(sscanf(dateStr, f->fmt, &t->yr, &week, &weekday) == 3) {
                     return i; }}}
            else if(f->numFields == 2) {
               int week;
               if(sscanf(dateStr, f->fmt, &t->yr, &week) == 2) {
                  return i; }}
         }
      }
      return NoMatch;
   }

   // Prefill a 'S_DTInt' with zeros. Depending on 'dateStr' 2 to 8 of the fields will
   // be filled by the parse.
   S_DTInt t = (S_DTInt){.yr=0, .mnth=0, .day=0, .hr=0, .min=0, .sec=0, .ofsHr=0, .ofsMin=0};

   if(parse(dateStr, &t) == NoMatch) {
      return FALSE; }
   else {                                                                           // else was a ISO8601, successfully read.
      S_DateTime dt = _S_DateTime_Make(t.yr, t.mnth, t.day, t.hr, t.min, t.sec);    // Transcribe 'S_DTInt' fields into a 'S_DateTime'.

      if( !Legal_YMDHMS(&dt) ) {                                                    // Values in string are legal?
         return FALSE;                                                              // No! One or more illegal field -> fail
         }
      else {                                                                        // else correct format and correct numbers
         *absTimeOut = YMDHMS_To_Secs(&dt);                                         // Make into seconds since 1st Jan 2000.
         return TRUE;                                                               // and return success.
         }
      }
}

// --------------------------------- eof ---------------------------------------
