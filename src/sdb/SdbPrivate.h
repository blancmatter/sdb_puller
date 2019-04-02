/*
** Header File Name:
**    SdbPrivate.h
**
** Purpose:
**    Private header file for Status Database (SDB) functions
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay
**
** Version:
**    $Id: SdbPrivate.h,v 0.90 2007/10/04 10:49:52 mjf Exp $
**
** History:
**    $Log: SdbPrivate.h,v $
**    Revision 0.90  2007/10/04 10:49:52  mjf
**    Tidy-up for porting to Linux - no functional changes.
**
**    Revision 0.89  2007/09/14 15:55:00  mjf
**    SDB_1_11.
**
**    Revision 0.88  2007/07/17 10:20:02  mjf
**    Added functionality to pass raw UDP packets from Sdb
**    to a MySql database.
**
**    Revision 0.87  2007/03/07 12:49:54  sxf
**    Added SdbRecv process and build for linux platform.
**
**    Revision 0.86  2006/10/09 19:38:41  mjf
**    Increase in number of concurrent open files to 16.
**
**    Revision 0.85  2006/10/03 10:47:10  sxf
**    Baseline SDB_1_07.
**
**    Revision 0.84  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.83  2006/06/27 16:29:09  mjf
**    Default process priority is 19.
**
**    Revision 0.82  2005/06/20 13:35:31  mjf
**    Minor changes to aid porting to Linux - no functional changes.
**
**    Revision 0.81  2005/06/17 09:42:38  mjf
**    SDB_1_03.
**
**    Revision 0.80  2004/10/08 12:29:19  mjf
**    Version 1.02.
**
**    Revision 0.79  2004/05/06 08:50:30  mjf
**    Version 1.01 - addition of SPLC units for T2-408 Services PLC.
**
**    Revision 0.78  2004/04/06 14:01:37  mjf
**    Version 1.00 - ported to QNX v6.
**
**    Revision 0.77  2004/01/19 14:23:39  sxf
**    Modify "find" syntax for QNX6.
**
**    Revision 0.76  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.75  2003/10/07 09:23:32  sxf
**    Updated for baseline SDB_0_60.
**
**    Revision 0.74  2003/08/08 09:50:02  mjf
**    SDB v0.59 - addition of PLC System Status units.
**
**    Revision 0.73  2003/06/05 12:55:52  dws
**    Version 0.58
**
**    Revision 0.72  2003/05/30 11:29:25  mjf
**    Version 0.57.
**
**    Revision 0.71  2003/03/21 15:15:57  mjf
**    Version 0.56.
**
**    Revision 0.70  2003/02/04 16:30:19  mjf
**    Version 0.55.
**
**    Revision 0.69  2002/09/20 14:41:05  man
**    Updated version ready for new baseline.
**
**    Revision 0.68  2002/08/13 17:29:22  mjf
**    Version 0.53.
**
**    Revision 0.67  2002/08/05 10:18:24  mjf
**    Version 0.52.
**
**    Revision 0.66  2002/07/27 11:02:55  mjf
**    Addition of two command-line parameters, to specify the path of the
**    datafile storage, and to specify the number of days after which to
**    clean up SDB files.
**
**    Revision 0.65  2002/04/05 15:38:36  djm
**    Updated version and date.
**
**    Revision 0.64  2002/03/26 11:31:45  mjf
**    Version 0.49.
**
**    Revision 0.63  2002/03/20 17:49:46  man
**    Changed the path of the Sfr from ./ to /opt/ttl/bin
**
**    Revision 0.62  2002/01/10 17:41:47  mjf
**    Version 0.47.
**
**    Revision 0.61  2001/12/17 17:30:07  mjf
**    Version 0.46.
**
**    Revision 0.60  2001/12/13 15:10:19  mjf
**    Version 0.45.
**
**    Revision 0.59  2001/11/06 16:38:37  djm
**    Updated version number.
**
**    Revision 0.58  2001/10/23 09:35:04  mjf
**    Addition of RETRIEVE_L service, to retrieve the last 'n' measurements
**    from disk - like RETRIEVE_F but for the last (not first) measurements.
**
**    Revision 0.57  2001/10/12 11:24:58  mjf
**    Addition of a 'CLEAR_1' service, which will allow the originator of the
**    message to selectively clear their own datum entries. The message takes
**    the same form as the 'CLEAR_S' service, but the data to clear are
**    specified rather than the data to be perserved.
**
**    Revision 0.56  2001/09/25 08:49:10  djm
**    Increased the maximum number of SDB data files that may
**    be open simultaneously.
**
**    Revision 0.55  2001/09/14 11:08:01  djm
**    Updated version number and date. Added new function prototype.
**
**    Revision 0.54  2001/07/17 08:51:47  mjf
**    Update to date and version. No functional changes to SDB in this version,
**    but testlist and testdump improved.
**
**    Revision 0.53  2001/04/24 11:20:59  mjf
**    Version 0.38 - set read/write attributes for created files, and
**    addition of E_SDB_UNSUPPORTED_UNITS.
**
**    Revision 0.52  2001/04/06 08:22:40  mjf
**    Version 0.37 - change of inclinometer units.
**
**    Revision 0.51  2001/03/27 18:57:06  djm
**    Update build date.
**
**    Revision 0.50  2001/03/27 18:52:36  djm
**    Updated baseline number.
**
**    Revision 0.49  2001/03/23 15:31:20  mjf
**    Version 0.35.
**
**    Revision 0.48  2001/02/20 14:26:59  mjf
**    Version 0.34.
**
**    Revision 0.47  2001/02/07 11:38:40  mjf
**    Change of path for database files from //1/tmp to /opt/ttl/data. Note
**    that this directory must exist.
**
**    Revision 0.46  2001/01/03 16:40:04  mjf
**    Correction to month.
**
**    Revision 0.45  2001/01/03 11:41:24  mjf
**    Increments to date, year and version.
**
**    Revision 0.44  2000/12/12 15:01:11  mjf
**    Correction to default unit assignments.
**
**    Revision 0.43  2000/12/11 16:17:21  mjf
**    Use CLU to perform CIL setup and submit version into the SDB.
**
**    Revision 0.42  2000/12/09 14:58:07  mjf
**    Addition of SUBMIT_1P service - post a submission expecting no reply.
**
**    Revision 0.41  2000/12/04 10:42:17  mjf
**    Enhanced log messages to include CIL name in addition to CIL ID.
**
**    Revision 0.40  2000/11/21 16:43:05  djm
**    Increment version number and added prototype for iSdbStorePrevData().
**
**    Revision 0.39  2000/11/09 09:22:56  mjf
**    Use correct units for system requests.
**
**    Revision 0.38  2000/11/03 16:03:32  djm
**    Updated the version numbers.
**
**    Revision 0.37  2000/10/24 13:44:42  djm
**    Updated the version number/dat for the recent SdbStore.c change.
**
**    Revision 0.36  2000/10/19 17:48:23  djm
**    Updated version number for the change to the LISTxxx services.
**
**    Revision 0.35  2000/10/16 15:35:22  djm
**    Version number change for the "testlist" / SdbList.c bug fixes.
**
**    Revision 0.34  2000/10/16 14:07:01  djm
**    Added global variable and constants to cope with file
**    write failures.
**
**    Revision 0.33  2000/10/13 14:59:24  djm
**    Increment version number.
**
**    Revision 0.32  2000/10/10 14:50:29  mjf
**    Updated for new generic MCP datum IDs.
**
**    Revision 0.31  2000/10/10 10:43:56  mjf
**    Version 0.18. Additions made to units in Sdb.h.
**
**    Revision 0.30  2000/10/04 10:57:56  djm
**    Increment version numbers and changed Sfr priority.
**
**    Revision 0.29  2000/09/28 13:44:22  djm
**    Updated version number following bug fix in SdbStore.c.
**
**    Revision 0.28  2000/09/21 11:12:17  djm
**    Adjusted command line areguments with which the SFR is called.
**    Also added system commands for cleaning up SDB files.
**
**    Revision 0.27  2000/09/19 15:51:54  djm
**    Changed version number for latest version of SDB code.
**
**    Revision 0.26  2000/09/11 08:31:18  djm
**    Made file-storage the default on program start.
**
**    Revision 0.25  2000/09/08 14:33:55  djm
**    ÿ¡ÿ¡Implemented new global data enumeration naming scheme.
**
**    Revision 0.24  2000/09/06 14:15:58  djm
**    Major overhaul of function prototypes to conform to the new
**    modular layout of the SDB code.
**
**    Revision 0.23  2000/09/01 09:34:20  djm
**    Removed extraneous code and updated version number.
**
**    Revision 0.22  2000/08/31 15:57:11  djm
**    Added some new function prototypes (and incremented version number).
**
**    Revision 0.21  2000/08/31 09:35:59  djm
**    Added new services and some function prototypes.
**
**    Revision 0.20  2000/08/30 11:40:57  djm
**    Added globals for command line argument parsing. Also removed
**    some internal function protoypes and added some global variables.
**
**    Revision 0.19  2000/08/22 17:56:08  djm
**    Changed some globals from internal to public. Changed necessary
**    function protocols to accept the change.
**
**    Revision 0.18  2000/08/18 14:39:18  djm
**    Alterations to handle new SFR application. This includes command
**    line arguments used by SfrFileRetr in spawning the SFR task.
**
**    Revision 0.17  2000/08/07 15:41:30  djm
**    Added CFU (configuration file utility) task code.
**
**    Revision 0.16  2000/07/21 14:04:06  djm
**    Added definitions, prototypes, etc. for file storage operation.
**
**    Revision 0.15  2000/07/14 15:59:18  djm
**    Added changes to cope with file storage within the SDB.
**
**    Revision 0.14  2000/06/26 14:37:54  djm
**    Added function prototypes for block/multi- requests.
**    Fixed problem with eSdbUnit_t being of undeterminate size
**    (this fouls up some message construction in SdbMulRetr.c).
**
**    Revision 0.13  2000/06/23 10:38:33  djm
**    Added function prototypes for COUNT- service handling functions.
**
**    Revision 0.12  2000/06/07 12:12:22  djm
**    Added addition global variable for handling syslogging options.
**
**    Revision 0.11  2000/06/02 13:16:19  djm
**    Added function prototype for iSdbLnkLstInsert().
**
**    Revision 0.10  2000/05/31 08:46:23  djm
**    Added function prototypes for linked list manipulation functions.
**
**    Revision 0.9  2000/05/21 15:30:22  djm
**    Changes to symbolic constants associated with CIL messages.
**    Also added some function prototypes for the new "list" functions.
**
**    Revision 0.8  2000/05/18 14:58:46  djm
**    Added function prototypes for iSdbAddData() and iSdbAutoSubmit().
**
**    Revision 0.7  2000/05/18 13:41:54  djm
**    Added new array of structures to hold SDB global, reported data.
**
**    Revision 0.6  2000/05/18 09:05:34  djm
**    Add function prototypes for iSdbAckReply() and iSdbErrReply().
**
**    Revision 0.5  2000/05/17 10:30:09  djm
**    Changes to data structures for internal storage of data.
**
**    Revision 0.4  2000/05/12 15:27:11  djm
**    Some new fields in data structures were added. Also added some
**    global variables.
**
**    Revision 0.3  2000/05/12 08:19:27  djm
**    Added hash structure definitions and some function prototypes.
**
**    Revision 0.2  2000/05/11 09:23:28  djm
**    Minor changes to relfect new CIL header structure.
**
**    Revision 0.1  2000/05/09 08:44:31  djm
**    Initial creation.
**
**
*/


