#include "unity.h"
#include <string.h>
#include "tdd_common.h"
#include "util.h"

/* =============================== Tests start here ==================================

   Tests the functions in src/printfs/print_S_BufC8.c:
   Print_BufC8(), Chain_BufC8(), CpyTail_BufC8(), Strlen_BufC8(), Strcpy_BufC8(),
   Strcat_BufC8(), Strcpy_ChainBufC8().
*/

/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}


// ================================ Print_BufC8() ================================================

/* --------------------------------- test_Print_Basic ------------------------------------------ */
void test_Print_Basic(void)
{
   C8 buf[20];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 out = {.cs = buf, .cnt = sizeof(buf)-1};

   S_BufC8 const *r = Print_BufC8(&out, "Val=%d", 42);

   TEST_ASSERT_EQUAL_PTR(&out, r);                             // Always returns 'out'.
   TEST_ASSERT_EQUAL_STRING("Val=42", buf);
   TEST_ASSERT_EQUAL_UINT16(strlen("Val=42"), out.cnt);        // 'cnt' -> chars actually printed.
   TEST_ASSERT_EQUAL_UINT8(0x5A, buf[strlen("Val=42")+1]);     // Untouched, past the '\0'.
}

/* --------------------------------- test_Print_Empty ------------------------------------------- */
void test_Print_Empty(void)
{
   C8 buf[10];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 out = {.cs = buf, .cnt = sizeof(buf)-1};

   Print_BufC8(&out, "");

   TEST_ASSERT_EQUAL_STRING("", buf);
   TEST_ASSERT_EQUAL_UINT16(0, out.cnt);
}

/* --------------------------------- test_Print_ZeroSpace ---------------------------------------- */
void test_Print_ZeroSpace(void)
{
   C8 buf[10];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 out = {.cs = buf, .cnt = 0};        // No room at all.

   Print_BufC8(&out, "abc");

   TEST_ASSERT_EQUAL_UINT16(0, out.cnt);
   TEST_ASSERT_EQUAL_UINT8(0x5A, buf[0]);      // Untouched; vsnprintf() with size==0 writes nothing.
}

/* --------------------------------- test_Print_Truncates -----------------------------------------

   NOTE: when the formatted text is >= the space available, 'out->cnt' comes back ONE more than
   the number of chars which actually land in 'out->cs'. Here 'cnt' is 5 on entry, so vsnprintf()
   (called with size==5) writes only 4 chars + '\0'; but 'out->cnt' ends up 5, which contradicts
   the function header ("'out->cnt' [is] the number of chars actually printed"). This test
   documents the code's ACTUAL, current behaviour; it looks like a minor pre-existing bug (one
   byte of 'out's capacity is never usable, and 'cnt' overstates the true string length by 1
   whenever truncation happens) but is out of scope to fix here.
*/
void test_Print_Truncates(void)
{
   C8 buf[10];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 out = {.cs = buf, .cnt = 5};

   Print_BufC8(&out, "%s", "123456");          // 6 chars asked for; only 5 chars-space.

   TEST_ASSERT_EQUAL_STRING("1234", buf);      // Only 4 chars + '\0' actually fit.
   TEST_ASSERT_EQUAL_UINT16(5, out.cnt);       // But 'cnt' reports 5.
}


// ================================ Chain_BufC8() =================================================

/* --------------------------------- test_Chain_Basic -------------------------------------------- */
void test_Chain_Basic(void)
{
   C8 buf[30];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 out = {.cs = buf, .cnt = sizeof(buf)-1};

   Chain_BufC8(&out, "AAA");
   TEST_ASSERT_EQUAL_STRING("AAA", buf);
   TEST_ASSERT_EQUAL_PTR(buf+3, out.cs);
   TEST_ASSERT_EQUAL_UINT16(sizeof(buf)-1-3, out.cnt);

   Chain_BufC8(&out, "-%d", 7);
   TEST_ASSERT_EQUAL_STRING("AAA-7", buf);
   TEST_ASSERT_EQUAL_PTR(buf+5, out.cs);
   TEST_ASSERT_EQUAL_UINT16(sizeof(buf)-1-5, out.cnt);
}

/* --------------------------------- test_Chain_NullInsertsSeparator ------------------------------

   fmt == NULL appends just a '\0' and advances 'cs' past it. Chaining a fresh string after that
   leaves the earlier string still properly '\0'-terminated -- i.e the two are kept distinct in
   the shared buffer, rather than the 2nd overwriting the 1st's terminator.
*/
void test_Chain_NullInsertsSeparator(void)
{
   C8 buf[30];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 out = {.cs = buf, .cnt = sizeof(buf)-1};

   Chain_BufC8(&out, "AAA");
   Chain_BufC8(&out, NULL);
   Chain_BufC8(&out, "BBB");

   TEST_ASSERT_EQUAL_STRING("AAA", buf);         // 1st string, still properly terminated.
   TEST_ASSERT_EQUAL_STRING("BBB", buf+4);       // 2nd string, chained after the separator.
}

