/*---------------------------------------------------------------------------
|
| This copies between bit-fields which are in underlying bytes. Any number of bits;
| any alignment relative to each other or to the underlying bytes. This is a clean
| copy; bytes outside the destination bit-filed are not modified, provide they
| can be read.
|
| The copy uses ports (S_Bit64KPorts) to map bit addresses to the underlying source
| and destination. So it can access fields on the far side of e.g a SPI or I2C
| bus. It was built to access the interfaces of those silicon subsystems with
| large register maps like a hoarders garage.
|
| The interface handles msbit-first and lsbbit-first in the underlying bytes. This
| allows it to handle little-endian and big-endian devices, whether the bit-order
| reads with the byte order or no.
|
| The S_Bit64KPorts interface is responsible for range checking that the bit addresses
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

#define _byte(bitAddr) bit64K_Byte(bitAddr)
#define _bit(bitAddr)  bit64K_BitBE(bitAddr)

// ------------------------- e.g mask(6,2) -> 0x60;  mask(4,3) -> 0x1C.
static U8 mask(U8 msb, U8 width) {
   return MakeAtoBSet_U8(msb, width > msb ? 0 : msb-width+1); }

static U8 maskAtoB(U8 msb, U8 lsb) {
   return MakeAtoBSet_U8(msb, lsb); }

// ----------------------------------------------------------------------------------------
static U8 orInto(U8 dest, U8 src, U8 srcMask) {
   return dest | (src & srcMask); }

/* ----------------------------- nextSrcByte --------------------------------------------------

   Get next byte from 'port[src]' into 'out', either directly or via a cache . 'src' is a bit-address.

   If there's a cache and it's not empty then get the byte from there. If the cache is empty
   then refill the cache from 'port[src]' and return 1st byte from cache.

   It's up to the caller to advance 'src' by one byte for every byte read. That so the cache
   is not reloaded with same data as before.
*/
PRIVATE bool nextSrcByte(S_Bit64KPorts const *port, U8 *out, S_Bit64K src, bit64K_T_Cnt bitsRem)
{
   if(port->cache != NULL) {                                            // There's a cache?
      S_byteQ *cc = port->cache;

      if(byteQ_Exists(cc) == TRUE ) {                                   // That cache is made? i.e it has a buffer?
         if( byteQ_Read(cc, out, 1) == 1) {                             // then (attempt to) read 1 byte from that cache?
            return true; }                                              // Got that byte... we are done.
         else {                                                         // else cache was empty...
                                                                        // ... so will (re)fill it from 'port', taking up to all remaining bytes/bits
            U8 reqBytes = MinU16( (bitsRem+1)/8, byteQ_Size(cc) );      // e.g 9 'bitsRem' require we read 2 'port' bytes

            if( port->getSrc(byteQ_ToFill(cc, reqBytes), _byte(src), reqBytes) ) {  // Refilled cache from 'port'?
               byteQ_Unlock(cc);                                        // Nope! Cache was locked before fill attempt. Unlock it.
               byteQ_Flush(cc);                                         // Cache state is unknown - flush it!
               return false; }                                          // then fail.
            else {                                                      // else cache was filled from 'port'
               byteQ_Unlock(cc);                                        // Was locked before fill. Unlock it.
               byteQ_Read(cc, out, 1);                                  // Now return 1st byte from the (re)filled cache.
               return true; }}}}                                        // Success.

   return port->getSrc(out, _byte(src), 1);     // No cache; return 1 byte directly from 'port'.
}


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

PUBLIC bool bit64K_Copy(S_Bit64KPorts const *port, S_Bit64K dest, S_Bit64K src, bit64K_T_Cnt numBits)
{
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
      if(port->rdDest == NULL) {                         // No rdDest()?
         db = 0; }                                       // then start with dest byte all-cleared.
      else {
         /* Will clear then fill 'open' bits in 't' from 'msb' rightward. If 'rem' > 'msb' this was
            fully 'msb' to b0.
         */
         if( false == port->rdDest(&db, _byte(dest), 1))
            { return false; }

         CLRB(db, mask(msb, open)); }                    // Clear 'open' bits from 'msb'.

      /* Copy 'open' bits from 'src' into the open field in 'db'. This needs either 1 or 2
         byte-reads from 'src', depending on whether the field to be copied crosses a byte
         boundary.
      */
      U8 sb;
      if(false == port->getSrc(&sb, _byte(src), 1))      // getSrc() into 's' the byte containing 'src'.
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
         if(false == port->getSrc(&sb, _byte(src)+1, 1)) // then the remainder of the field into 'sb'.
            { return false; }

         msb -= got;                                     // Move the 'dest' msb marker past the partial source field we already wrote.
         sb = rotL(sb, msb+1);                           // Rotate partial field left to align with destination slot.
         db = orInto(db, sb, mask(msb, open-got));       // Mask to select just that field and OR into dest byte.
      }

      if(false == port->wrDest(_byte(dest), &db, 1))     // Destination byte updated; put it back.
         { return false; }
   } // for(U8 rem = numBits...
   return true;
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

