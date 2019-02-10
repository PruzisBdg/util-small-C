/*---------------------------------------------------------------------------
|
|  Small-MCU, small RAM printf(), sprintf().
|
|  Minimal-RAM.  Writes as it parses. No buffers, no heap.
|
|  Builds with or without floating point. Efficient FP math.
|
|  Why this printf()?
|  ------------------
|
|  If your tool vendor has made a good hand crafted printf() then all is good. But if they've
|  just ported the gnu stdout to a small MCU then you will have a horror show at some point.
|  There are different implementations out there, but they are all RAM and stack hogs. Some
|  do their FP formatting in literal and inefficient teaching code style.
|
|  You would think there's a nice small-MCU printf() online somewhere. Think again!
|
|  This printf() isn't fully compliant, but it does all the MCU users actually need.
|
|  To use these, your application must supply a 'putchar()' and 'putBuf()' (named below) to be
|  linked into this lib. This gets round another issue with some MCU print libraries; that they
|  are hard wired to print to UART0. Fine if that's what you need, otherwise you are out of luck.
|
|     for printf().... extern void TPrint_PutCh(U8 ch);
|
|     for sprintf().... extern C8 *TPrint_BufPtr;
|                       extern void TPrint_ChIntoBuf(U8 ch);
|
|--------------------------------------------------------------------------*/

	#ifdef _UNIT_TEST
#include "tiny1_printf_unit_test.h"
extern float GetPwr10Float(S16 exp);
extern S32 ClipFloatToLong(float f);
	#else
#include "GenericTypeDefs.h"
#include "libs_support.h"
#include <math.h>
#include <ctype.h>
#include "arith.h"      // GetPwr10Float()
#include "tiny1_stdio.h"
#include <stdarg.h>
	#endif

PRIVATE U8  width;
PRIVATE U8  prec = 0;
PRIVATE BIT wrNeg = 0;           // If number is negative -> prepend '-'.
PRIVATE BIT wrPlus = 0;          // If '+' format specifier, '+' in front of positive number
PRIVATE T_PrintCnt printCnt;
PRIVATE BIT quoteCtrls = 0;		// If '1' the e.g '\t' will be quoted (as "\t")

#ifndef _TPRINT_IS
   #error "_TPRINT_IS must be TPRINT_TINY TPRINT_FLOAT or TPRINT_NO_FLOAT"
#endif

   #if _TPRINT_IS != TPRINT_TINY
PRIVATE U8 fieldCnt;             // Chars printed in field so far; for adding correct number of trailing spaces (if any)
   #endif

// Some file-global flags
PRIVATE BIT wrZero;              // When set, write a '0' for 0 when parsing a number
PRIVATE BIT isSigned;            // The current 16 bit number is signed

PRIVATE void (*putChPtr)(U8);    // Pointer to the user-supplied putchar()


#ifndef _TOOL_IS
   #error "Must define _TOOL_IS"
#else
   /* These #defs encode how data types are converted when passed as optional parms (...). The
      conversion is target dependent.
   */
   #if _TOOL_IS == TOOL_GCC || _TOOL_IS == _TOOL_GCC_ARM
      #define VA_ARG_S16      int
      #define VA_ARG_U16      unsigned int
      #define VA_ARG_CHAR     int
      #define VA_ARG_FLOAT    double
   #else                               // Raisonance C8051 and TI MPS340 Code Composer
      #define VA_ARG_S16      S16      // 16bit types remain 16bit.
      #define VA_ARG_U16      U16
      #define VA_ARG_CHAR     U16      // 8 bit promoted to 16.
      #define VA_ARG_FLOAT    float    // Default float is single precision.
   #endif
#endif

/*-----------------------------------------------------------------------------------
|
|  nibbleToASCII()
|
|  Convert 0x00 -> 0x0F to '0' -> 'F'; anything else becomes '\0'.
|
--------------------------------------------------------------------------------------*/

PRIVATE U8 nibbleToASCII(U8 n)
{
   return (U8)((n > 0x0F) ? '\0' : ((n > 9) ? n - 10 + 'A' : n + '0'));
}

/*-----------------------------------------------------------------------------------
|
|  putCh()
|
|  Write a char with the externally supplied putChPtr(). Also update the number of
|  chars written so far in this field.
|
|  If formatter is "%S" then quoteCtrls <- 1. Quote whitespace and non-printable chars.
|
--------------------------------------------------------------------------------------*/

