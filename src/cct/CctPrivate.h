/*******************************************************************************
** Module Name:
**    CctPrivate.h
**
** Purpose:
**    Private header file for Conputer Control Task.
**
** Description:
**    This file contains definitions internal to the CCT.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    -
**
** Author(s):
**    Martyn J. Ford (mjf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: CctPrivate.h,v 0.33 2007/12/04 14:55:36 mjf Exp $
**
** History:
**    $Log: CctPrivate.h,v $
**    Revision 0.33  2007/12/04 14:55:36  mjf
**    CCT_1_05.
**
**    Revision 0.32  2007/10/17 10:32:54  mjf
**    CCT_1_04.
**
**    Revision 0.31  2007/10/09 15:13:15  mjf
**    Fix to paths for Linux.
**
**    Revision 0.30  2007/10/04 10:26:27  mjf
**    CCT_1_03 - fix to typo in makefile, no functional changes.
**
**    Revision 0.29  2006/11/13 11:31:27  sxf
**    Updated minor version.
**
**    Revision 0.28  2006/11/13 11:25:40  sxf
**    Shutdown more elegantly than with "shutdown -f" - use verbose (-v) instead.
**
**    Revision 0.27  2006/10/02 10:27:11  sxf
**    Baseline CCT_1_01.
**
**    Revision 0.26  2004/04/22 14:16:47  mjf
**    Correction to operating system detection.
**
**    Revision 0.25  2004/04/20 16:05:01  mjf
**    Correction to paths for QNX versions.
**
**    Revision 0.24  2004/04/06 10:09:01  mjf
**    Version 1.00.
**
**    Revision 0.23  2003/12/17 15:01:09  mjf
**    Version 0.19.
**
**    Revision 0.22  2003/12/03 10:33:47  sxf
**    Updated for baseline CCT_0_19.
**
**    Revision 0.21  2003/05/19 14:35:06  sxf
**    Increase Cil receive timeout to 100ms from 1ms.
**
**    Revision 0.20  2002/10/29 15:30:30  mkb
**    Updated to V0.16
**
**    Revision 0.19  2002/10/29 15:18:47  mjf
**    Version 0.15.
**
**    Revision 0.18  2001/12/04 14:42:03  mjf
**    Version 0.14.
**
**    Revision 0.17  2001/11/30 15:22:52  mjf
**    Change of log message levels.
**
**    Revision 0.16  2001/11/07 09:10:14  mjf
**    Version 0.12.
**
**    Revision 0.15  2001/08/07 08:31:56  mjf
**    Addition of delay following the execution of a process. Set to 1000 ms
**    by default, may be altered on the command-line.
**
**    Revision 0.14  2001/07/25 10:04:28  mjf
**    Allow control messages from the TST as well as the MCP.
**
**    Revision 0.13  2001/07/17 13:39:57  mjf
**    Addition of definitions for '-shutdown' switch.
**
**    Revision 0.12  2001/07/04 16:00:17  mjf
**    Assume the default VMS CIL ID to be CC8.
**
**    Revision 0.11  2001/06/20 13:17:20  mjf
**    Changes to VMS logical names. Default CIL map now "CIL_MAP", and the
**    directory for default configuration file and reboot/shutdown scripts
**    is now "COMPTASK_DIR".
**
**    Revision 0.10  2001/06/13 10:42:33  mjf
**    If a set/get command not received from the MCP, send reply via the MCB.
**
**    Revision 0.9  2001/06/13 09:37:12  mjf
**    Query all the status information and submit it into the SDB on each
**    heartbeat.
**
**    Revision 0.8  2001/05/25 13:30:29  mjf
**    On an operating system shutdown or restart command, send the reply to
**    the command before performing the requested action.
**
**    Revision 0.7  2001/05/21 14:25:20  mjf
**    Check for non-zero pid before attempting a 'kill'.
**
**    Revision 0.6  2001/03/14 15:01:20  mjf
**    Allow only the PMM to issue 'set' commands.
**
**    Revision 0.5  2001/03/14 14:58:04  mjf
**    Porting to VMS complete.
**
**    Revision 0.4  2001/03/09 15:06:24  mjf
**    Increased POSIX compliance and full support for Linux.
**
**    Revision 0.3  2001/03/08 16:54:44  mjf
**    Full functionality for QNX added. Datums only submitted to the SDB
**    when they are re-determined. Query datums are re-determined on a
**    corresponding 'get' command.
**
**    Revision 0.2  2001/03/07 15:19:52  mjf
**    Submit to the SDB only the datums that have been updated or refreshed.
**
**    Revision 0.1  2001/03/07 12:00:48  mjf
**    Initial revision.
**
**
*******************************************************************************/

#ifndef CCT_PRIVATE_H_DEFINED
#define CCT_PRIVATE_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/

#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Wfl.h"
#include "Mcp.h"
#include "Cil.h"
#include "Clu.h"
#include "Sdb.h"
#include "Cfu.h"

#include <unistd.h>


/*
** Macro definitions
*/

