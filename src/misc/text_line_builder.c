/*---------------------------------------------------------------------------
|
|               Builds and formats text lines For small displays.
|
|   Text items can be justified, combined/overlaid, blinked or scrolled.
|   A text field can show 2 different strings alternately.
|
|   TextBld_ handles optional translations through string tables. If a transation
|   exists for a given text string AND the current language isn't the default (English)
|   then TextBld will substitute the string, correctly justifed.
|
|   TextBld_ supports optional scrolling. If a string or it's translation is too large
|   for the display window, TextBld_ will scroll it; otherwise no.
|
|   Public:
|       textBld_MakeLine()
|       textBld_Run()
|       textBld_Sync()
|
|       textBld_CenterStr()
|       textBld_LeftStr()
|       textBld_RightStr()
|       textBld_Overlay()
|       textBld_Blinks()
|       textBld_Scrolls()
|       textBld_ScrollToFit()
|       textBld_Alternates()
|       textBld_Alternate3()
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include <string.h>
#include <stdio.h>
#include "systime.h"
#include "arith.h"
#include "text_line_builder.h"

#define _DisplayLineLength (_TextBld_MaxDisplayLineBytes-1)

typedef struct
{
    TxtBld_S_Cfg const *cfg;
    U8              currLanguage;
    BOOL            blinkToggle,
                    showAlternate,
                    marksScroll;
    U8              alt3Div;
    U16             scrollCnt;
    T_LCDLineCnt    scrollSize;
} S_TxtBld;

PRIVATE S_TxtBld txtBld = { NULL, 0, FALSE, 0, 0 };


/* ----------------------- Strings & Translations ---------------------------------- */


#define _TextBld_NoTranslation (C8 const *)(0)
#define _LanguageIdx_English 0


/* ------------------------------ isBlankOrEmpty  ------------------------------------------- */

PRIVATE BOOL isBlankOrEmpty(C8 const *str) {
    U16 c;

    for (c = 0; c < MAX_U16 && str[c] != '\0'; c++) {
        if (str[c] != ' ') {
            return FALSE;
        }
    }
    return TRUE;
}

/* ------------------------------- translateFromTbl --------------------------------------------

   Returns 'englishStr' in the current language. Which may be English, in which
   case unchanged.
 */

PRIVATE C8 const * translateFromTbl(textBld_S_TranslationTbl const *langStrs, C8 const *englishStr) {
    U8 c;
    textBld_S_LangStrs const *ls;

    if (txtBld.currLanguage == 0 ||                 // current language is English? OR
        isBlankOrEmpty(englishStr))                 // empty/blank?
    {
        return englishStr;
    }                                               // then nothing to translate; just return the supplied 'englishStr'
    else                                            // else search for a translation.
    {
        for (c = 0, ls = langStrs->lines; c < langStrs->numLines; ls++, c++) // Search translations table
        {
            if (ls->strs[0] == englishStr)          // 1st field is the same (object) as supplied english
            {                                       // then we have a match....
                return                              // Translate if necessary and possible
                txtBld.currLanguage >= txtBld.cfg->numLanguages                 // Language setting is bogus?
                    ? "Illegal language idx"                                    // then say this everywhere
                    : (ls->strs[txtBld.currLanguage] == _TextBld_NoTranslation          // else, No translation for this language?
                        ? (ls->strs[_LanguageIdx_English] == _TextBld_NoTranslation     // and.. No English (either)?
                            ? "No translation"                                  // then show nothing
                            : ls->strs[_LanguageIdx_English])                   // else show English
                        : ls->strs[txtBld.currLanguage]);                       // else legal index AND there's a translation.. show that!
            }
        }
        // No match in translation table. Again return the English.
        return englishStr;
    }
}

/* -------------------------- Formatters --------------------------------------- */

// Enough to hold all the formatters which may be pending for a screen.
// Say, 4 per line.
#define _FormatHeap_Size 12
typedef U8 T_FormatHeapIdx;

