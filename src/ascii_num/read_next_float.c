/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>      // isdigit()
#include "arith.h"      // GetPwr10Float()

// Module-local vars. Avoid the overhead of passing these to subroutines.
PRIVATE BIT isNeg;               // Manitssa or exponent is negative
PRIVATE U8 IDATA digitsAfterDP;

/*-----------------------------------------------------------------------------------------
|
|  finishMantissa()
|
------------------------------------------------------------------------------------------*/

PRIVATE void finishMantissa(float *m)
{
   *m = *m * (isNeg ? -1.0 : 1.0) * GetPwr10Float(-digitsAfterDP);
}



/*-----------------------------------------------------------------------------------------
|
|  ReadASCIIToFloat()
|
|  Return the next number from 'inStr' into 'out', skipping any spaces. The number
|  can be an integer, hex (0xnn), or floating point as 123.45, 1.67E5.
|
|  This parser takes the largest sequence which complete's a number (maximal match).
|  This is necessarily so, as e.g 234E5 musn't return 234.0
|
|  Returns pointer to the first char after the last byte read, else 0 if could not 
|  parse a number.
|
|  Note: If a number is NOT parsed '*out' is undefined. It may have been modified.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 const * ReadASCIIToFloat(U8 const *inTxt, float *out) 
{
   U8 DATA ch;                   // the current char
   U8 IDATA digitCnt = 0;        // digits so ofr in mantissa or exponent
   S16 exponent;

   BIT   gotSign = 0;            // Mantissa or exponent was prefixed by '+' or '-'
   BIT   readExp = 0;            // am reading the exponent
   BIT   readMantissa = 0;       // am reading the maintissa
   BIT   readHex = 0;            // is '0xnnnnn'
   BIT   gotDP = 0;              // mantissa has a decimal point
   
   *out = 0;                     // Zero the output.
   
   // Init these module-local vars.
   digitsAfterDP = 0;
   isNeg = 0;

   while(1)                               // Process 1 char each loop....
   {                                      // until parsed the largest number or until definitely can't complete any number
      ch = *inTxt;                        // char to be handled this pass

      if( !readMantissa && !readExp && !readHex )     // Haven't reached mantissa?
      {     
         readMantissa = 1;                // Will be true unless get SPC (below)
      
         if( ch == ' ' )                  // SPC?
         {
            readMantissa = 0;             // then continue until reach mantissa
         }
         else if(isdigit(ch) )            // 0-9?
         {                                // then we are at the mantissa
            *out = (ch - '0');            // and this is the 1st digit
            digitCnt = 1;
         }
         else if(ch == '+' )              // '+'?
         {                                // then we are at mantissa
            gotSign = 1;                  // then note the sign (which must NOT) be followed by a HEX number)
         }
         else if(ch == '-' )              // '-'
         {
            isNeg = 1;                    // it's a negtive number
            gotSign = 1;
         }
         else                             // else none of the above
         {
            return 0;                     // so there's no number to parse. Return 0.
         }         
      }
      else if( readMantissa )                   // Reading mantissa now?
      {
         if( ch == '.' )                        // Got a DP (in any position)?
         {
            gotDP = 1;                          // then mark it. Note. Leading DP's are allowed
         }
         else if( ch == 'x' || ch == 'X' )      // Hex, perhaps?   
         {
            if(digitCnt == 1 && *out == 0.0 && !gotDP )  // Only if it was just '0x'?
            {
               readMantissa = 0;                // then stop doing this
               readHex = 1;                     // and will read following chars as hex
               digitCnt = 0;                    // Reset the digit count.
            }
            else                                // else this 'X' terminates a non-hexadecimal number
            {                                   // which is than mantissa * sign (no DP)
               finishMantissa(out);             // Calculate mantissa from digits, sign and DP
               return inTxt;
            }
         }
         else if( (ch == 'E' || ch == 'e') )    // Exponent, perhaps?
         {
            if( digitCnt >= 1 )                 // Was preceded complete non-Hex number?
            {                                   // then it's a legal 'E' starting an exponent
               readMantissa = 0;                // so stop processing mantissa
               readExp = 1;                     // and start doing processing exponent                                                
               finishMantissa(out);             // Calculate mantissa from digits, sign and DP
               isNeg = 0;                       // Reset 'neg' flags prior to parsing the exponent
               exponent = 0;                    // Also zero the exponent itself
               digitCnt = 0;                    // and reset the digit count.
            }
            else                                // else wasn't preceded by a complete non-Hex e.g '+E' or '0x44E'
            {
               return 0;                        // so there's no number here, return 0
            }
         }
         else if( isdigit(ch) )                 // Another manitissa digit
         {
            *out = (10.0 * *out) + (ch - '0');  // then update mantissa and count
            digitCnt++;
            
            if( gotDP)                          // Got DP?
            {
               digitsAfterDP++;                 // then also update DP count.
            }
         }
         else                                   // else not a (Hex) digit or an exponent
         {                                      // so that's the end of the number
            finishMantissa(out);                // Calculate mantissa from digits, sign and DP
            return inTxt++;                     // and return ptr to next char
         }
      }              // end: reading mantissa
      
      else if( readExp )                        // Reading exponent?
      {
         if( ch == '+' )                        // '+'?
         {                                      // then just goto next char
         }
         else if( ch == '-' )                   // '-'?
         {
            isNeg = 1;                          // then exponent will be negative
         }
         else if( isdigit(ch) )                 // Next digit (of exponent)?
         {
            if( digitCnt > 3 )                  // Can't be > +/-999
            {
               return 0;                        // Illegal number, return 0;
            }
            else                                // else it's a legal addition ot the exponent
            {
               exponent = 10 * exponent + (ch - '0');    // Update the value
               digitCnt++;                      // and the count
            }
            
         }
         else                                   // else not '+', '-' or '0-9'?
         {
            if( digitCnt == 0 )                 // There were no digits?  Was just 'E' followed by no numbers
            {                                   // then it wasn't an exponent; just the mantissa was the number
               return inTxt++;                  // So return (at next char) with mantissa as it was.                       
            }
            else                                // else terminated legal exponent
            {                                   // so scale 'out' by exponent
               *out = *out * GetPwr10Float( isNeg ? -exponent : exponent);
               return inTxt++;                  // and return at next char  
            }
         }
      }                 // end: reading exponent
      
      else if(readHex)                          // Reading Hex?
      {
         if( !isxdigit(ch) )                    // NOT 0-9 A-F?
         {                                      // then this terminates a legal number, whether there were hex digits
                                                // after 'X' or no ('0X' read as zero)
            return inTxt++;                     // so return with what we've got  
         }
         else                                   // else (another) Hex digit
         {                                      // Add it latest total
            if( digitCnt > 8 )                  // More than 32bit?
            {
               return 0;                        // then be sensible, say we're lost
            }
            else                                
            {                                   // else add the digit to the total
               *out = (16 * *out) + HexToNibble(ch);
               digitCnt++;                      // and update digit count
            }
         }
      }                 // end: reading Hex
      
      inTxt++;       // next char
   }                 //  end: while(1)
   return 0;      
}

// -------------------------------------- eof ------------------------------------------------

 
