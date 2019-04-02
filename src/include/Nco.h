/*
** Header File Name:
**    Nco.h
**
** Purpose:
**    Public header file for Axis Control Node (Comms Response)
**    functions.
**
** Description:
**    This code has been written to conform to the "TTL Coding 
**    Standard"  v0.4 29-Sep-1998.
**
** Authors:
**    rdl:     R. D. Lett (TTL).
**
** Version:
**    $Id: Nco.h,v 0.16 2004/04/01 15:55:50 sxf Exp $
**
** History:
**    $Log: Nco.h,v $
**    Revision 0.16  2004/04/01 15:55:50  sxf
**    Renamed NodeCmdList_t and AcnCommand_t to avoid clashes with Nci.h
**
**    Revision 0.15  2004/04/01 15:53:37  sxf
**    Renamed eAcnAuth_t to eNcoAuth_t
**
**    Revision 0.14  2001/09/21 07:44:52  mjf
**    Submit the process state into the SDB.
**
**    Revision 0.13  2001/09/19 14:53:35  mjf
**    Updates as a result of source file name changes. No functional changes.
**
**    Revision 0.12  2001/07/24 09:40:39  rdl
**    ÿ¡Renamed enumerations within NodeCmdList_t to prevent compiler errors
**    when both ACNComms.h & ACNResps.h are both included.
**    Modified named tag on status/error enumeration for same reason.
**
**    Revision 0.11  2001/03/20 12:13:50  mjf
**    Node now determined from the command-line rather than the node number
**    determined from the node ID. Allows portability.
**
**    Revision 0.10  2001/02/18 15:13:37  rdl
**    Equated local ACN Commands for Get and Set to hash
**    defines for MCP based generic Get and Set command hash
**    defines.
**
**    Revision 0.9  2001/02/05 15:17:55  rdl
**    Added enumerations for errors associated with handling of
**    SHUTDOWN message.
**
**    Revision 0.8  2001/02/02 13:30:26  rdl
**    Added enumerations to define local ACN Activate and Safe State
**    commands.
**    Added enumerations to define errors during creation of Activate
**    Safe State reply messages.
**    Added function definitions for Generate Activate and Generate
**    Safe State reply messages - Acknowledge and Completed format.
**
**    Revision 0.7  2000/12/17 12:01:53  rdl
**    Modification to accomodate Package OID and submission of data
**    to SDB via newly created SDB Submit Data function.
**
**    Revision 0.6  2000/12/06 14:12:08  rdl
**    General commenting and corrections.
**
**    Revision 0.5  2000/11/15 16:04:07  rdl
**    Amendments to allow execution on any Axis Control Node.
**
**    Revision 0.4  2000/11/10 16:52:48  rdl
**    Modified Cil Address and references for execution on
**    node six (6) as Cassegrain Node.
**
**    Revision 0.3  2000/11/01 13:04:37  rdl
**    Change to constant used to force enumerated list types to occupy four (4) bytes.
**
**    Revision 0.2  2000/10/31 16:10:35  rdl
**    Correction following implementation of additional compiler options (-wx, -we).
**
**    Revision 0.1  2000/09/20 14:07:09  rdl
**    Initial Version.
**
**
**
*/

#ifndef NCO_H_DEFINED
#define NCO_H_DEFINED             1

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_ACN_RESPS_MAIN
   #define E_NCO_EXTERN
   #define E_NCO_INIT(Initialisation) = Initialisation
#else
   #define E_NCO_EXTERN extern
   #define E_NCO_INIT(Initaliasation)
#endif

/* Required include files */

#include <limits.h>
#include "TtlSystem.h"      /* For Status_t definition                        */
#include "Mcp.h"
#include "Asi.h"

/* Enumerate list of status/error values */

enum eNCOStatus_e
{
   E_NCO_GEN_ERR = STATUS_START (NCO),      /* Miscellaneous error            */
   E_NCO_SRC_UNKNOWN,        /* Unlisted source CIL ID                        */
   E_NCO_DST_UNKNOWN,        /* Unknown destination CIL ID                    */
   E_NCO_CLA_UNKNOWN,        /* Unrecognised command line arg                 */
   E_NCO_CLASS_ERR,          /* Message class not accepted by NCO             */
   E_NCO_SRCEQDST_ERR,       /* SrcId == DstId for ERR class message          */

