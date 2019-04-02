/*
**
** Module Name :
**    Oid.h.
**
** Purpose:
**    This file contains the definitions, the enumerations and the structure
**    of the messages associated with the OidServer functions.
**
** Description:
**    This file contains a number of message structures that are similar, but
**    have different types. This has been done to differentiate between the
**    users of the message. IT may be that at a later date the messages could
**    be rationalised.
**
** Authors:
**    rdl:     R. D. Lett (TTL).
**
** Copyright (C) Telescope Technologies Limited, 2000
**
** Version:
**    $Id: Oid.h,v 0.33 2006/10/25 10:32:49 sxf Exp $
**
** History:
**    $Log: Oid.h,v $
**    Revision 0.33  2006/10/25 10:32:49  sxf
**    Minor change to function prototype to prevent warnings when included in other packages.
**
**    Revision 0.32  2004/05/06 13:13:55  mkb
**    Switched from range of task specific agents to a single
**    generic agent type to reduce overall number of processes and
**    simplify code logic.
**    Improve communications effeciency to end-clients by remembering
**    connection details.
**    Improved distribution of diagnostic messages.
**    General tidying of code.
**
**    Revision 0.31  2004/04/15 13:24:44  sxf
**    Initial port to QNX6.
**
**    Revision 0.30  2004/01/21 16:06:48  sxf
**    Minor modifications to port to QNX6.
**
**    Revision 0.29  2003/10/14 08:23:55  mjf
**    Small changes to increase max number of agents and to error logging.
**
**    Revision 0.28  2001/09/24 08:58:25  mjf
**    Submit tallies of agent allocation errors into the SDB.
**
**    Revision 0.27  2001/09/21 09:20:37  mjf
**    Submission of process state into the SDB. Shutdown delay reduced from
**    5 to 2 seconds.
**
**    Revision 0.26  2001/09/19 15:19:44  mjf
**    Updates as a result of source file name changes. No functional changes.
**
**    Revision 0.25  2001/07/23 13:32:36  mjf
**    Increase the maximum number of agents to 64. Some debug improved.
**
**    Revision 0.24  2001/05/18 08:43:26  mjf
**    On start-up, hold the pid of each spawned process and the command-line.
**    On the forwarding of each heartbeat message, poll for each spawned
**    process still being alive, and re-spawn any missing processes. Remove
**    the response to a signal when a child process is lost - a signal can
**    interrupt a 'Send'. Finally, on termination kill any remaining processes,
**    note that the only processes killed in this manner should be the agents.
**
**    Revision 0.23  2001/05/17 13:05:36  mjf
**    Addition of 'terminate' function to be called when a shutdown command
**    has passed onwards. This function delays, then shuts down the
**    RepServer before the process exits.
**
**    Revision 0.22  2001/03/22 09:34:37  mjf
**    Changing of line-endings to LF-only.
**
**    Revision 0.21  2001/03/19 17:36:49  mjf
**    Now uses a configuration file to specify the processes to be spawned.
**    Also now is portable with respect to node. The node is determined
**    using the first process name in the configuration file - this will be
**    either AZN, ELN or CSN (AMN ?).
**
**    Revision 0.20  2001/02/28 18:23:38  rdl
**    Removed temporary hash define for OID package id.
**
**    Revision 0.19  2001/02/05 11:41:02  rdl
**    Addition of new function definitions to handle PollSafe and
**    PollActive Agent functionality.
**
**    Revision 0.18  2001/01/26 16:18:04  rdl
**    Redefinition of all static functions to be external
**    apart from SignalHandler.
**
**    Revision 0.17  2000/12/17 12:14:28  rdl
**    Modification to accomodate Package OID and submission of data
**    to SDB via newly created SDB Submit Data function.
**
**    Revision 0.16  2000/12/03 11:55:01  rdl
**    Modifications as a result of splitting main source module etc.
**
**    Revision 0.15  2000/12/01 15:12:38  rdl
**    Correction of minor errors.
**
**    Revision 0.14  2000/12/01 10:14:00  rdl
**    General consolidation and tidying up of code.
**
**    Revision 0.13  2000/11/15 16:07:11  rdl
**    Amendments to allow execution on any Axis Control Node.
**
**    Revision 0.12  2000/11/02 17:04:35  rdl
**    Mods to produce first working version communicating with ECI.
**
**    Revision 0.11  2000/10/10 08:33:50  rdl
**    Rationalisation of header files including :
**    a). Conformance to TTL Coding Standard.
**    b). Additional function prototypes for TtlProcess.c
**    c). Additional error and status enumerations.
**
**    Revision 0.10  2000/10/09 13:34:50  rdl
**    Consolidation of definitions.
**
**    Revision 0.9  2000/10/06 16:20:25  rdl
**    Minor mods.
**
**    Revision 0.8  2000/10/06 16:15:30  rdl
**    Additional error enumerations.
**
**    Revision 0.7  2000/09/29 08:33:04  rdl
**    Correction to Message type enumeration.
**
**    Revision 0.6  2000/09/28 09:00:28  rdl
**    Consolidation before major changes.
**
**    Revision 0.5  2000/09/28 08:12:32  rdl
**    Minor Modifications.
**
**    Revision 0.4  2000/09/21 13:53:22  rdl
**    Addition of header and footer comment block.
**
**
**
*/

