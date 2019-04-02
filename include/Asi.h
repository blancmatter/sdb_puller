/*
**
** Module Name :
**    Asi.h.
**
** Purpose:y
**    This file contains the definitions, the enumerations and the structure
**    of the messages associated with the Agent Server functions.
**
** Description:
**    The message format is the same as that defined in the system include
**    file (sys/sys_msg.h). The messages are a continuation of those defined
**    in that file.
**    This file contains a number of message structures that are similar, but
**    have different types. This has been done to differentiate between the
**    users of the message. IT may be that at a later date the messages could
**    be rationalised.
**
** Authors:
**    rdl:     R. D. Lett (TTL).
**
** Version:
**    $Id: Asi.h,v 0.29 2004/04/27 12:39:47 mkb Exp $
**
** History:
**    $Log: Asi.h,v $
**    Revision 0.29  2004/04/27 12:39:47  mkb
**    Added generic agent type.
**
**    Revision 0.28  2004/04/08 15:18:06  sxf
**    Further QNX v6 porting.
**
**    Revision 0.27  2004/01/19 15:09:14  sxf
**    Minor changes porting to QNX6.
**
**    Revision 0.26  2003/10/02 15:21:57  man
**    Included an additional Can message to
**    zero the microlynx errors from Ttl software.
**
**    Revision 0.25  2002/02/14 10:18:43  rdl
**    Added additional error code for identifying invalid state for
**    reception of a MOVE Command.
**
**    Revision 0.24  2001/11/22 10:37:42  mjf
**    Additional message subtype, for TFP-generated proxy containing the
**    system time.
**
**    Revision 0.23  2001/10/24 13:47:20  mjf
**    Addition of proxy message 'type'.
**
**    Revision 0.22  2001/09/19 13:24:09  mjf
**    Renamed from being 'AgentServer.h'.
**
**    Revision 0.21  2001/04/20 11:03:39  pxh
**    Added new end-client commands for use by CanServer.
**
**    Revision 0.20  2001/01/31 12:36:42  djm
**    Added POLLSAFE/ACTIVE sub-types and command enumeration.
**
**    Revision 0.19  2000/12/11 11:58:00  rdl
**    Error codes for success made equivalent to SYS_NOMINAL.
**    Removeal of Cil Obj Val and Cil Attr Val Structures.
**
**    Revision 0.18  2000/12/01 15:12:38  rdl
**    Correction of minor errors.
**
**    Revision 0.17  2000/11/01 12:46:15  rdl
**    Change to constant used to force enumerated list types to occupy four (4) bytes.
**
**    Revision 0.16  2000/11/01 09:59:10  rdl
**    Change to enumerated type lists to force sizeof to report
**    size as four (4) bytes.
**
**    Revision 0.15  2000/10/25 17:03:58  rdl
**    Debugging of Node Agent server and associated communication functions.
**
**    Revision 0.14  2000/10/13 16:29:02  rdl
**    Re-organisation of message structures.
**    Re numbering of enumerated types to split into bands.
**
**    Revision 0.13  2000/10/10 08:33:50  rdl
**    Rationalisation of header files including :
**    a). Conformance to TTL Coding Standard.
**    b). Additional function prototypes for TtlProcess.c
**    c). Additional error and status enumerations.
**
**    Revision 0.12  2000/10/06 12:52:26  rdl
**    Addition definitions added to eMessageError enumeration type.
**
**    Revision 0.11  2000/10/05 12:55:24  rdl
**    Correction of spelling error.
**
**    Revision 0.10  2000/10/04 13:44:54  mkv
**    Included more error codes to enum eMessageError_t.
**
**    Revision 0.9  2000/10/04 09:45:24  rdl
**    Removal of structure element OID_Major.
**    Renaming of structure element OID_Minor to OID.
**
**    Revision 0.8  2000/10/04 08:48:20  rdl
**    Addition of Generic Error Code type definition.
**    Modification of other enumerations in method of definition.
**
**    Revision 0.7  2000/10/02 11:29:11  rdl
**    Correction of typo in Message Error enumeration.
**
**    Revision 0.6  2000/09/29 14:24:03  rdl
**    Additional Error Message enumerations defined.
**
**    Revision 0.5  2000/09/29 08:42:44  rdl
**    Removal of tabs added by vedit.
**
**    Revision 0.4  2000/09/29 08:34:14  rdl
**    Correction to Message Type enumeration.
**
**    Revision 0.3  2000/09/28 16:26:24  rdl
**    Renamed enumerated "MessageType" to indicate direction of flow of message.
**    Added additional message types.
**
**    Revision 0.2  2000/09/28 15:48:07  rdl
**    Consolidation of messages to use generic message structure.
**
**    Revision 0.1  2000/09/28 08:18:24  rdl
**    Initial Version.
**
**
**
*/

