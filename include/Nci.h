/*
** Header File Name:
**    Nci.h
**
** Purpose:
**    Public header file for Axis Control Node (Comms In)
**    functions.
**
** Description:
**    This code has been written to conform to the "TTL Coding 
**    Standard"  v0.4 29-Sep-1998.
**
** Authors:
**    rdl:     R. D. Lett (TTL).
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000.
**
** Version:
**    $Id: Nci.h,v 0.19 2004/06/07 13:55:05 mkb Exp $
**
** History:
**    $Log: Nci.h,v $
**    Revision 0.19  2004/06/07 13:55:05  mkb
**    No functional change.
**
**    Revision 0.18  2002/01/21 14:54:41  mjf
**    Check the deliverer of incoming messages to be either MCP or MCB.
**    Also report any problems with incoming messages at 'error' level.
**
**    Revision 0.17  2001/09/21 08:19:38  mjf
**    Submission of process state into the SDB. Also, submit tallies of
**    incoming commands received into the SDB.
**
**    Revision 0.16  2001/09/19 14:44:22  mjf
**    Updates as a result of source file name changes. No functional changes.
**
**    Revision 0.15  2001/03/20 11:12:29  mjf
**    Node now determined from the command-line rather than the node number
**    determined from the node ID. Allows portability.
**
**    Revision 0.14  2001/02/18 14:52:50  rdl
**    Equated local ACN Commands for Get and Set to hash
**    defines for MCP based generic Get and Set command hash
**    defines.
**
**    Revision 0.13  2001/02/05 14:36:19  rdl
**    Added enumeration for error code associated with AcnGenerateShutDownMsg.
**
**    Revision 0.12  2001/02/02 12:27:34  rdl
**    Addition of definitons for local ACN Activate and Safe State
**    Commands.
**    Addition of enumerations defining errors during creation of
**    Activate and Safe State messages.
**
**    Revision 0.11  2000/12/17 11:48:04  rdl
**    Modification to accomodate Package OID and submission of data
**    to SDB via newly created SDB Submit Data function.
**
**    Revision 0.10  2000/12/07 11:10:43  rdl
**    Removal of code enclosed in #define statements.
**
**    Revision 0.9  2000/12/06 14:02:26  rdl
**    General corrections and commenting.
**
**    Revision 0.8  2000/11/24 14:17:23  rdl
**    Modified to comply with TTL Coding Standard in order to
**    allow VMS to include ACNComms.h file.
**
**    Revision 0.7  2000/11/15 15:58:53  rdl
**    Amendments to allow execution on any Axis Control Node.
**
**    Revision 0.6  2000/11/08 18:16:21  rdl
**    Call added to perform network byte ordering swap.
**    Modifications to comply with TTL Coding Standard.
**    Reorganisation of #defines and enumerated lists.
**    Additional error handling with generate message functions.
**
**    Revision 0.5  2000/11/02 15:44:20  rdl
**    Mods to produce first working version communicating with ECI.
**
**    Revision 0.4  2000/11/01 13:04:37  rdl
**    Change to constant used to force enumerated list types to occupy four (4) bytes.
**
**    Revision 0.3  2000/10/31 16:10:35  rdl
**    Correction following implementation of additional compiler options (-wx, -we).
**
**    Revision 0.2  2000/10/25 16:51:05  rdl
**    Debugging Node Server and assocuiated communication functions.
**
**    Revision 0.1  2000/09/20 13:44:08  rdl
**    Initial Version.
**
**
**
*/

#ifndef NCI_H_DEFINED
#define NCI_H_DEFINED             1

/* Required include files */

#include <limits.h>
#include "TtlSystem.h"       /* For Status_t definition                       */
#include "Mcp.h"
#include "Cil.h"             /* eCilMsg_t */

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_ACN_COMMS_MAIN
   #define E_NCI_EXTERN
   #define E_NCI_INIT(Initialisation) = Initialisation
#else
   #define E_NCI_EXTERN extern
   #define E_NCI_INIT(Initaliasation)
#endif

/* Enumerate list of status/error values */

enum eAcnStatus_e
{
   E_NCI_GEN_ERR = STATUS_START (NCI), /* Miscellaneous error                 */
   E_NCI_SRC_UNKNOWN,        /* Unlisted source CIL ID                        */
   E_NCI_DST_UNKNOWN,        /* Unknown destination CIL ID                    */
   E_NCI_CLA_UNKNOWN,        /* Unrecognised command line arg                 */
   E_NCI_CLASS_ERR,          /* Message class not accepted by NCI             */
   E_NCI_SRCEQDST_ERR,       /* SrcId == DstId for ERR class message          */

   E_NCI_CIL_DST_WRONG,      /* Message not for this Cil Address              */
   E_NCI_CIL_CLASS_WRONG,    /* Cil Class not for this application process    */
   E_NCI_CIL_SERVICE_WRONG,  /* Cil Service not for this application process  */
                             /* Invalid command from MCS to ACN               */
   E_NCI_INVALID_MCS_ACN_COMMAND,

