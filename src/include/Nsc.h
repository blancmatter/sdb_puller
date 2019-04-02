/*
**
** Module Name:
**       Nsc.h
**
** Purpose:
**
** Description:
**      This file provides the public prototypes and definitions
**      for use with the time and frequency processor package.
**
** Authors:
**    JDM:     J. D. Mills (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**    $Id: Nsc.h,v 0.17 2004/05/19 09:05:54 mkb Exp $
**
** History:
**    $Log: Nsc.h,v $
**    Revision 0.17  2004/05/19 09:05:54  mkb
**    Set scheduler policy and priority using OS independant call.
**
**    Revision 0.16  2004/05/06 13:01:16  mkb
**    Remove redundant experimental TFP code.
**    Improved performance of communications to other processes by
**    remembering connection details.
**    Create database of end-clients and useful OIDs.
**    Improved diagnostic information.
**    Improved handling of incoming messages.
**    Shutdown end-clients in reverse order to spawn rather than
**    by reading shutdown commands from the configuration file.
**
**    Revision 0.15  2003/05/02 13:06:55  mkb
**    Add support for L2 limit recovery.
**
**    Revision 0.14  2001/11/29 11:31:19  mjf
**    Re-introduction of 'halt demand' OID.
**
**    Revision 0.13  2001/10/17 12:07:08  mjf
**    Overahul to state model and OID list. There is now a seperate enable/
**    disable and home commands, and a latching stop demand. The state model
**    now includes 'disabled' and 'enabled'. The process states of the end-
**    clients are combined together to produce the overall node state.
**
**    Revision 0.12  2001/09/19 15:13:06  mjf
**    Updates as a result of source file name changes. No functional changes.
**
**    Revision 0.11  2001/09/05 12:29:40  djm
**    Added "user reported" state enumerated list (eNscUserAxisStates_e/t).
**    Also added additional OID and error messages.
**
**    Revision 0.10  2001/08/20 10:31:40  mjf
**    Addition of enum name to states.
**
**    Revision 0.9  2001/08/08 11:04:54  djm
**    Added extra error message.
**
**    Revision 0.8  2001/05/18 10:20:05  mjf
**    Updated public header file to import Generic State Controller offsets.
**    No functional changes.
**
**    Revision 0.7  2000/12/21 11:52:59  jdm
**    Application revision OID added.
**
**    Revision 0.6  2000/12/18 14:40:20  jdm
**    Extra error code added for end-client
**    termination failure.
**
**    Revision 0.5  2000/12/18 11:35:41  jdm
**    SAFE_STATE and ACTIVATE commands added.
**    New SAFE state added.
**
**    Revision 0.4  2000/11/07 09:06:08  jdm
**    State definition enum made public.
**
**    Revision 0.3  2000/11/03 14:32:45  jdm
**    Un-used error messages removed.
**
**    Revision 0.2  2000/11/03 09:29:16  jdm
**    Additional error code added.
**
**    Revision 0.1  2000/11/02 11:38:03  jdm
**    Initial creation.
**
**    Revision 0.6  2000/10/18 07:20:18  jdm
**    Name server registration name changed to be non node specific.
**
**    Revision 0.5  2000/10/11 14:51:58  jdm
**    Updated for use with new server type.
**
**
*/

/*
** =============================================================================
** Start of include files:
** =============================================================================
*/

#ifndef _NSC_HEADER_INCLUDED
#define _NSC_HEADER_INCLUDED

#include "TtlSystem.h"      /* Include the package definitions. */
#include "Gsc.h"            /* Include Generic State Controller definitions. */

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
   E_NSC_MESSAGE_NOT_RECOGNISED = STATUS_START(NSC),
   E_NSC_FAILED_TO_CONFIGURE_TIMER,
   E_NSC_OID_TABLE_INIT_FAILURE,
   E_NSC_TOP_LEVEL_INVALID_STATE,
   E_NSC_FAILED_TO_SPAWN_END_CLIENTS,
   E_NSC_FAILED_TO_SHUTDOWN_NODE,
   E_NSC_FAILED_TO_WARMBOOT_NODE,
   E_NSC_FAILED_TO_SEND_MESSAGE_TO_END_CLIENT,
   E_NSC_FAILED_TO_SHUTDOWN_END_CLIENTS,
   E_NSC_FAILED_TO_DETERMINE_USER_STATE,
    
   E_NSC_SOFTWARE_ERROR = STATUS_START(NSC) + 0xFFFF
}eNscClientStatus_t;

