/*
**
** Module Name:
**       Nmc.h
**
** Purpose:
**
** Description:
**      This file provides the public prototypes and definitions
**      for use with the node's motion controller package.
**
** Authors:
**    JDM:     J. D. Mills (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**    $Id: Nmc.h,v 0.12 2004/05/19 08:57:34 mkb Exp $
**
** History:
**    $Log: Nmc.h,v $
**    Revision 0.12  2004/05/19 08:57:34  mkb
**    Set scheduler policy and priority using OS independant call.
**
**    Revision 0.11  2004/05/06 12:52:07  mkb
**    Removed redundant, experimental TFP code.
**    Improved handling of incoming messages.
**    Minor improvement to diagnostic information.
**
**    Revision 0.10  2003/04/28 15:29:41  mkb
**    Add OID to support L2 limit recovery.
**
**    Revision 0.9  2002/03/06 17:14:48  djm
**    Added new error codes.
**
**    Revision 0.8  2001/08/28 10:58:03  djm
**    Removed incorrect comma at end of enum list.
**
**    Revision 0.7  2001/08/20 10:29:07  mjf
**    Addition of enum name to states.
**
**    Revision 0.6  2001/02/01 10:04:40  jdm
**    E_NMC_SERVER_PRI definition added.
**
**    Revision 0.5  2000/12/21 09:12:38  jdm
**    Package revision and shutdown OIDs added.
**
**    Revision 0.4  2000/12/12 13:32:12  jdm
**    Error codes for drive enable/disable added.
**
**    Revision 0.3  2000/11/09 10:12:20  jdm
**    External definition of NMC error code is now used.
**
**    Revision 0.2  2000/11/07 10:18:27  jdm
**    Updated for use with piblic
**    enum definitions.
**
**    Revision 0.1  2000/11/03 10:18:06  jdm
**    Initial creation.
**
**
*/

#ifndef _NMC_HEADER_INCLUDED
#define _NMC_HEADER_INCLUDED

/*
** =============================================================================
** Start of include files:
** =============================================================================
*/

#include "TtlSystem.h"      /* Include the package definitions. */

/*
** =============================================================================
** End of include files:
** =============================================================================
*/

/*
** =============================================================================
** Start of Type Definitions:
** =============================================================================
*/

/* Enumerate this package's error codes. */
typedef enum{
   E_NMC_MESSAGE_NOT_RECOGNISED = STATUS_START(NMC),
   E_NMC_FAILED_TO_CONFIGURE_TIMER,
   E_NMC_OID_TABLE_INIT_FAILURE,
   E_NMC_INVALID_STATE,
   E_NMC_FAILED_TO_SEND_MESSAGE_TO_END_CLIENT,
   E_NMC_FAILED_TO_RELEASE_BRAKES,
   E_NMC_FAILED_TO_APPLY_BRAKES,
   E_NMC_FAILED_TO_ENABLE_DRIVE,
   E_NMC_FAILED_TO_DISABLE_DRIVE,
   E_NMC_FAILED_MIF_COMMUNICATIONS,
   E_NMC_CLIENT_SET_FAILURE,   
   E_NMC_CLIENT_GET_FAILURE,   

   E_NMC_SOFTWARE_ERROR = STATUS_START(NMC) + 0xFFFF
}eNmcClientStatus_t;

/* Enumerate the OID's owned by this package. */
typedef enum eNmcDataId_e{
   D_NMC_DATAID_BOL = OID_START(NMC),  /* Offset the OID's by the package ID. */    /* 0000 */
   D_NMC_PROC_STATE,                   /* The nmc process state. */                 /* 0001 */
   D_NMC_APP_VERSION,                  /* NMC package revision. */                  /* 0002 */
   D_NMC_SERVER_SHUTDOWN,              /* Server shutdown command flag. */          /* 0003 */

   D_NMC_STATE,                        /* The motion control FSM state. */          /* 0004 */
   D_NMC_PRIVATE_AXIS_START_COMMAND,   /* The private axis start command. */        /* 0005 */
   D_NMC_PRIVATE_AXIS_STOP_COMMAND,    /* The private axis stop command. */         /* 0006 */
   D_NMC_PRIVATE_L2_LIMIT_MASK_COMMAND,   /* Mask the L2 limit during recovery. */  /* 0007 */

   D_NMC_DATAID_EOL                    /* End of list marker. */
}eNmcDataId_t;

/* Enumerate the state-machine states. */
typedef  enum eNmcStates_e
{
   E_NMC_STATE_PRECONTROLLING,
   E_NMC_STATE_NODE_STOPPED,
   E_NMC_STATE_RELEASING_BRAKES,
   E_NMC_STATE_NODE_AVAILABLE,
   E_NMC_STATE_NODE_STOPPING,
   E_NMC_STATE_APPLYING_BRAKES
}  eNmcStates_t;

/*
** =============================================================================
** End of Type Definitions:
** =============================================================================
*/


/*
** =============================================================================
** Start of Specific Definitions:
** =============================================================================
*/

/* Define this server's name-server name and the name of its executeable. */
#define E_NMC_REG_SERVER_NAME   "E_TTL_NMC"
#define E_NMC_SERVER_PROC       "Nmc"
#define E_NMC_SERVER_PATH       ""

#ifdef  E_WFL_OS_QNX6
#define E_NMC_SERVER_PRI                 50
#endif
#ifdef  E_WFL_OS_QNX4
#define E_NMC_SERVER_PRI                 20
#endif

#endif  /* _NMC_HEADER_INCLUDED */                                                    

/*
** =============================================================================
** End of header:   Nmc.h
** =============================================================================
*/