#ifndef ASI_H_INCLUDED

#include "Wfl.h"

#ifdef E_WFL_OS_QNX4
#ifndef __SYS_MSG_H_INCLUDED
#include <sys/sys_msg.h>
#endif
#endif

#include "Sdb.h"

/*
** =============================================================================
**
** =============================================================================
*/

/*
** =============================================================================
** This definition defines the server identity.
** It is intended that this shall replace TYPE field in the sysmsg_hdr
** structure contained in the system file syssys_msg.h.
** =============================================================================
*/

#define    E_TTL_AGENT_SERVER               0xfffe
#define    E_TTL_PROXY_MESSAGE              0xfffd

/*
** =============================================================================
** These definitions define :
** a). The maximum size of the name that can be registered with the
**     QNX Name Server.
** b). The maximum size of the executable process
** c). The maximum size of the path to the executable process
** =============================================================================
*/

#define    E_TTL_MAX_PROC_NAME_LENGTH           32
#define    E_TTL_MAX_EXE_NAME_LENGTH            64
#define    E_TTL_MAX_PATH_NAME_LENGTH           64

/*
** =============================================================================
** This definition defines the size of the payload within the End Client
** message structure.
** =============================================================================
*/

#define    E_TTL_OIDDATA_LEN                     8

/*
** =============================================================================
** These enumeration definitions are intended to replace the SUBTYPE field in
** the sysmsg_hdr structure contained in the system file sys/sys_msg.h.
** The enumerations are used to specify the different commands that may be
** issued to an Axis Control Node (ACN).
** =============================================================================
*/
typedef  enum  eAgtMsgType_s
{
   E_TTL_AGT_MSG_TYPE_START_LIST   =  0,    /* Start of list - DO NOT USE     */

   SDB_SUBMIT_SUBTYPE,                      /* Submission of data to SDB      */
   MCS_HEARTBEAT_GET_SUBTYPE,               /* Heartbeat Status request       */
   MCS_HEARTBEAT_GET_REPLY_SUBTYPE,         /* Heartbeat Status reply         */
   MCS_GETVALUE_SUBTYPE,                    /* Get Value Command request      */
   MCS_SETVALUE_SUBTYPE,                    /* Set Value Command request      */
   MCS_GETVALUE_REPLY_SUBTYPE,              /* Get Value Command reply        */
   MCS_SETVALUE_REPLY_SUBTYPE,              /* Set Value Command reply        */
   MCS_GETATTR_SUBTYPE,                     /* Get Attribute Command request  */
   MCS_SETATTR_SUBTYPE,                     /* Set Attribute Command request  */
   MCS_GETATTR_REPLY_SUBTYPE,               /* Get Attribute Command reply    */
   MCS_SETATTR_REPLY_SUBTYPE,               /* Set Attribute Command reply    */
   MCS_TRACKCMD_SUBTYPE,                    /* Track Command Set Request      */
   MCS_TRACKCMD_REPLY_SUBTYPE,              /* Track Command Set Reply        */
   MCS_POLLSAFE_SUBTYPE,                    /* Poll safe Command Get Request  */
   MCS_POLLSAFE_REPLY_SUBTYPE,              /* Poll safe Command Get Reply    */
   MCS_POLLACTIVE_SUBTYPE,                  /* Poll active Command Get Req'st */
   MCS_POLLACTIVE_REPLY_SUBTYPE,            /* Poll active Command Get Reply  */
   TFP_TIMER_PROXY_SUBTYPE,                 /* Proxy from TFP containing time */

   E_TTL_AGT_MSG_TYPE_END_LIST,             /* End of list - DO NOT USE       */
   E_TTL_AGT_MSG_TYPE_MAX_VALUE = INT_MAX   /* Req'd to force size to 4 bytes */
} eAgtMsgType_t ;

