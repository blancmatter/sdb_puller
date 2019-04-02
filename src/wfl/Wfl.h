/*
** Header File Name:
**    Wfl.h
**
** Purpose:
**    Header file for TTL Wrapper-function Library package.
**
** Description:
**    Contains the macros, type definitions and function prototypes needed to 
**    use the TTL Wrapper-function Library package.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Authors:
**    Steve Foale (sxf)
**
** Version:
**    $Id: Wfl.h,v 0.18 2007/10/05 09:23:33 mjf Exp $
**
** History:
**    $Log: Wfl.h,v $
**    Revision 0.18  2007/10/05 09:23:33  mjf
**    Function prototypes re-organised to reflect moving of some Wfl.c
**    functions into new file WflMsg.c.
**
**    Revision 0.17  2007/08/15 15:04:20  mjf
**    If the '-ansi' compilation switch is used, only include the initial
**    Operating System definitions.
**
**    Revision 0.16  2007/03/12 14:59:53  sxf
**    Minor modifications for building without error under linux.
**
**    Revision 0.15  2006/07/10 15:24:02  mjf
**    Cygwin support added.
**
**    Revision 0.14  2005/06/20 13:48:45  mjf
**    First phase of changes to aid porting to Linux. Public header compiles
**    but library not yet ported.
**
**    Revision 0.13  2004/06/14 07:59:35  sxf
**    Added eWflFileTruncate().
**
**    Revision 0.12  2004/06/09 12:24:01  mkb
**    Use pre-processor instructions to remove definitions that cause
**    compiler errors under VMS.
**
**    Revision 0.11  2004/06/05 13:44:47  mkb
**    Added pulse code for Get or Set messages.
**
**    Revision 0.10  2004/06/03 10:25:26  sxf
**    Corrected WflWatchdog... functions.
**
**    Revision 0.9  2004/05/07 11:07:36  mjf
**    Addition of interrupt-masking functions and find-connection.
**
**    Revision 0.8  2004/04/27 12:36:34  mkb
**    Added seperate 'Fast' functions to attach a pulse, send a
**    pulse and detach a pulse.
**
**    Revision 0.7  2004/04/16 10:47:50  sxf
**    Use eWflMsg_t for send/receive prototypes.
**
**    Revision 0.6  2004/03/24 10:34:42  sxf
**    Added eWflFindChannel and eWflSendMsgToPid.
**
**    Revision 0.5  2004/03/02 14:09:18  sxf
**    Interim check-in.
**
**    Revision 0.4  2004/02/20 15:18:07  man
**    Update for checking QNX4 code.
**
**    Revision 0.3  2004/02/17 09:43:43  man
**    Added several system header files.
**
**    Revision 0.2  2004/02/12 17:03:28  sxf
**    Check in a working version under QNX6.
**
**    Revision 0.1  2004/02/10 11:46:03  sxf
**    Initial version.
**
**
*/

#ifndef WFL_H_DEFINED
#define WFL_H_DEFINED

#include "TtlSystem.h"

/* 
** Macro definitions
*/

#define E_WFL_OS_UNSUPPORTED

#if defined __QNX__ && !defined __QNXNTO__

#undef  E_WFL_OS_UNSUPPORTED
#define E_WFL_OS_QNX4

#elif defined __QNXNTO__

#undef  E_WFL_OS_UNSUPPORTED
#define E_WFL_OS_QNX6

#endif

#if defined __VMS

#undef  E_WFL_OS_UNSUPPORTED
#define E_WFL_OS_VMS

#endif

#if defined __linux__

#undef  E_WFL_OS_UNSUPPORTED
#define E_WFL_OS_LINUX

#endif

#if defined __CYGWIN__

#undef  E_WFL_OS_UNSUPPORTED
#define E_WFL_OS_CYGWIN

#endif

#if defined E_WFL_OS_UNSUPPORTED

#error OPERATING SYSTEM NOT SUPPORTED

#endif


/*
** If compiling with the '-ansi' switch, only include the OS definitions and
** proceed no further past this point.
*/

#ifndef __STRICT_ANSI__

#if defined E_WFL_OS_QNX4

#include <string.h>
#include <sys/time.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/sys_msg.h>
#include <mqueue.h>
#include <sys/pci.h>
#include <conio.h>
#include <sys/io_msg.h>
#include <sys/irqinfo.h>
#include <sys/fd.h>
#include <sys/prfx.h>
#include <sys/types.h>
#include <sys/sched.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/psinfo.h>
#include <i86.h>
#include <netinet/in.h>
#include <sys/qioctl.h>
#include <unistd.h>
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