/* --------------------------------- test_Chain_NullAtZeroSpace ----------------------------------- */
void test_Chain_NullAtZeroSpace(void)
{
   C8 buf[4] = "AAA";
   S_BufC8 out = {.cs = buf+3, .cnt = 0};        // No space left.

   Chain_BufC8(&out, NULL);

   TEST_ASSERT_EQUAL_PTR(buf+3, out.cs);         // Unchanged; no space to add even 1 char.
   TEST_ASSERT_EQUAL_UINT16(0, out.cnt);
}

/* --------------------------------- test_Chain_Truncates ------------------------------------------

   Same 'cnt' quirk as Print_BufC8() (see test_Print_Truncates); shown here as it also makes
   Chain_BufC8() advance 'out->cs' one byte past the '\0' it actually wrote.
*/
void test_Chain_Truncates(void)
{
   C8 buf[10];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 out = {.cs = buf, .cnt = 5};

   Chain_BufC8(&out, "%s", "123456");

   TEST_ASSERT_EQUAL_STRING("1234", buf);        // Only 4 chars + '\0' actually written.
   TEST_ASSERT_EQUAL_PTR(buf+5, out.cs);         // 'cs' advanced past the '\0' (at buf+4) to buf+5.
   TEST_ASSERT_EQUAL_UINT16(0, out.cnt);
}


// ================================ CpyTail_BufC8() ================================================

/* --------------------------------- test_CpyTail_Found -------------------------------------------- */
void test_CpyTail_Found(void)
{
   C8 const srcStr[] = "HelloWorldFoo";
   S_BufC8_ro src = {.cs = srcStr, .cnt = (U16)strlen(srcStr)};

   C8 destBuf[20];
   memset(destBuf, 0x5A, sizeof(destBuf));
   S_BufC8 dest = {.cs = destBuf, .cnt = sizeof(destBuf)-1};

   S_BufC8 const *r = CpyTail_BufC8(&dest, &src, "World");

   TEST_ASSERT_EQUAL_PTR(&dest, r);
   TEST_ASSERT_EQUAL_STRING("WorldFoo", destBuf);
   TEST_ASSERT_EQUAL_UINT16(strlen("WorldFoo"), dest.cnt);
}

/* --------------------------------- test_CpyTail_KeyAtEnd ------------------------------------------ */
void test_CpyTail_KeyAtEnd(void)
{
   C8 const srcStr[] = "HelloWorld";
   S_BufC8_ro src = {.cs = srcStr, .cnt = (U16)strlen(srcStr)};

   C8 destBuf[20];
   S_BufC8 dest = {.cs = destBuf, .cnt = sizeof(destBuf)-1};

   CpyTail_BufC8(&dest, &src, "World");

   TEST_ASSERT_EQUAL_STRING("World", destBuf);
   TEST_ASSERT_EQUAL_UINT16(5, dest.cnt);
}

/* --------------------------------- test_CpyTail_Truncated ------------------------------------------ */
void test_CpyTail_Truncated(void)
{
   C8 const srcStr[] = "HelloWorldFoo";
   S_BufC8_ro src = {.cs = srcStr, .cnt = (U16)strlen(srcStr)};

   C8 destBuf[10];
   memset(destBuf, 0x5A, sizeof(destBuf));
   S_BufC8 dest = {.cs = destBuf, .cnt = 4};       // Room for only 4 chars.

   CpyTail_BufC8(&dest, &src, "World");            // Tail is "WorldFoo" (8 chars); too big for 'dest'.

   TEST_ASSERT_EQUAL_STRING("Worl", destBuf);
   TEST_ASSERT_EQUAL_UINT16(4, dest.cnt);
   TEST_ASSERT_EQUAL_UINT8(0x5A, destBuf[5]);      // Past the '\0'; untouched.
}

/* --------------------------------- test_CpyTail_NotFound -------------------------------------------- */
void test_CpyTail_NotFound(void)
{
   C8 const srcStr[] = "HelloWorldFoo";
   S_BufC8_ro src = {.cs = srcStr, .cnt = (U16)strlen(srcStr)};

   C8 destBuf[20] = "Stale";
   S_BufC8 dest = {.cs = destBuf, .cnt = sizeof(destBuf)-1};

   CpyTail_BufC8(&dest, &src, "xyz");

   TEST_ASSERT_EQUAL_STRING("", destBuf);
   TEST_ASSERT_EQUAL_UINT16(0, dest.cnt);
}