/*
** =============================================================================
** These enumerations define the type of the message that may be issued.
** =============================================================================
*/

typedef  enum  eMessageType_s
{
                                  /* Start of message type list -  Do Not Use */
   E_TTL_MESSAGE_TYPE_START_LIST = 40,

   E_TTL_AGENT_AVAILABLE,         /* Agent - Server  : reporting for Work     */
   E_TTL_SERVER_AGENT_REQUEST,    /* Server - Agent  : Work Package to do     */
   E_TTL_AGENT_SERVER_RESULT,     /* Agent - Server  : Work Results           */
   E_TTL_CLIENT_SERVER_REQUEST,   /* Client - Server : Service Request        */
   E_TTL_SERVER_CLIENT_RESULT,    /* Server - Client : Client Request Result  */

   E_TTL_MESSAGE_TYPE_END_LIST,   /* End of message type list -  Do Not Use   */
   E_TTL_MESSAGE_TYPE_MAX_VALUE = INT_MAX   /* Req'd to force size to 4 bytes */
} eMessageType_t ;

/*
** =============================================================================
** These enumerations define the service that the agent has been requested
** to provide.
** =============================================================================
*/

typedef  enum  eAgentType_s
{
   E_TTL_AGENT_TYPE_START_LIST = 50,   /* Start : Agent type list - Don't Use */

   E_TTL_AGENT_GET_VALUE,              /* Handles - Get Value Cmds            */
   E_TTL_AGENT_SET_VALUE,              /* Handles - Set Value Cmds            */
   E_TTL_AGENT_GET_ATTRIB,             /* Handles - Get Attribute Cmds        */
   E_TTL_AGENT_SET_ATTRIB,             /* Handles - Set Attribute Cmds        */

   E_TTL_AGENT_HEARTBEAT,              /* Handles - Heartbeat Responses       */
   E_TTL_AGENT_TRACK_CMDS,             /* Handles - Track commands            */
   E_TTL_AGENT_POLLSAFE_CMD,           /* Handles safe command polling        */
   E_TTL_AGENT_POLLACTIVE_CMD,         /* Handles activate command polling    */

   E_TTL_AGENT_GENERIC,                /* Generic agent to handle any message */

   E_TTL_AGENT_TYPE_END_LIST,          /* End of Agent type list - Do Not Use */
   E_TTL_AGENT_TYPE_MAX_VALUE = INT_MAX     /* Req'd to force size to 4 bytes */
} eAgentType_t ;

/*
** =============================================================================
** These enumerations define the service that the End Client has been requested
** to provide.
** =============================================================================
*/

