/*---------------------------------------------------------------------------
|
| This copies between bit-fields which are in underlying bytes. Any number of bits;
| any alignment relative to each other or to the underlying bytes. This is a clean
| copy; bytes outside the destination bit-filed are not modified, provide they
| can be read.
|
| The copy uses ports (bit64K_Ports) to map bit addresses to the underlying source
| and destination. So it can access fields on the far side of e.g a SPI or I2C
| bus. It was built to access the interfaces of those silicon subsystems with
| large register maps like a hoarders garage.
|
| The interface handles msbit-first and lsbbit-first in the underlying bytes. This
| allows it to handle little-endian and big-endian devices, whether the bit-order
| reads with the byte order or no.
|
| The bit64K_Ports interface is responsible for range checking that the bit addresses
| given to bit64K_Copy() fit the source and destination devices. The destination MUST
| be byte-readable, so sub-byte fields can be inserted and the resulting byte written
| back.
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

// ---------------------------- Shift left if ls > 0 else shift right.
static U8 shiftLR(U8 n, S8 ls) {
   return
      ls == 0
         ? n
         : (ls > 0
            ? n << ls
            : n >> (-ls)); }

// ----------------------------------------------------------------------------------------
static U8 rotL(U8 n, U8 by) {
   by &= 0x07;
   return (n<<by) | (n>>( (-by)&0x07 )); }

#define _byte(bitAddr)        bit64K_Byte(bitAddr)
#define _bit(bitAddr)         bit64K_BitBE(bitAddr)
#define _AminusBbits(a,b)     bit64K_AddBits(a, -(b))

// ------------------------- e.g mask(6,2) -> 0x60;  mask(4,3) -> 0x1C.
static U8 mask(U8 msb, U8 width) {
   return MakeAtoBSet_U8(msb, width > msb ? 0 : msb-width+1); }

static U8 maskAtoB(U8 msb, U8 lsb) {
   return MakeAtoBSet_U8(msb, lsb); }

// ----------------------------------------------------------------------------------------
static U8 orInto(U8 dest, U8 src, U8 srcMask) {
   return dest | (src & srcMask); }

// ----------------------------------------------------------------------------------------
static bool reloadCache_RdByte(bit64K_Ports const *port, U8 *out, S_Bit64K src, bit64K_T_Cnt bitsRem)
{
   S_byteBuf *bb = &port->cache->q;
   U8 reqBytes = MinU16( (bitsRem+1)/8, byteBuf_Size(bb) );    // e.g 9 'bitsRem' require we read 2 'port' bytes

   if( port->src.get(byteBuf_ToFill(bb, reqBytes), _byte(src), reqBytes) ) {  // Refilled cache from 'port'?
      port->cache->atByte = 0;
      byteBuf_Unlock(bb);                                      // Nope! Cache was locked before fill attempt. Unlock it.
      byteBuf_Flush(bb);                                       // Cache state is unknown - flush it!
      return false; }                                          // then fail.
   else {                                                      // else cache was filled from 'port'
      port->cache->atByte = _byte(src);                        // Mark that cache start from the byte containing 'src'.
      byteBuf_Unlock(bb);                                      // Was locked before fill. Unlock it.
      byteBuf_Read(bb, out, 1);                                // Now return 1st byte from the (re)filled cache.
      return true; }
}

/* ----------------------------- first/nextSrcByte --------------------------------------------------

   Get next byte from 'port[src]' into 'out', either directly or via a cache . 'src' is a bit-address.

   If there's a cache and it's not empty then get the byte from there. If the cache is empty
   then refill the cache from 'port[src]' and return 1st byte from cache.

   It's up to the caller to advance 'src' by one byte for every byte read. That so the cache
   is not reloaded with same data as before.
*/
static bool readFromCache(bit64K_Ports const *port, U8 *out, S_Bit64K src, bit64K_T_Cnt bitsRem)
{
   if(_byte(src) >= port->cache->atByte) {                  // 'src' may be in this cache; address is high than cache start?
      S_byteBuf *bb = &port->cache->q;

      U16 ofs = _byte(src) - port->cache->atByte;           // Offset relative to cache start would be...

      if(ofs < byteBuf_Count(bb)) {                         // That offset is within the cache? so data is in there
         if( 1 == byteBuf_ReadAt(bb, out, ofs, 1)) {        // then (attempt to) read 1 byte from that cache?
            return true; }}}                                // Got that byte... we are done.

   return false;     // Some fail!
}