#ifndef SDBPRIVATE_H_DEFINED
#define SDBPRIVATE_H_DEFINED



/*
** Required include files
*/

#include <stdio.h>                /* For FILE definition */

#include "TtlSystem.h"            /* For Status_t definition */
#include "Cil.h"                  /* For CIL ID parameter */
#include "Sdb.h"
#include "Clu.h"



/*
** Application details (as used by the CLU calls)
*/

#define I_SDB_PROGRAM_NAME   "Sdb"
#define I_SDB_PROGRAM_TLA    "SDB"
#define I_SDB_PROGRAM_ABOUT  "Status Database"
#define I_SDB_PROGRAM_TITLE  I_SDB_PROGRAM_TLA " - " I_SDB_PROGRAM_ABOUT
#define I_SDB_RELEASE_DATE   "1 October 2007"
#define I_SDB_YEAR           "2000-07"
#define I_SDB_MAJOR_VERSION  1
#define I_SDB_MINOR_VERSION  12



/*
** Application common command line argument defaults.
*/

#define I_SDB_DFLT_QUIET     FALSE
#define I_SDB_DFLT_VERBOSE   TRUE
#define I_SDB_DFLT_SYSLOG    TRUE
#define I_SDB_DFLT_DEBUG     E_LOG_NOTICE
#define I_SDB_DFLT_PRIORITY  19
#define I_SDB_DFLT_HELP      FALSE
#define I_SDB_DFLT_LOG       ""
#define I_SDB_DFLT_CIL       "SDB"
#ifdef E_WFL_OS_QNX4
#define I_SDB_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#else
#define I_SDB_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#endif

