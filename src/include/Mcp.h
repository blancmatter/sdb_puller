/*******************************************************************************
**
**       Copyright (c) Telescope Technologies Limited (TTL) 2000-02.
** 
** Filename: 
**    Mcp.h
**
** Purpose:
**    Public header file for Master Control Process (MCP).
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v0.7.
**
** Traceability to Software Design: 
**    MCP Detailed Design v0.05.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** Version:
**    $Id: Mcp.h,v 0.50 2007/09/04 09:33:29 mjf Exp $
**
** History:
**    $Log: Mcp.h,v $
**    Revision 0.50  2007/09/04 09:33:29  mjf
**    Reword to text for Fail Override.
**
**    Revision 0.49  2007/07/13 11:25:25  mjf
**    Addition of datum to indicate if the enclosure is open, and log elapsed
**    times for usage statistics.
**
**    Revision 0.48  2005/07/14 10:31:02  mjf
**    Addition of Operational Pause system request.
**
**    Revision 0.47  2004/06/25 11:40:00  mjf
**    Addition of text for user selections.
**
**    Revision 0.46  2004/06/25 11:10:32  mjf
**    Addition of 5 user selections for PMM configuration file.
**
**    Revision 0.45  2004/06/16 14:52:51  mjf
**    Addition of System Requests to select the focal station/rotator.
**
**    Revision 0.44  2004/01/28 14:10:17  mjf
**    Addition of Force Oper On/Off to text table.
**
**    Revision 0.43  2003/08/07 08:38:55  mjf
**    Addition of system request to force the Operational On/Off flag.
**
**    Revision 0.42  2003/05/20 10:18:37  mjf
**    Addition of D_PMM_OPERATIONAL_ON, to denote whether an 'Operational On'
**    system request mas been made.
**
**    Revision 0.41  2003/02/03 11:22:50  mjf
**    Renaming of system request Start Observation to Operational On, etc.
**
**    Revision 0.40  2002/03/25 13:33:11  mjf
**    Addition of PMM datum IDs for previous MCP action.
**
**    Revision 0.39  2002/03/14 10:44:50  mjf
**    Addition of PMM datum entries to contain detail of any actions currently
**    being implemented.
**
**    Revision 0.38  2002/03/06 08:23:56  mjf
**    Addition of datum entries for APS outlet names.
**
**    Revision 0.37  2002/02/27 18:03:23  mjf
**    Additional error code for CHB.
**
**    Revision 0.36  2002/01/23 09:57:48  mjf
**    Addition of errors for UIT, AIT & MIT clearing their data from the SDB.
**
**    Revision 0.35  2002/01/23 09:56:13  mjf
**    Changes/additions to PMM error codes.
**
**    Revision 0.34  2001/11/09 10:09:19  mjf
**    Addition of string for software-only shutdown.
**
**    Revision 0.33  2001/11/07 14:24:59  mjf
**    Addition of system request for software-only shutdown.
**
**    Revision 0.32  2001/09/04 14:34:00  mjf
**    Addition of system request to re-read the start/stop observation items
**    from the configuration file.
**
**    Revision 0.31  2001/08/28 12:47:22  mjf
**    Addition of system request to cancel a current start/stop observation.
**
**    Revision 0.30  2001/07/26 14:59:37  mjf
**    Addition of static strings to explain enumerations.
**
**    Revision 0.29  2001/06/20 08:38:27  mjf
**    Additional PMM errors.
**
**    Revision 0.28  2001/06/07 09:57:39  mjf
**    Addition of low-power error for PMM.
**
**    Revision 0.27  2001/06/06 12:24:52  mjf
**    Addition of system request for full restart. SDB datums added for
**    imminent system restart or shutdown. Addition of some more error status.
**
**    Revision 0.26  2001/06/04 12:30:53  mjf
**    Addition of datum for submitting the number of times a node is rebooted,
**    and changing of 'standby' system request to 'stop observation'.
**
**    Revision 0.25  2001/05/11 08:54:37  mjf
**    Addition of PMM submitting end and start of operational time into SDB.
**
**    Revision 0.24  2001/05/08 09:39:00  mjf
**    Addition of authorisation level of 'partial' - process is only
**    authorised for system requests, and not for the routing of commands
**    via the MCB.
**
**    Revision 0.23  2001/05/03 10:15:26  mjf
**    Change of implementation of system request definitions, and addition
**    of some overrides.
**
**    Revision 0.22  2001/04/10 15:57:34  mjf
**    Addition of PMM errors.
**
**    Revision 0.21  2001/04/09 14:40:45  mjf
**    Addition of system request to start an observation, offsets within
**    system requests, and PMM status.
**
**    Revision 0.20  2001/02/27 10:59:39  mjf
**    Addition of more system requests.
**
**    Revision 0.19  2001/02/23 16:09:43  mjf
**    Addition of smart trim and smart boost to UIT datums. Completion of
**    generic datum assignments.
**
**    Revision 0.18  2001/01/04 14:20:30  mjf
**    Addition of error code for the inability to resolve a label found in the
**    configuration file.
**
**    Revision 0.17  2000/12/11 18:19:40  mjf
**    Moving of generic message service numbers into TtlSystem.h. Addition
**    of length of SET_1 command.
**
**    Revision 0.16  2000/11/22 11:59:37  mjf
**    Addition of error in contents of PMM configuration file.
**
**    Revision 0.15  2000/11/07 14:19:25  mjf
**    Removal of MCP datums for generic SDB submissions, now in TtlSystem.h.
**
**    Revision 0.14  2000/10/17 15:35:41  mjf
**    Addition of system requests and more PMM errors.
**
**    Revision 0.13  2000/10/11 15:33:58  mjf
**    Enumerated types added for requesting and granting/refusing both
**    MCB authorisation control and MCP system requests.
**
**    Revision 0.12  2000/10/10 08:58:06  mjf
**    Addition of generic SDB datums for requested/granted authorisation
**    state, and MCP system requests.
**
**    Revision 0.11  2000/10/04 14:47:55  mjf
**    Addition of AIT errors generated when interfacing to an APS.
**
**    Revision 0.10  2000/09/29 08:34:52  mjf
**    New file header in response to coding standard v0.7.
**
**    Revision 0.9  2000/09/26 12:48:40  mjf
**    Addition of errors for accessing UPS, APS and modem interface processes.
**
**    Revision 0.8  2000/09/22 10:02:20  mjf
**    Update SDB datums. Some renamed for clarity, some added for UPS & APS.
**
**    Revision 0.7  2000/09/20 08:21:50  mjf
**    Addition of data IDs and status enums for UIT, AIT and MIT.
**
**    Revision 0.6  2000/09/15 16:20:20  mjf
**    Addition of PMM error accessing hardware watchdog card.
**
**    Revision 0.5  2000/09/15 10:01:54  mjf
**    Addition of SDB datum IDs and statuses for both PMM and CHB.
**
**    Revision 0.4  2000/09/14 08:33:04  mjf
**    Addition of MCP SDB submission datums. The proc state is common to all
**    processes. Following this is the first user datum which is the first
**    that may be allocated on an application-specific basis.
**
**    Revision 0.3  2000/08/24 08:43:07  mjf
**    Addition of process state to the safe-state-completed message, and the
**    activate-completed message.
**
**    Revision 0.2  2000/08/23 10:59:15  mjf
**    Responses to safe-state and activate messages now in two stages.
**
**    Revision 0.1  2000/08/14 16:34:23  mjf
**    Initial revision.
**
**
*******************************************************************************/