/* Required include files */

#include "TtlSystem.h"                 /* For Status_t definition             */
#include "Cil.h"
#include "Mcp.h"
#include "Asi.h"
#include "Tqw.h"

#define TEST_PROGRAM

#ifndef OID_SERVER_H_DEFINED
#define OID_SERVER_H_DEFINED                     1

#ifdef M_ACN_NODE_SERVER_MAIN
   #define E_ACN_EXTERN
   #define E_ACN_INIT(Initialisation) = Initialisation
#else
   #define E_ACN_EXTERN extern
   #define E_ACN_INIT(Initaliasation)
#endif

enum eOidStatus_e
{
   E_OID_GEN_ERR = STATUS_START (OID), /* Miscellaneous error                 */
   E_OID_UNKNOWN_AGENT_TYPE,           /* Unknown Agent                       */
   E_OID_UNKNOWN_AGENT_MSGTYPE,        /* Unknown message from agent          */
                             /* Endclient name not found for specified agent  */
   E_OID_ENDCLIENT_NAME_NOT_FOUND,     
   E_OID_SPECIFIED_AGENT_NOT_ALIVE,    /* Specified agent is not executing    */

   E_OID_SUBPROCESS_NOT_STARTED,
   E_OID_SUBPROCESS_STARTED_OK,
   E_OID_NODE_UNRESOLVED,              /* Unable to determine which node      */
   E_OID_CONFIG_FILE_MISSING,          /* Unable to open configuration file   */
   E_OID_CONFIG_FILE_FORMAT,           /* Error in format of config file      */

   E_OID_EOSTATUS_LIST,      /* End of list marker (DO NOT USE FOR STATUS)    */
   E_OID_STATUS_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes      */
} ;

typedef Int32_t eOidStatus_t ;

/* Enumerate the OID's owned by this package. */
typedef enum eOidDataId_e 
{
   D_OID_DATAID_BOL = OID_START (OID), /* Offset the OID's by the package ID. */   /* 0000 */
   D_OID_PROC_STATE,                   /* The node comms in process state.   */    /* 0001 */
   D_OID_VERSION,                      /* Version number                     */    /* 0002 */
   D_OID_SERVER_SHUTDOWN,              /* Server shutdown command flag.      */    /* 0003 */

                                       /* Error tally for allocating agents. */
                                       /* Must be same order as eAgentType.  */
   D_OID_ERR_AGENT_GET_VALUE,          /* Errors allocating get value agent  */
   D_OID_ERR_AGENT_SET_VALUE,          /* Errors allocating set value agent  */
   D_OID_ERR_AGENT_GET_ATTRIB,         /* Errors allocating get attrib agent */
   D_OID_ERR_AGENT_SET_ATTRIB,         /* Errors allocating set attrib agent */
   D_OID_ERR_AGENT_HEARTBEAT,          /* Errors allocating heartbeat agent  */
   D_OID_ERR_AGENT_TRACK,              /* Errors allocating track agent      */
   D_OID_ERR_AGENT_POLLSAFE,           /* Errors allocating pollsafe agent   */
   D_OID_ERR_AGENT_POLLACTIVE,         /* Errors allocating pollactive agent */
   D_OID_ERR_AGENT_GENERIC,            /* Errors allocating generic agent    */

   D_OID_DATAID_EOL,                   /* End of list marker.                */
   D_OID_DATAID_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes     */
} eOidDataId_t ;