/*
** Determine operating system
*/
#undef I_CCT_OS_LINUX
#undef I_CCT_OS_QNX4
#undef I_CCT_OS_QNX6
#undef I_CCT_OS_VMS

#if defined __GNUC__ && !defined __QNXNTO__

#define I_CCT_OS_LINUX 

#endif

#if defined __QNX__ && !defined __QNXNTO__

#define I_CCT_OS_QNX4

#endif

#if defined __QNXNTO__

#define I_CCT_OS_QNX6

#endif

#if defined __VMS

#define I_CCT_OS_VMS

#endif

/* about the CCT */

#define I_CCT_PROGRAM_NAME   "Cct"
#define I_CCT_PROGRAM_ABOUT  "Computer Control Task, operating system control"
#define I_CCT_YEAR           "2001-07"
#define I_CCT_RELEASE_DATE   "4 December 2007"
#define I_CCT_MAJOR_VERSION  1
#define I_CCT_MINOR_VERSION  05

/* definitions for using CIL */

#define I_CCT_CIL_CHB        E_CIL_CHB
#define I_CCT_CIL_PMM        E_CIL_MCP
#define I_CCT_CIL_SDB        E_CIL_SDB
#define I_CCT_CIL_MCB        E_CIL_MCB
#define I_CCT_CIL_TST        E_CIL_TST

/* only process allowed to issue 'set' commands is the PMM */

#define I_CCT_CIL_CONTROL    I_CCT_CIL_PMM

/* for testing, also use TST to issue 'set' commands */
#define I_CCT_CIL_ALTCONTROL I_CCT_CIL_TST

/* maximum length of CIL message to be received by CCT */

#define I_CCT_CIL_DATALEN    256

/* timeouts in mSec for receiving CIL messages */

#define I_CCT_CIL_RX_TIMEOUT 100
#define I_CCT_CIL_NO_TIMEOUT 0

/* default waiting time in mSec between executing each process */

#define I_CCT_DFLT_EXE_DELAY 1000

/* maximum length of string used for logging */

#define I_CCT_MAX_STRING_LEN 256

/* time after which safe-state is assumed if no heartbeats */

#define I_CCT_LOSS_HB_SEC    3
#define I_CCT_LOSS_HB_NSEC   0

/* for declaring, defining and initialising global variables */

#ifdef M_CCT_MAIN_C
#define I_CCT_EXTERN
#define I_CCT_INIT( Initialisation ) = Initialisation
#else
#define I_CCT_EXTERN         extern
#define I_CCT_INIT( Initialisation )
#endif

/* invalid CIL ID and name */

#define I_CCT_INVALID_CIL_ID -1
#define I_CCT_INVALID_CILSTR "???"

/* invalid index */

#define I_CCT_INVALID_INDEX  -1

/* definitions for custom runtime parameters */

#define I_CCT_CUSTOM_MATCH   4
#define I_CCT_SWITCH_SHUT    "shutdown <name>"
#define I_CCT_SWITCH_DELAY   "delay [ms]"
#define I_CCT_EXPL_SHUT      "Config file for alternative shutdown procedure"
#define I_CCT_EXPL_DELAY     "Delay between executing consecutive processes"

/* defaults for common runtime parameters */

#define I_CCT_DFLT_QUIET     FALSE
#define I_CCT_DFLT_VERBOSE   !( I_CCT_DFLT_QUIET )
#define I_CCT_DFLT_SYSLOG    TRUE
#define I_CCT_DFLT_DEBUG     E_LOG_NOTICE
#define I_CCT_DFLT_PRIORITY  10
#define I_CCT_DFLT_HELP      FALSE
#define I_CCT_DFLT_LOG       "\0"

#ifdef I_CCT_OS_QNX4    /* QNX4   */
#define I_CCT_DFLT_CIL       "CC0"
#define I_CCT_DFLT_CONFIG    "/opt/ttl/etc/Cct.cfg"
#define I_CCT_DFLT_SHUTDOWN  ""
#define I_CCT_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#endif

#ifdef I_CCT_OS_QNX6    /* QNX6   */
#define I_CCT_DFLT_CIL       "CC0"
#define I_CCT_DFLT_CONFIG    "/ttl/sw/etc/Cct.cfg"
#define I_CCT_DFLT_SHUTDOWN  ""
#define I_CCT_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#endif

#ifdef I_CCT_OS_LINUX   /* Linux */
#define I_CCT_DFLT_CIL       "CC0"
#define I_CCT_DFLT_CONFIG    "/ttl/sw/etc/Cct.cfg"
#define I_CCT_DFLT_SHUTDOWN  ""
#define I_CCT_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#endif

#ifdef I_CCT_OS_VMS      /* OpenVMS */
#define I_CCT_DFLT_CIL       "CC8"
#define I_CCT_DFLT_CONFIG    "COMPTASK_DIR:Cct.cfg"
#define I_CCT_DFLT_SHUTDOWN  "COMPTASK_DIR:CctAppStop.cfg"
#define I_CCT_DFLT_CIL_MAP   "CIL_MAP"
#endif


/*
** Type definitions
*/

/* custom arguments for PMM */