PRIVATE void putCh(U8 ch)
{
   if(!isprint(ch) && quoteCtrls == 1)                             // Non-printable? AND we are quoted non-printables
   {
      putChPtr('\\');                                              // Backslash
      if(ch == '\t' || ch == '\r' || ch == '\n')                   // '\t', '\r', '\n'? ...
	  {
		  putChPtr(ch == '\t' ? 't' : (ch == '\r' ? 'r' : 'n'));   // ...are quoted as written
		  printCnt += 2;                                           // e.g '\r' -> 2 chars
	  }
	  else                                                         // any other whitespace...
	  {
         putChPtr('x');                                            // ...is quoted as hex.
         putChPtr(nibbleToASCII(HIGH_NIBBLE(ch)));
         putChPtr(nibbleToASCII(LOW_NIBBLE(ch)));
         printCnt += 4;                                            // e.g '\x34' -> 4 chars
	  }
   }
   else                                                            // else is printable? OR we are not quoting anything?
   {
      putChPtr(ch);                                                // so print 'ch' as-is.
      printCnt++;
   }
      #if _TPRINT_IS != TPRINT_TINY
   fieldCnt++;
      #endif
}


/*-----------------------------------------------------------------------------------
|
|  wrHexASCII
|
|  Write byte 'n' as uppercase Hex ASCII
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrHexASCII(U8 n)
{
   putCh(nibbleToASCII(HIGH_NIBBLE(n)));
   putCh(nibbleToASCII(LOW_NIBBLE(n)));
}


/*-----------------------------------------------------------------------------------
|
|  wrHex16
|
|  Write word 'n' as uppercase Hex ASCII.
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrHex16(U16 n)
{
   wrHexASCII(HIGH_BYTE(n));
   wrHexASCII(LOW_BYTE(n));
}

/*-----------------------------------------------------------------------------------
|
|  wrHex16
|
|  Write word 'n' as uppercase Hex ASCII. If upper byte is zero then write just the low byte.
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrHex16Short(U16 n)
{
   if( HIGH_BYTE(n) )
      { wrHexASCII(HIGH_BYTE(n)); }
   wrHexASCII(LOW_BYTE(n));
}


/*-----------------------------------------------------------------------------------
|
|  wrHex32
|
|  Write long word 'n' as uppercase Hex ASCII.
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrHex32(U32 n)
{
   wrHex16(HIGH_WORD(n));
   wrHex16(LOW_WORD(n));
}

/*-----------------------------------------------------------------------------------
|
|  putSpc()
|
--------------------------------------------------------------------------------------*/

PRIVATE void putSpc(void)
{
   putCh(' ');
}


/*-----------------------------------------------------------------------------------
|
|  wrDigit()
|
|  Write a single digit, and maybe a leading zero.
|
--------------------------------------------------------------------------------------*/

PRIVATE BIT wrDigit(U8 n)
{
   if(n || wrZero)         // Print (leading) zero? OR digit is non-zero
   {
      if( wrNeg )          // Needs leading '-'? (because number was negative)
      {
         putCh('-');       // then write it.
         wrNeg = 0;        // and clear flag, meaning we prepend to just leading digit.
         wrPlus = 0;       // Also clear the '+' specify, as we are done with the leading digit.
      }
      else if(wrPlus)      // else, if a positive number, needs leading '+'?
      {
         putCh('+');       // then write '+'
         wrPlus = 0;       // and clear flag, meaning we prepend to just leading digit.
      }
      putCh((U8)(n + '0'));// then print the digit
      wrZero = 1;          // After this, will print any in-number zeroes.
      return 1;            // we printed a digit, return TRUE.
   }
   else                    // else not printing.
   {
      return 0;            // no print, so return FALSE;
   }
}

/*-----------------------------------------------------------------------------------
|
|  chkPadZero()
|
|  If the 'pwr'th digit is less than the number of significant digits to be printed
|  ('prec') then set 'wrZero' left-pad with zeros. Also, if there's a leading
|  '+' or '-' then one less leading zero to print.
|
--------------------------------------------------------------------------------------*/