PRIVATE bool nextSrcByte(bit64K_Ports const *port, U8 *out, S_Bit64K src, bit64K_T_Cnt bitsRem, bool firstRead)
{
   if(port->cache != NULL) {                                            // There's a cache?
      S_byteBuf *bb = &port->cache->q;

      if(byteBuf_Exists(bb) == TRUE ) {                                 // That cache is made? i.e it has a buffer?
         if(firstRead == true) {                                        // Is the 1st byte requested from 'src'?
            if( true == readFromCache(port, out, src, bitsRem)) {       // See if 'src' is already cached. Got it from there?
               return true; }}                                          // Yep! Done!
         else {                                                         // else is 2nd,3rd... byte-read of this read
            if( byteBuf_Read(bb, out, 1) == 1) {                        // If cache is large enough then all bytes should be cached. Read should succeed.
               return true; }}                                          // Got byte. Done!

         // Couldn't read byte from cache. Either 'src' jumped or must cache another line for existing transaction
         return reloadCache_RdByte(port, out, src, bitsRem); }}         // Reloaded cache at 'src' and got 1st byte??
   // There's no cache.
   return port->src.get(out, _byte(src), 1);                            // Return 1 byte directly from 'port'.
}


// -------------------------- Endian-aware iterator for buffer ptrs. -----------------------

typedef struct endianPtrTag T_EndianPtr;

typedef struct endianPtrTag {
   U8* (*next)(T_EndianPtr*);    // returns '_at', iterated.
   U8 const * _at;               // Current ptr.
   } T_EndianPtr;

// Increment / decrement '_at'.
PRIVATE U8 *epIncr(T_EndianPtr *di) {
   di->_at++;
   return (U8*)di->_at; }

PRIVATE U8 *epDecr(T_EndianPtr *di) {
   di->_at--;
   return (U8*)di->_at; }

// Pre-position buffer ptr and set direction, depending system endian vs target endian.
PRIVATE U8* epNew(T_EndianPtr *ep, U8 const *bufStart, bit64K_T_Cnt nbits, E_EndianIs bitFieldEndian)
{
   ep->_at = bufStart;     // Start ptr at buffer start; may reposition it below.

   // If the port/bit-field endian is opposite to system endian then reverse the bit-field bytes into the buffer.
   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
         bool up = bitFieldEndian == eBigEndian ? false : true;
      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
         bool up = bitFieldEndian == eLittleEndian ? false : true;
      #else
         bool up = true;
      #endif

      /* If multiple byte transfer AND byte order is to be reversed then pre-position '_at' at the
         high address and will count backwards.
      */
      if(nbits > 8 && up == false)
         { ep->_at += (nbits-1)/8; }
   #else
      #warning "bit64K_Out() Endian undefined - bytes will always be copied no-reverse."
      #define _bump(ptr)  (ptr)++
   #endif // __BYTE_ORDER__

   ep->next = up == true ? epIncr : epDecr;
   return (U8 *)ep->_at;
}

// ------------- ends: endian pointer. ----------------------


// --------------------------------------------------------------------------------------
static bool legalBitAddr(bit64K_Range const *r, S_Bit64K addr) {
   return                                             // 'addr' NOT legal if? ...
      r->bits.max > r->bits.min &&                    // Did set a bit-range for this 'port'? i.e 'min' and 'max' not both zero, AND
      (addr < r->bits.min && addr > r->bits.max)      // 'addr' is outside that range?
         ? false : true; }


/*-----------------------------------------------------------------------------------------
|
|  bit64K_Copy()
|
|  Using 'port', copy 'numBits' from 'src' to 'dest', where these are 64K bitfield addresses.
|  'port' maps 'src', and 'dest' to the actual data source and sink.
|
|  Returns false if a 'port' operation fails or if the parameters for the transfer are illegal.
|
------------------------------------------------------------------------------------------*/

