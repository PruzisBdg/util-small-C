/* -------------------------------------------------------------------------
|
|               Utility Library - Common inludes
|
|  To enable Utility funcs source code to build as a lib
|
----------------------------------------------------------------------------- */

#ifndef LIBS_SUPPORT_H
#define LIBS_SUPPORT_H

#include "GenericTypeDefs.h"		// Redirect to Generac's typedefs

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

// ================== Add/remap pre-Generac stuff ============================

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

#define TOOL_GCC 1
#define _TOOL_IS TOOL_GCC

#define bool BOOL
#define false FALSE
#define true TRUE
#include <stddef.h>
#include <stdio.h>

#endif // LIBS_SUPPORT_H

// --------------------------------- eof ---------------------------------------