                             /* Message generation Null Pointer Errors        */ 
   E_NCO_GEN_HBEAT_REPLY_NULL_PTR,          /* HeartBeat Message              */
   E_NCO_GEN_DIA_REPLY_NULL_PTR,		/* Diagnose Message               */
   E_NCO_GEN_HALT_REPLY_NULL_PTR,		/* Halt Message                   */
   E_NCO_GEN_HOME_REPLY_NULL_PTR,		/* Home Message                   */
   E_NCO_GEN_START_REPLY_NULL_PTR,		/* Start Message                  */
   E_NCO_GEN_STOP_REPLY_NULL_PTR,		/* Stop Message                   */
   E_NCO_GEN_MOVE_REPLY_NULL_PTR,		/* Move Message                   */
   E_NCO_GEN_TRACK_REPLY_NULL_PTR,		/* Track Message                  */
   E_NCO_GEN_GET_REPLY_NULL_PTR,		/* Get Message                    */
   E_NCO_GEN_SET_REPLY_NULL_PTR,		/* Set Message                    */
   E_NCO_GEN_GETA_REPLY_NULL_PTR,		/* Get Attr Message               */
   E_NCO_GEN_SETA_REPLY_NULL_PTR,		/* Set Attr Message               */
   E_NCO_GEN_UNHOME_REPLY_NULL_PTR,		/* UnHome Message                 */
   E_NCO_GEN_WARMBOOT_REPLY_NULL_PTR,		/* WarmBoot Message               */
   E_NCO_GEN_MOVEUNHOMED_REPLY_NULL_PTR,	/* Move Unhomed Message           */
   E_NCO_GEN_RESETHEAD_REPLY_NULL_PTR,		/* Reset Head Message             */
   E_NCO_GEN_SELMALGOL_REPLY_NULL_PTR,		/* Select Motion Algorithm Message*/
   E_NCO_GEN_RECOVER_REPLY_NULL_PTR,	     /* Recover Message                */
	E_NCO_GEN_ACTIVATE_ACK_REPLY_NULL_PTR,   /* Acknowledge Activate Message	*/
	E_NCO_GEN_ACTIVATE_COM_REPLY_NULL_PTR,   /* Complete Activate Message	*/
   E_NCO_GEN_SAFESTATE_ACK_REPLY_NULL_PTR,  /* Acknowledge Safe Satte Message */ 
   E_NCO_GEN_SAFESTATE_COM_REPLY_NULL_PTR,  /* Complete Safe State Message    */ 
	E_NCO_GEN_SHUTDOWN_ACK_REPLY_NULL_PTR,   /* Acknowledge Shut Down Message	*/
	
   E_NCO_ACK_ACTIVATE_NO_REPLY,
	E_NCO_COM_ACTIVATE_NO_REPLY,
	E_NCO_ACK_SAFESTATE_NO_REPLY,
	E_NCO_COM_SAFESTATE_NO_REPLY,
   E_NCO_ACK_SHUTDOWN_NO_REPLY,
	
	                                         /* Specific Application Errors    */
   E_NCO_INVALID_SERVICE_REQUEST,           /* Invalid service command        */

   E_NCO_NODE_UNRESOLVED,              /* Unable to resolve node on start-up  */

   E_NCO_EOSTATUS_LIST,      /* End of list marker (DO NOT USE FOR STATUS)    */
	E_NCO_STATUS_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes      */
} ;

typedef Int32_t eNCOStatus_t ;	/* eAcnStatus_t		*/

/* Enumerate the OID's owned by this package. */
typedef enum eNcoDataId_e 
{
    D_NCO_DATAID_BOL = OID_START (NCO), /* Offset the OID's by the package ID. */   /* 0000 */
    D_NCO_PROC_STATE,                   /* The node comms out process state.  */    /* 0001 */
    D_NCO_VERSION,                      /* Version number                     */    /* 0002 */
    D_NCO_SERVER_SHUTDOWN,              /* Server shutdown command flag.      */    /* 0003 */

    D_NCO_DATAID_EOL,                   /* End of list marker. */
    D_NCO_DATAID_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes     */
}eNcoDataId_t ;


/* Clearance level for forwarding messages */

