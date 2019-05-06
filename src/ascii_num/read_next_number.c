/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <ctype.h>      // isdigit()
#include "arith.h"      // GetPwr10Float()
#include <float.h>
#include <math.h>
#include "wordlist.h"

// Module-local vars. Avoid the overhead of passing these to subroutines.
PRIVATE BIT isNeg;               // Manitssa or exponent is negative
PRIVATE U8 IDATA digitsAfterDP;

/*-----------------------------------------------------------------------------------------
|
|  finishMantissa()
|
------------------------------------------------------------------------------------------*/

PRIVATE float finishMantissa(float *m)
{
   *m = *m * (isNeg ? -1.0 : 1.0) * GetPwr10Float(-digitsAfterDP);
   return *m;
}

/*-----------------------------------------------------------------------------------------
|
|  finishInt()
|
|  Insert 'n' into 'fi' as a signed or unsigned int, depending on sign 'isNeg'.
|  Return false if 'n' won't fit
|
------------------------------------------------------------------------------------------*/

PRIVATE bool finishInt(T_FloatOrInt *fi, U32 n, BIT _isNeg)
{
   fi->gotInt = true;               // Is integer, signed or unsigned.

   if(isNeg == 1)                   // Negative?
   {
      if(n > (U32)MAX_S32+1)        // But will not fit in S32?
      {
         return false;              // fail.
      }
      else
      {
         fi->num.asS32 = -(S32)n;   // else make negative an put in S32
         fi->gotUnsigned = false;   // and it is signed.
      }
   }
   else                             // else positive number.
   {
      if(n > MAX_S32)               // Will not fit in S32?
      {
         fi->num.asU32 = n;         // then put it in U32
         fi->gotUnsigned = true;    // and is unsigned.
      }
      else
      {
         fi->num.asS32 = n;         // else it's signed in S32.
         fi->gotUnsigned = false;
      }
   }
   return true;      // Success; made an integer in 'fi'.
}

/*-----------------------------------------------------------------------------------------
|
|  finishFloat()
|
------------------------------------------------------------------------------------------*/

