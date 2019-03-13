/*---------------------------------------------------------------------------
|
|
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
|  byteQ_Init()
|
------------------------------------------------------------------------------------------*/

PUBLIC void byteQ_Init(S_byteQ *q, U8 *buf, U8 size)
{
   q->buf = buf;
   q->size = size;
   byteQ_Flush(q);
}


/*-----------------------------------------------------------------------------------------
|
|  byteQ_Exists() s
|
------------------------------------------------------------------------------------------*/

PUBLIC BOOL byteQ_Exists(S_byteQ *q)
{
   return q->buf == NULL || q->size == 0 ? FALSE : TRUE;    // TRUE if non-zero-sized buf[].
}


/*-----------------------------------------------------------------------------------------
|
|  byteQ_Write()
|
|  Write 'bytesToWrite' from 'src' to 'q'. Don't write any of 'src' unless all will fit.
|
|  Return 0 if the write didn't happen.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteQ_Write(S_byteQ *q, U8 const *src, U8 bytesToWrite)
{
   U8 c;

   if(q->locked ||                           // Queue locked?
      bytesToWrite > q->size - q->cnt )      // or not enough room?
   {
      return 0;                              // then can't do this write
   }
   else                                      // else we can proceed
   {
      q->locked = 1;                         // Lock it now, for duration of write

      for( c = 0; c < bytesToWrite; c++ )    // For each byte to write
      {
         q->buf[q->put++] = src[c];          // Write that byte
         if(q->put >= q->size)               // and bump/wrap the put ptr.
            { q->put = 0; }
      }
      q->cnt += bytesToWrite;                // Update the queue count
      q->locked = 0;                         // and we're done; unlock the queue.
      return 1;                              // Return success
   }
}


/*-----------------------------------------------------------------------------------------
|
|  byteQ_Read()
|
|  Read 'bytesToRead' from 'q' to 'dest'. Doesn't read any unless it can get them all
|
|  Return 0 if the read didn't happen.
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteQ_Read(S_byteQ *q, U8 *dest, U8 bytesToRead )
{
   U8 c;

   if(q->locked || bytesToRead > q->cnt )    // Queue locked or not enough room?
   {
      return 0;                              // then can't do this write
   }
   else                                      // else we can proceed
   {
      q->locked = 1;                         // Lock it now, for duration of write

      for( c = 0; c < bytesToRead; c++ )     // For each byte to write
      {
         dest[c] = q->buf[q->get++];         // Write that byte
         if(q->get >= q->size)               // and bump/wrap the put ptr.
            { q->get = 0; }
      }
      q->cnt -= bytesToRead;                 // Update the queue count
      q->locked = 0;                         // and we're done; unlock the queue.
      return 1;                              // Return success
   }
}


/*-----------------------------------------------------------------------------------------
|
|  byteQ_Flush()
|
|  Force a flush on 'q', overriding any locks.
|
|  Note: If someone else has the queue, this can be dangerous.
|
------------------------------------------------------------------------------------------*/

PUBLIC void byteQ_Flush(S_byteQ *q)
{
   q->get = q->put = q->cnt = 0;
   q->locked = 0;
}


/*-----------------------------------------------------------------------------------------
|
|  byteQ_Locked()
|
------------------------------------------------------------------------------------------*/

PUBLIC BIT byteQ_Locked(S_byteQ *q)
{
   return q->locked;
}


/*-----------------------------------------------------------------------------------------
|
|  byteQ_ToFill()
|
|  Return the buffer for a naked fill of the queue.
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 * byteQ_ToFill(S_byteQ *q, U8 cnt)
{
   byteQ_Flush(q);
   q->locked = 1;
   q->cnt = cnt;
   return q->buf;
}

/*-----------------------------------------------------------------------------------------
|
|  byteQ_Unlock()
|
------------------------------------------------------------------------------------------*/

PUBLIC void byteQ_Unlock(S_byteQ *q)
{
   q->locked = 0;
}


/*-----------------------------------------------------------------------------------------
|
|  byteQ_Count()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 byteQ_Count ( S_byteQ *q)
{
   return q->cnt;
}

/*-----------------------------------------------------------------------------------------
|
|  byteQ_Size()
|
------------------------------------------------------------------------------------------*/

PUBLIC U8 byteQ_Size  ( S_byteQ *q)
{
   return q->size;
}

// ------------------------------------ eof -------------------------------------------------
