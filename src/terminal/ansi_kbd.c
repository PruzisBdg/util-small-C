/*---------------------------------------------------------------------------
|
| Maps multi-byte ANSI keyboard codes to unique (mostly 1-byte) enumerations.
| These enums are mapped form 0x80 up so they can co-exist with printable ASCII.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/* There are too many key-codes to fit in 0x80 - 0xFF so we have an 'overflow; above 0xFF which
   holds the less-used key combination keystrokes.
*/
typedef enum {
   // ---- Low page 0x80 - 0xFF.
   NoKey = 0,

	// Natural system keys, are named here just for convenience.
	ESC=27, BKSPC=8, TAB=9, ENTER=13, SPC=32, CtlBKSPC = 127,

	// All control combos are mapped 0x80 ->
	AltENTER=0x80, ShTAB, CtlTAB, AltTAB, PRTSCRN, BRK,

	// Extended keys
	INS,    DEL,    HOME,    END,    PGUP,    PGDN,    UP,    LEFT,    RIGHT,    DOWN,
	ShINS,  ShDEL,  ShHOME,  ShEND,  ShPGUP,  ShPGDN,  ShUP,  ShLEFT,  ShRIGHT,  ShDOWN,
	CtlINS, CtlDEL, CtlHOME, CtlEND, CtlPGUP, CtlPGDN, CtlUP, CtlLEFT, CtlRIGHT, CtlDOWN,
	AltINS, AltDEL, AltHOME, AltEND, AltPGUP, AltPGDN, AltUP, AltLEFT, AltRIGHT, AltDOWN,

	// F1-F12
	F1,F2,F3,F4,F6,F7,F8,F9,F10,F11,F12,

	// The above mappings fit below 0xFF; can be in 'T_KeyU8', U8.
	// Numeric Keypad.  Shift 0-9 are '0'-'9'.
	Pad1,Pad2,Pad3,Pad4,Pad5,Pad6,Pad7,Pad8,Pad9,Pad0,PadDP,

	CtlA, CtlW, CtlC, CtlX, CtlV, CtlS, CtllQ,

	// ---- High page 0x100 ->

	LastU8Key,
	FirstU16Key = 0x100,

	ShF1,  ShF2,  ShF3,  ShF4,  ShF6,  ShF7,  ShF8,  ShF9,  ShF10,  ShF11,  ShF12,
	CtlF1, CtlF2, CtlF3, CtlF4, CtlF6, CtlF7, CtlF8, CtlF9, CtlF10, CtlF11, CtlF12,
	AltF1, AltF2, AltF3, AltF4, AltF6, AltF7, AltF8, AltF9, AltF10, AltF11, AltF12,

	ShPadMUL,

	CtlPad1, CtlPad2,CtlPad3,CtlPad4,CtlPad5,CtlPad6,CtlPad7,CtlPad8,CtlPad9,CtlPad0,CtlPadDP,
		CtlPadENTER,CtlPadDIV,CtlPadMUL,CtlPadMINUS,CtlPadPLUS,

		AltPadENTER,AltPadDIV,AltPadMINUS,AltPadPLUS,

	CtlB,CtlD,CtlE,CtlF,CtlG,CtlH,CtlI,CtlJ,CtlL,CtlM,CtlN,CtlO,CtlP,CtlQ,CtlR,CtlT,CtlU,CtlY,CtlZ,

	// Extended keys start here.
	AltA,AltB,AltC,AltD,AltE,AltF,AltG,AltH,AltI,AltJ,AltL,AltM,AltN,AltO,AltP,AltQ,AltR,AltS,AltT,AltU,AltV,AltW,AltX,AltY,AltZ,
	Alt0,Alt1,Alt2,Alt3,Alt4,Alt5,Alt6,Alt7,Alt8,Alt9

} E_ExtKeys;

CASSERT(LastU8Key <= 0x100);     // Make sure the low-page keys stay below 0xFF.

typedef U8 T_KeyU8;
typedef U16 T_KeyU16;

// A printable or control which a keyboard can make AND which will be returned as-is.
PRIVATE bool aNaturalKey(U8 ch) {
   return
      ch == ESC || ch == BKSPC || ch == TAB || ch == CtlBKSPC ||
      (ch >= ' ' && ch <= '~'); }

// 2-byte key-codes are prefixed with 0 or 224.
PRIVATE bool aPrefix(U8 ch) {
   return ch == 0 || ch == 224; }


// To map 'F' keys which are contiguous in a range.
typedef struct {U8 l; U8 u; T_KeyU16 base;} S_MapRange;     // e.g F1-F10 are 59-67.

static inline bool inside(U8 ch, U8 l, U8 u)
   { return (ch >= l && ch <= u) ? true : false; }

static T_KeyU8 foundIn(U8 ch, S_MapRange const *rng) {
   return
      inside(ch, rng->l, rng->u) == false
         ? NoKey
         : rng->base + (ch - rng->l); }

// All the 'F' keys and their Shift,Ctl,Alts.
PRIVATE T_KeyU16 mapFKeys(U8 ch) {
   S_MapRange const fkeys[] = {
      { .l = 59,  .u = 67,  .base = F1    },
      { .l = 84,  .u = 93,  .base = ShF1  },
      { .l = 94,  .u = 103, .base = CtlF1 },
      { .l = 104, .u = 113, .base = AltF1 },
      { .l = 133, .u = 134, .base = F11   },
      { .l = 135, .u = 136, .base = ShF11 },
      { .l = 137, .u = 138, .base = CtlF11},
      { .l = 139, .u = 140, .base = AltF11}};

   for(U8 i = 0; i < RECORDS_IN(fkeys); i++) {
      T_KeyU8 ek;
      if( (ek = foundIn(ch, &fkeys[i])) != NoKey ) {
         return ek; }}
   return NoKey;
}