#elif defined E_WFL_OS_QNX6

#include <time.h>
#include <signal.h>
#ifndef __STRICT_ANSI__
#include <sys/netmgr.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/iomsg.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>
#endif
#include <mqueue.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <arpa/inet.h>
#include <netdb.h>

#elif defined E_WFL_OS_VMS

#include <time.h>
#include <signal.h>

#elif defined E_WFL_OS_LINUX

#include <time.h>
#include <signal.h>
#ifndef __STRICT_ANSI__
/*#include <sys/netmgr.h>*/
#include <sys/types.h>
#include <pthread.h>
/*#include <sys/neutrino.h>*/
/*#include <sys/iofunc.h>*/
/*#include <sys/dispatch.h>*/
/*#include <sys/iomsg.h>*/
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
/*#include <Ph.h>*/
/*#include <Pt.h>*/
/*#include <Ap.h>*/
#endif
#include <mqueue.h>
/*#include <hw/pci.h>*/
/*#include <hw/inout.h>*/
#include <arpa/inet.h>
#include <netdb.h>

#elif defined E_WFL_OS_CYGWIN

#include <time.h>
#include <signal.h>
#ifndef __STRICT_ANSI__
/*#include <sys/netmgr.h>*/
#include <pthread.h>
/*#include <sys/neutrino.h>*/
/*#include <sys/iofunc.h>*/
/*#include <sys/dispatch.h>*/
/*#include <sys/iomsg.h>*/
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
/*#include <Ph.h>*/
/*#include <Pt.h>*/
/*#include <Ap.h>*/
#endif
/*#include <mqueue.h>*/
/*#include <hw/pci.h>*/
/*#include <hw/inout.h>*/
#include <arpa/inet.h>
#include <netdb.h>

#endif


#define E_WFL_THIS_NODE     0
#define E_WFL_MAX_MSG_SIZE  100
#define E_WFL_MAX_BUFF      100
#define E_WFL_MAX_CHANNELS  16

/*
** The MQ_NOTIFY_ALWAYS flag is only available for QNX4
*/
#if defined E_WFL_OS_QNX4

#define E_WFL_MQ_NOTIFY_ALWAYS MQ_NOTIFY_ALWAYS

#elif defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX || defined E_WFL_OS_CYGWIN

#define E_WFL_MQ_NOTIFY_ALWAYS NULL

#endif
/*
** Type definitions
*/

#if defined E_WFL_OS_QNX4

typedef pid_t              eWflPulseId_t;

typedef struct eWflPulseHdr_s
{
   Uint16_t      type;
   Uint16_t      subtype;
   Int8_t        code;
} eWflPulseHdr_t;

typedef int                eWflNameAttach_t;
typedef pid_t              eWflProxPulsId_t;
typedef int                eWflCtp_t;

#elif defined E_WFL_OS_QNX6

typedef int           eWflPulseId_t;
#ifndef __STRICT_ANSI__
typedef struct _pulse eWflPulseHdr_t;
typedef name_attach_t eWflNameAttach_t;
typedef struct sigevent *  eWflProxPulsId_t;
typedef resmgr_context_t   eWflCtp_t;
#else
/*
** Dummy typedefs for ANSI compilations.
*/
typedef int eWflProxPulseId_t;
typedef int eWflPulseHdr_t;
typedef int eWflNameAttach_t;
#endif

#elif defined E_WFL_OS_VMS
/*
** Dummy definitions for VMS.
*/
typedef int              eWflPulseId_t;

typedef struct eWflPulseHdr_s
{
   Uint16_t      type;
   Uint16_t      subtype;
   Int8_t        code;
} eWflPulseHdr_t;

typedef int                eWflNameAttach_t;
typedef int                eWflProxPulsId_t;
typedef int                eWflCtp_t;

#elif defined E_WFL_OS_LINUX
/*
** Dummy definitions for Linux.
*/
typedef int              eWflPulseId_t;

typedef struct eWflPulseHdr_s
{
   Uint16_t      type;
   Uint16_t      subtype;
   Int8_t        code;
} eWflPulseHdr_t;

typedef int                eWflNameAttach_t;
typedef int                eWflProxPulsId_t;
typedef int                eWflCtp_t;

#elif defined E_WFL_OS_CYGWIN
/*
** Dummy definitions for Cygwin.
*/
typedef int              eWflPulseId_t;

typedef struct eWflPulseHdr_s
{
   Uint16_t      type;
   Uint16_t      subtype;
   Int8_t        code;
} eWflPulseHdr_t;