enum iCctCustomArg_e
{
   I_CCT_ARG_SHUTDOWN = 0,             /* shutdown application config file */
   I_CCT_ARG_EXE_DELAY,                /* delay between executing processes */
   I_CCT_ARG_ENDLIST
};

/* structure for holding internal datums */

typedef struct
{
   eSdbDatum_t Datum;                  /* datums to be submitted to the SDB */
   eTtlTime_t ModificationTime;        /* last time the datum was modified at */
   Bool_t   StatusObtained;            /* datum obtained in last cycle */
   Status_t ( * Function )( void );    /* pointer to set/get function handler */
} iCctParam_t;

/* structure for SDB submissions */

typedef struct
{
   Uint32_t NumElts;                   /* element count for submission to SDB */
                                       /* datums to be submitted to the SDB */
   eSdbDatum_t Datum[ D_CCT_DATAID_EOL ]; 
} iCctSdbData_t;

/* structure for CCT data store */

typedef struct
{
   ProcState_t ActualState;            /* overall actual state of CCT */
   ProcState_t NominalState;           /* whether in safe-state or not */
   Int32_t  CilId;                     /* our own CIL identifier */
   char     CilName[ E_CIL_IDLEN ];    /* name of our own CIL address */
   char     ConfigShutdown             /* name of app shutdown config file */
               [ E_CFU_STRING_LEN ];
   Int32_t  DelayExe;                  /* delay (ms) between process execs */
   eTtlTime_t TimeDelayExe;            /* exec delay as a TTL time type */
   int      HeartbeatCount;            /* number of heartbeats received */
   int      HeartbeatLossCount;        /* count at which heartbeats lost */
   Bool_t   ReplySent;                 /* TRUE when reply sent to command */
                                       /* internal copy of datums */
   iCctParam_t Param[ D_CCT_DATAID_EOL ];
   eTtlTime_t LastHeartbeat;           /* time of last received heartbeat */
   eTtlTime_t HeartbeatTimeOut;        /* after which PMM assumes safe-state */
   eTtlTime_t HeartbeatLossTime;       /* next time when PMM goes safe-state */
   eCilMsg_t ChbResponseMsg;           /* CIL response to heartbeat message */ 
   eCilMsg_t McpReplyMsg;              /* replies to MCP messages */ 
   eCilMsg_t CmdReplyMsg;              /* replies to incoming commands */ 
   eCilMsg_t SdbSubmissionMsg;         /* single-datum SDB submission message */
   eCilMsg_t RxMsg;                    /* incoming CIL message */
} iCctData_t;


/*
** Variables - external to all except CctMain
*/ 

/* CCT data area */

I_CCT_EXTERN iCctData_t iCctData;

/* for parsing custom command-line arguments */

I_CCT_EXTERN eCluArgSpec_t eCluCustomArg[]
#ifdef M_CCT_MAIN_C
 = { { I_CCT_SWITCH_SHUT,  I_CCT_CUSTOM_MATCH, I_CCT_EXPL_SHUT,  FALSE, NULL },
     { I_CCT_SWITCH_DELAY, I_CCT_CUSTOM_MATCH, I_CCT_EXPL_DELAY, FALSE, NULL },
     { E_CLU_EOL,          0,                  E_CLU_EOL,        FALSE, NULL } }
#endif
;


/*
** Function prototypes
*/

/* CctMain.c */
Status_t iCctMonitorComputer ( void );
Status_t iCctReceiveMessage  ( int Timeout );
void     iCctShutdown        ( void );

/* CctInit.c */
Status_t iCctSetup           ( int ArgCount, char *ArgPtr[] );
Status_t iCctParseCommandLine( int ArgCount, char *ArgPtr[] );
Status_t iCctProcessConfig   ( void );

/* CctOsInterface.c */
Status_t iCctOsDataInit      ( void );
Status_t iCctOsAvailable     ( void );
Status_t iCctComputerReady   ( void );
Status_t iCctAppRunning      ( void );
Status_t iCctAppStart        ( void );
Status_t iCctAppKill         ( void );
Status_t iCctOsRestart       ( void );
Status_t iCctOsClose         ( void );
Status_t mCctRunConfigFile   ( char * ConfigFile, int * ProcessCount, 
                               pid_t * ProcessPid );
void     mCctSplitArgs       ( char * CommandPtr );

/* CctUtil.c */
Status_t iCctSubmitStatus    ( void );
void     iCctOutputStatus    ( Bool_t TimeFlag );
void     iCctSetState        ( ProcState_t CctState );
void     iCctRestoreState    ( void );
ProcState_t iCctGetState     ( void );
Status_t iCctSendCilMsg      ( Int32_t CilId, eCilMsg_t * CilMsg );
Status_t iCctProcessHeartbeat( eTtlTime_t * TimeReceived );
Status_t iCctProcessShutdown ( void );
Status_t iCctProcessSafeState( void );
Status_t iCctProcessActivate ( void );
Status_t iCctProcessCommand  ( void );
void     iCctSetReply        ( eCilMsg_t * MsgPtr, Status_t Status );


#endif

/*
** EOF
*/