PRIVATE void chkPadZero(U8 pwr)
{
   if(prec >                              // Significant digits GT?...
      ((wrPlus == 1 || wrNeg == 1)        // if to prepend "+' or '-"?...
         ? pwr+1                          //    ... GT pwr'th + 1
         : pwr))                          //    ... else GT pwr'th
      { wrZero = 1; }
}

/*-----------------------------------------------------------------------------------
|
|  getPwr10_U16
|
|  Return 10^n  where n = [0..4]
|
--------------------------------------------------------------------------------------*/

PRIVATE U16 CONST powersOf10[] = {1,10,100,1000,10000};

PRIVATE U16 getPwr10_U16(U8 pwr) { return powersOf10[pwr]; }


/*-----------------------------------------------------------------------------------
|
|  wrU16Rem
|
|  Print the 1's, 10's or 100's etc of 'n', depending on 'pwr'. Return the remainder
|  after the printed portion has been subtracted.
|
|  E.g if 'n' = 4721 (decimal) and 'pwr' = 3 (1000's), then print '4' and return 721.
|
|  Successive calls to this print a number as a decimal
|
--------------------------------------------------------------------------------------*/

PRIVATE U16 wrU16Rem(U16 n, U8 pwr)
{
   if(n == 0 && pwr == 0)                    // Last digit and it's zero?
   {
      putCh('0');                            // then print '0'.
   }
   else                                      // else may print digit, depending on value and width-specifier.
   {
      U8 ms;                                 // (Presumed) most significant decimal digit

      ms = n / getPwr10_U16(pwr);            // Get msd

      chkPadZero(pwr);                       // Check if should left-pad with zero

      if( !wrDigit(ms) )                     // Did not write digit?
      {
         if(!wrZero && width > pwr)          // then need more spaces to justify?
         {
            putSpc();                        // then write one.
         }
      }
      return n - (ms * getPwr10_U16(pwr));   // Return the remainder
   }
}

/*-----------------------------------------------------------------------------------
|
|  printLeftSpaces
|
|  If the field width for an integer (16 or 32bit) is more than the max number of digits
|  which will be printed (including '-'), then print the spaces needed to right justify
|  the printed number. 'maxDigits' is 5 for a 16 bit int, 10 for a long (32bit).
|
--------------------------------------------------------------------------------------*/

PRIVATE void printLeftSpaces(U8 maxDigits)
{
  /* If field width is greater than maximum U32 decimal digits, then print all the leading
      spaces which will be there regardless of the value of 'n'.
   */
   if( (wrNeg || wrPlus) && width ) // Will prepend '-" or '+' to the digits?
   {
      width--;                      // then one space less to print
   }
   while(width > maxDigits )        // (While) field width > max decimal digits?
   {
      putSpc();                     // write a space
      width--;                      // and 1 less to write.
   }
 }

/*-----------------------------------------------------------------------------------
|
|  wrU16
|
|  Print unsigned 16 bit decimal
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrU16(U16 n)
{
   wrZero = 0;                                                             // No leading zeros
   printLeftSpaces(RECORDS_IN(powersOf10));                                // Print spaces needed to right-justify the number.
   wrU16Rem(wrU16Rem(wrU16Rem(wrU16Rem( wrU16Rem(n ,4), 3), 2), 1), 0);    // Print 10,1000's, 1000's 100's .....
}


/*-----------------------------------------------------------------------------------
|
|  wrInt16
|
|  Print 'n' as decimal, signed or unsigned.
|
|  Note that 'n' can be a U16; it won't be truncated
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrInt16(S16 n)
{
   if(n < 0 && isSigned)      // Format was '%d' or '%i' and 'n' is negative?
   {
      wrNeg = 1;              // Will write '-' before 1st significant digit.
      n = -n;                 // and flip the number positive
   }
   wrU16(n);                  // Print the (positive) number.
}

/*-----------------------------------------------------------------------------------
|
|  getPwr10L
|
|  Return 10^n, where 'n' = [0..9].
|
--------------------------------------------------------------------------------------*/

PRIVATE U32 CONST powersOf10L[] = {1L,10L,100L,1000L,10000L, 100000L, 1000000, 10000000L, 100000000L, 1000000000L};

PRIVATE U32 getPwr10L(U8 pwr) { return powersOf10L[pwr]; }


