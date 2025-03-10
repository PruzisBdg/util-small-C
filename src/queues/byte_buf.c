/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include "arith.h"

/*-----------------------------------------------------------------------------------------
|
|  copyOut()
|
------------------------------------------------------------------------------------------*/

PRIVATE void copyOut(S_byteBuf *b, U8 *out, U8 numBytes)
{
   U8 c;

   for( c = 0; c < numBytes; c++ )     // For each byte to read
   {
      out[c] = b->buf[b->get++];       // Copy to 'out'.
   }
}

/*-----------------------------------------------------------------------------------------
|
|  copyIn()
|
------------------------------------------------------------------------------------------*/

PRIVATE void copyIn(S_byteBuf *b, U8 const *src, U8 cnt)
{
   for(U8 c = 0; c < cnt; c++ )
   {
      b->buf[b->cnt++] = src[c];
   }
}

/*-----------------------------------------------------------------------------------------
|
|  copyInReversed()
|
------------------------------------------------------------------------------------------*/

PRIVATE void copyInReversed(S_byteBuf *b, U8 const *src, U8 cnt)
{
   for(U8 c = 0; c < cnt; c++ )
   {
      b->buf[b->cnt++] = src[cnt-c-1];
   }
}

/*-----------------------------------------------------------------------------------------
|
|  fillUp()
|
------------------------------------------------------------------------------------------*/

PRIVATE void fillUp(S_byteBuf *b, U8 n, U8 cnt)
{
   for(U8 c = 0; c < cnt; c++ )
   {
      b->buf[b->cnt++] = n;
   }
}

/*-----------------------------------------------------------------------------------------
|
|  moveUp()
|
------------------------------------------------------------------------------------------*/

PRIVATE void moveUp(S_byteBuf *b, U8 from, U8 to, U8 cnt)
{
   to += (cnt-1); from += (cnt-1);

   for(U8 c = 0; c < cnt; c++)
   {
      b->buf[to--] = b->buf[from--];
   }
}

/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Init()
|
------------------------------------------------------------------------------------------*/

