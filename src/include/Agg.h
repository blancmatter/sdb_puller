/********************************************************************************
** Module Name:
**    Agg.h
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
**    $Id: Agg.h,v 0.26 2008/01/04 09:55:57 sxf Exp $
**
** History:
**    $Log: Agg.h,v $
**    Revision 0.26  2008/01/04 09:55:57  sxf
**    Remove frame skip to make the Agi compatible with QNX4 Agg.
**
**    Revision 0.25  2007/10/30 10:52:14  mjf
**    Tweak to local path for non-QNX.
**
**    Revision 0.24  2007/10/26 11:33:29  mjf
**    Addition of file/directory alternatives for non-QNX.
**
**    Revision 0.23  2006/11/28 16:06:53  sxf
**    Added frame skip back in.
**
**    Revision 0.22  2006/09/27 08:43:54  sxf
**    Modified for QNX6.3.
**
**    Revision 0.20  2005/10/04 14:40:42  sxf
**    Brought in line with QNX4 baseline AGG_0_41.
**
**    Revision 0.19  2004/04/16 09:20:26  mjf
**    Tidying of porting to QNX v6.
**
**    Revision 0.18  2004/04/16 07:30:02  mjf
**    Checking in of QNX v6 porting work done by MAN.
**
**    Revision 0.17  2004/01/16 16:13:59  man
**    Removed frameskip and set minimum integration time
**    to 200ms.
**
**    Revision 0.16  2003/11/24 14:40:03  man
**    Set the minimum integration time to zero.
**
**    Revision 0.15  2003/09/30 13:53:38  man
**    Included additional message from Agg to indicate
**    where the FITS file has been saved.
**
**    Revision 0.14  2003/09/29 08:08:36  man
**    Included some additional messaging between the Agg and Agi
**    to allow calibration parameters to be retrieved and set.
**
**    Revision 0.13  2002/12/10 09:36:34  man
**    Included simulation state to indicate when
**    we're using simulation data.
**
**    Revision 0.12  2002/11/05 13:11:50  man
**    Included macros to report the progress of the calibration
**    file acquisition to the Agi.
**
**    Revision 0.11  2002/10/30 16:25:03  man
**    Included additional activity states for Agi
**    for when calibration images are being obtained.
**    Increased maximum integration time to 600 seconds.
**
**    Revision 0.10  2002/10/10 09:23:47  man
**    Included additional Agi activity states and changed
**    default log path to "/opt/ttl/data/".
**
**    Revision 0.9  2002/05/17 14:50:52  man
**    Included an additional activity to show when the
**    autoguider is calculating threshold levels.
**
**    Revision 0.8  2002/05/16 09:24:26  man
**    Included additional activity states for the Agi to
**    show when the Agg is performing bias, dark and flat
**    corrections.
**
**    Revision 0.7  2002/03/22 17:19:24  man
**    Included a command function to save the current full frame image to
**    disk as a FITS file.
**
**    Revision 0.6  2002/01/24 07:53:13  sxf
**    Create new enum for choice of fast readout - disk or movie.
**
**    Revision 0.5  2001/11/09 11:14:02  sxf
**    First stable version.
**
**    Revision 0.4  2001/10/17 15:41:00  sxf
**    Fisrt version with basic functionality.
**
**    Revision 0.3  2001/10/10 13:17:07  sxf
**    First version for testing with Ags.
**
**    Revision 0.2  2001/10/08 15:32:25  sxf
**    Revised to make code more in line with TTL coding standards.
**
**    Revision 0.1  2001/10/02 10:13:58  sxf
**    Initial version.
**
**
*******************************************************************************/


#ifndef AGG_H_DEFINED
#define AGG_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/

#include "TtlSystem.h"
#include "Wfl.h"
#include "Mcp.h"
#include "Sdb.h"


/*
** Local include files
*/


/*
** Macro definitions
*/

#ifdef M_AGG_MAIN_C
#define E_AGG_EXTERN
#else
#define E_AGG_EXTERN extern
#endif

/* Status monitor command codes */
#define E_AGG_SMG_PRINT_MESSAGE             1
#define E_AGG_SMG_NEW_GUIDE_IMAGE           2
#define E_AGG_SMG_AUTO_PARAMETER_CHANGE     3
#define E_AGG_SMG_FAST_READOUT_ERROR        4
#define E_AGG_SMG_FAST_READOUT_NEWFRAME     5
#define E_AGG_SMG_CHANGE_OF_STATE           6
#define E_AGG_SMG_NEW_FULL_IMAGE            7
#define E_AGG_SMG_NEW_STARLOG               8
#define E_AGG_SMG_DOWNLOAD_PROGRESS         9
#define E_AGG_SMG_SUBSYSTEM_STATE_CHANGE    10
#define E_AGG_SMG_BIAS_PROGRESS             11
#define E_AGG_SMG_DARK_PROGRESS             12
#define E_AGG_SMG_FLAT_PROGRESS             13
#define E_AGG_SMG_CALIB_PARAM_CHANGE        14
#define E_AGG_SMG_FITS_FILENAME             15