/*-----------------------------------------------------------------------------------
|
|  wrU32Rem
|
|  Print the 100's, 10's, 1's etc of 'n', depending on 'pwr'. Return the remainder
|  after the printed portion has been subtracted.
|
|  E.g if 'n' = 4721 (decimal) and 'pwr' = 3 (1000's), then print '4' and return 721.
|
|  This is the 32bit version of 'wrU16Rem()'.
|
|  N.B This is slow code, 32 bit divison. However it does deliver the digits
|  in printable order, highest 1st.
|
--------------------------------------------------------------------------------------*/

PRIVATE U32 wrU32Rem(U32 n, U8 pwr)
{
   if(n == 0 && pwr == 0)              // Last digit and it's zero?
   {
      putCh('0');                      // then print '0'.
   }
   else                                // else may print digit, depending on value and width-specifier.
   {
      U8 q = (U8)(n/getPwr10L(pwr));   // Get millions, 1000's, hundreds, etc, by division

      chkPadZero(pwr);                 // Check if should left-pad with zero (if current digit is zero)
      wrDigit(q);
      return n - (q*getPwr10L(pwr));   // Return the remainder.
   }
}

/*-----------------------------------------------------------------------------------
|
|  wrU32
|
|  Print unsigned decimal
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrU32(U32 n)
{
   U8 pwr;

   wrZero = 0;                                        // Clear 'leading zero' flag
   printLeftSpaces(RECORDS_IN(powersOf10L));          // If field width > largest U32 then print all the spaces we know we'll need, whatever the number

   /* First, find the most significant decimal digit to be printed. This will be either the first sig.
      digit of the number OR, if the precision specifier ('prec') is higher, a leading zero (e.g '0032').

      Also, if fied width is greater than significant digits, inserts spaces between the field width
      and the 1st sig. digit.

      For small numbers this is faster than eating through the number with successive calls to wrU32Rem(),
      each of which involves 32bit division.
   */
   for(pwr = RECORDS_IN(powersOf10L)-1; pwr; pwr-- )  // From 10,000,000, 1,000,000 downwards
   {
      if( n >= getPwr10L(pwr) || prec > pwr )          // n > 10^pwr (n > 100000, n > 10000 etc) ? OR precision > pwr?
      {
         break;                                       // then break to start printing digits
      }
      else if(width > pwr)                            // Need (another) leading space?
      {
         putSpc();                                    // then print spc and continue.
      }
   }
   pwr++;                                             // Loop exits with e.g pwr = 0 for 1 sig. digit. Bump++.

   do                                                 // For up to 10 digits ( 0xFFFFFFFF = 4E9 )
   {
      pwr--;                                          // Pre-decrement, so range is 10 -> 0
      n = wrU32Rem(n,pwr);                            // Print from high 10-powers downwards
   }
   while(pwr);                                        // while there are digits to print.
}


