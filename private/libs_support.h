/* -------------------------------------------------------------------------
|
|               Utility Library - Common inludes
|
|  To enable Utility funcs source code to build as a lib
|
----------------------------------------------------------------------------- */

#ifndef LIBS_SUPPORT_H
#define LIBS_SUPPORT_H

#include "GenericTypeDefs.h"		// Redirect to typedefs
#include <stdlib.h>

// ======= Bits and stuff, which were in Tiny1's 'common.h'

/* To manipulate a bit at a given (bit) position */
#define _BitM(bit) (1 << (bit))
#define _notB(bit)  0

/*   To manipulate bits with a bit mask.  */
#define SETB(reg,mask) {(reg) |= (mask);}
#define CLRB(reg,mask) {(reg) &= ~(mask);}

#define BSET(reg,mask) ((reg) & (mask))
#define BCLR(reg,mask) (!((reg) & (mask)))

#define ALL_SET(v,m) (((v)&(m))==(m))
#define ANY_SET(v,m) (((v)&(m))!=0)
#define NOT_SET(v,m) (((v)&(m))==0)

#define CLIP(n, min, max) (((n)<(min))?(min):(((n)>(max))?(max):(n)))


// ================== Add/remap 8051 memory regions ============================

#define RAM_IS
#define rIDATA
#define CONST_FP const
#define GENERIC
#define CODE const
#define DATA
#define IDATA

#define BOOLEAN BOOL

// ---- Text Line Builder. Fixed for HSB, for now
#define _SystemTick_msec (10)
#define _TxtBld_DisplayLineLength 20
#define _TxtBld_NumLangauges 4

// ========================== For text line builder =============================

// Fixed at 4 languages for now... for HSB.
#define LANGUAGE_COUNT 4

// Fixed at 20chars for now, for HSB.
#define _TextBld_MaxDisplayLineBytes 21

// A line with a phrase in each of the supported languages.
typedef struct { C8 const *strs[LANGUAGE_COUNT]; } textBld_S_LangStrs;

#define TOOL_GCC            1
#define _TOOL_IS TOOL_GCC

#define bool BOOL
#define false FALSE
#define true TRUE
#include <stddef.h>
#include <stdio.h>

// Compile-time assert.
#define CAT(x,y)  x##y
#define XCAT(x,y)  CAT(x,y)
#define CASSERT(condition) enum { XCAT(_compile_assert_var_at_line_,  __LINE__) = 1/(condition) };

// Export some normally private functions for Unity TDD.
#ifdef UNITY_TDD
   #define _EXPORT_FOR_TEST PUBLIC
#else
   #define _EXPORT_FOR_TEST PRIVATE
#endif

#ifdef _COMPILER_IS_TI_MSP430
    #define __ORDER_LITTLE_ENDIAN__ 1
    #define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#endif

#define _TARGET_X86_CONSOLE        1
#define _TARGET_UNITY_TDD          2
#define _TARGET_X86_LIB            3
#define _TARGET_LIB_ARM_GCC        4
#define _TARGET_LIB_430_CCS_SMALL  5
#define _TARGET_LIB_AVR32          6

#ifdef __TARGET_IS_X86_LIB
   #define _TARGET_IS _TARGET_X86_LIB
#else
   #ifdef __TARGET_IS_CONSOLE
      #define _TARGET_IS _TARGET_X86_CONSOLE
   #else
      #ifdef __TARGET_IS_UNITY_TDD
         #define _TARGET_IS _TARGET_UNITY_TDD
      #else
         #ifdef __TARGET_IS_LIB_GCC_ARM
            #define _TARGET_IS _TARGET_LIB_ARM_GCC
         #else
            #ifdef _TARGET_IS_LIB_430_CCS_SMALL_SMALL
               #define _TARGET_IS _TARGET_LIB_430_CCS_SMALL
            #else
               #ifdef _TARGET_IS_LIB_AVR32
                  #define _TARGET_IS _TARGET_LIB_AVR32
               #else
                  #error "_TARGET_IS must be defined"
               #endif
            #endif
         #endif
      #endif
   #endif
#endif

#if _TARGET_IS == _TARGET_LIB_AVR32
   #ifndef __BYTE_ORDER__
      #define __ORDER_BIG_ENDIAN__ 1
      #define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
   #endif
#endif

#endif // LIBS_SUPPORT_H

// --------------------------------- eof ---------------------------------------
