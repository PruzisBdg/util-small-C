#include "libs_support.h"
#include "util.h"

PUBLIC C8 const * PrintEndian(E_EndianIs e) {
   return
      e == eNoEndian
         ? "none"
         : (e == eLittleEndian
               ? "little"
               : (e == eBigEndian
                  ? "big"
                  : "Illegal value for E_EndianIs")); }

// ------------------------------------- eof ---------------------------------------------------

