/*******************************************************************************
** Module Name:
**    CmtPrivate.h
**
** Purpose:
**    Private header file for Computer Monitoring Task.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.01 1-Nov-2000.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: CmtPrivate.h,v 1.27 2007/10/16 08:33:09 mjf Exp $
**
** History:
**    $Log: CmtPrivate.h,v $
**    Revision 1.27  2007/10/16 08:33:09  mjf
**    CMT_1_09.
**
**    Revision 1.26  2007/10/04 10:27:15  mjf
**    CMT_1_08.
**
**    Revision 1.25  2006/10/17 13:57:45  sxf
**    Baseline CMT_1_07.
**
**    Revision 1.24  2006/09/28 12:47:04  sxf
**    Updated for QNX6.3SP2.
**
**    Revision 1.23  2006/08/23 09:49:12  mjf
**    Minor improvements for Linux.
**
**    Revision 1.22  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 1.21  2005/07/14 09:20:46  sxf
**    Basic monitoring functionality now implemented. Monitoring of:
**       o CPU load
**       o Disk space
**       o Used memory
**
**    Revision 1.20  2004/06/21 17:31:47  mjf
**    Version 1.02.
**
**    Revision 1.19  2004/06/15 16:59:24  mjf
**    Version 1.01.
**
**    Revision 1.18  2004/04/06 12:47:07  mjf
**    Re-application of SXF's QNX v6 porting to CMT_0_11.
**
**    Revision 1.17  2004/03/26 14:29:20  mjf
**    Version number fixed. No functional changes.
**
**    Revision 1.16  2003/12/03 10:30:37  sxf
**    Updated for baseline CMT_0_09.
**
**    Revision 1.15  2003/12/02 14:46:25  sxf
**    Corrected prototype for iCmtGetLinuxDiskFree
**
**    Revision 1.14  2003/10/07 08:45:26  sxf
**    Updated for baseline CMT_0_08.
**
**    Revision 1.13  2003/09/19 12:46:30  mkb
**    Update version number from 0.06 to 0.07.
**
**    Revision 1.12  2003/09/19 12:39:29  mkb
**    Change VMS path to config file to UTILS_DIR to be consitant
**    with CCT.
**
**    Revision 1.11  2003/05/19 14:18:12  sxf
**    Increased Cil receive timeout to 100ms from 1ms.
**
**    Revision 1.10  2003/02/11 14:24:07  sxf
**    Updated for release CMT_0_05
**
**    Revision 1.9  2003/02/11 11:17:21  sxf
**    Removed spurious command line argument.
**    ,
**
**    Revision 1.8  2003/02/10 16:46:15  sxf
**    Add prototype for iCmtDetermineState
**
**    Revision 1.7  2003/02/06 11:59:50  sxf
**    Added variables for disk free warning.
**
**    Revision 1.6  2002/08/01 11:13:29  sxf
**    Updated for version 0.04.
**
**    Revision 1.5  2002/08/01 10:44:37  sxf
**    Major changes due to splitting into system specific source files and
**    addition of config file to control what is reported.
**
**    Revision 1.4  2002/01/18 10:10:50  mjf
**    Checking in of development left by ASP.
**
**    Revision 1.3  2001/03/16 21:31:04  asp
**    Hardware monitoring added.
**    Port to VMS updated.
**
**    Revision 1.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 1.1  2000/11/14 17:29:44  asp
**    Initial revision
**
**
*******************************************************************************/


#ifndef CMT_PRIVATE_H_DEFINED
#define CMT_PRIVATE_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/


#include "TtlSystem.h"

#include "Cmt.h"

#include "Cil.h"
#include "Clu.h"
#include "Sdb.h"


/*
** Local include files
*/


/*
** Macro definitions
*/

/*
** Determine operating system
*/
#undef I_CMT_OS_LINUX
#undef I_CMT_OS_QNX4
#undef I_CMT_OS_QNX6
#undef I_CMT_OS_VMS

#if defined __GNUC__ && !defined __QNXNTO__

#define I_CMT_OS_LINUX 

#endif

#if defined __QNX__ && !defined __QNXNTO__

#define I_CMT_OS_QNX4

#endif

#if defined __QNXNTO__

#define I_CMT_OS_QNX6

#endif

#if defined __VMS

#define I_CMT_OS_VMS

#endif

/* About the CMT */

