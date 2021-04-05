#include "libs_support.h"
   #if _TARGET_IS == _TARGET_UNITY_TDD
#include "unity.h"
   #else
#define TEST_FAIL()
#define TEST_FAIL_MESSAGE(m)   printf(m)
#define TEST_ASSERT_TRUE(...)  if(false == (__VA_ARGS__)) { printf("false at %s %u\r\n", __FILE__, __LINE__);}
#define TEST_ASSERT_TRUE_MESSAGE(...) __VA_ARGS__
#define TEST_ASSERT_EQUAL_UINT8(...)
#define TEST_ASSERT_EQUAL_UINT8_MESSAGE(...)
#define TEST_ASSERT_EQUAL_UINT32(...)
#define TEST_ASSERT_FALSE(...)
#define TEST_ASSERT_EQUAL_MEMORY(...)
#define TEST_ASSERT_EQUAL_STRING(...)
#define TEST_ASSERT_EQUAL_PTR(...)
#define TEST_ASSERT_EQUAL_UINT16(...)
#define TEST_ASSERT_EQUAL_INT8(...)
   #endif // _TARGET_IS


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "tdd_common.h"
#include "util.h"
#include <malloc.h>

PRIVATE heapLog_S heapLog;

/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
    heapLog_Make(&heapLog);
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
   heapLog_Delete(&heapLog);
}

// ----- Malloc() and log that malloc() -------------------------------------------------------
PUBLIC void * heapStr_TDDMalloc(size_t n) {
   return heapLog_Malloc(&heapLog, n); }

// ------ Free() and log that free() -----------------------------------------------------------
PUBLIC void heapStr_TDDFree(void *p) {
   heapLog_Free(&heapLog, p); }

/* ---------------------------- test_New_then_Delete ------------------------------------------ */
void test_New_then_Delete(void)
{
   S_HeapStr * s = heapStr_New(100);                        // New Holder.

   bool rtn = heapStr_Write(s, "Steven");                   // Write it
   TEST_ASSERT_EQUAL_UINT8(true, rtn);                      // Write succeeds
   TEST_ASSERT_EQUAL_STRING( heapStr_Ref(s), "Steven");     // And this is what's in there.

   rtn = heapStr_Delete(&s);                                // Delete it.
   TEST_ASSERT_EQUAL_UINT8(true, rtn);                      // Which succeeds.
   TEST_ASSERT_EQUAL_STRING( "", heapStr_Ref(s));           // Detached Holder now references "".

   rtn = heapStr_Write(s, "Cerium");                        // Try to write the deleted reference.
   TEST_ASSERT_EQUAL_UINT8(false, rtn);                     // The write attempt should fail.

   //printf("%s\r\n", heapLog_Report( (C8[100]){}, NULL, &heapLog));

   TEST_ASSERT_EQUAL_UINT16(heapLog.mallocs.put, 2);        // two malloc()s, one for string, one for it's Holder.
   TEST_ASSERT_EQUAL_UINT8(true, heapLog_Match(&heapLog));  // Free()s match mallocs()?
}

/* ----------------------------------- test_Renew ------------------------------------------ */
void test_Renew(void)
{
   // An empty Reference made on Stack in undefined state.
   S_HeapStr * s0 = NULL;

   S_HeapStr *s1 = heapStr_Renew(&s0, 10);                  // New Holder (using _Renew())
   TEST_ASSERT_TRUE(s1 != NULL);                            // Actually made a Holder (is non-NULL)
   TEST_ASSERT_EQUAL_PTR(s0, s1);                           // Return and input reference the same new Holder.

   bool rtn = heapStr_Write(s1, "Steven");                  // Write it
   TEST_ASSERT_EQUAL_UINT8(rtn, true);                      // Write succeeds
   TEST_ASSERT_EQUAL_STRING( heapStr_Ref(s1), "Steven");    // And this is what's in there.

   S_HeapStr * s2 = heapStr_Renew(&s1, 10);                 // Ask for a new reference same size as the existing one.
   TEST_ASSERT_EQUAL_PTR(s1, s2);                           // Renew() will reuse the existing reference.

   S_HeapStr * s3 = heapStr_Renew(&s1, 20);                 // Ask for a new reference larger than the existing one.
   rtn = heapStr_Write(s3, "Waynes World");                 // Fill it with more then 10 bytes.
   TEST_ASSERT_EQUAL_UINT8(rtn, true);                      // Fill succeeds
   TEST_ASSERT_EQUAL_STRING( heapStr_Ref(s3), "Waynes World");    // And this is what's in there.

   rtn = heapStr_Delete(&s3);                               // Delete it.
   TEST_ASSERT_EQUAL_UINT8(rtn, true);                      // Which succeeds.
   TEST_ASSERT_EQUAL_STRING( "", heapStr_Ref(s3));          // Detached Holder now references "".

   TEST_ASSERT_EQUAL_UINT16(heapLog.mallocs.put, 4);
   TEST_ASSERT_EQUAL_UINT8(true, heapLog_Match(&heapLog));  // Deleted above so free()s0 match mallocs()?
}

