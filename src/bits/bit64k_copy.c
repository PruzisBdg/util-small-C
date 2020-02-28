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
| Public:
|     bit64K_NewPort()
|     bit64K_ResetPort()
|     bit64K_FlushCache()
|     bit64K_ParmFitsField()
|     bit64K_In()
|     bit64K_Out()
|     bit64K_Copy()
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
#define _bitBE(bitAddr)       bit64K_BitBE(bitAddr)
#define _bitLE(bitAddr)       bit64K_BitLE(bitAddr)
#define _AminusBbits(a,b)     bit64K_AddBits(a, -(b))

// ------------------------- e.g mask(6,2) -> 0x60;  mask(4,3) -> 0x1C.
static U8 mask(U8 msb, U8 width) {
   return MakeAtoBSet_U8(msb, width > msb ? 0 : msb-width+1); }

static U8 maskAtoB(U8 msb, U8 lsb) {
   return MakeAtoBSet_U8(msb, lsb); }

// ----------------------------------------------------------------------------------------
static U8 orInto(U8 dest, U8 src, U8 srcMask) {
   return dest | (src & srcMask); }

/* ============================== Read Cache ==============================================

   To query multiple clustered items in a bit-address space without generating a (SPI) transaction
   for each read.
*/
#define _NoCacheAddr 0xFFFF      // This will not match any reasonable cache address.

static bool reloadCache(bit64K_Ports const *port, T_bit64K src, bit64K_T_Cnt bitsRem)
{
   if(port->cache == NULL) {                 // This port has no cache, actually?
      return true; }                         // then succeed in doing nothing.
   else {                                    // else continue.
      S_byteBuf *bb = &port->cache->q;
      U8 reqBytes = MinU16( (bitsRem+7)/8, byteBuf_Size(bb) );    // e.g 9 'bitsRem' require we read 2 'port' bytes

      /* To refill cache from 'port', get() from the port directly into the byteBuf_ buffer (which is
         returned by byteBuf_ToFill(). Normally this is unsafe way of filling byteBuf_. But we prechecked
         above that 'reqBytes' will not overfill the buffer.
      */
      if( port->src.get(byteBuf_ToFill(bb, reqBytes), _byte(src), reqBytes) == false) {  // Failed get() from 'port'?
         port->cache->atByte = _NoCacheAddr;                      // then cache state is unknown; Mark that we have nothing...
         byteBuf_Unlock(bb);                                      // Cache was locked before fill attempt. Unlock it.
         byteBuf_Flush(bb);                                       // Cache state is unknown - flush it!
         return false; }
      else {                                                      // else cache buffer was filled from 'port'
         port->cache->atByte = _byte(src);                        // Mark that cache start from the byte containing 'src'.
         byteBuf_Unlock(bb);                                      // Was locked before fill. Unlock it.
         return true; }
   }
}

// ----------------------------------------------------------------------------------------
static bool reloadCache_RdByte(bit64K_Ports const *port, U8 *out, T_bit64K src, bit64K_T_Cnt bitsRem)
{
   if( true == reloadCache(port, src, bitsRem) ) {
      byteBuf_Read(&port->cache->q, out, 1);                // Now return 1st byte from the (re)filled cache.
      return true; }
   return false;
}

// ----------------------------------------------------------------------------------------
static bool readFromCache(bit64K_Ports const *port, U8 *out, T_bit64K src, bit64K_T_Cnt bitsRem)
{
   if(_byte(src) >= port->cache->atByte) {                  // 'src' may be in this cache; address is high than cache start?
      S_byteBuf *bb = &port->cache->q;

      U16 ofs = _byte(src) - port->cache->atByte;           // Offset relative to cache start would be...

      if(ofs < byteBuf_Count(bb)) {                         // That offset is within the cache? so data is in there
         if( 1 == byteBuf_ReadAt(bb, out, ofs, 1)) {        // then (attempt to) read 1 byte from that cache?
            return true; }}}                                // Got that byte... we are done.

   return false;     // Some fail!
}

// ============================ ends: read cache =================================================


