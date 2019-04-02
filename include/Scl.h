/*
** Header File Name:
**    Scl.h
**
** Purpose:
**    Header file for SCL functions
**
** Description:
**    This header file contains definitions for the Serial Communications
**    library. This package is designed for communicating with a serial
**    port (e.g. /dev/ser1).
**    This code has been written to conform to the "TTL Coding 
**    Standard"  v0.4 29-Sep-1998.
**
** Authors:
**    djm: Derek J. McKay
**    mkb: Mark Bowman
**
** Version:
**    $Id: Scl.h,v 0.10 2001/10/05 14:07:27 mkb Exp $
**    (see History if not defined here)
**
** History:
**    $Log: Scl.h,v $
**    Revision 0.10  2001/10/05 14:07:27  mkb
**    Added eSclBurstReceive function.
**    Provides a function to receive a message of unknown length. Provided the
**    first character arrives within the timeout period, the message will
**    grow until an inter-character timeout is exceeded or there is no more
**    room in the provided buffer.
**
**    Revision 0.9  2000/12/06 16:10:37  djm
**    Added new error code for eSclSend() timeouts.
**
**    Revision 0.8  2000/11/08 16:50:39  mkv
**    Added timeout overflow fault status.
**
**    Revision 0.7  2000/11/06 16:54:37  mkv
**    Added to eSclStatus_t for eSclSend() timeout.
**
**    Revision 0.6  2000/02/29 12:48:36  djm
**    Changed prototypes for functions now returning status,
**    rather than passing it back as status arguments.
**    Also made changes to the SCL error enumeration.
**
**    Revision 0.5  2000/02/28 16:40:51  djm
**    Corrected error in E_SCL_FLUSH_FAIL comment.
**
**    Revision 0.4  2000/02/28 15:17:59  djm
**    Initial deposit into RCS of working version.
**
**    28-Feb-2000 djm v0.4 Implemented STATUS_START usage in eSclStatus_t
**    23-Feb-2000 djm v0.3 Updated error codes.
**    21-Feb-2000 djm v0.2 Updated function prototypes.
**    18-Feb-2000 djm v0.1 Initial creation.
**
*/


#ifndef SCL_H_DEFINED
#define SCL_H_DEFINED


/* Required include files */
#include <limits.h>
#include "TtlSystem.h"


/* General type definitions */

typedef int eSclPort_t;           /* Identifier for open serial ports */


/* Returned status values */

typedef enum {
   E_SCL_GEN_ERR = STATUS_START(SCL),   /* Miscellanous error */
   E_SCL_OPEN_FAIL,               /* Unable to open requested device */
   E_SCL_CLOSE_FAIL,              /* Failure on closing device */
   E_SCL_INSUF_DATA,              /* Non-positive amount of data to Tx */
   E_SCL_SEND_ERR,                /* Failure to send serial data */
   E_SCL_INCOMPLETE,              /* Not all characters were sent */
   E_SCL_DRAIN_ERR,               /* Failure to drain output buffer */
   E_SCL_RECV_ERR,                /* Failure to receive serial data */
   E_SCL_TIMEOUT,                 /* Insufficient chars rx'd in time */
   E_SCL_TX_TIMEOUT,              /* Insufficient chars tx'd in time */
   E_SCL_FLUSH_FAIL,              /* Error flushing input/output buffers */
   E_SCL_CONFIG_ERR,              /* Error configuring serial device */
   E_SCL_ZERO_TIMEOUT,            /* Send timeout value is zero */
   E_SCL_TIMEOUT_OVERFLOW,        /* Specified timeout was too large */
   E_SCL_TIMEOUT_INIT_ERR,        /* Failed to create timer */
   E_SCL_STATUS_EOL,              /* End Of List */
   E_SCL_STATUS_MAX = INT_MAX     /* Max value */
} eSclStatus_t;


/* Possible parity options */

typedef enum {
   E_SCL_NOPARITY = 0,            /* No parity checking */
   E_SCL_ODDPARITY,               /* Use odd parity checking */
   E_SCL_EVENPARITY               /* Use even parity checking */
} eSclParity_t;


/* Function prototypes */

extern Status_t eSclOpen(eSclPort_t *id, char *DevName);
extern Status_t eSclClose(eSclPort_t id);
extern Status_t eSclSend(eSclPort_t id, char *buf, Int32_t nch, Int32_t Timeout);
extern Status_t eSclReceive(eSclPort_t id, char *Buffer, int *NumChars, 
   int MinChars, int MaxChars, int Timeout);
extern Status_t eSclBurstReceive(eSclPort_t id, char *Buffer, int *NumChars, 
   int MaxChars, int Timeout, int InterCharTime);
extern Status_t eSclClear(eSclPort_t id);
extern Status_t eSclConfig(eSclPort_t PortID, int Baud, int DataBits, 
   int StopBits, eSclParity_t Parity);



#endif


/* EOF */