typedef struct __attribute__((packed))
{
    T_FormatHeapIdx idx;
    textBld_S_StrPlusFormat buf[_FormatHeap_Size];
} S_StrFormatHeap;

/*------------------------------------ newFormat -------------------------------

    Returns a fresh format, left-justified, nothing else, with an empty string for safety.
 */

PRIVATE textBld_S_StrPlusFormat * newFormat(void)
{
    textBld_S_StrPlusFormat *f;
    static S_StrFormatHeap   formatHeap;

    if(++formatHeap.idx >= _FormatHeap_Size)        // Next slot please!
        { formatHeap.idx = 0; }

    f = &formatHeap.buf[formatHeap.idx];
    f->format = 0;                              // Clean out the format; left justified; nothing else. Blinks etc will be added.
    f->ofs = 0;                                 // Zero offset. (which may be used e.g for scrolling).
    f->str.asStr = "";                          // A format should never be used without content. But, just in case, give it something safe.
    f->link = (textBld_S_StrPlusFormat*)0;      // And is not chained to another format.
    return f;
}

// ---- Format flags.

#define _StrFormat_JustifyMask  0x03
#define _StrFormat_LeftJustify  0x00
#define _StrFormat_Centred      0x01
#define _StrFormat_RightJustify 0x02

#define _RdJustify(f)           ((f) & _StrFormat_JustifyMask)
#define _WrJustify(f,j) { (f) = (((f) & ~_StrFormat_JustifyMask) | (j)); }

#define _StrFormat_Blink        _BitM(3)
#define _StrFormat_Scroll       _BitM(4)
#define _StrFormat_ScrollToFit  _BitM(5)
#define _StrFormat_IsAlternate  _BitM(6)        // Formatter holds 2 links to text to be displayed alternately.
#define _StrFormat_Append       _BitM(7)
#define _StrFormat_JoinSpaced   _BitM(8)        // With '_Append', adds a space between (words).
#define _StrFormat_Translation  _BitM(9)        // A list of translations of the (English) string.
#define _StrFormat_Is3Way       _BitM(10)       // 3 items displayed alternately.

// A safe empty format
//PRIVATE textBld_S_StrPlusFormat nullFormat = { {""}, NULL, 0, 0 };

/* ------------------------------ chain ------------------------------------------

   Apply op() to the chain of formats with head 's'.
*/

PRIVATE void chain(textBld_S_StrPlusFormat *s, void(*op)(textBld_S_StrPlusFormat*))
{
    while(s != 0) {
       op(s);
       s = s->link;
    }
}

// ----- Constructors ----

/* ---------------------- left/right/centered/ -----------------------------------

   Lift 'str' into a newly-minted format. Justified left/right/ center.
*/

PUBLIC textBld_S_StrPlusFormat * textBld_CenterStr(C8 const *str) {
    textBld_S_StrPlusFormat *s = newFormat();
    _WrJustify(s->format, _StrFormat_Centred);
    s->str.asStr = str;
    return s;
}

/* ------------------------------ textBld_LeftStr ----------------------------------- */

PUBLIC textBld_S_StrPlusFormat * textBld_LeftStr(C8 const *str) {
    textBld_S_StrPlusFormat *s = newFormat();
    _WrJustify(s->format, _StrFormat_LeftJustify);
    s->str.asStr = str;
    return s;
}

/* ------------------------------ textBld_RightStr ----------------------------------- */

PUBLIC textBld_S_StrPlusFormat * textBld_RightStr(C8 const *str) {
    textBld_S_StrPlusFormat *s = newFormat();
    _WrJustify(s->format, _StrFormat_RightJustify);
    s->str.asStr = str;
    return s;
}

/* -------------------------- textBld_LiftTranslation -----------------------------------

   Lift a string array of translations, e.g  {"Yes","Si","Oui","Sim"}.
*/
PUBLIC textBld_S_StrPlusFormat * textBld_LiftTranslation(C8 const * const *strArray) {
    textBld_S_StrPlusFormat *s = newFormat();
    SETB(s->format, _StrFormat_Translation);
    s->str.asStrArray = strArray;
    return s;
}


