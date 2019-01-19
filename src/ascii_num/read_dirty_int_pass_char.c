/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/


#include "libs_support.h"
#include "util.h"
#include <ctype.h>      // isdigit()
#include "arith.h"

PRIVATE U8  IDATA  digitCnt;   // digits so far
PRIVATE BIT        isNeg;
PRIVATE S16 IDATA  n;


/*-----------------------------------------------------------------------------------------
|
|  addChToAcc()
|
|  Add next ASCII digit 'ch' to the integer accumulator 'n'
|
------------------------------------------------------------------------------------------*/

PRIVATE void addChToAcc(U8 ch) { n = (10 * n) + ch - '0'; }

   
/*-----------------------------------------------------------------------------------------
|
|  ReadDirtyASCIIInt_ByCh_Init()
|
|  Call to reset an integer parse sequence.
|
------------------------------------------------------------------------------------------*/

PUBLIC void ReadDirtyASCIIInt_ByCh_Init(void)
{
   n = 0; 
   isNeg = 0;
   digitCnt = 0;
} 

/*-----------------------------------------------------------------------------------------
|
|  ReadDirtyASCIIInt_ByCh()
|
|  Same as ReadDirtyASCIIInt() but gets fed a char at a time. 
|
|  Feed successive 'ch' to this function. It parses the next signed 16bit integer
|  and returns it in 'out'. Returns 1 if completed and integer, else 0.
|
|  This parser takes the largest sequence which complete's an integer (maximal 
|  match). The number may be bounded by any non-numeric chars, whitespace or
|  non-printing chars. The number may not be split. If a digit is directly preceeded
|  by '-', the number is read as negative. If the string of digits is unbroken then
|  the function grabs the largest 4 or 5 digits which form a legal integer (+/-32768).
|  The following digit is the start of the next number.
|
|  Returns pointer to the first char after the last byte read, else 0 if reached
|  '\0' without parsing a number
|
|  Note: If a number is NOT parsed '*out' is undefined. It may have been modified.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT ReadDirtyASCIIInt_ByCh(U8 ch, S16 *out) 
{
   if( !isdigit(ch) )            // This char isn't a digit?
   {
      if(digitCnt)               // but we have at least one already?
      {
         *out = isNeg ? -n : n;  // then we have a number. Apply sign
         n = 0;                  // Zero accumulator for next number to be parsed
         isNeg = 0;              // Unless we get '-', the next number is positive.
         digitCnt = 0;           // for which we have no digits for yet
         return 1;               // and return 1 saying we got a number
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
         if( (MulS16(n, 10) + ch) > (isNeg ? 32768L+'0' : 32767L+'0') )   // 5 digits exceeds +/-S16?
         {
            n = ch - '0';        // then keep 5th as start of next number.
            isNeg = 0;           // which is postive, there was no preceeding '-'
            digitCnt = 1;        // and we have the 1st digit already
         }
         else                    // else 5th digit makes a legal integer
         {
            addChToAcc(ch);      // so add it
            digitCnt = 0;        // Next digit must start a new number, so zero digit count.
         }                       
         *out = isNeg ? -n : n;  // Either way, we have a number. Apply sign
         if(!digitCnt) {n = 0;}  // Starting a new digit? then zero the accumulator.
         return 1;               // and return 1 saying we got a number
      }
      else                       // else 3 or fewer digits
      {                          
         addChToAcc(ch);         // so add new digit to these
         digitCnt++;             // and keep going.
      }
   }
   return 0;                     // No number yet; Keep going  
} 

// ----------------------------------- eof --------------------------------------- 