/*-----------------------------------------------------------------------------------
|
|  wrInt32
|
|  Print 'n' as (long) decimal, signed or unsigned.
|
|  Note that 'n' can be a U32; it won't be truncated
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrInt32(S32 n)
{
   if(n < 0 && isSigned)      // If format was '%d' or '%i' and it's negative?
   {
      wrNeg = 1;              // Will print '-'
      n = -n;                 // invert the number
   }
   wrU32(n);                  // Print (positive) 'n' with this.
}


// ====================== Floating-Point Support ==================================
// ====================== Floating-Point Support ==================================
// ====================== Floating-Point Support ==================================





#if _TPRINT_IS == TPRINT_FLOAT

/*-----------------------------------------------------------------------------------
|
|  wrS16
|
|  Print signed 'n' as decimal
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrS16(S16 n)
{
   isSigned = 1;
   wrInt16(n);
}

/*-----------------------------------------------------------------------------------
|
|  wrS32_Decpt
|
|  Print unsigned decimal, with a decimal point inserted 'dp' from right. Also with
|  leading and trailing zeros removed, and zeros padded to right of decimal, if necessary.
|
|     (12345, 2)  ->  123.45
|     (99, 4)     ->  0.0099
|     (9900, 4)   ->  0.99
|     (007934, 2) ->  79.34
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrS32_Decpt(S32 n, U8 dp)
{
   U8 c, pwr;

   if(n == 0)                          // Is zero?
   {
      putCh('0');                      // then print '0'
      if(dp)                           // Has DP?
      {
         putCh('.');                   // then print DP
         while(dp--)
            { putCh('0'); }            // and the required number of trailing '0's.
      }
   }
   else                                // else it's non-zero so..
   {
      if( n < 0 )                      // Negative?
      {
         putCh('-');                   // then print minus,
         n = -n;                       // and invert the number
      }

      /* First, find the most significant decimal digit by comparision. For small
         numbers this is faster than eating through the number with successive calls
         to wrU32Rem(), each of which involves 32bit division.

         Note 2^32 = 4.29E9, so start at 10^9 and work down
      */
      for(pwr = 9; pwr; pwr-- )        // From left to right
      {
         if( n >= getPwr10L(pwr) )     // n >= 100000, n >= 10000, n >= 1000 etc?
            { break; }
      }

      /* If 'dp' position exceeds number of significant digits then we must prepend
       * '0.' and maybe pad zeros too.
      */
      if( dp > pwr )
      {
         putCh('0');
         putCh('.');
         c = dp;
         while(--c > pwr ) { putCh('0'); }
      }

      // Finally, print out the actual digits. Insert a dp if necessary.

      pwr++;                           // Will predecrement, so backup one.

      do                               // For up to 10 digits ( 0xFFFFFFFF = 4E9 )
      {
         pwr--;                        // Pre-decrement, so range is 10 -> 0

         n = wrU32Rem(n,pwr);          // Print from high 10-powers downwards

         if( pwr == dp && pwr > 0 )    // Insert decimal point here?, if there are no trailing digits
            { putCh('.'); }
      }
      while(pwr);
   }
}

// i.e  2^31 = 2.4E9
#define _MaxS32Digits 9

/*-----------------------------------------------------------------------------------
|
|  getExponent10
|
|  Get the nearest power of 10 for 'f', from the binary exponent mantissa x 2^exp,
|  where 'mantissa' is [0.5.. 1.0].
|
|  Note: Use single-precision (float) functions to avoid defaulting to double
|  (which is overkill for a diagnostic embedded printf).
|
--------------------------------------------------------------------------------------*/

PRIVATE S16 getExponent10(float f)
{
   int  exp2N;    // exponent as 2^N

   // Hack the float, returns the exponent.
   frexpf(f, &exp2N);

   // Get the corresponding power of 10, rounded down  ( 10 = 2^3.3219.... )
   return (S16)floorf( exp2N/3.32192809489f);
}



/*-----------------------------------------------------------------------------------
|
|  wrFloatExp
|
|  Write a floating point number in exponential format 'prec' digits after the
|  decimal point, but no more than 9 digits total.
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrFloatExp(float f, U8 _prec)
{
   S16  exp10;    // exponent as 10^N

   // First, how big is this number? Get the power of 10 corresponding to the binary
   // exponent of 'f', rounded down.
   exp10 = getExponent10(f);

   /* Now, divide the original number by 10^exp10. This normalises it into [1..10].
      Multiply by 10^(prec-1). This gets us a number with 'prec' digits to the left
      of the decimal point. Convert this to a long int and print it, inserting a
      decimal point after the 1st significant digit. This gives us e.g '4.297'.
   */
   _prec = MinU8(_prec, _MaxS32Digits);
   wrZero = 0;                                           // Suppress leading zeros
   wrS32_Decpt(f / GetPwr10Float(exp10-_prec), _prec);

   // Print the exponent e.g 'E12'
   putCh('E');
   width = 0;           // Exponent digits are right after 'E', so no justify
   wrS16(exp10);
}

/*-----------------------------------------------------------------------------------
|
|  wrFloatFixed
|
|  Write a floating point number in exponential format with 'prec' digits after
|  the decimal point. Print up to 9 digits; if more are needed then print in
|  exponential format.
|
--------------------------------------------------------------------------------------*/

