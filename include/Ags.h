/********************************************************************************
** Module Name:
**    Ags.h
**
** Purpose:
**    Public header file for the Autoguider Service Task.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    Autoguider Control System Detailed Design Document v0.01.
**
** Author(s):
**    Stephen Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: Ags.h,v 0.12 2004/04/16 09:31:00 mjf Exp $
**
** History:
**    $Log: Ags.h,v $
**    Revision 0.12  2004/04/16 09:31:00  mjf
**    Checking in of QNX v6 porting done by MAN.
**
**    Revision 0.11  2003/05/01 10:52:24  man
**    Pass the frame mean, frame rms and case temperature
**    from the agg to ags.
**
**    Revision 0.10  2003/01/23 09:35:14  man
**    Included two Oid's for CCD case temperature
**    and cooler power level.
**
**    Revision 0.9  2002/05/07 09:37:04  sxf
**    Created Status codes for returning to TCS.
**
**    Revision 0.8  2002/03/05 16:39:42  sxf
**    Add comments.
**
**    Revision 0.7  2002/02/05 14:52:56  sxf
**    Added D_AGS_AGTEMP
**
**    Revision 0.6  2001/12/04 14:30:26  sxf
**    Integrate with rest of telescope.
**
**    Revision 0.5  2001/11/12 16:22:10  sxf
**    Remove D_AGS_THIS_TELESCOPE datum.
**
**    Revision 0.4  2001/11/05 12:08:47  mkb
**    Added D_AGS_AGSTATE - state of autoguider subsystem.
**
**    Revision 0.3  2001/10/17 15:39:06  sxf
**    First version with basic functionality.
**
**    Revision 0.2  2001/09/25 14:46:10  sxf
**    Initial version with basic functionality.
**
**    Revision 0.1  2001/09/18 14:59:02  sxf
**    Initial version.
**
**
*******************************************************************************/


#ifndef AGS_H_DEFINED
#define AGS_H_DEFINED


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

#ifdef M_AGS_MAIN_C
#define E_AGS_EXTERN
#else
#define E_AGS_EXTERN extern
#endif


/*
** Type definitions
*/


/* Enumerated list of status and error values */

typedef struct eAgsCmdBody_s
{
   Int32_t  Command;
   Int32_t  Status;
   Int32_t  Param1;
   Int32_t  Param2;
} eAgsCmdBody_t;

typedef enum eAgsStatus_e
{
   E_AGS_GEN_ERR = STATUS_START(AGS),/* Miscellaneous error. */
   E_AGS_UNEXPECTED_MSG,             /* Unexpected message class. */
   E_AGS_UNKNOWN_SOURCE,             /* Unexpected message source. */
   E_AGS_NO_HEARTBEATS,              /* Heartbeats have been missed. */
   E_AGS_INVREPLY,                   /* Invalid reply to an AGS command. */
   E_AGS_CMDQ_EMPTY,                 /* Command queue empty. */
   E_AGS_CONFIG_FILE_ERROR,          /* Error reading config file. */
   E_AGS_INVALID_DATUM,              /* Datum not in range of eAgsDataId_t. */
   E_AGS_INV_CMD,                    /* Invalid command. */
   E_AGS_BAD_CMD,                    /* Badly formed command. */
   E_AGS_CMDQ_ERR,                   /* Error with command queue to Agg. */
   E_AGS_INV_STATE,                  /* Invalid state requested. */
   E_AGS_UNKNOWN_OID,                /* Request to set unknown OID. */ 
   E_AGS_CMDQ_FULL,                  /* Command queue full. */
   E_AGS_CMD_TIMEOUT,                /* Command to Agg timed out. */
   E_AGS_CMD_NOT_PERMITTED,          /* Command to Agg not permitted. */
   E_AGS_LOOP_STOPPING,              /* Command rejected - already stopping. */
   E_AGS_LOOP_RUNNING,               /* Command rejected - already running. */
   E_AGS_LOOP_ERROR,                 /* Error starting guide loop. */
   E_AGS_BAD_FORMAT,                 /* Badly formatted command sent to Agg. */
   E_AGS_HW_ERR,                     /* Error reported by camera hardware. */
   E_AGS_STATUS_EOL                  /* End of status list marker (do not use) */
} eAgsStatus_t;

/* Services offered by the AGS. */
typedef enum eAgsService_e
{

   E_AGS_CMD       = SERVICE_START( AGS ) /* Issue autoguider command. */

} eAgsService_t;

/* Enumerated list of commands that can be sent to the AGS (from the TCS). */
typedef enum eAgsCmd_e
{
   E_AGS_CMD_BOL,                     /* Bottom of list. */
   E_AGS_AUTOGUIDE_ON_BRIGHTEST,      /* Guide on brightest object. */
   E_AGS_AUTOGUIDE_ON_RANK,           /* Guide on n'th brightest object. */
   E_AGS_AUTOGUIDE_ON_RANGE,          /* Guide on object in range of star mags. */
   E_AGS_AUTOGUIDE_ON_PIXEL,          /* Guide on object nearest to pixel. */
   E_AGS_AUTOGUIDE_OFF,               /* Stop guide loop. */
   E_AGS_CONF_EXP_TIME,               /* Configure exposure (integration) time. */
   E_AGS_CONF_FRAME_RATE,             /* Configure frame rate. */
   E_AGS_CONF_FRAME_AVERAGE,          /* Configure frame average. */
   E_AGS_CONF_CALIB,                  /* Configure grey level->star mag calcs. */
   E_AGS_START_SESSION,               /* Start session (turn on cooling). */
   E_AGS_END_SESSION,                 /* End session (turn off cooling). */
   E_AGS_LOGGING,                     /* Turn logging on or off for Agg. */
   E_AGS_CMD_EOL                      /* End of list. */
} eAgsCmd_t;