#ifndef MCP_H_DEFINED
#define MCP_H_DEFINED


/*
** Include files
*/

#include "TtlSystem.h"


/*
** Macro definitions
*/

/* lengths of command messages */

#define E_MCP_HEARTBEAT_CMD_LEN   0    /* length of heartbeat message */
#define E_MCP_SHUTDOWN_CMD_LEN    0    /* length of shutdown message */
#define E_MCP_SAFESTATE_CMD_LEN   0    /* length of safe-state message */
#define E_MCP_ACTIVATE_CMD_LEN    0    /* length of activate message */
                                       /* length of set command */
#define E_MCP_SET_1_CMD_LEN       sizeof( eTtlSet_t )

/* lengths of response messages (bytes) */

#define E_MCP_HEARTBEAT_RSP_LEN   4    /* heartbeat response length */

/* lengths of acknowledgement messages (bytes) */

#define E_MCP_SHUTDOWN_ACK_LEN    0    /* shutdown acknowledgement length */

/* lengths of 'command being acted upon' messages */

#define E_MCP_SAFESTATE_ACT_LEN   0    /* safe-state being acted upon length */
#define E_MCP_ACTIVATE_ACT_LEN    0    /* activate being acted upon length */

/* lengths of 'command completed' messages */

#define E_MCP_SAFESTATE_COM_LEN   4    /* safe-state completed length */
#define E_MCP_ACTIVATE_COM_LEN    4    /* activate completed length */

/* lengths of strings for enumeration names */

#ifdef E_MCP_INCLUDE_NAMES

#define E_MCP_STRING_LEN          64

#endif


/*
** Type definitions
*/ 

/* States of MCB authorisation request/refused/obtained made via SDB */

typedef enum eMcpAuthState_e
{

   E_MCP_AUTH_BOL,                     /* Beginning of list */

   E_MCP_AUTH_NONE,                    /* Authorisation is not requested */
   E_MCP_AUTH_REQUEST,                 /* Application requests authorisation */
   E_MCP_AUTH_GRANTED,                 /* Application granted authorisation */
   E_MCP_AUTH_REFUSED,                 /* Application refused authorisation */
   E_MCP_AUTH_SYSREQ_ONLY,             /* Application can make system requests only */

   E_MCP_AUTH_EOL                      /* End of list marker */

} eMcpAuthState_t;

/* Enumeration of MCP system requests */

typedef enum eMcpSysReqIndex_e
{
   E_MCP_SYSREQ_NULL = 0,              /* No system request (dummy) */
   E_MCP_SYSREQ_SAFE_STATE,            /* Request for safe-state */
   E_MCP_SYSREQ_ACTIVATE,              /* Request for activate following safe-state */
   E_MCP_SYSREQ_FULL_SHUTDOWN,         /* Request for full shutdown */
   E_MCP_SYSREQ_FULL_STARTUP,          /* Request for full start-up (unused) */
   E_MCP_SYSREQ_PARTIAL_SHUTDOWN,      /* Request for partial shutdown */
   E_MCP_SYSREQ_PARTIAL_STARTUP,       /* Request for partial start-up */
   E_MCP_SYSREQ_OPERATIONAL_ON,        /* Request for operational on */
   E_MCP_SYSREQ_OPERATIONAL_OFF,       /* Request for operational off */
   E_MCP_SYSREQ_IMMED_SHUTDOWN,        /* Request for immediate full shutdown */
   E_MCP_SYSREQ_FULL_RESTART,          /* Request for full system restart */
   E_MCP_SYSREQ_FAIL_OVERRIDE,         /* Request for shutdown on failure override */
   E_MCP_SYSREQ_FAIL_RESTORE,          /* Request to cancel shutdown on failure override */
   E_MCP_SYSREQ_TIME_OVERRIDE,         /* Request for operational time override */
   E_MCP_SYSREQ_TIME_RESTORE,          /* Request to cancel operational time override */
   E_MCP_SYSREQ_WMS_OVERRIDE,          /* Request for bad weather override */
   E_MCP_SYSREQ_WMS_RESTORE,           /* Request to cancel bad weather override */
   E_MCP_SYSREQ_EPT_OVERRIDE,          /* Request for EPT problem override */
   E_MCP_SYSREQ_EPT_RESTORE,           /* Request to cancel EPT problem override */
   E_MCP_SYSREQ_SPT_OVERRIDE,          /* Request for SPT problem override */
   E_MCP_SYSREQ_SPT_RESTORE,           /* Request to cancel SPT problem override */
   E_MCP_SYSREQ_NODE_OVERRIDE,         /* Request for node problem override */
   E_MCP_SYSREQ_NODE_RESTORE,          /* Request to cancel node problem override */
   E_MCP_SYSREQ_CANCEL_OPER_ON,        /* Request to cancel an operational on */
   E_MCP_SYSREQ_CANCEL_OPER_OFF,       /* Request to cancel an operational off */
   E_MCP_SYSREQ_REREAD_CFG,            /* Request to re-read sections of config file */
   E_MCP_SYSREQ_SOFT_SHUTDOWN,         /* Request for software-only shutdown */
   E_MCP_SYSREQ_FORCE_OPER_ON,         /* Request to force that operational on already done */
   E_MCP_SYSREQ_FORCE_OPER_OFF,        /* Request to force that operational off already done */
   E_MCP_SYSREQ_OPERATIONAL_PAUSE,     /* Request for operational pause */
   E_MCP_SYSREQ_FOCUS_CASS,            /* Request to select Cassegrain focal station */
   E_MCP_SYSREQ_FOCUS_NASP,            /* Request to select Nasmyth+ focal station */
   E_MCP_SYSREQ_FOCUS_NASM,            /* Request to select Nasmyth- focal station */
   E_MCP_SYSREQ_FOCUS_PRIME,           /* Request to select prime focal station */
   E_MCP_SYSREQ_SEL_USER_1,            /* Request to select user option 1 */
   E_MCP_SYSREQ_SEL_USER_2,            /* Request to select user option 2 */
   E_MCP_SYSREQ_SEL_USER_3,            /* Request to select user option 3 */
   E_MCP_SYSREQ_SEL_USER_4,            /* Request to select user option 4 */
   E_MCP_SYSREQ_SEL_USER_5,            /* Request to select user option 5 */

   E_MCP_SYSREQ_EOL                    /* End of list marker */


} eMcpSysReqIndex_t;

/* Offsets within system requests - same order as eMcpSysRequest_e below */

typedef enum eMcpSysReqOffset_e
{

   E_MCP_SYSREQ_REQ = 0,               /* System request */
   E_MCP_SYSREQ_ACT,                   /* Action in progress */
   E_MCP_SYSREQ_COM,                   /* Action completed */
   E_MCP_SYSREQ_ERR,                   /* Unable to perform action */

   E_MCP_SYSREQ_OFFSET                 /* Size of offset between system requests */

} eMcpSysReqOffset_t;

/* 
** States of MCP system requests made in datum for each process in the SDB. 
** These are the definitions that should be submitted and retrieved to/from the
** SDB to implement system requests.
*/