typedef  enum  eEndClientCommand_s
{
                        /* Start : End Client Command type list - Do Not Use  */
   E_TTL_ENDCLIENT_COMMAND_START_LIST = 60,

   E_TTL_ENDCLIENT_GET_VALUE,          /* Handles - Get Value Cmds            */
   E_TTL_ENDCLIENT_SET_VALUE,          /* Handles - Set Value Cmds            */
   E_TTL_ENDCLIENT_GET_ATTRIB,         /* Handles - Get Attribute Cmds        */
   E_TTL_ENDCLIENT_SET_ATTRIB,         /* Handles - Set Attribute Cmds        */
   E_TTL_ENDCLIENT_OPEN_CHAN,          /* CanServer - Open Channel Cmd        */
   E_TTL_ENDCLIENT_CLOSE_CHAN,         /* CanServer - Close Channel Cmd       */
   E_TTL_ENDCLIENT_SET_MOVE,           /* CanServer - Set Move Cmd            */
   E_TTL_ENDCLIENT_GET_POS,            /* CanServer - Get Position Cmd        */
   E_TTL_ENDCLIENT_GET_MVG,            /* CanServer - Get MVG Flag Cmd        */
   E_TTL_ENDCLIENT_GET_ERROR,          /* CanServer - Get Error Cmd           */
   E_TTL_ENDCLIENT_SET_OUTPUT,         /* CanServer - Set Output Cmd          */
   E_TTL_ENDCLIENT_GET_INPUT,          /* CanServer - Get Input Cmd           */
   E_TTL_ENDCLIENT_STOP,               /* CanServer - Stop Cmd                */
   E_TTL_ENDCLIENT_INIT_LYNX,          /* CanServer - Initialise uLynx Cmd    */
   E_TTL_ENDCLIENT_DISABLE_LYNX,       /* CanServer - Disable uLynx Cmd       */
   E_TTL_ENDCLIENT_GET_CTR2,           /* CanServer - Get CTR2 Cmd            */
   E_TTL_ENDCLIENT_GET_CTR3,           /* CanServer - Get CTR3 Cmd            */
   E_TTL_ENDCLIENT_SET_ERROR,          /* CanServer - Zero MicroLynx Error    */

                        /* End of End Client Command type list - Do Not Use   */
   E_TTL_ENDCLIENT_COMMAND_END_LIST,
                                       /* Req'd to force size to 4 bytes      */
   E_TTL_ENDCLIENT_COMMAND_MAX_VALUE = INT_MAX
} eEndClientCommand_t ;

/*
** =============================================================================
**
** =============================================================================
*/

/*
** =============================================================================
** These enumerations define the Generic Common Error Codes that may be
** generated as a result of commands issued to an Axis Control Node.
** =============================================================================
*/