                             /* Message generation Null Pointer Errors        */ 
   E_NCI_GEN_HBEAT_MSG_NULL_PTR,       /* HeartBeat Message                   */
   E_NCI_GEN_DIA_MSG_NULL_PTR,         /* Diagnose Message                    */
   E_NCI_GEN_HALT_MSG_NULL_PTR,        /* Halt Message                        */
   E_NCI_GEN_HOME_MSG_NULL_PTR,        /* Home Message                        */
   E_NCI_GEN_START_MSG_NULL_PTR,       /* Start Message                       */
   E_NCI_GEN_STOP_MSG_NULL_PTR,        /* Stop Message                        */
   E_NCI_GEN_MOVE_MSG_NULL_PTR,        /* Move Message                        */
   E_NCI_GEN_TRACK_MSG_NULL_PTR,       /* Track Message                       */
   E_NCI_GEN_GET_MSG_NULL_PTR,         /* Get Message                         */
   E_NCI_GEN_SET_MSG_NULL_PTR,         /* Set Message                         */
   E_NCI_GEN_GETA_MSG_NULL_PTR,        /* Get Attr Message                    */
   E_NCI_GEN_SETA_MSG_NULL_PTR,        /* Set Attr Message                    */
   E_NCI_GEN_UNHOME_MSG_NULL_PTR,      /* UnHome Message                      */
   E_NCI_GEN_WARMBOOT_MSG_NULL_PTR,    /* WarmBoot Message                    */
   E_NCI_GEN_MOVEUNHOMED_MSG_NULL_PTR, /* Move Unhomed Message                */
   E_NCI_GEN_RESETHEAD_MSG_NULL_PTR,   /* Reset Head Message                  */
   E_NCI_GEN_SELMALGOL_MSG_NULL_PTR,   /* Select Motion Algorithm Message     */
   E_NCI_GEN_RECOVER_MSG_NULL_PTR,     /* Recover Message                     */
	E_NCI_GEN_SAFESTATE_MSG_NULL_PTR,   /* Safe State Message                  */
	E_NCI_GEN_ACTIVATE_MSG_NULL_PTR,    /* Activate Message                    */
	E_NCI_GEN_SHUTDOWN_MSG_NULL_PTR,    /* Shutdown Message                    */

                        /* Message generation Incorrect Message Size Errors   */ 
   E_NCI_GEN_HBEAT_MSG_SIZE_ERR,       /* HeartBeat Message                   */
   E_NCI_GEN_DIA_MSG_SIZE_ERR,         /* Diagnose Message                    */
   E_NCI_GEN_HALT_MSG_SIZE_ERR,        /* Halt Message                        */
   E_NCI_GEN_HOME_MSG_SIZE_ERR,        /* Home Message                        */
   E_NCI_GEN_START_MSG_SIZE_ERR,       /* Start Message                       */
   E_NCI_GEN_STOP_MSG_SIZE_ERR,        /* Stop Message                        */
   E_NCI_GEN_MOVE_MSG_SIZE_ERR,        /* Move Message                        */
   E_NCI_GEN_TRACK_MSG_SIZE_ERR,       /* Track Message                       */
   E_NCI_GEN_GET_MSG_SIZE_ERR,         /* Get Message                         */
   E_NCI_GEN_SET_MSG_SIZE_ERR,         /* Set Message                         */
   E_NCI_GEN_GETA_MSG_SIZE_ERR,        /* Get Attr Message                    */
   E_NCI_GEN_SETA_MSG_SIZE_ERR,        /* Set Attr Message                    */
   E_NCI_GEN_UNHOME_MSG_SIZE_ERR,      /* UnHome Message                      */
   E_NCI_GEN_WARMBOOT_MSG_SIZE_ERR,    /* WarmBoot Message                    */
   E_NCI_GEN_MOVEUNHOMED_MSG_SIZE_ERR, /* Move Unhomed Message                */
   E_NCI_GEN_RESETHEAD_MSG_SIZE_ERR,   /* Reset Head Message                  */
   E_NCI_GEN_SELMALGOL_MSG_SIZE_ERR,   /* Select Motion Algorithm Message     */
   E_NCI_GEN_RECOVER_MSG_SIZE_ERR,     /* Recover Message                     */
	
	                                    /* Specific Application Errors         */
	E_NCI_INVALID_SERVICE_REQUEST,
   
   E_NCI_NODE_UNRESOLVED,              /* Unable to resolve node on start-up  */
   E_NCI_DELIVERER_ERR,                /* Message has incorrect deliverer     */

   E_NCI_EOSTATUS_LIST,      /* End of list marker (DO NOT USE FOR STATUS)    */
   E_NCI_STATUS_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes      */
} ;

typedef Int32_t eAcnStatus_t ;