/* Enumerated list of keywords that can be sent to the AGS (from the TCS). */
typedef enum eAgsKeyword_e
{
   E_AGS_KEYWORD_ON,
   E_AGS_KEYWORD_OFF
} eAgsKeyword_t;


/* Enumerated list of states reported to the SDB by the autoguider. */
typedef enum eAgsState_e
{
  E_AGS_OFF,                  /* Not ready to accept observing commands. */
  E_AGS_ON_BRIGHTEST,         /* Supplying guiding corrections on brightest */
                              /* non-saturated source. */
  E_AGS_ON_RANGE,             /* Supplying guiding corrections on brightest */
                              /* non-saturated object in supplied range. */ 
  E_AGS_ON_RANK,              /* Supplying guiding corrections on nth */
                              /* brightest non-saturated object. */
  E_AGS_ON_PIXEL,             /* Supplying guiding corrections on object */
                              /* closest to supplied pixel. */
  E_AGS_IDLE,                 /* Ready to accept observing commands. */
  E_AGS_WORKING,              /* Currently acquiring guide source. */
  E_AGS_INITIALISING,         /* Not ready for operational use. */
  E_AGS_FAILED,               /* Failed to find a guide source. */
  E_AGS_INTERACTIVE_WORKING,  /* Currently being used interactively but */
                              /* not supplying guiding data. */
  E_AGS_INTERACTIVE_ON,       /* Currently being used interactively and */
                              /* suppliying guiding data. */
  E_AGS_ERROR,                /* Recoverable error. */
  E_AGS_NON_RECOVERABLE_ERROR /* Non recoverable error. */
} eAgsState_t;   

/* Global data for submission to the Status Database (SDB) */
typedef enum eAgsDataId_e
{
   D_AGS_DATAID_BOL = 0,     /* Begining of data list */

   D_AGS_PROC_STATE          /* Process state datum */
      = D_MCP_PROC_STATE,
   D_AGS_AUTH_STATE          /* Requested/granted authorisation state */
      = D_MCP_AUTH_STATE,
   D_AGS_SYS_REQUEST         /* System requests made to MCP */
      = D_MCP_SYS_REQUEST,
   D_AGS_APP_VERSION         /* Application version number */
      = D_MCP_APP_VERSION,
   D_AGS_AGSTATE = D_MCP_FIRST_USER_DATUM,  /* State of autoguider subsystem. */
   D_AGS_WINDOW_TLX,         /* X pixel of top left guide win coord. */
   D_AGS_WINDOW_TLY,         /* Y pixel of top left guide win coord. */
   D_AGS_WINDOW_BRX,         /* X pixel of bottom right guide win coord. */
   D_AGS_WINDOW_BRY,         /* Y pixel of bottom right guide win coord. */
   D_AGS_INTTIME,            /* Int time for current/last guide loop. */
   D_AGS_FRAMESKIP,          /* Frame skip for current/last guide loop. */
   D_AGS_GUIDEMAG,           /* Star magnitude of guide star on loop start. */
   D_AGS_CENTROIDX,          /* X pixel of current/last centroid. */
   D_AGS_CENTROIDY,          /* Y pixel of current/last centroid. */
   D_AGS_FWHM,               /* Measured FWHM of current/last centroid. */
   D_AGS_PEAKPIXEL,          /* Star magnitude of current/last centroid. */
   D_AGS_AGTEMP,             /* Chip temperature. */
   D_AGS_AGCASETEMP,         /* Autoguider case temperature. */
   D_AGS_AGPERCPOW,          /* Autoguider cooler power. */
   D_AGS_AGFRAMEMEAN,        /* Autoguider frame mean */
   D_AGS_AGFRAMERMS,         /* Autoguider frame rms */
   D_AGS_DATAID_EOL          /* End of list.                                */
} eAgsDataId_t;


/* 
** Define the priorities of process states to be used
** in the state model for this process. States that are not listed
** here are not used in the state model. Lower priority states
** may not over ride higher priority states.
*/

#define E_AGS_NUM_POS_STATES 6


E_AGS_EXTERN ProcState_t eAgsStatePriority[]
#ifdef M_AGS_MAIN_C
={
    SYS_INIT_STATE,   /* Lowest Priority. */
    SYS_OKAY_STATE,
    SYS_WARN_STATE,
    SYS_SAFE_STATE,
    SYS_STANDBY_STATE,
    SYS_FAILED_STATE  /* Highest priority. */
}
#endif
;

#define E_AGS_UNKNOWN 0

#define E_AGS_SRC_ID  E_CIL_AGS
#define E_AGS_DEST_ID E_CIL_AGG

#endif

/*******************************************************************************
** End of File Name: Ags.h
*******************************************************************************/
