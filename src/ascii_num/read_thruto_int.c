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
|  ReadThruToASCIIInt()
|
|  Return the next number from 'inStr' into 'out', skipping anything allowed by 'stripThis()'
|  up to the string terminator '\0'. The number must be a signed 16bit integer.
|
|  If 'stripThis()' is NULL the skip just whitespace i.e isspace()
|
|  This parser takes the largest sequence which completes an integer (maximal
|  match). If a digit is directly preceded by '-', the number is read as negative.
|
|  Returns pointer to the first char after the last byte read, else 0 if reached
|  '\0' (NULL) without parsing a number
|
|  Note: If a number is NOT parsed '*out' is 0.
|
------------------------------------------------------------------------------------------*/

PUBLIC C8 const * ReadThruToASCIIInt(C8 const *inTxt, S16 *out, BIT(*stripWith)(C8 ch))
{
   U8 DATA  ch;         // the current char
   U8 IDATA digitCnt;   // significant digits so far
   BIT      isNeg;      // Got preceding '-'
   BIT      got1st;     // Got a 1st digit, even if it's a leading zero.
   BIT      leadPlus;

   isNeg = 0;
   digitCnt = 0;
   leadPlus = 0;
   got1st = 0;

   while(1)
   {
      ch = *inTxt;                              // char to be handled this pass

      if(ch == '\0' && got1st == 0)             // End of string and no digits, not even zeros?
      {
         return NULL;                           // then we didn't get a number
      }
      else                                      // otherwise keep going
      {
         if( !isdigit(ch) )                     // This char isn't a digit?
         {
            if(got1st == 1)                     // but we had at least one digit already? ...even a zero.
            {
               break;                           // then we have a number; break to return it.
            }
            else                                // else we don't have number yet. Keep going
            {
               if(ch == '-')                    // Negative (perhaps)?
               {
                  if(stripWith != NULL)         // There's a Stripper?
                  {
                     if(!stripWith('-'))        // This Stripper does NOT take '-'?
                     {
                        isNeg = 1;              // then mark the '-', maye be a real minus, directly in front of the 1st digit of new number
                     }
                  }
                  else                          // else there's no Stripper, so keep the '-' always
                  {
                     isNeg = 1;
                  }
               }
               else                             // else not a '-'
               {
                  if(ch == '+')                 // Maybe e.g '+1234'?
                  {
                     leadPlus = 1;              // Mark; will bail if next char is not a digit.
                  }
                  else                          // else not '+' or '-'
                  {                             // See if it can be stripped.
                     if(stripWith == NULL)      // No custom strip-list?
                     {                          // then strip just leading spaces
                        if(!isspace(ch) || isNeg == 1)        // Not space? OR had '-' earlier
                        {
                           return NULL;         // then got no number OR that earlier '-' wasn't directly before a digit, so not a minus sign. Fail & done.
                        }
                     }
                     else                       // else there's a custom strip-list
                     {
                        if(leadPlus == 1)       // Got a '+' earlier?
                        {
                           if(!stripWith('+'))  // May NOT strip this '+'
                           {
                              return NULL;      // then got no number. Fail & done.
                           }
                           leadPlus = 0;        // Clear so we don't re-ask above.
                        }
                        if(isNeg == 1)          // Got a '-' earlier?
                        {
                           if(!stripWith('-'))  // May NOT strip this '+'
                           {
                              return NULL;      // then got no number. Fail & done.
                           }
                           isNeg = 0;           // Clear so we don't re-ask above.
                        }
                        if(!stripWith(ch))      // Current 'ch' may NOT be stripped
                        {
                           return NULL;         // then, again, got no number. Fail & done.
                        }
                     }
                  }
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
