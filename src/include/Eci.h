/*****************************************************************************
** Header File Name:
**     Eci.h
**
** Purpose:
**     Contains the external definitions and variables of the ECI.     
**
** Description:
**      
**     This file contains the enumerated types used by ECI instances
**     and their agents. These include the agent commands
**     and the instance mode/priority table.
**
** Authors:
**     Lorna J. Slater (ljs)
**
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
**
**
** Version:
**     $Id: Eci.h,v 0.27 2004/10/12 12:20:14 mjf Exp $
**    
** History:   
**     $Log: Eci.h,v $
**     Revision 0.27  2004/10/12 12:20:14  mjf
**     Removal of incorrect include paths.
**
**     Revision 0.26  2004/04/07 09:19:21  sxf
**     Initial Eci port to QNX6.
**
**     Revision 0.25  2003/04/08 10:59:03  dws
**     Addition of Error states for Std screen and generic errors for the ECI.
**
**     Revision 0.24  2003/02/28 10:05:09  dws
**     Added status code for proxy setup failure.
**
**     Revision 0.23  2003/02/12 10:07:26  dws
**     Added generic fail status for std related screens.
**
**     Revision 0.22  2003/02/04 11:55:00  dws
**     Added status codes for std.
**
**     Revision 0.21  2002/10/24 13:19:32  dws
**     Changes to bring inline with HTI_0_16. new Itr
**     window to allow all 3 axes to be homed.
**
**     Revision 0.20  2002/02/28 14:16:21  djm
**     Changed the name of the default log file.
**
**     Revision 0.19  2002/01/02 11:09:46  djm
**     Corrected problem with the first user datum not being
**     assigned the correct value.
**
**     Revision 0.18  2001/10/29 11:19:48  djm
**     Added DatumID for "user name". This has not been implemented
**     yet, and the DatumID is merely a placehold at this stage.
**
**     Revision 0.17  2001/10/11 09:27:12  djm
**     Added new error code.
**
**     Revision 0.16  2001/09/21 16:10:58  djm
**     Changed to cope with renamed Oid/Agent header files.
**
**     Revision 0.15  2001/09/21 15:07:24  djm
**     Added new error codes.
**
**     Revision 0.14  2001/08/02 14:49:52  djm
**     Added new error code. Also improved indentation.
**
**     Revision 0.13  2001/07/25 08:15:17  ljs
**     Added new error states applicable to user authentication.
**
**     Revision 0.12  2001/07/10 16:50:05  ljs
**     Added 'NULL_CONV_FUNC' error.
**
**     Revision 0.11  2001/05/18 14:00:24  ljs
**     Modified error code name to do with not being able to
**     find the required graphic.
**
**     Revision 0.10  2001/05/17 13:27:32  ljs
**     Added new errors HIDDEN widget and NULL LABEL.
**
**     Revision 0.9  2001/02/26 15:59:22  ljs
**     Added path to log file.
**
**     Revision 0.8  2001/02/23 15:57:19  ljs
**     Added Acn test files.
**
**     Revision 0.7  2000/12/20 10:14:56  ljs
**     Added new error code.
**
**     Revision 0.6  2000/12/13 20:52:42  ljs
**     Added D_MCP_APP_VERSION.
**
**     Revision 0.5  2000/12/05 16:16:30  ljs
**     Trivial Changes.
**
**     Revision 0.4  2000/12/04 09:04:34  ljs
**     Added new error code.
**
**     Revision 0.3  2000/11/29 15:25:39  ljs
**     Added E_ECI_CIL constants defining the beginning of the ECI instance
**     and the ERT instance cil IDs. Not currently being used.
**
**     Revision 0.2  2000/11/27 18:38:32  ljs
**     Added two new error codes.
**
**     Revision 0.1  2000/11/13 16:51:07  ljs
**     Initial revision after file re-naming and re-organisation.
**
**                
*****************************************************************************/



