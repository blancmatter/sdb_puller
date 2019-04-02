/*******************************************************************************
** Module Name:
**    Pms.h
**
** Purpose:
**    Public header file for Primary Mirror Support State Controller.
**
** Description:
**    This file contains exported by the PMS. Note that the PMS application is
**    built from the AMS source code with compile-time directives to build two
**    different applications. The two public headers Ams.h and Pms.h are 
**    interchangable between the two builds. Fot this reason this header files
**    defines some AMS-named definitions and evaluates them using the PMS 
**    package ID offsets.
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
**    $Id: Pms.h,v 0.9 2004/04/23 16:04:14 mkb Exp $
**
** History:
**    $Log: Pms.h,v $
**    Revision 0.9  2004/04/23 16:04:14  mkb
**    First working QNX 6 version.
**
**    Revision 0.8  2003/05/06 14:01:09  mkb
**     Report state for each mechanism end client to SDB.
**
**    Revision 0.7  2001/11/28 14:34:47  mjf
**    Re-introduction of 'halt demand' OID.
**
**    Revision 0.6  2001/10/17 15:33:21  mjf
**    Change of OIDs to match the NSC, and implementation of enable,
**    disable, home and stop to all end-clients in the sub-system.
**
**    Revision 0.5  2001/09/19 16:12:00  mjf
**    Updates as a result of source file name changes. No functional changes.
**
**    Revision 0.4  2001/08/16 11:20:22  mjf
**    Implementation of axis stopped OID, which is checked upon a safe-state
**    request. A safe-state request halts mechanisms rather than disables
**    them, and an activate request no longer automatically enables them.
**    Shutdown mechanism improved, and log messages improved.
**
**    Revision 0.3  2001/07/17 09:13:23  mjf
**    Enable including of both Ams.h and Pms.h by the same source file.
**
**    Revision 0.2  2001/03/28 17:44:12  mjf
**    Correction to typo.
**
**    Revision 0.1  2001/03/27 14:38:26  mjf
**    Initial revision.
**
**
*******************************************************************************/

#ifndef _PMS_H_DEFINED
#define _PMS_H_DEFINED


/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */
#include "Gsc.h"             /* Generic (node) State Controller OID offsets   */


#define E_PMS_REG_SERVER_NAME    "E_TTL_AMN_PMS"
#define E_PMS_SERVER_PROC        "Pms"
#define E_PMS_SERVER_PRI         10
#define E_PMS_SERVER_PATH        "" 

enum ePmsStatus
{
    E_PMS_GEN_ERROR = STATUS_START(PMS),
    E_PMS_INIT_FAIL,
    E_PMS_SHUTDOWN_ERROR,
    E_PMS_TIMER_SETUP_ERR,
    E_PMS_TIMER_CLOSE_ERR,
    E_PMS_INVALID_STATE,
    E_PMS_SERVER_NOT_FOUND,
    E_PMS_SEND_FAIL,
    E_PMS_INVALID_COMMAND,
    E_PMS_ENDCLIENT_SPAWN_FAIL,
    E_PMS_ENDCLIENT_NOT_FOUND,
    E_PMS_RECEIVE_ERR
};

/*
** Package OID list.
*/

typedef enum ePmsDataId_e
{
   D_PMS_DATAID_BOL                    /* Offset OIDs by generic AMS Id.  */
    = OID_START(PMS) + D_GSC_DATAID_BOL,  

   D_PMS_PROC_STATE                    /* Overall process state           */
    = OID_START(PMS) + D_GSC_PROC_STATE,  
   D_PMS_APP_VERSION                   /* Software version number         */
    = OID_START(PMS) + D_GSC_APP_VERSION,  
   D_PMS_STATE                         /* Task state                      */
    = OID_START(PMS) + D_GSC_TOP_LEVEL_STATE,  
   D_PMS_SHUTDOWN                      /* Task shutdown flag              */
    = OID_START(PMS) + D_GSC_SHUTDOWN,  
   D_PMS_ENABLE                        /* Enable flag                     */
    = OID_START(PMS) + D_GSC_ENABLE,  
   D_PMS_HOME                          /* Home flag                       */
    = OID_START(PMS) + D_GSC_HOME,           
   D_PMS_STOP_DEMAND                   /* Stop demand                     */
    = OID_START(PMS) + D_GSC_STOP_DEMAND, 
   D_PMS_SAFE_STATE                    /* Safe-state command              */
    = OID_START(PMS) + D_GSC_SAFE_STATE_COMMAND,  
   D_PMS_ACTIVATE                      /* Activate command                */
    = OID_START(PMS) + D_GSC_ACTIVATE_COMMAND,  
   D_PMS_STOPPED_STATUS                /* Stopped status                  */
    = OID_START(PMS) + D_GSC_AXIS_STOPPED, 
   D_PMS_HALT_DEMAND                   /* Halt demand                     */
    = OID_START(PMS) + D_GSC_HALT_DEMAND, 

   D_PMS_ENDCLIENT_STATE_CAN,          /* Process state of CAN end client */
   D_PMS_ENDCLIENT_STATE_INC,          /* Process state of INC end client */
   D_PMS_ENDCLIENT_STATE_SIF,          /* Process state of SIF end client */
   D_PMS_ENDCLIENT_STATE_ACM,          /* Process state of ACM end client */

   D_PMS_DATAID_EOL                    /* End-of-list marker              */
} ePmsDataId_t;


/*
** External function prototypes
*/

#endif

/*
** EOF
*/