typedef  enum  eGenComErrCodes_e
{
   E_TTL_GETSET_SUCCESS            = SYS_NOMINAL,/* Get / Set Operation Success */
   E_TTL_GETSETATTR_SUCCESS        = SYS_NOMINAL,/* Get / Set Attribute Success */
   E_TTL_GET_INVALID_OID           =   1,   /* Get Op. - Invalid OID          */
   E_TTL_SET_INVALID_OID           =   2,   /* Set Op. - Invalid OID          */
   E_TTL_SET_OID_READ_ONLY         =   3,   /* Set Op. - OID is Read Only     */
   E_TTL_SET_VALUE_BELOW_MIN       =   4,   /* Set Op. - Value below min.     */
   E_TTL_SET_VALUE_ABOVE_MAX       =   5,   /* Set Op. - Value Above max.     */
   E_TTL_GETATTR_INVALID_OID       =   6,   /* Get Attr Op. - Invalid OID     */
   E_TTL_GETATTR_INVALID_PARAM1    =   7,   /* Get Attr Op. - Invalid Parm1   */
   E_TTL_GETATTR_INVALID_PARAM2    =   8,   /* Get Attr Op. - Invalid Parm2   */
   E_TTL_GETATTR_INVALID_PARAM3    =   9,   /* Get Attr Op. - Invalid Parm3   */
   E_TTL_GETATTR_INVALID_PARAM4    =  10,   /* Get Attr Op. - Invalid Parm4   */
   E_TTL_GETATTR_INVALID_PARAM5    =  11,   /* Get Attr Op. - Invalid Parm5   */
   E_TTL_SETATTR_INVALID_OID       =  12,   /* Set Attr Op. - Invalid OID     */
   E_TTL_SETATTR_INVALID_PARAM1    =  13,   /* Set Attr Op. - Invalid Parm1   */
   E_TTL_SETATTR_INVALID_PARAM2    =  14,   /* Set Attr Op. - Invalid Parm2   */
   E_TTL_SETATTR_INVALID_PARAM3    =  15,   /* Set Attr Op. - Invalid Parm3   */
   E_TTL_SETATTR_INVALID_PARAM4    =  16,   /* Set Attr Op. - Invalid Parm4   */
   E_TTL_SETATTR_INVALID_PARAM5    =  17,   /* Set Attr Op. - Invalid Parm5   */
   E_TTL_TRACK_INVALID_PARM        = 101,   /* Track Cmd. - Invalid Parm      */
   E_TTL_TRACK_INVALID_STATE       = 102,   /* Track Cmd. - Invalid State     */
   E_TTL_TRACK_RESP_NOT_AVAIL      = 103,   /* Track Cmd. - Not Available     */
   E_TTL_TRACK_RESP_NOT_VALID      = 104,   /* Track Cmd. - Invalid Resp      */
   E_TTL_MOVEUNHOME_INVALID_PARM   = 105,   /* Move Unhomed Cmd. - Invalid Parm  */
   E_TTL_MOVEUNHOME_INVALID_STATE  = 106,   /* Move Unhomed Cmd. - Invalid State */
   E_TTL_MOVEUNHOME_RESP_NOT_AVAIL = 107,   /* Move Unhomed Cmd. - Not Available */
   E_TTL_MOVEUNHOME_RESP_NOT_VALID = 108,   /* Move Unhomed Cmd. - Invalid Resp  */
   E_TTL_UNHOME_INVALID_PARM       = 109,   /* Unhome Cmd. - Invalid Parm     */
   E_TTL_UNHOME_INVALID_STATE      = 110,   /* Unhome Cmd. - Invalid State    */
   E_TTL_HALT_INVALID_STATE        = 111,   /* Halt Cmd. - Invalid State      */
   E_TTL_START_INVALID_STATE       = 112,   /* Start Cmd. - Invalid State     */
   E_TTL_DIAGNOSE_INVALID_STATE    = 113,   /* Daignose Cmd. - Invalid State  */
   E_TTL_SIMULATE_INVALID_PARM     = 114,   /* Simulate Cmd. - Invalid Parm   */
   E_TTL_KEEPUNHOMED_INVALID_STATE = 115,   /* Keep Unhomed Cmd. - Invalid State     */
   E_TTL_SELMOTALGOL_INVALID_PARM  = 116,   /* Sel. Motion Algol Cmd. - Invalid Parm */
   E_TTL_WARMBOOT_INVALID_PARM     = 117,   /* Warm Boot Cmd. - Invalid Parm  */
   E_TTL_HOME_INVALID_PARM         = 118,   /* Home Cmd. - Invalid Parm       */
   E_TTL_HOME_INVALID_STATE        = 119,   /* Home Cmd. - Invalid State      */
   E_TTL_RECOVER_INVALID_PARM      = 120,   /* Recover Cmd. - Invalid Parm    */
   E_TTL_RECOVER_INVALID_LIMIT     = 121,   /* Recover Cmd. - Invalid Limit   */
   E_TTL_RECOVER_INVALID_STATE     = 122,   /* Recover Cmd. - Invalid State   */
   E_TTL_RESETHEAD_INVALID_PARM    = 123,   /* Reset Head Cmd. - Invalid Parm */
   E_TTL_RESETHEAD_INVALID_STATE   = 124,   /* Reset Head Cmd. - Invalid State */
   E_TTL_SINUSOID_INVALID_PARM     = 125,   /* Sinusoid Cmd. - Invalid Parm   */
   E_TTL_SINUSOID_INVALID_STATE    = 126,   /* Sinusoid Cmd. - Invalid State  */

   E_TTL_MOVE_INVALID_STATE        = 127,   /* Move Cmd. - Invalid State      */

   E_TTL_ERROR_CODE_END_LIST,     /* End of Gen Error Code List - Do Not Use  */
   E_TTL_ERROR_CODE_MAX_VALUE = INT_MAX     /* Req'd to force size to 4 bytes */
} eGenComErrCodes_t ;

/*
** =============================================================================
** These enumerations define the possible errors that may be generated as a
** result of handling a message.
** =============================================================================
*/