PUBLIC bool bit64K_Copy(bit64K_Ports const *port, S_Bit64K dest, S_Bit64K src, bit64K_T_Cnt numBits)
{
   if( false == legalBitAddr(&port->dest.range, dest) ||
       false == legalBitAddr(&port->src.range, src))
      { return false; }
   else {
      U8 open;
      for(bit64K_T_Cnt rem = numBits; rem > 0; rem -= open, src += open, dest += open)
      {
         // rdDest() byte containing 'dest'. Clear those bits which will be updated.
         U8 db;
         U8 msb = _bit(dest);                               // This is the msbit of the 'dest' field..
         open = rem > msb+1 ? msb+1 : rem;                  // Will clear these many bits.

         /* If there's a port.rdDest() then will read current destination byte clear the destination
            bit field for the bits to be inserted frm 'src'. Otherwise start with the destination byte
            clear.
         */
         if(port->dest.rd == NULL) {                         // No rdDest()?
            db = 0; }                                       // then start with dest byte all-cleared.
         else {
            /* Will clear then fill 'open' bits in 't' from 'msb' rightward. If 'rem' > 'msb' this was
               fully 'msb' to b0.
            */
            if( false == port->dest.rd(&db, _byte(dest), 1))
               { return false; }

            CLRB(db, mask(msb, open)); }                    // Clear 'open' bits from 'msb'.

         /* Copy 'open' bits from 'src' into the open field in 'db'. This needs either 1 or 2
            byte-reads from 'src', depending on whether the field to be copied crosses a byte
            boundary.
         */
         U8 sb;
         if(false == port->src.get(&sb, _byte(src), 1))      // getSrc() into 's' the byte containing 'src'.
            { return false; }

         /* 's' has _bit(src)+1 bits from 'src'. Take some or all of these, depending on how
            much more we 'ask' for.
         */
         U8 got = MinU8(open, _bit(src)+1);                 // Take up to 'open' bits from 'src' forward to b0 of byte 't'

         /* Align the bits from 'src' (in 's')
         */
         sb = shiftLR(sb, msb-_bit(src));                   // Shift left or right to align the field from 'src' with 'dest'.
         db = orInto(db, sb, mask(msb, got));               // Mask to select just that field and OR into dest byte.

         if(open > got)                                     // Got a partial destination field from 1st byte?
         {
            if(false == port->src.get(&sb, _byte(src)+1, 1)) // then the remainder of the field into 'sb'.
               { return false; }

            msb -= got;                                     // Move the 'dest' msb marker past the partial source field we already wrote.
            sb = rotL(sb, msb+1);                           // Rotate partial field left to align with destination slot.
            db = orInto(db, sb, mask(msb, open-got));       // Mask to select just that field and OR into dest byte.
         }

         if(false == port->dest.wr(_byte(dest), &db, 1))     // Destination byte updated; put it back.
            { return false; }
      } // for(U8 rem = numBits...
      return true;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  bit64K_Out()
|
|  Using 'port', copy 'numBits' from 'src' to 'dest', where 'src' is a 64K bitfield address.
|
|  If 'srcEndian' is opposite to the system __BYTE_ORDER__ then bytes from 'src' will be
|  copied into endian in reverse order (relative to ascending source address).
|
------------------------------------------------------------------------------------------*/

#define _msb7  7
#define _8bits 8

PUBLIC bool bit64K_Out(bit64K_Ports const *port, U8 *dest, S_Bit64K src, bit64K_T_Cnt numBits, U8 srcEndian)
{
   if( false == legalBitAddr(&port->src.range, src))           // 'src' not legal?
      { return false; }                                        // then fail!
   else {                                                      // else continue.
      T_EndianPtr di;
      dest = epNew(&di, dest, numBits, srcEndian);            // Endian-aware 'dest'.

      /* Fill each dest byte from a bytes-worth of source bit-field. Depending on alignment, this will
         be from a single source byte or 2 consecutive source bytes.
      */
      U8 ask; bool is1st = true;
      for(bit64K_T_Cnt rem = numBits; rem > 0; rem -= ask, src += ask, dest = di.next(&di))
      {
         /* Copy 'open' bits from 'src' into the 'dest' byte. This needs either 1 or 2
            byte-reads from 'src', depending on whether the field to be copied crosses a byte
            boundary.
         */
         // First, get 1st byte into 'sb', while maybe refilling any cache.
         U8 sb;
         if(false == nextSrcByte(port, &sb, src, rem, is1st))         // Read 1st byte from 'port'? ..either directly or via cache
            { return false; }
         is1st = false;

         ask = rem > _8bits ? _8bits : rem;                   // This pass, from 1 or2 bytes, take (maybe all of) 'rem' but no more than 8 bits.
         U8 got = MinU8(ask, _bit(src)+1);                    // From the 1st byte take b[src:0], but no more than 'ask'.

         sb = shiftLR(sb, (S8)ask-_bit(src)-1);               // (Maybe) left-justify or right-justify the 'got' bits (from 1st byte) to their position determined by 'ask'.
         U8 db = 0;                                           // ...any space to the right of these to b0 will be filled from 2nd byte.
         db = orInto(db, sb, mask(ask-1, got));               // Mask to select just that field and OR into dest byte.

         if(ask > got)                                        // Got a partial destination field from 1st byte?
         {                                                    // then get 2nd byte into 'sb', while maybe refilling any cache.
            if(false == nextSrcByte(port, &sb, src+_8bits, rem-got, false)) // Read 2nd byte from 'port'? ..either directly or via cache
               { return false; }

            sb = rotL(sb, ask-got);                           // Rotate 2nd byte to fit in
            db = orInto(db, sb, maskAtoB(ask-got-1,0));       // Mask to select just that field and OR into dest byte.
         }
         *dest = db;                                          // Write out dest byte.
      } // for(U8 rem = numBits...
      return true;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  bit64K_In()
|
|  Using 'port', copy 'numBits' from 'src' to 'dest', where 'dest' is a 64K bitfield address.
|
|  If 'srcEndian' is opposite to the system __BYTE_ORDER__ then bytes from 'src' will be
|  copied into endian in reverse order (relative to ascending source address).
|
------------------------------------------------------------------------------------------*/

PUBLIC bool bit64K_In(bit64K_Ports const *port, S_Bit64K dest, U8 const *src, bit64K_T_Cnt numBits, U8 destEndian)
{
   if( false == legalBitAddr(&port->dest.range, dest))            // 'dest' not legal?
      { return false; }                                           // then fail!
   else {                                                         // else continue.
      T_EndianPtr si;
      src = epNew(&si, src, numBits, destEndian);                 // Endian-aware 'src'.

      for(bit64K_T_Cnt rem = numBits; rem > 0; dest += _8bits, src = si.next(&si))
      {
         /* If there's a port.rdDest() then will read current destination byte (into 'db'). Then clear
            and fill 'open' bits in 'db' from 'msb' rightward. If 'rem' > 'msb' this will be fully
            'msb' to b0.
               If no port.rdDest() then just zero 'db'.
         */
         U8 msb = _bit(dest);                                     // This is the msbit of the 'dest' field..
         U8 open = rem > msb+1 ? msb+1 : rem;                     // Will clear these many bits.

         bit64K_atByte destAt = _byte(dest);

         U8 db;
         if(port->dest.rd == NULL) {                               // No rdDest()?
            db = 0; }                                             // then start with dest byte clear.
         else {
            if( false == port->dest.rd(&db, destAt, 1))            // Get 'dest' into 'db'.
               { return false; }

            CLRB(db, mask(msb, open)); }                          // Clear 'open' bits from 'msb' downward.

         U8 sb = shiftLR(*src, msb-_msb7);                        // Shift left or right to align the field from 'src' with 'dest'.
         db = orInto(db, sb, mask(msb, open));                    // Mask to select just that field and OR into dest byte.

         if(false == port->dest.wr(destAt, &db, 1))                // Destination byte updated; put it back.
            { return false; }

         rem -= open;                                             // These many bits remaining.

         // If copied to 'destAt' just part of the byte read from 'src' then must copy the remainder to
         // 'destAt' + 1, left justified.

         if(open < _8bits && rem > 0)                             // Used just part of 'src' byte? AND bits left to transfer?
         {
            U8 write = _8bits - open;                             // Bits remaining in the byte read from 'src'

            if(rem < write) {                                     // Only some of those remaining bits needed to complete transfer?
               write = rem;                                       // then will transfer just what we need (to complete)
               rem = 0; }                                         // and after that the whole transfer will be done.
            else {                                                // else will use all remaining bits from 'src'.
               rem -= write; }                                    // Zero or more bits left to complete whole transfer.

            if(port->dest.rd == NULL) {                            // No rdDest()?
               db = 0; }                                          // then start with dest byte all-cleared.
            else {
               if( false == port->dest.rd(&db, destAt+1, 1))       // else get destination
                  { return false; }
               else
                  { CLRB(db, mask(_msb7, write)); }}              // Clear 'open' bits from 'msb'.

            sb = rotL(*src, open);                                // Rotate partial field left to align with destination slot.
            db = orInto(db, sb, mask(_msb7, write));              // Mask to select just that field and OR into dest byte.

            if(false == port->dest.wr(destAt+1, &db, 1))           // Destination byte updated; put it back.
               { return false; }
         }
      } // for(U8 rem = numBits...
      return true;
   }
}

// ---------------------------------- eof --------------------------------  -
