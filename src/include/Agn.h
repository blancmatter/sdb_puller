/*
** Header File Name:
**    Agn.h
**
** Purpose:
**    Public Header file for AcnAgent.c
**
** Description:
**    This header file contains the AcnAgent defines & function prototypes.
**
**    This code has been written to conform to "TTL Coding Standard" v0.6.
**
** Authors:
**    mkv : Mark Vernon (TTL)
**
** Copyright (C) Telescope Technologies Limited, 2000
**
** Version:
**    $Id: Agn.h,v 0.21 2001/09/20 10:56:08 mjf Exp $
**
** History:
**    $Log: Agn.h,v $
**    Revision 0.21  2001/09/20 10:56:08  mjf
**    Partially re-written to produce one executable 'Agn', with the agent
**    type determined by the command-line switch.
**
**    Revision 0.20  2001/09/19 13:37:10  mjf
**    Updates as a result of source file name changes. No functional changes.
**
**    Revision 0.19  2001/05/18 13:05:35  mjf
**    Submission of agent version into the SDB.
**
**    Revision 0.18  2001/03/22 09:40:08  mjf
**    Changing of line endings to LF-only.
**
**    Revision 0.17  2001/03/20 14:42:42  mjf
**    Node now specified by a command-line switch rather than tied to the
**    QNX node ID, for portability.
**
**    Revision 0.16  2001/02/05 11:55:44  djm
**    Changed server path.
**
**    Revision 0.15  2000/12/06 11:39:57  rdl
**    Minor corrections to remove unrequired code and addition of comments.
**
**    Revision 0.14  2000/12/03 10:26:48  rdl
**    Replacement of non specific error codes (SYS_WARNING etc)
**    with package specific error codes (E_AGN_XXX).
**
**    Revision 0.13  2000/12/03 09:05:37  rdl
**    Replacement of "Acn/ACN" with "Agn/AGN" where appropriate in order
**    to differentiate between Agent and Node Functions/Errors.
**
**    Revision 0.12  2000/12/01 17:29:25  rdl
**    Modifications to allow correct running on multiple nodes.
**
**    Revision 0.11  2000/11/20 12:23:55  rdl
**    Consolidation of header files following partitioning of AcnAgent.c
**
**    Revision 0.10  2000/11/17 12:13:57  rdl
**    Modifications following partitioning of AcnAgent.c.
**
**    Revision 0.9  2000/11/01 13:16:07  rdl
**    Minor corrections.
**
**    Revision 0.8  2000/10/19 09:03:22  mkv
**    Bug Fixes.
**
**    Revision 0.7  2000/10/16 11:25:51  mkv
**    Placed status error codes in AgentServer.h.
**
**    Revision 0.6  2000/10/13 14:54:26  mkv
**    Included Clu Header.
**
**    Revision 0.5  2000/10/13 09:03:29  mkv
**    Made Error Status Public.
**
**    Revision 0.4  2000/10/12 08:40:20  mkv
**    Put public error codes in place.
**
**    Revision 0.3  2000/10/09 13:34:50  rdl
**    Consolidation of definitions.
**
**    Revision 0.2  2000/10/06 15:45:34  mkv
**    Updated wrt coding standard.
**
**    Revision 0.1  2000/10/05 12:19:16  mkv
**    Initial Revision.
**
**
*/

#ifndef _ACN_AGENT_H_DEFINED
#define _ACN_AGENT_H_DEFINED           1

/* #include "Clu.h"                    */   /* Include the command line utilities. */

/*
** MKV these names may have to be concatinated strings with a spawn number
** Define this server's name-server name and the name of its executeable.
*/

#ifdef M_AGN_AGENT_MAIN
   #define  E_AGN_EXTERN
   #define  E_AGN_INIT (Initialisation) = Initialisation
#else
   #define  E_AGN_EXTERN extern
   #define  E_AGN_INIT (Initialisation)
#endif

/* Enumerate list of status / error values */

enum eAgnStatus_e
{
   E_AGN_GEN_ERR = STATUS_START(AGN),  /* Miscellaneous error                 */
   E_AGN_WRONG_EXE_NAME,               /* Executable filename not as expected */
                             /* Expected custom argument not supplied         */
   E_AGN_EXPECTED_CUSTOM_ARG_MISSING,
   E_AGN_TOO_MANY_CUSTOM_ARGS,         /* Too many custom args supplied       */
                             /* Wrong Custom argument for specified agent     */
   E_AGN_WRONG_CUSTOM_ARG_FOR_AGT_TYPE_1,
   E_AGN_WRONG_CUSTOM_ARG_FOR_AGT_TYPE_2,
   E_AGN_WRONG_CUSTOM_ARG_FOR_AGT_TYPE_3,
   E_AGN_WRONG_CUSTOM_ARG_FOR_AGT_TYPE_4,
   E_AGN_WRONG_CUSTOM_ARG_FOR_AGT_TYPE_5,
   E_AGN_WRONG_CUSTOM_ARG_FOR_AGT_TYPE_6,
   E_AGN_WRONG_CUSTOM_ARG_FOR_AGT_TYPE_7,

