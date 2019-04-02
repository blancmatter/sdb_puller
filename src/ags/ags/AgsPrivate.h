/*******************************************************************************
** Module Name:
**    AgsPrivate.h
**
** Purpose:
**    Private header file for Autoguider Service Task..
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
**    $Id: AgsPrivate.h,v 0.38 2007/10/26 11:34:05 mjf Exp $
**
** History:
**    $Log: AgsPrivate.h,v $
**    Revision 0.38  2007/10/26 11:34:05  mjf
**    Addition of file/directory alternatives for non-QNX.
**
**    Revision 0.37  2007/07/19 09:33:01  mjf
**    AGS_1_06.
**
**    Revision 0.36  2006/10/11 09:59:33  sxf
**    Correct prototype.
**
**    Revision 0.35  2006/10/11 09:41:54  sxf
**    Brought in line with AGS_0_19.
**
**    Revision 0.34  2006/10/04 13:27:08  sxf
**    Baseline AGS_1_04.
**
**    Revision 0.33  2006/10/03 15:37:43  sxf
**    Baseline AGS_1_04.
**
**    Revision 0.30  2006/01/31 14:31:15  sxf
**    Handle request to go SAFE without creating an error.
**    I n line with AGS_0_18.
**    ,
**
**    Revision 0.29  2005/10/04 12:40:53  sxf
**    Brought in line with QNX4 baseline AGS_0_17.
**
**    Revision 0.28  2004/04/16 09:33:56  mjf
**    Tidying up of QNX v6 changes to create version 1.00.
**
**    Revision 0.27  2004/04/16 09:31:00  mjf
**    Checking in of QNX v6 porting done by MAN.
**
**    Revision 0.26  2003/09/04 08:10:40  sxf
**    Updated for baseline AGS_0_13.
**
**    Revision 0.25  2003/09/04 07:54:57  sxf
**    Updated for baseline AGS_0_12.
**
**    Revision 0.24  2003/05/01 10:56:10  man
**    Updated version for new baseline.
**
**    Revision 0.23  2003/05/01 10:52:24  man
**    Pass the frame mean, frame rms and case temperature
**    from the agg to ags.
**
**    Revision 0.22  2003/01/23 09:35:14  man
**    Included two Oid's for CCD case temperature
**    and cooler power level.
**
**    Revision 0.21  2002/07/05 15:26:47  sxf
**    Updated for release AGS_0_10.
**
**    Revision 0.20  2002/05/07 09:38:53  sxf
**    Increase receive timeout.
**
**    Revision 0.19  2002/04/10 17:17:21  sxf
**    Version 0.09.
**
**    Revision 0.18  2002/03/26 11:27:35  sxf
**    Updated for baseline AGS_0_08.
**
**    Revision 0.17  2002/03/26 10:11:17  sxf
**    Add units for SDB data.
**
**    Revision 0.16  2002/03/19 10:29:56  sxf
**    Updated for baseline AGS_0_07.
**
**    Revision 0.15  2002/03/18 11:32:23  sxf
**    Centroid command has one more response parameter, autoguide on has one less.
**
**    Revision 0.14  2002/03/05 16:43:10  sxf
**    Updated for baseline AGS_0_06.
**
**    Revision 0.13  2002/03/01 15:32:45  sxf
**    Updated for version 0.05
**
**    Revision 0.12  2002/02/26 11:38:51  sxf
**    Add enumerated type iAgsMonitorCmds_t.
**
**    Revision 0.11  2002/02/05 14:55:01  sxf
**    Updated version.
**
**    Revision 0.10  2002/02/05 10:52:49  sxf
**    Added D_AGS_AGTEMP SDB datum.
**
**    Revision 0.9  2002/02/05 08:23:19  sxf
**    Increase receive timeout.
**
**    Revision 0.8  2002/01/21 09:17:33  sxf
**    Add prototype for iAgsAddCmdToQueueStart.
**
**    Revision 0.7  2001/12/13 15:19:32  sxf
**    Use explicit path to mcc for Cil map etc.
**
**    Revision 0.6  2001/12/05 14:02:49  mjf
**    Version increment.
**
**    Revision 0.5  2001/12/03 09:13:59  sxf
**    Update CIL definitions in preparation for integration.
**
**    Revision 0.4  2001/11/09 10:50:55  sxf
**    Updated Oid table to report data to SDB correctly.
**
**    Revision 0.3  2001/10/17 15:39:06  sxf
**    First version with basic functionality.
**
**    Revision 0.2  2001/09/21 13:59:39  sxf
**    Further development.
**
**    Revision 0.1  2001/09/18 14:11:07  sxf
**    ÿ¡Initial version.
**
**
*******************************************************************************/