// ---- Operators ----

/* ----------------------------- textBld_ToLeft/Right/Center --------------------------- */

PUBLIC textBld_S_StrPlusFormat * textBld_ToLeft(textBld_S_StrPlusFormat * s)
    { _WrJustify(s->format, _StrFormat_LeftJustify); return s; }

PUBLIC textBld_S_StrPlusFormat * textBld_ToRight(textBld_S_StrPlusFormat * s)
    { _WrJustify(s->format, _StrFormat_RightJustify); return s; }

PUBLIC textBld_S_StrPlusFormat * textBld_ToCenter(textBld_S_StrPlusFormat * s)
    { _WrJustify(s->format, _StrFormat_Centred); return s; }

/* ------------------------------ textBld_Blinks ----------------------------------- */

PRIVATE BOOL toggle(BOOL b) { return b == TRUE ? FALSE : TRUE; }

PRIVATE void blinkOp(textBld_S_StrPlusFormat * s) { s->format |= _StrFormat_Blink; }

PUBLIC textBld_S_StrPlusFormat * textBld_Blinks(textBld_S_StrPlusFormat * s) {
   chain(s, blinkOp);
   return s;
}


/* ------------------------------ textBld_Scrolls ----------------------------------- */

PUBLIC textBld_S_StrPlusFormat * textBld_Scrolls(textBld_S_StrPlusFormat * s)
{
    SETB(s->format, _StrFormat_Scroll);
    CLRB(s->format, _StrFormat_ScrollToFit);
    return s;
}

/* ------------------------------ textBld_ScrollToFit ----------------------------------- */

PUBLIC textBld_S_StrPlusFormat * textBld_ScrollToFit(textBld_S_StrPlusFormat * s)
{
    SETB(s->format, _StrFormat_ScrollToFit);
    CLRB(s->format, _StrFormat_Scroll);
    return s;
}

/* ------------------------------ textBld_Alternates -----------------------------------

   Makes a fork to 2 formats which will be printed alternately, every few secs.
*/

PUBLIC textBld_S_StrPlusFormat * textBld_Alternates(textBld_S_StrPlusFormat * a, textBld_S_StrPlusFormat * b)
{
    textBld_S_StrPlusFormat *s = newFormat();
    s->format = _StrFormat_IsAlternate;
    s->link = a;            // Print this....
    s->str.asLink = b;      // ...alternating with this.
    return s;
}

/* ------------------------------ textBld_Alternate3 -----------------------------------

   Makes a fork to 3 formats which will be printed alternately, every few secs.
*/

PUBLIC textBld_S_StrPlusFormat * textBld_Alternate3(textBld_S_StrPlusFormat * a, textBld_S_StrPlusFormat * b, textBld_S_StrPlusFormat * c)
{
    textBld_S_StrPlusFormat *s = newFormat();
    s->format = _StrFormat_Is3Way;
    s->link = a;            // Print this....
    textBld_S_StrPlusFormat *s1 = newFormat();
    s->str.asLink = s1;      // ...alternating with this.
    s1->link = b;
    s1->str.asLink = c;
    return s;
}


/* -------------------------------- textBld_Overlay --------------------------------

    Overlay 'top' on 'base'. Spaces in 'top' are transparent.
 */

PUBLIC textBld_S_StrPlusFormat * textBld_Overlay(textBld_S_StrPlusFormat * base, textBld_S_StrPlusFormat * top) {

    // 'base' may already be a chain of formats; so must find the end and tack on 'top'.
    textBld_S_StrPlusFormat *s = base;

    while(s->link != NULL)                                          // While there's (another)link...
        { s = s->link; }                                            // follow the link.

    s->link = top;
    return base;
}

/* -------------------------------- textBld_Join --------------------------------

    Append 'toAppend' to 'base'.

    Justifications (left, centered, right) apply to the final string and are propogated from
    'base', overwriting any justifiers in 'toAppend'.

    'blinks' is not propogated rightward.
 */