#define I_CMT_PROGRAM_NAME   "Cmt"
#define I_CMT_PROGRAM_ABOUT  "Computer Monitoring Task"
#define I_CMT_YEAR           "2000-07"
#define I_CMT_RELEASE_DATE   "15 October 2007"
#define I_CMT_MAJOR_VERSION  1
#define I_CMT_MINOR_VERSION  9
#define I_CMT_MVERSIONS_PER_VERSION 1000

/* Definitions of CIL addresses that shall be used */

#define I_CMT_CIL_CHB        E_CIL_CHB /* Heartbeat source */
#define I_CMT_CIL_PMM        E_CIL_MCP /* Process control */
#define I_CMT_CIL_SDB        E_CIL_SDB /* Database */
#define I_CMT_CIL_MCB        E_CIL_MCB /* Command router */

/* Maximum length of CIL message to be received by CMT */

#define I_CMT_CIL_DATALEN    256

/* Timeout in milliseconds for receiving CIL messages */

#define I_CMT_CIL_RX_TIMEOUT 100

/* Time after which safe-state is assumed if no heartbeats have been received */

#define I_CMT_LOSS_HB_SEC    3
#define I_CMT_LOSS_HB_NSEC   0

/* For unavailable data the units are set to E_SDB_NO_UNITS */
#define I_CMT_BAD_UNITS E_SDB_NO_UNITS

/* For declaring, defining and initialising global variables */

#ifdef M_CMT_MAIN_C
#define I_CMT_EXTERN
#define I_CMT_INIT( Initialisation ) = Initialisation
#else
#define I_CMT_EXTERN         extern
#define I_CMT_INIT( Initialisation )
#endif

/* Parameters for configuring the serial port */

#define I_CMT_BAUD           9600
#define I_CMT_DATABITS       8
#define I_CMT_STOPBITS       1
#define I_CMT_PARITY         E_SCL_EVENPARITY

/* String and length of name of serial port device */

#define I_CMT_DEVICE_STRING  "%s%d"
#define I_CMT_DEFAULT_DEVICE "/dev/ser"
#define I_CMT_DEVICE_LEN     16

/* Maximum timeout for serial port replies - specified in microseconds */

#define I_CMT_TX_TIMEOUT     1000

/* Maximum buffer sizes for messages sent/received to/from serial port */

#define I_CMT_TX_BUFFER_SIZE 256
#define I_CMT_RX_BUFFER_SIZE 256

/* Invalid CIL ID (used as an error trap) */

#define I_CMT_INVALID_CIL_ID -1

/* Invalid index (used as an error trap) */

#define I_CMT_INVALID_INDEX  -1

/* CMT defaults - may be overriden with custom runtime parameters */

#define I_CMT_DFTL_PORT      -1

/* Defaults for common runtime parameters */

#define I_CMT_DFLT_QUIET     FALSE
#define I_CMT_DFLT_VERBOSE   !( I_CMT_DFLT_QUIET )
#define I_CMT_DFLT_SYSLOG    TRUE
#define I_CMT_DFLT_DEBUG     E_LOG_NOTICE
#define I_CMT_DFLT_PRIORITY  10
#define I_CMT_DFLT_HELP      FALSE
#define I_CMT_DFLT_LOG       "\0"
#define I_CMT_DFLT_CIL       "CM0"

#ifdef I_CMT_OS_QNX4    /* QNX4   */
#define I_CMT_DFLT_CONFIG    "/opt/ttl/etc/Cmt.cfg"
#define I_CMT_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"

#define I_CMT_DISK1_ROOT     "/"
#define I_CMT_DISK2_ROOT     "/disk2/"
#define I_CMT_DISK3_ROOT     "/opt/"
#define I_CMT_DISK4_ROOT     "/var/"

#endif

#ifdef I_CMT_OS_QNX6    /* QNX6   */
#define I_CMT_DFLT_CONFIG    "/ttl/sw/etc/Cmt.cfg"
#define I_CMT_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"

#define I_CMT_DISK1_ROOT     "/"
#define I_CMT_DISK2_ROOT     "/disk2/"
#define I_CMT_DISK3_ROOT     "/opt/"
#define I_CMT_DISK4_ROOT     "/var/"

#endif
#ifdef I_CMT_OS_LINUX   /* Linux */
#define I_CMT_DFLT_CONFIG    "/ttl/sw/etc/Cmt.cfg"
#define I_CMT_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#endif

#ifdef I_CMT_OS_VMS      /* OpenVMS */
#define I_CMT_DFLT_CONFIG    "UTILS_DIR:Cmt.cfg"
#define I_CMT_DFLT_CIL_MAP   "TCSLIBC_DIR:Cil.map"
#endif


