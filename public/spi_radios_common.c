/* --------------------------------------------------------------------------------------

   Exports common to SPI-connected ISM Radios; currently ST S2LP & Si4463.

   This file is included in the drivers for individual radios, currently s2lp_drv.h
   and si4x_drv.h.  Include this C file just once (in another C file).
*/

PUBLIC C8 const * radio_HAL_Status_Name(T_HAL_Status s) {
   switch(s) {
      case HAL_OK:      return "HAL_OK";
      case HAL_ERROR:   return "HAL_ERROR";
      case HAL_BUSY:    return "HAL_BUSY";
      case HAL_TIMEOUT: return "HAL_TIMEOUT";
      case HAL_NACK:    return "HAL_NACK";
      default: return "Unknown HAL_STATUS";
   }
}

// ---------------------------- eof --------------------------------------------------