/*
** Custom command line arguments (definition macros)
*/

#define I_SDB_CUSTOM_FILESTORE    0
#define I_SDB_CUSTOM_NOFILESTORE  1
#define I_SDB_CUSTOM_DATAPATH     2
#define I_SDB_CUSTOM_CLEANUP      3
#define I_SDB_CUSTOM_MYSQLHOST    4
#define I_SDB_CUSTOM_MYSQLPORT    5
#define I_SDB_NUM_CUSTOM_ARGS     6

/*
** Global custom argument specification (note the string concatenation
** between the E_SDB_XXX definitions and the string that follows (i.e.
** there is no comma).
*/

#ifdef M_SDB_MAIN
   eCluArgSpec_t eCluCustomArg[ I_SDB_NUM_CUSTOM_ARGS + 1 ] =
   {
      {
         E_SDB_FILESTORESTR "", 4, 
         "Store/retrieve old results to/from file", FALSE, NULL
      },
      {
         E_SDB_NOFILESTORESTR "", 3, 
         "Don't store/retrieve old results to/from file", FALSE, NULL
      },
      {
         E_SDB_DATAPATH " <path>", 3, 
         "Specify alternative path for data files", FALSE, NULL
      },
      {
         E_SDB_CLEANUP " [days]", 3, 
         "Specify lifetime of SDB files before cleanup", FALSE, NULL
      },
      {
         E_SDB_MYSQLHOST " <host>", 3,
         "MySQL database server hostname", FALSE, NULL
      },
      {
         E_SDB_MYSQLPORT " <port>", 3,
         "MySQL database port", FALSE, NULL
      },
      {
         E_CLU_EOL, 0, E_CLU_EOL, FALSE, NULL
      }
   };