/* Enumerate the OID's owned by this package. */
typedef enum eNscDataId_e{

   D_NSC_DATAID_BOL                    /* Offset OIDs by generic NSC Id. */
    = OID_START(NSC) + D_GSC_DATAID_BOL,  

   D_NSC_PROC_STATE                    /* Overall process state          */
    = OID_START(NSC) + D_GSC_PROC_STATE,  
   D_NSC_APP_VERSION                   /* Software version number        */
    = OID_START(NSC) + D_GSC_APP_VERSION,  
   D_NSC_TOP_LEVEL_STATE               /* Node state                     */
    = OID_START(NSC) + D_GSC_TOP_LEVEL_STATE,  
   D_NSC_NODE_SHUTDOWN_COMMAND         /* Node shutdown flag             */
    = OID_START(NSC) + D_GSC_SHUTDOWN,  
   D_NSC_ENABLE                        /* Enable flag                    */
    = OID_START(NSC) + D_GSC_ENABLE,  
   D_NSC_HOME                          /* Home flag                      */
    = OID_START(NSC) + D_GSC_HOME,           
   D_NSC_STOP_DEMAND                   /* Stop demand                    */
    = OID_START(NSC) + D_GSC_STOP_DEMAND, 
   D_NSC_SAFE_STATE_COMMAND            /* Safe-state command             */
    = OID_START(NSC) + D_GSC_SAFE_STATE_COMMAND,  
   D_NSC_ACTIVATE_COMMAND              /* Activate command               */
    = OID_START(NSC) + D_GSC_ACTIVATE_COMMAND,  
   D_NSC_AXIS_STOPPED                  /* Stopped status                 */
    = OID_START(NSC) + D_GSC_AXIS_STOPPED, 
   D_NSC_HALT_DEMAND                   /* Halt demand                    */
    = OID_START(NSC) + D_GSC_HALT_DEMAND, 
   D_NSC_RECOVER_LIMIT                 /* Recover from L2 limit          */
    = OID_START(NSC) + D_GSC_RECOVER_LIMIT, 

   D_NSC_USER_REPORTED_STATE,          /* State of axis reported to user */
   D_NSC_HOMED,                        /* Flag indicating homed status   */

   D_NSC_DATAID_EOL                    /* End-of-list marker             */

}eNscDataId_t;

/* Define the states for our top-level FSM. */
typedef  enum eNscTopLevelStates_e
{
   E_NSC_TOP_STATE_PRECONTROLLING,
   E_NSC_TOP_STATE_CONFIGURING_NODE,
   E_NSC_TOP_STATE_DISABLED,
   E_NSC_TOP_STATE_ENABLED,
   E_NSC_TOP_STATE_RUNNING_NODE,
   E_NSC_TOP_STATE_INTERLOCKED,
   E_NSC_TOP_STATE_FAILED,
   E_NSC_TOP_STATE_SAFE,
   E_NSC_TOP_STATE_EXIT
}  eNscTopLevelStates_t;

/* Define the states for an overall interpretted state for the axis */
typedef  enum eNscUserAxisStates_e
{
   E_NSC_USR_STATE_BOL,       /* Beginning of list (DO NOT USE!) */

   E_NSC_USR_STATE_UNKNOWN,   /* Indeterminable state */
   E_NSC_USR_STATE_INIT,      /* Axis software initialising */
   E_NSC_USR_STATE_INTLKD,    /* Interlocked or in pendant control */
   E_NSC_USR_STATE_DISABLED,  /* Axis is currently disabled */
   E_NSC_USR_STATE_UNHOMED,   /* Axis is ready to be homed */
   E_NSC_USR_STATE_HOMING,    /* Axis is being homed */
   E_NSC_USR_STATE_IDLE,      /* Axis not moving, but ready for move/track */
   E_NSC_USR_STATE_MOVING,    /* Axis moving */
   E_NSC_USR_STATE_TRACKING,  /* Axis tracking */
   E_NSC_USR_STATE_STOPPING,  /* Axis stopping */
   E_NSC_USR_STATE_LIMITED,   /* Axis is limited, or recovering from limit */
   E_NSC_USR_STATE_SAFE,      /* Axis (and software) is currently safe */
   E_NSC_USR_STATE_FAILED,    /* Axis software/mech's has failed */

   E_NSC_USR_STATE_EOL        /* End of list (DO NOT USE!) */

}  eNscUserAxisStates_t;

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
#define E_NSC_REG_SERVER_NAME   "E_TTL_NSC"
#define E_NSC_SERVER_PROC       "Nsc"

#define E_NSC_SERVER_PATH       ""

#ifdef  E_WFL_OS_QNX6
#define E_NSC_SERVER_PRI                 45
#endif
#ifdef  E_WFL_OS_QNX4
#define E_NSC_SERVER_PRI                 15
#endif

#endif  /* _NSC_HEADER_INCLUDED */                                                    

/*
** =============================================================================
** End of header:   Nsc.h
** =============================================================================
*/