PUBLIC void byteBuf_Init(S_byteBuf *b, U8 *buf, U8 size)
{
   b->buf = buf;
   b->size = size;
   byteBuf_Flush(b);
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Exists() s
|
------------------------------------------------------------------------------------------*/

PUBLIC BOOL byteBuf_Exists(S_byteBuf *b)
{
   return b->buf == NULL || b->size == 0 ? FALSE : TRUE;    // TRUE if non-zero-sized buf[].
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Write()
|
|  Write 'bytesToWrite' from 'src' to 'b'. Don't write any of 'src' unless all will fit.
|
|  Return 0 if the write didn't happen.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_Write(S_byteBuf *b, U8 const *src, U8 bytesToWrite)
{
   if(b->locked ||                           // Buffer locked?
      bytesToWrite > b->size - b->cnt )      // or not enough room?
   {
      return 0;                              // then can't do this write
   }
   else                                      // else we can proceed
   {
      b->locked = 1;                         // Lock it now, for duration of write
      copyIn(b, src, bytesToWrite);
      b->locked = 0;                         // and we're done; unlock the queue.
      return 1;                              // Return success
   }
}

/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Write_reversed()
|
|  Same as byteBuf_Write() but 'src[]' are copied in reverse order.
|  Write 'bytesToWrite' from 'src' to 'b'. Don't write any of 'src' unless all will fit.
|
|  Return 0 if the write didn't happen.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_Write_reversed(S_byteBuf *b, U8 const *src, U8 bytesToWrite)
{
   if(b->locked ||                           // Buffer locked?
      bytesToWrite > b->size - b->cnt )      // or not enough room?
   {
      return 0;                              // then can't do this write
   }
   else                                      // else we can proceed
   {
      b->locked = 1;                         // Lock it now, for duration of write
      copyInReversed(b, src, bytesToWrite);  // .... but reversed.
      b->locked = 0;                         // and we're done; unlock the queue.
      return 1;                              // Return success
   }
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Insert()
|
|  Write 'bytesToWrite' from 'src' to 'b' at index 'insertAt', moving up any bytes already
|  there. Don't write any of 'src' unless everything will fit (including any moved bytes).
|
|  If 'insertAt' is beyond the current 'put' then the gap between the existing data and
|  the inserted data is zero-filled.
|
|  Return 0 if the insert didn't happen.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_Insert(S_byteBuf *b, U8 const *src, U8 insertAt, U8 numBytes)
{
   if(b->locked ||                              // Buffer locked?
      (U16)numBytes + MaxU8(insertAt, b->cnt) > b->size )    // or not enough room?
   {
      return 0;                                 // then can't do this insertion
   }
   else                                         // else we can proceed
   {
      b->locked = 1;                            // Lock it now, for duration of write

      if(insertAt < b->cnt)                     // Must insert into existing data?
      {                                         // Move up data from the insertion point to make a gap.
         U8 afterInsert = b->cnt - insertAt;    // These many data bytes after the insertion point.
         moveUp(b, insertAt, insertAt + numBytes, afterInsert);
         b->cnt = insertAt;                     // Place 'put' here.
         copyIn(b, src, numBytes);              // Copy in bytes to be inserted.
         b->cnt += afterInsert;                 // 'put' beyond to past the last data byte.
      }
      else                                      // else insertion point is past existing data
      {
         fillUp(b, 0, b->cnt - insertAt);       // Zero fill from last data to the insertion point
         copyIn(b, src, numBytes);              // Append the bytes to insert.
      }

      b->locked = 0;                            // and we're done; unlock the queue.
      return 1;                                 // Return success
   }
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Read()
|
|  Read 'bytesToRead' from 'b' to 'dest'. Doesn't read any unless it can get them all
|
|  Return 0 if the read didn't happen.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_Read(S_byteBuf *b, U8 *dest, U8 bytesToRead )
{
   if(b->locked || b->get + bytesToRead > b->cnt )    // Buffer locked or not 'bytesToRead' bytes from 'get'?
   {
      return 0;                              // then can't do this read
   }
   else                                      // else we can proceed
   {
      b->locked = 1;                         // Lock it now, for duration of read
      copyOut(b, dest, bytesToRead);
      b->locked = 0;                         // and we're done; unlock the queue.
      return 1;                              // Return success
   }
}

/*-----------------------------------------------------------------------------------------
|
|  byteBuf_ReadAt()
|
|  Read 'bytesToRead' from 'b'['from'] to 'dest'. Doesn't read any unless it can get them all.
|
|  b->get is set to 'from', bytes are read out from there and 'get' is advanced. So to re-read
|  the buffer from the start, for example, do
|        byteBuf_ReadAt(b,dest,0,numBytes),
|  then zero or more
|        byteBuf_ReadAt(b,dest,numBytes)
|
|  Return 0 if the read didn't happen.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_ReadAt(S_byteBuf *b, U8 *dest, U8 from, U8 bytesToRead )
{
   if(b->locked || from + bytesToRead > b->cnt )    // Buffer locked or not 'bytesToRead' bytes from 'get'?
   {
      return 0;                              // then can't do this read
   }
   else                                      // else we can proceed
   {
      b->locked = 1;                         // Lock it now, for duration of read
      b->get = from;
      copyOut(b, dest, bytesToRead);
      b->locked = 0;                         // and we're done; unlock the queue.
      return 1;                              // Return success
   }
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Flush()
|
|  Force a flush on 'b', overriding any locks.
|
|  Note: If someone else has the queue, this can be dangerous.
|
------------------------------------------------------------------------------------------*/

PUBLIC void byteBuf_Flush(S_byteBuf *b)
{
   b->get = b->cnt = 0;
   b->locked = 0;
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Locked()
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_Locked(S_byteBuf *b)
{
   return b->locked;
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_PutAt()
|
|  Return the current 'put'.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 * byteBuf_PutAt(S_byteBuf *b)
{
   if(b->locked || b->cnt >= b->size)
   {
      return NULL;
   }
   else
   {
      return b->buf + b->cnt;
   }
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_ToFill()
|
|  Return the buffer for a naked fill of the queue. Preset indices and count for how the
|  buffer will be after the fill is done.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 * byteBuf_ToFill(S_byteBuf *b, U8 cnt)
{
   byteBuf_Flush(b);
   b->locked = 1;
   b->cnt = cnt;
   return b->buf;
}

/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Start()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 * byteBuf_Start(S_byteBuf *b)
{
   return b->buf;
}
/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Reserve()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 * byteBuf_Reserve(S_byteBuf *b, U8 cnt)
{
   if(b->locked || cnt > b->size - b->cnt )  // Buffer locked or there are not 'cnt' bytes free?
   {
      return NULL;                           // then can't reserve 'cnt' bytes.
   }
   else                                      // else we can proceed
   {
      b->locked = 1;                         // Lock it now, for duration of reserve
      U8 reservedAt = b->cnt;
      b->cnt += cnt;                         // Advance the buffer count past what we reserved.
      b->locked = 0;                         // and we're done; unlock the queue.
      return b->buf + reservedAt;            // Return the reserved section.
   }
}

/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Unlock()
|
------------------------------------------------------------------------------------------*/

PUBLIC void byteBuf_Unlock(S_byteBuf *b)
{
   b->locked = 0;
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Count()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 byteBuf_Count(S_byteBuf *b)
{
   return b->cnt;
}

/*-----------------------------------------------------------------------------------------
|
|  byteBuf_ForcePut()
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_ForcePut(S_byteBuf *b, U8 newPut)
{
   if(b->locked || newPut >= b->size)     // locked? OR 'p' is beyond buffer.
   {
      return 0;                           // then fail.
   }
   else
   {
      b->cnt = newPut;                    // else apply 'newPut'
      return 1;                           // and success.
   }
}


/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Size()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 byteBuf_Size(S_byteBuf *b)
{
   return b->size;
}

/*-----------------------------------------------------------------------------------------
|
|  byteBuf_Free()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 byteBuf_Free(S_byteBuf *b)
{
   return b->size - b->cnt;
}


/* ----------------------------------------------------------------------------------------
|
|  byteBuf_TakeBack()
|
|  Take back i.e unwrite 'nBytes' bytes from 'b'. If necessary back up 'get' so it is never
|  past 'cnt' i.e you took back more bytes than there were to read and now there are exactly
|  zero bytes to read.
|
|  If 'nBytes' is more than the number of bytes in 'b' then empties 'b'
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteBuf_TakeBack(S_byteBuf *b, U8 nBytes)
{
   if(b->locked)
   {
      return 0;
   }
   else
   {
      b->locked = 1;
      b->cnt = nBytes >= b->cnt ? 0 : b->cnt - nBytes;   // Remove 'nBytes', up to emptying 'b'.
      if(b->get > b->cnt)                                // 'get' now past 'cnt' i.e 'put'
         {b->get = b->cnt;}                              // then back-up 'get' there are exactly 0 bytes to read.
      b->locked = 0;
      return 1;                                          // Always succeeds.
   }

}

// ------------------------------------ eof -------------------------------------------------