typedef int                eWflNameAttach_t;
typedef int                eWflProxPulsId_t;
typedef int                eWflCtp_t;

#endif

#define E_WFL_PULSECODE_INIT  32

typedef enum eWflPulseCode_s
{
   E_WFL_PULSECODE_TIMED = E_WFL_PULSECODE_INIT,
   E_WFL_PULSECODE_INTERPROCESS,
   E_WFL_PULSECODE_INTRAPROCESS,
   E_WFL_PULSECODE_MQUEUE,
   E_WFL_PULSECODE_EVENT,
   E_WFL_PULSECODE_GETSET
} eWflPulseCode_t;


typedef struct eWflMsg_s
{
   eWflPulseHdr_t  Hdr;
   eWflPulseId_t   PulseId;
   char            Msg[ E_WFL_MAX_MSG_SIZE ];
} eWflMsg_t;

#if defined E_WFL_OS_QNX4

typedef union
{
   unsigned short          type;
   unsigned short          status;
   struct _io_open         open;
   struct _io_open_reply   open_reply;
   struct _io_close        close;
   struct _io_close_reply  close_reply;
   struct _io_read         read;
   struct _io_read_reply   read_reply;
   struct _io_write        write;
   struct _io_write_reply  write_reply;
   struct _io_qioctl       qioctl;
   struct _io_qioctl_reply qioctl_reply;
   struct _io_dup_reply    dup_reply;
} eWflResMgrMsg_t;

#endif

#if defined E_WFL_OS_QNX6

#ifndef __STRICT_ANSI__
typedef resmgr_context_t eWflResMgrMsg_t;
#endif

#endif

#if defined E_WFL_OS_QNX4
#define E_WFL_IO_OPEN     _IO_OPEN
#define E_WFL_IO_CLOSE    _IO_CLOSE
#define E_WFL_IO_DUP      _IO_DUP
#define E_WFL_IO_READ     _IO_READ
#define E_WFL_IO_WRITE    _IO_WRITE
#define E_WFL_IO_QIOCTL   _IO_QIOCTL
#endif 

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX || defined E_WFL_OS_CYGWIN
#define E_WFL_IO_OPEN     _IO_CONNECT
#define E_WFL_IO_CLOSE    _IO_CLOSE
#define E_WFL_IO_DUP      _IO_DUP
#define E_WFL_IO_READ     _IO_READ
#define E_WFL_IO_WRITE    _IO_WRITE
#define E_WFL_IO_QIOCTL   _IO_MSG
#endif

#if defined E_WFL_OS_QNX4
typedef msg_t     eWflMsgStat_t;
typedef short int eWflMsgNbyte_t;
typedef long      eWflMsgZero_t;
#endif 

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX || defined E_WFL_OS_CYGWIN
typedef int     eWflMsgStat_t;
typedef int     eWflMsgNbyte_t;
typedef int     eWflMsgZero_t;
#endif
      
#if !defined E_WFL_OS_VMS

typedef struct eWflTimedPulse_s
{
   eWflPulseId_t PulseId;    /* Unique ID for proxy */
   timer_t       TimerId;    /* ID for timer */ 
   unsigned int  ValSecs;    /* Number of seconds for timer to expire */
   unsigned int  ValNSecs;   /* Number of nanoseconds for timer to expire */
   unsigned int  IntSecs;    /* Number of seconds for timer interval */
   unsigned int  IntNSecs;   /* Number of nanoseconds for timer interval */
} eWflTimedPulse_t;

#endif

typedef struct eWflWatchdogPci_s
{
   unsigned VendorId;              /* Vendor ID of watchdog card */
   unsigned DeviceId;              /* Device ID of watchdog card */
   unsigned BaseAddrRegsIndex;     /* Base address registers */
   unsigned InitOffset;            /* Offset from base address for init */
   unsigned InitValue;             /* Value to write to initialise watchdog*/
   unsigned ResetOffset;           /* Offset from base address for reset */
   unsigned ResetValue;            /* Value to write to reset watchdog */
   unsigned CloseOffset;           /* Offset from base address to close */
   unsigned CloseValue;            /* Value to write to close watchdog */
   unsigned CloseCount;            /* Number of times to write close byte */
} eWflWatchdogPci_t;

typedef enum eWflMessageType_s
{
   E_WFL_MSGTYPE_BOL,              /* Bottom of list, do not use */
   E_WFL_MSGTYPE_IP_PULSE,         /* Interprocess pulse/proxy */
   E_WFL_MSGTYPE_IP_MESSAGE,       /* Interprocess message */
   E_WFL_MSGTYPE_EOL               /* End of list - do not use */
} eWflMessageType_t;

