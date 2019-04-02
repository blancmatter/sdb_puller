/*******************************************************************************
** Module Name:
**    Cmt.h
**
** Purpose:
**    Public header file for Computer Monitoring Task.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.01 01-Nov-2000.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: Cmt.h,v 0.5 2006/03/21 11:21:56 sxf Exp $
**
** History:
**    $Log: Cmt.h,v $
**    Revision 0.5  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 0.4  2002/08/01 10:42:28  sxf
**    Added extra status return codes.
**
**    Revision 0.3  2001/03/16 21:24:59  asp
**    Added voltage monitoring
**
**    Revision 0.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 0.1  2000/11/01 17:32:34  asp
**    *** empty log message ***
**
**    Revision 1.1  2000/11/01 17:29:25  asp
**    Initial revision
**
**
*******************************************************************************/


#ifndef CMT_H_DEFINED
#define CMT_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/

#include "TtlSystem.h"
#include "Mcp.h"
#include "Sdb.h"


/*
** Local include files
*/


/*
** Macro definitions
*/

/* Size of 'action-in-progress' messages sent when commands received */
#define E_CMT_ACT_LEN_CMD    0

/* Size of 'action-completed' messages sent when commands done */
#define E_CMT_COM_LEN_CMD    sizeof( Int32_t )


/*
** Type definitions
*/

/* Enumerated list of status and error values */
typedef enum eCmtStatus_e
{
   E_CMT_ERR_GEN = STATUS_START(CMT),  /* Miscellaneous error */
   E_CMT_ERR_SETUP,                    /* Error performing setup */
   E_CMT_ERR_HEARTBEATS,               /* Heartbeats lost, assumed safe-state */
   E_CMT_ERR_CIL_RX,                   /* Unexpected CIL message received */
   E_CMT_ERR_SDB_SUBMIT,               /* Error performing SDB submission */
   E_CMT_FILE_ERR,                     /* File system error. */
   E_CMT_BADPARAM,                     /* Bad parameter read. */
   E_CMT_NO_OSINFO,                    /* Unable to retrieve OS info. */
   E_CMT_NO_NTPINFO                    /* Unable to retrieve NTP info. */  
} eCmtStatus_t;


/* CMT messages - services offered by the CMT */
typedef enum eCmtService_e
{
   E_CMT_HEARTBEAT = E_MCP_HEARTBEAT,  /* Heartbeat command */
   E_CMT_SHUTDOWN  = E_MCP_SHUTDOWN,   /* Shutdown command */
   E_CMT_SAFESTATE = E_MCP_SAFESTATE,  /* Safestate command */
   E_CMT_ACTIVATE  = E_MCP_ACTIVATE    /* Activate command */
} eCmtService_t;


/* Global data for submission to Status Database (SDB) */
typedef enum eCmtDataId_e
{
   D_CMT_DATAID_BOL = 0,        /* Begining of data list */

   D_CMT_PROC_STATE  = D_MCP_PROC_STATE,        /* Process state datum   */
   D_CMT_AUTH_STATE  = D_MCP_AUTH_STATE,        /* Authorisation state   */
   D_CMT_SYS_REQUEST = D_MCP_SYS_REQUEST,       /* System request to MCP */
   D_CMT_APP_VERSION = D_MCP_APP_VERSION,       /* Application version number*/

   /* System data retrieved from PC and submitted to SDB */ 
   D_CMT_CPU_USED_PERCENT = D_MCP_FIRST_USER_DATUM, /* CPU Usage percentage */

   /* The disk usage entries must be sequential */
   D_CMT_DISK1_FREE_PERCENT,    /* Disk1 usage as a percentage */
   D_CMT_DISK2_FREE_PERCENT,    /* Disk2 usage as a percentage */
   D_CMT_DISK3_FREE_PERCENT,    /* Disk3 usage as a percentage */
   D_CMT_DISK4_FREE_PERCENT,    /* Disk4 usage as a percentage */

   D_CMT_MEMORY_FREE_PERCENT,   /* Memory usage as a percentage */

   D_CMT_TEMPERATURE1,          /* Temperature sensor 1 in mC */ 
   D_CMT_TEMPERATURE2,          /* Temperature sensor 2 in mC  */ 
   D_CMT_TEMPERATURE3,          /* Temperature sensor 3 in mC */ 

   D_CMT_FAN1_RPM,              /* Fan1 rotation rpm */
   D_CMT_FAN2_RPM,              /* Fan2 rotation rpm */
   D_CMT_FAN3_RPM,              /* Fan3 rotation rpm */

   D_CMT_VOLT_CORE,             /* CPU core voltage */
   D_CMT_VOLT_REF,              /* CPU reference voltage */
   D_CMT_VOLT_IO,               /* CPU IO voltage */
   D_CMT_VOLT_POS_5V,           /* CPU Plus 5V supply */
   D_CMT_VOLT_POS_12V,          /* CPU Plus 12V supply */
   D_CMT_VOLT_NEG_5V,           /* CPU Minus 5V supply */
   D_CMT_VOLT_NEG_12V,          /* CPU Minus 12V supply */

   D_CMT_NTP_LOCKED,            /* NTP locked flag */
      
   D_CMT_DATAID_EOL             /* End of list marker */
} eCmtDataId_t;


#endif

/*******************************************************************************
** End of File Name: Cmt.h
*******************************************************************************/