typedef enum eMcpSysRequest_e
{

   E_MCP_SYSREQ_NONE                  = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NULL ),
   E_MCP_SYSREQ_UNAUTHORISED          = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NULL ),

   E_MCP_SYSREQ_REQ_SAFE_STATE        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SAFE_STATE ),
   E_MCP_SYSREQ_ACT_SAFE_STATE        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SAFE_STATE ),
   E_MCP_SYSREQ_COM_SAFE_STATE        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SAFE_STATE ),
   E_MCP_SYSREQ_ERR_SAFE_STATE        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SAFE_STATE ),

   E_MCP_SYSREQ_REQ_ACTIVATE          = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_ACTIVATE ),
   E_MCP_SYSREQ_ACT_ACTIVATE          = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_ACTIVATE ),
   E_MCP_SYSREQ_COM_ACTIVATE          = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_ACTIVATE ),
   E_MCP_SYSREQ_ERR_ACTIVATE          = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_ACTIVATE ),

   E_MCP_SYSREQ_REQ_FULL_SHUTDOWN     = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_SHUTDOWN ),
   E_MCP_SYSREQ_ACT_FULL_SHUTDOWN     = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_SHUTDOWN ),
   E_MCP_SYSREQ_COM_FULL_SHUTDOWN     = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_SHUTDOWN ),
   E_MCP_SYSREQ_ERR_FULL_SHUTDOWN     = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_SHUTDOWN ),

   E_MCP_SYSREQ_REQ_FULL_STARTUP      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_STARTUP ),
   E_MCP_SYSREQ_ACT_FULL_STARTUP      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_STARTUP ),
   E_MCP_SYSREQ_COM_FULL_STARTUP      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_STARTUP ),
   E_MCP_SYSREQ_ERR_FULL_STARTUP      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_STARTUP ),
                                             
   E_MCP_SYSREQ_REQ_PARTIAL_SHUTDOWN  = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_SHUTDOWN ),
   E_MCP_SYSREQ_ACT_PARTIAL_SHUTDOWN  = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_SHUTDOWN ),
   E_MCP_SYSREQ_COM_PARTIAL_SHUTDOWN  = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_SHUTDOWN ),
   E_MCP_SYSREQ_ERR_PARTIAL_SHUTDOWN  = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_SHUTDOWN ),
   
   E_MCP_SYSREQ_REQ_PARTIAL_STARTUP   = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_STARTUP ),
   E_MCP_SYSREQ_ACT_PARTIAL_STARTUP   = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_STARTUP ),
   E_MCP_SYSREQ_COM_PARTIAL_STARTUP   = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_STARTUP ),
   E_MCP_SYSREQ_ERR_PARTIAL_STARTUP   = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_PARTIAL_STARTUP ),

   E_MCP_SYSREQ_REQ_OPERATIONAL_ON    = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_ON  ),
   E_MCP_SYSREQ_ACT_OPERATIONAL_ON    = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_ON  ),
   E_MCP_SYSREQ_COM_OPERATIONAL_ON    = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_ON  ),
   E_MCP_SYSREQ_ERR_OPERATIONAL_ON    = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_ON  ),

   E_MCP_SYSREQ_REQ_OPERATIONAL_OFF   = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_OFF ),
   E_MCP_SYSREQ_ACT_OPERATIONAL_OFF   = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_OFF ),
   E_MCP_SYSREQ_COM_OPERATIONAL_OFF   = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_OFF ),
   E_MCP_SYSREQ_ERR_OPERATIONAL_OFF   = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_OFF ),

   E_MCP_SYSREQ_REQ_IMMED_SHUTDOWN    = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_IMMED_SHUTDOWN ),
   E_MCP_SYSREQ_ACT_IMMED_SHUTDOWN    = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_IMMED_SHUTDOWN ),
   E_MCP_SYSREQ_COM_IMMED_SHUTDOWN    = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_IMMED_SHUTDOWN ),
   E_MCP_SYSREQ_ERR_IMMED_SHUTDOWN    = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_IMMED_SHUTDOWN ),

   E_MCP_SYSREQ_REQ_FULL_RESTART      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_RESTART ),
   E_MCP_SYSREQ_ACT_FULL_RESTART      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_RESTART ),
   E_MCP_SYSREQ_COM_FULL_RESTART      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_RESTART ),
   E_MCP_SYSREQ_ERR_FULL_RESTART      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FULL_RESTART ),

   E_MCP_SYSREQ_REQ_FAIL_OVERRIDE     = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_OVERRIDE ),
   E_MCP_SYSREQ_ACT_FAIL_OVERRIDE     = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_OVERRIDE ),
   E_MCP_SYSREQ_COM_FAIL_OVERRIDE     = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_OVERRIDE ),
   E_MCP_SYSREQ_ERR_FAIL_OVERRIDE     = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_OVERRIDE ),

   E_MCP_SYSREQ_REQ_FAIL_RESTORE      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_RESTORE ),
   E_MCP_SYSREQ_ACT_FAIL_RESTORE      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_RESTORE ),
   E_MCP_SYSREQ_COM_FAIL_RESTORE      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_RESTORE ),
   E_MCP_SYSREQ_ERR_FAIL_RESTORE      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FAIL_RESTORE ),

   E_MCP_SYSREQ_REQ_TIME_OVERRIDE     = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_OVERRIDE ),
   E_MCP_SYSREQ_ACT_TIME_OVERRIDE     = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_OVERRIDE ),
   E_MCP_SYSREQ_COM_TIME_OVERRIDE     = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_OVERRIDE ),
   E_MCP_SYSREQ_ERR_TIME_OVERRIDE     = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_OVERRIDE ),

   E_MCP_SYSREQ_REQ_TIME_RESTORE      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_RESTORE ),
   E_MCP_SYSREQ_ACT_TIME_RESTORE      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_RESTORE ),
   E_MCP_SYSREQ_COM_TIME_RESTORE      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_RESTORE ),
   E_MCP_SYSREQ_ERR_TIME_RESTORE      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_TIME_RESTORE ),

   E_MCP_SYSREQ_REQ_WMS_OVERRIDE      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_OVERRIDE ),
   E_MCP_SYSREQ_ACT_WMS_OVERRIDE      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_OVERRIDE ),
   E_MCP_SYSREQ_COM_WMS_OVERRIDE      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_OVERRIDE ),
   E_MCP_SYSREQ_ERR_WMS_OVERRIDE      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_OVERRIDE ),

   E_MCP_SYSREQ_REQ_WMS_RESTORE       = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_RESTORE ),
   E_MCP_SYSREQ_ACT_WMS_RESTORE       = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_RESTORE ),
   E_MCP_SYSREQ_COM_WMS_RESTORE       = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_RESTORE ),
   E_MCP_SYSREQ_ERR_WMS_RESTORE       = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_WMS_RESTORE ),

   E_MCP_SYSREQ_REQ_EPT_OVERRIDE      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_OVERRIDE ),
   E_MCP_SYSREQ_ACT_EPT_OVERRIDE      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_OVERRIDE ),
   E_MCP_SYSREQ_COM_EPT_OVERRIDE      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_OVERRIDE ),
   E_MCP_SYSREQ_ERR_EPT_OVERRIDE      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_OVERRIDE ),
                                                                                                            
   E_MCP_SYSREQ_REQ_EPT_RESTORE       = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_RESTORE ),
   E_MCP_SYSREQ_ACT_EPT_RESTORE       = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_RESTORE ),
   E_MCP_SYSREQ_COM_EPT_RESTORE       = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_RESTORE ),
   E_MCP_SYSREQ_ERR_EPT_RESTORE       = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_EPT_RESTORE ),

   E_MCP_SYSREQ_REQ_SPT_OVERRIDE      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_OVERRIDE ),
   E_MCP_SYSREQ_ACT_SPT_OVERRIDE      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_OVERRIDE ),
   E_MCP_SYSREQ_COM_SPT_OVERRIDE      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_OVERRIDE ),
   E_MCP_SYSREQ_ERR_SPT_OVERRIDE      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_OVERRIDE ),
                                                                                                            
   E_MCP_SYSREQ_REQ_SPT_RESTORE       = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_RESTORE ),
   E_MCP_SYSREQ_ACT_SPT_RESTORE       = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_RESTORE ),
   E_MCP_SYSREQ_COM_SPT_RESTORE       = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_RESTORE ),
   E_MCP_SYSREQ_ERR_SPT_RESTORE       = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SPT_RESTORE ),

   E_MCP_SYSREQ_REQ_NODE_OVERRIDE     = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_OVERRIDE ),
   E_MCP_SYSREQ_ACT_NODE_OVERRIDE     = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_OVERRIDE ),
   E_MCP_SYSREQ_COM_NODE_OVERRIDE     = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_OVERRIDE ),
   E_MCP_SYSREQ_ERR_NODE_OVERRIDE     = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_OVERRIDE ),
                                                                                                            
   E_MCP_SYSREQ_REQ_NODE_RESTORE      = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_RESTORE ),
   E_MCP_SYSREQ_ACT_NODE_RESTORE      = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_RESTORE ),
   E_MCP_SYSREQ_COM_NODE_RESTORE      = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_RESTORE ),
   E_MCP_SYSREQ_ERR_NODE_RESTORE      = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_NODE_RESTORE ),

   E_MCP_SYSREQ_REQ_CANCEL_OPER_ON    = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_ON  ),
   E_MCP_SYSREQ_ACT_CANCEL_OPER_ON    = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_ON  ),
   E_MCP_SYSREQ_COM_CANCEL_OPER_ON    = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_ON  ),
   E_MCP_SYSREQ_ERR_CANCEL_OPER_ON    = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_ON  ),

   E_MCP_SYSREQ_REQ_CANCEL_OPER_OFF   = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_OFF ),
   E_MCP_SYSREQ_ACT_CANCEL_OPER_OFF   = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_OFF ),
   E_MCP_SYSREQ_COM_CANCEL_OPER_OFF   = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_OFF ),
   E_MCP_SYSREQ_ERR_CANCEL_OPER_OFF   = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_CANCEL_OPER_OFF ),

   E_MCP_SYSREQ_REQ_REREAD_CFG        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_REREAD_CFG ),
   E_MCP_SYSREQ_ACT_REREAD_CFG        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_REREAD_CFG ),
   E_MCP_SYSREQ_COM_REREAD_CFG        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_REREAD_CFG ),
   E_MCP_SYSREQ_ERR_REREAD_CFG        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_REREAD_CFG ),

   E_MCP_SYSREQ_REQ_SOFT_SHUTDOWN     = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SOFT_SHUTDOWN ),
   E_MCP_SYSREQ_ACT_SOFT_SHUTDOWN     = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SOFT_SHUTDOWN ),
   E_MCP_SYSREQ_COM_SOFT_SHUTDOWN     = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SOFT_SHUTDOWN ),
   E_MCP_SYSREQ_ERR_SOFT_SHUTDOWN     = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SOFT_SHUTDOWN ),

   E_MCP_SYSREQ_REQ_FORCE_OPER_ON     = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_ON  ),
   E_MCP_SYSREQ_ACT_FORCE_OPER_ON     = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_ON  ),
   E_MCP_SYSREQ_COM_FORCE_OPER_ON     = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_ON  ),
   E_MCP_SYSREQ_ERR_FORCE_OPER_ON     = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_ON  ),

   E_MCP_SYSREQ_REQ_FORCE_OPER_OFF    = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_OFF ),
   E_MCP_SYSREQ_ACT_FORCE_OPER_OFF    = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_OFF ),
   E_MCP_SYSREQ_COM_FORCE_OPER_OFF    = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_OFF ),
   E_MCP_SYSREQ_ERR_FORCE_OPER_OFF    = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FORCE_OPER_OFF ),

   E_MCP_SYSREQ_REQ_OPERATIONAL_PAUSE = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_PAUSE ),
   E_MCP_SYSREQ_ACT_OPERATIONAL_PAUSE = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_PAUSE ),
   E_MCP_SYSREQ_COM_OPERATIONAL_PAUSE = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_PAUSE ),
   E_MCP_SYSREQ_ERR_OPERATIONAL_PAUSE = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_OPERATIONAL_PAUSE ),

   E_MCP_SYSREQ_REQ_FOCUS_CASS        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_CASS  ),
   E_MCP_SYSREQ_ACT_FOCUS_CASS        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_CASS  ),
   E_MCP_SYSREQ_COM_FOCUS_CASS        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_CASS  ),
   E_MCP_SYSREQ_ERR_FOCUS_CASS        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_CASS  ),
                                                                                                      
   E_MCP_SYSREQ_REQ_FOCUS_NASP        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASP  ),
   E_MCP_SYSREQ_ACT_FOCUS_NASP        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASP  ),
   E_MCP_SYSREQ_COM_FOCUS_NASP        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASP  ),
   E_MCP_SYSREQ_ERR_FOCUS_NASP        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASP  ),
                                                                                                      
   E_MCP_SYSREQ_REQ_FOCUS_NASM        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASM  ),
   E_MCP_SYSREQ_ACT_FOCUS_NASM        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASM  ),
   E_MCP_SYSREQ_COM_FOCUS_NASM        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASM  ),
   E_MCP_SYSREQ_ERR_FOCUS_NASM        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_NASM  ),
                                                                                                      
   E_MCP_SYSREQ_REQ_FOCUS_PRIME       = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_PRIME ),
   E_MCP_SYSREQ_ACT_FOCUS_PRIME       = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_PRIME ),
   E_MCP_SYSREQ_COM_FOCUS_PRIME       = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_PRIME ),
   E_MCP_SYSREQ_ERR_FOCUS_PRIME       = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_FOCUS_PRIME ),
                                                                                                      
   E_MCP_SYSREQ_REQ_SEL_USER_1        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_1  ),
   E_MCP_SYSREQ_ACT_SEL_USER_1        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_1  ),
   E_MCP_SYSREQ_COM_SEL_USER_1        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_1  ),
   E_MCP_SYSREQ_ERR_SEL_USER_1        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_1  ),
                                                                                                      
   E_MCP_SYSREQ_REQ_SEL_USER_2        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_2  ),
   E_MCP_SYSREQ_ACT_SEL_USER_2        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_2  ),
   E_MCP_SYSREQ_COM_SEL_USER_2        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_2  ),
   E_MCP_SYSREQ_ERR_SEL_USER_2        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_2  ),
                                                                                                      
   E_MCP_SYSREQ_REQ_SEL_USER_3        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_3  ),
   E_MCP_SYSREQ_ACT_SEL_USER_3        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_3  ),
   E_MCP_SYSREQ_COM_SEL_USER_3        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_3  ),
   E_MCP_SYSREQ_ERR_SEL_USER_3        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_3  ),
                                                                                                      
   E_MCP_SYSREQ_REQ_SEL_USER_4        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_4  ),
   E_MCP_SYSREQ_ACT_SEL_USER_4        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_4  ),
   E_MCP_SYSREQ_COM_SEL_USER_4        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_4  ),
   E_MCP_SYSREQ_ERR_SEL_USER_4        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_4  ),
                                                                                                      
   E_MCP_SYSREQ_REQ_SEL_USER_5        = E_MCP_SYSREQ_REQ + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_5  ),
   E_MCP_SYSREQ_ACT_SEL_USER_5        = E_MCP_SYSREQ_ACT + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_5  ),
   E_MCP_SYSREQ_COM_SEL_USER_5        = E_MCP_SYSREQ_COM + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_5  ),
   E_MCP_SYSREQ_ERR_SEL_USER_5        = E_MCP_SYSREQ_ERR + ( E_MCP_SYSREQ_OFFSET * E_MCP_SYSREQ_SEL_USER_5  )
                                                                                                      
} eMcpSysRequest_t;