typedef  enum  eMessageError_s
{
   E_TTL_ERR_NONE                   = SYS_NOMINAL,    /* No Error             */
   E_TTL_INVALID_HDR_TYPE           =  1,   /* MCS Header type                */
   E_TTL_INVALID_HDR_SUBTYPE        =  2,   /* MCS Header subtype             */
   E_TTL_INVALID_CIL_SERVICE        =  3,   /* CIL Service                    */
   E_TTL_INVALID_CIL_SOURCEID       =  4,   /* CIL Source Id                  */
   E_TTL_INVALID_CIL_SEQNUM         =  5,   /* CIL Sequence Number            */
   E_TTL_INVALID_CLIENT_MSG_TYPE    =  6,   /* Client Message Type            */
   E_TTL_INVALID_SERVER_MSG_TYPE    =  7,   /* Server Message Type            */
   E_TTL_INVALID_AGENT_MSG_TYPE     =  8,   /* Agent Message Type             */
   E_TTL_INVALID_ENDCLIENT_MSG_TYPE =  9,   /* EndClient Message Type         */
   E_TTL_INVALID_AGT_TYPE           = 10,   /* Agent Type                     */
   E_TTL_INVALID_ENDCLIENTNAME      = 11,   /* End Client Registered Proc Name */
   E_TTL_EMPTY_ENDCLIENTNAME        = 12,   /* End Client Proc Name blank or empty */
   E_TTL_INVALID_OID                = 13,   /* invalid Object Identifier      */

   E_TTL_NO_CLIENT_AVAILABLE        = 14,   /* No Client available            */
   E_TTL_NO_SERVER_AVAILABLE        = 15,   /* No Server available            */
   E_TTL_NO_AGENT_AVAILABLE         = 16,   /* No Agent available             */
   E_TTL_NO_ENDCLIENT_AVAILABLE     = 17,   /* No EndClient available         */
   E_TTL_AGENT_NOT_EXECUTING        = 18,   /* Agent not executing / running  */
   E_TTL_SERVER_NOT_EXECUTING       = 19,   /* Server not executing / running */
   E_TTL_CLIENT_NOT_EXECUTING       = 20,   /* Client not executing / running */
   E_TTL_ENDCLIENT_NOT_EXECUTING    = 21,   /* End Client not executing / running */

   E_TTL_OPERATION_NOT_ALLOWED      = 22,   /* Operation Not Allowed          */
   E_TTL_INVALID_OPERATION          = 23,   /* Invalid Operation : Not Get or Set */
   E_TTL_INVALID_OID_VALUE          = 24,   /* OID value out of range         */
   E_TTL_WRITE_PROTECT              = 25,   /* OID Write protected            */
   E_TTL_NOT_FOUND                  = 26,   /* Could not link to owning process  */
   E_TTL_INVALID_UNITS_CODE         = 27,   /* Object Value Units not recognised */

   E_TTL_NO_LOG_FILE_AVAILABLE,             /* No log file available          */
   E_TTL_INVALID_PID,                       /* Process id is zero             */
   E_TTL_INIT_MSG_NULL_PTR,                 /* Passed NULL ptr into function  */
   E_TTL_REPORT_MSG_NULL_PTR,               /* Passed NULL ptr into function  */
   E_TTL_VALID_REQ_NULL_PTR,                /* Passed NULL ptr into function  */
   E_TTL_VALID_RESP_NULL_PTR,               /* Passed NULL ptr into function  */
   E_TTL_PROC_ALIVE_NULL_PTR,               /* Passed NULL ptr into function  */
   E_TTL_ENDC_REQ_NULL_PTR,                 /* Passed NULL ptr into function  */
   E_TTL_ENDC_RESP_NULL_PTR,                /* Passed NULL ptr into function  */
   E_TTL_DEBUG_REQ_VAL_MSG_NULL_PTR,        /* Passed NULL ptr into function  */
   E_TTL_DEBUG_REQ_ATR_MSG_NULL_PTR,        /* Passed NULL ptr into function  */
   E_TTL_DEBUG_RESP_MSG_NULL_PTR,           /* Passed NULL ptr into function  */

   E_TTL_NOT_IMPLEMENTED            = 999,  /* Not Implemented - For test use */

   E_TTL_ERROR_END_LIST,                    /* End of error list - Do Not Use */
   E_TTL_ERROR_MAX_VALUE = INT_MAX          /* Req'd to force size to 4 bytes */
} eMessageError_t ;

