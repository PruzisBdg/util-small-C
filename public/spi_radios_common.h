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

PUBLIC C8 const * radio_HAL_Status_Name(T_HAL_Status s);

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
   // (o) Send and receive equal numbers of bytes
   T_HAL_Status (*sendRcv)(void *spiHdl, U8 const *txFrom, U8 *rxTo, U8 payloadBytes, U16 timeout_msec, bool holdCS);
} radio_McuSpi;


/* ============================== Wireless MBus (WMBus) ==========================================

   These enumerate the framing and packet handling for a particular of type of Data Link Layer
   (mainly) in an WMBus link . Each unique both for send direction i.e to-Meter or from-Meter
   AND also to the device Meter or Other.
   Right now we have enums just for Meter cuz Meter (Polar Integrated) is all we are doing.
*/
typedef enum {
  eWMBusMode_NotCfgd = 0,           // No MBUs mode applies; Must be zero (0).
  
  // Mode S
  eWMBusMode_S1_M2O,                //  S1 & S2 (long preamble) - length = WMBUS_prmbl_ctrl + 279 (in "01" bit pairs) , Sync word = 0x7696 (18 bits)
  eWMBusMode_S1m_M2O,               //  S1m (short preamble) - length = WMBUS_prmbl_ctrl + 48 (in "01" bit pairs) , Sync word = 0x7696 (18 bits)
  eWMBusMode_S2_O2M,                //  S2, (other to meter) - same as 'eWMBusMode_T_MtrRcv'.
  eWMBusMode_S1_M2O_Rcv,
  
  // Mode T
  eWMBusMode_T_O2M,                 //  T2 (other to meter) - length = WMBUS_prmbl_ctrl + 15 (in "01" bit pairs) , Sync word = 0x7696 (18 bits)
  eWMBusMode_T_M2O,                 //  T1 & T2 (meter to other) - length = WMBUS_prmbl_ctrl + 19 (in "01" bit pairs) ,  Sync word = 0x3D (10 bits)
  eWMBusMode_T_M2O_Rcv,             //  Receiving T1 frames sent from eWMBusMode_T_M2O.
  
  // Mode C
  eWMBusMode_C_O2M_FmtA,
  eWMBusMode_C_O2M_FmtB,
  eWMBusMode_C_M2O_FmtA,
  eWMBusMode_C_M2O_FmtB,
  eWMBusMode_C_M2O_FmtA_Rcv,        // Receive Format A or Format B frames only. For in-house use, for now, we 
  eWMBusMode_C_M2O_FmtB_Rcv,        // do not have and do not need auto A/B decoding based on the Sync Word.
  
  eWMBusMode_R_Meter,               //  R2, short header - length = WMBUS_prmbl_ctrl + 39 (in "01" bit pairs) , Sync word = 0x7696 (18 bits)
  eWMBusMode_CW,
  eWMBusMode_StdPkt,                // Not and MBus packet. Just testing the S2LP.
  eWMBusMode_Last = eWMBusMode_StdPkt
} radio_eWMBus_Mode;

static inline bool wmbus_isModeS(radio_eWMBus_Mode m) {
   return m == eWMBusMode_S1_M2O || m == eWMBusMode_S1m_M2O || m == eWMBusMode_S2_O2M || m == eWMBusMode_S1_M2O_Rcv; }

static inline bool wmbus_isModeT(radio_eWMBus_Mode m) {
   return m == eWMBusMode_T_O2M || m == eWMBusMode_T_M2O || m == eWMBusMode_T_M2O_Rcv; }

static inline bool wmbus_isModeC(radio_eWMBus_Mode m) {
   return m == eWMBusMode_C_O2M_FmtA     || m == eWMBusMode_C_O2M_FmtB || 
          m == eWMBusMode_C_M2O_FmtA     || m == eWMBusMode_C_M2O_FmtB || 
          m == eWMBusMode_C_M2O_FmtA_Rcv || m == eWMBusMode_C_M2O_FmtB_Rcv; }
   
static inline bool wmbus_IsRcvr(radio_eWMBus_Mode m) {
   return m == eWMBusMode_S1_M2O_Rcv || m == eWMBusMode_T_M2O_Rcv || m == eWMBusMode_C_M2O_FmtA_Rcv || m == eWMBusMode_C_M2O_FmtB_Rcv ; }
   
// 'NotCfgd' is NOT an MBus mode - though it is a legal enumeration.
#define radio_IsWMBusMode(m)  ((m) > eWMBusMode_NotCfgd && (m) <= eWMBusMode_Last)
// Includes 'NotCfgd'
#define radio_IsWMBusModeEnum(m)  ((m) >= eWMBusMode_NotCfgd && (m) <= eWMBusMode_Last)


// ============================= ends: WMBus ================================================================



#endif // SPI_RADIOS_COMMON_H