#else
   extern eCluArgSpec_t eCluCustomArg[];
#endif

/*
** Path to directory where SDB storage and units files are kept.
*/

#ifdef E_WFL_OS_QNX4
#define I_SDB_DB_FILE_PATH   "/opt/ttl/data/" /* Path to write db files to */
#else
#define I_SDB_DB_FILE_PATH   "/ttl/sw/data/" /* Path to write db files to */
#endif
#define I_SDB_PATH_SEPARATOR '/'

/*
** MySQL defaults
*/
#define I_SDB_DFLT_MYSQLHOST  "mizar"          /* Default MySQL host */
#define I_SDB_DFLT_MYSQLPORT  "13320"          /* Default MySQL port */

/* SDB configuration constants */

#define I_SDB_HIST_LIMIT  10   /* Max.No. data stored per defn (>1) */
#define I_SDB_SAFE_AFTER   3   /* The number of seconds without heartbeats */
                               /* before going to safe state */      

/* SDB CIL configuration constants */

#ifdef E_WFL_OS_QNX4
#define I_SDB_CIL_MAPNAME  "/opt/ttl/etc/Cil.map" /* CIL map filename */
#else
#define I_SDB_CIL_MAPNAME  "/ttl/sw/etc/Cil.map" /* CIL map filename */
#endif
#define I_SDB_CIL_ID       E_CIL_SDB   /* SDB's default CIL ID */
#define I_SDB_HBEAT_ID     E_CIL_CHB   /* Expected source of heartbeats */
#define I_SDB_ROUTER_ID    E_CIL_MCB   /* Router CIL ID */
#define I_SDB_TIMEOUT      1000        /* CIL Rx timeout (in milliseconds) */
#define I_SDB_IDLEAFTER    5           /* No. Rx timeouts for state -> "IDLE" */
#define I_SDB_DATASIZE     16384       /* Max.size of accepted CIL messages */
#define I_SDB_MAXIDS       4095        /* Limit to data definitions */