/* Define the OID base of the agent errors */

#define D_OID_ERR_AGENT_BASE        ( D_OID_ERR_AGENT_GET_VALUE - 1 )


#define E_ACN_MAX_NUM_OF_AGENTS     128
#define E_MAX_OID                   20

#define    E_TTL_TOP_SIXTEEN_BIT_MASK           0xff0000
#define    E_TTL_TOP_SIXTEEN_BIT_SHIFTR               16


/* macro for checking the validity of OID values */
#define VALID_OID(a)          ((a > 0) && (a < MAX_OID)) ? TRUE : FALSE


#if defined USE_DUMMY_TABLE

/* For AGENT usage only */

typedef struct
{
    Int32_t nType ;          /* data type specifier, use DataType enum        */
    Int32_t nWidth ;         /* data field width bin bytes [ use sizeof() ]   */

}  DataDescriptor_t ;

/* 
** Elemental structure of the dummy ADT table used for testing.
** This structure differs from that in acnAdtPrivate.h in that 
** the structure contains object value and timestamp instead of
** a pointer to the location of these values.                  
*/

typedef struct 
{
    Int16_t          nOID ;            /* object identifier                   */

    char             chAttrib ;        /* value attribute,        'R' / 'W'   */
    DataDescriptor_t Descriptor ;      /* no bytes in data element            */

    double           nMinVal ;         /* minimum value of variable           */
    double           nMaxVal ;         /* maximum value of variable           */

    Bool_t           bFRepActive ;     /* fast report active flag             */
    char             chFRepAttrib ;    /* fast report attribute, 'R' / 'W'    */

    Bool_t           bRepActive ;      /* report active flag                  */
    char             chRepAttrib ;     /* report attribute,      'R' / 'W'    */

    Int32_t          nUnits ;          /* refer to sdb.h, eSdbUnits_t struct  */

    Bool_t           bChanged ;        /* flag indicates value has changed    */

    Int32_t          adtValueHigh ;    /* offset to value in structure        */
    Int32_t          adtValueLow ;     /* offset to value in structure        */

    eTtlTime_t       ttsTimetamp ;     /* offset to timestamp in structure    */
} Object_t ;


enum DataType
{
    BOOL     = 1,
    INT8     = 2,
    INT16    = 3,
    INT32    = 4,
    DOUBLE   = 5,
    TTL_TIME = 6
} ;

typedef struct
{
   Int32_t Value ;           /* Actual value of the global task data          */
   Int32_t Units ;           /* The units of that value (for decoding)        */
} eAcnTaskData_t ;


#endif

extern Status_t iOidServerSetup (int            argc,
                                 char           *argv[],
                                 eCilMsg_t      *MsgPtr) ;
                                             
                     
extern Status_t iAcnProcessMsg (pid_t         SenderPid,
                                eWflMsg_t    *MsgPtr ) ;
                                
extern Status_t iAcnProcessRequest (eWflMsg_t    *MsgPtr, 
                                    eWflPulseId_t  ClientPid) ;

extern Status_t iAcnForwardResultToClient (eWflMsg_t    *MsgPtr) ;
                                             
extern Status_t iAcnReportRequestError (eWflMsg_t  *MsgPtr,
                                        pid_t       ClientPid,
                                        Int32_t     ErrorNum) ;
                                          
extern Status_t iAcnForwardRequestToAgent (eWflMsg_t    *MsgPtr,
                                           pid_t         AgentPid,
                                           char         *ProcName) ;
                                                    
extern void iAcnSignalHandler (int SigNum) ;

extern pid_t    iAcnSpawnProcess    ( char *CommandLine );

extern Status_t  iAcnDetermineEndClientName (Int32_t   OIDValue,
                                             char      *ProcessName) ;

extern Status_t  iOidSubmitSDBData (void) ;


#endif                                 /* OID_SERVER_H_DEFINED                 */ 

/*
** =============================================================================
** End of File : OidServer.h
** =============================================================================
*/