/*
**
** Module Name:
**       Asc.h
**
** Purpose:
**
** Description:
**      This file provides the public prototypes and definitions
**      for use with the node's ASC package.
**
** Authors:
**    JDM:     J. D. Mills (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**    $Id: Asc.h,v 0.7 2004/06/07 13:13:34 mkb Exp $
**
** History:
**    $Log: Asc.h,v $
**    Revision 0.7  2004/06/07 13:13:34  mkb
**    Add OID to specify that TFP pulses occuring within the same
**    time period should be processed (not processed by default).
**
**    Revision 0.6  2004/05/19 08:44:34  mkb
**    Add process priority for QNX6
**
**    Revision 0.5  2004/05/06 12:26:25  mkb
**    Improve diagnostic information.
**    Improve handling of time caching.
**    Improve detection of message types.
**
**    Revision 0.4  2001/02/01 09:56:50  jdm
**    E_ASC_SERVER_PRI definition added.
**
**    Revision 0.3  2000/12/21 09:39:11  jdm
**    Package revision and shutdown OIDs added.
**
**    Revision 0.2  2000/11/09 10:17:42  jdm
**    Local definition of ASC removed.
**
**    Revision 0.1  2000/11/09 08:17:07  jdm
**    Initial creation.
**
**
*/

#ifndef _ASC_HEADER_INCLUDED
#define _ASC_HEADER_INCLUDED

/*
** =============================================================================
** Start of include files:
** =============================================================================
*/

#include "TtlSystem.h"      /* Include the package definitions. */

/*
** =============================================================================
** End of include files:
** =============================================================================
*/

/*
** =============================================================================
** Start of Type Definitions:
** =============================================================================
*/

/* Enumerate this package's error codes. */
typedef enum{
   E_ASC_MESSAGE_NOT_RECOGNISED = STATUS_START(ASC),
   E_ASC_FAILED_TO_CONFIGURE_TIMER,
   E_ASC_OID_TABLE_INIT_FAILURE,
   E_ASC_FAILED_TO_SEND_MESSAGE_TO_END_CLIENT,
   E_ASC_FAILED_TO_CONFIGURE_INTERRUPT_PROXY,
   E_ASC_TIMING_STUTTER,                    /* Timing stutter. Performance may be degraded. */
   
   E_ASC_SOFTWARE_ERROR = STATUS_START(ASC) + 0xFFFF
}eAscClientStatus_t;

/* Enumerate the OID's owned by this package. */
typedef enum eAscDataId_e{
   D_ASC_DATAID_BOL = OID_START(ASC),  /* Offset the OID's by the package ID. */       /* 0000 */
   D_ASC_PROC_STATE,                   /* The ASC process state. */                    /* 0001 */
   D_ASC_APP_VERSION,                  /* ASC package revision. */                     /* 0002 */
   D_ASC_SERVER_SHUTDOWN,              /* Server shutdown command flag. */             /* 0003 */

   D_ASC_STOP_ENABLE,                  /* The Enable the stop function. */             /* 0004 */
   D_ASC_AXIS_STOPPED_MOVING,          /* The axis has stopped moving. */              /* 0005 */
   D_ASC_AXIS_STOP_FAILURE,            /* Failed to stop the axis moving. */           /* 0006 */
   D_ASC_INTERRUPT_STUTTER_IGNORE,     /* Ignore TFP pulses if timestamp not unique. *//* 0007 */
   D_ASC_DATAID_EOL                    /* End of list marker. */
}eAscDataId_t;

/*
** =============================================================================
** End of Type Definitions:
** =============================================================================
*/


/*
** =============================================================================
** Start of Specific Definitions:
** =============================================================================
*/

/* Define this server's name-server name and the name of its executeable. */
#define E_ASC_REG_SERVER_NAME   "E_TTL_ASC"
#define E_ASC_SERVER_PROC       "Asc"
#define E_ASC_SERVER_PATH       ""

#ifdef  E_WFL_OS_QNX6
#define E_ASC_SERVER_PRI                 50
#endif
#ifdef  E_WFL_OS_QNX4
#define E_ASC_SERVER_PRI                 20
#endif

#endif  /* _ASC_HEADER_INCLUDED */                                                    

/*
** =============================================================================
** End of header:   Asc.h
** =============================================================================
*/