#ifndef AGS_PRIVATE_H_DEFINED
#define AGS_PRIVATE_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/
#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Cil.h"
#include "Clu.h"
#include "Sdb.h"
#include "Ags.h"
#include "Agg.h"

/*
** Local include files
*/


/*
** Macro definitions
*/

/* About the AGS */

#define I_AGS_PROGRAM_NAME   "Ags"
#define I_AGS_PROGRAM_ABOUT  "Autoguider Service Task"
#define I_AGS_YEAR           "2003-07"
#define I_AGS_RELEASE_DATE   "24 October 2007"
#define I_AGS_MAJOR_VERSION  1
#define I_AGS_MINOR_VERSION  7 
#define I_AGS_APP_VERSION    (1000*I_AGS_MAJOR_VERSION + I_AGS_MINOR_VERSION)

/* Maximum number of commands to be stored in AGS queue. */

#define I_AGS_LEN_CMDQ       10

/* Maximum length of CIL message to be received by AGS */

#define I_AGS_CIL_DATALEN    256

/* Timeout in milliseconds for receiving CIL messages */

#define I_AGS_CIL_RX_TIMEOUT 1500         /* Was 1500. */ 
#define I_AGS_CIL_ITEM_MAX   380        /* Max number of items per Sdb */

/* Time after which safe-state is assumed if no heartbeats have been received */

#define I_AGS_LOSS_HB_SEC    3
#define I_AGS_LOSS_HB_NSEC   0

#define I_AGS_AGG_TIMEOUT    3         /* Timeout for AGG commands. */

#define I_AGS_NULL_SEQNUM    0


/* For declaring, defining and initialising global variables */

#ifdef M_AGS_MAIN_C
#define I_AGS_EXTERN
#define I_AGS_INIT( Initialisation ) = Initialisation
#else
#define I_AGS_EXTERN         extern
#define I_AGS_INIT( Initialisation )
#endif


/* Defaults for common runtime parameters */

#define I_AGS_DFLT_QUIET     FALSE
#define I_AGS_DFLT_VERBOSE   !( I_AGS_DFLT_QUIET )
#define I_AGS_DFLT_SYSLOG    TRUE
#define I_AGS_DFLT_DEBUG     E_LOG_NOTICE
#define I_AGS_DFLT_PRIORITY  10
#define I_AGS_DFLT_HELP      FALSE
#define I_AGS_DFLT_LOG       "\0"
#define I_AGS_DFLT_CIL       "AGS"

#ifdef E_WFL_OS_QNX4

#define I_AGS_DFLT_CONFIG    "//1/opt/ttl/etc/Ags.cfg"
#define I_AGS_DFLT_CIL_MAP   "//1/opt/ttl/etc/Cil.map"

#elif defined E_WFL_OS_QNX6

#define I_AGS_DFLT_CONFIG    "/ttl/sw/etc/Ags.cfg"
#define I_AGS_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"

#else

#define I_AGS_DFLT_CONFIG    "/ttl/sw/etc/Ags.cfg"
#define I_AGS_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#endif

/* Command-line specification: switch label text and explanation text */


#define I_AGS_ARG_GUIDE       0
#define I_AGS_SWITCH_GUIDE    "guide <CIL>"
#define I_AGS_EXPL_GUIDE       "Guide process cil id (default AGG)"

/* Definitions of CIL addresses. */

#define I_AGS_CIL_CHB        E_CIL_CHB
#define I_AGS_CIL_MCP        E_CIL_MCP
#define I_AGS_CIL_SDB        E_CIL_SDB