#define WFL 1

typedef enum eWflStatus_s
{
   E_WFL_NO_ERR = STATUS_START( WFL ), /* no error - unused */
   E_WFL_NOOSSUPPORT,        /* Not supported under this OS */
   E_WFL_NOIOPRIVITY,        /* Unable to gain IO privity */
   E_WFL_NOPCIBIOS,          /* PCI bios not found */
   E_WFL_NOPCIREAD,          /* Unable to read from PCI bus */
   E_WFL_NOPCIMMAP,          /* Unable to memory map PCI dev */
   E_WFL_PCINOUNMAP,         /* Unable to unmap memory */
   E_WFL_NOOSINFO,           /* Unable to read OS info */
   E_WFL_NOPCIDEVICE,        /* PCI device not found */
   E_WFL_SHMERR,             /* Shared memory error */
   E_WFL_NOSETSCHED,         /* Unable to set scheduler */
   E_WFL_NOTIMER,            /* Unable to create timer */
   E_WFL_NOTIMERUPDATE,      /* Unable to update timer */
   E_WFL_NOSETPERIOD,        /* Unable to set clock period */
   E_WFL_NOPULSE,            /* No pulses have been set up */
   E_WFL_NOTPULSE,           /* Message received wasn't a pulse */
   E_WFL_PULSERCVERR,        /* Pulse receive error */
   E_WFL_PULSESNDERR,        /* Pulse send error */
   E_WFL_NOCHANNEL,          /* No channel currently open */
   E_WFL_NOATTACH,           /* Unable to attach name to channel */ 
   E_WFL_NODETACH,           /* Unable to detach name */
   E_WFL_NONAMEOPEN,         /* Unable to open name for server connect */
   E_WFL_NONAMECLOSE,        /* Unable to close name for server connect */
   E_WFL_MSGRCVERR,          /* Error trying to receive message */
   E_WFL_MSGSNDERR,          /* Error trying to send message */
   E_WFL_MSGREPLYERR,        /* Error replying to message */
   E_WFL_NOTERMREAD,         /* Unable to read from terminal device */
   E_WFL_ERR_WATCHDOG,       /* Unable to initialise watchdog */
   E_WFL_NODEVNO,            /* Unable to obtain device number */
   E_WFL_NOPREFIX,           /* Unable to attach device name to process */
   E_WFL_NOIRQATTACH,        /* Unable to attach interrupt handler */
   E_WFL_NOIRQDETACH,        /* Unable to detach interrupt handler */
   E_WFL_NODPP,              /* No dispatch handle */
   E_WFL_RESMGERR,           /* Error in resource manager block */
   E_WFL_RESRECERR,          /* Error in resource manager receive */
   E_WFL_RESREPERR,          /* Error in resource manager reply */
   E_WFL_MSGREADERR,         /* Error reading message */
   E_WFL_MSGWRITERR,         /* Error writing message */
   E_WFL_DEVCTLERR,          /* Device control error */
   E_WFL_NOPHHAND,           /* Error assigning input processing function */
   E_WFL_FILEERR,            /* Error handling file */
   E_WFL_INTERRUPTED         /* Function was interrupted */
} eWflStatus_t;


/* 
** Function prototypes
*/

#if !defined E_WFL_OS_VMS && !defined E_WFL_OS_CYGWIN

/*
** WflPci.c
*/
Status_t eWflPciSetup        ( void );
Status_t eWflPciFindDevice   ( unsigned ,
                               unsigned ,
                               unsigned ,
                               unsigned *,
                               unsigned * );
Status_t eWflPciReadConfig32 ( unsigned ,
                               unsigned ,
                               unsigned ,
                               unsigned ,
                               char * );

Status_t eWflPciReadConfig8 ( unsigned ,
                               unsigned ,
                               unsigned ,
                               unsigned ,
                               char * );

Status_t eWflPciMap( Uint32_t ,
                     void **  ,
                     size_t    );

Status_t eWflPciUnmap( void *, size_t  );

void         eWflOut8       ( int , int );
unsigned int eWflIn8        ( int );

/*
** Wfl.c
*/   

Status_t     eWflSetScheduler(     int Policy,
                                   int Priority );

Status_t     eWflTimerCreate(      clockid_t         ,
                                   struct sigevent  *,
                                   timer_t          * );

Status_t     eWflSetClockPeriod(   unsigned long );

