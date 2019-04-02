/*******************************************************************************
** Module Name:
**    CctOsInterface.h
**
** Purpose:
**    Header file for operating system interface in CCT.
**
** Description:
**    This file contains definitions for the interface between the CCT and the 
**    operating system.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    MCP Detailed Design v0.06
**
** Author(s):
**    Martyn J. Ford (mjf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: CctOsInterface.h,v 0.7 2006/11/13 11:27:02 mkb Exp $
**
** History:
**    $Log: CctOsInterface.h,v $
**    Revision 0.7  2006/11/13 11:27:02  mkb
**    Shutdown more elegantly than with "shutdown -f" - use verbose (-v) instead.
**
**    Revision 0.6  2004/01/16 09:38:27  sxf
**    Minor changes porting to QNX6.
**
**    Revision 0.5  2001/06/20 13:17:20  mjf
**    Changes to VMS logical names. Default CIL map now "CIL_MAP", and the
**    directory for default configuration file and reboot/shutdown scripts
**    is now "COMPTASK_DIR".
**
**    Revision 0.4  2001/03/14 14:58:04  mjf
**    Porting to VMS complete.
**
**    Revision 0.3  2001/03/09 15:06:24  mjf
**    Increased POSIX compliance and full support for Linux.
**
**    Revision 0.2  2001/03/08 16:54:44  mjf
**    Full functionality for QNX added. Datums only submitted to the SDB
**    when they are re-determined. Query datums are re-determined on a
**    corresponding 'get' command.
**
**    Revision 0.1  2001/03/07 12:00:48  mjf
**    Initial revision.
**
**
*******************************************************************************/

#ifndef CCT_INTERFACE_H_DEFINED
#define CCT_INTERFACE_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/


/*
** Local include files
*/


/*
** Macro definitions
*/

/* maximum number of processes that may be started in the application */

#define M_CCT_MAX_PIDS       256

/* index to program name in array of command arguments */

#define M_CCT_ARG_PROGNAME   0

/* string containing a space, used when parsing a command-line */

#define M_CCT_STR_SPACE      " "

/* invalid pid used by operating system */

#define M_CCT_PID_INVALID    -1

/* invalid flag used by operating system */

#define M_CCT_OS_INVALID     -1

/* number of a non-existent signal - used to check validity of a pid */

#define M_CCT_SIGNAL_NONE    0


#ifdef E_WFL_OS_QNX4

/* operating system commands */

#define M_CCT_CMD_RESTART    "shutdown -f"
#define M_CCT_CMD_CLOSE      "shutdown -fb"

#endif

#ifdef E_WFL_OS_QNX6

/* operating system commands */

#define M_CCT_CMD_RESTART    "shutdown -v"
#define M_CCT_CMD_CLOSE      "shutdown -vb"

#endif

#ifdef E_WFL_OS_LINUX

/* operating system commands */

#define M_CCT_CMD_RESTART    "/sbin/shutdown -r now"
#define M_CCT_CMD_CLOSE      "/sbin/shutdown -h now"

#endif


#ifdef E_WFL_OS_VMS

/* operating system commands */

#define M_CCT_CMD_RESTART    "@COMPTASK_DIR:TTL_VMS_REBOOT.COM"
#define M_CCT_CMD_CLOSE      "@COMPTASK_DIR:TTL_VMS_SHUTDOWN.COM"

#endif


/*
** Variables
*/


/*
** Type definitions
*/


#endif

/*
** EOF
*/