#define I_AGS_CIL_AGG        E_CIL_AGG
#define I_AGS_CIL_TCS        E_CIL_TCS

#define I_AGS_AUTOGUIDE_ON_BRIGHTEST_PARS  2
#define I_AGS_AUTOGUIDE_ON_RANK_PARS       3
#define I_AGS_AUTOGUIDE_ON_RANGE_PARS      4
#define I_AGS_AUTOGUIDE_ON_PIXEL_PARS      4
#define I_AGS_AUTOGUIDE_OFF_PARS           1
#define I_AGS_CONF_EXP_TIME_PARS           2
#define I_AGS_CONF_FRAME_RATE_PARS         2
#define I_AGS_CONF_FRAME_AVERAGE_PARS      2
#define I_AGS_CONF_CALIB_PARS              3

#define I_AGS_PER_MILLI_UNITS              0.001

#define I_AGS_INFO_RSP_PARS       9
#define I_AGS_CENTROID_RSP_PARS   5
#define I_AGS_AUTOGUIDE_RSP_PARS  6

typedef enum iAgsMonitorCmds_s
{
   I_AGS_CENTROID_ONLY,         /* Send CENTROID only. */
   I_AGS_CENTROID_INFO          /* Send CENTROID and INFO commands. */
} iAgsMonitorCmds_t;

/* Structure for SDB submissions */

typedef struct iAgsSdbDat_e
{
   Uint32_t NumElts;                   /* Element count for submission to SDB */
                                       /* Datums to be submitted to the SDB */
   eSdbDatum_t Datum[ D_AGS_DATAID_EOL ]; 
} iAgsSdbData_t;

/* Structure for AGS data store */

typedef struct iAgsData_s
{
   ProcState_t DemandState;         /* Demand state of AGS. */
   ProcState_t ActualState;         /* Actual state of AGS. */
   Int32_t     CilId;               /* Our own CIL identifier. */
   Int32_t     Centroid_X;          /* X pixel of latest centroid. */
   Int32_t     Centroid_Y;          /* Y pixel of latest centroid. */
   Int32_t     Cetroid_FWHM;        /* FWHM of latest centroid. */
   Int32_t     Centroid_Peak;       /* Peak grey level of latest centroid. */
   Int32_t     GuideWin_TLX;        /* Guide window top left, x pixel. */
   Int32_t     GuideWin_TLY;        /* Guide window top left, y pixel. */
   Int32_t     GuideWin_BRX;        /* Guide window bottom right, x pixel. */
   Int32_t     GuideWin_BRY;        /* Guide window bottom right, y pixel. */
   Int32_t     IntTime;             /* IUntegration time. */
   Int32_t     Fskip;               /* Frame skip. */ 
   Int32_t     GuideMag;            /* Magnitude of guide source. */
   Int32_t     AggCilId;            /* Agg CIL ID */
   Int32_t     AggPid;              /* Process ID for AGG.  */
   Int32_t     AggState;            /* Last reported AGG state. */
   float       AggTemp;             /* Temperature reported from Agg. */
   float       AggCase;             /* Case temperature reported from Agg. */
   float       AggFrameMean;        /* Frame mean */
   float       AggFrameRms;         /* Frame Rms */
   Int32_t     AggPercPow;          /* Percentage cooler power. */
   Int32_t     AggTempState;        /* Last reported AGG temperature state. */
   Int32_t     HeartbeatCount;      /* Number of heartbeats received */
   Int32_t     HeartbeatLossCount;  /* Count at which heartbeats lost */
   eTtlTime_t  LastHeartbeatTime;   /* Time of last received heartbeat */
   eCilMsg_t   ChbResponseMsg;      /* CIL response to heartbeat message */ 
   eCilMsg_t   ReplyMsg;            /* Replies to MCP messages */ 
   eCilMsg_t   SdbSubmissionMsg;    /* SDB submission message */
   eCilMsg_t   RxMsg;               /* Message received from TCS. */
   Bool_t      SdbSubmitReq;        /* Submission to SDB required */
   Bool_t      SafeStateReq;        /* Request to go safe. */
   Uint32_t    SafeStateSeqNum;     /* Sequence no. associated w/ safe cmd */
   Uint32_t    AggCmdSeqNum;        /* Agg command sequence number. */
} iAgsData_t;


