
#include "libs_support.h"
#include "util.h"
#include "tdd_common.h"

   #if _TARGET_IS == _TARGET_UNITY_TDD
#include "unity.h"
   #else
#define TEST_FAIL()
#define TEST_ASSERT_EQUAL_UINT8_MESSAGE(...)
#define TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(...)
#define TEST_FAIL_MESSAGE(...)
#define TEST_ASSERT_EQUAL_UINT8(...)
#define TEST_ASSERT_EQUAL_PTR(...)
   #endif // _TARGET_IS

/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    //srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

/* ---------------------------------- Interrupts -------------------------------------------

   The Si4xxx interface has interrupts grouped in bytes, Map from the bytes to these bit-fields.
*/
// Bits for all individual and group interrupts.
typedef union __attribute__((packed)) {
   U32   asU32;
         // Packet Group.
   struct { U32
         filtMatch      :1,   // Incoming packet matched filter.
         filtMiss       :1,   // Incoming packet did not match; was discarded.
         txDataSent     :1,   // Bytes in TX Fifo were sent (in a packet)
         gotPkt         :1,   // Expected number of bytes from incoming packetare in RX FIFO.
         crcErr         :1,   // Bad CRC on incoming packet.
         txFifoLow      :1,   // TX Fifo below low water mark
         rxFifoHigh     :1,   // RX Fifo above high water mark
         // Modem Group
         gotPostamble   :1,  // Postamble-detect was enabled and got postamble.
         noSync         :1,   // Sync-detect enabled and timeout waiting for sync.
         gotSync        :1,   // Sync-detect enabled and Got sync
         rssiJump       :1,   // RSSI jumped by > threshold.
         rssiGT         :1,   // RSSI above (static) threshold.
         noPreamble     :1,   // Timeout waiting for preamble.
         gotPreamble    :1,   // Got preamble.
         // Chip Group
         cal            :1,   // Doing cal
         fifoUO         :1,   // FIFO over/underflow
         newState       :1,   // Changed state
         badCmd         :1,   // Error processing command
         nowReady       :1,   // Si4xxx has completed power-up initialisation.
         lowBatt        :1,   // Below global low batt thold.
         wut            :1,   // Wakeup timer expired.

         pktGroup       :1,
         modemGroup     :1,
         chipGroup      :1; } flags;
} si4x_Irqs;

#define _Si4x_NumInterrupts      (21)
#define _Si4x_NumInterruptGroups (3)

// An IRQ callback gets the IRQ flags, clears any that it handles, and returns a reference to the (revised) flags.
typedef si4x_Irqs const * (*si4x_T_IrqCB)(si4x_Irqs *irqs);
#define _si4x_IrqCB(name) si4x_Irqs const * name(si4x_Irqs *irqs)

// ------------------------------- ends: Interrupts -------------------------------------


/* ---------------------------------- test_CBStack ---------------------------------------- */