/* --------------------------------- test_CpyTail_KeyBeyondSrcCnt --------------------------------------

   'key' is present in the underlying char[] but past 'src->cnt'; must be treated as not-found
   because CpyTail_BufC8() only searches the first 'src->cnt' chars of 'src'.
*/
void test_CpyTail_KeyBeyondSrcCnt(void)
{
   C8 const srcStr[] = "HelloWorldFoo";
   S_BufC8_ro src = {.cs = srcStr, .cnt = 5};      // Only "Hello" is in-bounds.

   C8 destBuf[20] = "Stale";
   S_BufC8 dest = {.cs = destBuf, .cnt = sizeof(destBuf)-1};

   CpyTail_BufC8(&dest, &src, "World");

   TEST_ASSERT_EQUAL_STRING("", destBuf);
   TEST_ASSERT_EQUAL_UINT16(0, dest.cnt);
}

/* --------------------------------- test_CpyTail_DegenerateInputs -------------------------------------- */
void test_CpyTail_DegenerateInputs(void)
{
   C8 destBuf[10] = "Stale";
   S_BufC8 dest = {.cs = destBuf, .cnt = sizeof(destBuf)-1};

   C8 const srcStr[] = "Hello";
   S_BufC8_ro src = {.cs = srcStr, .cnt = (U16)strlen(srcStr)};
   S_BufC8_ro emptySrc = {.cs = "", .cnt = 0};
   S_BufC8_ro nullCsSrc = {.cs = NULL, .cnt = 5};
   S_BufC8_ro zeroCntSrc = {.cs = srcStr, .cnt = 0};      // non-empty 'cs' but 'cnt' == 0.

   CpyTail_BufC8(&dest, NULL, "key");                    // src == NULL
   TEST_ASSERT_EQUAL_STRING("Stale", destBuf);

   CpyTail_BufC8(&dest, &nullCsSrc, "key");               // src->cs == NULL
   TEST_ASSERT_EQUAL_STRING("Stale", destBuf);

   CpyTail_BufC8(&dest, &emptySrc, "key");               // src is empty
   TEST_ASSERT_EQUAL_STRING("Stale", destBuf);

   CpyTail_BufC8(&dest, &zeroCntSrc, "key");             // src->cnt == 0
   TEST_ASSERT_EQUAL_STRING("Stale", destBuf);

   CpyTail_BufC8(&dest, &src, NULL);                     // key == NULL
   TEST_ASSERT_EQUAL_STRING("Stale", destBuf);

   CpyTail_BufC8(&dest, &src, "");                       // key is empty
   TEST_ASSERT_EQUAL_STRING("Stale", destBuf);
}


// ================================ Strlen_BufC8() =================================================

void test_Strlen_Null(void)
{
   TEST_ASSERT_EQUAL_UINT16(0, Strlen_BufC8(NULL));
}

void test_Strlen_Empty(void)
{
   C8 buf[1] = "";
   S_BufC8 s = {.cs = buf, .cnt = 10};
   TEST_ASSERT_EQUAL_UINT16(0, Strlen_BufC8(&s));
}

void test_Strlen_Basic(void)
{
   C8 buf[20] = "Steven";
   S_BufC8 s = {.cs = buf, .cnt = sizeof(buf)-1};
   TEST_ASSERT_EQUAL_UINT16(6, Strlen_BufC8(&s));
}

void test_Strlen_NoTerminatorWithinCnt(void)
{
   C8 buf[5] = {'A','B','C','D','E'};       // No '\0' anywhere in 'buf'.
   S_BufC8 s = {.cs = buf, .cnt = 5};
   TEST_ASSERT_EQUAL_UINT16(5, Strlen_BufC8(&s));      // Returns 'cnt', capped.
}


// ================================ Strcpy_BufC8() ==================================================

void test_Strcpy_Basic(void)
{
   C8 buf[20];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   S_BufC8 const *r = Strcpy_BufC8(&dest, "Steven");

   TEST_ASSERT_EQUAL_PTR(&dest, r);
   TEST_ASSERT_EQUAL_STRING("Steven", buf);
   TEST_ASSERT_EQUAL_UINT16(sizeof(buf)-1, dest.cnt);       // 'cnt' is unchanged by Strcpy_BufC8().
}

void test_Strcpy_Truncates(void)
{
   C8 buf[6];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 const dest = {.cs = buf, .cnt = 4};

   Strcpy_BufC8(&dest, "Steven");

   TEST_ASSERT_EQUAL_STRING("Stev", buf);
   TEST_ASSERT_EQUAL_UINT16(4, dest.cnt);
}