/* Global data submitted to the SDB by the PMM */

typedef enum ePmmDataId_e
{

   D_PMM_DATAID_BOL = 0,               /* Beginning of data list */

   D_PMM_PROC_STATE = D_MCP_PROC_STATE,   /* PMM process state */
   D_PMM_AUTH_STATE = D_MCP_AUTH_STATE,   /* PMM authorisation state */
   D_PMM_SYS_REQUEST = D_MCP_SYS_REQUEST, /* PMM process state */
   D_PMM_APP_VERSION = D_MCP_APP_VERSION, /* PMM process state */

   D_PMM_TELESCOPE_STATE               /* Combined overall telescope state */
      = D_MCP_FIRST_USER_DATUM, 
   D_PMM_SYSTEM_STATE,                 /* Combined overall system state */
   D_PMM_SOFTWARE_STATE,               /* Combined state of all software processes */
   D_PMM_POWER_STATE,                  /* Combined state of power supplies */ 
   D_PMM_SAFESTATE_QTY,                /* Number of times all processes taken to safe-state */
   D_PMM_ACTIVATE_QTY,                 /* Number of times all processes activated after safe-state */
   D_PMM_RECOVERED_QTY,                /* Number of times process recovery has been performed */
   D_PMM_RESTARTED_QTY,                /* Number of times process restart has been performed */
   D_PMM_REBOOTED_QTY,                 /* Number of times a node reboot has been performed */
   D_PMM_SYSREQ_AUTH_ONLY,             /* Software controller is only authorised for system requests */
   D_PMM_OPERATIONAL_END_ELAPSED,      /* End of operational time (UTC), secs since 1970 */
   D_PMM_OPERATIONAL_START_ELAPSED,    /* Start of operational time (UTC), secs since 1970 */
   D_PMM_OPERATIONAL_END_HOUR,         /* Hour of the end of operational time (UTC) */
   D_PMM_OPERATIONAL_END_MINUTE,       /* Minute of the end of operational time (UTC) */
   D_PMM_OPERATIONAL_START_HOUR,       /* Hour of the start of operational time (UTC) */
   D_PMM_OPERATIONAL_START_MINUTE,     /* Minute of the start of operational time (UTC) */
   D_PMM_WITHIN_OPERATIONAL_TIME,      /* TRUE if within operational time */
   D_PMM_OVERRIDE_SYS_FAIL,            /* If overriding the automatic shutdown on failure */
   D_PMM_OVERRIDE_TIME_OF_DAY,         /* If overriding the time of day checking */
   D_PMM_OVERRIDE_WMS,                 /* If overriding the state of the WMS */
   D_PMM_OVERRIDE_EPT,                 /* If overriding the state of the EPT */
   D_PMM_OVERRIDE_SPT,                 /* If overriding the state of the SPT */
   D_PMM_OVERRIDE_NODE,                /* If overriding the states of the nodes */
   D_PMM_SYSTEM_RESTART_IMMINENT,      /* If a full system restart is imminent */
   D_PMM_SYSTEM_SHUTDOWN_IMMINENT,     /* If a full system shutdown is imminent */
   D_PMM_ACTION_CURRENT,               /* Current action in progress */
   D_PMM_ACTION_TARGET,                /* Target process for a current action */
   D_PMM_ACTION_DATUM,                 /* Datum relevant to current action */
   D_PMM_ACTION_VALUE,                 /* Value relevant to current action */
   D_PMM_ACTION_START,                 /* Start time for current action (secs) */
   D_PMM_ACTION_TIMEOUT,               /* Timeout time for current action (secs) */
   D_PMM_PREV_ACTION,                  /* Previous action */
   D_PMM_PREV_TARGET,                  /* Target process for previous action */
   D_PMM_PREV_DATUM,                   /* Datum relevant to previous action */
   D_PMM_PREV_VALUE,                   /* Value relevant to previous action */
   D_PMM_PREV_START,                   /* Start time for previous action (secs) */
   D_PMM_PREV_TIMEOUT,                 /* Timeout time for previous action (secs) */
   D_PMM_OPERATIONAL_ON,               /* If an 'Operational On' has been requested */
   D_PMM_CURRENT_SELECTION,            /* SSE encoded current selection */
   D_PMM_ENCLOSURE_OPEN,               /* If the enclosure is currently open */
   D_PMM_TIME_OP_HOURS,                /* Elapsed time of Operational Hours */
   D_PMM_TIME_OPER_ON,                 /* Elapsed time of Operational mode On */
   D_PMM_TIME_OBS_POSSIBLE,            /* Elapsed time of possible observing */
   D_PMM_TIME_ENCL_OPEN,               /* Elapsed time of enclosure open */
   D_PMM_TIME_SYSTEM_OKAY,             /* Elapsed time of System Okay state */
   D_PMM_TIME_SYSTEM_WARN,             /* Elapsed time of System Warn state */
   D_PMM_TIME_SYSTEM_SUSPEND,          /* Elapsed time of System Suspend state */
   D_PMM_TIME_SYSTEM_STANDBY,          /* Elapsed time of System Standby state */
   D_PMM_TIME_TELESCOPE_OKAY,          /* Elapsed time of Telescope Okay state */   
   D_PMM_TIME_TELESCOPE_WARN,          /* Elapsed time of Telescope Warn state */   
   D_PMM_TIME_TELESCOPE_SUSPEND,       /* Elapsed time of Telescope Suspend state */
   D_PMM_TIME_TELESCOPE_STANDBY,       /* Elapsed time of Telescope Standby state */
   D_PMM_TIME_SERVICES_OKAY,           /* Elapsed time of Services Okay state */   
   D_PMM_TIME_SERVICES_WARN,           /* Elapsed time of Services Warn state */   
   D_PMM_TIME_SERVICES_SUSPEND,        /* Elapsed time of Services Suspend state */
   D_PMM_TIME_SERVICES_STANDBY,        /* Elapsed time of Services Standby state */
   D_PMM_TIME_ENCLOSURE_OKAY,          /* Elapsed time of Enclosure Okay state */   
   D_PMM_TIME_ENCLOSURE_WARN,          /* Elapsed time of Enclosure Warn state */   
   D_PMM_TIME_ENCLOSURE_SUSPEND,       /* Elapsed time of Enclosure Suspend state */
   D_PMM_TIME_ENCLOSURE_STANDBY,       /* Elapsed time of Enclosure Standby state */
   D_PMM_TIME_WEATHER_OKAY,            /* Elapsed time of Weather Okay state */   
   D_PMM_TIME_WEATHER_WARN,            /* Elapsed time of Weather Warn state */   
   D_PMM_TIME_WEATHER_SUSPEND,         /* Elapsed time of Weather Suspend state */
   D_PMM_TIME_WEATHER_STANDBY,         /* Elapsed time of Weather Standby state */

   D_PMM_DATAID_EOL                    /* End of list marker */

} ePmmDataId_t;