/*
** =============================================================================
** Object Value Structure :
** =============================================================================
*/

typedef struct
{
   Int32_t    nOID ;                   /* Object ID                           */
   Int32_t    nError ;                 /* Error Code                          */
   Int32_t    DataType ;               /* Type of data in data parm fields    */
   Int32_t    ParamHigh ;              /* Upper 32 bits of data.              */
   Int32_t    ParamLow ;               /* Lower 32 bits of data.              */
   eTtlTime_t TimeStamp ;              /* TTL time structure                  */
   eSdbUnit_t Units ;                  /* Variable Units for data value       */
} ObjVal_t ;

/*
** =============================================================================
** Attribute Value Structure :
** =============================================================================
*/

typedef struct
{
   Int32_t     nOID ;                  /* object ID                           */
   Int32_t     nError ;                /* error value field                   */
   Int32_t     nValue_RW ;             /* object value read or write char     */
   Int32_t     nReport ;               /* normal reporting enabled flag       */
   Int32_t     nReport_RW ;            /* normal reporting read or write char */
   Int32_t     nFast ;                 /* fast reporting enabled flag         */
   Int32_t     nFast_RW ;              /* fast reporting read or write char   */
   eSdbUnit_t  nUnits ;                /* variable units data for SDB         */
} AttrVal_t ;

/*
** =============================================================================
** CIL Data Message Header structue :
** Describes the message structure that is used within the ACN to save the
** CIL specific data.
** =============================================================================
*/

typedef  struct
{
   Int32_t      Service ;              /* Cil Service                         */
   Int32_t      SourceId ;             /* CIL Source id                       */
   Uint32_t     SeqNum ;               /* CIL Sequence Number                 */
} eCILDataStore_t ;

/*
** =============================================================================
** Agent - Agent/Server Message Header structue :
** Describes the message structure that is used within the ACN by the Agent
** Server and the various agents.
** =============================================================================
*/

typedef  struct
{
   eMessageType_t  MsgType ;           /* Agent-Server Msg Type               */
   eAgentType_t    AgtType ;           /* Agent Type                          */
                                       /* End Client Registered Proc Name     */
   char            EndClientName [E_TTL_MAX_PROC_NAME_LENGTH] ;
   Int32_t         CommsErrorCode ;    /* Transmission layer Error Code       */
} eAgentMsg_t ;

/*
** =============================================================================
** End Client Message Header structue :
** Describes the message structure that is used between the Agent and the
** End Client.
** =============================================================================
*/

typedef  struct
{
   eEndClientCommand_t   EndClientCommand ;
   Int32_t               OIDData [E_TTL_OIDDATA_LEN] ;
} eEndClientMsg_t ;