typedef enum 
{
    E_ACN_MIN_CMD                    =  0,

    E_ACN_DIAGNOSE_CMD               =  1,
    E_ACN_GET_CMD                    =  E_MCP_GET_1,       /* formerly 2      */
    E_ACN_SET_CMD                    =  E_MCP_SET_1,       /* formerly 3      */
    E_ACN_STOP_CMD                   =  4,
    E_ACN_START_CMD                  =  5,
    E_ACN_HALT_CMD                   =  6,
    E_ACN_MOVEUNHOMED_CMD            =  7,
    E_ACN_TRACK_CMD                  =  8,
    E_ACN_MOVE_CMD                   =  9,
    E_ACN_UNHOME_CMD                 = 10,
    E_ACN_SINUSOID_CMD               = 11,
    E_ACN_GETSINUSOID_CMD            = 12,
    E_ACN_GETLOG_CMD                 = 13,
    E_ACN_GETLOGLIST_CMD             = 14,
    E_ACN_SETLOGLIST_CMD             = 15,
    E_ACN_SETLOGDEPTH_CMD            = 16,
    E_ACN_CLEARLOG_CMD               = 17,
    E_ACN_GETFASTLOG_CMD             = 18,
    E_ACN_GETNEXTFASTLOG_CMD         = 19,
    E_ACN_GETFASTLOGLIST_CMD         = 20,
    E_ACN_SETFASTLOGLIST_CMD         = 21,
    E_ACN_SETFASTLOGDEPTH_CMD        = 22,
    E_ACN_SETFASTLOGFREQUENCY_CMD    = 23,
    E_ACN_STARTFASTLOG_CMD           = 24,
    E_ACN_STOPFASTLOG_CMD            = 25,
    E_ACN_SETNOFASTLOGS_CMD          = 26,
    E_ACN_HOME_CMD                   = 27,
    E_ACN_SELECTMOTIONALGORITHM_CMD  = 28,
    E_ACN_CONTACTORRESET_CMD         = 29,
    E_ACN_RESETHEAD_CMD              = 30, 
    E_ACN_SIMULATE_CMD               = 31,
    E_ACN_WARMBOOT_CMD               = 32,
    E_ACN_RECOVER_CMD                = 33,
    E_ACN_KEEPUNHOMED_CMD            = 34,
    E_ACN_SHUTDOWN_CMD               = 35,
    E_ACN_GET_ATTR_CMD               = 36,
    E_ACN_SET_ATTR_CMD               = 37,
    E_ACN_RESET_NODE_CMD             = 38,

    /* Insert additional commands here */
  
    /* 
    ** The following two commands are not issued from the 
    ** Node Communications In Process but are used within the Node 
    ** Server process to generate the second part of an Activate or
    ** Safe State message that is passed to the Poll Agent.
    */
 
    E_ACN_ACTIVATE_CMD               = 39,
    E_ACN_SAFESTATE_CMD              = 40,

    E_ACN_HEARTBEAT_CMD              = 255,

    /* This must be the last entry in this enumeration */
    E_ACN_MAX_CMD,
    E_ACN_CMD_LIST_MAX_VALUE = INT_MAX      /* Req'd to force size to 4 bytes */
    
} NodeCmdList_t ;

/* Enumerate the OID's owned by this package. */
typedef enum eNciDataId_e 
{
    D_NCI_DATAID_BOL = OID_START (NCI), /* Offset the OID's by the package ID. */   /* 0000 */
    D_NCI_PROC_STATE,                   /* The node comms in process state.   */    /* 0001 */
    D_NCI_VERSION,                      /* Version number                     */    /* 0002 */
    D_NCI_SERVER_SHUTDOWN,              /* Server shutdown command flag.      */    /* 0003 */

                                        /* Start of message ID tallies.       */
    D_NCI_TALLY_BASE = OID_START (NCI) + 0x100,

    D_NCI_DATAID_EOL = D_NCI_TALLY_BASE + E_ACN_MAX_CMD,      /* End of list. */
    D_NCI_DATAID_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes     */
}eNciDataId_t ;

/* Clearance level for forwarding messages */

typedef Int32_t eAcnAuth_t ;


/* ACN command set (services offered to other programs) */

typedef enum
{
   E_NCI_HEARTBEAT = E_MCP_HEARTBEAT,  /* Heartbeat command                   */
   E_NCI_SHUTDOWN  = E_MCP_SHUTDOWN,   /* Shutdown command                    */
   E_NCI_SAFESTATE = E_MCP_SAFESTATE,  /* Safestate command                   */
   E_NCI_ACTIVATE  = E_MCP_ACTIVATE,   /* Activate command                    */

   E_NCI_SET_AUTH = SERVICE_START(NCI),/* Cause the ACN Comms In to terminate */
   
   E_NCI_COMMAND_EOL,                  /* End of list marker - DO NOT USE     */
   E_NCI_COMMANDS_MAX_VALUE = INT_MAX  /* Req'd to force size to 4 bytes      */
} eAcnCommands_t ;


#endif

/*
** =============================================================================
** End of File : ACNComms.h
** =============================================================================
*/