                                       /* Null pointer errors                 */
   E_AGN_INIT_AGENT_MSG_NULL_POINTER,  /* Function : iAgnInitialisation       */
   E_AGN_UNABLE_TO_INIT_LOG_FILE,
   E_AGN_UNABLE_TO_LOCATE_OWN_PROCESS,
   E_AGN_UNABLE_TO_LOCATE_SERVER,
   E_AGN_WORK_AGENT_MSG_NULL_POINTER,  /* Function : iAgnBuildReportForWorkMsg*/
                             /* Function : iAgnReqEndClientAndWaitResp        */
   E_AGN_ENDCLIENT_AGENT_MSG_NULL_POINTER,
   E_AGN_SERVER_NOT_RUNNING,           /* Function : iAgnRespToServAndWaitReq */
                                       /* Function : iAgnValidateServReqMsg   */
   E_AGN_SERVREQ_AGENT_MSG_NULL_POINTER, 
   E_AGN_ALIVE_AGENT_MSG_NULL_POINTER, /* Function : iAgnProcessAlive         */
	
                                       /* Specific Application errors         */
   E_AGN_ERROR_AT_PREVIOUS_LEVEL,      /* Error already identified            */
   E_AGN_INVALID_SERVER_REQUEST,       /* Request from server invalid         */
   E_AGN_SEND_TO_END_CLIENT_FAILED,    /* Unable to send to endclient process */
   E_AGN_INVALID_END_CLIENT_RESPONSE,  /* Response from endclient invalid     */
	E_AGN_UNEXPECTED_MSG_TYPE,          /*                                     */
	E_AGN_INVALID_AGENT_TYPE,           /* Unrecognised Agent Type             */
	E_AGN_INVALID_AGENT_TYPE_1,         /* Invalid agent type for this agent   */
	E_AGN_INVALID_AGENT_TYPE_2,         /* Invalid agent type for this agent   */
	E_AGN_INVALID_AGENT_TYPE_3,         /* Invalid agent type for this agent   */
	E_AGN_SEQUENCE_NO_ERROR,            /* General sequence number error       */
   E_AGN_NO_END_CLIENT_NAME,           /* Empty Endclient name                */
	E_AGN_INVALID_END_CLIENT_NAME,      /* End client name is invalid          */
	E_AGN_UNABLE_TO_LOCATE_END_CLIENT,  /* End client not found                */
	E_AGN_INVALID_MOTION_COMMAND,       /* Invalid motion command received     */
	
   E_AGN_SRC_UNEXPECTED,               /* Unexpected Cil Source Id            */
	
   E_AGN_NODE_UNRESOLVED,              /* Unable to resolve node on start-up  */

   E_AGN_EOSTATUS_LIST,      /* End of list marker (DO NOT USE FOR STATUS)    */
   E_AGN_STATUS_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes      */
} ;

/* Enumerate the OIDs owned by this package - copy of the Heartbeat Agent */

typedef enum eAgnDataId_e 
{

   D_AGN_DATAID_BOL = OID_START (AGN), /* Offset the OIDs by the package ID. */
   D_AGN_PROC_STATE,                   /* The node comms in process state.   */
   D_AGN_VERSION,                      /* Version number                     */
   D_AGN_SERVER_SHUTDOWN,              /* Server shutdown command flag.      */
   D_AGN_SERVER_PID,                   /* Server process id                  */

   D_AGN_DATAID_EOL,                   /* End of list marker. */
   D_AGN_DATAID_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes     */

} eAgnDataId_t ;


/*
#define E_AGN_REG_SERVER_NAME   "E_TTL_AZR_AGN_Agent"
*/
#define E_AGN_SERVER_PROC       "Agn"
#define E_AGN_SERVER_PATH       "//1/opt/ttl/bin"
#define E_AGN_SERVER_PRI        10
                                       /* MKV Remember to change this to 'bin'*/

#define E_AGN_SEND_OK                  0
#define E_AGN_SEND_ERROR              -1



#endif                                 /* _ACN_AGENT_H_DEFINED                */

/*
** =============================================================================
** End of Function : AcnAgent.h
** =============================================================================
*/