/* ----------------------------- first/nextSrcByte --------------------------------------------------

   Get next byte from 'port[src]' into 'out', either directly or via a cache . 'src' is a bit-address.

   If there's a cache and it's not empty then get the byte from there. If the cache is empty
   then refill the cache from 'port[src]' and return 1st byte from cache.

   It's up to the caller to advance 'src' by one byte for every byte read. That so the cache
   is not reloaded with same data as before.
*/
PRIVATE bool nextSrcByte(bit64K_Ports const *port, U8 *out, T_bit64K src, bit64K_T_Cnt bitsRem, bool firstRead)
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


/* ---------------------------- legalRange -------------------------------------------------

  Return true if [addr... addr+numBits] is inside the bit-address range 'r'.
  If r.min and r.max are both zero then there's no check; always return true.

  Note that 'min' == 'max' is a legal range, encompassing just 1 bit.
*/
static bool legalRange(bit64K_Range const *r, T_bit64K addr, bit64K_T_Cnt numBits) {
   return                                                   // 'addr' NOT legal if? ...
      (r->bits.min > 0 || r->bits.max > 0) &&               // Set a range for this 'port'? ...i.e 'min', 'max' not both zero, AND
      r->bits.max >= r->bits.min &&                         // that min/max pair is legal? AND
      (
         addr < r->bits.min ||                              // Low end of range below limit? OR
         (numBits > 0 && (addr+numBits) > (r->bits.max+1) ) // high end of range above upper limit?
      )
         ? false : true; }          // then out-of-range -> false.


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