/* Global data submitted to the SDB by the CHB */

typedef enum eChbDataId_e
{

   D_CHB_DATAID_BOL = 0,               /* Beginning of data list */

   D_CHB_PROC_STATE = D_MCP_PROC_STATE,   /* CHB process state */
   D_CHB_AUTH_STATE = D_MCP_AUTH_STATE,   /* CHB authorisation state */
   D_CHB_SYS_REQUEST = D_MCP_SYS_REQUEST, /* CHB process state */
   D_CHB_APP_VERSION = D_MCP_APP_VERSION, /* CHB process state */

   D_CHB_HEARTBEAT_FREQ                /* Heartbeat frequency */
      = D_MCP_FIRST_USER_DATUM,     
   D_CHB_TX_QTY,                       /* Number of elapsed heartbeat periods */
   D_CHB_RX_ACCUM,                     /* Total number of received heartbeat responses */
   D_CHB_REQUIRED_RSP,                 /* Number of expected heartbeat responses per period */
   D_CHB_ACTUAL_RSP,                   /* Number of heartbeat responses received in last period */

   D_CHB_DATAID_EOL                    /* End of list marker */

} eChbDataId_t;

/* Global data for submission to Status Database (SDB) */

typedef enum eUitDataId_e
{
   D_UIT_DATAID_BOL = 0,               /* Beginning of data list */

   D_UIT_PROC_STATE = D_MCP_PROC_STATE,   /* UIT process state */
   D_UIT_AUTH_STATE = D_MCP_AUTH_STATE,   /* UIT authorisation state */
   D_UIT_SYS_REQUEST = D_MCP_SYS_REQUEST, /* UIT process state */
   D_UIT_APP_VERSION = D_MCP_APP_VERSION, /* UIT process state */
                                       
   D_UIT_ON_LINE                       /* UPS power obtained from line supply */
      = D_MCP_FIRST_USER_DATUM,
   D_UIT_ON_BATTERY,                   /* UPS power obtained from battery */
   D_UIT_BATTERY_LOW,                  /* UPS battery level low */
   D_UIT_REPLACE_BATTERY,              /* Replace UPS battery */
   D_UIT_OVERLOADED_OUTPUT,            /* UPS power output overloaded */ 
   D_UIT_BATTERY_VOLTAGE,              /* UPS battery voltage */
   D_UIT_LINE_INPUT_VOLTAGE,           /* UPS line input voltage */
   D_UIT_OUTPUT_VOLTAGE,               /* UPS output voltage */
   D_UIT_POWER_LOAD,                   /* UPS power load */
   D_UIT_BATTERY_LEVEL,                /* UPS battery level */
   D_UIT_ESTIMATED_RUNTIME,            /* UPS estimated runtime on battery */
   D_UIT_INTERNAL_TEMPERATURE,         /* UPS internal temperature */
   D_UIT_SMART_TRIM,                   /* UPS is compensating for a high voltage */
   D_UIT_SMART_BOOST,                  /* UPS is compensating for a low voltage */

   D_UIT_DATAID_EOL                    /* End of list marker */
} eUitDataId_t;