/* Mode for files created by the SDB - 'user', 'group' and 'other' read/write */

#define I_SDB_FILE_MODE    ( S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH )

/*
** Global data declaration/definition/initialisation. See Sdb.h for
** descriptions and ordering of the SDB's TaskData.
*/

E_SDB_EXTERN eSdbTaskData_t  iSdbTaskData[]
#ifdef M_SDB_MAIN
   = {
      { 0,              E_SDB_NO_UNITS },
      { SYS_INIT_STATE, E_SDB_PROCSTATE_UNITS },
      { 0,              E_SDB_AUTH_STATE_UNITS },
      { 0,              E_SDB_SYSREQ_UNITS },
      { 0,              E_SDB_MVERSION_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS },
      { 0,              E_SDB_NO_UNITS }
   }
#endif
;



/*
** SDB basic data storage structure (time/data-type dependant)
*/

struct iSdbEvent_s
{
   struct iSdbEvent_s  *NextPtr; /* Pointer to previous datum in linked-list */
   struct iSdbEvent_s  *PrevPtr; /* Pointer to next datum in linked-list */
   eTtlTime_t        TimeStamp;  /* Time associated with the value */
   Int32_t           Value;      /* */
};
typedef struct iSdbEvent_s iSdbEvent_t;


/*
** SDB basic data element defintion (common for all measurments of that sort)
*/

struct iSdbDefn_s
{
   struct iSdbDefn_s *NextHashPtr; /* Pointer to next item in hash list */ 
   struct iSdbEvent_s *NewestPtr;  /* Ptr to latest item of data linked list */
   struct iSdbEvent_s *OldestPtr;  /* Ptr to earliest item of data lnk'd list */
   struct iSdbEvent_s *LastSubPtr; /* Ptr to last submitted data (not */
                                   /* necessarily the latest (= NewestPtr)) */
   Uint32_t   NumData;       /* Number of data elts in RAM for this defn */
   Int32_t    SourceId;      /* ID of source of datum */
   Int32_t    DatumId;       /* ID number of the datum itself */
   Int32_t    Units;         /* Data Units */
   Bool_t     UnitsRecorded; /* Data Units */
   Bool_t     ValueRecorded; /* Has the latest rx'dvalue been written to file */
   Int32_t    FileIndex;     /* File index number for last place written to */
/* iSdbCode_t Code; */       /* ID code for efficient storage to file */
};
typedef struct iSdbDefn_s iSdbDefn_t;


/*
** SDB file index system
*/

struct iSdbDbFile_s
{
   FILE *FilePtr;            /* File pointer to the file (if open) */
   eTtlTime_t StartTime;     /* Time of the start of the file */
   eTtlTime_t LastAccessed;  /* Time when the file was last accessed */
};
typedef struct iSdbDbFile_s iSdbDbFile_t;

#define I_SDB_MAX_SQLHOST    32        /* Maximum no. chars in MySQL host name */
#define I_SDB_MAX_SQLPORT    32        /* Maximum no. chars in MySQL port */
#define I_SDB_MAX_FILENAME   32        /* Maximum no. chars in DbFile name */
#define I_SDB_MAX_DB_FILES   16        /* Max no. of permitted database files */
#define I_SDB_MAX_FILE_ERRS  5         /* Max fails before changing state */
                                       /* This should be less than the */
                                       /* hbeat-loss timeout */
#define I_SDB_DFLT_CLEANUP   28        /* Default days before file cleanup */