/* Seriousness rating of status messages */
#define E_AGG_SMG_NUM_SERIOUSNESS       4
#define E_AGG_SMG_INFORMATION           0
#define E_AGG_SMG_WARNING                1
#define E_AGG_SMG_ERROR                 2
#define E_AGG_SMG_CRITICAL              3

/* Persistence of status messages */
#define E_AGG_SMG_PERSISTANT            0
#define E_AGG_SMG_OVERWRITEABLE         1

/* Constants for the auto parameter change message */
#define E_AGG_SMG_AUTO_WINDOW           1
#define E_AGG_SMG_AUTO_ITIME            2
#define E_AGG_SMG_AUTO_SKIP             4

/* Constants for subsystem state change message */
#define E_AGG_SMG_CHANNEL_SESSION       1
#define E_AGG_SMG_CHANNEL_CAMERA        2
#define E_AGG_SMG_CHANNEL_AGS           3

/* Individual subsystem states */
#define E_AGG_SMG_SS_SESSIONON          1
#define E_AGG_SMG_SS_SESSIONOFF         2
#define E_AGG_SMG_SS_CAMERAEXPOSE       3
#define E_AGG_SMG_SS_CAMERADOWNLOAD     4
#define E_AGG_SMG_SS_CAMERAIDLE         5
#define E_AGG_SMG_SS_CAMERADEAD         6
#define E_AGG_SMG_SS_AGSREADY           7
#define E_AGG_SMG_SS_AGSACQUIRE         8
#define E_AGG_SMG_SS_AGSSTARLOG         9
#define E_AGG_SMG_SS_AGSGUIDING         10
#define E_AGG_SMG_SS_AGSERROR           11
#define E_AGG_SMG_SS_AGSDARK            12
#define E_AGG_SMG_SS_AGSBIAS            13
#define E_AGG_SMG_SS_AGSFLAT            14
#define E_AGG_SMG_SS_AGSTHRESH          15
#define E_AGG_SMG_SS_AGSLOAD            16 
#define E_AGG_SMG_SS_AGSFITS            17
#define E_AGG_SMG_SS_AGSCALIMG          18
#define E_AGG_SMG_SS_AGSACQBIAS         19
#define E_AGG_SMG_SS_AGSACQDARK         20
#define E_AGG_SMG_SS_AGSACQFLAT         21
#define E_AGG_SMG_SS_CAMERASIM          22


/* Limits for parameters passed to the AGG, as defined in the ICD */
#define E_AGG_ICD_MIN_MAGNITUDE       0
#define E_AGG_ICD_MAX_MAGNITUDE       65535.0
#define E_AGG_ICD_MIN_RANK            1
#define E_AGG_ICD_MAX_RANK            10
#define E_AGG_ICD_MIN_STARMAG         0.0
#define E_AGG_ICD_MAX_STARMAG         20.0
#define E_AGG_ICD_MIN_PIXEL           0.0
#define E_AGG_ICD_MAX_PIXEL           1100.0
#define E_AGG_ICD_MIN_ITIME           200
#define E_AGG_ICD_MAX_ITIME           600000
#define E_AGG_ICD_MIN_SKIP            0
#define E_AGG_ICD_MAX_SKIP            2100000000
#define E_AGG_ICD_MIN_PERMINUTE       2
#define E_AGG_ICD_MAX_PERMINUTE       300
#define E_AGG_ICD_MIN_FRAMEAVERAGE    1
#define E_AGG_ICD_MAX_FRAMEAVERAGE    100
#define E_AGG_ICD_MIN_CALIB           -100.0
#define E_AGG_ICD_MAX_CALIB           100.0

#define E_AGG_CMD_BODY_LEN  256

#define E_AGG_FANWORKTRESH  10  /* Threshold for fan to be working. */

#ifdef E_WFL_OS_QNX4

#define E_AGG_LOGPATH       "//1/opt/ttl/data"  /* Default path for Agg to log to. */
#define E_AGG_GUIDEDUMPNAME "//1/opt/ttl/data/AggGuideImage" /* Default guide dump filename. */
#define E_AGG_LOCAL_LOGPATH "//3/localflash/etc/" /* Default local log path */

#elif defined E_WFL_OS_QNX6