/* Global data for submission to Status Database (SDB) */

typedef enum eAitDataId_e
{
   D_AIT_DATAID_BOL = 0,               /* Beginning of data list */

   D_AIT_PROC_STATE = D_MCP_PROC_STATE,   /* AIT process state */
   D_AIT_AUTH_STATE = D_MCP_AUTH_STATE,   /* AIT authorisation state */
   D_AIT_SYS_REQUEST = D_MCP_SYS_REQUEST, /* AIT process state */
   D_AIT_APP_VERSION = D_MCP_APP_VERSION, /* AIT process state */
                                       
   D_AIT_OUTLET_1                      /* Status of APS outlet 1 */
      = D_MCP_FIRST_USER_DATUM,
   D_AIT_OUTLET_2,                     /* Status of APS outlet 2 */
   D_AIT_OUTLET_3,                     /* Status of APS outlet 3 */
   D_AIT_OUTLET_4,                     /* Status of APS outlet 4 */
   D_AIT_OUTLET_5,                     /* Status of APS outlet 5 */
   D_AIT_OUTLET_6,                     /* Status of APS outlet 6 */
   D_AIT_OUTLET_7,                     /* Status of APS outlet 7 */
   D_AIT_OUTLET_8,                     /* Status of APS outlet 8 */

   D_AIT_NAME_1,                       /* Name of APS outlet 1 */
   D_AIT_NAME_2,                       /* Name of APS outlet 2 */
   D_AIT_NAME_3,                       /* Name of APS outlet 3 */
   D_AIT_NAME_4,                       /* Name of APS outlet 4 */
   D_AIT_NAME_5,                       /* Name of APS outlet 5 */
   D_AIT_NAME_6,                       /* Name of APS outlet 6 */
   D_AIT_NAME_7,                       /* Name of APS outlet 7 */
   D_AIT_NAME_8,                       /* Name of APS outlet 8 */

   D_AIT_DATAID_EOL                    /* End of list marker */
} eAitDataId_t;

/* Global data for submission to Status Database (SDB) */

typedef enum eMitDataId_e
{
   D_MIT_DATAID_BOL = 0,               /* Beginning of data list */

   D_MIT_PROC_STATE = D_MCP_PROC_STATE,   /* MIT process state */
   D_MIT_AUTH_STATE = D_MCP_AUTH_STATE,   /* MIT authorisation state */
   D_MIT_SYS_REQUEST = D_MCP_SYS_REQUEST, /* MIT process state */
   D_MIT_APP_VERSION = D_MCP_APP_VERSION, /* MIT process state */
                                       
   D_MIT_PARAM_1 = D_MCP_FIRST_USER_DATUM,  /* First user-available datum */ 
   D_MIT_PARAM_2,                           /* Second user-available datum */
   D_MIT_PARAM_3,                           /* Third user-available datum */ 

   D_MIT_DATAID_EOL                    /* End of list marker */
} eMitDataId_t;


/* Status and errors generated by the PMM */