typedef Int32_t eNcoAuth_t ;


/* ACN command set (services offered to other programs) */

typedef enum
{
   E_NCO_HEARTBEAT = E_MCP_HEARTBEAT,  /* Heartbeat command                   */
   E_NCO_SHUTDOWN  = E_MCP_SHUTDOWN,   /* Shutdown command                    */
   E_NCO_SAFESTATE = E_MCP_SAFESTATE,  /* Safestate command                   */
   E_NCO_ACTIVATE  = E_MCP_ACTIVATE,   /* Activate command                    */

	                               /* Cause the ACN Comms Out to terminate     */
   E_NCO_SET_AUTH = SERVICE_START (NCO),

	E_NCO_COMMAND_EOL,                  /* End of list marker - DO NOT USE     */
	E_NCO_COMMANDS_MAX_VALUE = INT_MAX  /* Req'd to force size to 4 bytes      */
} eNcoCommands_t ;

typedef enum 
{
    E_NCO_MIN_CMD                    =  0,
    
    E_NCO_DIAGNOSE_CMD               =  1,
    E_NCO_GET_CMD                    =  E_MCP_GET_1,       /* formerly 2      */
    E_NCO_SET_CMD                    =  E_MCP_SET_1,       /* formerly 3      */
    E_NCO_STOP_CMD                   =  4,
    E_NCO_START_CMD                  =  5,
    E_NCO_HALT_CMD                   =  6,
    E_NCO_MOVEUNHOMED_CMD            =  7,
    E_NCO_TRACK_CMD                  =  8,
    E_NCO_MOVE_CMD                   =  9,
    E_NCO_UNHOME_CMD                 = 10,
    E_NCO_SINUSOID_CMD               = 11,
    E_NCO_GETSINUSOID_CMD            = 12,
    E_NCO_GETLOG_CMD                 = 13,
    E_NCO_GETLOGLIST_CMD             = 14,
    E_NCO_SETLOGLIST_CMD             = 15,
    E_NCO_SETLOGDEPTH_CMD            = 16,
    E_NCO_CLEARLOG_CMD               = 17,
    E_NCO_GETFASTLOG_CMD             = 18,
    E_NCO_GETNEXTFASTLOG_CMD         = 19,
    E_NCO_GETFASTLOGLIST_CMD         = 20,
    E_NCO_SETFASTLOGLIST_CMD         = 21,
    E_NCO_SETFASTLOGDEPTH_CMD        = 22,
    E_NCO_SETFASTLOGFREQUENCY_CMD    = 23,
    E_NCO_STARTFASTLOG_CMD           = 24,
    E_NCO_STOPFASTLOG_CMD            = 25,
    E_NCO_SETNOFASTLOGS_CMD          = 26,
    E_NCO_HOME_CMD                   = 27,
    E_NCO_SELECTMOTIONALGORITHM_CMD  = 28,
    E_NCO_CONTACTORRESET_CMD         = 29,
    E_NCO_RESETHEAD_CMD              = 30, 
    E_NCO_SIMULATE_CMD               = 31,
    E_NCO_WARMBOOT_CMD               = 32,
    E_NCO_RECOVER_CMD                = 33,
    E_NCO_KEEPUNHOMED_CMD            = 34,
    E_NCO_SHUTDOWN_CMD               = 35,
    E_NCO_GET_ATTR_CMD               = 36,
    E_NCO_SET_ATTR_CMD               = 37,
    E_NCO_RESET_NODE_CMD             = 38,
 
	 /* 
	 ** The following two commands are not issued from the 
	 ** Node Communications In Process but are used within the Node 
	 ** Server process to generate the second part of an Activate or
	 ** Safe State message that is passed to the Poll Agent.
	 */
	 
	 E_NCO_ACTIVATE_CMD               = 39,
	 E_NCO_SAFESTATE_CMD              = 40,

    /* Insert additional commands here */

    E_NCO_HEARTBEAT_CMD              = 255,

    /* This must be the last entry in this enumeration */
    E_NCO_MAX_CMD,
    E_NCO_CMD_LIST_MAX_VALUE = INT_MAX      /* Req'd to force size to 4 bytes */
} NcoNodeCmdList_t ;


#endif

/*
** =============================================================================
** End of File : ACNResps.h
** =============================================================================
*/