typedef struct iAgsOidTable_s
{
   Int32_t     Oid;             /* Identification index - same as a DatumId */
   eSdbUnit_t  Units;           /* Units associated with the object  */
   Int32_t     Value;           /* Value to be submitted  */
   Bool_t      Changed;         /* If Changed submit to SBD.  */
   eTtlTime_t  TimeStamp;       /* Time of last change of this value.. */
   Int32_t (*CvtFn)( Int32_t ); /* Pointer to conversion function */
} iAgsOidTable_t;

typedef struct iAgsCmdQueue_s *iAgsCmdPtr_t;

typedef struct iAgsCmdQueue_s
{
   eCilMsg_t    TcsCmd;
   eCilMsg_t    AggCmd;
   eTtlTime_t   TimeOut;
   iAgsCmdPtr_t NextCmdPtr;

} iAgsCmd_t;

/*
** Variables - external to all except AgsMain module
*/ 

/*
** This table is indexed into by Oid.
** The order of each block of items must be the same as that defined in the
** SptDataID list in Spt.h. The Oid Table must be in the same order as
** the enumerated Datum Ids.  When adding new OIDs to this table ensure 
** that the I_AGS_FINAL_DATA_ID etc are updated if appropriate.
*/

I_AGS_EXTERN iAgsOidTable_t iAgsOidTable[]
#ifdef M_AGS_MAIN_C
={
/*__Oid_________________Units_____________________Value_______________Changed__TimeStamp__CvtFn*/

{D_AGS_DATAID_BOL     , E_SDB_INVALID_UNITS     , E_AGS_UNKNOWN     , FALSE  , {0,0},     NULL },
{D_AGS_PROC_STATE     , E_SDB_PROCSTATE_UNITS   , SYS_INVALID_STATE , FALSE  , {0,0},     NULL },
{D_AGS_AUTH_STATE     , E_SDB_AUTH_STATE_UNITS  , E_MCP_AUTH_NONE   , FALSE  , {0,0},     NULL },
{D_AGS_SYS_REQUEST    , E_SDB_SYSREQ_UNITS      , E_MCP_SYSREQ_NONE , FALSE  , {0,0},     NULL },
{D_AGS_APP_VERSION    , E_SDB_MVERSION_UNITS    , 0                 , FALSE  , {0,0},     NULL },
{D_AGS_AGSTATE        , E_SDB_NO_UNITS          , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_WINDOW_TLX     , E_SDB_MPIXEL_UNITS      , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_WINDOW_TLY     , E_SDB_MPIXEL_UNITS      , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_WINDOW_BRX     , E_SDB_MPIXEL_UNITS      , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_WINDOW_BRY     , E_SDB_MPIXEL_UNITS      , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_INTTIME        , E_SDB_MSEC_UNITS        , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_FRAMESKIP      , E_SDB_NO_UNITS          , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_GUIDEMAG       , E_SDB_MSTARMAG_UNITS    , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_CENTROIDX      , E_SDB_MPIXEL_UNITS      , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_CENTROIDY      , E_SDB_MPIXEL_UNITS      , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_FWHM           , E_SDB_MPIXEL_UNITS      , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_PEAKPIXEL      , E_SDB_NO_UNITS          , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_AGTEMP         , E_SDB_MCELSIUS_UNITS    , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_AGCASETEMP     , E_SDB_MCELSIUS_UNITS    , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },
{D_AGS_AGPERCPOW      , E_SDB_MPERCENT_UNITS    , E_AGS_UNKNOWN     ,  TRUE  , {0,0},     NULL },

{D_AGS_DATAID_EOL     , E_SDB_INVALID_UNITS     , E_AGS_UNKNOWN     , FALSE  , {0,0},     NULL }
}
#endif
;

/* Define first and last datum IDs to be submitted to the SDB. */
#define I_AGS_FIRST_DATUMID    D_MCP_FIRST_USER_DATUM
#define I_AGS_FINAL_DATUMID    D_AGS_AGPERCPOW