#define E_AGG_LOGPATH       "/ttl/sw/data"  /* Default path for Agg to log to. */
#define E_AGG_GUIDEDUMPNAME "/ttl/sw/data/AggGuideImage" /* Default guide dump filename. */
#define E_AGG_LOCAL_LOGPATH "/net/acc/dom/etc/" /* Default local log path */

#else

#define E_AGG_LOGPATH       "/ttl/sw/data"  /* Default path for Agg to log to. */
#define E_AGG_GUIDEDUMPNAME "/ttl/sw/data/AggGuideImage" /* Default guide dump filename. */
#define E_AGG_LOCAL_LOGPATH "/ttl/sw/data/" /* Default local log path */

#endif

/*
** Type definitions
*/

typedef struct eAggCmdBody_s
{
   Int32_t   Command;
   Int32_t   Status;
   char      Parameters[ E_AGG_CMD_BODY_LEN ];
} eAggCmdBody_t;


/* Enumerated list of status and error values */

typedef enum eAggStatus_e
{
   E_AGG_GEN_ERR = STATUS_START(AGG),  /* Miscellaneous error. */
   E_AGG_LOOP_STOPPING,      /* Guide loop stopping. */
   E_AGG_LOOP_RUNNING,       /* Guide loop running. */
   E_AGG_CMD_NOT_PERMITTED,  /* Command not permitted. */
   E_AGG_CMD_BAD_FORMAT,     /* Command has bad format. */
   E_AGG_MEMERR,             /* Error allocating memory. */
   E_AGG_SEM_ERR,            /* Semaphore error. */
   E_AGG_SHMEM_ERR,          /* Shared memory error. */
   E_AGG_HW_ERR,             /* Hardware error. */
   E_AGG_CMD_UNKNOWN,        /* Unknown command. */
   E_AGG_CMD_CONTEXT_1,      /* Last starlog entry. */
   E_AGG_CAMOK,              /* Camera OK. */
   E_AGG_PROXERR,            /* Error creating proxy. */
   E_AGG_FORKERR,            /* Error forking process. */
   E_AGG_BADPARAM,           /* Parameter in error. */
   E_AGG_IMERR,              /* Image error. */
   E_AGG_NOSTAR,             /* No suitable star found. */
   E_AGG_COMMSERR,           /* Comms error. */
   E_AGG_DMERR,              /* Error in defect map. */
   E_AGG_FILERR,             /* File error. */  
   E_AGG_INTCANCEL,          /* Integration cancelled by user. */  
   E_AGG_CMDREPLIED,         /* Reply already sent in response to command. */      
   E_AGG_STATUS_EOL          /* End of status list marker (do not use) */
} eAggStatus_t;

/* Services offered by the AGG. */
typedef enum eAggService_e
{

   E_AGG_CMD       = SERVICE_START( AGG ) /* Issue autoguider command. */

} eAggService_t;

/* Enumerated list of commands that can be sent to the AGG. */

typedef enum eAggCmd_e
{
   E_AGG_CMD_BOL,            /* Bottom of list. */
   E_AGG_CMD_STARTSESSION,   /* Start session, turn cooling on. */
   E_AGG_CMD_ENDSESSION,     /* End session, turn cooling off. */
   E_AGG_CMD_AUTOGUIDE,      /* Autoguide start/stop. */
   E_AGG_CMD_SETFIELD,       /* Set field window. */
   E_AGG_CMD_REGFIFILE,      /* Register field image file. */
   E_AGG_CMD_RESETSLOG,      /* Reset starlog pointer. */
   E_AGG_CMD_NEXTSLOG,       /* Get next starlog entry. */
   E_AGG_CMD_REGGIFILE,      /* Register guide image file. */
   E_AGG_CMD_FULLIMAGE,      /* Get full image. */
   E_AGG_CMD_SETITIME,       /* Set integration time. */
   E_AGG_CMD_GUILOGIN,       /* Autoguider GUI login. */
   E_AGG_CMD_ACKIMAGE,       /* Acknowledge new image. */
   E_AGG_CMD_SETGWINDOW,     /* Set guide window. */
   E_AGG_CMD_FIELD,          /* Field - ie autoguide, no guide pkts. */
   E_AGG_CMD_LOOPON,         /* Loop on. */
   E_AGG_CMD_SETTEMP,        /* Set target chip temperature. */
   E_AGG_CMD_GETTEMP,        /* Get current chip temperature. */
   E_AGG_CMD_SETLOGPATH,     /* Set logging path. */
   /* E_AGG_CMD_SETFSKIP,        Set frame skip. */
   E_AGG_CMD_ACQDFRAME,      /* Acquire dark frame. */
   E_AGG_CMD_CENTROID,       /* Centroid. */
   E_AGG_CMD_ITTRACK,        /* Integration time tracking. */
   E_AGG_CMD_SETLOOPAV,      /* Set loop average. */ 
   E_AGG_CMD_ACQFF,          /* Acquire flat field. */
   E_AGG_CMD_FREADOUT,       /* Fast readout. */
   E_AGG_CMD_INFO,           /* Get information about guide loop. */
   E_AGG_CMD_CONFIG,         /* Configure guide loop parameters. */
   E_AGG_CMD_GWTRACK,        /* Guide window tracking. */
   E_AGG_CMD_SVFITS,         /* Save full frame to fits */
   E_AGG_CMD_BFRAME,         /* Acquire bias frame */
   E_AGG_CMD_GETCALIB,       /* Get calibration filenames */
   E_AGG_CMD_SETCALIB,       /* Command to set calibration parameters */
   E_AGG_CMD_STOP_PROGRAM,   /* Stop the Agg from running. */
   E_AGG_CMD_EOL             /* End of list. */
} eAggCmd_t;


