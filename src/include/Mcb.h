/*
** Header File Name:
**    Mcb.h
**
** Purpose:
**    Public header file for Master Command Broker (MCB) functions
**
** Description:
**    This code has been written to conform to the "TTL Coding 
**    Standard"  v0.4 29-Sep-1998.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    mjf: Martyn J. Ford (TTL)
**
** Version:
**    $Id: Mcb.h,v 0.23 2001/05/08 08:14:20 mjf Exp $
**
** History:
**    $Log: Mcb.h,v $
**    Revision 0.23  2001/05/08 08:14:20  mjf
**    Change of EPT-only control flag to be partial-control. When this flag
**    is set it will only route commands from the authorised controller to
**    the EPT and SPT, although the number/identity of these processes may
**    be changed later.
**
**    Revision 0.22  2001/02/28 15:43:36  mjf
**    Addition of definition for the default EPT-only flag.
**
**    Revision 0.21  2001/02/28 15:03:43  mjf
**    Addition of EPT-only flag, which allows control messages from the
**    authorised controller to be routed if destined for the EPT only.
**    The setting of this flag and the authorisation level now done using
**    generic 'set' and 'get' commands.
**
**    Revision 0.20  2000/11/15 11:14:05  mjf
**    ÿ©MCB to have default authorisation level as well as 'none'. Currently
**    set default to TCS for design proving - the default will eventually
**    revert back to 'none'.
**
**    Revision 0.19  2000/10/11 15:32:17  mjf
**    Addition of definition of no authorisation level, and status added for
**    packet routing denied due to lack of authorisation.
**
**    Revision 0.18  2000/10/10 14:39:22  mjf
**    Updated for new generic MCP datum IDs.
**
**    Revision 0.18  2000/10/10 14:36:56  mjf
**    Updated for new generic MCP datum IDs.
**
**    Revision 0.17  2000/10/06 09:09:36  mjf
**    Addition of error in requested authorisation level.
**
**    Revision 0.16  2000/09/08 12:52:36  djm
**    Implemented the new global data enumeration.
**
**    Revision 0.15  2000/09/01 14:32:24  djm
**    Fix some incorrect comment on the global data.
**
**    Revision 0.14  2000/09/01 11:40:21  djm
**    Added newline character for strict ANSI compliance.
**
**    Revision 0.13  2000/08/31 15:51:50  djm
**    Implemented CLU package use and new global task data management.
**
**    Revision 0.12  2000/08/15 15:22:33  mjf
**    Addition of a copy of the state to data structure. Used for restoring
**    state after safe-state.
**
**    Revision 0.11  2000/07/05 10:33:18  djm
**    Now makes use of the SERVICE_START macro for services.
**
**    Revision 0.10  2000/06/21 10:04:24  djm
**    Added a new error code (and reorganised the error list.
**
**    Revision 0.9  2000/05/12 08:16:50  djm
**    Minor changes to enumerated error list.
**
**    Revision 0.8  2000/04/11 15:30:36  djm
**    Added additional global states for statistics and
**    increased verbose reporting.
**
**    Revision 0.7  2000/04/10 18:18:46  djm
**    Added MCB command set.
**
**    Revision 0.6  2000/04/07 13:57:25  djm
**    Removed local process state definition. Now using the
**    ProcState_t type defined in TtlSystem.h.
**
**    Revision 0.5  2000/04/04 16:46:52  djm
**    Added extra item to eMcbStatus_t.
**
**    Revision 0.4  2000/04/03 16:30:44  djm
**    Added some error numbers to the eMcbStatus enumerated list.
**
**    Revision 0.3  2000/03/29 14:05:40  djm
**    Removed "fake" package ID number.
**    Now using the TtlSystem.h definition.
**
**    Revision 0.2  2000/03/28 18:24:12  djm
**    Added status enumeration.
**
**    Revision 0.1  2000/03/28 15:39:27  djm
**    Initial addition to RCS archive.
**
**
*/


#ifndef MCB_H_DEFINED
#define MCB_H_DEFINED


/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_MCB_MAIN
   #define E_MCB_EXTERN
   #define E_MCB_INIT(Initialisation) = Initialisation