PRIVATE void wrFloatFixed(float f, U8 _prec)
{
   S16  exp10;    // exponent as 10^N

   // First, how big is this number? Get the power of 10 corresponding to the binary
   // exponent of 'f', rounded down.
   exp10 = getExponent10(f);

//printf("*** %4.2f %d %d %4.1f\r\n",f, prec, exp10, GetPwr10Float(prec));

   /* If the won't fit within 9 digits in fixed format, then write it with an
      exponent.
   */
   if( (_prec + exp10) > 9 )
   {
      wrFloatExp(f, _prec);
   }
   else     // else write in fixed format
   {
      // If 'f' < 0 then will need leading zeros to right of decimal point.
      if( exp10 < 0 ) { wrZero = 1; } else { wrZero = 0; }

      /* Now, decimal shift 'f' left by the number of trailing digits. Then read and
         print it as an integer, inserting a decimal point before the trailing digits.
      */
      wrS32_Decpt(ClipFloatToLong(f * GetPwr10Float(_prec)), _prec);
   }
}

#endif // TPRINT_FLOAT == 1






// ====================== end: Floating-Point Support ==================================
// ====================== end: Floating-Point Support ==================================
// ====================== end: Floating-Point Support ==================================


/* If using tiny1_print_heap1w(), this counts the
*/
	#ifdef TPRINT_USE_STRING_HEAP
extern void TPrint_ReturnToHeap(U8 const *from, U16 numBytes);
	#endif

/*-----------------------------------------------------------------------------------
|
|  tprintf
|
|  The printout routine, called by printf(), sprintf()
|
--------------------------------------------------------------------------------------*/