/* Enumerated list of keywords used to build up commands to the AGG. */

typedef enum eAggCmdKeyword_e
{
   E_AGG_ON,                 /* Turn on (ie guiding, loop). */ 
   E_AGG_OFF,                /* Turn off (ie guiding, loop). */
   E_AGG_BRIGHTEST,          /* Guide on brightest source. */
   E_AGG_RANGE,              /* Guide on source within magnitude range. */
   E_AGG_RANK,               /* Guide on source of given rank. */
   E_AGG_PIXEL,              /* Guide on source closest to supplied pixel. */
   E_AGG_CONF_EXPTIME,       /* Configure exposure time. */
   E_AGG_CONF_FRATE,         /* Configure frame rate. */
   E_AGG_CONF_FAVERAGE,      /* Configure frame average. */ 
   E_AGG_CONF_CALIB          /* Configure calibration (grey level->rank). */
} eAggCmdKeyword_t;


typedef enum eAggFastReadCmd_e
{
   E_AGG_DISK,               /* Fast readout to disk. */
   E_AGG_MOVIE               /* Fast readout "movie". */
} eAggFastReadCmd_t;

/* 
** Enumerated list of Agg states.
*/
typedef enum eAggState_e
{
   E_AGG_STATE_UNKNOWN          = 0x0,
   E_AGG_STATE_OK               = 0x10,
   E_AGG_STATE_OFF,
   E_AGG_STATE_STANDBY,
   E_AGG_STATE_IDLE,
   E_AGG_STATE_WORKING,
   E_AGG_STATE_INIT,
   E_AGG_STATE_FAILED,
   E_AGG_STATE_INTWORK,
   E_AGG_STATE_ERROR            = 0x100,
   E_AGG_STATE_NONRECERR,
   E_AGG_STATE_LOOP_RUNNING     = 0x1000,
   E_AGG_STATE_GUIDEONBRIGHT    = E_AGG_STATE_LOOP_RUNNING + E_AGG_BRIGHTEST,
   E_AGG_STATE_GUIDEONRANGE     = E_AGG_STATE_LOOP_RUNNING + E_AGG_RANGE,
   E_AGG_STATE_GUIDEONRANK      = E_AGG_STATE_LOOP_RUNNING + E_AGG_RANK,
   E_AGG_STATE_GUIDEONPIXEL     = E_AGG_STATE_LOOP_RUNNING + E_AGG_PIXEL,
   E_AGG_STATE_INTON,
   E_AGG_TSTATE_AT_TEMP         = 0x10000,
   E_AGG_TSTATE_ABOVE_TEMP,
   E_AGG_TSTATE_BELOW_TEMP,
   E_AGG_TSTATE_ERROR,
   E_AGG_STATE_EOL
} eAggState_t;   

/* 
** Enumerated list of starlog entry states. 
*/
typedef enum eAggSeState_e
{
  E_AGG_SE_BOL,              /* Bottom of list. Do not use. */
  E_AGG_SE_OK,                   
  E_AGG_SE_INCOMPLETE,           
  E_AGG_SE_UNDERSIZED,           
  E_AGG_SE_OVERSIZED,            
  E_AGG_SE_TOODIM,               
  E_AGG_SE_TOOBRIGHT,             
  E_AGG_SE_UNDERDIFFUSE,         
  E_AGG_SE_OVERDIFFUSE,          
  E_AGG_SE_POORSHAPE,            
  E_AGG_SE_PROXIMITY,            
  E_AGG_SE_TOONEAREDGE,          
  E_AGG_SE_IN_KNOWN_DEFECT,   
  E_AGG_SE_COSMIC_RAY,
  E_AGG_SE_IS_GUIDE_STAR,
  E_AGG_SE_EOL               /* End of list. Do not use. */
} eAggSeState_t;

#endif

/*******************************************************************************
** End of File Name: Agg.h
*******************************************************************************/
