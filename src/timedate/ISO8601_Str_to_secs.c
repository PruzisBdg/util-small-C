/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

/* ----------------------- ISO8601_ToSecs ---------------------------------

   Convert a ISO8601 TimeDate string in 'dateStr' into seconds since year 2000 in absTimeOut'.
   'dateStr' may be in any of the ISO8601 absolute time formats.

   Returns:
      - E_ISO8601_None if 'dateStr' could not be parsed. All of 'dateStr',
        'absTime' and 'utcOfs' remain unchanged.

      - E_ISO8601_TimeDate_UtcOfs if 'dateStr' had also a UTC offset, returned in 'utcOfs'.

      - E_ISO8601_Date, if 'dateStr' was just a date. 'absTime' is set to the start
        of the day i.e 1 sec past midnite.

      - E_ISO8601_WeekDate, if 'dateStr' was a weekdate. Again 'absTime' is set start
        of the day.

   If successful parse then 'dateStr' is advanced to the 1st char beyond the end of
   the ISO8601 text (which may be end of string).

   If 'strictLen' then 'dateStr' must be exactly one of the ISO8601 formats with no trailing
   chars. If strict is off then trailing chars are allowed e.g '2000-01-02T12:05:14zzzzzz'. However
   this also mean that malformed ISO8601 may match when they should not. E.g '2000-01-02T12:05', which
   is missing the secs field will match Date-only and be read as '2000-01-02'.
      Strict == off is useful when the Date-Time is embedded in a larger string (which it usually is);
   the parser will extract the Date-Time and advance to the remaining text.

   If not successful 'dateStr', absTime' and 'utcOfs' are not modified
*/
PUBLIC E_ISO8601Fmts ISO8601_ToSecs(C8 const **dateStr, T_Seconds32 *absTime, S32 *utcOfs_secs, BOOL strictLen)
{
   // sscanf() needs parms to 'ISO8601Formatter' to be all 'int'; cannot output directly to
   // S_DateTime because S_DateTime has some U8 field.
   typedef struct {int yr, mnth, week, day, hr, min, sec, ofsHr, ofsMin; } S_DTInt;

   // ISO8601 formats (see fmts[] below).
   typedef enum {
      OfsPlus=0, OfsMinus, OfsZ, Full, Packed, DateOnly,       // Gregorian YMD 'T'
      WeekDate, WeekOnly,
      WeekDatePacked, WeekOnlyPacked,                          // ISO Week-Date 'W'
      NoMatch = 0xFF } E_Fmts;

   bool isAWeekDate(E_Fmts f) {
      return
         f == WeekDate || f == WeekOnly || f == WeekDatePacked || f == WeekOnlyPacked
            ? true : false; }

   // --------------------------------------------------------------------------------------
   E_Fmts parse(C8 const **dateStr, S_DTInt *t)
   {
      typedef struct {C8 const *fmt; U8 len, numFields;} S_Fmts;
      #define _formatter(fmt, str, fields) {fmt, sizeof(str)-1, fields}

      PRIVATE S_Fmts fmts[] = {
         [OfsPlus]         = _formatter("%04d-%02d-%02dT%02d:%02d:%02d+%02d:%02d%n", "2000-01-01T00:00:00+00:00",  8 ),
         [OfsMinus]        = _formatter("%04d-%02d-%02dT%02d:%02d:%02d-%02d:%02d%n", "2000-01-01T00:00:00-00:00",  8 ),
         [OfsZ]            = _formatter("%04d-%02d-%02dT%02d:%02d:%02dZ%n",          "2000-01-01T00:00:00Z",       6 ),
         [Full]            = _formatter("%04d-%02d-%02dT%02d:%02d:%02d%n",           "2000-01-01T00:00:00",        6 ),
         [Packed]          = _formatter("%04d%02d%02dT%02d%02d%02d%n",               "20000101T000000",            6 ),
         [DateOnly]        = _formatter("%04d-%02d-%02d%n",                          "2000-01-01",                 3 ),
         [WeekDate]        = _formatter("%04d-W%02d-%01d%n",                         "2000-W01-7",                 3 ),
         [WeekOnly]        = _formatter("%04d-W%02d%n",                              "2000-W01",                   2 ),
         [WeekDatePacked]  = _formatter("%04dW%02d%01d%n",                           "2000W017",                   3 ),
         [WeekOnlyPacked]  = _formatter("%04dW%02d%n",                               "2000W01",                    2 ),
         };

      int parseCnt=2;
      E_Fmts parseInner(C8 const *dateStr, S_DTInt *t, int *_parseCnt) {

         /* Try each of the ISO8601 parses, from largest number of fields to the smallest, and use the first
            one that succeeds.
               Try longest first means that a full-length Date/Time/UTC will be tried before shorter
            alternatives - and won't match prematurely on e.g Date-only.

            For each possible parse, check that 'dateStr' is long enough; If 'strict' then it must be exact
            length. Then try a parse. If sscanf() grabs the correct number of fields then accept the match
            output the fields.
         */
         U16 len = strlen(dateStr);

         for(E_Fmts i = 0; i < RECORDS_IN(fmts); i++)
         {
            S_Fmts const *f = &fmts[i];

            if(len == f->len ||                             // 'dateStr' exactly same length as it's ISO8601 match? OR
               (strictLen == false && len >= f->len)) {     // if 'strictLen' is off, 'dateStr' is at least as long?

               if(f->numFields == 8) {
                  if(sscanf(dateStr, f->fmt, &t->yr, &t->mnth, &t->day, &t->hr, &t->min, &t->sec, &t->ofsHr, &t->ofsMin, _parseCnt) == 8) {
                     return i; }}
               else if(f->numFields == 6) {
                  if(sscanf(dateStr, f->fmt, &t->yr, &t->mnth, &t->day, &t->hr, &t->min, &t->sec, _parseCnt) == 6) {
                        return i; }}
               else if(f->numFields == 3) {
                  if(i == DateOnly) {
                     if(sscanf(dateStr, f->fmt, &t->yr, &t->mnth, &t->day, _parseCnt) == 3) {
                        return i; }}
                  else if(i == WeekDate || i == WeekDatePacked) {
                     if(sscanf(dateStr, f->fmt, &t->yr, &t->week, &t->day, _parseCnt) == 3) {
                        return i; }}}
               else if(f->numFields == 2) {                 // Can be only WeekOnly or WeekOnlyPacked?
                  if(sscanf(dateStr, f->fmt, &t->yr, &t->week, _parseCnt) == 2) {
                     t->day = 1;                            // Successfully read a week-only WeekDate so Day <- 1 (Mon).
                     return i; }}
            }
         }
         *_parseCnt = 0;
         return NoMatch;
      } // ends: parseInner()


      E_Fmts fmtTag;
      if(NoMatch == ( fmtTag = parseInner(*dateStr, t, &parseCnt))) {
         return NoMatch; }
      else {
         if(parseCnt != fmts[fmtTag].len) {
            return NoMatch; }
         else {
            *dateStr += parseCnt;
            return fmtTag;}}
   } // ends: parse()

   /* Prefill a 'S_DTInt' with zeros. Depending on 'dateStr' 2 to 8 of the fields will
      be filled by the parse.
   */
   S_DTInt t = (S_DTInt){.yr=0, .mnth=0, .day=0, .hr=0, .min=0, .sec=0, .ofsHr=0, .ofsMin=0};

   E_Fmts fmt;

   if( (fmt = parse(dateStr, &t)) == NoMatch) {                            // Parsed 'dateStr'?. No!...
      return E_ISO8601_None; }                                             // ...then got nothing.
   else {                                                                  // else was some ISO8601, successfully read.
      if( isAWeekDate(fmt) == true )                                       // WeekDate, with or without a Day, packed or not-packed?
      {
         // Successfully convert to a WeekDate? Then return seconds.
         T_Days16 days;
//printf("yr %u wk %u day %u -> days %u\r\n", t.yr, t.week, t.day, days);
         if(_Illegal_Days16 != (days = WeekDateToDays( &(S_WeekDate){.yr = t.yr, .week = t.week, .day = t.day } ))) {
            *absTime = (T_Seconds32)days * 24 * 3600;
            return E_ISO8601_WeekDate; }
         return E_ISO8601_None;                                            // Fail! Was formatted as a WeekDate but contents were not legal.
      }
      else                                                                 // else must be some Gregorian (YMD) time.
      {
         // For any ISO8601 TimeDate, Date or WeekDate transcribe 'S_DTInt' fields
         // into a 'S_DateTime'.
         S_DateTime dt = _S_DateTime_Make(t.yr, t.mnth, t.day, t.hr, t.min, t.sec);

         if( !Legal_YMDHMS(&dt) ) {                                        // Values in string are legal?
            return E_ISO8601_None; }                                       // No! One or more illegal fields -> No valid output
         else {                                                            // else correct format and correct numbers
            *absTime = YMDHMS_To_Secs(&dt);                                // Make into seconds since 1st Jan 2000.

            switch(fmt) {                                                  // What ISO8601 did we get?
               case OfsZ: {                                                // TimeDate + zero-UTC offset?
                  if(utcOfs_secs != NULL) {                                // there's a place to write the offset
                     *utcOfs_secs = 0; }                                   // then write zero-offset there.
                  return E_ISO8601_TimeDate_UtcOfs; }                      // and say that's wot we got

               case OfsPlus: {                                             // Timedate with positive UTC offset?
                  if(utcOfs_secs != NULL) {                                // There's a place to write this offset?
                     *utcOfs_secs = 3600 * t.ofsHr + 60 * t.ofsMin; }      // then write as seconds.
                  return E_ISO8601_TimeDate_UtcOfs; }                      // and say we got UTC offset (too)
               case OfsMinus: {                                            // ...with negative UTC offset?
                  if(utcOfs_secs != NULL) {
                     *utcOfs_secs = -(3600 * t.ofsHr + 60 * t.ofsMin); }   // seconds are negative
                  return E_ISO8601_TimeDate_UtcOfs; }

               case Full:                                                  // Time/Date (no UTC offset)...
               case Packed:                                                // ... either full or packed?
                  return E_ISO8601_TimeDate;
               case DateOnly:                                              // just Date
                  return E_ISO8601_Date;
               // LCOV_EXCL_START               .... Never reach 'default' because of 'NoMatch' above.
               default:
                  return E_ISO8601_None;                                   // else some Fail.
               // LCOV_EXCL_STOP
               }
            }
      } // isAWeekDate(fmt) == false
   }
}


/* ----------------------- ISO8601_EzToSecs ---------------------------------

   A simpler (original) version of ISO8601_ToSecs() (above). This simplified version
   is always 'strict' i.e 'dateStr' must be exactly an ISO8601 Time/Date and nothing
   more. It also does not return UTC offset, even if that is appended to 'dateStr'.

   Returns TRUE if parsed any ISO8601 Date/Time or Date; otherwise FALSE and 'absTimeut'
   is unmodified.
*/
PUBLIC BOOL ISO8601_EzToSecs(C8 const *dateStr, T_Seconds32 *absTimeOut ) {
   C8 const *p = dateStr;
   return
      ISO8601_ToSecs(&p, absTimeOut, NULL, true) == E_ISO8601_None
         ? false : true;
}




// --------------------------------- eof ---------------------------------------