PUBLIC textBld_S_StrPlusFormat * textBld_Join(textBld_S_StrPlusFormat * base, textBld_S_StrPlusFormat * toAppend) {

    // 'base' may already be a chain of formats; so must find the end and tack on 'topAppend'.
    textBld_S_StrPlusFormat *s = base;

    while(s->link != NULL)                                          // While there's (another)link...
        { s = s->link; }                                            // follow the link.

    SETB(s->format, _StrFormat_Append);                             // At the end... mark an append
    s->link = toAppend;                                             // and link to 'toAppend'
    return base;                                                    // Return the entire longer format.
}

/* -------------------------------- textBld_JoinSpaced --------------------------------

    As for 'textBld_Join' but adds a space ebtween (words).
 */
PUBLIC textBld_S_StrPlusFormat * textBld_JoinSpaced(textBld_S_StrPlusFormat * base, textBld_S_StrPlusFormat * toAppend) {

    textBld_S_StrPlusFormat *s = base;

    // 'base' may already be a chain of formats; so must find the end and tack on 'topAppend'.
    while(s->link != NULL)                                          // While there's (another)link...
        { s = s->link; }                                            // follow the link.

    SETB(s->format, _StrFormat_Append | _StrFormat_JoinSpaced);     // At the end... mark an append, spaced
    s->link = toAppend;                                             // and link to 'toAppend'
    return base;                                                    // Return the entire longer format.
}


/* ------------------------------ clearLine ----------------------------------- */

PRIVATE void clearLine(C8 * buf) {
    memset(buf, ' ', _DisplayLineLength);
    buf[_DisplayLineLength] = '\0';
}

/* ----------------------------- overlayLine ------------------------------------------

   Overlay string 'top' onto string 'bottom'. Blanks/spaces in 'top' are 'clear',
   meaning they do not overwite. 'top' and 'bottom' may be different lengths; there
   are left-aligned. The resulting overlay is no longer than the original 'bottom';
   if 'top' is larger then extra chars in it are unused.
*/

PRIVATE void overlayLCDLine(C8 *bottom, C8 const *top)
{
    T_LCDLineCnt c;
    // Overlay number of chars which both strings have.
    T_LCDLineCnt len = MinU16(strlen(bottom), strlen(top));

    for(c = 0; c < len; c++)
    {
        if(top[c] != ' ')                           // Top char is not blank
            { bottom[c] = top[c]; }                 // ... then copy it onto bottom.
    }
}


/* ---- Text Line Heap

    Holds several screen lines and possibly overlays on them. Oldest line is
    overwritten by newest entry.
*/
#define _LineHeapSize 5     // Should be enuf for 2-line LCD with a couple of overlays

typedef struct {
    T_FormatHeapIdx idx;
    C8 buf[_LineHeapSize][_TextBld_MaxDisplayLineBytes];
} S_LineHeap;

PRIVATE C8 * newBlankLine(void)
{
    static S_LineHeap h;

    if (++h.idx >= _LineHeapSize) { h.idx = 0; } // Advance to next heap slot
    clearLine(h.buf[h.idx]);
    return h.buf[h.idx];
}