#else
   #define E_MCB_EXTERN extern
   #define E_MCB_INIT(Initaliasation)
#endif


/* Required include files */

#include "TtlSystem.h"    /* For Status_t definition */
#include "Mcp.h"
#include "Cil.h"          /* For definition of invalid authorisation level */


/* No authorisation level */

#define E_MCB_NO_AUTH_LEVEL   E_CIL_BOL

/* Default authorisation level */

#define E_MCB_DFLT_AUTH_LEVEL E_CIL_TCS

/* Default state of 'partial control' flag */

#define E_MCB_DFLT_PARTIAL    E_SYS_FALSE

/* Enumerate list of status/error values */

enum eMcbStatus_e
{
   E_MCB_GEN_ERR = STATUS_START(MCB),  /* Miscellaneous error */
   E_MCB_SRC_UNKNOWN,        /* Unlisted source CIL ID */
   E_MCB_DST_UNKNOWN,        /* Unknown destination CIL ID */
   E_MCB_CLA_UNKNOWN,        /* Unrecognised command line arg */
   E_MCB_CLASS_ERR,          /* Message class not accepted by MCB */
   E_MCB_SRCEQDST_ERR,       /* SrcId == DstId for ERR class message */
   E_MCB_AUTH_REQ_ERR,       /* Error in requested authorisation level */
   E_MCB_DENIED_AUTH,        /* Packet routing denied due to authorisation */
   E_MCB_COMMAND_ERR,        /* General error in incoming MCB command */
   E_MCB_COMMAND_LEN_ERR,    /* Error in incoming MCB command */
   E_MCB_COMMAND_DATUM_ERR,  /* Datum error in incoming MCB command */
   E_MCB_PARTIAL_CONTROL_ERR,/* Error in requested partial control */

   E_MCB_EOSTATUS_LIST       /* End of list marker (DO NOT USE FOR STATUS) */
};

typedef Int32_t eMcbStatus_t;


/* Clearance level for forwarding messages */

typedef Int32_t eMcbAuth_t;


/* MCB command set (services offered to other programs) */

typedef enum
{
   E_MCB_HEARTBEAT = E_MCP_HEARTBEAT,   /* Heartbeat command */
   E_MCB_SHUTDOWN  = E_MCP_SHUTDOWN,    /* Shutdown command */
   E_MCB_SAFESTATE = E_MCP_SAFESTATE,   /* Safestate command */
   E_MCB_ACTIVATE  = E_MCP_ACTIVATE,    /* Activate command */

   E_MCB_SET_1     = E_MCP_SET_1,       /* Set an MCB datum - MCP only */
   E_MCB_GET_1     = E_MCP_GET_1        /* Get an MCB datum */

} eMcbCommands_t;


/* Global data for submission to Status Database (SDB) */
typedef enum eMcbDataId_e
{

   D_MCB_DATAID_BOL = 0,     /* Begining of data list. Do not use as an ID */

   D_MCB_PROC_STATE          /* Task state(-machine) variable */
      = D_MCP_PROC_STATE,
   D_MCB_AUTH_STATE          /* Requested/granted suthorisation state */
      = D_MCP_AUTH_STATE,
   D_MCB_SYS_REQUEST         /* System requests made to the MCP */
      = D_MCP_SYS_REQUEST,
   D_MCB_APP_VERSION         /* Application version */
      = D_MCP_APP_VERSION,

   D_MCB_AUTH_LEVEL          /* Permitted command routing authorisation level */
      = D_MCP_FIRST_USER_DATUM,
   D_MCB_QTY_ROUTED,         /* No. routed CIL packets since run-time */
   D_MCB_QTY_DENIED,         /* No. denied CIL packets since run-time */
   D_MCB_QTY_HBEAT,          /* No. heartbeats received since run-time */
   D_MCB_QTY_COMMANDS,       /* No. commands the MCB received since run-time */
   D_MCB_QTY_ERRORS,         /* No. comms/data errors since run-time */
   D_MCB_PARTIAL_CONTROL,    /* Only commands for some targets may be routed */

   D_MCB_DATAID_EOL          /* End of list marker - do not use as an index */

}  eMcbDataId_t;


#endif


/* EOF */
