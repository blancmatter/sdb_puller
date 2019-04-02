/***************************************************************************
** File Name:
**    Tcs.h
** Purpose:
**    TCS public header file.
**
** Description:
**    Defines TCS status codes, data identifiers for TCS, RCS and AGS
**    and TCS services.
**
** Author:      
**    Mark Bowman (mkb)
**    Stephen Foale (sxf)
**    Tony Wright (tpw)
**
** Copyright (C) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    1.26 11/04/03 sxf
**    
** History:
**   Revision 1.26 11/04/03 sxf Added reporting of pre and post-pointing model
**                              Alt/Az positions & associated UTC time.
**   Revision 1.25 19/12/01 mkb Improved comments on status codes and removed
**                              unused codes.
**   Revision 1.24 05/06/01 sxf Added E_TCS_PROCQUOTA.
**   Revision 1.23 22/05/01 sxf Added E_TCS_ERR_HANDLE.
**   Revision 1.22 15/05/01 mkb Added E_TCS_MESSAGE.
**   Revision 1.21 08/05/01 sxf Merged classes D1-5 and D1-7.
**   Revision 1.20 09/04/01 mkb Moved PIGGSD enums to PIGDEF
**   Revision 1.19 05/04/01 sxf Updated for Mother.
**   Revision 1.18 01/03/01 tpw Added E_TCS_NETDISALL.
**   Revision 1.17 01/03/01 tpw Added E_TCS_NETUNK.
**   Revision 1.16 01/03/01 tpw Added E_TCS_NETPARSE.
**   Revision 1.15 27/02/01 tpw Added E_TCS_TIMEOUT. Also rev noos now X.yy
**   Revision 1.14 22/02/01 tpw Added E_TCS_*** for use from USER (was TCS__***)
**   Revision 1.13 12/02/01 tpw Removed trailing comma from 11.C
**   Revision 1.12 08/02/01 sxf Added E_TCS_NOTREC.
**   Revision 1.11 06/02/01 tpw added E_TCS_MECHERR, VERBCOMP and VERBUNCOMP
**   Revision 1.10 02/02/01 tpw added E_TCS_OVER
**   Revision 1.09 31/01/01 sxf Added eTcsProcAcp.
**   Revision 1.08 13/12/00 sxf Added E_TCS_WRONGVER error code.
**   Revision 1.07 28/11/00 sxf Merged classes D1-3 and D1-4.
**   Revision 1.06 23/11/00 sxf Removed include of Mcp.h.
**   Revision 1.05 08/11/00 sxf Adde E_TCS_SET_REPLY status.
**   Revision 1.04 18/10/00 mkb Include Mcp.h and remove all PROC_STATE defs.
**   Revision 1.03 17/10/00 mkb Offset Data identifiers by 
**                             D_MCP_FIRST_USER_DATUM & remove 
**                             D_TCS_PROC_STATE
**   Revision 1.02 10/10/00 mkb Replaced E_TCS_COMMS_QUEUE_ERROR with 
**                            E_TCS_QUEUE_ERROR, E_TCS_COMMS_POOL_ERROR with
**                            E_TCS_POOL_ERROR and remove 
**                            E_TCS_COMMS_NOMESSAGE.
**   Revision 1.01 29/9/00 mkb Added E_TCS_DATUM_NOT_AVAIL to error codes.
**   Revision 1.0 21/9/00 mkb Initial revision
**   Version 0.9  27/9/00 sxf Renamed E_TCS_COMMS_INVALID_MESSAGE_TYPE to be
**                            less than 32 characters for compatibility with
**                            Fortran.  
**   Version 0.8  26/9/00 sxf Added new error codes.
**   Version 0.7  15/9/00 mkb Added SDB data identifiers.
**   Version 0.6   9/9/00 sxf Moved status codes to tcslog.h (also rename
**                            some to shorter/more general names).
**   Version 0.5  31/8/00 sxf Moved definition of E_TCS_NOMINAL to TCSLOG.H
**   Version 0.4   7/7/00 mkb Expanded header to comply with TTL standards.
**   Version 0.3   5/7/00 mkb Added E_TCS_CMD for network command services.
**   Version 0.2  15/5/00 mkb Added E_TCS_DATAEXCH_INIT_FAILURE. 
**   Version 0.1   5/5/99 mkb Original version.
*****************************************************************************/

#ifndef TCS_H_DEFINED
#define TCS_H_DEFINED

/* local includes. */
#include "TtlSystem.h"

