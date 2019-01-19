/*---------------------------------------------------------------------------
|
|               Builds and formats text lines.
|                   For small displays
|
|--------------------------------------------------------------------------*/

#ifndef TEXT_LINE_BUILDER_H
#define TEXT_LINE_BUILDER_H

#include "libs_support.h"
#include <string.h>
#include <stdio.h>
#include "systime.h"
#include "arith.h"

// Lines of a phrase in each of the languages. 'textBld_S_LangStrs' is defined in 'libs_support.h'
// because the number of langauges is baked in.
typedef struct {
    textBld_S_LangStrs const *lines;    
    U8 numLines;
} textBld_S_TranslationTbl;

// An empty entry in a translation table
#define _TextBld_NoTranslation (C8 const *)(0)

typedef U8  T_LCDLineCnt;           // LCD line has 20 chars.
typedef U16 textBld_T_StrFormat;    // Format flags
typedef U8  textBld_T_Language;     // Indexes a laguage. Current 4 off; English = 0

typedef union
{                                                   // This may hold....
    C8 const                           *asStr;      // ... a text string OR
    C8 const * const                   *asStrArray; // ...translations e.g  {"Yes","Si","Oui","Sim"} OR
    struct textBld_S_StrPlusFormat_Tag *asLink;     // ...another one of these.
} textBld_U_StrORLInk;


typedef struct __attribute__((packed))
  textBld_S_StrPlusFormat_Tag {
    textBld_U_StrORLInk                str;         // Text(s) or a link (see above)
    struct textBld_S_StrPlusFormat_Tag *link;       // May add another string/format.
    textBld_T_StrFormat                format;      // the formatter; blinks, centred etc.
    T_LCDLineCnt                       ofs;         // x-offset. used for scrolling or positioning.
} textBld_S_StrPlusFormat;




PUBLIC textBld_S_StrPlusFormat * textBld_CenterStr(C8 const *str);
PUBLIC textBld_S_StrPlusFormat * textBld_LeftStr(C8 const *str);
PUBLIC textBld_S_StrPlusFormat * textBld_RightStr(C8 const *str);
// To lift a string into the formatter is simply to give it the default format, left-justified.
#define textBld_Lift textBld_LeftStr

PUBLIC textBld_S_StrPlusFormat * textBld_LiftTranslation(C8 const * const *strArray);


PUBLIC textBld_S_StrPlusFormat * textBld_ToLeft(textBld_S_StrPlusFormat * s);
PUBLIC textBld_S_StrPlusFormat * textBld_ToRight(textBld_S_StrPlusFormat * s);
PUBLIC textBld_S_StrPlusFormat * textBld_ToCenter(textBld_S_StrPlusFormat * s);
PUBLIC textBld_S_StrPlusFormat * textBld_Overlay(textBld_S_StrPlusFormat * base, textBld_S_StrPlusFormat * top);
PUBLIC textBld_S_StrPlusFormat * textBld_Join(textBld_S_StrPlusFormat * base, textBld_S_StrPlusFormat * toAppend);
PUBLIC textBld_S_StrPlusFormat * textBld_JoinSpaced(textBld_S_StrPlusFormat * base, textBld_S_StrPlusFormat * toAppend);
PUBLIC textBld_S_StrPlusFormat * textBld_Blinks(textBld_S_StrPlusFormat * s);
PUBLIC textBld_S_StrPlusFormat * textBld_Scrolls(textBld_S_StrPlusFormat * s);
PUBLIC textBld_S_StrPlusFormat * textBld_ScrollToFit(textBld_S_StrPlusFormat * s);
PUBLIC textBld_S_StrPlusFormat * textBld_Alternates(textBld_S_StrPlusFormat * a, textBld_S_StrPlusFormat * b);
PUBLIC textBld_S_StrPlusFormat * textBld_Alternate3(textBld_S_StrPlusFormat * a, textBld_S_StrPlusFormat * b, textBld_S_StrPlusFormat * c);

typedef struct
{
    textBld_T_Language numLanguages, dfltLanguage;
    U8 displayLinelength;
} TxtBld_S_Cfg;

PUBLIC void         textBld_Init(TxtBld_S_Cfg const * cfg);
PUBLIC C8 const *   textBld_MakeLine(textBld_S_TranslationTbl const *tbl, textBld_S_StrPlusFormat const *s);
PUBLIC void         textBld_Run(BOOL syncTimers);
PUBLIC void         textBld_Sync(void);
PUBLIC void         textBld_Setlanguage(textBld_T_Language lang);


#define _TextBld_Run_SyncTimers TRUE
#define _TextBld_Run_NoSync     FALSE

// #def this if there are conflicts with any of the abridged formatter names.
    #ifndef TEXTBLD_USE_FULL_FORMATTER_NAMES
// Lifts
#define _lift           textBld_Lift
#define _liftLangs      textBld_LiftTranslation      // Translations are included with (English) text.
#define _leftL          textBld_LeftStr
#define _rightL         textBld_RightStr
#define _centerL        textBld_CenterStr

// With translations embedded.
#define _leftLlang      textBld_LiftTranslation
#define _rightLlang(l)  _right(_liftLangs(l))
#define _centerLlang(l) _center(_liftLangs(l))

// Applicators
#define _left           textBld_ToLeft
#define _right          textBld_ToRight
#define _center         textBld_ToCenter
#define _blinks         textBld_Blinks
#define _scrolls        textBld_Scrolls
#define _scrollToFit    textBld_ScrollToFit

// Combinators
#define _join           textBld_Join
#define _joinSpaced     textBld_JoinSpaced
#define _overlays       textBld_Overlay
#define _alternates     textBld_Alternates
#define _alternate3     textBld_Alternate3
    #endif

#endif // TEXT_LINE_BUILDER_H

// ------------------------------ eof -----------------------------------------