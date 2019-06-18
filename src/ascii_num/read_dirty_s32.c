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

PRIVATE void addChToAcc(S32 *n, U8 ch) { *n = (10 * *n) + ch - '0'; }



/*-----------------------------------------------------------------------------------------
|
|  ReadDirtyASCII_S32()
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

PUBLIC C8 const * ReadDirtyASCII_S32(C8 const *inTxt, S32 *out)
{
   U8 DATA  ch;         // the current char
   U8 IDATA digitCnt;   // significant digits so far
   BIT      isNeg;      // Got preceding '-'
   BIT      got1st;     // Got a 1st digit, even if it's a leading zero.

   isNeg = 0;
   digitCnt = 0;
   got1st = 0;

   while(1)
   {
      ch = *inTxt;                     // char to be handled this pass

      if(ch == '\0' && got1st == 0)    // End of string and no digits, not even zeros?
      {
         return NULL;                  // then we didn't get a number
      }
      else                             // otherwise keep going
      {
         if( !isdigit(ch) )            // This char isn't a digit?
         {
            if(got1st == 1)            // but we had at least one digit already? ...even a zero.
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
            if(digitCnt >= 9)          // 10 digits already? ...
            {
               /* ... then next digit may exceed +/-2147483647. If next digit will blow
                  the budget then close out the number with existing 9. Otherwise
                  add new digit to existing number.

                  Remember, the number being parsed is positive at this point. The
                  sign is added later. The positive and negative limits for an integer
                  are 1 different, and so limit test must reflect this.
               */
               if( (MulS32(*out, 10) + ch) > (isNeg ? (S64)MAX_S32 + 1 + '0' : (S64)MAX_S32 +'0') )   // 5 digits exceeds +/-S32?
               {
                  break;               // then break with existing 9 digit number.
               }                       // 'inTxt' is not advanced, so keeps 10th as start of next number.
               else                    // else still a legal integer with 9th digit added.
               {
                  addChToAcc(out, ch); // so add this 9th digit. Now we have a complete number
                  inTxt++;             // advance parse ptr to next digit
                  break;               // done with this number, so break from parse loop
               }
            }
            else                       // else 8 or fewer digits
            {
               if(got1st == 0)       // 1st digit?
               {
                  got1st = 1;
                  *out = 0;            // then zero accumulator.
               }

               if(digitCnt > 0 || ch != '0') // Not a leading zero?
               {
                  addChToAcc(out, ch); // then add new digit to accumulator
                  digitCnt++;          // and bump digits count.
               }
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