/* Status value used within TCS. */
/* Note: STATUS_START is defined in TtlSystem.h. */
typedef enum 
{
  E_TCS_NOMINAL = STATUS_START(TCS), 
                             /* (0x90000) Normal operation.                 */
  E_TCS_GEN_ERR,             /* (0x90001) Miscellaneous error.              */
  E_TCS_ERR_OPENING_FILE,    /* (0x90002) Unable to open file.              */
  E_TCS_ERR_LOGGING_TO_FILE, /* (0x90003) Failure in logging to file.       */
  E_TCS_ERR_DATETIME,        /* (0x90004) Error forming date/time.          */
  E_TCS_ERR_INV_DEST,        /* (0x90005) Invalid destination for message.  */
  E_TCS_ERR_LOG_FAILED,      /* (0x90006) Report message to TCS log failed. */
  E_TCS_NOMESSAGE,           /* (0x90007) No messages left on queue.        */
  E_TCS_ERR_RTL,             /* (0x90008) Run Time Library error.           */
  E_TCS_ERR_GUI_DAT,         /* (0x90009) Autoguider data error.            */
  E_TCS_ASTQUEERR,           /* (0x9000a) Error queuing AST.                */
  E_TCS_NOINITAST,           /* (0x9000b) Invalid INITAST for process.      */
  E_TCS_NOINI,               /* (0x9000c) TCS did not initialise.           */
  E_TCS_TERMASTQERR,         /* (0x9000d) Error queuing TERMAST.            */
  E_TCS_PRCNOTERM,           /* (0x9000e) Process not terminated.           */
  E_TCS_PRCNOINI,            /* (0x9000f) Process not initialised.          */
  E_TCS_EXPIRED,             /* (0x90010) Time stamp expired.               */
  E_TCS_INVRESP,             /* (0x90011) Invalid response.                 */
  E_TCS_NOQB,                /* (0x90012) Unable to get queue block.        */
  E_TCS_SIGNOTDEL,           /* (0x90013) Signal not delivered.             */
  E_TCS_NOPRI,               /* (0x90014) Failed to set priority.           */
  E_TCS_CILFLUSH,            /* (0x90015) Failed to flush CIL queue.        */
  E_TCS_ENABLEAST,           /* (0x90016) Failed to reenable AST.           */
  E_TCS_NOCORR,              /* (0x90017) Unable to correlate.              */
  E_TCS_NOACTION,            /* (0x90018) No action required.               */
  E_TCS_DUPID,               /* (0x90019) Duplicate ID.                     */
  E_TCS_TTERR,               /* (0x9001a) Transaction table error.          */
  E_TCS_MESSMISS,            /* (0x9001b) Message missing.                  */
  E_TCS_TIMEERR,             /* (0x9001c) Time error.                       */
  E_TCS_INVID,               /* (0x9001d) Invalid identifier.               */
  E_TCS_NOERR,               /* (0x9001e) Failed to send error message.     */
  E_TCS_ERRTOQ,              /* (0x9001f) Error writing to queue.           */
  E_TCS_DUMPMESSAGE,         /* (0x90020) Message abandoned.                */
  E_TCS_NORECV,              /* (0x90021) Receive failure.                  */
  E_TCS_TIMEOUTERR,          /* (0x90022) Error checking for timeout.       */
  E_TCS_DISABLEAST,          /* (0x90023) Error disabling AST.              */
  E_TCS_TRANSERR,            /* (0x90024) Transmit error.                   */
  E_TCS_NOGETMESSAGE,        /* (0x90025) Error getting message.            */
  E_TCS_PACKERR,             /* (0x90026) Error packing message.            */
  E_TCS_INVMESS,             /* (0x90027) Invalid message type.             */
  E_TCS_LENERR,              /* (0x90028) Mesasge length error.             */
  E_TCS_NOHASH,              /* (0x90029) Failed to find hash entry.        */
  E_TCS_MALLOC_ERROR,        /* (0x9002a) Memory allocation failure.        */
  E_TCS_DTENT,               /* (0x9002b) Failed to create data table entry.*/
  E_TCS_NOINIQ,              /* (0x9002c) Error initialising queues.        */
  E_TCS_NOENCEFS,            /* (0x9002d) Error enabling CEFs.              */
  E_TCS_NOEXH,             /* (0x9002e) Error declaring AST as exit handler.*/
  E_TCS_NOCREPRC,            /* (0x9002f) Error creating process.           */
  E_TCS_RANGERROR,           /* (0x90030) Range error.                      */
  E_TCS_ILLEGALMEC,          /* (0x90031) Illegal mechanism.                */
  E_TCS_INVPROC,             /* (0x90032) Invalid process.                  */
  E_TCS_INVALID_ARGUMENT,    /* (0x90033) Invalid argument.                 */
  E_TCS_SYSCALL_ERROR,       /* (0x90034) Error returned by a system call.  */
  E_TCS_POOL_ERROR,          /* (0x90035) Message pool error.               */
  E_TCS_QUEUE_ERROR,         /* (0x90036) Queue error.                      */
  E_TCS_FILE_NOT_FOUND,      /* (0x90037) File not found.                   */
  E_TCS_FILE_READ_ERROR,     /* (0x90038) Error read from a file.           */ 
  E_TCS_INVALID_VALUE,       /* (0x90039) Invalid value.                    */
  E_TCS_FLUSH_ERR,           /* (0x9003a) Error flushing to disk.           */
  E_TCS_FCLOSE_ERR,          /* (0x9003b) Error closing file.               */
  E_TCS_DATUM_NOT_AVAIL,     /* (0x9003c) Datum not available at this time. */
  E_TCS_SET_REPLY,           /* (0x9003d) Reply received from a SET command.*/
  E_TCS_WRONGVER,            /* (0x9003e) Process has wrong build version.  */
  E_TCS_ILLEGAL_AGSTATE,   /* (0x9003f) Cannot recognise reported AGS state.*/
  E_TCS_REPLY_ERROR,         /* (0x90040) Error in reply.                   */
  E_TCS_OVER,                /* (0x90041) Command overriden.                */
  E_TCS_MECHERR,             /* (0x90042) A mechanism is in error.          */
  E_TCS_VERBCOMP,            /* (0x90043) Verb has completed.               */
  E_TCS_VERBUNCOMP,          /* (0x90044) Verb has not completed yet.       */
  E_TCS_NOTREC,              /* (0x90045) Not recognised.                   */
  E_TCS_AMBIG,               /* (0x90046) Ambiguous command or keyword.     */
  E_TCS_BADFILE,      /* (0x90047) File cannot be opened or does not exist. */
  E_TCS_BADIO,               /* (0x90048) Failed input/output operation.    */
  E_TCS_BADKEYW,             /* (0x90049) Bad keyword.                      */
  E_TCS_BADPARAM,            /* (0x9004a) Bad parameter.                    */
  E_TCS_BADRANGE,            /* (0x9004b) Parameter range error.            */ 
  E_TCS_CATEMPTY,            /* (0x9004c) Catalogue is empty.               */
  E_TCS_DUPENTRY,            /* (0x9004d) Duplicate catalogue entry.        */
  E_TCS_ENGREJECT,    /* (0x9004e) Command not allowed in engineering mode. */
  E_TCS_IGNORED,             /* (0x9004f) Command results were ignored.     */
  E_TCS_MANUAL,              /* (0x90050) Mechanism is under manual control.*/
  E_TCS_NOENTRY,     /* (0x90051) No corresponding catalogue entry located. */
  E_TCS_NOFOLLOW,            /* (0x90052) Mechanism is not set to track.    */
  E_TCS_NOMOVING,            /* (0x90053) Mechanism is not moving.          */  
  E_TCS_NOTRACK,             /* (0x90054) Mechanism is not tracking.        */  
  E_TCS_STATEREJECT,         /* (0x90055) Inappropriate state for command.  */
  E_TCS_VALID,               /* (0x90056) A successfull command completion. */
  E_TCS_TIMEOUT,             /* (0x90057) Network command has timed out.    */
  E_TCS_NETPARSE,          /* (0x90058) Network command has failed to parse.*/
  E_TCS_NETUNK,              /* (0x90059) Network command unknown.          */
  E_TCS_NETDISALL,           /* (0x9005a) Command not allowed over network. */
  E_TCS_NOGETLOG,            /* (0x9005b) Unable to decode logical.         */
  E_TCS_PRCSTART_FAILED,     /* (0x9005c) Failed to start a TCS process.    */
  E_TCS_MOTHER_ALREADY_DEAD,/* (0x9005d) Supervisory process has terminated.*/
  E_TCS_NOASCEFC,       /* (0x9005e) Unable to associate event flag cluster.*/
  E_TCS_INVSTATE,            /* (0x9005f) Invalid state.                    */
  E_TCS_NOMONPROC,           /* (0x90060) Unable to monitor a TCS process.  */
  E_TCS_RECOVERING,          /* (0x90061) TCS is recovering from a failure. */
  E_TCS_MESSAGE,             /* (0x90062) Message waiting.                  */
  E_TCS_ERR_HANDLE,          /* (0x90063) Message reported by error handler.*/
  E_TCS_DISK_ERR,            /* (0x90064) TCS detected disk error.          */
  E_TCS_PROCQUOTA,           /* (0x90065) Process quota exceeded.           */
  E_TCS_NETDISABLED,         /* (0x90066) Network interface is disabled.    */
  E_TCS_COM_ERR              /* (0x90067) Communications error.             */
} eTcsStatus_t;