PUBLIC bool bit64K_Copy(bit64K_Ports const *port, T_bit64K dest, T_bit64K src, bit64K_T_Cnt numBits)
{
   if( false == legalRange(&port->dest.range, dest, numBits) ||
       false == legalRange(&port->src.range, src, numBits))
      { return false; }
   else {
      U8 open;
      for(bit64K_T_Cnt rem = numBits; rem > 0; rem -= open, src += open, dest += open)
      {
         // rdDest() byte containing 'dest'. Clear those bits which will be updated.
         U8 db;
         U8 msb = _bitBE(dest);                             // This is the msbit of the 'dest' field..
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

         /* 's' has _bitBE(src)+1 bits from 'src'. Take some or all of these, depending on how
            much more we 'ask' for.
         */
         U8 got = MinU8(open, _bitBE(src)+1);                 // Take up to 'open' bits from 'src' forward to b0 of byte 't'

         /* Align the bits from 'src' (in 's')
         */
         sb = shiftLR(sb, msb-_bitBE(src));                   // Shift left or right to align the field from 'src' with 'dest'.
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
|  If 'dest' is NULL there's no copy; just reloads the read cache.
|  If no bits requested then do nothing.
|
|  If 'srcEndian' is opposite to the system __BYTE_ORDER__ then bytes from 'src' will be
|  copied into endian in reverse order (relative to ascending source address).
|     To copy bytes non-reversed regardless of __BYTE_ORDER__ set 'srcEndian' to 'eNoEndian'.
|
|  Returns 'false' if:
|     - 'numBits' is too large to fit in 'dest'
|           The size of 'dest' is set by 'port.src.maxOutBytes'. If 'maxOutBytes' is zero there's no size-check.
|     - 'src' is not a legal bit-address
|     - the read range 'src'...'src'+'numBits' is outside the source address range set on 'port'
|           Source address range is 'port.src.range.min ... max'. If both 'min' and 'max' are 0 the there's no range-check.
|     - port.src.get() fails.
|
------------------------------------------------------------------------------------------*/

#define _msb7  7
#define _8bits 8

#define _numBytesFrom(b)  (((b)+7)/8)

PUBLIC bool bit64K_Out(bit64K_Ports const *port, U8 *dest, T_bit64K src, bit64K_T_Cnt numBits, E_EndianIs srcEndian)
{
   if( false == legalRange(&port->src.range, src, numBits))    // 'src' (bit address) outside stated range?
      { return false; }                                        // then fail!
   else if(dest == NULL) {                                     // else no destination specified? ...
         return reloadCache(port, src, numBits); }             // ... is an instruction to (re)load the cache at 'src'.
   else if(numBits == 0) {                                     // else zero data requested?
      return true; }                                           // then, maybe reloaded cache, but do nothing else. Always true.
   else if(port->src.maxOutBytes > 0 &&                        // Test if 'numBits' will fit in 'dest'? AND...
           bit64K_Byte(numBits) > port->src.maxOutBytes)       // ...too many 'numBits' to fit in dest?
      { return false; }                                        // then fail!
   else {                                                      // else do a read.
      T_EndianPtr di;
      dest = EndianPtr_New(&di, dest, _numBytesFrom(numBits), srcEndian);     // Endian-aware 'src'.

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
         if(false == nextSrcByte(port, &sb, src, rem, is1st))  // Read 1st byte from 'port'? ..either directly or via cache
            { return false; }
         is1st = false;

         ask = rem > _8bits ? _8bits : rem;                    // This pass, from 1 or2 bytes, take (maybe all of) 'rem' but no more than 8 bits.
         U8 got = MinU8(ask, _bitBE(src)+1);                   // From the 1st byte take b[src:0], but no more than 'ask'.

         sb = shiftLR(sb, (S8)ask-_bitBE(src)-1);              // (Maybe) left-justify or right-justify the 'got' bits (from 1st byte) to their position determined by 'ask'.
                                                               // ...any space to the right of these to b0 will be filled from 2nd byte.
         U8 db = sb & mask(ask-1, got);                        // Mask to select just that field and OR into dest byte.

         if(ask > got)                                         // Got a partial destination field from 1st byte?
         {                                                     // then get 2nd byte into 'sb', while maybe refilling any cache.
            if(false == nextSrcByte(port, &sb, src+_8bits, rem-got, false)) // Read 2nd byte from 'port'? ..either directly or via cache
               { return false; }

            sb = rotL(sb, ask-got);                            // Rotate 2nd byte to fit in
            db = orInto(db, sb, maskAtoB(ask-got-1,0));        // Mask to select just that field and OR into dest byte.
         }
         *dest = db;                                           // Write out dest byte.
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
|  If 'srcIsEndian' == true AND 'destEndian' is opposite to the system __BYTE_ORDER__ AND
|  multiple bytes are to be copied from 'src' then these bytes will be reversed from 'src'
|  into 'dest'.
|
------------------------------------------------------------------------------------------*/

PUBLIC bool bit64K_In(bit64K_Ports const *port, T_bit64K dest, U8 const *src, bit64K_T_Cnt numBits, E_EndianIs destEndian, bool srcIsEndian)
{
	bool final_multi = false;	// Indicates the last byte of the source
	U8 extra_byte = 0;			// Extra byte used to add byte when 24 bit number
	
   if( false == legalRange(&port->dest.range, dest, numBits))     // 'dest' range not legal?
      { return false; }                                           // then fail!
   else if(numBits == 0) {                                        // else nothing to write?
      return true; }                                              // then succeed in doing nothing
   else {                                                         // else continue.
      /* If 'srcIsEndian' == true then make and endian aware point which reverses 'src' into
         dest if they are different endians. Otherwise 'src' just counts up (into dest).
      */ 	 
	 
	 // Left justifies source based on number of bytes and position of first bit
	 if(destEndian == eBigEndian && srcIsEndian) { 	 
		  U8 * temp_src;			// Used as temp source pointer after left justification		    
	  
		  if((_numBytesFrom(numBits) == 2)) {
			  U16 source_number16 = src[0] | (U16)src[1] << 8;			 // Change to original int to allow bit shift
		  
			  if(numBits % 8 != 0)
				source_number16 = source_number16 << (8-(numBits % 8));  // Left justify based on num of bits
		  
			  temp_src = source_number16;
			  src = &temp_src;
		  }
	  
		  else if ((_numBytesFrom(numBits) == 3))
		  {
		  
			  U32 source_number24 = src[0] | (U32)src[1] << 8 | (U32)src[2] << 16;
			  if(numBits % 8 != 0)
				{source_number24 = source_number24 << (16-(numBits % 8));}
			  else
				{source_number24 = source_number24 << 8;}
			
			  temp_src = source_number24;			    
			  src = &temp_src;
			  extra_byte++;												 // Add byte to account for extra 8 bit shift to get number left justified
		  }
	 
		 else if ((_numBytesFrom(numBits) == 4)) {		 
			 U32 source_number32 = src[0] | (U32)src[1] << 8 | (U32)src[2] << 16 | (U32)src[3] << 24;
			 if(numBits % 8 != 0)
				{source_number32 = source_number32 << (8-(numBits % 8));}
					
			 temp_src = source_number32;
			 src = &temp_src;
		 }	 
	}
      T_EndianPtr si;
      src = EndianPtr_New(&si, src, _numBytesFrom(numBits) + extra_byte,
                  srcIsEndian == true ? destEndian : eNoEndian);  // Endian-aware 'src'. 

      for(bit64K_T_Cnt rem = numBits; rem > 0; dest += _8bits, src = si.next(&si))
      {
         /* If there's a port.rdDest() then will read current destination byte (into 'db'). Then clear
            and fill 'open' bits in 'db' from 'msb' rightward. If 'rem' > 'msb' this will be fully
            'msb' to b0.
               If no port.rdDest() then just zero 'db'.
         */

         /* 'dest' is a bit-address in the endianess of the target. Extract the absolute
            bit position (in a byte) of the 'dest'.
         */
         U8 msb, open;
         if(destEndian == eBigEndian) {
            msb = _bitBE(dest);
            open = rem > msb+1 ? msb+1 : rem;
         }
         else {
            U8 lsb = _bitLE(dest);
            msb = MinU8(_msb7, lsb+rem-1);
            open = rem > _8bits-lsb ? _8bits-lsb : rem;
         }
         bit64K_atByte destAt = _byte(dest);

         U8 db;
         // Read the destination byte and mask out to bits to be updated. However, don't bother if the
         // whole byte will be overwritten.
         if(port->dest.rd == NULL || open == _8bits) {            // No rdDest()? OR will write the whole byte?
            db = 0; }                                             // then start with dest byte clear.
         else {
            if( false == port->dest.rd(&db, destAt, 1))           // Get 'dest' into 'db'.
               { return false; }
            CLRB(db, mask(msb, open)); }                          // Clear 'open' bits from 'msb' downward.

         // Align the left (msbit) of the source field with left of dest.
         U8 sb;
         if(destEndian == eBigEndian & !final_multi) {
            sb = shiftLR(*src, msb+1-MinU8(_8bits, rem)); }		  // This accounts for shift when one byte and not big endian dependent
		 else if(destEndian == eBigEndian & final_multi) {
		    sb = shiftLR(*src, msb+1-_8bits); }					  // Used when source is big endian dependent, last case should also shift by same amount as before
         else {
            sb = shiftLR(*src, msb+1-open); }                     // Align the left (msbit) of the source field with left of dest.

          db = orInto(db, sb, mask(msb, open));                   // Mask to select just that field and OR into dest byte.

         if(false == port->dest.wr(destAt, &db, 1))               // Destination byte updated; put it back.
            { return false; }

         rem -= open;                                             // These many bits remaining.

         // If copied to 'destAt' just part of the byte read from 'src' then must copy the remainder to
         // 'destAt' + 1, left justified for big-endian; right-justified for little-endian.
         if(open < _8bits && rem > 0)                             // Used just part of 'src' byte? AND bits left to transfer?
         {
            U8 write =  _8bits - open;							  // Bits remaining in the byte read from 'src'                                                          

            if(rem < write) {                                     // Only some of those remaining bits needed to complete transfer?
               write = rem;                                       // then will transfer just what we need (to complete)
               rem = 0; }                                         // and after that the whole transfer will be done.
			else {                                                // else will use all remaining bits from 'src'.
               rem -= write; }                                    // Zero or more bits left to complete whole transfer.

            // Read the destination byte and mask out to bits to be updated. However, don't bother if the
            // whole byte will be overwritten.
            if(port->dest.rd == NULL || write == _8bits) {        // No rdDest()? OR will write the whole byte?
               db = 0; }                                          // then start with dest byte all-cleared.
            else {
               if( false == port->dest.rd(&db, destAt+1, 1))      // else get destination
                  { return false; }
               else {
                  if(destEndian == eBigEndian)
                     { CLRB(db, mask(_msb7, write)); }
                  else
                     { CLRB(db, mask(write-1, write)); }
               }}              // Clear 'open' bits from 'msb'.

            if(destEndian == eBigEndian & !final_multi) {
               sb = shiftLR(*src, _8bits-write);                  // Shift partial field left to align with destination slot.
               db = orInto(db, sb, mask(_msb7, write)); }         // Mask to select just that field and OR into dest byte.
			else if(destEndian == eBigEndian & final_multi) {
				sb = shiftLR(*src, open);						  // Shift partial field left to align with destination slot.
				db = orInto(db, sb, mask(_msb7, write)); }        // Mask to select just that field and OR into dest byte.
			else {
				 sb = shiftLR(*src, -open);                        // Shift partial field left to align with destination slot.
				 db = orInto(db, sb, mask(write-1, write)); }       // Mask to select just that field and OR into dest byte.

            if(false == port->dest.wr(destAt+1, &db, 1))          // Destination byte updated; put it back.
               { return false; }
		    if(rem <= _8bits & srcIsEndian)						  // Used for tail handling when is endian
				{final_multi = true;}
         }
      } // for(U8 rem = numBits...
      return true;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  bit64K_NewPort()
|
------------------------------------------------------------------------------------------*/

PUBLIC bool bit64K_NewPort(bit64K_Ports *p, bit64K_Cache *cache, U8 *cacheBuf, U8 cacheBytes)
{
   if(cache == NULL || cacheBuf == NULL || cacheBytes == 0) { // No cache, no buffer supplied?
      p->cache = NULL; }                                    // then say 'no cache'.
   else {
      p->cache = cache;                                     // else attach the cache.
      byteBuf_Init( &p->cache->q, cacheBuf, cacheBytes);    // and attach buffer to cache, which starts flushed
      p->cache->atByte = _NoCacheAddr; }
   return true;
}

/*-----------------------------------------------------------------------------------------
|
|  bit64K_FlushCache()
|
------------------------------------------------------------------------------------------*/

PUBLIC bool bit64K_FlushCache(bit64K_Ports *p)
{
   if( p->cache != NULL ) {                  // There's a cache?
      byteBuf_Flush(&p->cache->q);           // Flush it!
      p->cache->atByte = _NoCacheAddr; }     // and the cache now does not point to any source address.
   return true;      // Return true even if there's no cache.
}

/*-----------------------------------------------------------------------------------------
|
|  bit64K_ResetPort()
|
------------------------------------------------------------------------------------------*/

PUBLIC bool bit64K_ResetPort(bit64K_Ports *p)
{
   if( p->cache != NULL ) {
      byteBuf_Flush(&p->cache->q);
      p->cache->atByte = _NoCacheAddr;
      }
   return true;
}

/*-----------------------------------------------------------------------------------------
|
|  bit64K_ParmFitsField()
|
|  Return true if value in 'numBytes' from 'parm' will fit in 'fieldBits'. If 'parmHasEndian'
|  then apply endian conversion if necessary.
|
------------------------------------------------------------------------------------------*/

static bool bitsFit(U8 n, U8 nbits) {
   return (n & ~MakeAtoBSet_U8(nbits-1, 0)) == 0 ? true : false; }

PUBLIC bool bit64K_ParmFitsField(U8 const *parm, U8 parmBytes, bit64K_T_Cnt fieldBits, bool parmHasEndian)
{
   if(fieldBits == 0 || parmBytes == 0  ||               // No destination field? OR no source bytes? OR
      _8bits * fieldBits == parmBytes ||                 // field is exactly 'parmBytes'? OR
      _numBytesFrom(fieldBits) > parmBytes ) {           // More field bytes than parm bytes?
      return true; }                                     // then succeed.
   else if(parmBytes > (fieldBits+7)/_8bits) {           // Too many 'parmBytes' for destination field?
      return false; }                                    // then always fail, regardless of source content.
   if(fieldBits <= 8 && parmBytes == 1) {                // Just 1 byte?
      return bitsFit(parm[0], fieldBits); }              // then test just that byte
   else {                                                // else check the msb of the parm fits top field bits.
      // Choose the msb.
      #ifdef __BYTE_ORDER__
         #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            U8 msb = parmHasEndian == true ? parm[parmBytes-1]: parm[0];
         #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            U8 msb = parm[0];
         #else
            U8 msb = parm[0];
         #endif

      #else
         U8 msb = parm[0];
         #warning "bit64K_Out() Endian undefined - bytes will always be copied no-reverse."
      #endif // __BYTE_ORDER__

      return bitsFit(msb, fieldBits % _8bits); 
	  //return true;
	  }
}

// ---------------------------------- eof --------------------------------  -