void test_CBStack(void)
{
   // Make a Callbacks Stack, size = 2.
   cbStack_S cbs;
   bool rtn = cbStack_Init(&cbs, &(cbStack_Cfg const){.buf = (cbStack_T_Callback[2]){}, .size = 2} );
   // Succeeds, _Free() == 2.
   TEST_ASSERT_EQUAL_UINT8(true, rtn);
   TEST_ASSERT_EQUAL_UINT8(2, cbStack_Free(&cbs));

   // Try to make a non-zero sized CB Stack, but with a NULL buffer. Will fail.
   cbStack_S cbs2;
   rtn = cbStack_Init(&cbs, &(cbStack_Cfg const){.buf = NULL, .size = 2} );
   TEST_ASSERT_EQUAL_UINT8(false, rtn);

   U8 lowBattRunCnt, badCmdRunCnt, cnt3 = 0;

   _si4x_IrqCB(lowBatt) { lowBattRunCnt++; irqs->flags.lowBatt = 0; return irqs; }
   _si4x_IrqCB(badCmd)  { badCmdRunCnt++;  irqs->flags.badCmd = 0;  return irqs; }
   _si4x_IrqCB(cb3) { cnt3++; return irqs; }

   // Add one callback and run it.
   {
      // Add one callback.
      rtn = cbStack_Chain(&cbs, (cbStack_Irqs)lowBatt);
      // Succeeds and Free() 2 -> 1
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(1, cbStack_Free(&cbs));

      // Run callbacks with 2 interrupts, one for this callback, plus another.
      si4x_Irqs irqs = (si4x_Irqs){.flags.lowBatt = 1, .flags.badCmd = 1};
      si4x_Irqs const * iRtn = cbStack_Run(&cbs, &irqs);
      // cbStack_Run() should return'irqs' supplied to it.
      TEST_ASSERT_EQUAL_PTR(iRtn, &irqs);
      // lowBatt() was run once.
      TEST_ASSERT_EQUAL_UINT8(1,lowBattRunCnt);
      // 'lowBat' interrupt was cleared (by lowBatt())
      TEST_ASSERT_EQUAL_UINT8(0, iRtn->flags.lowBatt);
      // the other interrupt was not modified.
      TEST_ASSERT_EQUAL_UINT8(1, iRtn->flags.badCmd);
   }

   // Add a NULL callback. Nothing to add; succeeds in doing nothing.
   {
      rtn = cbStack_Chain(&cbs, NULL);
      // Succeeds; _Free() remains == 1
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(1, cbStack_Free(&cbs));
   }


   // Add lowBatt() again. Succeeds, but as it's already on the Stack it is not added again.
   {
      rtn = cbStack_Chain(&cbs, (cbStack_Irqs)lowBatt);
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      // _Free() is still == 1.
      TEST_ASSERT_EQUAL_UINT8(1, cbStack_Free(&cbs));
   }

   // Add another (different) callback. Run callbacks again.
   {
      rtn = cbStack_Chain(&cbs, (cbStack_Irqs)badCmd);
      // Succeeds; _Free() -> 0
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, cbStack_Free(&cbs));

      // Run callbacks with 2 interrupts, one for this callback, plus another.
      si4x_Irqs const * iRtn = cbStack_Run(&cbs, &(si4x_Irqs){.flags.lowBatt = 1, .flags.badCmd = 1});
      // Both lowBatt() and badCmd() were run once.
      TEST_ASSERT_EQUAL_UINT8(2,lowBattRunCnt);
      TEST_ASSERT_EQUAL_UINT8(1,badCmdRunCnt);
      // Both interrupts cleared by their handlers
      TEST_ASSERT_EQUAL_UINT8(0, iRtn->flags.lowBatt);
      TEST_ASSERT_EQUAL_UINT8(0, iRtn->flags.badCmd);
   }

   // (Try to) add a 3rd callback. No room on Stack; so should fail.
   {
      rtn = cbStack_Chain(&cbs, (cbStack_Irqs)cb3);
      // Fail; _Free() remains == 0
      TEST_ASSERT_EQUAL_UINT8(false, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, cbStack_Free(&cbs));

      // Existing callbacks work same as above.
      si4x_Irqs const * iRtn = cbStack_Run(&cbs, &(si4x_Irqs){.flags.lowBatt = 1, .flags.badCmd = 1});
      TEST_ASSERT_EQUAL_UINT8(3,lowBattRunCnt);
      TEST_ASSERT_EQUAL_UINT8(2,badCmdRunCnt);
      TEST_ASSERT_EQUAL_UINT8(0, iRtn->flags.lowBatt);
      TEST_ASSERT_EQUAL_UINT8(0, iRtn->flags.badCmd);
   }

   // Remove the 1st callback by name. 2nd should remain.
   {
      rtn = cbStack_UnChain(&cbs, (cbStack_Irqs)lowBatt);
      // Succeeds; _Free() -> 1
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(1, cbStack_Free(&cbs));
   }

   // Add lowBatt() back in. Stack has 2 CBs again.
   {
      rtn = cbStack_Chain(&cbs, (cbStack_Irqs)lowBatt);
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, cbStack_Free(&cbs));
   }

   // Try to remove a CB which isn't in the Stack. Says Success! but no change to Stack.
   {
      rtn = cbStack_UnChain(&cbs, (cbStack_Irqs)cb3);
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, cbStack_Free(&cbs));
   }

   // Try to remove a NULL CB. Says Success! but no change to Stack.
   {
      rtn = cbStack_UnChain(&cbs, NULL);
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(0, cbStack_Free(&cbs));
   }

   // Pop callbacks (remove in reverse order)
   {
      // This should remove lowBatt() which was the last-added above.
      rtn = cbStack_DropLast(&cbs);
      // Operation succeeds.
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      // Now there's one callback remaining and one Free().
      TEST_ASSERT_EQUAL_UINT8(1, cbStack_Free(&cbs));

      lowBattRunCnt = badCmdRunCnt = 0;      // Clear run counts for convenience.

      // Run callbacks; lowBatt() should be gone, badCmd() remaining.
      si4x_Irqs const * iRtn = cbStack_Run(&cbs, &(si4x_Irqs){.flags.lowBatt = 1, .flags.badCmd = 1});
      // lowBatt() did not run;
      TEST_ASSERT_EQUAL_UINT8(0,lowBattRunCnt);
      TEST_ASSERT_EQUAL_UINT8(1, iRtn->flags.lowBatt);
      // badCmd() is still on Stack and it did run.
      TEST_ASSERT_EQUAL_UINT8(1,badCmdRunCnt);
      TEST_ASSERT_EQUAL_UINT8(0, iRtn->flags.badCmd);

      // Pop again. This should remove badCmd(), leaving Stack empty.
      rtn = cbStack_DropLast(&cbs);
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      // Free() 1 -> 2 i.e Stack is now empty.
      TEST_ASSERT_EQUAL_UINT8(2, cbStack_Free(&cbs));

      // Finally, pop again on a empty Stack. This should succeed, doing nothing.
      rtn = cbStack_DropLast(&cbs);
      TEST_ASSERT_EQUAL_UINT8(true, rtn);
      TEST_ASSERT_EQUAL_UINT8(2, cbStack_Free(&cbs));
   }

}

// ============================================== EOF ==============================================
