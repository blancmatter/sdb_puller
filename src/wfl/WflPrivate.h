/*
** Header File Name:
**    WflPrivate.h
**
** Purpose:
**    Private header file for TTL Wrapper-function library.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Authors:
**    Steve Foale (sxf)
**
** Version:
**    $Id: WflPrivate.h,v 0.3 2006/07/10 15:24:02 mjf Exp $
**
** History:
**    $Log: WflPrivate.h,v $
**    Revision 0.3  2006/07/10 15:24:02  mjf
**    Cygwin support added.
**
**    Revision 0.2  2005/06/20 13:48:45  mjf
**    First phase of changes to aid porting to Linux. Public header compiles
**    but library not yet ported.
**
**    Revision 0.1  2004/02/10 11:46:03  sxf
**    Initial version.
**
**
*/

#ifndef WFL_PRIVATE_H_DEFINED
#define WFL_PRIVATE_H_DEFINED

/* 
** Macro definitions
*/

/* PCI hardware watchdog card offset for device register address */

#define M_WFL_WD_DEVICE_REG  0x18

/* PCI hardware watchdog card - bitwise AND with device register address */

#define M_WFL_WD_REG_MASK    0xFFFFFFFE

/*
** Determine operating system 
*/
#if defined __QNX__ && !defined __QNXNTO__

#define I_WFL_OS_QNX4

#elif defined __QNXNTO__ 

#define I_WFL_OS_QNX6

#elif defined __VMS

#define I_WFL_OS_VMS

#elif defined __linux__

#define I_WFL_OS_LINUX

#elif defined __CYGWIN__

#define I_WFL_OS_CYGWIN

#else

#define I_WFL_OS_UNSUPPORTED

#error OPERATING SYSTEM NOT SUPPORTED

#endif

/* 
** System includes for each operating system
*/

#ifdef I_WFL_OS_QNX4

#include <conio.h>
#include <sys/osinfo.h>
#include <sys/pci.h>
#include <sys/dev.h>
#include <sys/sched.h>

#endif

#ifdef I_WFL_OS_QNX6

#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <sched.h>

#endif


#include "Log.h"

/*
** Type definitions
*/

/*
** Structure to hold parameters about the PCI watchdog card 
*/

typedef struct iWflWatchdog_s
{
   Bool_t   Exists;                /* flag TRUE if watchdog exists */
   unsigned Index;                 /* Index into array for valid watchdog */
   unsigned BusNum;                /* the bus number of the PCI device */
   unsigned DevFunctionNum;        /* the device and function number */
   Uint32_t DeviceReg;             /* point to I/O mapped device register */
} iWflWatchdog_t;

/* 
** Function prototypes
*/


#endif

/*
** EOF
*/
