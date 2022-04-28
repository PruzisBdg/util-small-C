/* --------------------------------------------------------------------------------------

   Exports common to SPI-connected ISM Radios; currently ST S2LP & Si4463.

   This file is included in the drivers for individual radios, currently s2lp_drv.h
   and si4x_drv.h.
*/

#ifndef SPI_RADIOS_COMMON_H
#define SPI_RADIOS_COMMON_H

#include "libs_support.h"

/* ------------------------------ MCU SPI Interface ------------------------------------------- */

typedef enum { HAL_OK = 0x00U, HAL_ERROR = 0x01U, HAL_BUSY = 0x02U, HAL_TIMEOUT  = 0x03U, HAL_NACK = 0x04U  } T_HAL_Status;

// For sendRcv()
#define _radio_SendRcv_ReleaseCS false
#define _radio_SendRcv_HoldCS	true

typedef struct {
   void         *hdl;                        // The MCU-specific SPI driver - is given to the MCU-specific calls.
   U16          timeout_msec;                // Timeout for a blocking transfer - given to the MCU-specific calls.
   T_HAL_Status (*start)(void);              // Assert CS; lock SPI etc
   T_HAL_Status (*end)(void);                // Negate CS; unlock SPI etc.
   T_HAL_Status (*send)   (void *spiHdl, U8 const *txFrom,           U8 numBytes, U16 timeout_msec);	// Send data (after header)
   T_HAL_Status (*rcv)    (void *spiHdl,                   U8 *rxTo, U8 numBytes, U16 timeout_msec, bool holdCS);	// Receive data (after header)
   T_HAL_Status (*xferHdr)(void *spiHdl, U8 const *txFrom, U8 *rxTo, U16 timeout_msec, bool holdCS);	// Send header/get status (2 bytes)
   T_HAL_Status (*sendRcv)(void *spiHdl, U8 const *txFrom, U8 *rxTo, U8 payloadBytes, U16 timeout_msec, bool holdCS);	// Send header and exchange data
} radio_McuSpi;



#endif // SPI_RADIOS_COMMON_H