/*
** =============================================================================
** Generic ACN Message Structure :
** This a generic message that is used for internal messages between the
** multiple processes within the Axis Control Node (ACN).
** The GenPayLoad will contain either :
** a). Object Value structure    (ObjVal_t).
** b). Attribute Value structure (AttrVal_T).
**
** This is used for the following messages :
**
** Message Subtype : _MCS_HEARTBEAT_GET_SUBTYPE
** Used by : Internal Node Processes to request the current state of the node.
**           The data obtained is returned in the response to the reception of
**           the Heartbeat.
**
** Message Subtype : _MCS_HEARTBEAT_GET_REPLY_SUBTYPE
** Used by : Internal Node Processes to return the reply to a request for the
**           current state of the node.
**           The data obtained is returned in the response to the reception of
**           the Heartbeat.
**
** Message Subtype : _MCS_AET_GETVALUE_SUBTYPE
** Used by : Internal Node Processes to request the current value of parameter
**           in the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_AET_SETVALUE_SUBTYPE
** Used by : Internal Node Processes to set the current value of parameter in
**           the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_AET_GETVALUE_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to get the
**           current value of parameter in the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_AET_SETVALUE_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to set the
**           current value of parameter in the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_GETVALUE_SUBTYPE
** Used by : Internal Node Processes to request the current value of parameter
**           in the MCS.
**
** Message Subtype : _MCS_SETVALUE_SUBTYPE
** Used by : Internal Node Processes to set the current value of parameter in
**           the MCS.
**
** Message Subtype : _MCS_GETVALUE_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to get the
**           current value of parameter in the MCS.
**
** Message Subtype : _MCS_SETVALUE_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to set the
**           current value of parameter in the MCS.
**
** Message Subtype : _MCS_AET_GETATTR_SUBTYPE
** Used by : Internal Node Processes to request the current attributes of a
**           parameter in the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_AET_SETATTR_SUBTYPE
** Used by : Internal Node Processes to set the current attributes of a
**           parameter in the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_AET_GETATTR_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to get the current
**           attributes of a parameter in the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_AET_SETATTR_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to set the current
**           attributes of a parameter in the Absolute Encoder Task (AET).
**
** Message Subtype : _MCS_GETATTR_SUBTYPE
** Used by : Internal Node Processes to request the current attributes of a
**           parameter in the Axis Control Node Task (ACN).
**
** Message Subtype : _MCS_SETATTR_SUBTYPE
** Used by : Internal Node Processes to set the current attributes of a
**           parameter in the Axis Control Node Task (ACN).
**
** Message Subtype : _MCS_GETATTR_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to get the current
**           attributes of a parameter in the Axis Control Node Task (ACN).
**
** Message Subtype : _MCS_SETATTR_REPLY_SUBTYPE
** Used by : Internal Node Processes to reply to the request to set the current
**           attributes of a parameter in the Axis Control Node Task (ACN).
**
** =============================================================================
*/

#ifdef E_WFL_OS_QNX4
typedef   struct
{
   struct   _sysmsg_hdr   hdr ;        /* QNX specific message header struct  */
   eCILDataStore_t    CILStore ;       /* Storage structure for CIL data      */
   eAgentMsg_t        AgtMsg ;         /* Agent/Server message Header struct  */
   eEndClientMsg_t    EndClientMsg ;   /* End Client message header struct    */
} eGenACNMsg_t ;
#endif

#ifdef E_WFL_OS_QNX6
typedef   struct
{
#if 0
   struct   _pulse    hdr ;            /* QNX specific message header struct  */
#endif
   eCILDataStore_t    CILStore ;       /* Storage structure for CIL data      */
   eAgentMsg_t        AgtMsg ;         /* Agent/Server message Header struct  */
   eEndClientMsg_t    EndClientMsg ;   /* End Client message header struct    */
} eGenACNMsg_t ;
#endif

/*
** =============================================================================
** Message Subtype : _SDB_SUBMIT_SUBTYPE
** Used by : Internal Node Processes to submit data to SDB via SDB Reporter.
** =============================================================================
*/

#ifdef E_WFL_OS_QNX4
typedef   struct
{
   struct   _sysmsg_hdr   hdr ;    /* QNX specific message header struct  */
   eWflPulseId_t       PulseId;    /* Connection identifier */
   ObjVal_t     ObjVal ;           /* Object Identifier Structure         */
} SdbSubmit_t ;
#endif

#ifdef E_WFL_OS_QNX6
typedef   struct
{
   struct   _pulse   hdr ;         /* QNX specific message header struct  */ 
   eWflPulseId_t       PulseId;    /* Connection identifier */
   ObjVal_t          ObjVal ;      /* Object Identifier Structure         */
   Int32_t           Dummy[3];

} SdbSubmit_t ;
#endif


/*
** =============================================================================
**
** =============================================================================
*/

#define ASI_H_INCLUDED                 1
#endif

/*
** =============================================================================
** End of File : AgentServer.h
** =============================================================================
*/