/* Characters necessary to match custom command-line arguments */

#define I_CMT_CUSTOM_MATCH   1

#define I_CMT_OSCALL_FAILED  -1

/* Statistics information */
#define I_CMT_DISK_LAST      D_CMT_DISK4_FREE_PERCENT
#define I_CMT_DISK_MAX       4
#define I_CMT_MILLIPERCENT   100000.0
#define I_CMT_KILOBYTE       1024
#define I_CMT_VMS_DISKTYPE   "*D*"
#define I_CMT_QNX_BLOCKSIZE  2048

/*
** Type definitions
*/

/* Custom arguments for CMT */

enum iCmtCustomArg_e
{
   I_CMT_ARG_PORT = 0,
   I_CMT_ARG_ENDLIST
};

/*
** Enumerated list of hardware monitoring chips.
*/
typedef enum iCmtHwMonChip_e
{
   I_CMT_HWMON_BOL,
   I_CMT_HWMON_NONE,       /* Perform no hardware monitoring. */
   I_CMT_HWMON_AUTO,       /* Autodetect hardware. */
   I_CMT_HWMON_AS99127F,   /* Asustech. */
   I_CMT_HWMON_W83XXXXX,   /* Winbond. */
   I_CMT_HWMON_GL520SM,    /* Gensys. */
   I_CMT_HWMON_MAX1247,    /* Maxim. */
   I_CMT_HWMON_EOL
} iCmtHwMonChip_t;

/*
** Strings corresponding to enumerated list above.
** These strings are those to be used in the config file. 
** NB Strings and enum list must be in identical order. 
*/

#define I_CMT_MAX_BUFLEN          80  /* General buffer length. */
#define I_CMT_MAX_FILENAME_LEN    256 /* Maximum file length. */

/*
** Define idetifiers and names to be used in configuration file
** to choose which method to attempt to talk to the hardware
** monitoring chip for this particular node.
*/

#define I_CMT_HWMETHOD_NONE       0   /* No hardware monitoring. */
#define I_CMT_HWMETHOD_IOPORT     1   /* IO port method for HW monitor. */
#define I_CMT_HWMETHOD_SMBUS      2   /* SMBUS method for HW monitor. */

#define I_CMT_HWMETHOD_NAME_NONE   "NONE"    /* No hardware monitoring. */
#define I_CMT_HWMETHOD_NAME_IOPORT "IOPORT"  /* IO port name string, */
#define I_CMT_HWMETHOD_NAME_SMBUS  "SMBUS"   /* SMBUS name string. */

#define I_CMT_CPU_LOAD_PERIOD     30         /* Only calculate CPU load every N cycles */
#define I_CMT_NTP_CHECK_PERIOD    60         /* Only check NTP lock every N cycles */
#define I_CMT_MEM_FREE_PERIOD     30         /* Only calculate memory free every N cycles */

#ifdef M_CMT_MAIN_C
const char iCmtHwMonChipNames[ ][ I_CMT_MAX_BUFLEN ] =
{
   "UNKNOWN CHIP", /* I_CMT_HWMON_BOL */
   "NONE",         /* I_CMT_HWMON_NONE */
   "AUTO",         /* I_CMT_HWMON_AUTO */
   "AS99127F",     /* I_CMT_HWMON_AS99127F */
   "W83XXXXX",     /* I_CMT_HWMON_W83XXXXX */
   "GL520SM",      /* I_CMT_HWMON_GL520SM */
   "MAX1247",      /* I_CMT_HWMON_MAX1247 */
   "UNKNOWN_CHIP"  /* I_CMT_HWMON_EOL */
};
#else
extern char iCmtHwMonChipNames[ ][ I_CMT_MAX_BUFLEN ];
#endif

/* Structure for SDB submissions */

typedef struct
{
   Uint32_t NumElts;                        /* Element count for submission to SDB */
                                            
   eSdbDatum_t Datum[ D_CMT_DATAID_EOL ];   /* Data to be submitted to the SDB */

} iCmtSdbData_t;

/* Structure for CMT data store */