/* ----------------------------------- strncpy_scroll ---------------------------------------------

   Copy from 'src' to 'dest' and rotate copy by 'scrollCnt' (which is advanced in textBld_Run() to
   scroll the text). 'src' make be larger than 'dest'. strncpy_scroll() pads with (2) spaces so the
   start and end of 'src' text are not glued together.

   Scroll is leftwards.  's' holds the current scroll state.
*/
PRIVATE void strncpy_scroll(C8 *dest, C8 const *src, T_LCDLineCnt srclen, textBld_S_StrPlusFormat *s)
{
    T_LCDLineCnt c, srcOfs, destLen;

    // Insert spaces between repeats of 'src'. Otherwise the start and end of message will be 'glued'
    // together (in the scrolling display).
    #define _PadsSpaces 2

    /* Copy from 'src' to 'dest', rotating by current scroll offset (which is advanced every (0.8sec))
       Append 2 virtual spaces to src, so that the 1st and chars in 'src' are not glued .
    */
    destLen = MinU8(srclen, _DisplayLineLength);   // Whatever size 'src', will not copy more than size of 'dest'.

    for(c = 0; c < destLen; c++, dest++)
    {
        // 'src' pointer is (dest' ptr + scroll offset ) mod (src virtual length)
        srcOfs = ((U16)c + txtBld.scrollCnt) % (srclen + _PadsSpaces);

        *dest =                         // Copy char to dest...
            srcOfs >= srclen            // Past end of actual 'src' (into the virtual padding)
                ? ' '                   // then copy in spaces
                : src[srcOfs];          // else copy an actual 'src' char.
    }
    s->ofs++;
    txtBld.marksScroll = TRUE;          // Mark that we are scrolling something...
                                        // ...To show scrolled item for longer before switching to alternate.
    // Capture the length of the longest line being scrolled (if more than one).
    txtBld.scrollSize = MaxU8(txtBld.scrollSize, srclen);
}


// --------------------------------- mayTranslate ------------------------------------------

PRIVATE C8 const * mayTranslate(textBld_S_TranslationTbl const *tbl, textBld_S_StrPlusFormat const *s)
{
    return
        BSET(s->format, _StrFormat_Translation)
            ? s->str.asStrArray[txtBld.currLanguage]
            : (tbl == NULL
                ? s->str.asStr
                : translateFromTbl(tbl, s->str.asStr));
}

/* ------------------------------------- addBlanks ----------------------------------------------

    Add 'n' blanks to 'str', returning a (terminated) string.
*/
PRIVATE C8 * addBlanks(C8 *str, U8 n)
{
    C8 *eos = str + strlen(str);
    memset(eos, ' ', n);
    eos[n] = '\0';
    return str;
}

/* -------------------------------------- textBld_MakeLine -------------------------------------------

   Convert 's', string(s) and their formatters into a text line.  Apply any language 'translations'.
*/