typedef enum ePmmStatus_e
{

   E_PMM_ERR_GEN = STATUS_START( MCP ),/* Miscellaneous error */
   E_PMM_ERR_SETUP,                    /* Error performing setup */
   E_PMM_ERR_CONFIG_FILE,              /* Error accessing the configuration file */
   E_PMM_ERR_CONFIG_PARAMS,            /* Error in contents of configuration file */
   E_PMM_ERR_ITEM_COUNT,               /* Too many start-up or shutdown items */
   E_PMM_ERR_CIL_RX,                   /* Unexpected CIL message received */
   E_PMM_ERR_CIL_NAME,                 /* Error matching CIL name */
   E_PMM_ERR_STALE_STATES,             /* No fresh process states from CHB */
   E_PMM_ERR_STALE_UPS_STATUS,         /* No fresh status received from a UPS */
   E_PMM_ERR_STALE_APS_STATUS,         /* No fresh status received from an APS */
   E_PMM_ERR_STALE_MODEM_STATUS,       /* No fresh status received from the modem */
   E_PMM_ERR_SPAWN,                    /* Error spawning software process */
   E_PMM_ERR_SAFE_ATTAIN,              /* Software process unable to attain safe-state */
   E_PMM_ERR_ACTIVATE,                 /* Software process unable to be activated */
   E_PMM_ERR_MEM_ALLOC,                /* Error allocating memory */
   E_PMM_ERR_WATCHDOG,                 /* Error interfacing to hardware watchdog */
   E_PMM_ERR_UPS,                      /* UPS unable to respond to command */
   E_PMM_ERR_APS,                      /* APS unable to respond to command */
   E_PMM_ERR_MODEM,                    /* Modem unable to respond to command */
   E_PMM_ERR_AUTH_UNITS,               /* Authorisation state read with incorrect units */
   E_PMM_ERR_AUTH_LEVEL,               /* Disallowed MCB authorisation request */
   E_PMM_ERR_SYSREQ_UNITS,             /* System request read with incorrect units */
   E_PMM_ERR_SYSREQ_AUTH,              /* Unauthorised system request made */
   E_PMM_ERR_SYSREQ_CMD,               /* Error in system request */
   E_PMM_ERR_SYSREQ_COMPLETE,          /* Unable to complete system request */
   E_PMM_ERR_SYSREQ_REFUSED,           /* System request refused due to system state */
   E_PMM_ERR_UNRESOLVED_LABEL,         /* Unable to resolve label in look-up table */
   E_PMM_ERR_SYSTEM_FAILURE,           /* System is in failed state */
   E_PMM_ERR_FAILURE_RESTART,          /* System being restarted due to failure */
   E_PMM_ERR_OPERATIONAL_ON,           /* Unable to execute operational on request */
   E_PMM_ERR_STANDBY,                  /* Unable to execute system standby request */
   E_PMM_ERR_OP_HOURS_FILE,            /* Error accessing the operational hours file */
   E_PMM_ERR_OP_HOURS_PARAMS,          /* Error in contents of operational hours file */
   E_PMM_ERR_OP_HOURS,                 /* Error encountered checking the operational hours */
   E_PMM_ERR_REPEATED_PROCESS_FAIL,    /* Repeated failure of a process */
   E_PMM_ERR_REPEATED_SYSTEM_FAIL,     /* Repeated failure of the whole system */
   E_PMM_ERR_POWER_LOW,                /* Detected low power causing system shutdown */
   E_PMM_ERR_SET_COMMAND,              /* Set command failed to complete */
   E_PMM_ERR_GET_COMMAND,              /* Get command failed to complete */
   E_PMM_ERR_PREV_GET_COMMAND,         /* Previous 'get' command failed for use in 'set' */
   E_PMM_ERR_PROC_READY,               /* Wait for process to be ready failed to complete */
   E_PMM_ERR_CALLOUT,                  /* Callout for intervention failed to complete */
   E_PMM_ERR_PROC_WAIT,                /* Wait for process OID failed to complete */
   E_PMM_ERR_SYSREQ_ORIG_MATCH,        /* Attempt to interrupt a non-originated system request */
   E_PMM_ERR_SYSREQ_CANCEL_MATCH,      /* Attempt to cancel a unmatched system request */
   E_PMM_ERR_SYSREQ_ALREADY_PROG,      /* A system request is already in progress */

   E_PMM_ERR_EOL                       /* End-of-list */

} ePmmStatus_t;

/* Status and errors generated by the CHB */

typedef enum eChbStatus_e
{

   E_CHB_ERR_GEN = STATUS_START( CHB ),/* Miscellaneous error */
   E_CHB_ERR_SETUP,                    /* Error performing setup */
   E_CHB_ERR_TIMER_SETUP,              /* Error setting up the timer */
   E_CHB_ERR_CIL_NAME,                 /* Error matching CIL name */
   E_CHB_ERR_CIL_RX,                   /* Unexpected CIL message received */
   E_CHB_ERR_SDB_SUBMIT,               /* Error performing SDB submission */
   E_CHB_ERR_REPLY_TIMEOUT,            /* Process sends reply after timeout */

   E_CHB_ERR_EOL                       /* End-of-list */

} eChbStatus_t;

/* Status and errors generated by the UIT */

typedef enum eUitStatus_e
{

   E_UIT_ERR_GEN = STATUS_START( UIT ),/* Miscellaneous error */
   E_UIT_ERR_SETUP,                    /* Error performing setup */
   E_UIT_ERR_HEARTBEATS,               /* Heartbeats lost, assumed safe-state */
   E_UIT_ERR_CIL_RX,                   /* Unexpected CIL message received */
   E_UIT_ERR_SDB_SUBMIT,               /* Error performing SDB submission */
   E_UIT_ERR_UPS_START,                /* Error starting communications with UPS */
   E_UIT_ERR_SDB_CLEAR,                /* Error clearing data from the SDB */

   E_UIT_ERR_EOL                       /* End-of-list */

} eUitStatus_t;

/* Status and errors generated by the AIT */

typedef enum eAitStatus_e
{

   E_AIT_ERR_GEN = STATUS_START( AIT ),/* Miscellaneous error */
   E_AIT_ERR_SETUP,                    /* Error performing setup */
   E_AIT_ERR_HEARTBEATS,               /* Heartbeats lost, assumed safe-state */
   E_AIT_ERR_CIL_RX,                   /* Unexpected CIL message received */
   E_AIT_ERR_SDB_SUBMIT,               /* Error performing SDB submission */
   E_AIT_ERR_OUTLET_STATUS,            /* Error obtaining outlet status */
   E_AIT_ERR_ABOUT_APS,                /* Error obtaining MasterSwitch details */
   E_AIT_ERR_INTERFACE_OFF,            /* Request when interface not running */
   E_AIT_ERR_SDB_CLEAR,                /* Error clearing data from the SDB */

   E_AIT_ERR_EOL                       /* End-of-list */

} eAitStatus_t;

/* Status and errors generated by the MIT */

typedef enum eMitStatus_e
{

   E_MIT_ERR_GEN = STATUS_START( MIT ),/* Miscellaneous error */
   E_MIT_ERR_SETUP,                    /* Error performing setup */
   E_MIT_ERR_HEARTBEATS,               /* Heartbeats lost, assumed safe-state */
   E_MIT_ERR_CIL_RX,                   /* Unexpected CIL message received */
   E_MIT_ERR_SDB_SUBMIT,               /* Error performing SDB submission */
   E_MIT_ERR_SDB_CLEAR,                /* Error clearing data from the SDB */

   E_MIT_ERR_EOL                       /* End-of-list */

} eMitStatus_t;


/*
** Variable definitions - note use of #ifdef to compile in static data
*/ 

#ifdef E_MCP_INCLUDE_NAMES

/* Strings for the enumeration eMcpAuthState_e */

static char eMcpAuthStateName[ E_MCP_AUTH_EOL ][ E_MCP_STRING_LEN ] = 
{
   "",
   "Monitor",
   "Request",
   "Control",
   "Refused",
   "Sys.Req."
};

/* Strings for the enumeration eMcpSysReqIndex_e */

