/* --------------------------------------------------------------------------------------

   Exports common to SPI-connected ISM Radios; currently ST S2LP & Si4463.

   This file is included in the drivers for individual radios, currently s2lp_drv.h
   and si4x_drv.h.
*/

#ifndef SPI_RADIOS_COMMON_H
#define SPI_RADIOS_COMMON_H

#include "libs_support.h"

/* ------------------------------ MCU SPI Interface -------------------------------------------

   A radio must supply 'hdl' and 'timeout_msec'. It MAY supply any to all of start() thru sendRcv().
*/

typedef enum { HAL_OK = 0x00U, HAL_ERROR = 0x01U, HAL_BUSY = 0x02U, HAL_TIMEOUT  = 0x03U, HAL_NACK = 0x04U  } T_HAL_Status;

// For sendRcv()
#define _radio_Spi_ReleaseCS false
#define _radio_Spi_HoldCS   true

/* ------------------------------- radio_McuSpi ---------------------------------------

   The Host SPI interface for a Radio.

   (r) = required; the Host must supply this. A Radio driver does not have to check that
   it is available.

   (o) = optional: A Radio Driver must check that the Host has supplied this - and should fail
   gracefully if it does need it but the Host has not supplied it.
*/
typedef struct {
   /* (o) Usually a MCU-specific SPI driver - is given to the MCU-specific calls. This is a pass-thru; the
      Radio Driver should forward it as-is and not use it.
   */
   void         *hdl;
   // Timeout for a blocking transfer - given to the MCU-specific calls.
   U16          timeout_msec;
   // (o) Assert CS; lock SPI etc
   T_HAL_Status (*start)(void);
   // (o) Negate CS; unlock SPI etc.
   T_HAL_Status (*end)(void);
   // (r) Send data (possibly after exchanging a header)
   T_HAL_Status (*send)   (void *spiHdl, U8 const *txFrom,           U8 numBytes, U16 timeout_msec, bool holdCS);
   // (r) Receive data (possibly after exchanging a header)
   T_HAL_Status (*rcv)    (void *spiHdl,                   U8 *rxTo, U8 numBytes, U16 timeout_msec, bool holdCS);
   // (o) Send header/get status (2 bytes on S2LP)
   T_HAL_Status (*xferHdr)(void *spiHdl, U8 const *txFrom, U8 *rxTo, U16 timeout_msec,              bool holdCS);
   // (o) Send and receive equal numbrs of bytes
   T_HAL_Status (*sendRcv)(void *spiHdl, U8 const *txFrom, U8 *rxTo, U8 payloadBytes, U16 timeout_msec, bool holdCS);
} radio_McuSpi;



#endif // SPI_RADIOS_COMMON_H