void test_Strcpy_NullSrc(void)
{
   C8 buf[10] = "Stale";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   Strcpy_BufC8(&dest, NULL);

   TEST_ASSERT_EQUAL_STRING("Stale", buf);      // Unchanged.
}

void test_Strcpy_EmptySrc(void)
{
   C8 buf[10] = "Stale";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   Strcpy_BufC8(&dest, "");

   TEST_ASSERT_EQUAL_STRING("Stale", buf);      // Unchanged.
}


// ================================ Strcat_BufC8() ===================================================

void test_Strcat_Basic(void)
{
   C8 buf[20] = "Foo";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   S_BufC8 const *r = Strcat_BufC8(&dest, "Bar");

   TEST_ASSERT_EQUAL_PTR(&dest, r);
   TEST_ASSERT_EQUAL_STRING("FooBar", buf);
   TEST_ASSERT_EQUAL_UINT16(sizeof(buf)-1, dest.cnt);       // 'cnt' is unchanged.
}

void test_Strcat_OntoEmpty(void)
{
   C8 buf[20] = "";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   Strcat_BufC8(&dest, "Bar");

   TEST_ASSERT_EQUAL_STRING("Bar", buf);
}

void test_Strcat_Truncates(void)
{
   C8 buf[6] = "Foo";                        // 'cnt' = 5; 3 chars used, 2 chars open.
   S_BufC8 const dest = {.cs = buf, .cnt = 5};

   Strcat_BufC8(&dest, "Bar");

   TEST_ASSERT_EQUAL_STRING("FooBa", buf);   // Only 2 of "Bar"'s chars fit.
}

void test_Strcat_NullSrc(void)
{
   C8 buf[10] = "Foo";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   Strcat_BufC8(&dest, NULL);

   TEST_ASSERT_EQUAL_STRING("Foo", buf);
}

void test_Strcat_EmptySrc(void)
{
   C8 buf[10] = "Foo";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   Strcat_BufC8(&dest, "");

   TEST_ASSERT_EQUAL_STRING("Foo", buf);
}


// ================================ Strcpy_ChainBufC8() ================================================

void test_ChainCpy_Basic(void)
{
   C8 buf[20];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   S_BufC8 next = Strcpy_ChainBufC8(&dest, "Steven");

   TEST_ASSERT_EQUAL_STRING("Steven", buf);
   TEST_ASSERT_EQUAL_PTR(buf+6, next.cs);
   TEST_ASSERT_EQUAL_UINT16(dest.cnt-6, next.cnt);      // Chars-free remaining, after the 6 copied in.
}

void test_ChainCpy_Chained(void)
{
   C8 buf[20];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   S_BufC8 next = Strcpy_ChainBufC8(&dest, "Foo");
   next = Strcpy_ChainBufC8(&next, "Bar");

   // Unlike Chain_BufC8(), there's no separator between chained copies: the 2nd copy's
   // start overwrites the 1st copy's '\0', so the two runs together as one C-string.
   TEST_ASSERT_EQUAL_STRING("FooBar", buf);
   TEST_ASSERT_EQUAL_PTR(buf+6, next.cs);
}

void test_ChainCpy_Truncates(void)
{
   C8 buf[6];
   memset(buf, 0x5A, sizeof(buf));
   S_BufC8 const dest = {.cs = buf, .cnt = 4};

   S_BufC8 next = Strcpy_ChainBufC8(&dest, "Steven");

   TEST_ASSERT_EQUAL_STRING("Stev", buf);
   TEST_ASSERT_EQUAL_PTR(buf+4, next.cs);
   TEST_ASSERT_EQUAL_UINT16(0, next.cnt);
}

void test_ChainCpy_NullSrc(void)
{
   C8 buf[10] = "Stale";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   S_BufC8 next = Strcpy_ChainBufC8(&dest, NULL);

   TEST_ASSERT_EQUAL_STRING("Stale", buf);
   TEST_ASSERT_EQUAL_PTR(buf, next.cs);              // Copy of 'dest' as-is.
   TEST_ASSERT_EQUAL_UINT16(dest.cnt, next.cnt);
}

void test_ChainCpy_EmptySrc(void)
{
   C8 buf[10] = "Stale";
   S_BufC8 const dest = {.cs = buf, .cnt = sizeof(buf)-1};

   S_BufC8 next = Strcpy_ChainBufC8(&dest, "");

   TEST_ASSERT_EQUAL_STRING("Stale", buf);
   TEST_ASSERT_EQUAL_PTR(buf, next.cs);
}


// ----------------------------------------- eof --------------------------------------------