Status_t     eWflGetClockPeriod(   unsigned long * );

Status_t     eWflReadTermData(     int               ,
                                   void             *,
                                   int               ,
                                   int               ,
                                   int               ,
                                   int               ,
                                   int              * );

Bool_t       eWflKeyboardHit(      void               );

char         eWflGetKeyPress(      void               );

Status_t     eWflDevCtlMsg(        int filedes, 
                                   unsigned request, 
                                   void *sbuf, 
                                   unsigned sbytes, 
                                   void *rbuf, 
                                   unsigned rbytes );

Status_t     eWflFileTruncate(     int   FileDesc,
                                   off_t Size );

Status_t     eWflDelay(            int MilliSeconds ); 


/*
** WflMsg.c
*/

Status_t     eWflCreatePulse(      struct sigevent *, 
                                   eWflPulseId_t   * );

Status_t     eWflCheckPulse(       eWflPulseId_t );

Status_t     eWflCreateTimedPulse( eWflTimedPulse_t * );

Status_t     eWflPulseOnMqueueEmpty( mqd_t            ,
                                     eWflPulseId_t   *,
                                     struct sigevent * );

Status_t     eWflUpdateTimer(      eWflTimedPulse_t * );

Status_t     eWflReceivePulse(     eWflPulseId_t    * );

Status_t     eWflDetachPulse(      eWflPulseId_t      );

Status_t     eWflNameAttach(       const char       *,
                                   eWflNameAttach_t * );

Status_t     eWflNameDetach(       eWflNameAttach_t * );

Status_t     eWflNameOpen(         const char       *,
                                   eWflPulseId_t    * );

Status_t     eWflNameClose(        eWflPulseId_t );

Status_t     eWflMsgReceive(       eWflMsg_t        *,
                                   int               ,
                                   eWflPulseId_t    *,
                                   eWflPulseId_t    * );

Status_t     eWflMsgSend(          eWflPulseId_t,
                                   const eWflMsg_t *,
                                   int              ,
                                   eWflMsg_t       *,
                                   int                );

Status_t     eWflMsgSendToPid( pid_t          Pid, 
                               int            Chid,
                               const eWflMsg_t *SendMsgPtr,                              
                               eWflMsg_t      *ReplyMsgPtr,
                               int            SendSize,
                               int            ReplySize );

Status_t     eWflMsgSendNoBlock(   eWflPulseId_t    , 
                                   const eWflMsg_t *,
                                   int              ,
                                   eWflMsg_t       *,
                                   int                );

Status_t     eWflSndMsgToPid ( pid_t          , 
                               eWflMsg_t     *,
                               int              );

Status_t     eWflSendPulse( pid_t Pid, int Channel );
Status_t     eWflAttachPulseFast( pid_t Pid, int Channel, int *CoidPtr );
Status_t     eWflSendPulseFast  ( int Coid );
Status_t     eWflDetachPulseFast( int Coid );

Status_t     eWflFindChannel( pid_t Pid, int *Chid );

Status_t     eWflFindConnection( pid_t          Pid,
                                 int           *Chid,
                                 eWflPulseId_t *Coid );

Status_t     eWflMsgReply(         eWflPulseId_t     , 
                                   const eWflMsg_t  *,
                                   int                );

Status_t     eWflDeviceAttach(     int              * ); 

#ifndef __STRICT_ANSI__
Status_t     eWflEnableInterrupt( void ); 
Status_t     eWflDisableInterrupt( void ); 

Status_t     eWflHintAttach( int  ,
                             void *, 
                             int  * );
                         
Status_t     eWflHintDetach( int );

Status_t     eWflMaskInterrupt( int Intr, int Id );

Status_t     eWflUnmaskInterrupt( int Intr, int Id );

Status_t     eWflSetProxPulse( eWflProxPulsId_t *,
                               struct sigevent  *,
                               eWflPulseId_t    * );

Status_t     eWflCMsgReceive ( pid_t *pid,
                               void *msg,
                               unsigned  nbytes );


/*
** WflPhab.c
*/

#if defined E_WFL_OS_QNX4 || defined E_WFL_OS_QNX6

Status_t     eWflPtAddInput( PtAppContext_t app_context,
                             pid_t pid,
                             PtInputCallbackProc_t input_func,
                             void *data);

#endif

#endif

#endif

/*
** WflWatchdog.c
*/
Status_t eWflWatchdogInit( void );
Status_t eWflWatchdogReset   ( void );
Status_t eWflWatchdogClose   ( void );
#endif

#endif

/*
** EOF
*/