#ifndef  ECI_H_DEFINED
#define  ECI_H_DEFINED

/* TTL headers. */
#include "Mcp.h"
#include "Asi.h"    /* Agent Server Interface */
#include "Cil.h"
#include "Wfl.h"
#ifdef E_WFL_OS_QNX4
#include "Hti.h"
#define E_ECI_CIL_MAPNAME    "/opt/ttl/etc/Cil.map"
#endif

#ifdef E_WFL_OS_QNX6
#include "Hti.h"
#define E_ECI_CIL_MAPNAME    "/ttl/sw/etc/Cil.map"
#endif

/* Constants. */


#define E_ECI_QNX_MSG_SIZE   8    /* Size of messages between ECI and ERT. */ 
#define E_ECI_CIL_MSG_SIZE   24   /* Expected maximum size of system messages. */ 
#define E_ECI_CIL_HBEAT_TO   1500 /* Time out, after this if no heartbeat. */  

#define E_ECI_CIL_EI_BASE    I_ECI_CIL_EI0    /* Base Cil ID of ECI instances. */  
#define E_ECI_CIL_ER_BASE    I_ECI_CIL_ER0    /* Base Cil ID of ERT instances. */  
#define E_ECI_CIL_HB_BASE    E_ECI_CIL_ER_BASE 
                                /* Base Cil ID of the process that responds */  
                                /* to heatbeats. */  
#define E_ECI_QTY_INSTANCES  (I_ECI_CIL_EI0 - I_ECI_CIL_ER0)
                             /* Maximum number of instances of the ECI. */  
                             /* The Cil Ids must be declared sequentially. */
  
#ifdef E_WFL_OS_QNX4
#define E_ECI_LOG_FILE  "//1/opt/ttl/data/Eci.log"
#endif

#ifdef E_WFL_OS_QNX6
#define E_ECI_LOG_FILE  "/ttl/sw/data/Eci.log"
#endif

                             /* Log location for Eci and Ert. */


/* 
** Engineering Reporting Task (ERT) commands, called by parent ECI process
** or other ECI instances.
** Commands are passed via QNX Send and Receive messages between the
** processes. Must be 8-bit.
*/

typedef enum
{
   E_ECI_BOL = 0,        /* Beginning of list, not a command. */

   E_ECI_PARENT,         /* Identifies the Parent Eci of an Ert */ 
   E_ECI_WATCHDOG_RESET, /* resets the watchdog timer for the EI process */
   E_ECI_RELEASE_BLOCK,  /* Stops the Ert from being blocked if ECI is late */
   E_ECI_SHUTDOWN,   /* Tells the Ert that the ECI is exiting normally */
   E_ECI_GROUP_KILL     /* Terminates process group, dire use only */


} eEciERTCommands_t;


/*
** ECI errors
*/

