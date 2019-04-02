/*******************************************************************************
** Module Name:
**    Aei.h
**
** Purpose:
**    Public header file for AEI package.
**
** Description:
**    Used to declare exported data, functions and definitions for the
**    AEI package.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    ...
**
** Author(s):
**    Derek J. McKay (djm)
**    Jon D. Mills (jdm)
**    Robert D. Lett (rdl)
**    Mark K. Vernon (mkv)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000-2001
**
** Version:
**    $Id: Aei.h,v 0.6 2003/04/28 15:39:48 sxf Exp $
**
** History:
**    $Log: Aei.h,v $
**    Revision 0.6  2003/04/28 15:39:48  sxf
**    Add option to loop over baud rates for abs enc
**
**    Revision 0.5  2002/11/20 10:08:39  mjf
**    Addition of 'simulate' flag to prevent attempts to access the
**    serial port.
**
**    Revision 0.4  2001/09/14 14:49:13  mjf
**    Changes due to general node definitions collected together.
**
**    Revision 0.3  2001/03/19 19:19:20  mjf
**    Import the system-wide axis nodes names for the command-line args.
**
**    Revision 0.2  2001/03/10 14:08:13  djm
**    Added error code for invalid serial port number.
**
**    Revision 0.1  2001/03/02 15:36:02  djm
**    Initial creation.
**
**
*******************************************************************************/


#ifndef _AEI_H_DEFINED
#define _AEI_H_DEFINED


/*
** Code for coping with decl'n/def'n/init'n of global variables
*/

#ifdef M_AEI_MAIN
   #define E_AEI_EXTERN
   #define E_AEI_INIT(Initialisation) = Initialisation
#else
   #define E_AEI_EXTERN extern
   #define E_AEI_INIT(Initaliasation)
#endif


/*
** Required include files
*/

#include <limits.h>          /* For INT_MAX             */
#include "TtlSystem.h"       /* For Status_t and AEI definitions */

/*
** Strings used for command line arguments
*/

#define E_AEI_DIRECTSTR      "direct"
#define E_AEI_AZIMUTHSTR     E_ACN_STR_AZIMUTH
#define E_AEI_ELEVATIONSTR   E_ACN_STR_ELEVATION
#define E_AEI_CASSEGRAINSTR  E_ACN_STR_CASSEGRAIN
#define E_AEI_PORTNUMBERSTR "port [number]"
#define E_AEI_NOFIXED_BAUD  "nofixedbaud"
#define E_AEI_SIMULATESTR   "simulate"

/*
** Define error stati and return values
*/

enum eAeiStatus_e
{
   E_AEI_GEN_ERROR = STATUS_START(AEI),
   E_AEI_INIT_FAIL,
   E_AEI_SHUTDOWN_ERROR,
   E_AEI_INVALID_DATA_LEN,
   E_AEI_INVALID_COMMAND,
   E_AEI_REPLY_FAIL,
   E_AEI_SERVER_STARTUP_FAIL,
   E_AEI_SERVER_SHUTDOWN_FAIL,
   E_AEI_SERVER_NOT_RUNNING,
   E_AEI_SERVER_SEND_FAIL,
   E_AEI_SERIAL_TX_FAIL,
   E_AEI_SERIAL_TXLEN_ERR,
   E_AEI_SERIAL_RX_FAIL,
   E_AEI_SERIAL_CHK_ERR,
   E_AEI_ENCODER_NOT_FOUND,
   E_AEI_TIMER_SETUP_ERR,
   E_AEI_TIMER_CLOSE_ERR,
   E_AEI_TOO_MANY_AXES,          /* Too many axes specified on cmd-line */
   E_AEI_NO_AXES_SPECIFIED,      /* No axis specified on cmd-line */
   E_AEI_INVALID_AXIS_INDEX,     /* Unrecoginised axis index number */
   E_AEI_NO_CHKSM_POSSIBLE,      /* Message too short to generate checksum */
   E_AEI_HW_REPORTED_ERROR,      /* Hardware itself reports an error */
   E_AEI_GEAR_RATIO_ERROR,       /* Non-positive number of teeth on gear */

   E_AEI_AES_NOT_AVAILABLE,      /* Unable to detect AES (Abs.Enc.Server) */
   E_AEI_AES_GET_FAILURE,        /* Unable to get an OID value from the AES */ 
   E_AEI_AES_SET_FAILURE,        /* Unable to set an OID value in the AES */ 
   E_AEI_AES_REJECT,             /* Submission rejected by AES */
   E_AEI_SERIAL_PORT_CLOSED,     /* Serial port has not been opened for comms */
   E_AET_INVALID_PORT_NUM,       /* Invalid serial port number */

   E_AEI_STATUS_EOL,             /* End Of List               */
   E_AEI_STATUS_MAX = INT_MAX    /* enum forced to be 4 bytes */
};


#endif

/*******************************************************************************
** End of File Name: Aei.h
*******************************************************************************/