PUBLIC bool bit64K_Out(S_Bit64KPorts const *port, U8 *dest, S_Bit64K src, bit64K_T_Cnt numBits, U8 srcEndian)
{
   // If 'srcEndian' is opposite to the system __BYTE_ORDER__ then will flip bytes on copy-out.
   #define _fwd      1
   #define _reverse  0

   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
         bool dir = srcEndian == eBigEndian ? _reverse : _fwd;
      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
         bool dir = srcEndian == eLittleEndian ? _reverse : _fwd;
      #else
         bool dir = _fwd;
      #endif
      #define _bump(ptr)  dir == _fwd ? (ptr)++ : (ptr)--

      /* If multiple byte transfer AND byte order is to be reversed then pre-position 'dest' at the
         high address and will count backwards.
      */
      if(numBits > 8 && dir == _reverse)
         { dest += numBits/8; }
   #else
      #warning "bit64K_Out() Endian undefined - bytes will always be copied no-reverse."
      #define _bump(ptr)  (ptr)++
   #endif // __BYTE_ORDER__

   /* Fill each dest byte from a bytes-worth of source bit-field. Depending on alignment, this will
      be from a single source byte or 2 consecutive source bytes.
   */
   U8 ask;
   for(bit64K_T_Cnt rem = numBits; rem > 0; rem -= ask, src += ask, _bump(dest))
   {
      /* Copy 'open' bits from 'src' into the 'dest' byte. This needs either 1 or 2
         byte-reads from 'src', depending on whether the field to be copied crosses a byte
         boundary.
      */
      // First, get 1st byte into 'sb', while maybe refilling any cache.
      U8 sb;
      if(false == nextSrcByte(port, &sb, src, rem))         // Read 1st byte from 'port'? ..either directly or via cache
         { return false; }

      ask = rem > _8bits ? _8bits : rem;                   // This pass, from 1 or2 bytes, take (maybe all of) 'rem' but no more than 8 bits.
      U8 got = MinU8(ask, _bit(src)+1);                    // From the 1st byte take b[src:0], but no more than 'ask'.

      sb = shiftLR(sb, (S8)ask-_bit(src)-1);               // (Maybe) left-justify or right-justify the 'got' bits (from 1st byte) to their position determined by 'ask'.
      U8 db = 0;                                           // ...any space to the right of these to b0 will be filled from 2nd byte.
      db = orInto(db, sb, mask(ask-1, got));               // Mask to select just that field and OR into dest byte.

      if(ask > got)                                        // Got a partial destination field from 1st byte?
      {                                                    // then get 2nd byte into 'sb', while maybe refilling any cache.
         if(false == nextSrcByte(port, &sb, src+_8bits, rem-got)) // Read 2nd byte from 'port'? ..either directly or via cache
            { return false; }

         sb = rotL(sb, ask-got);                           // Rotate 2nd byte to fit in
         db = orInto(db, sb, maskAtoB(ask-got-1,0));       // Mask to select just that field and OR into dest byte.
      }
      *dest = db;                                          // Write out dest byte.
   } // for(U8 rem = numBits...
   return true;
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

PUBLIC bool bit64K_In(S_Bit64KPorts const *port, S_Bit64K dest, U8 const * src, bit64K_T_Cnt numBits, U8 srcEndian)
{
   // If 'srcEndian' is opposite to the system __BYTE_ORDER__ then will flip bytes on copy-out.
   #define _fwd      1
   #define _reverse  0

   #ifdef __BYTE_ORDER__
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
         bool dir = srcEndian == eBigEndian ? _reverse : _fwd;
      #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
         bool dir = srcEndian == eLittleEndian ? _reverse : _fwd;
      #else
         bool dir = _fwd;
      #endif
      #define _bump(ptr)  dir == _fwd ? (ptr)++ : (ptr)--

      /* If multiple byte transfer AND byte order is to be reversed then pre-position 'dest' at the
         high address and will count backwards.
      */
      if(numBits > 8 && dir == _reverse)
         { src += numBits/8; }
   #else
      #warning "bit64K_Out() Endian undefined - bytes will always be copied no-reverse."
      #define _bump(ptr)  (ptr)++
   #endif // __BYTE_ORDER__

   for(bit64K_T_Cnt rem = numBits; rem > 0; dest += _8bits, _bump(src))
   {
      /* If there's a port.rdDest() then will read current destination byte (into 'db'). Then clear
         and fill 'open' bits in 'db' from 'msb' rightward. If 'rem' > 'msb' this will be fully
         'msb' to b0.
            If no port.rdDest() then just zero 'db'.
      */
      U8 msb = _bit(dest);                                     // This is the msbit of the 'dest' field..
      U8 open = rem > msb+1 ? msb+1 : rem;                     // Will clear these many bits.

      bit64K_T_ByteOfs destAt = _byte(dest);

      U8 db;
      if(port->rdDest == NULL) {                               // No rdDest()?
         db = 0; }                                             // then start with dest byte clear.
      else {
         if( false == port->rdDest(&db, destAt, 1))            // Get 'dest' into 'db'.
            { return false; }

         CLRB(db, mask(msb, open)); }                          // Clear 'open' bits from 'msb' downward.

      U8 sb = shiftLR(*src, msb-_msb7);                        // Shift left or right to align the field from 'src' with 'dest'.
      db = orInto(db, sb, mask(msb, open));                    // Mask to select just that field and OR into dest byte.

      if(false == port->wrDest(destAt, &db, 1))                // Destination byte updated; put it back.
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

         if(port->rdDest == NULL) {                            // No rdDest()?
            db = 0; }                                          // then start with dest byte all-cleared.
         else {
            if( false == port->rdDest(&db, destAt+1, 1))       // else get destination
               { return false; }
            else
               { CLRB(db, mask(_msb7, write)); }}              // Clear 'open' bits from 'msb'.

         sb = rotL(*src, open);                                // Rotate partial field left to align with destination slot.
         db = orInto(db, sb, mask(_msb7, write));              // Mask to select just that field and OR into dest byte.

         if(false == port->wrDest(destAt+1, &db, 1))           // Destination byte updated; put it back.
            { return false; }
      }
   } // for(U8 rem = numBits...
   return true;

}

// ---------------------------------- eof --------------------------------  -
