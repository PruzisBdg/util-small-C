/*-----------------------------------------------------------------------------------------
|
|  ReadBinaryWord()
|
|  Return the binary number from 'inTxt' into 'out', skipping anything else except
|  the string terminator '\0' (a 'dirty' read). The number a binary of the form(s) 
|  0x1101101,  0x0100_10101_0101_0101.  It must start with '0x' and be no more than 
|  16 digits. Any number of underscores in any psostion(s) are OK.
|
|  This parser takes the largest sequence which complete's an integer (maximal 
|  match). The number may be bounded by any non-numeric chars, whitespace or
|  non-printing chars. The number may not be split. If a digit is directly preceeded
|  by '-', the number is read as negative.
|
|  Returns pointer to the first char after the last byte read, else 0 if reached
|  '\0' without parsing a number
|
|  Note: If a number is NOT parsed '*out' is undefined. It may have been modified.
|
|
|  
|   
------------------------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

PUBLIC U8 const * ReadDirtyBinaryWord(U8 const *inTxt, U16 *out) 
{
   U8          ch;                     // the current char
   static BIT  gotLeadZero, gotB;      // got '0' then 'b'
   static U8   digitCnt;               // Up to 16 binary digits.
   
   gotLeadZero = 0;     // Nothing parsed yet.
   gotB = 0;
   
   digitCnt = 0;        // No binary digits
   *out = 0;            // Zero accumulator
   
   while(1)
   {
      ch = *inTxt;                              // Get current char
   
      if( ch == '\0' )                          // End of string?
      { 
         if(!digitCnt)                          // Didin't get any numbers?
            { return 0; }                       // then failed
         else
            { return inTxt; }                   // else return end-of-string
      }
      else                                      // else something othe rthan '\0'
      {
         if( !gotLeadZero )                     // No leading zero yet
         {
            if(ch == '0')                       // Got '0'?
               { gotLeadZero = 1; }             // then it is the leading zero
         }
         else                                   // else got lead zero at least
         {
            if(!gotB)                           // Need 'b' right after leading zero?
            {
               if(ch == 'b' || ch == 'B')       // Got 'b' or 'B'?
                  { gotB = 1; }                 // then mark that we did
               else
                  { gotLeadZero = 0; }          // else go back to looking for lead zero
            }
            else                                // Got 'Ox'?; must follow with e.g '0100_0110'
            {
               if( ch == '_') {}                      // Underscore? , that's OK, keep going.
               else if( ch == '1' || ch == '0' )      // '1' or '0'?
               {
                  digitCnt++;                         // then bump digit count
                  *out = (*out << 1) + (ch == '1' ? 1 : 0);    // and update the total
                  
                  if( digitCnt >= 16 )          // Got a word?
                     { return inTxt; }          // then success, return ptr to next char.
               }
               else                             // else we didn't get '0','1' or '_'
               {
                  if( digitCnt )                // Got at least 1 digit?
                     { return inTxt; }          // Return success, the 1st char after what we parsed
                  else 
                     { gotLeadZero = 0;  }      // else start over.
               }
            }
         }
      }      
      inTxt++;                                  // Next char
   }
   return 0;                                    // Shouldn't get here bit... keeps some compilers happy
}

