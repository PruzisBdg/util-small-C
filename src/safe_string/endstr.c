/*---------------------------------------------------------------------------
|
|  $Workfile:  $
|
|  $Header:  $
|
|  $Log:  $
|
|--------------------------------------------------------------------------*/

#include "libs_support.h"
#include "util.h"
#include <string.h>


/* ------------------------------- EndStr -----------------------------------

   Spot the end of the non-const string.
*/

PUBLIC C8* EndStr(C8 * str) {
   return str + strlen((C8*)str);
}

PUBLIC C8 const * EndStrC(C8 const * str) {
   return str + strlen((C8*)str);
}

// ------------------------------- eof -----------------------------------