/* Enumerated list of TCS processes. */
typedef enum
{
  eTcsProcStart = 0,
  eTcsProcAcp,
  eTcsProcComms,
  eTcsProcDataexch,
  eTcsProcDisplay,
  eTcsProcLog, 
  eTcsProcMntlib,
  eTcsProcMother,
  eTcsProcMsg,
  eTcsProcNetwork,
  eTcsProcOwn,
  eTcsProcOwninit,
  eTcsProcPoint,
  eTcsProcSync,
  eTcsProcTcslib,
  eTcsProcTcslibc,
  eTcsProcTv,
  eTcsProcUser,
  eTcsProcEnd
} eTcsProcType;       

typedef enum 
{
  E_TCS_ENABLE,
  E_TCS_DISABLE
} eTcsProcChange_t;
   
/* TCS services. */
/* Note: SERVICE_START is defined in TtlSystem.h. */
enum eTcsServices {
    E_TCS_HEARTBEAT = 255,

    E_TCS_CMD = SERVICE_START(TCS)

};

/* Data identifiers for TCS data supplied to the SDB. */
typedef enum eTcsDataId_e
{
  D_TCS_DATAID_BOL = D_MCP_FIRST_USER_DATUM,   /* Beginning of List.      */
  D_TCS_SOURCE_NAME,       /* Name of Current Sky Source.                 */
  D_TCS_SOURCE_RA,         /* Current Source Right Ascension.             */
  D_TCS_SOURCE_DEC,        /* Current Source Declination.                 */
  D_TCS_SOURCE_DIFF_RA,    /* Differential Tracking in Right Ascension.   */
  D_TCS_SOURCE_DIFF_DEC,   /* Differential Tracking in Declination.       */
  D_TCS_SOURCE_EQUINOX,    /* Current Source Equinox.                     */
  D_TCS_SOURCE_PROPER_RA,  /* Proper motion of Source in Right Ascension. */
  D_TCS_SOURCE_PROPER_DEC, /* Proper motion of Source in Declination.     */ 
  D_TCS_SOURCE_EPOCH,      /* Epoch for Proper Motion.                    */
  D_TCS_SOURCE_PARALLAX,   /* Current Source Parallax.                    */
  D_TCS_SOURCE_RAD_VEL,    /* Current Source Radial Velocity.             */
  D_TCS_SOURCE_COORD_TYPE, /* Current Source Coordinate System.           */
  D_TCS_LOC_APP_SID_TIME,  /* Local Apparent Sidereal Time.               */
  D_TCS_PARALLACTIC_ANGLE, /* Parallactic Angle.                          */
  D_TCS_AIR_MASS,          /* Current Air Mass of Observation.            */
  D_TCS_GUIDING_STATE,     /* State of Autoguided Tracking.               */
  D_TCS_GUIDING_REF_X,     /* Reference CCD Column for Guide Star.        */
  D_TCS_GUIDING_REF_Y,     /* Reference CCD Row for Guide Star.           */
  D_TCS_AUTOGUIDER,        /* Current Autoguider.                         */
  D_TCS_INSTRUMENT,        /* Current Instrument.                         */
  D_TCS_FOCAL_STAT,        /* Current Focal Station.                      */
  D_TCS_OP_MODE,           /* Telescope Control System Operating Mode.    */
  D_TCS_SKY_POS_ANGLE,     /* Sky position angle.                         */
  D_TCS_MNT_APP_AZ,        /* Pre-pointing model azimuth angle.           */
  D_TCS_MNT_APP_AL,        /* Pre-pointing model altitude angle.          */
  D_TCS_MNT_PP_AZ,         /* Post-pointing model azimuth angle.          */
  D_TCS_MNT_PP_AL,         /* Post-pointing model altitude angle.         */
  D_TCS_MNT_TSEC,          /* UTC timestamp (secs) associated with Alt/Az */
  D_TCS_MNT_TNSEC,         /* UTC timestamp (nsec) associated with Alt/Az */
  D_TCS_DATAID_EOL         /* End of list.                                */ 
} eTcsDataId_t;


/* Data identifiers for RCS data supplied to the SDB. */
typedef enum eRcsDataId_e
{
  D_RCS_DATAID_BOL,        /* Beginning of List.                          */
  D_RCS_DUMMY  = D_MCP_FIRST_USER_DATUM, /* Robotic Control System State. */
  D_RCS_DATAID_EOL         /* End of list.                                */ 
} eRcsDataId_t;

#endif

/***************************************************************************
** End of File Name: Tcs.h
***************************************************************************/
