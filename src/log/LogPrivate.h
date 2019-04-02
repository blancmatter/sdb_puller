/*
** Header File Name:
**    LogPrivate.h
**
** Purpose:
**    Private header file for System Logging (LOG) functions.
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay
**
** Version:
**    $Id: LogPrivate.h,v 0.10 2006/07/10 15:18:12 mjf Exp $
**
** History:
**    $Log: LogPrivate.h,v $
**    Revision 0.10  2006/07/10 15:18:12  mjf
**    Default host now 'localhost'.
**
**    Revision 0.9  2005/06/20 13:31:45  mjf
**    Minor changes to aid porting to Linux - no functional changes.
**
**    Revision 0.8  2004/01/19 13:56:47  sxf
**    Modified default path for QNX6.#
**
**    Revision 0.7  2000/12/18 11:12:40  mjf
**    Code added to use optional configuration parameters to specify the
**    logging parameters.
**
**    Revision 0.6  2000/11/21 14:28:10  djm
**    Increased the size of the prefix/instance substrings allowed in the
**    eLogSetup() call.
**
**    Revision 0.5  2000/11/21 10:09:10  djm
**    Updated eLogSetup() function prototype and added some length
**    check #defines.
**
**    Revision 0.4  2000/10/04 11:00:15  djm
**    Added symbolic constant to change whether LOG uses connection
**    or fire-and-forget UDP packet transmission.
**
**    Revision 0.3  2000/07/14 15:57:32  djm
**    Added \n at end of file for GCC port.
**
**    Revision 0.2  2000/06/06 13:38:03  djm
**    Made independant from syslog.h. Slight name changes.
**
**    Revision 0.1  2000/06/05 11:34:17  djm
**    Initial creation.
**
**
*/


#ifndef LOGPRIVATE_H_DEFINED
#define LOGPRIVATE_H_DEFINED



/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition */
#include "Wfl.h"
#include "Log.h"
#include "Cil.h"


/* Symbolic constants */

#define I_LOG_ISSYSLOG            /* Un-define if not available on this system */
#define I_LOG_SHOWTEXT            /* NOT YET IMPLEMENTED */
#define I_LOG_LANGUAGE            /* NOT YET IMPLEMENTED */
#define I_LOG_MAXSIZE      256    /* Maximum size of a log message */
#define I_LOG_MAXPREFIXLEN 32     /* Max. string-length of an executable name */
#define I_LOG_MAXINSTLEN   32     /* Max. string-length of an instance name */
#define I_LOG_NOSTATUS     0      /* Status to report, when there is no status */
#define I_LOG_DEST_ID  E_CIL_LOG  /* Destination CIL ID for messages */

#define I_LOG_DEFNAME  "syslog"   /* Default service name */
#define I_LOG_DEFPROT  "udp"      /* Default service protocol */
#define I_LOG_DEFHOST  "localhost" /* Default service host name */
#define I_LOG_DEFPORT  514        /* Default service port number */

#ifdef E_WFL_OS_VMS
#define I_LOG_DFLT_CFG_FILE  "TCSLIBC_DIR:Log.cfg"
#elif defined E_WFL_OS_QNX4       /* Default configuration file, QNX v4 */
#define I_LOG_DFLT_CFG_FILE  "/opt/ttl/etc/Log.cfg"
#else
#define I_LOG_DFLT_CFG_FILE  "/ttl/sw/etc/Log.cfg"
#endif

#define I_LOG_LABEL_SERVICE  "NAME"    /* service name label (config file) */
#define I_LOG_LABEL_PROTOCOL "PROTOCOL"/* service protcol label (config file) */
#define I_LOG_LABEL_HOST     "HOST"    /* service host label (config file) */
#define I_LOG_LABEL_PORT     "PORT"    /* service port label (config file) */

#define I_LOG_CFG_PARAM_NAME 1
#define I_LOG_CFG_PARAM_ARG  2


/* Define I_LOG_CONNECTION_BASED to use connect()/send() instead of sendto() */

/*
#define I_LOG_CONNECTION_BASED
*/


/* Global variables */

Bool_t iLogPrintCon;         /* TRUE if we print to terminal/console */
Bool_t iLogPrintNet;         /* TRUE if we send to syslogd */
char   *iLogPrefixPtr;       /* Pointer to string with callers ID string */
char   *iLogInstancePtr;     /* Pointer to string with callers instance ID */
int    iLogPrioMask;         /* Priority mask for which messages to print */
FILE   *iLogFilePtr;         /* Pointer to string with local log file name */
int    iLogSkt;              /* UDP socket file handler */
struct sockaddr iLogSktAddr; /* UDP socket address */


#endif

/* EOF */