/* AGS data area (containing global variables) */

I_AGS_EXTERN iAgsData_t iAgsData;
I_AGS_EXTERN iAgsCmd_t *iAgsCmdQueuePtr;


/* AgsInit.c */
extern Status_t iAgsSetup( int, char ** );
extern Status_t iAgsParseCommandLine( int, char ** );
extern Status_t iAgsProcessConfig( void );
extern void     iAgsSetState( ProcState_t );
extern Status_t iAgsCheckAndSet(  eAgsDataId_t , Int32_t  );

/* AgsMain.c */
extern Status_t iAgsMonitorAgg( void );
extern Status_t iAgsReceiveMessage( void );
extern Status_t iAgsCheckSafeState( void );
extern Status_t iAgsProcessStateChange( void );

/* AgsProcess.c */
extern Status_t iAgsReceiveReply ( void );
extern Status_t iAgsReceiveCommand ( void );
extern Status_t iAgsProcessHeartbeat( eTtlTime_t * );
extern Status_t iAgsProcessMonitorCommands( iAgsMonitorCmds_t );
extern Status_t iAgsProcessShutdown( void );
extern Status_t iAgsProcessSafeState ( void );
extern Status_t iAgsProcessActivate ( void );
extern Status_t iAgsProcessCmdQueue( void );
extern Status_t iAgsSendTopCmd();

/* AgsReply.c */
extern Status_t iAgsAckReply( eCilMsg_t * );
extern Status_t iAgsActReply( eCilMsg_t * );
extern Status_t iAgsComReply( eCilMsg_t * );
extern Status_t iAgsErrReply( eCilMsg_t *, Int32_t );
extern Status_t iAgsSet1Reply( eCilMsg_t *, eTtlSet_t *, Int32_t );

/* AgsUtil.c */
extern Status_t    iAgsAddCmdToQueue( iAgsCmd_t *, 
                                      eAgsCmdBody_t *,
                                      eAggCmdBody_t * );
extern Status_t    iAgsAddCmdToQueueStart( iAgsCmd_t *,
                                           eAgsCmdBody_t *,
                                           eAggCmdBody_t * );
extern Bool_t      iAgsSearchCmdQueue( Int32_t );
extern Status_t    iAgsDelCmdFromQueue( void );
extern Status_t    iAgsSendCilMsg ( Int32_t, eCilMsg_t *);
extern Status_t    iAgsRequestStateChange( void );
extern void        iAgsRestoreState( void );
extern void        iAgsOutputStatus( Bool_t );
extern ProcState_t iAgsGetState( void );
extern void        iAgsShutdown( void );


/*( AgsReport.c */
extern Status_t iAgsSubmitStatus( void );


I_AGS_EXTERN eCilMsg_t iAgsNullTcsCmd
#ifdef M_AGS_MAIN_C
   = {   
      (Int32_t)(NULL),    /* Address code for source */
      (Int32_t)(NULL),    /* Address code for the destination */
      (Int32_t)(NULL),    /* Qualifier on component of service transaction */
      (Int32_t)(NULL),    /* Defines transaction service being used */
      (Uint32_t)(NULL),   /* Sequence number used by client */
      (Int32_t)(NULL),
      (Int32_t)(NULL),    /* Absolute time associated with the CIL message */
      (void *)(NULL),     /* Pointer to memory containing (space for) data */
      (size_t)(NULL)      /* Number of bytes allocated for the data field */
     }
#endif
;

/*
**For parsing custom command-line arguments
**Format: Text, Chars-to-match, Explanation, FALSE, NULL
*/
I_AGS_EXTERN eCluArgSpec_t  eCluCustomArg[]
#ifdef M_AGS_MAIN_C
   = { { I_AGS_SWITCH_GUIDE,  3, I_AGS_EXPL_GUIDE,  FALSE, NULL },
       { E_CLU_EOL,           0, E_CLU_EOL,        FALSE, NULL } }
#endif
;

#endif

/*******************************************************************************
** End of File Name: AgsPrivate.h
*******************************************************************************/