/* ----------------------------------- test_Read ------------------------------------------ */
void test_Read(void)
{
   S_HeapStr * s = heapStr_New(30);                            // New Holder.
   bool rtn = heapStr_Write(s, "NinetyNine");                  // Fill it

   #define _BufSize 20

   // Read the whole string.
   C8 out[_BufSize] = {[0 ... _BufSize-1] = 0x5A};             // out[], prefilled with 0x5A
   C8 const *p = heapStr_Read(s, out, _BufSize-1);             // Copy to out[].
   TEST_ASSERT_EQUAL_PTR(p, out);                              // Always returns 'out[]'.
   TEST_ASSERT_EQUAL_STRING("NinetyNine", out);
   TEST_ASSERT_EQUAL_INT8(0x5A, out[sizeof("NinetyNine")]);    // Byte after end-of-string is untouched.

   // Read part of the string.
   memset(out, 0x5A, _BufSize);
   p = heapStr_Read(s, out, 5);                                // Request just 5 chars.
   TEST_ASSERT_EQUAL_PTR(p, out);                              // Always returns 'out[]'.
   TEST_ASSERT_EQUAL_STRING("Ninet", out);                     // Copied out just 5 chars (as a string)
   TEST_ASSERT_EQUAL_INT8(0x5A, out[sizeof("Ninet")]);         // Byte after end-of-string is untouched.

   // Delete the string then try reading it. Should get ""
   heapStr_Delete(&s);
   memset(out, 0x5A, _BufSize-1);
   p = heapStr_Read(s, out, 8);                                // Request some chars.
   TEST_ASSERT_EQUAL_PTR(p, out);                              // Always returns 'out[]'.
   TEST_ASSERT_EQUAL_STRING("", out);

   TEST_ASSERT_EQUAL_UINT8(true, heapLog_Match(&heapLog));     // Deleted 's' (above) so free()s == malloc()s
}

/* --------------------------------- test_OversizeWrite ------------------------------------------ */
void test_OversizeWrite(void)
{
   S_HeapStr * s = heapStr_New(10);                            // New Holder for just 10 chars.

   bool rtn = heapStr_Write(s, "PairOfAcesJacksHigh");         // Try to fill it with something larger.
   TEST_ASSERT_EQUAL_UINT8(true, rtn);                         // Fill does succeed
   TEST_ASSERT_EQUAL_STRING( "PairOfAces", heapStr_Ref(s));    // ...but only 10 chars.

   heapStr_Delete(&s);
   TEST_ASSERT_EQUAL_UINT8(true, heapLog_Match(&heapLog));     // // Deleted 's' (above) so free()s == malloc()s
}

/* --------------------------------- test_Take ------------------------------------------ */
void test_Take(void)
{
   S_HeapStr * s = heapStr_New(30);                            // New Holder.
   bool rtn = heapStr_Write(s, "NinetyNine");                  // Fill it

   #define _BufSize 20

   // Take the whole string; i.e copy it and then remove the source reference.
   C8 out[_BufSize] = {[0 ... _BufSize-1] = 0x5A};             // out[], prefilled with 0x5A
   C8 const *p = heapStr_Take(&s, out, _BufSize-1);            // Take to out[].
   TEST_ASSERT_EQUAL_UINT16(0, s->tag);                        // Tag should be zero (0), indicating 's' is gone.
   TEST_ASSERT_EQUAL_PTR(p, out);                              // Always returns 'out[]'.

   TEST_ASSERT_EQUAL_STRING("NinetyNine", out);
   TEST_ASSERT_EQUAL_INT8(0x5A, out[sizeof("NinetyNine")]);    // Byte after end-of-string is untouched.

   TEST_ASSERT_EQUAL_STRING( "", heapStr_Ref(s));              // Detached Holder now references "".

   TEST_ASSERT_EQUAL_UINT8(true, heapLog_Match(&heapLog));     // _Take() deleted so heap is empty; free()s matched mallocs().
}


/* --------------------------------- test_Append ------------------------------------------ */
void test_Append(void)
{
   S_HeapStr * s = heapStr_New(17);                      // New Holder, start empty i.e ""
   bool rtn = heapStr_Append(s, "Star");                            // Append, which should be same as write.
   TEST_ASSERT_EQUAL_UINT8(true, rtn);                   // Append succeeds
   TEST_ASSERT_EQUAL_STRING( "Star", heapStr_Ref(s));

   rtn = heapStr_Append(s, "-Crossed");
   TEST_ASSERT_EQUAL_UINT8(true, rtn);                      // Write succeeds
   TEST_ASSERT_EQUAL_STRING( "Star-Crossed", heapStr_Ref(s));     // And this is what's in there.


   rtn = heapStr_Append(s, " Lovers");
   TEST_ASSERT_EQUAL_UINT8(true, rtn);                      // Write succeeds
   TEST_ASSERT_EQUAL_STRING( "Star-Crossed Love", heapStr_Ref(s));     // And this is what's in there.

}

// ----------------------------------------- eof --------------------------------------------