// Map key-codes one-by-one
typedef struct { U8 ch; T_KeyU8 ky;}  S_MapU8;

PRIVATE T_KeyU8 mapExtCh(S_MapU8 const *m, U8 size, U8 ch) {
   for(U8 i = 0; i < size; i++, m++) {
      if(m->ch == ch)  {
         return m->ky; }}
   return NoKey;}

// Map the '224' extended key-codes.
PRIVATE T_KeyU16 mapZeroExt(U8 ch)
{
   #define _loMap(c, k) { .ch = (c), .ky = (k) }

   S_MapU8 const loMap[] = {
      _loMap(82,  INS),
      _loMap(83,  DEL),
      _loMap(71,  HOME),
      _loMap(79,  END),
      _loMap(73,  PGUP),
      _loMap(81,  PGDN),
      _loMap(72,  UP),
      _loMap(75,  LEFT),
      _loMap(77,  RIGHT),
      _loMap(80,  DOWN),

      _loMap(146, ShINS),
      _loMap(147, ShDEL),
      _loMap(119, ShHOME),
      _loMap(117, ShEND),
      _loMap(132, ShPGUP),
      _loMap(118, ShPGDN),
      _loMap(141, ShUP),
      _loMap(115, ShLEFT),
      _loMap(116, ShRIGHT),
      _loMap(145, ShDOWN),

      _loMap(162, AltINS),
      _loMap(163, AltDEL),
      _loMap(151, AltHOME),
      _loMap(159, AltEND),
      _loMap(153, AltPGUP),
      _loMap(161, AltPGDN),
      _loMap(152, AltUP),
      _loMap(155, AltLEFT),
      _loMap(157, AltRIGHT),
      _loMap(154, AltDOWN),

      _loMap(79, Pad1),
      _loMap(80, Pad2),
      _loMap(81, Pad3),
      _loMap(75, Pad4),
      _loMap(76, Pad5),
      _loMap(77, Pad6),
      _loMap(71, Pad7),
      _loMap(72, Pad8),
      _loMap(73, Pad9),
      _loMap(82, Pad0),
      _loMap(83, PadDP),
   };

   // For compactness, the high-page key-codes are encoded as 1
   // For compactness, the high-page key-codes are encoded as 1 byte, offset

   #define _hiMap(c, k) { .ch = (c), .ky = (k)-FirstU16Key }

   S_MapU8 const hiMap[] = {
      _hiMap(144, ShPadMUL),

      _hiMap(117, CtlPad1),
      _hiMap(145, CtlPad2),
      _hiMap(118, CtlPad3),
      _hiMap(115, CtlPad4),
      _hiMap(143, CtlPad5),
      _hiMap(116, CtlPad6),
      _hiMap(119, CtlPad7),
      _hiMap(141, CtlPad8),
      _hiMap(132, CtlPad9),
      _hiMap(146, CtlPad0),
      _hiMap(146, CtlPadDP),

      _hiMap(142, CtlPadDIV),
      _hiMap(78,  CtlPadMUL),
      _hiMap(149, CtlPadMINUS),
      _hiMap(150, CtlPadPLUS),

      _hiMap(166, AltPadENTER),
      _hiMap(74,  AltPadDIV),
      _hiMap(149, AltPadMINUS),
      _hiMap(55,  AltPadPLUS)};

   T_KeyU16 rtn;
   return
      (rtn = mapExtCh(loMap, RECORDS_IN(loMap), ch)) != NoKey           // Found code in low map?
         ? rtn                                                          // then that code is our answer.
         : ( (rtn = mapExtCh(hiMap, RECORDS_IN(hiMap), ch)) != NoKey    // else found code in high map?
            ? rtn+FirstU16Key                                           // then answer is from that map plus high-page offset
            : NoKey);                                                   // else not found in low-map or high-map. Wasn't a legal keycode.
}

/* ------------------------------- AnsiKey_GetCh ------------------------------------------

   Given a stream of 'ch' from a keyboard, returns the keypresses, regular or extended.

   If adding a 'ch' does complete a keystroke then returns 'NoKey'.

   Any 'ch' or 'ch' sequence which isn't from a keyboard also returns 'NoKey'; and reset the
   keyboard decode state.
*/
#define _NoPrefix 0xFF     // Not 0 or 224 (prefixes for 2-byte keys)

PUBLIC T_KeyU16 AnsiKey_GetCh(U8 ch)
{
   static U8 pfx = _NoPrefix;          // Until we get 1st 0 or 224.

   if(pfx == _NoPrefix)                // Not decoding a prefix?
   {
      if(aNaturalKey(ch)) {            // Printable or whitespace?
         return ch; }                  // return as-is.
      else {                           // else see if it's a prefix
         if( aPrefix(ch))              // Is 0 or 224?
            { pfx = ch; }              // then remember the prefix
         return NoKey; }               // and wait for the 2nd byte.
   }
   else                                // else previous byte was a prefix
   {
      if(pfx == 0) {                   // That prefix was '0'?
         pfx = _NoPrefix;              // whatever happens now, we will be done decoding and extended key.
         return mapZeroExt(ch); }      // Map the '0;nn'; Returns 'NoKey' if not legal.
      else if(pfx == 224) {            // That prefix was 224?, F1-F12
         pfx = _NoPrefix;              // whatever happens now, we will be done decoding and extended key.
         return mapFKeys(ch); }        // Map the '224;nn' to F1-F12 Returns 'NoKey' if not legal.
      else{                            // else illegal value for 'prefix'. How did this happen??!!
         pfx = _NoPrefix;              // No matter; just reset...
         return NoKey; }                // ...and we got nothing.
   }
}

// ------------------------------ eof ---------------------------------------