static char eMcpSysReqIndexName[ E_MCP_SYSREQ_EOL ][ E_MCP_STRING_LEN ] = 
{
   "",
   "Safe-state",
   "Activate",
   "Full Shutdown",
   "Full Start-up",
   "Partial Shutdown",
   "Partial Start-up",
   "Operational On",
   "Operational Off", 
   "Immediate System Shutdown",
   "Full System Restart",
   "Fail Override",
   "Cancel Fail Override",
   "Operational Time Override",
   "Cancel Operational Time Override",
   "Bad Weather Override",
   "Cancel Bad Weather Override",
   "EPT Problem Override",
   "Cancel EPT Problem Override",
   "SPT Problem Override",
   "Cancel SPT Problem Override",
   "Node Problem Override",
   "Cancel Node Problem Override",
   "Cancel Operational On",
   "Cancel Operational Off",
   "Refresh Configuration Items",
   "Software-only Shutdown",
   "Force Operational On flag",
   "Force Operational Off flag",
   "Operational Pause",
   "Select Cassegrain Focus",
   "Select Nasmyth+ Focus",
   "Select Nasmyth- Focus",
   "Select Prime Focus",
   "User Selection 1", 
   "User Selection 2", 
   "User Selection 3", 
   "User Selection 4", 
   "User Selection 5" 
};

/* Strings for the enumeration eMcpSysReqOffset_e */

static char eMcpSysReqOffsetName[ E_MCP_SYSREQ_OFFSET ][ E_MCP_STRING_LEN ] = 
{
   "Request",
   "In Progress",
   "Completed",
   "Error"
};

/* Strings for the enumeration eMcpSysRequest_e - note only initial entries */

static char eMcpSysRequestName[ E_MCP_SYSREQ_OFFSET ][ E_MCP_STRING_LEN ] = 
{
   "None",
   "Unauthorised",
   "",
   ""
   /*
   ** Note that further entries should be obtained by concatenating the strings 
   ** from eMcpSysReqIndexName and eMcpSysReqOffsetName.
   */
};

#endif


/******************************************************************************/
/*                                                                            */
/* Heartbeat Transmission Message                                             */
/*                                                                            */
/* This CIL message is transmitted periodically by the CHB, and is described  */
/*  by the following parameters :-                                            */
/*                                                                            */
/* SourceId   E_CIL_CHB                                                       */
/* DestId     As appropriate                                                  */
/* Class      E_CIL_CMD_CLASS                                                 */
/* Service    E_MCP_HEARTBEAT                                                 */
/* DataLen    E_MCP_HEARTBEAT_CMD_LEN                                         */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Heartbeat Response Message                                                 */
/*                                                                            */
/* This CIL message is transmitted by an application in response to a         */
/*  Heartbeat Transmission Message, and is described by the following :-      */
/*                                                                            */
/* SourceId   As appropriate                                                  */
/* DestId     E_CIL_CHB                                                       */
/* Class      E_CIL_RSP_CLASS                                                 */
/* Service    E_MCP_HEARTBEAT                                                 */
/* DataLen    E_MCP_HEARTBEAT_RSP_LEN                                         */
/*                                                                            */
/* The message body contains one element of type ProcState_t, this containing */
/* the state of the process originating the heartbeat response message.       */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Shutdown Message                                                           */
/*                                                                            */
/* This CIL message is transmitted by the PMM to request shutdown from a      */
/*  software process :-                                                       */
/*                                                                            */
/* SourceId   E_CIL_MCP                                                       */
/* DestId     As appropriate                                                  */
/* Class      E_CIL_CMD_CLASS                                                 */
/* Service    E_MCP_SHUTDOWN                                                  */
/* DataLen    E_MCP_SHUTDOWN_CMD_LEN                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Shutdown Acknowledgement Message                                           */
/*                                                                            */
/* This CIL message is transmitted by an application as an acknowledgement to */
/*  a Shutdown Message, and is described by the following :-                  */
/*                                                                            */
/* SourceId   As appropriate                                                  */
/* DestId     E_CIL_MCP                                                       */
/* Class      E_CIL_ACK_CLASS                                                 */
/* Service    E_MCP_SHUTDOWN                                                  */
/* DataLen    E_MCP_SHUTDOWN_ACK_LEN                                          */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Safe-State Message                                                         */
/*                                                                            */
/* This CIL message is transmitted by the PMM to request safe-state from a    */
/*  software process :-                                                       */
/*                                                                            */
/* SourceId   E_CIL_MCP                                                       */
/* DestId     As appropriate                                                  */
/* Class      E_CIL_CMD_CLASS                                                 */
/* Service    E_MCP_SAFESTATE                                                 */
/* DataLen    E_MCP_SAFESTATE_CMD_LEN                                         */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Safe-State Command In Progress Message                                     */
/*                                                                            */
/* This CIL message is transmitted by an application as an acknowledgement to */
/*  a Safe-State Message to indicate that the action is in progress, and is   */
/*  described by the following :-                                             */
/*                                                                            */
/* SourceId   As appropriate                                                  */
/* DestId     E_CIL_MCP                                                       */
/* Class      E_CIL_ACT_CLASS                                                 */
/* Service    E_MCP_SAFESTATE                                                 */
/* DataLen    E_MCP_SAFESTATE_ACT_LEN                                         */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Safe-State Completed Message                                               */
/*                                                                            */
/* This CIL message is transmitted by an application when requested           */
/*  Safe-State has been attained, and is described by the following :-        */
/*                                                                            */
/* SourceId   As appropriate                                                  */
/* DestId     E_CIL_MCP                                                       */
/* Class      E_CIL_COM_CLASS                                                 */
/* Service    E_MCP_SAFESTATE                                                 */
/* DataLen    E_MCP_SAFESTATE_COM_LEN                                         */
/*                                                                            */
/* The message body contains one element of type ProcState_t, this containing */
/* the attained state (safe) of the process originating the message.          */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Activate Message                                                           */
/*                                                                            */
/* This CIL message is transmitted by the PMM to request activation of a      */
/*  software process from safe-state :-                                       */
/*                                                                            */
/* SourceId   E_CIL_MCP                                                       */
/* DestId     As appropriate                                                  */
/* Class      E_CIL_CMD_CLASS                                                 */
/* Service    E_MCP_ACTIVATE                                                  */
/* DataLen    E_MCP_ACTIVATE_CMD_LEN                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Activate Command In Progress Message                                       */
/*                                                                            */
/* This CIL message is transmitted by an application as an acknowledgement to */
/*  an Activate Message to indicate that the action is in progress, and is    */
/*  described by the following :-                                             */
/*                                                                            */
/* SourceId   As appropriate                                                  */
/* DestId     E_CIL_MCP                                                       */
/* Class      E_CIL_ACT_CLASS                                                 */
/* Service    E_MCP_ACTIVATE                                                  */
/* DataLen    E_MCP_ACTIVATE_ACT_LEN                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Activate Completed Message                                                 */
/*                                                                            */
/* This CIL message is transmitted by an application when requested           */
/*  Activate has been attained, and is described by the following :-          */
/*                                                                            */
/* SourceId   As appropriate                                                  */
/* DestId     E_CIL_MCP                                                       */
/* Class      E_CIL_COM_CLASS                                                 */
/* Service    E_MCP_ACTIVATE                                                  */
/* DataLen    E_MCP_ACTIVATE_COM_LEN                                          */
/*                                                                            */
/* The message body contains one element of type ProcState_t, this containing */
/* the restored state of the process originating the message.                 */
/*                                                                            */
/******************************************************************************/


#endif

/*
** EOF
*/