typedef struct
{
   ProcState_t ActualState;            /* Overall actual state of CMT */
   ProcState_t NominalState;           /* Whether in safe-state or not */
   Bool_t      DiskWarn;               /* Warning that disk nearly full */
   Bool_t      NtpWarn;                /* Warning that NTP is unlocked */
   Int32_t  CilId;                     /* Our own CIL identifier */
   int      DiskFreeWarn[ I_CMT_DISK_MAX ]; /* Warn when disk free % is */
                                       /* lower than DiskFreeWarn[ ] */
   char     DisksToMonitor[I_CMT_DISK_MAX ][ I_CMT_MAX_FILENAME_LEN ];
                                       /* Paths of disks to monitor. */
   int      NumDisksToMonitor;         /* Number of disks to monitor. */
   iCmtHwMonChip_t HwMonitorChip;      /* Type of hardware monitoring chip. */
   int      HwMonitorMethod;           /* Hardware monitoring method. */
   int      HeartbeatCount;            /* Number of heartbeats received */
   int      HeartbeatLossCount;        /* Count at which heartbeats lost */
   int      StatusRequestCount;        /* Count of currently pending requests */
   Bool_t   SdbDatumChanged[ D_CMT_DATAID_EOL ];
                                       /* Only submit changed SDB data. */
   iCmtSdbData_t SdbParams;            /* For submitting data into the SDB */
   eTtlTime_t LastHeartbeat;           /* Time of last received heartbeat */
   eTtlTime_t HeartbeatTimeOut;        /* After which PMM assumes safe-state */
   eTtlTime_t HeartbeatLossTime;       /* Next time when PMM goes safe-state */
   eCilMsg_t ChbResponseMsg;           /* CIL response to heartbeat message */ 
   eCilMsg_t McpReplyMsg;              /* Replies to MCP messages */ 
   eCilMsg_t CmdReplyMsg;              /* Replies to incoming commands */ 
   eCilMsg_t StatusMsg;                /* Message containing latest status */
   eCilMsg_t SdbSubmissionMsg;         /* SDB submission message */
   eCilMsg_t RxMsg;                    /* Incoming CIL message */
} iCmtData_t;


/*
** Variables - external to all Cmt modules
*/

/* CMT data area (containing global variables) */

I_CMT_EXTERN iCmtData_t iCmtData;


/*
** Define first and final SDB datum.
*/
#define I_CMT_FIRST_DATUMID    D_MCP_FIRST_USER_DATUM
#define I_CMT_FINAL_DATUMID    D_CMT_DATAID_EOL - 1

/* Array of SDB datum units - MUST BE IN THE SAME ORDER AS 'eCmtDataId_e' */

I_CMT_EXTERN eSdbUnit_t iCmtSdbDatumType[]
#ifdef M_CMT_MAIN_C
   = { E_SDB_INVALID_UNITS,            /* For start of list - not submitted */

       E_SDB_PROCSTATE_UNITS,          /* Proc state  */
       E_SDB_AUTH_STATE_UNITS,         /* Auth state */
       E_SDB_INVALID_UNITS,            /* Sys Request */
       E_SDB_MVERSION_UNITS,           /* App. version */

       E_SDB_MPERCENT_UNITS,           /* CPU used percent */

       E_SDB_MPERCENT_UNITS,           /* Disk1 usage percent */
       E_SDB_MPERCENT_UNITS,           /* Disk2 usage percent */
       E_SDB_MPERCENT_UNITS,           /* Disk3 usage percent */
       E_SDB_MPERCENT_UNITS,           /* Disk4 usage percent */

       E_SDB_MPERCENT_UNITS,           /* Memory free percent */
       
       E_SDB_MCELSIUS_UNITS,           /* Temperature1 */
       E_SDB_MCELSIUS_UNITS,           /* Temperature2 */
       E_SDB_MCELSIUS_UNITS,           /* Temperature3 */

       E_SDB_RPM_UNITS,                /* Fan1 RPM */
       E_SDB_RPM_UNITS,                /* Fan2 RPM */
       E_SDB_RPM_UNITS,                /* Fan3 RPM */
       
       E_SDB_MVOLT_UNITS,              /* CPU Minus 12V          */
       E_SDB_MVOLT_UNITS,              /* CPU Core voltage       */
       E_SDB_MVOLT_UNITS,              /* CPU Reference  voltage */
       E_SDB_MVOLT_UNITS,              /* CPU IO voltage         */
       E_SDB_MVOLT_UNITS,              /* CPU Plus   5V          */
       E_SDB_MVOLT_UNITS,              /* CPU Plus  12V          */
       E_SDB_MVOLT_UNITS,              /* CPU Minus  5V          */       
       E_SDB_MVOLT_UNITS,              /* CPU Minus 12V          */

       E_SDB_INVALID_UNITS             /* For end of list - not submitted */
     }
#endif
;



/* For parsing custom command-line arguments */