E_SDB_EXTERN iSdbDbFile_t  iSdbDbFileList[ I_SDB_MAX_DB_FILES ] ;


/*
** Global variables
*/

E_SDB_EXTERN Bool_t                 /* Store old data to file, etc. */
   iSdbFileStore E_SDB_INIT(TRUE);
E_SDB_EXTERN Int32_t iSdbCilId;     /* CIL ID number to use */
E_SDB_EXTERN ProcState_t            /* Default "OKAY" state of SDB */
   iSdbNominalState E_SDB_INIT(SYS_OKAY_STATE);           /* ditto */
E_SDB_EXTERN eTtlTime_t
   iSdbHeartBeatTime;               /* Timestamp when the last HB was rx'd */
E_SDB_EXTERN int                    /* Failure counter */
   iSdbNumWriteFails E_SDB_INIT(0);
E_SDB_EXTERN char                   /* Path for data files */
   iSdbDatafilePath[ I_SDB_MAX_FILENAME ];
E_SDB_EXTERN int                    /* Days to determine SDB file cleanup */
   iSdbCleanupDays   E_SDB_INIT( I_SDB_DFLT_CLEANUP );
E_SDB_EXTERN char                   /* MySql hostname */
   iSdbMySqlHost[ I_SDB_MAX_SQLHOST ];
E_SDB_EXTERN char                   /* MySql port */
   iSdbMySqlPort[ I_SDB_MAX_SQLPORT ];
E_SDB_EXTERN Bool_t iSdbSendToSql E_SDB_INIT( FALSE );
E_SDB_EXTERN Bool_t iSdbSqlLoggingOK E_SDB_INIT( TRUE );



/*
** Recovery of raw SDB data from file is handled via a spawned process
** (the SDB File Recovery (SFR) task). The following are required for
** that task.
*/

#define I_SDB_SFR_USE                  /* Define to use file storage? */

#ifdef I_SDB_SFR_USE

#include "Sfr.h"                       /* Path & name of SFR task to spawn */

#ifdef E_WFL_OS_QNX4
#define I_SDB_SFR_PATHNAME    "/opt/ttl/bin/Sfr"  
#else
#define I_SDB_SFR_PATHNAME    "/ttl/sw/bin/Sfr"  
#endif

#ifdef M_SDB_FILERETR
   /* The order of the arguments below IS important */
   /* The numerical arguments are maxima, and will be overwirtten */
   /* Thus, they should represent the maximum width of the string */
   char *iSdbSfrArgv[] = 
   {
      "Sfr",
      "-quiet",
      "-syslog",
      "-debug", "5",
      "-prio", "9",
      "-" E_SFR_SENDTOSTR,   /* Note the string concatenation */
      "99",                  /* This needs to be max.width, not actual value */
      "-" E_SFR_REQUESTSTR,  /* Note the string concatenation */
      "00000000000000000000000000000000000000000000000000000000", /* ditto */
      "-" E_SFR_SEQNUMSTR,   /* Note the string concatenation */
      "1234567890123456",    /* ditto */
      "-" E_SFR_LASTSTR,     /* Note the string concatenation */
      "0",                   /* Data : 0 = first, 1 = last */
      NULL  
   };
#endif  /* End of #ifdef M_SDB_FILERETR */

#endif  /* End of #ifdef I_SDB_SFR_USE */

/*
** Definitions for file management (auto-cleanups). Commands allow the datafile
** path and cleanup period (days) to be added into the string at runtime.
*/

#define I_SDB_CLEAN_UNITSFILE_CMD      "sort -u -o %s" E_SDB_UNITS_FILENAME \
                                        " %s" E_SDB_UNITS_FILENAME
#if defined E_WFL_OS_QNX4

#define I_SDB_CLEAN_DATAFILES_CMD      "find %s -name \'*.sdb' -ftime +%d" \
                                        " -exec rm {}\\;"

#elif defined E_WFL_OS_QNX6

/*
** Base cleanup on modification time of files (-mtime is POSIX but -ftime
** is only QNX4).
*/
#define I_SDB_CLEAN_DATAFILES_CMD      "find %s -name \'*.sdb' -mtime +%d" \
                                        " -exec rm {}\\;"