PUBLIC C8 const * textBld_MakeLine(textBld_S_TranslationTbl const *tbl, textBld_S_StrPlusFormat const *s)
{
    T_LCDLineCnt        len = 0;
    C8 const            *str;
    textBld_T_StrFormat  f;
    C8                  *ln;
    C8                  *out = NULL;

    #define _AppendBuf_MaxChars 100
    static C8           appendBuf[_AppendBuf_MaxChars+1];
    textBld_S_StrPlusFormat const *s1;

    /* If this formatter is an alternate it does not itself hold anything to printf. It has two
       branches, each of which should be something printable. Pick which of these to print
       right now.
    */
    if(BSET(s->format, _StrFormat_IsAlternate))                     // Is an alternate?
    {
        s = txtBld.showAlternate                                    // Show printable B?
                ? (s->str.asLink == NULL  ? NULL : s->str.asLink)   // then, if B is printable. link to it, else NULL
                : (s->link == NULL ? NULL : s->link);               // else, if A is printable.....
    }
    /* Likewise for 3-way formats. A 3-way has a fork, one of which is to (another) fork.
       Each end should be something printable.
    */
    else if(BSET(s->format, _StrFormat_Is3Way))
    {
        s = txtBld.alt3Div == 2
                ? (s->str.asLink->str.asLink == NULL ? NULL : s->str.asLink->str.asLink)
                : (txtBld.alt3Div == 1
                    ? (s->str.asLink->link == NULL ? NULL : s->str.asLink->link)
                    : (s->link == NULL ? NULL : s->link));
    }

    if(s == NULL)                                                   // No format?
        { return ""; }                                              // then return a safe empty str.
    else {                                                          // else convert 's' into a printable
        do {                                                        // Loop down thru the strings/formatters, until end of chain
            ln = newBlankLine();                                    // A blank line to write.

            // Translate the string if necessary AND if a translations are available
            str = mayTranslate(tbl, s);

            /* If we are joining strings together to make the output line then do this first
               in a oversized local buffer which will be mapper onto the LCD line once it's fully
               built.
            */
            if(BSET(s->format, _StrFormat_Append) && s->link != NULL)           // Joining strings? AND there's a link to the string to append?
            {
                /* Then copy base string into 'build' buffer.

                   If blinking the base item AND we are in 'off' period, then copy in blanks
                   instead of the letters.
                */
                if((BCLR(s->format, _StrFormat_Blink) || txtBld.blinkToggle == FALSE)) {    // Show letters?
                    strncpy(appendBuf, str, _AppendBuf_MaxChars);               // then copy in (translated) base string
                }                                                               // ...but no more than will fit in Build buffer.
                else                                                            // else don't show
                {
                    appendBuf[0] = '\0';                                        // so empty append buf
                    addBlanks(appendBuf, MinU8( _AppendBuf_MaxChars,  strlen(str))); // and add blanks instead
                }

                s1 = s; while(s1->link != NULL)                                 // Until end of list to be be joined/overlaid
                {
                    if(BSET(s1->format, _StrFormat_Append))                     // Append next string?
                    {
                        if( (len = strlen(appendBuf)) >= _AppendBuf_MaxChars-1) // But 'build' buffer is full?
                        {
                            break;                                              // then quit rightaway
                        }
                        else                                                    // else there's room to add string.
                        {
                            if(BSET(s1->format, _StrFormat_JoinSpaced)) {       // Insert SPC between words to be joind?
                                addBlanks(appendBuf,1);                         // then append blank first.
                                len++; }                                        // One char longer

                            s1 = s1->link;                                      // Goto what's to be appended or overlaid

                            /* If blinking the item to be appended AND we are in 'off' period, then add blanks
                               instead of the letters.
                            */
                            if((BCLR(s1->format, _StrFormat_Blink) || txtBld.blinkToggle == FALSE))     // Show letters?
                            {                                                   // then translate and append 's1'
                                strncat( appendBuf, mayTranslate(tbl, s1), _AppendBuf_MaxChars - len);
                            }                                                   // but no more than will fit in Build buffer.
                            else                                                // else don't show
                            {
                                addBlanks( appendBuf,                           // so add blanks instead
                                    MinU8( _AppendBuf_MaxChars - len,
                                           strlen(mayTranslate(tbl, s1))));
                            }
                        }
                    }
                    else
                    {
                        s1 = s1->link;
                        overlayLCDLine(appendBuf + len, mayTranslate(tbl, s1));
                    }
                }
                str = appendBuf;
            }
            f = s->format;                                              // These are the formatters.

            /* If there's something to write onto the blank line then do so, applying any formatters for that
               line, including _blinks = '_StrFormat_Blink'.

               If we are in the off-period of a blink OR there's nothing to write onto 'ln' then the line
               remains blank (from  ln = newBlankLine(), above).
            */
            if( !isBlankOrEmpty(str) &&                                         // Print text if.... String has printable content? AND...
                (
                  str == appendBuf ||                                           // ...blink was already handled in 'append' construction? (above) OR
                  (BCLR(f, _StrFormat_Blink) ||                                 // this text does not blink? OR
                  txtBld.blinkToggle == FALSE)                                  // even if it does blink, we are in the on-period?
                ) )
            {                                                                   // AND we are supposed to show it?
                if(BSET(f, _StrFormat_Scroll) ||                                            // Scroll this field (always)? OR
                  (BSET(f, _StrFormat_ScrollToFit) && strlen(str) > _DisplayLineLength) )  // scroll this filed if to wide for screen?
                {
                    strncpy_scroll(ln, str, strlen(str), (textBld_S_StrPlusFormat*)s);
                }
                else                                                        // else don't scroll, justify left/right/center.
                {
                    len = MinU8(strlen(str), _DisplayLineLength);          // Make sure we don't overrun display

                    if( _RdJustify(f) == _StrFormat_Centred)                // Centered?
                    {
                        strncpy(&ln[(_DisplayLineLength - len) / 2], str, len);
                    }
                    else if( _RdJustify(f) == _StrFormat_LeftJustify)
                    {
                        strncpy(ln, str, len);
                    }
                    else if( _RdJustify(f) == _StrFormat_RightJustify)
                    {
                        strncpy(&ln[_DisplayLineLength - len], str, len);
                    }
                }
                // Formatted line should be terminated at end of LCD, but make sure.
                ln[_DisplayLineLength] = '\0';
            }
            if(out == NULL)                             // First pass thru, i.e 'out' == 0
                { out = ln; }                           // then point 'out' to the line we just made.
            else                                        // else already made a line, must overlay this one on it.
            {
                overlayLCDLine(out, ln);
            }            // Do do...
            s = s->link;                                // Move to whatever is linked to this format
        }
        while(s != 0 && BCLR(f, _StrFormat_Append));    // ..... until we reach the end of the chain of formats

        return out;
    }
}