typedef enum eEciStatus_e{

   E_ECI_GEN_ERR = STATUS_START(ECI),              /* General Purpose error */
   E_ECI_MALLOC_FAIL,
   E_ECI_CILSETUP_FAIL,
   E_ECI_SEND_FAIL,            /* Cil Send Command Failed. */
   E_ECI_RECEIVE_FAIL,         /* Cil Receive Command Failed. */
   E_ECI_BAD_REPLY,            /* Reply was not of the expected size or content. */
   E_ECI_DEAD_ERT,             /* From ECI when the ERT has died. */
   E_ECI_SPAWN_FAIL,           /* The child processes'(ERT) spawn has failed. */
   E_ECI_NO_POLL,              /* From the ERT if the ECI misses a poll. */
   E_ECI_COMMAND_FAIL,         /* A mechanism command failed. */
   E_ECI_INVALID_ENTRY,        /* A user has entered an outof range value. */
   E_ECI_PHAB_FUNCTION_FAILED, /* A PhAB function call has failed. */
   E_ECI_NULL_WIDGET_PTR,      /* A widget pointer has been found to be NULL. */
   E_ECI_NULL_WIDGET_ARRAY,    /* A widget array has been found to be NULL. */
   E_ECI_NULL_CONV_FUNC,       /* A conversion function is missing. */
   E_ECI_SDB_INVALID_UNITS,    /* The SDB has given Invalid units for a datum. */
   E_ECI_UNABLE_TO_OPEN_FILE,  /* A file open failed. */
   E_ECI_UNSUPPORTED_DATUM,    /* A file open failed. */
   E_ECI_MISSING_GRAPHIC,      /* Couldn't find a required graphical image. */
   E_ECI_USER_LOC_UNKNOWN,     /* User's location cannot be determined. */
   E_ECI_USER_UNKNOWN,         /* User's identity cannot be determined. */
   E_ECI_NO_GFX_ENVIRONMENT,   /* Couldn't detect any graphics environment. */
   E_ECI_UNKNOWN_SUBSYS,       /* Subsystem not known */
   E_ECI_NO_WDIGET,            /* No widget detected/recognised */
   E_ECI_CIL_LOOKUP_FAIL,      /* Cil name lookup fail */
   E_ECI_STD_TEXT_CONVERT_FAIL, /* Fail to convert cil and datum pair to text */
   E_ECI_STD_CFG_FILE_SAVE_FAIL, /* Failed to save config file */
   E_ECI_STD_TIME_INVALID,     /* The entered times are invalid */
   E_ECI_STD_FAIL,             /* The std failed to run */
   E_ECI_PROXY_SETUP_FAIL,     /* Failed to setup a proxy timer */
   E_ECI_WMS_ENGINEERING_FAIL,
   E_ECI_FTP_INPROGRESS,
   E_ECI_FTP_INVALID_CFG,
   E_ECI_FTP_WRITE_FAILED,
   E_ECI_FTP_FAILED,
   E_ECI_EOERR_LIST,           /* End error list marker (DON'T USE FOR STATUS) */
   E_ECI_STATUS_MAX_VALUE = INT_MAX    /* Req'd to force size to 4 bytes */
} eEciStatus_t;

/* Global data for submission to Status Database (SDB) */
typedef enum eEciDataId_e
{

   D_ECI_DATAID_BOL = 0,    /* Begining of data list. Do not use as an ID */

   D_ECI_PROC_STATE         /* Task state(-machine) variable */
      = D_MCP_PROC_STATE,
   D_ECI_AUTH_STATE         /* Requested/granted authorisation state */
      = D_MCP_AUTH_STATE,
   D_ECI_SYS_REQUEST        /* To shutdown, startup and go to safe state */
      = D_MCP_SYS_REQUEST,
   D_ECI_VERSION            /* For MCP to submit Version num to SDB */
      = D_MCP_APP_VERSION,

   D_ECI_USER_NAME          /* SSE encoded user-name of account running ECI */
      = D_MCP_FIRST_USER_DATUM,

   D_ECI_DATAID_EOL       /* End of list marker - do not use as an index */

}  eEciDataId_t;


/* Union of all possible buffers (payloads) that can accompany ACN commands */

typedef struct
{
      Int32_t    Data;
      eTtlTime_t TimeStamp;
}  eEciAcnPos_t;



typedef union 
{
      Bool_t    Diagnose;
      eEciAcnPos_t     Position; 
      Int32_t   Home;
      Int32_t   Unhome;
      Int32_t   SelMotnAlg;
      Int32_t   ResetHead;
      Bool_t    Simulate;
      Bool_t    WarmBoot;
      Int32_t   Recover;
      Bool_t    KeepUnhomed;
      ObjVal_t  ObjVal;       /* At the moment defined in AgentServer.h */
      AttrVal_t AttrVal;      /* At the moment defined in AgentServer.h */  

} eEciAcnPayload_t;


#endif  /* ECI_H_DEFINED */

/***************************************************************************** 
** End of file: Eci.h 
*****************************************************************************/