I_CMT_EXTERN eCluArgSpec_t  eCluCustomArg[]
#ifdef M_CMT_MAIN_C
   = { { E_CLU_EOL,         0,                  E_CLU_EOL,       FALSE, NULL } }
#endif
;


/*
** Function prototypes
*/

/* CmtMain.c */
Status_t iCmtMonitor         ( void );
Status_t iCmtReceiveMessage  ( void );
void     iCmtShutdown        ( void );
void     iCmtDetermineState  ( void );

/* CmtInit.c */
Status_t iCmtSetup           ( int ArgCount, char *ArgPtr[] );
Status_t iCmtParseCommandLine( int ArgCount, char *ArgPtr[] );
Status_t iCmtProcessConfig   ( void );

/* CmtInterface.c */
Status_t iCmtGetExtStatus    ( void );

/* CmtReport.c */
Status_t iCmtSubmitStatus    ( void );

/* CmtUtil.c */
void     iCmtOutputStatus    ( Bool_t TimeFlag );
Status_t iCmtSafeState       ( void );
Status_t iCmtActivate        ( void );
void     iCmtSetState        ( ProcState_t CmtState );
void     iCmtRestoreState    ( void );
ProcState_t iCmtGetState     ( void );
Status_t iCmtSendCilMsg      ( Int32_t CilId, eCilMsg_t * CilMsg );
Status_t iCmtSubmitToSdb     ( iCmtSdbData_t * SdbData );
Status_t iCmtProcessHeartbeat( eTtlTime_t * TimeReceived );
Status_t iCmtProcessShutdown ( void );
Status_t iCmtProcessSafeState( void );
Status_t iCmtProcessActivate ( void );
Status_t iCmtProcessCommand  ( void );
void     iCmtReadHardware    ( void );
int      iCmtDetectHardware  ( void );

Status_t iCmtGetCpuUsed( void );
Status_t iCmtGetDiskFree( void );
Status_t iCmtGetMemoryFree( void );
Status_t iCmtGetTemperatures( void );
Status_t iCmtGetFanSpeed( void );
Status_t iCmtGetVoltages( void );
Status_t iCmtGetNtpLocked( void );

/* CmtQnx.c */

#ifdef I_CMT_OS_QNX4

Status_t iCmtGetQnxCpuUsed( Int32_t *, Int32_t * );
Status_t iCmtGetQnxDiskFree( Int32_t *, Int32_t *, Int32_t *,
                             Int32_t *, Int32_t * );
Status_t iCmtGetQnxMemoryFree( Int32_t *, Int32_t *);
Status_t iCmtGetQnxTemperature( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetQnxFanSpeed( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetQnx4NtpLocked( Bool_t * );

#endif

/* CmtQnx.c */

#ifdef I_CMT_OS_QNX6

Status_t iCmtGetQnx6CpuUsed( Int32_t *, Int32_t * );
Status_t iCmtGetQnx6DiskFree( Int32_t *, Int32_t *, Int32_t *,
                             Int32_t *, Int32_t * );
Status_t iCmtGetQnx6MemoryFree( Int32_t *, Int32_t *);
Status_t iCmtGetQnx6Temperature( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetQnx6FanSpeed( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetQnx6NtpLocked( Bool_t * );

#endif

/* CmtVms.c */
#ifdef __VMS

Status_t iCmtGetVmsCpuUsed( Int32_t *, Int32_t * );
Status_t iCmtGetVmsDiskFree( Int32_t *, Int32_t *, Int32_t *,
                             Int32_t *, Int32_t * );
Status_t iCmtGetVmsMemoryFree( Int32_t *, Int32_t *);
Status_t iCmtGetVmsTemperature( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetVmsFanSpeed( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetVmsNtpLocked( Bool_t * );


#endif

/* CmtLinux.c */
#ifdef I_CMT_OS_LINUX

Status_t iCmtGetLinuxCpuUsed( Int32_t *, Int32_t * );
Status_t iCmtGetLinuxDiskFree( Int32_t *, Int32_t *, Int32_t *,
                             Int32_t *, Int32_t * );
Status_t iCmtGetLinuxMemoryFree( Int32_t *, Int32_t *);
Status_t iCmtGetLinuxTemperature( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetLinuxFanSpeed( Int32_t *, Int32_t *, Int32_t *, Int32_t *);
Status_t iCmtGetLinuxNtpLocked( Bool_t * );


#endif

#endif

/*******************************************************************************
** End of File Name: CmtPrivate.h
*******************************************************************************/
