/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>      // isdigit()
#include "arith.h"

/*-----------------------------------------------------------------------------------------
|
|  addChToAcc()
|
|  Add next ASCII digit 'ch' to the integer accumulator 'n'
|
------------------------------------------------------------------------------------------*/

PRIVATE void addChToAcc(S16 *n, U8 ch) { *n = (10 * *n) + ch - '0'; }



/*-----------------------------------------------------------------------------------------
|
|  ReadDirtyASCIIInt()
|
|  Return the next number from 'inStr' into 'out', skipping anything else except
|  the string terminator '\0'. The number must be a signed 16bit integer.
|
|  This parser takes the largest sequence which completes an integer (maximal
|  match). The number may be bounded by any non-numeric chars, whitespace or
|  non-printing chars. The number may not be split. If a digit is directly preceded
|  by '-', the number is read as negative.
|
|  Returns pointer to the first char after the last byte read, else 0 if reached
|  '\0' (NULL) without parsing a number
|
|  Note: If a number is NOT parsed '*out' is 0.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const * ReadDirtyASCIIInt(C8 const *inTxt, S16 *out)
{
   U8 DATA  ch;         // the current char
   U8 IDATA digitCnt;   // digits so far
   BIT      isNeg;

   isNeg = 0;
   digitCnt = 0;

   while(1)
   {
      ch = *inTxt;                     // char to be handled this pass

      if(ch == '\0' && !digitCnt)      // End of string and no digits?
      {
         return NULL;                     // then we didn't get a number
      }
      else                             // otherwise keep going
      {
         if( !isdigit(ch) )            // This char isn't a digit?
         {
            if(digitCnt)               // but we have at least one already?
            {
               break;                  // then we have a number; break to return it.
            }
            else                       // else we don't have number yet. Keep going
            {
               if(ch == '-')           // Negative (perhaps)?
               {
                  isNeg = 1;           // then mark that, it may in front of the 1st digit of new number
               }
               else                    // else not a '-'
               {
                  isNeg = 0;           // so cancel 'neg' (if it was set)
               }
            }
         }
         else                          // else this char is a digit
         {
            if(digitCnt >= 4)          // 4 digits already? ...
            {
               /* ... then next digit may exceed +-32767. If next digit will blow
                  the budget then close out the number with existing 4. Otherwise
                  add new digit to existing number.

                  Remember, the number being parsed is positive at this point. The
                  sign is added later. The positive and negative limits for an integer
                  are 1 different, and so limit test must reflect this.
               */
               if( (MulS16(*out, 10) + ch) > (isNeg ? 32768L+'0' : 32767L+'0') )   // 5 digits exceeds +/-S16?
               {
                  break;               // then break with existing 4 digit number.
               }                       // 'inTxt' is not advanced, so keeps 5th as start of next number.
               else                    // else still a legal integer with 5th digit added.
               {
                  addChToAcc(out, ch); // so add this 5th digit. Now we have a complete number
                  inTxt++;             // advance parse ptr to next digit
                  break;               // done with this number, so break from parse loop
               }
            }
            else                       // else 3 or fewer digits
            {
               if(digitCnt == 0)       // 1st digit?
               {
                  *out = 0;            // then zero accumulator.
               }

               addChToAcc(out, ch);    // so add new digit to these
               digitCnt++;             // and keep going.
            }
         }
      }
      inTxt++;                         // Onto the next char.
   }

   // We have a number. Apply sign
   if(isNeg) { *out = -*out; }
   return inTxt;                       // and return with ptr to 1st char after the number
}

// ----------------------------------- eof ---------------------------------------