// --------------------------------------- PRIVATE ------------------------------------------------

PRIVATE T_Timer blinkTimer = 0;
PRIVATE T_Timer scrollTimer = 0;
PRIVATE U8 altsDiv;


/* ------------------------------------------- textBld_Run --------------------------------------

   Call before each display refresh. Runs blink, scroll and alternates' timing.
*/
PUBLIC void textBld_Run(BOOL syncTimers)
{

    if(syncTimers)
    {
        altsDiv = 0;
        txtBld.showAlternate = FALSE;
        txtBld.alt3Div = 0;
        MarkNow(&blinkTimer);
        MarkNow(&scrollTimer);
    }

    if(ResetIfElapsed(&blinkTimer, _TicksSec(0.5)))                 // Every 1/2 sec
    {
        txtBld.blinkToggle = toggle(txtBld.blinkToggle);            // Toggle 'blink'.

        if(++altsDiv >= 5 +                                          // Every 2 1/2 secs but, ...
                // ... if something is scrolling, longer enuf to show the whole line.
                (txtBld.scrollSize > _DisplayLineLength
                    ? AminusBU8(txtBld.scrollSize, _DisplayLineLength) + 5
                    : 0)) {
            altsDiv = 0;
            txtBld.scrollCnt = 0;                                   // Make sure any scrolls start at LHS.
            txtBld.showAlternate = toggle(txtBld.showAlternate);    // Toggle between any alternate text.
            if(++txtBld.alt3Div >= 3)                                  // Goto to next of any 3-way text.
                { txtBld.alt3Div = 0; } }

    }

    if(ResetIfElapsed(&scrollTimer, _TicksSec(0.2)))               // Every 1/4sec
        { txtBld.scrollCnt++; }                                     // Scroll 1 char.

    txtBld.marksScroll = FALSE;                                     // Clear 'somethings scrolling' semaphore.
    txtBld.scrollSize = 0;                                          // reset this for next time.
}


/* ---------------------------------- textBld_Sync --------------------------------------------

   Sync timers for blinks and alternates.
*/

PUBLIC void textBld_Sync(void)
{
    altsDiv = 0;
    txtBld.showAlternate = FALSE;
    txtBld.alt3Div = 0;
    MarkNow(&blinkTimer);
    MarkNow(&scrollTimer);
    txtBld.scrollCnt = 0;
    txtBld.marksScroll = FALSE;
    txtBld.scrollSize = 0;
}

/* ------------------------------------- textBld_Init -------------------------------------------- */

PUBLIC void textBld_Init(TxtBld_S_Cfg const * cfg)
{
    txtBld.cfg = cfg;
    txtBld.currLanguage = cfg->dfltLanguage;
}


/* ------------------------------------- textBld_Setlanguage -------------------------------------------- */

PUBLIC void textBld_Setlanguage(U8 lang)
{
    txtBld.currLanguage = MinU8(lang, txtBld.cfg->numLanguages-1);
}



// ----------------------------- eof -----------------------------------------