PUBLIC T_PrintCnt tprintf_internal(void (*putChParm)(U8), C8 CONST *fmt, va_list arg)
{
   BIT gotPcent = 0;
   BIT gotEsc = 0;
   BIT gotWidth = 0;
   BIT gotLong = 0;
   BIT zeroPad = 0;
   U8 ch;
   U8 GENERIC *p;

      #if _TPRINT_IS == TPRINT_TINY
   U8 idx = 0;
      #else
   S16 idx = 0;
      #endif

      #if _TPRINT_IS != TPRINT_TINY
   BIT gotPrec = 0;
      #endif

   putChPtr = putChParm;                                    // Make putCh() available everwhere in this file.
   printCnt = 0;											// Init emit counter

   while((ch = (U8)fmt[idx]) != '\0')                       // Until the end of the format string
   {
      quoteCtrls = 0;                                       // Only "%S" enables control-quotes; anything else defeats it.

      if(gotPcent)                                          // Parsing a '%'?
      {
         isSigned = 0;                                      // Unless made 1 below

         // Parse the optional modifiers; remember each one you get
         if(ch == '+')
         {
            wrPlus = 1;
         }
         else if( !gotLong && ch == 'l' )                        // 'l' for 32bit fixed point and double floats
         {
            gotLong = 1;
            gotWidth = 1;
         }
         else if(!gotWidth && isdigit(ch))                  //  digit of a width modifier?
         {
            if( width == 0 && ch == '0' )                   // Is 1st digit (of the width modifier)? AND is '0'
            {
               zeroPad = 1;                                 // then doesn't affect width value, but may be to left-pad a %d with zeros. Mark for later.
            }
            else if(isdigit(ch))                            // else it's (a digit) and hence part of the number specifying the width
            {
               width = (U8)(10 * width) + (U8)(ch - '0');   // so update the width from the digit.
            }
         }
            #if _TPRINT_IS != TPRINT_TINY

         else if( !gotPrec && ch == '.' )                   // Precision is the 2nd digit in e.g %5.3
         {                                                  // Not optional if there's been a '.'
            gotWidth = 1;
            gotPrec = 1;

            idx++;                                          // then advance to digit
            if( !isdigit(ch = (U8)fmt[idx]) )               // Wasn't a digit?
               { break; }                                   // then format error, we're done
            else
               { prec = (U8)(ch - '0'); }                   // else precision is this
         }
            #endif

         // else no more modifiers, decode and print the type

         else
         {
            gotWidth = 1;

               #if _TPRINT_IS != TPRINT_TINY
            fieldCnt = 0;
               #endif

            switch(ch)                                         // What follows the '%'?
            {
               case 'd':                                       // Signed integer?
               case 'i':                                       // equivalent to "%d"
                  isSigned = 1;                                // then set flag to force signed printout

               case 'u':                                       // Unsigned int?
                  if( zeroPad && width > prec )                // Got e.g '%04d'?, i.e left-pad with zeros to make at least 4 digits
                  {
                     prec = width;                             // then the width '4' was actually a precision specifier...
                  }                                            // ... Sigh. Who cooked up these formatters?!
                  if( gotLong )                                // Signed or unsigned, print it thru here
                     { wrInt32((S32)va_arg(arg, S32)); }
                  else
                     { wrInt16((S16)va_arg(arg, VA_ARG_S16)); }
                  break;

               case 'c':                                       // Character?
                  putCh((U8)va_arg(arg, VA_ARG_CHAR));
                  break;

               case 'S':
			      quoteCtrls = 1;                              // Non-printables will be quoted.
               case 's':                                       // String?
               {
                  p = va_arg(arg, U8 GENERIC *);               // Get ptr to this string

                     #ifdef TPRINT_USE_STRING_HEAP
                  /* If (maybe) printing to string heap, will count how many string chars printed and offer that
                     count to the string heap. If this string was from the heap (it may be e.g a constant string, which
                     is not), then release that heap space, the string has been consumed.
				  */
                  U16 printsWas = printCnt;                    // Mark chars printed so far.
				  U8 GENERIC const *strAt = p;
                     #endif

                  while( (ch = *(p++)) != '\0')                // Until end of string...
                     { putCh(ch); }							   // print chars and count them.

                     #ifdef TPRINT_USE_STRING_HEAP
                  /* Offer to the heap the number of chars emitted, plus 1 for '\0'. Note that putCh() may emit more than one
                     printable per char given to it (when printing unicode or non-printables); 'printCnt' counts actual chars
					 emitted.
                  */
                  TPrint_ReturnToHeap(strAt, printCnt + 1 - printsWas);
                     #endif
                  break;
               }
               case 'x':                                       // Small 'x'. Print a single byte if no 'l' AND small number.
                  if(!gotLong) {                               // No 'l' modifier?
                     wrHex16Short((U16)va_arg(arg, VA_ARG_U16)); // then print 16bit number, omitting high byte if it is zero.
                     break;
                     }                                         // else print 32 bit hex (below)

               case 'X':                                       // always uppercase '0x2ABD'
                  if( gotLong)                                 // Got 'l' modifier?
                     { wrHex32((U32)va_arg(arg, U32)); }       // then print 32bit unsigned as hex
                  else                                         // else no 'l' modifier
                     { wrHex16((U16)va_arg(arg, VA_ARG_U16)); }// so print 16 unsigned as hex.
                  break;

                  /* Note that the Hex formats are not ANSI standard, where 'X' means an uppercase
                     and 'x' means lowercase. But printing just a byte is something we want quite
                     often so bend the rules.
                  */

                     // ------- Floating point support
                     #if _TPRINT_IS == TPRINT_FLOAT

               case 'e':
               case 'E':
                  if(!gotPrec) { prec = 4; }                   // If don't get precision for float, this is default.
                  wrFloatExp((float)va_arg(arg, VA_ARG_FLOAT), prec); // Write E-float
                  break;

               case 'f':
               case 'F':
                  if(!gotPrec) { prec = 4; }                   // If don't get precision for float, this is default.
                  wrFloatFixed((float)va_arg(arg, VA_ARG_FLOAT), prec); // Write fixed-float
                  break;

                     #endif
                     // -------- end: Floating point support

               case '%':                                       // Escaped '%'?
                  putCh('%');                                  // then print '%'
                  goto skip;
            }

               #if _TPRINT_IS != TPRINT_TINY
            while(fieldCnt < width)                         // Field must be filled out?
               {  putCh(' ');  }                            // then insert spaces (Each putCh increments 'fieldCnt')
               #endif
skip:
            gotPcent = 0;                                   // Done with formatter, clear for next
         }
      }
      else if(gotEsc)                                       // Got '\'
      {
         switch(ch)                                         // What's next char?
         {
            case 'a': putCh(0x07); break;                   // Beep
            case 'b': putCh(0x08); break;                   // Backspace
            case 'f': putCh(0x0C); break;                   // Formfeed
            case 't': putCh(0x09); break;                   // Tab
            case 'r': putCh(0x0D); break;                   // CR
            case 'n': putCh(0x0A); break;                   // LF
            case 'v': putCh(0x0B); break;                   // Vert. tab
            case '\\': putCh('\\'); break;                  // escaped '\' so printf a '\'
            // Not an escape. Print the '\' plus the ch.
            default: putCh('\\'); putCh(ch);
            // Note: Does not handle \xnnnn (hex literal) or \nnn (octal literal)
         }
         gotEsc = 0;                                        // and done with '\'
      }
      else                                                  // else we're neither handling and escape ('\') or a formatter ('%')
      {
         switch(ch)                                         // So what is the next char?
         {
            case '%':                                       // Its a formatter
               gotPcent = 1;
               gotLong = 0;                                 // Setup to decode modifiers.
               gotWidth = 0; width = 0;                     // Unless we get width.
               zeroPad = 0;
               wrNeg = 0;                                   // Unless number is negative.
               wrPlus = 0;                                  // Unless get a '+' format specifier.

                  #if _TPRINT_IS != TPRINT_TINY
               gotPrec = 0; prec = 0;                       // Unless we get precision, this is the default
                  #endif
               break;                                       // ... then will process the formats

            case '\\':                                      // It's an escape AND we are not already escaped.. mark that we are in escape now.
                if(!gotEsc)
                    {gotEsc = 1;}
                break;

            default: putCh(ch);                             // else its printable, do just that
         }
      }
      idx++;                                                // Goto next char in format string
   }
   return printCnt;											 // Return number of chars written.
}