#else

#define I_SDB_CLEAN_DATAFILES_CMD      "find %s -name \'*.sdb' -mtime +%d" \
                                        " -exec rm {} \\;"
                                        
#endif

/*
** Hash table definitions
*/

#define I_SDB_HASHSIZE 101       /* Hash table size */
#define I_SDB_HASHMOD  31        /* Algorithm key for hash-key generation */

E_SDB_EXTERN iSdbDefn_t  *iSdbHashTable[I_SDB_HASHSIZE];  /* Hash table */


/*
** MySQL functionality enabled 
*/


#define I_SDB_MYSQL_USE 1
/*
** Function prototypes
*/

extern void iSdbReport(char *FormatPtr, ...);
extern void iSdbDisplay(void);
extern void iSdbUsage(char *ExecNamePtr, char *MessagePtr);
extern Status_t iSdbSetup(int argc, char *argv[], eCilMsg_t *MsgPtr);
extern Status_t iSdbProcess(Int32_t DelivererId, eCilMsg_t *MsgPtr);

extern void iSdbSetState(ProcState_t State);
extern Status_t iSdbSafeState(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern void iSdbGoSafe(void);
extern Status_t iSdbActivate(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbShutdown(Int32_t DelivererId, eCilMsg_t *MsgPtr);

extern Status_t iSdbHeartbeat(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbSubmit(Int32_t DelivererId, Bool_t RespondFlag, 
                           eCilMsg_t *MsgPtr);
extern Status_t iSdbAddData(eSdbDatum_t *DatumPtr);
extern Status_t iSdbRetrieve(Bool_t Robust, Int32_t DelivererId, 
                             eCilMsg_t *MsgPtr);
extern iSdbDefn_t *iSdbHashInstall(Int32_t SourceId, Int32_t DatumId);
extern iSdbDefn_t *iSdbHashLookup(Int32_t SourceId, Int32_t DatumId);

extern Status_t iSdbAckReply(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbActReply(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbComReply(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbErrReply(Int32_t DelivererId, eCilMsg_t *MsgPtr,
                             Int32_t ErrCode);

extern Status_t iSdbAutoSubmit(void);
extern Status_t iSdbListSources(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbListData(Int32_t DelivererId, eCilMsg_t *MsgPtr);

extern Status_t iSdbLnkLstCreate(iSdbDefn_t *DefnPtr, iSdbEvent_t *EventPtr);
extern Status_t iSdbLnkLstAppend(iSdbDefn_t *DefnPtr, iSdbEvent_t *EventPtr);
extern Status_t iSdbLnkLstPrepend(iSdbDefn_t *DefnPtr, iSdbEvent_t *EventPtr);
extern Status_t iSdbLnkLstInsert(iSdbDefn_t *DefnPtr,
                   iSdbEvent_t *PrevEventPtr, iSdbEvent_t *NewEventPtr);
extern Status_t iSdbLnkLstClip(iSdbDefn_t *DefnPtr);

extern Status_t iSdbCountSources(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbCountData(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbCountMsrments(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbCountMulReq(eSdbMulReq_t *MulReqPtr);

extern Status_t iSdbMulRetr(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbGetBlock(eSdbMulReq_t *MulReqPtr, char *BufPtr,
                   size_t *BufLenPtr);

extern Status_t iSdbCleanup(void);

extern Status_t iSdbClearSource(Int32_t DelivererId, eCilMsg_t *MsgPtr);
extern Status_t iSdbClearData  (Int32_t DelivererId, eCilMsg_t *MsgPtr);

extern Status_t iSdbStoreUnits(iSdbDefn_t *DefnPtr);

extern Status_t iSdbStorePrevData(iSdbDefn_t *DefnPtr);
extern Status_t iSdbStoreData(Bool_t Force, iSdbDefn_t *DefnPtr);

extern Status_t iSdbFileRetr(Int32_t DelivererId, eCilMsg_t *MsgPtr, 
                             Bool_t LastData);



#endif


/* EOF */
