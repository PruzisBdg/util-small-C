/*---------------------------------------------------------------------------
|
|  
|   
|--------------------------------------------------------------------------*/


#include "libs_support.h"
#include "util.h"
#include "arith.h"

/* ------------------------------------- TblSearchSARAscend_KeyU16 -----------------------------------------

   Find a match for 'keyToMatch' in a 'tbl' of records indexed by a U16 key. Return a pointer to the 
   start of the matching record. Else 0 (null) if no match found.
   
   The key must be at the start of the record.
   
   For large tables this routine tries a SAR search first. A SAR search will always succeed if the 
   keys are in ASCENDING order (and, of course, if the key is actually in the table). If the SAR
   search won't terminate it means that keys are out of order. Then the routine tries a linear search.
   
   So, for large tables, keys should preferably be in ascending order.
   
   Inputs:
      tbl            - start of the table
      numRecords     - 1 to 0xFFFF
      bytePerRecord  - the interval between records, 0 - 0xFFFF
      keyToMatch     - 
   
   Maximum table records = 0xFFFF; maximum record bytes = 0xFFFF.
*/

PUBLIC void const * TblSearchSAR_AscendKeyU16(void const *tbl, U16 numRecords, U8 bytesPerRec, U16 keyToMatch) {

   U16 idx, step;
   U8 loopCnt, maxLoop;
   void const *p;
   U16 key;

   if(  numRecords < 20 ) {                                    // Short table? then do linear search
      for( idx = 0, p = tbl; 
            idx < numRecords; idx++,
            p = (void const *)((uintptr_t)p + bytesPerRec) ) { // From table start, for each record
         if( *(U16*)p == keyToMatch ) {                        // Matched key?
            return p;                                          // then return record
            }
         }
      return (void const *)0;                                  // else no match - return null
      }
   else {                                                      // else longer table, do SAR search
      idx = numRecords/2;                                      // Start midway up the table.
      step = numRecords/4 + 1;                                 // First step will be 1/4 up or 1/4 down.
      loopCnt = 0;
   
      /* If keys are in order, A SAR search will terminate within log2(numRecords) loops. Will 
         exit the SAR search if more loops than this.
      */
      maxLoop = MSB_U16(numRecords) + 2;     // Add 2 as a cushion.                  

      while(1) {                                               // Until search terminates...
         p = (void const*)(uintptr_t)((uintptr_t)tbl + (idx * (U32)bytesPerRec)); // p <- start of record
         key = *(U16*)p;                                       // 'key' is the 1st field of the record.
      
         if( key == keyToMatch ) {                             // Matched 'keyToMatch'?
            return p;                                          // then we're done. Return start of the record
            }
         else if( ++loopCnt > maxLoop ) {                      // Too many loops? Means keys are out of order...
                                                               // ... so fall back to linear search
            for( idx = 0, p = tbl;                             // From table start, for each record
                  idx < numRecords; idx++,
                  p = (void const *)((uintptr_t)p + bytesPerRec) ) {
               if( *(U16*)p == keyToMatch ) {                  // Matched key?
                  return p;                                    // then return the record
                  }
               }
            return (void const *)0;                            // else no match; return null
            }
         else if( step == 1 &&                                 // else... Step size has dropped to minimum? AND
            (idx == 0 || idx >= numRecords-1) ) {              // we are at top or bottom of table?
            return (void const *)0;                            // then there's no match in the table; return fail.
            }
         else if( key > keyToMatch) {                          // else... Too far up the table?
            idx -= step;                                       // then step down
            }
         else if( key < keyToMatch ) {                         // else... Too far down the table?
            idx += step;                                       // then step up
            }
         if( step > 1 )                                        // Step size can shrink yet?
            { step /= 2; }                                     // then halve it for next go-around.
         }           // end: while(1)
      }           // end: else (do SAR search)
}

// --------------------------------------- eof --------------------------------------