// exported just for tiny1_print_heap1w()
PUBLIC T_PrintCnt tprintf_PutCh(U8 ch)
{
	T_PrintCnt was = printCnt;
	putCh(ch);
	return printCnt - was;
}

/*-----------------------------------------------------------------------------------
|
|  tprintf
|
|  The printout routine, called by printf(), sprintf()
|
--------------------------------------------------------------------------------------*/

// Your application must supply this
extern void TPrint_PutCh(U8 ch);

/*-----------------------------------------------------------------------------------
|
|  printf()
|
--------------------------------------------------------------------------------------*/

PUBLIC T_PrintCnt tiny1_printf(C8 FMT_QUALIFIER *fmt, ...)
{
   T_PrintCnt  cnt;
   va_list     ap;

   va_start(ap, fmt);
   cnt = tprintf_internal(TPrint_PutCh, fmt, ap);
   va_end(ap);
   return cnt;		// Return number of chars pushed to stream.
}



/*-----------------------------------------------------------------------------------
|
|  sprintf(), sprintf_safe()
|
|  The 'safe' version limits the number of chars printed to the output 'buf'
|
--------------------------------------------------------------------------------------*/


// Your application must supply these
extern C8 *TPrint_BufPtr;              // This ptr used internally  by TPrint_ChIntoBuf()
extern void TPrint_ChIntoBuf(U8 ch);   // So can be a plain buffer or a stream.

PUBLIC T_PrintCnt tiny1_sprintf(C8 *buf, C8 FMT_QUALIFIER *fmt, ...)
{
   T_PrintCnt cnt;
   va_list ap;

   va_start(ap, fmt);
   TPrint_BufPtr = buf;
   cnt = tprintf_internal(TPrint_ChIntoBuf, fmt, ap);
   putCh('\0');			// Output must be a string, so add terminating '\0'.
   va_end(ap);
   return cnt;			// Return number of chars in string, excluding '\0';
}

extern tiny1_S_SafeSprintf TPrint_Safe;		// If using tiny1_sprintf_safe(), to limit chars added to output buffer.
extern void TPrint_ChIntoBuf_Safe(U8 ch);			// So can be a plain buffer or a stream.

PUBLIC T_PrintCnt tiny1_sprintf_safe(tiny1_S_SafeBuf const *out, C8 FMT_QUALIFIER *fmt, ...)
{
   T_PrintCnt cnt;
   va_list ap;
   va_start(ap, fmt);

   TPrint_Safe.put = out->buf;			// Put form start of 'buf'
   TPrint_Safe.cnt = 0;					// Counts characters added.
   TPrint_Safe.maxCh = out->maxCh;		// Will add no more than.

   cnt = tprintf_internal(TPrint_ChIntoBuf_Safe, fmt, ap);
   putCh('\0');			// Output must be a string, so add terminating '\0'.
   va_end(ap);
   return cnt;			// Return number of chars in string, excluding '\0';
}


// --------------------------------- eof -------------------------------------------
