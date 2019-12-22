/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <stdio.h>

/* ----------------------- ISO8601StrToSecs ---------------------------------

   Convert a ISO8601 date+time string in 'dateStr' into seconds since year 2000
   in absTimeOut'. 'dateStr' must be in the extended formay, i.e:
            "YYYY-MM-DDTHH:MM:SS"
   ('-' separates date elements; 'T' separates the date from the time and
     ':' separates the time elements.)

   Return TRUE if success, FALSE if,
      -  dateStr format is not ISO8601
      - any of year, month etc illegal value  e.g month = 44
      - year is beyond 2130, because U32 can count 132 years (beyond 2000)
      - year is before 2000.
*/
extern U8 const ISO8601Formatter[];

PUBLIC BOOLEAN ISO8601StrToSecs( C8 const *dateStr, T_Seconds32 *absTimeOut ) {

   S_DateTime t;

   if( sscanf(dateStr, ISO8601Formatter, &t.ymd.yr, &t.ymd.mnth, &t.ymd.day, &t.hr, &t.min, &t.sec) != 6) { // Wasn't ISO8601 string?
      return FALSE;                                                                    // then fail
      }
   else if( !Legal_YMDHMS(&t) ) {                                                      // else values in string are legal?
      return FALSE;                                                                    // No! One or more illegal field -> fail
      }
   else {                                                                              // else correct format and correct numbers
      *absTimeOut = YMDHMS_To_Secs(&t);                                                // Make into seconds since 1st Jan 2000.
      return TRUE;                                                                     // and return success.
      }
}

// --------------------------------- eof ---------------------------------------