PRIVATE bool finishFloat(T_FloatOrInt *out, float f)
{
   if( fpclassify(f) & (FP_INFINITE | FP_NAN) != 0)
   {
      return false;
   }
   else
   {
      out->num.asFloat = f;        // then return it as float
      out->gotInt = false;          // Say its a float (i.e not an int)
      out->gotUnsigned = false;     // Park to be tidy.
      return true;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  ReadASCIIToNum()
|
|  Return the next number from 'inStr' into 'out', skipping any spaces/delimiters.
|  The number can be an integer, hex (0xnn), or floating point as 123.45, 1.67E5.
|
|  Delimiters to be skipped are defined in Str_Delimiters (from the wordlist package).
|  For example, to grab "TAG = number" set Str_Delimiters = " ="; starting from tag will boof
|  past " = " and grab 'number'.
|
|  This parser takes the largest sequence which complete's a number (maximal match).
|  This is necessarily so, as e.g 234E5 musn't return 234.0
|
|  Returns pointer to the first char after the last byte translated to a number, else NULL
|  if could not parse a number.
|
|  'T_FloatOrInt *out' can hold a float, U32 or S32. Rules are:
|     - A hex number e.g 0x1234 is always returned as U32 i.e
|          out.gotInt <- true AND out.gotUnsigned <- true
|
|     - else, if out.alwaysOutputFloat == true, output a float
|
|     - else, if the number is e.g '12.34' or '12E34' output a float
|
|     - else the number is an int e.g '1234'. If that number is positive and > MAX_S32
|       then output a U32 (out.gotUnsigned <- true)
|
|     - else output as S32 (out.gotUnsigned <- false)
|
|  Return NULL if could not parse a number into out, which may be because:
|     - there was no number.
|     - number was a fixed or floating point format but was too large for float
|     - number was positive integer but > MAX_U32
|     - number was negative integer but < MIN_S32
|
|  Note: If a number is NOT parsed '*out' is undefined. It may have been modified.
|
------------------------------------------------------------------------------------------*/
PUBLIC U8 const * ReadASCIIToNum(U8 const *inTxt, T_FloatOrInt *out)
{
   U8 DATA ch;                   // the current char
   U8 IDATA digitCnt = 0;        // digits so ofr in mantissa or exponent
   S16 exponent;

   BIT   gotSign = 0;            // Mantissa or exponent was prefixed by '+' or '-'
   BIT   readExp = 0;            // am reading the exponent
   BIT   readMantissa = 0;       // am reading the maintissa
   BIT   readHex = 0;            // is '0xnnnnn'
   BIT   gotDP = 0;              // mantissa has a decimal point

   // Init these module-local vars.
   digitsAfterDP = 0;
   isNeg = 0;

   float fl;
   U32 _u32;

   while(1)                               // Process 1 char each loop....
   {                                      // until parsed the largest number or until definitely can't complete any number
      ch = *inTxt;                        // char to be handled this pass

      if( !readMantissa && !readExp && !readHex )     // Haven't reached mantissa?
      {
         if( Str_Delimiter(ch) == 1)            // A delimiter specified by 'Str_Delimiters'?, Default is SPC.
         {
            if(gotSign)                         // Already read '+' or '-'?
            {                                   // then '-' or '+' is not connected to a number.
               return NULL;                     // so fail.
            }
            else
            {
               readMantissa = 0;                // else continue until reach mantissa
            }
         }
         else if(isdigit(ch) )                  // 0-9?
         {                                      // then we are at the mantissa
            fl = _u32 = (ch - '0');             // and this is the 1st digit, (as both int and float as we don't yet know which the output will be).
            digitCnt = 1;
            readMantissa = 1;
         }
         else if(ch == '+' )                    // '+'?
         {                                      // then we are at mantissa
            gotSign = 1;                        // then note the sign (which must NOT) be followed by a HEX number)
            isNeg = 0;                          // override any preceding '-' (i.e -+123 -> +123)
         }
         else if(ch == '-' )                    // '-'
         {
            isNeg = 1;                          // it's a negative number
            gotSign = 1;
         }
         else if(ch == '.')
         {
            fl = 0;
            readMantissa = 1;
            gotDP = 1;
         }
         else                                   // else none of the above
         {
            return NULL;                        // so there's no number to parse. Return 0.
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
            if(digitCnt == 1 && fl == 0.0 && !gotDP )  // Only if it was just '0x'?
            {
               readMantissa = 0;                // then stop doing this
               readHex = 1;                     // and will read following chars as hex
               digitCnt = 0;                    // Reset the digit count.
            }
            else                                // else this 'X' terminates a non-hexadecimal number
            {                                   // which is than mantissa * sign (no DP)
               if(gotDP || out->reqFloat)
               {
                  out->num.asFloat = finishMantissa(&fl);             // Calculate mantissa from digits, sign and DP
                  out->gotInt = false;
               }
               else
               {
                  if( finishInt(out, _u32, isNeg) == false)
                  {
                     return NULL;
                  }
               }
               return inTxt;
            }
         }
         else if( (ch == 'E' || ch == 'e') )    // Exponent, perhaps?
         {
            if( digitCnt >= 1 )                 // Was preceded complete non-Hex number?
            {                                   // then it's a legal 'E' starting an exponent
               readMantissa = 0;                // so stop processing mantissa
               readExp = 1;                     // and start doing processing exponent
               finishMantissa(&fl);             // Calculate mantissa from digits, sign and DP
               isNeg = 0;                       // Reset 'neg' flags prior to parsing the exponent
               exponent = 0;                    // Also zero the exponent itself
               digitCnt = 0;                    // and reset the digit count.
            }
            else                                // else wasn't preceded by a complete non-Hex e.g '+E' or '0x44E'
            {
               return NULL;                     // so there's no number here, return 0
            }
         }
         else if( isdigit(ch) )                 // Another mantissa digit
         {
            fl = (10.0 * fl) + (ch - '0');      // then update mantissa and count
            _u32 = 10 * _u32 + (ch - '0');
            digitCnt++;

            if(gotDP)                           // Got DP?
            {
               digitsAfterDP++;                 // then also update DP count.
            }
         }
         else                                   // else not a (Hex) digit or an exponent
         {                                      // so that's the end of the number
            if(gotDP || out->reqFloat)
            {
               out->num.asFloat = finishMantissa(&fl);                // Calculate mantissa from digits, sign and DP.
               out->gotInt = false;
            }
            else
            {
               if( finishInt(out, _u32, isNeg) == false)
               {
                  return NULL;
               }
            }
            return inTxt;                       // return tail on first non-digit.
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
            else                                // else it's a legal addition to the exponent
            {
               exponent = 10 * exponent + (ch - '0');    // Update the value
               digitCnt++;                      // and the count
            }
         }
         else                                   // else not '+', '-' or '0-9'?
         {
            if( digitCnt == 0 )                 // There were no digits?  Was just 'E' followed by no numbers
            {                                   // then it wasn't an exponent; just the mantissa was the number
               if(digitsAfterDP > 0 || out->reqFloat)    // Was a decimal fraction? OR requested float regardless?
               {
                  if( finishFloat(out, fl) == false)
                  {
                     return NULL;
                  }
               }
               else                             // else it's an integer AND did not request float regardless
               {
                  if(finishInt(out, _u32, isNeg) == false)
                  {
                     return NULL;
                  }
               }
               return inTxt-1;                  // So return with mantissa as it was and with tail on the 'E'...
            }                                   // which wasn't an exponent after all.
            else                                // else terminated legal exponent
            {                                   // so scale 'fl' by exponent to give the final float output.
               fl = fl * GetPwr10Float( isNeg ? -exponent : exponent);

               if( finishFloat(out, fl) == false)
               {
                  return NULL;
               }
               else
               {
                  return inTxt;
               }
            }
         }
      }                 // end: reading exponent

      else if(readHex)                          // Reading Hex?
      {
         if( digitCnt > 8 )                     // More than 32bit?
         {                                      // then the Hex number is larger then 32 bit...
            out->gotInt = true;                 // A fail, but at least say it was an unsigned int.
            out->gotUnsigned = true;
            return NULL;                        // Too big for 'out'. Fail!
         }
         else                                   // else, if a digit, add that digit to the total
         {
            if( !isxdigit(ch) )                 // NOT 0-9 A-F?
            {                                   // then this terminates a legal number, whether there were hex digits after 'X'.
               out->num.asU32 = _u32;
               out->gotInt = true;              // A Hex number is an unsigned int.
               out->gotUnsigned = true;
               return inTxt;                    // Return the tail after the last hex digit.
            }
            else                                // else (another) Hex digit
            {                                   // Add it latest total
               _u32 = 16 * _u32 + HexToNibble(ch);
               digitCnt++;                      // and update digit count
            }
         }
      }                 // end: reading Hex

      inTxt++;       // next char
   }                 //  end: while(1)
   return 0;
}

// -------------------------------------- eof ------------------------------------------------


