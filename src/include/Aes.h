/*******************************************************************************
** Module Name:
**    Aes.h
**
** Purpose:
**    Public header file for AES package.
**
** Description:
**    Used to declare exported data, functions and definitions for the
**    AES package.
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
**    $Id: Aes.h,v 0.6 2002/08/12 15:55:25 djm Exp $
**
** History:
**    $Log: Aes.h,v $
**    Revision 0.6  2002/08/12 15:55:25  djm
**    Added a new error code, used when the AEI fails to submit
**    data.
**
**    Revision 0.5  2002/03/25 16:04:14  djm
**    Added two new data. These are for identifying submitted
**    values containing the application version number, as well
**    as that of the client application (AEI).
**
**    Revision 0.4  2002/02/28 14:37:52  djm
**    Added a new OID for counting the number of read-faults.
**
**    Revision 0.3  2001/09/07 13:12:59  djm
**    Added new OID (D_AES_CLIENT_SHUTDOWN) in order to allow the
**    AEI to register that it is terminating.
**
**    Revision 0.2  2001/06/18 10:21:10  mjf
**    Addition of definitions for the TQW library.
**
**    Revision 0.1  2001/03/02 13:00:21  djm
**    Initial creation.
**
**
*******************************************************************************/


#ifndef _AES_H_DEFINED
#define _AES_H_DEFINED


/*
** Code for coping with decl'n/def'n/init'n of global variables
*/

#ifdef M_AES_MAIN
   #define E_AES_EXTERN
   #define E_AES_INIT(Initialisation) = Initialisation
#else
   #define E_AES_EXTERN extern
   #define E_AES_INIT(Initaliasation)
#endif


/*
** Required include files
*/

#include <limits.h>          /* For INT_MAX             */
#include "TtlSystem.h"       /* For Status_t and AES definitions */


/*
** QNX definitions for server set-up
*/

#define E_AES_REG_SERVER_NAME   "E_TTL_AES_AesServer"
#define E_AES_SERVER_PROC       "Aes"
#define E_AES_SERVER_PATH       "/opt/ttl/bin/Aes"
#define E_AES_SERVER_PRI        10


/*
** Define error stati and return values
*/

enum eAesStatus_e
{
    E_AES_GEN_ERROR = STATUS_START(AES),
    E_AES_INIT_FAIL,
    E_AES_SHUTDOWN_ERROR,
    E_AES_INVALID_DATA_LEN,
    E_AES_INVALID_COMMAND,
    E_AES_REPLY_FAIL,
    E_AES_SERVER_STARTUP_FAIL,
    E_AES_SERVER_SHUTDOWN_FAIL,
    E_AES_SERVER_NOT_RUNNING,
    E_AES_SERVER_SEND_FAIL,
    E_AES_TIMER_SETUP_ERR,
    E_AES_TIMER_CLOSE_ERR,
    E_AES_TOO_MANY_AXES,          /* Too many axes specified on cmd-line */
    E_AES_NO_AXES_SPECIFIED,      /* No axis specified on cmd-line */
    E_AES_INVALID_AXIS_INDEX,     /* Unrecoginised axis index number */
    E_AES_NO_CHKSM_POSSIBLE,      /* Message too short to generate checksum */
    E_AES_HW_REPORTED_ERROR,      /* Hardware itself reports an error */
    E_AES_GEAR_RATIO_ERROR,       /* Non-positive number of teeth on gear */
    E_AES_AEI_SHUTDOWN_TIMEOUT,   /* AES timedout waiting for AEI to terminate */
    E_AES_AEI_NO_RECENT_DATA,     /* AEI has not submitted data recently */

    E_AES_STATUS_EOL,             /* End Of List               */
    E_AES_STATUS_MAX = INT_MAX    /* enum forced to be 4 bytes */
};


/*
** Package OID enumeration list
*/

typedef enum eAesDataId_e
{
   D_AES_DATAID_BOL = OID_START(AES),  /* Offset OIDs by package ID */

   D_AES_PROC_STATE,         /* AesServer process state */
   D_AES_SERVER_SHUTDOWN,    /* AesServer shutdown request */
   D_AES_POSITION,           /* Absolute Encoder position reading */
   D_AES_PINION_TEETH,       /* Number of teeth on pinion gear */
   D_AES_SPUR_TEETH,         /* Number of teeth on spur gear */

   D_AES_GREY_CODED,         /* TRUE if grey-coded binary is used */
   D_AES_STATUS,             /* Status bit field for encoder op. params */
   D_AES_TANNENBAUM_FMT,     /* TRUE if tannenbaum binary code is used */
   D_AES_MULTITURN_BITS,     /* Multiturn active bits, Christmastree format */
   D_AES_SINGLETURN_BITS,    /* Sinlgeturn active bits, Christmastree format */

   D_AES_SCALE,              /* Scaling factor */
   D_AES_UNITS,              /* Units per revolution */
   D_AES_MULTIPLIER,         /* Scaling multiplier */
   D_AES_REVS,               /* Number of revolutions before clocking */
   D_AES_ORIGIN,             /* Offset applied to read encoder value */
   D_AES_OFFSET,             /* Offset applied to read encoder value */

   D_AES_TYPE,               /* Encoder type code */
   D_AES_USER_SER_NUM,       /* User assigned serial number */
   D_AES_COUNTS_PER_TURN,    /* Resolution of a single turn */
   D_AES_MAX_REVS,           /* Maximum number of revolutions */
   D_AES_FACTORY_SER_NUM,    /* Factory assigned serial number */

   D_AES_TOTAL_OP_TIME,      /* Total accumulated operating time of encoder */

   D_AES_CLIENT_SHUTDOWN,    /* Aei shutdown */

   D_AES_READ_FAULTS,        /* Abs.Enc. read failures since start-up */

   D_AES_AES_VERSION,        /* Application version of AES itself */
   D_AES_AEI_VERSION,        /* Application version of the AEI client */

   D_AES_DATAID_EOL,                   /* End-of-list marker */
   D_AES_DATAID_MAX = INT_MAX          /* enum forced to be 4 bytes */
} eAesDataId_t;



#endif

/*******************************************************************************
** End of File Name: Aes.h
*******************************************************************************/

