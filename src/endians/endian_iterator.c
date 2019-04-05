#include "libs_support.h"
#include "util.h"

/*-----------------------------------------------------------------------------------------
|
| Endian-aware iterator for buffer ptrs.
|
| Provides an iterator to reverse a multi-byte variable (from external to the host system) when
| when the endianness of the variable is opposite to that of the system.
|
------------------------------------------------------------------------------------------*/

// Increment / decrement '_at'.
PRIVATE U8 *epIncr(T_EndianPtr *di) {
   di->_at++;
   return (U8*)di->_at; }

PRIVATE U8 *epDecr(T_EndianPtr *di) {
   di->_at--;
   return (U8*)di->_at; }

// Pre-position buffer ptr and set direction, depending system endian vs target endian.
PUBLIC U8* EndianPtr_New(T_EndianPtr *ep, U8 const *bufStart, U16 nbytes, E_EndianIs bitFieldEndian)
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
      if(nbytes > 1 && up == false)
         { ep->_at += nbytes-1; }
   #else
      #warning "bit64K_Out() Endian undefined - bytes will always be copied no-reverse."
   #endif // __BYTE_ORDER__

   ep->next = up == true ? epIncr : epDecr;        // 'next' will be 'up' or 'down', depending.
   return (U8 *)ep->_at;                           // Return 1st byte.
}

// ======================================== EOF =======================================================
