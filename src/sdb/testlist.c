/*
** Module Name:
**    testlist.c
**
** Purpose:
**    A test program to list the contents of the System Database (SDB).
**
** Description:
**    This test program will list the contents of the System Database
**    (SDB), which is used to hold the engineering data from the 
**    telescope system.
**
**    For the purpose of a package name, this program uses STL.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    mjf: Martyn J. Ford (TTL)
**
** Version:
**    $Id: testlist.c,v 0.27 2005/06/20 13:35:31 mjf Exp $
**
** History:
**    $Log: testlist.c,v $
**    Revision 0.27  2005/06/20 13:35:31  mjf
**    Minor changes to aid porting to Linux - no functional changes.
**
**    Revision 0.26  2004/10/08 12:27:36  mjf
**    SDB_1_02.
**
**    Revision 0.25  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.24  2004/01/16 08:55:04  sxf
**    Minor changes to port to QNX6.
**
**    Revision 0.23  2003/05/30 11:27:54  mjf
**    Updated for Services PLC ICD v3.3 support - needs completion.
**
**    Revision 0.22  2002/08/07 11:33:01  dws
**    Added code to decode SPT status data this is only displayed
**    when -names is specified on commandline
**
**    Revision 0.21  2002/03/22 16:32:10  mjf
**    Run at priority 9 rather than 10.
**
**    Revision 0.20  2001/10/23 11:08:36  mjf
**    Addition of '-lastdata [n]' switch to retrieve the last 'n'
**    measurements from disk for each datum.
**
**    Revision 0.19  2001/07/12 12:16:33  mjf
**    Allow specification of the source datum in hex or decimal.
**
**    Revision 0.18  2001/03/05 10:43:07  mjf
**    Do not write out a log file by default.
**
**    Revision 0.17  2001/02/14 13:53:02  mjf
**    Use HTI library for Header-to-ID conversions in testlist and testinject.
**
**    Revision 0.16  2001/01/23 11:02:33  mjf
**    Show usage if command-line arguments incorrect.
**
**    Revision 0.15  2001/01/22 16:55:22  mjf
**    Use CLU for command-line arguments. Addition of switch to specify the
**    path of the ID lookup tables. Addition of function to filter on a single
**    datum.
**
**    Revision 0.14  2001/01/18 12:14:49  mjf
**    Addition of command-line option '-names' to attempt to display the
**    names of the datums and units.
**
**    Revision 0.13  2000/11/03 16:02:51  djm
**    Added ability to select the CIL ID of the server with which to talk.
**
**    Revision 0.12  2000/10/16 15:33:56  djm
**    Minor bug fix to cope with new LISTSOURCE/LISTDATA SDB commands.
**
**    Revision 0.11  2000/10/02 16:12:33  djm
**    Allow user to specify a specific SourceId from which to disaply data.
**
**    Revision 0.10  2000/08/29 14:37:02  djm
**    Changed default CIL map location.
**
**    Revision 0.9  2000/06/02 16:04:06  djm
**    Added another diagnostic message.
**
**    Revision 0.8  2000/05/31 10:49:49  djm
**    Changed code to make use of TIM package functions.
**
**    Revision 0.7  2000/05/31 08:49:00  djm
**    Fixed output to be less confusing when overwriting old output.
**
**    Revision 0.6  2000/05/30 15:09:27  djm
**    Neatened and corrected output.
**
**    Revision 0.5  2000/05/24 16:13:26  djm
**    Added more stringent checking on message sizes.
**    Also fixed some minor bugs.
**
**    Revision 0.4  2000/05/24 15:37:01  djm
**    Put more stringent checking on received message lengths.
**
**    Revision 0.3  2000/05/23 15:21:42  djm
**    Amended command line arguments, and removed some useless code.
**    Also made some minor changes to output format.
**
**    Revision 0.2  2000/05/23 14:46:17  djm
**    Implemented bulk of program functionality.
**
**    Revision 0.1  2000/05/21 15:27:16  djm
**    Initial creation.
**
**
*/


/* Include files */
#include "TtlSystem.h"
#include "Sdb.h"
#include "Cil.h"
#include "Tim.h"
#include "Clu.h"
#include "Log.h"
#include "Hti.h"

/* Declare the SPT text data for this application */
#define E_SPT_DEVICE_TEXT_DECLARE
#include "Spt.h"


/* Application details - normally found in XxxPrivate.h */

#define I_XXX_PROGRAM_NAME   "testlist"
#define I_XXX_PROGRAM_ABOUT  "List the contents of the SDB"
#define I_XXX_RELEASE_DATE   "20 June 2005"
#define I_XXX_YEAR           "2000-05"
#define I_XXX_MAJOR_VERSION  0
#define I_XXX_MINOR_VERSION  6

/* Common arguments defaults */

#define M_XXX_DFLT_QUIET     FALSE
#define M_XXX_DFLT_VERBOSE   TRUE
#define M_XXX_DFLT_SYSLOG    TRUE
#define M_XXX_DFLT_DEBUG     E_LOG_NOTICE
#define M_XXX_DFLT_PRIORITY  9
#define M_XXX_DFLT_HELP      FALSE
#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_CONFIG    "/opt/ttl/etc/testlist.cfg"
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#define M_STL_DFLT_ID_PATH   "/opt/ttl/etc"   /* Default path for ID tables */
#else
#define M_XXX_DFLT_CONFIG    "/ttl/sw/etc/testlist.cfg"
#define M_XXX_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#define M_STL_DFLT_ID_PATH   "/ttl/sw/etc"   /* Default path for ID tables */
#endif
#define M_XXX_DFLT_LOG       "\0"
#define M_XXX_DFLT_CIL       "TU0"

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_SERVER  0
#define M_XXX_CUSTOM_DELAY   1
#define M_XXX_CUSTOM_NUM     2
#define M_XXX_CUSTOM_CLEAR   3
#define M_XXX_CUSTOM_NOCLEAR 4
#define M_XXX_CUSTOM_SOURCE  5
#define M_XXX_CUSTOM_DATUM   6
#define M_XXX_CUSTOM_NAMES   7
#define M_XXX_CUSTOM_LAST    8

#define M_XXX_CUSTOM_ARGS    9

/* Definitions */

#define M_STL_ID_FILE      "IdTable.idx"    /* ID table file */
#define M_STL_UNIT_FILE    "UnitsTable.idx" /* Units table file */
#define M_STL_DATASIZE     16384      /* Max size of accepted CIL messages */
#define M_STL_TIMEOUT      300        /* CIL Rx timeout (in milliseconds) */
/* NB: the timeout needs to be faster than the CHB heartbeat period */
#define M_STL_DFLT_CIL_ID  E_CIL_TU0  /* Default CIL ID */
#define M_STL_SDB_CIL_ID   E_CIL_SDB  /* ID of database server */
#define M_STL_MAXSRC       256        /* Max no. of IDs to deal with */
#define M_STL_MAXDAT       4096       /* Max no. of IDs to deal with */
#define M_STL_MAXPACKET    256        /* Max number of IDs to retrieve at once */
#define M_STL_NAMELEN      8          /* Max length for a CIL name */
#define M_STL_TIMESTRLEN   24         /* Max lenght for timestamp strings */
#define M_STL_ESC_CHAR     27         /* Escape char for ANSI esc.sequences */
#define M_STL_STRING_LEN   128        /* Maximum length of string for display */
#define M_STL_IDTABLE_STR  "%s %s \"%[^\"]\"" /* Format of entry in ID table */
#define M_STL_IDTABLE_PARAMS 3        /* Number of params in ID table entry */
#define M_STL_STR_UNITS    "_UNITS"   /* Trailing unit name for removal */
                                      /* String for display with sign-on */
#define M_SLT_STR_FIELDS   "Output fields :-\n" \
           " CIL           CIL name\n" \
           " ScID          Source CIL ID\n" \
           " DtId          Datum ID\n" \
           " Unit          Units for data element\n" \
           " Date          Date in format DD/MM/YY (UTC)\n" \
           " Time(UTC)     UTC of data element value\n" \
           " Value(hex)    Hexadecimal representation of data element value\n" \
           " Value(dec)    Decimal representation of data element value\n\n" \


/* Global data */

typedef struct
{
   Int32_t CilId;            /* CIL address ID */
   Int32_t SdbCilId;         /* CIL address ID of the database */
   unsigned int Delay;       /* Pause between each update in milliseconds */
   int NumUpdates;           /* Number of updates to perform before exiting */
   Bool_t ClrScreen;         /* Clear screen between displays */
   Int32_t SourceId;         /* Limited source IDs to display */
   Int32_t DatumId;          /* Limited datum IDs to display */
   Bool_t DisplayNames;      /* Use look-up table to display identifier names */
   Bool_t LastData;          /* To force retrieval of the lastest datum */
   Int32_t LastCount;        /* Number of lastest measurements retrieved */
} mStlCmdLineArgs_t;

mStlCmdLineArgs_t mStlCmdLineArgs
   = {
      M_STL_DFLT_CIL_ID,     /* Default CIL ID */
      M_STL_SDB_CIL_ID,      /* Default servers CIL ID */
      1000,                  /* Default delay */
      1,                     /* Default number of updates */
      FALSE,                 /* Clear screen between displays */
      -1,                    /* Limited source IDs to display */
      -1,                    /* Limited datum IDs to display */
      FALSE,                 /* Don't display names by default */
      FALSE,                 /* No forcing retrieval of the lastest datum */
      5                      /* Default count of last measurements */
   };



#define M_STL_SPLC_ALARMSTATE_NORMAL   0
#define M_STL_SPLC_ALARMSTATE_WARNING  1
#define M_STL_SPLC_ALARMSTATE_STAGE2   2
#define M_STL_SPLC_ALARMSTATE_STAGE3   4
#define M_STL_SPLC_ALARMSTATE_STAGE4   8


/* for parsing custom command-line arguments */



eCluArgSpec_t eCluCustomArg[ M_XXX_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                       FALSE, NULL */ 
{ { "server <name>",   3, "SDB server CIL name",              FALSE, NULL },
  { "delay [msec]",    3, "Delay between updates (msec)",     FALSE, NULL },
  { "num [updates]",   3, "Number of updates before exiting "
                          "(0=continuous)",                   FALSE, NULL },
  { "clear",           3, "Clear terminal on each iteration", FALSE, NULL },
  { "noclear",         3, "Don't clear the terminal",         FALSE, NULL },
  { "source <name>",   3, "Only recover data from CIL name",  FALSE, NULL },
  { "datum [ID]",      3, "Only recover data from datum ID",  FALSE, NULL },
  { "names",           3, "Display datum ID and units names", FALSE, NULL },
  { "lastdata [count]", 3, "Force retrieval of last data",     FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                          FALSE, NULL }
};


/* Other global variables */

Uint32_t mStlSeqNum = 0;     /* CIL message sequence number */
char mStlCilName[E_CIL_EOL][M_STL_NAMELEN];   /* Storage for CIL names */

/* Function prototypes */

Status_t mStlParseArgs(int argc, char *argv[]);
Status_t mStlGetDatumIds(Int32_t SourceId,
                         Uint32_t *NumData, Int32_t *DatListPtr);
Status_t mStlGetSourceIds(Uint32_t *NumSources, Int32_t *SrcListPtr);
Status_t mStlGetAndShow(Int32_t SourceId,
                        Uint32_t NumData, Int32_t *DatListPtr);
Status_t mStlGetLastAndShow( Int32_t SourceId, Int32_t DatumId );
Status_t mStlDisplayProccesorMode(unsigned int ProccessorMode);
Status_t mStlDisplayPlcData(Int32_t Value, Int32_t Units);

#ifdef _DWS_
Status_t mStlDisplayLimitText(mStlSplcLimitDevice_t *LimitDevice);
Status_t mStlDisplayPlcAlarmState(unsigned int AlarmState);
Status_t mStlDisplayTwoStateText(unsigned int DisplayText, unsigned int DeviceStatus, unsigned int DisplayTextSense);
Status_t mStlDisplayFourStateText(unsigned int DisplayText, unsigned int DeviceStatus, unsigned int DisplayTextSense);
#endif

int main(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    Top level function of the "testlist" program.
**
** Description:
**    Top level function of the "SDB lister". It is called on 
**    startup of the executable.
**
** Arguments:
**    int argc                 (in)
**       Number of arguments on the command line (including the
**       executable name).
**    char *argv[]             (in)
**       Array of null-terminated character strings containing
**       the command line arguments.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-Jan-2001 mjf Added use of CLU.
**    23-May-2000 djm Added timestamp decoding.
**    21-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;        /* Function return status variable */

   Uint32_t NumSources;    /* Number of source IDs in the SDB */
   Uint32_t NumData;       /* Number of datum IDs for a give source in SDB */
   int ItrCtr;             /* Loop variable counter - iterations */
   int SrcCtr;             /* Loop variable counter - source IDs */
   Int32_t SrcList[M_STL_MAXSRC];       /* List of source IDs */
   Int32_t DatList[M_STL_MAXDAT];       /* List of datum IDs */
   Int32_t SourceId;       /* ID of source currently being operated with */
   int Id;                 /* CIL ID for name lookup */


   /* Parse command line arguments (CLAs) */
   Status = mStlParseArgs(argc, argv);
   if(Status != SYS_NOMINAL)
   {
      return EXIT_FAILURE;
   }

   /* Do program setup */
   if(mStlCmdLineArgs.ClrScreen == TRUE)
   {
      printf("%c[2J", M_STL_ESC_CHAR);
   }

   Status = eCluSignOn( NULL, NULL );
   if(Status != SYS_NOMINAL)
   {
      printf
      (
         "Error: Failed to sign-on (%d = 0x%x)\n", 
         Status, Status
      );
      exit(EXIT_FAILURE);
   }

   /* attempt to determine the CIL ID */
   Status = eCilLookup( eCluCommon.CilMap, eCluCommon.CilName, 
                        &mStlCmdLineArgs.CilId );

   /* attempt to perform CIL setup now CIL ID has been determined */
   Status = eCilSetup( eCluCommon.CilMap, mStlCmdLineArgs.CilId );

   /* check status returned - report any errors */
   switch ( Status )
   {
      case SYS_NOMINAL :
         /* not an error - successful completion */
         break;

      default :
         printf( "Unable to perform CIL setup : error %X\n", Status );
         exit(EXIT_FAILURE);
         break;
   }

   /* Fill in array of CIL names for rapid access */
   strcpy(mStlCilName[0], "???");
   for(Id = E_CIL_BOL+1; Id < E_CIL_EOL; Id++)
   {
      Status = eCilName( eCluCommon.CilMap, Id, M_STL_NAMELEN, mStlCilName[Id]);
      if(Status != SYS_NOMINAL)
      {
         printf("Error getting name for Id=0x%x\n", Id);
         exit(EXIT_FAILURE);
      }
   }


   /* Main processing loop */ 
   for
   (
      ItrCtr = 0;
      (ItrCtr < mStlCmdLineArgs.NumUpdates) || (mStlCmdLineArgs.NumUpdates == 0);
      ItrCtr++
   )
   {
      /* Pause for pre-specified time, if not doing it for the first time */
      if(ItrCtr != 0)
      {
         sleep( mStlCmdLineArgs.Delay / 1000 );
      }

      if(mStlCmdLineArgs.ClrScreen == TRUE)
      {
         printf("%c[1;1f", M_STL_ESC_CHAR);
      }

      /* Print the title for the data output (if this changes at all, then */
      /* you need to change the "output fields" list in mStlUsage() */
      if ( mStlCmdLineArgs.DisplayNames == FALSE )
      {
         printf( "\n_CIL_ScID_DtID_Unit_"
                 "Date_____Time(UTC)___Val(hex)___Val(dec)_\n" );
      }
      else
      {
         printf( "\n_CIL_Datum______________Unit__________"
                 "Date_____Time(UTC)____Val(hex)_(dec)_\n" );
      }

      /* Get list of sources in SDB */
      if(mStlCmdLineArgs.SourceId == -1)
      {
         Status = mStlGetSourceIds(&NumSources, &SrcList[0]);
         if(Status != SYS_NOMINAL)
         {
            printf("Failed to recover source ID list from SDB%c[K\n",
                   M_STL_ESC_CHAR);
            continue;
         }
      }
      else
      {
         NumSources = 1;
         SrcList[0] = mStlCmdLineArgs.SourceId;
      }


      /* Loop over each source */
      for(SrcCtr = 0; SrcCtr < NumSources; SrcCtr++)
      {
         /* Note the source ID to use */
         SourceId = SrcList[SrcCtr];

         /* If data to be retrieved from all datum IDs */
         if ( mStlCmdLineArgs.DatumId == -1 )
         {
            /* Get list of datum IDs in SDB for the given source */
            Status = mStlGetDatumIds(SourceId, &NumData, &DatList[0]);
            if(Status != SYS_NOMINAL)
            {
               printf("Failed to recover datum ID list from SDB%c[K\n",
                      M_STL_ESC_CHAR);
               continue;
            }
         }
         /* else only a single datum is to be retrieved */
         else
         {
            NumData = 1;
            DatList[ 0 ] = mStlCmdLineArgs.DatumId;
         }

         /* DEBUG - The following call is a prototype for data printing */
         mStlGetAndShow(SourceId, NumData, &DatList[0]);

      }  /* End of for('each source') */

   }  /* End of for('each required interation') */


   /* Terminate the program */
   return EXIT_SUCCESS;

}  /* End of main() */



Status_t mStlParseArgs(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    mStlParseArgs
**
** Type:
**    Status_t
**
** Purpose:
**    Command line argument processing function for the STL program.
**
** Description:
**    Takes the command line arguments (as specified as arguments
**    to the  main() function) and processes them to fill in the
**    global structure.
**
**    NOTE: If the command line argument handling is change, then
**    one must remember to update the mStlUsage() function as well.
**
**    Also note that non-TTL coding standard conventions were used
**    for the arguments argc & argv, as they are "standard" ISO/C
**    usage in this form.
**
** Arguments:
**    int argc                 (in)
**       Number of arguments on the command line (including the
**       executable name). As in main().
**    char *argv[]             (in)
**       Array of null-terminated character strings containing
**       the command line arguments. As in main().
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-Jan-2001 mjf Added use of CLU.
**    02-Oct-2000 djm Added source/datum parsing.
**    23-May-2000 djm Removed the unused "-verbose" and "-quiet" switches.
**    23-May-2000 djm Added "-(no)clear", "-delay" and "-num" switches.
**    21-May-2000 djm Initial creation (adapted from mSdbParseArgs()).
**
*/

   int      NumValues;       /* Number of values read from sscanf() */
   unsigned Delay;           /* Temp delay variable */
   unsigned Iters;           /* Temp variable for number of display updates */
   unsigned Datum;           /* Temp variable for datum to display */
   Int32_t  CilUnit;         /* Some other specified CIL unit */
   char     Switch[ E_CLU_SWITCH_LEN ];/* to report error parsing a switch */
   Status_t Status;          /* Return variable for function calls */
   Status_t CluStatus;       /* Return variable for CLU function calls */



   /* data for command-line utilities */
   eCluProgNamePtr              = I_XXX_PROGRAM_NAME;
   eCluProgAboutPtr             = I_XXX_PROGRAM_ABOUT;
   eCluReleaseDatePtr           = I_XXX_RELEASE_DATE;
   eCluYearPtr                  = I_XXX_YEAR;
   eCluMajorVer                 = I_XXX_MAJOR_VERSION;
   eCluMinorVer                 = I_XXX_MINOR_VERSION;
  
   /* set default parameters before parsing command-line */
   eCluCommon.Quiet             = M_XXX_DFLT_QUIET;
   eCluCommon.Verbose           = M_XXX_DFLT_VERBOSE;
   eCluCommon.Syslog            = M_XXX_DFLT_SYSLOG;
   eCluCommon.DebugLevel        = M_XXX_DFLT_DEBUG;
   eCluCommon.Priority          = M_XXX_DFLT_PRIORITY;
   eCluCommon.Help              = M_XXX_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, M_XXX_DFLT_CONFIG  );
   strcpy( eCluCommon.LogFile,    M_XXX_DFLT_LOG     );
   strcpy( eCluCommon.CilName,    M_XXX_DFLT_CIL     );
   strcpy( eCluCommon.CilMap,     M_XXX_DFLT_CIL_MAP );

   /* register with the command-line utilities (CLU), ignore CIL map */
   CluStatus = eCluSetup( argc, argv, 
                          E_CLU_IGN_QUIET | E_CLU_IGN_VERBOSE 
                           | E_CLU_IGN_SYSLOG | E_CLU_IGN_NOSYSLOG
                           | E_CLU_IGN_DEBUG | E_CLU_IGN_PRIORITY
                           | E_CLU_IGN_CONFIGFILE | E_CLU_IGN_LOGFILE );
   
   /* if setup was successful */
   if ( CluStatus == SYS_NOMINAL )
   {
      /* parse the incoming command-line */
      CluStatus = eCluParseArgs( Switch );

      /* if command-line parsed successfully */
      if ( CluStatus == SYS_NOMINAL )
      {  
         /* check for help requested */
         if ( eCluCommon.Help == TRUE )
         {
            /* output info about displayed fields */
            printf( M_SLT_STR_FIELDS );
            /* exit the program */
            exit( EXIT_SUCCESS );
         }

         /* check for alternate SDB server */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_SERVER ) == E_CLU_ARG_SUPPLIED )
         {
            /* try to look alternate server up */
            Status = eCilLookup( eCluCommon.CilMap, 
                                 eCluGetCustomParam( M_XXX_CUSTOM_SERVER ), 
                                 &CilUnit );
            if( Status != SYS_NOMINAL )
            {
               printf( "CIL name \"%s\" not recognised\n", 
                       eCluGetCustomParam( M_XXX_CUSTOM_SERVER ) );
               return E_SDB_CLA_UNKNOWN;
            }
            /* If we get this far, alls well. We'll try to use this CIL ID */
            mStlCmdLineArgs.SdbCilId = CilUnit;
            printf( "Using server CIL ID %d (= 0x%2.2x, %s)\n",
                    CilUnit, CilUnit, 
                    eCluGetCustomParam( M_XXX_CUSTOM_SERVER ) );
         }

         /* check for delay */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_DELAY ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to recover it */
            NumValues = sscanf( eCluGetCustomParam( M_XXX_CUSTOM_DELAY ), 
                                "%u", &Delay );
            if( NumValues == 1 )
            {
               mStlCmdLineArgs.Delay = Delay;
            }
            else
            {
               printf( "Delay value \"%s\" is not valid\n", 
                       eCluGetCustomParam( M_XXX_CUSTOM_DELAY ) );
               return E_SDB_CLA_UNKNOWN;
            }
         }

         /* check for number of updates */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_NUM ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to recover it */
            NumValues = sscanf( eCluGetCustomParam( M_XXX_CUSTOM_NUM ), 
                                "%u", &Iters );
            if( NumValues == 1 )
            {
               mStlCmdLineArgs.NumUpdates = Iters;
            }
            else
            {
               printf( "Number of updates, \"%s\", is not valid\n", 
                       eCluGetCustomParam( M_XXX_CUSTOM_NUM ) );
               return E_SDB_CLA_UNKNOWN;
            }
         }

         /* check for existence of clear */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_CLEAR ) == E_CLU_ARG_SUPPLIED )
         {
            mStlCmdLineArgs.ClrScreen = TRUE;
         }

         /* check for existence of noclear */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_NOCLEAR ) == E_CLU_ARG_SUPPLIED )
         {
            mStlCmdLineArgs.ClrScreen = FALSE;
         }

         /* check for existence of source */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_SOURCE ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to look it up */
            Status = eCilLookup( eCluCommon.CilMap, 
                                 eCluGetCustomParam( M_XXX_CUSTOM_SOURCE ), 
                                 &CilUnit );
            if( Status != SYS_NOMINAL )
            {
               printf( "CIL name \"%s\" not recognised\n", 
                       eCluGetCustomParam( M_XXX_CUSTOM_SOURCE ) );
               return E_SDB_CLA_UNKNOWN;
            }
            /* If we get this far, alls well. We'll try to use this CIL ID */
            mStlCmdLineArgs.SourceId = CilUnit;
            printf( "Using a single Source ID %d (= 0x%2.2x, %s)\n",
                    CilUnit, CilUnit, 
                    eCluGetCustomParam( M_XXX_CUSTOM_SOURCE ) );
         }

         /* check for existence of datum */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_DATUM ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to recover it */
            Datum = strtol( eCluGetCustomParam( M_XXX_CUSTOM_DATUM ), NULL, 0 );
            mStlCmdLineArgs.DatumId = Datum;
            printf( "Using a single datum ID %d (= 0x%2.2x)\n", Datum, Datum );
         }

         /* check for existence of names */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_NAMES ) == E_CLU_ARG_SUPPLIED )
         {
            mStlCmdLineArgs.DisplayNames = TRUE;
         }

         /* check for existence of ID path */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_LAST ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to recover it */
            mStlCmdLineArgs.LastCount 
               = strtol( eCluGetCustomParam( M_XXX_CUSTOM_LAST ), NULL, 0 );
            mStlCmdLineArgs.LastData = TRUE;
         }
      }
      /* else command-line not parsed successfully */
      else
      {
         eCluShowUsage( NULL, NULL );
         printf( "Error parsing the command-line switch '%s'\n", Switch );
         return CluStatus;
      }
   }
   /* else command-line not parsed successfully */
   else
   {
      printf( "Error parsing the command-line\n" );
      return CluStatus;
   }

   /* Terminate the function and return success */
   return SYS_NOMINAL;

}  /* End of mStlParseArgs() */


Status_t mStlGetSourceIds(
   Uint32_t *NumSourcesPtr,
   Int32_t *SrcListPtr
)
/*
** Function Name:
**    mStlGetSourceIds
**
** Type:
**    Status_t
**
** Purpose:
**    Create list of source IDs by interrogating the SDB.
**
** Description:
**    This function uses a CIL message to ask the SDB for a
**    list of all the sources that have submitted data to the
**    SDB since it was first run.
**
** Arguments:
**    Uint32_t *NumSourcesPtr  (out)
**       Pointer to a variable containing the number of sources
**       that are in the SDB.
**    Int32_t *SrcListPtr      (out)
**       A pointer to an array of source IDs.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-May-2000 djm Changed variable names to fit TTL coding standard.
**    19-May-2000 djm Initial creation.
**
*/
{
   Status_t Status;          /* Function call return value */
   char *DataBufPtr;         /* Buffer to hold CIL message data */
   char *TmpBufPtr;          /* Temporary pointer to buffer */
   eCilMsg_t Msg;            /* CIL message structure for Tx/Rx */
   Int32_t DelivererId;      /* CIL ID of the deliverer of a Rx'd CIL msg */
   Int32_t SrcCtr;           /* Loop variable counter for source IDs */
   Int32_t SourceId;         /* Temporary storage for a source ID */


   *NumSourcesPtr = 0;

   /* Allocate a memory buffer */
   DataBufPtr = TTL_MALLOC(M_STL_DATASIZE);
   if(DataBufPtr == NULL)
   {
      printf("Unable to allocate memory\n");
      return SYS_ERROR_NON_FATAL;
   }


   /* Set the buffer size variable to max.capacity */
   Msg.SourceId = mStlCmdLineArgs.CilId;
   Msg.DestId = mStlCmdLineArgs.SdbCilId;
   Msg.Service = E_SDB_LISTSOURCES;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.SeqNum = ++mStlSeqNum;
   Msg.TimeStamp.t_sec = 0;
   Msg.TimeStamp.t_nsec = 0;
   Msg.DataPtr = DataBufPtr;
   Msg.DataLen = 0;

   /* Send CIL message */
   Status = eCilSend(mStlCmdLineArgs.SdbCilId, &Msg);
   if(Status != SYS_NOMINAL)
   {
      printf("Unable to send source list request (0x%x)\n", Status);
      return Status;
   }

   /* Block, waiting for an inbound message */
   while(
      ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStlSeqNum)) &&
      (Status != E_CIL_TIMEOUT)
   )
   {
      Msg.DataLen = M_STL_DATASIZE;
      Status = eCilReceive(M_STL_TIMEOUT, &DelivererId, &Msg);
   }

/* DEBUG - TEST DIAGNOSTIC CODE */
/*
{
int n;
printf("--- DataLen = %d\n", Msg.DataLen);
printf("Rx (0x%2.2x, %d): ", Msg.SourceId, Msg.Service);
for(n = 0; n < Msg.DataLen; n++)
  printf("%2.2x ", ((char *)Msg.DataPtr)[n]);
printf("\n");
}
*/

   /* Check the CIL receive for a timeout result */
   if(Status == E_CIL_TIMEOUT)
   {
      printf("Timeout waiting for list of source IDs%c[K\n", M_STL_ESC_CHAR);
      return Status;
   }

   /* Check successful CIL reply for other errors */
   if(Status != SYS_NOMINAL)
   {
      printf("CIL receive error for source ID list (0x%x)\n");
      return Status;
   }


   /* Check the returned message length */
   if(Msg.DataLen < sizeof(*NumSourcesPtr))
   {
      TTL_FREE(DataBufPtr);
      printf(
         "Message truncated (%d of >=%d expected)\n",
         Msg.DataLen, sizeof(*NumSourcesPtr)
      );
      return SYS_ERROR_NON_FATAL;
   }

   /* Read out the number of returned source IDs */
   TmpBufPtr = DataBufPtr;
   memcpy(NumSourcesPtr, TmpBufPtr, sizeof(*NumSourcesPtr));
   *NumSourcesPtr = ntohl(*NumSourcesPtr);
   TmpBufPtr += sizeof(*NumSourcesPtr);


   /* Check the overall message length */
   if(Msg.DataLen != 
      sizeof(*NumSourcesPtr) + (*NumSourcesPtr * sizeof(SourceId)))
   {
      TTL_FREE(DataBufPtr);
      *NumSourcesPtr = 0;
      printf("Overall message length error\n");
      return SYS_ERROR_NON_FATAL;
   }

   /* Work through the message, putting these IDs into our list */
   for(SrcCtr = 0; SrcCtr < *NumSourcesPtr; SrcCtr++)
   {
      memcpy(&SourceId, TmpBufPtr, sizeof(SourceId));
      SourceId = ntohl(SourceId);
      TmpBufPtr += sizeof(SourceId);
      SrcListPtr[SrcCtr] = SourceId;
   }


   TTL_FREE(DataBufPtr);
   return Status;

}  /* End of mStlGetSourceIds() */




Status_t mStlGetDatumIds(
   Int32_t SourceId,
   Uint32_t *NumDataPtr,
   Int32_t *DatListPtr
)
/*
** Function Name:
**    mStlGetSourceIds
**
** Type:
**    Status_t
**
** Purpose:
**    Create list of datum IDs for a given source by asking the SDB.
**
** Description:
**    This function uses a CIL message to ask the SDB for a
**    list of all the datum IDs that have been submitted to the
**    SDB since it was first run for a specified source ID.
**
** Arguments:
**    Uint32_t *NumDataPtr  (out)
**       Pointer to a variable containing the number of datum IDs
**       that are in the SDB for the specified source ID.
**    Int32_t *DatListPtr      (out)
**       A pointer to an array of datum IDs.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-May-2000 djm Changed some variable names to match coding standard.
**    19-May-2000 djm Initial creation.
**
*/
{
   Status_t Status;          /* Function call return value */
   char *DataBufPtr;         /* Buffer to hold CIL message data */
   char *TmpBufPtr;          /* Temporary pointer to buffer */
   eCilMsg_t Msg;            /* CIL message structure for Tx/Rx */
   Int32_t DelivererId;      /* CIL ID of the deliverer of a Rx'd CIL msg */
   Int32_t DatCtr;           /* Loop variable counter for datum IDs */
   Int32_t DatumId;          /* Temporary storage for a datum ID */
   Int32_t TmpSourceId;      /* Temporary storage for a source ID */

   *NumDataPtr = 0;

   /* Allocate a memory buffer */
   DataBufPtr = TTL_MALLOC(M_STL_DATASIZE);
   if(DataBufPtr == NULL)
   {
      printf("Unable to allocate memory\n");
      return SYS_ERROR_NON_FATAL;
   }


   /* Fill in CIL message header */
   Msg.SourceId = mStlCmdLineArgs.CilId;
   Msg.DestId = mStlCmdLineArgs.SdbCilId;
   Msg.Service = E_SDB_LISTDATA;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.SeqNum = ++mStlSeqNum;
   Msg.TimeStamp.t_sec = 0;
   Msg.TimeStamp.t_nsec = 0;
   Msg.DataPtr = DataBufPtr;

   /* Fill in CIL message body. Note that can't use Source ID */
   /* directly because it is a constant argument */
   TmpSourceId = htonl(SourceId);
   memcpy(DataBufPtr, &TmpSourceId, sizeof(TmpSourceId));
   Msg.DataLen = sizeof(TmpSourceId);


   /* Send CIL message */
   Status = eCilSend(mStlCmdLineArgs.SdbCilId, &Msg);
   if(Status != SYS_NOMINAL)
   {
      printf("Unable to send datum list request (0x%x)\n", Status);
      return Status;
   }

   /* Block, waiting for an inbound message */
   while(
      ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStlSeqNum)) &&
      (Status != E_CIL_TIMEOUT)
   )
   {
      Msg.DataLen = M_STL_DATASIZE;
      Status = eCilReceive(M_STL_TIMEOUT, &DelivererId, &Msg);
   }


   /* Check the CIL receive for a timeout result */
   if(Status == E_CIL_TIMEOUT)
   {
      printf("Timeout waiting for list of datum IDs%c[K\n", M_STL_ESC_CHAR);
      return Status;
   }

   /* Check successful CIL reply for other errors */
   if(Status != SYS_NOMINAL)
   {
      printf("CIL receive error for datum ID list (0x%x)\n");
      return Status;
   }

   /* Check the returned message length is sufficient to hold NumDataPtr */
   if(Msg.DataLen < sizeof(*NumDataPtr))
   {
      TTL_FREE(DataBufPtr);
      printf("Data return message truncated\n");
      return SYS_ERROR_NON_FATAL;
   }

   /* Read out the number of returned data IDs */
   TmpBufPtr = DataBufPtr;
   memcpy(NumDataPtr, TmpBufPtr, sizeof(*NumDataPtr));
   *NumDataPtr = ntohl(*NumDataPtr);
   TmpBufPtr += sizeof(*NumDataPtr);


   /* Check the overall message length */
   if(Msg.DataLen != sizeof(*NumDataPtr) + (*NumDataPtr * sizeof(DatumId)))
   {
      TTL_FREE(DataBufPtr);
      *NumDataPtr = 0;
      printf("Message length error\n");
      return SYS_ERROR_NON_FATAL;
   }

   /* Work through the message, putting these IDs into our list */
   for(DatCtr = 0; DatCtr < *NumDataPtr; DatCtr++)
   {
      memcpy(&DatumId, TmpBufPtr, sizeof(DatumId));
      DatumId = ntohl(DatumId);
      TmpBufPtr += sizeof(DatumId);
      DatListPtr[DatCtr] = DatumId;
   }


   TTL_FREE(DataBufPtr);
   return Status;

}  /* End of mStlGetDatumIds() */




Status_t mStlGetAndShow
(
   Int32_t SourceId,
   Uint32_t NumData,
   Int32_t *DatListPtr 
)
/*
** Function Name:
**    mStlGetSourceIds
**
** Type:
**    Status_t
**
** Purpose:
**    Create list of datum IDs for a given source by asking the SDB.
**
** Description:
**    This function uses a CIL message to ask the SDB for a
**    list of all the datum IDs that have been submitted to the
**    SDB since it was first run for a specified source ID.
**
** Arguments:
**    Int32_t SourceId       (in)
**       The source Id to operate for.
**    Uint32_t NumData       (in)
**       A variable containing the number of datum IDs
**       that are in the SDB for the specified source ID.
**    Int32_t *DatListPtr      (in)
**       A pointer to an array of datum IDs.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    31-May-2000 djm Altered to make use of new TIM functions.
**    23-May-2000 djm Changed date format in output.
**    23-May-2000 djm Changed output format.
**    22-May-2000 djm Initial creation.
**
*/
{
   Status_t Status;          /* Function call return value */
   char *DataBufPtr;         /* Buffer to hold CIL message data */
   char *TmpBufPtr;          /* Temporary pointer to buffer */
   eCilMsg_t Msg;            /* CIL message structure for Tx/Rx */
   Int32_t DelivererId;      /* CIL ID of the deliverer of a Rx'd CIL msg */
   int DtmCtr;               /* Loop variable counter - datum IDs */
   Uint32_t PacketTally;     /* Number of Datum IDs in a packet for retrieval */
   Uint32_t DatumLeft;       /* Number of Datum IDs left to retrieve */ 
   Uint32_t DatumRetrieved;  /* Number of Datum IDs retrieved */ 
   Uint32_t NboNumData;      /* Number of Datum IDs in Network Byte-Order */
   Uint32_t TmpNumData;      /* Number of Datum IDs in hardware Byte-Order */
   Int32_t NboSourceId;      /* Source ID in Network Byte-Order */
   Int32_t NboDatumId;       /* Datum ID in Network Byte-Order */
   eSdbDatum_t Datum;        /* Data element extracted from message */
   Int32_t ProcessId;        /* CIL process ID */
   char TimeStr[M_STL_TIMESTRLEN];    /* String for date/time */
   char DatumStr[ M_STL_STRING_LEN ]; /* Datum string for display */
   char UnitsStr[ M_STL_STRING_LEN ]; /* Units string for display */


   /* Formulate CIL message to request data */


   /* Allocate a memory buffer */
   DataBufPtr = TTL_MALLOC(M_STL_DATASIZE);
   if(DataBufPtr == NULL)
   {
      printf(
         "Unable to allocate memory while processing source 0x%x\n",
         SourceId
      );
      return SYS_ERROR_NON_FATAL;
   }

   /* Retrieve data in packets */
   DatumLeft = NumData;
   DatumRetrieved = 0;
   while ( DatumLeft > 0 )
   {
      if ( DatumLeft > M_STL_MAXPACKET )
      {
         PacketTally = M_STL_MAXPACKET;
      }
      else
      {
         PacketTally = DatumLeft;
      }

      /* Fill in CIL message header */
      Msg.SourceId = mStlCmdLineArgs.CilId;
      Msg.DestId = mStlCmdLineArgs.SdbCilId;
      Msg.Service = E_SDB_RETRIEVE_1;
      Msg.Class = E_CIL_CMD_CLASS;
      Msg.SeqNum = ++mStlSeqNum;
      Msg.TimeStamp.t_sec = 0;
      Msg.TimeStamp.t_nsec = 0;
      Msg.DataPtr = DataBufPtr;
      Msg.DataLen = sizeof(PacketTally) + sizeof(eSdbSngReq_t) * PacketTally;

      /* Construct the CIL message body */
      TmpBufPtr = DataBufPtr;
      NboNumData = htonl(PacketTally);
      memcpy(TmpBufPtr, &NboNumData, sizeof(NboNumData));
      TmpBufPtr += sizeof(NboNumData);

      /* Put the source/datum ID pairs into the message */
      NboSourceId = htonl(SourceId);
      for(DtmCtr = DatumRetrieved; 
          DtmCtr < ( DatumRetrieved + PacketTally ); 
          DtmCtr++)
      {
         memcpy(TmpBufPtr, &NboSourceId, sizeof(NboSourceId));
         TmpBufPtr += sizeof(NboSourceId);
         NboDatumId = htonl(DatListPtr[DtmCtr]);
         memcpy(TmpBufPtr, &NboDatumId, sizeof(NboDatumId));
         TmpBufPtr += sizeof(NboDatumId);
      }

      /* Send the CIL message */
      Status = eCilSend(mStlCmdLineArgs.SdbCilId, &Msg);
      if(Status != SYS_NOMINAL)
      {
         printf(
         "Unable to send datum list request for source 0x%x (Status=0x%x)\n",
            SourceId, Status
         );
         return Status;
      }

      /* Block, waiting for an inbound message */
      while(
         ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStlSeqNum)) &&
         (Status != E_CIL_TIMEOUT)
      )
      {
         Msg.DataLen = M_STL_DATASIZE;
         Status = eCilReceive(M_STL_TIMEOUT, &DelivererId, &Msg);
      }

      if(Status != SYS_NOMINAL)
      {
         printf(
         "Failed to retrieve data for source 0x%x from the SDB (Status=0x%x)\n",
            SourceId, Status
         );
         return Status;
      }

      /* Make a copy of the data buffer pointer */
      TmpBufPtr = Msg.DataPtr;

      /* Extract the number of data submitted from the message */
      memcpy(&NboNumData, TmpBufPtr, sizeof(NboNumData));
      TmpNumData = ntohl(NboNumData);
      TmpBufPtr += sizeof(TmpNumData);

      /* Check the message length against the calculated message length */
      /* based on the number of contained data elements */
      if(Msg.DataLen != (sizeof(TmpNumData) + (TmpNumData * sizeof(eSdbDatum_t))) )
      {
         printf(
             "Incorrect ammount of data for source 0x%x "
             "(%d of %d expected data bytes received)",
             SourceId, Msg.DataLen,
             sizeof(TmpNumData) + (TmpNumData * sizeof(eSdbDatum_t))
         );
      }   

      /* Reduce the number of remaining datum IDs */
      DatumLeft -= PacketTally;
      DatumRetrieved += PacketTally;
   
      /* Work through the message, printing data as we go */
      for(DtmCtr = 0; DtmCtr < TmpNumData; DtmCtr++)
      {
         /* Extract all the components of one data element */
         memcpy(&Datum, TmpBufPtr, sizeof(Datum));
         TmpBufPtr += sizeof(Datum);
   
         /* Convert from network to hardware byte order */
         Datum.SourceId = ntohl(Datum.SourceId);
         Datum.DatumId = ntohl(Datum.DatumId);
         Datum.Units = ntohl(Datum.Units);
         Datum.Msrment.Value = ntohl(Datum.Msrment.Value);
         Datum.Msrment.TimeStamp.t_sec = ntohl(Datum.Msrment.TimeStamp.t_sec);
         Datum.Msrment.TimeStamp.t_nsec = ntohl(Datum.Msrment.TimeStamp.t_nsec);
         
         ProcessId = Datum.SourceId;
         if((ProcessId <= E_CIL_BOL) || (ProcessId >= E_CIL_EOL))
         {
            ProcessId = 0;
         }
   
         Status = eTimToString(&Datum.Msrment.TimeStamp, 
                               M_STL_TIMESTRLEN, TimeStr);
         if(Status != SYS_NOMINAL)
         {
            return Status;
         }
   
         if ( mStlCmdLineArgs.DisplayNames == FALSE )
         {
            printf(
               " %s %4.4x %4.4x  %2.2x  %s  %8.8x = %d%c[K\n",
               mStlCilName[ProcessId], Datum.SourceId, Datum.DatumId,
               Datum.Units,
               TimeStr,
               Datum.Msrment.Value,
               Datum.Msrment.Value,
               M_STL_ESC_CHAR
            );
         }
         else
         {
            printf(
               " %s %-18.18s %-13.13s %s %8.8x %d%c[K\n",
               mStlCilName[ProcessId], 
               eHtiGetDataString( Datum.SourceId, Datum.DatumId, DatumStr ),
               eHtiGetUnitString( Datum.Units, UnitsStr ),
               TimeStr,
               Datum.Msrment.Value,
               Datum.Msrment.Value,
               M_STL_ESC_CHAR
              );
               if( Datum.SourceId == E_CIL_SPT )
               {
                 mStlDisplayPlcData(Datum.Msrment.Value, Datum.Units);
               }
         }
   
         /* If the 'last data' flag has been specified */
         if ( mStlCmdLineArgs.LastData == TRUE )
         {
            printf( "Start of archived measurements...\n" );
            /* Show the last few measurements retrieved from disk */
            mStlGetLastAndShow( Datum.SourceId, Datum.DatumId );
            printf( "...end of archived measurements\n" );
         }
   
      }  /* End of for('each datum') */

   }

   /* If we get this far, all worked. Return success */
   TTL_FREE(DataBufPtr);
   return SYS_NOMINAL;

}  /* End of mStlGetAndShow() */


Status_t mStlGetLastAndShow( Int32_t SourceId, Int32_t DatumId )
{
   Status_t Status;          /* Function call return value */
   char *DataBufPtr;         /* Buffer to hold CIL message data */
   char *TmpBufPtr;          /* Temporary pointer to buffer */
   eCilMsg_t Msg;            /* CIL message structure for Tx/Rx */
   Int32_t DelivererId;      /* CIL ID of the deliverer of a Rx'd CIL msg */
   Int32_t Units;            /* Units of retrieved data */
   Int32_t NumMsrments;      /* Number of measurements retrieved */
   int     Msrment;          /* Counter for looping around measurements */
   eSdbMulReq_t MulReq;      /* To request data from SDB */
   eSdbMsrment_t * MsrmentPtr; /* Pointer to a retrieved measurement */
   char TimeStr[M_STL_TIMESTRLEN];    /* String for date/time */
   char DatumStr[ M_STL_STRING_LEN ]; /* Datum string for display */
   char UnitsStr[ M_STL_STRING_LEN ]; /* Units string for display */


   /* Allocate a memory buffer */
   DataBufPtr = TTL_MALLOC(M_STL_DATASIZE);
   if(DataBufPtr == NULL)
   {
      printf(
         "Unable to allocate memory while processing source 0x%x\n",
         SourceId
      );
      return SYS_ERROR_NON_FATAL;
   }

   /* Formulate CIL message to request data */


   /* Fill in CIL message header */
   Msg.SourceId = mStlCmdLineArgs.CilId;
   Msg.DestId = mStlCmdLineArgs.SdbCilId;
   Msg.Service = E_SDB_RETRIEVE_L;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.SeqNum = ++mStlSeqNum;
   Msg.TimeStamp.t_sec = 0;
   Msg.TimeStamp.t_nsec = 0;
   Msg.DataPtr = &MulReq;
   Msg.DataLen = sizeof( MulReq );

   /* Construct the CIL message body */
   MulReq.SourceId    = SourceId;
   MulReq.DatumId     = DatumId;
   MulReq.NumMsrments = mStlCmdLineArgs.LastCount;
   MulReq.OldestTime.t_sec = 0;
   MulReq.OldestTime.t_nsec = 0;
   eTimGetTime( &MulReq.NewestTime );


   /* Send the CIL message */
   eCilConvert32bitArray( Msg.DataLen, Msg.DataPtr );
   Status = eCilSend(mStlCmdLineArgs.SdbCilId, &Msg);
   if(Status != SYS_NOMINAL)
   {
      printf(
         "Unable to send retrieve (L) request for source 0x%x (Status=0x%x)\n",
         SourceId, Status
      );
      return Status;
   }

   /* Block, waiting for an inbound message */
   while(
      ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStlSeqNum)) &&
      (Status != E_CIL_TIMEOUT)
   )
   {
      /* Adjust the receive buffer */
      Msg.DataPtr = DataBufPtr;
      Msg.DataLen = M_STL_DATASIZE;
      /* Allow 100 times as long as normal, due to the disk access required */
      Status = eCilReceive(M_STL_TIMEOUT * 100, &DelivererId, &Msg);
   }

   if(Status != SYS_NOMINAL)
   {
      printf(
         "Failed to retrieve (L) data for source 0x%x from the SDB (Status=0x%x)\n",
         SourceId, Status
      );
      return Status;
   }

   /* Convert the received buffer to host byte-order */
   eCilConvert32bitArray( Msg.DataLen, Msg.DataPtr );
 
   /* Make a copy of the data buffer pointer */
   TmpBufPtr = Msg.DataPtr;

   /* First item in message is the source ID - skip */
   TmpBufPtr += sizeof( SourceId );

   /* Next item in message is the datum ID - skip */
   TmpBufPtr += sizeof( DatumId );

   /* Next item in message is the units */
   memcpy( &Units, TmpBufPtr, sizeof( Units ) );
   TmpBufPtr += sizeof( Units );

   /* Next item in message is the number of measurement */
   memcpy( &NumMsrments, TmpBufPtr, sizeof( NumMsrments ) );
   TmpBufPtr += sizeof( NumMsrments );


   /* Work through the message, printing data as we go */
   for( Msrment = 0; Msrment < NumMsrments; Msrment++ )
   {
      /* Point to the measurement */
      MsrmentPtr = ( eSdbMsrment_t * ) TmpBufPtr;

      Status = eTimToString( &MsrmentPtr->TimeStamp, 
                             M_STL_TIMESTRLEN, TimeStr );

      if ( mStlCmdLineArgs.DisplayNames == FALSE )
      {
         printf(
            " %s %4.4x %4.4x  %2.2x  %s  %8.8x = %d%c[K\n",
            mStlCilName[ SourceId], SourceId, DatumId,
            Units,
            TimeStr,
            MsrmentPtr->Value,
            MsrmentPtr->Value,
            M_STL_ESC_CHAR
            
         );
      }
      else
      {
         printf(
            " %s %-18.18s %-13.13s %s %8.8x %d%c[K\n",
            mStlCilName[ SourceId ], 
            eHtiGetDataString( SourceId, DatumId, DatumStr ),
            eHtiGetUnitString( Units, UnitsStr ),
            TimeStr,
            MsrmentPtr->Value,
            MsrmentPtr->Value,
            M_STL_ESC_CHAR
            );
         if( SourceId == E_CIL_SPT )
         {
            mStlDisplayPlcData(MsrmentPtr->Value, Units);
         }
      }

      /* Increment the measurement pointer */
      TmpBufPtr += sizeof( eSdbMsrment_t );
   }

   TTL_FREE(DataBufPtr);

   return SYS_NOMINAL;

}

/*******************************************************************************
** Function Name :
**    mStlDisplayPlcData
**
** Purpose :
**    To decode the words sent by the services plc
**
** Description :
**    
**
** Return Type:
**    Status_t
**
**   
** Arguments :
**    Int32_t Value
**       Contains the outputted value from the unit
**    
**    Int32_t Units
**       Identifies which unit the value comes from
** Authors :
**    Dan W. Smith(dws)
**
**
*******************************************************************************/ 

char * mStlPlcText( Int32_t Value, Int32_t Units, int DisplayText );
char * mStlPlcMasked( Int32_t Units, int Masked );
char * mStlPlcFault( Int32_t Value );
int mStlGetBitNumber( Int32_t Value );

Status_t mStlDisplayPlcData(Int32_t Value, Int32_t Units)
{  
   eSptDevice2State_t *pDevice2State;
   eSptDevice4State_t *pDevice4State;
   eSptDeviceLimit_t  *pDeviceLimit;
   eSptDeviceSensor_t *pDeviceSensor;
   eSptDeviceMotor_t  *pDeviceMotor;      
   eSptDeviceActuator_t *pDeviceActuator;      
   eSptDevicePLCRevStat_t *pDevicePLCRevStat;
   eSptDevicePLCRevNum_t *pDevicePLCRevNum;
   eSptDevicePLCStatus_t *pDevicePLCStatus;

   switch( Units )
   {
      case E_SDB_PLC_2_STATE_UNITS :
      
         pDevice2State = ( eSptDevice2State_t * ) &Value;
         printf( "   %-14s %-6s %-8s\n", 
                 mStlPlcText( pDevice2State->DeviceStatus, Units, pDevice2State->DisplayText ),
                 mStlPlcMasked( Units, pDevice2State->Masked ),
                 mStlPlcFault( Value ) );
         break;

      case E_SDB_PLC_4_STATE_UNITS :
      
         pDevice4State = ( eSptDevice4State_t * ) &Value;
         printf( "   %-14s %-6s %-8s\n", 
                 mStlPlcText( pDevice4State->DeviceStatus, Units, pDevice4State->DisplayText ),
                 mStlPlcMasked( Units, pDevice4State->Masked ),
                 mStlPlcFault( Value ) );
         break;
         
      case E_SDB_PLC_LIMIT_UNITS :
      
         pDeviceLimit = ( eSptDeviceLimit_t * ) &Value;
         break;
         
      case E_SDB_PLC_SENSOR_UNITS :
      
         pDeviceSensor = ( eSptDeviceSensor_t * ) &Value;
         break;

      case E_SDB_PLC_MOTOR_UNITS :

         pDeviceMotor = ( eSptDeviceMotor_t * ) &Value;
         break;

      case E_SDB_PLC_ACTUATOR_UNITS :

         pDeviceActuator = ( eSptDeviceActuator_t * ) &Value;
         break;
         
      case E_SDB_PLC_STATUS_UNITS :
      
         pDevicePLCStatus = ( eSptDevicePLCStatus_t * ) &Value;
         break;
         
      case E_SDB_PLC_VERSION_UNITS :
      
         pDevicePLCRevStat = pDevicePLCRevStat;
         pDevicePLCRevNum  = pDevicePLCRevNum;
         break;

      default :

         break;
   }

   return SYS_NOMINAL;
}

char * mStlPlcText( Int32_t Value, Int32_t Units, int DisplayText )
{
   static char Buffer[ 32 ];

   switch ( eSptDeviceSpec[ Units - E_SDB_PLC_BOL_UNITS ].TextIndex )
   {
      /* Index using text and value reported */
      case E_SPT_INDEX_TEXT_VALUE :
         strcpy( Buffer, eSptDeviceSpec[ Units - E_SDB_PLC_BOL_UNITS ].Spec[ DisplayText ][ Value ].DeviceText );
         break;

      /* Index using text and bit number */
      case E_SPT_INDEX_TEXT_BIT :
         strcpy( Buffer, 
                 eSptDeviceSpec[ Units - E_SDB_PLC_BOL_UNITS ].Spec[ DisplayText ][ mStlGetBitNumber( Value ) ].DeviceText );
         break;

      /* Index using bit and value reported */
      case E_SPT_INDEX_BIT_VALUE :
         strcpy( Buffer, 
                 eSptDeviceSpec[ Units - E_SDB_PLC_BOL_UNITS ].Spec[ mStlGetBitNumber( Value ) ][ Value ].DeviceText );
         break;

      /* Index using value and bit number */
      case E_SPT_INDEX_VALUE_BIT :
         strcpy( Buffer, 
                 eSptDeviceSpec[ Units - E_SDB_PLC_BOL_UNITS ].Spec[ Value ][ mStlGetBitNumber( Value ) ].DeviceText );
         break;

      /* Index using value and text */
      case E_SPT_INDEX_VALUE_TEXT :
         strcpy( Buffer, 
                 eSptDeviceSpec[ Units - E_SDB_PLC_BOL_UNITS ].Spec[ Value ][ DisplayText ].DeviceText );
         break;

      default :
         sprintf( Buffer, "???" );
         break;
   }

   return Buffer;
}

char * mStlPlcMasked( Int32_t Units, int Masked )
{
   static char Buffer[ 32 ];

   switch ( Units )
   {
      case E_SDB_PLC_LIMIT_UNITS :






         break;

      default :
         if ( Masked )
         {
            strcpy( Buffer, "MASKED" );
         }
         else
         {
            strcpy( Buffer, "" );
         }
         break;
   }

   return Buffer;
}

char * mStlPlcFault( Int32_t Value )
{
   eSptDeviceGeneral_t * pGeneral;
   static char Buffer[ 32 ];

   pGeneral = ( eSptDeviceGeneral_t * ) &Value;

   sprintf( Buffer, "%2s%2s%2s%2s", 
            pGeneral->Warning     ? " 1" : "  ",
            pGeneral->Stage2Alarm ? " 2" : "  ",
            pGeneral->Stage3Alarm ? " 3" : "  ",
            pGeneral->Stage4Alarm ? " 4" : "  " );

   return Buffer;
}

int mStlGetBitNumber( Value )
{
   int Mask;
   int BitNumber;

   BitNumber = 0;

   do
   {
      Mask = ( 1 << BitNumber );
      if ( Mask & Value )
      {
         return BitNumber;
      }
      BitNumber++;
   }
   while ( BitNumber < E_SPT_TEXT_COLS );

   return BitNumber;
}



        

#ifdef _DWS_




   /*
   ** The value will be overlayed onto one of the structures below this is determined by the  Unit that 
   ** The Value is from
   */
   mStlSplcTwoStateDevice_t *TwoStateDevice; /* Pointer to a two state device structure  */
   mStlSplcFourStateDevice_t *FourStateDevice; /* Pointer to Four state device strucutre */
   mStlSplcLimitDevice_t  *LimitDevice; /* Pointer to a limit device structue */
   mStlSplcSensor_t *SensorDevice; /* Pointer to a Sensor device structure */
   mStlSplcMotor_t *MotorDevice; /* Pointer to a Motor device strucutre */
   mStlSplcActuator_t *ActuatorDevice; /* Ponter to an Actuator device structure */
   mStlSplcPlcStatus_t *StatusDevice; /* Pointer to an Plc Status device structure */


   E_SDB_PLC_BOL_UNITS,      /* PLC device units beginning of list */
   E_SDB_PLC_2_STATE_UNITS,  /* PLC 2-state device */
   E_SDB_PLC_4_STATE_UNITS,  /* PLC 4-state device */
   E_SDB_PLC_LIMIT_UNITS,    /* PLC Limit device */
   E_SDB_PLC_SENSOR_UNITS,   /* PLC Sensor device */
   E_SDB_PLC_MOTOR_UNITS,    /* PLC Motor device */
   E_SDB_PLC_ACTUATOR_UNITS, /* PLC Actuator device */
   E_SDB_PLC_STATUS_UNITS,   /* PLC status */
   E_SDB_PLC_VERSION_UNITS,  /* PLC version */
   E_SDB_PLC_EOL_UNITS,      /* PLC device units end of list */
 


   switch(Units)
   {
      case E_SDB_PLC_2_STATE_UNITS:
      {
            /*
            ** Overlay Value onto TwoStatedevice 
            */
            TwoStateDevice  = (mStlSplcTwoStateDevice_t *) &Value;

            if( TwoStateDevice->DeviceStatus == 0)
            {
               printf("%6s", "False");
            }
            else if(TwoStateDevice->DeviceStatus == 1)
            {
               printf("%6s", "True");
            }
          


            if( TwoStateDevice->IsMasked == 0)
            {
               printf(" %10s","Active");
            }
            else if(TwoStateDevice->IsMasked == 1)
            {
               printf("%10s","Masked");
            }
           mStlDisplayTwoStateText(TwoStateDevice->DisplayText, TwoStateDevice->DeviceStatus, TwoStateDevice->DisplayTextSense);
           mStlDisplayPlcAlarmState(TwoStateDevice->AlarmState);
           printf("\n");
         }
         break;
      case  E_SDB_PLC_4_STATE_UNITS:
         {
            /*
            **Overlay Value onto FourStateDevice
            */
            FourStateDevice = (mStlSplcFourStateDevice_t *) &Value;

            printf("%5i", FourStateDevice->DeviceStatus);

            if(FourStateDevice->IsMasked == 0)
            {
               printf("%10s", "Active");
            }
            else if(FourStateDevice->IsMasked == 1)
            {
               printf("10s", "Masked");
            }

            mStlDisplayFourStateText(FourStateDevice->DisplayText, FourStateDevice->DeviceStatus, FourStateDevice->DisplayTextSense);
            mStlDisplayPlcAlarmState(FourStateDevice->AlarmState);
            printf("\n");
         }
         break;
      case E_SDB_PLC_LIMIT_UNITS:
         {
            LimitDevice =(mStlSplcLimitDevice_t *) &Value; 

            if(LimitDevice->LThreeA == 0)
            {
               printf("%5s", "L3AN");
            }
            else if(LimitDevice->LThreeA == 1)
            {
               printf("%5s","L3AT");
            }
            
                        
            if(LimitDevice->LTwoA == 0)
            {
               printf("%5s", "L2AN");
            }
            else if(LimitDevice->LTwoA == 1)
            {
               printf("%5s", "L2AT");
            }

            if(LimitDevice->LThreeB == 0)
            {
               printf("%5s", "L3BN");
            }
            else if(LimitDevice->LTwoB == 1)
            {
               printf("%5s", "L3BT");
            }

            if(LimitDevice->LTwoB == 0)
            {
               printf("%5s", "L2BN");
            }
            else if(LimitDevice->LTwoB == 1)
            {
               printf("%5s", "L2BT");
            }

            mStlDisplayLimitText(LimitDevice);
            mStlDisplayPlcAlarmState(LimitDevice->AlarmState);
            printf("\n");
         }
      case E_SDB_PLC_SENSOR_UNITS:
         {
            SensorDevice = (mStlSplcSensor_t *) &Value;
             
            if(SensorDevice->IsLowLow == 0)
            {
               printf("%8s", "LowLow");
            }
            else if(SensorDevice ->IsLowLow == 1)
            {
               printf("%5s", "Norm");
            }
            
            if(SensorDevice->IsLow == 0)
            {
               printf("%8s", "Low");
            }
            else if(SensorDevice ->IsLow == 1)
            {
               printf("%5s", "Norm");
            }
            
            if(SensorDevice->IsHigh == 0)
            {
               printf("%8s", "High");
            }
            else if(SensorDevice ->IsLowLow == 1)
            {
               printf("%5s", "Norm");
            }
            
            if(SensorDevice->IsHighHigh == 0)
            {
               printf("%9s", "HighHigh");
            }
            else if(SensorDevice ->IsHighHigh == 1)
            {
               printf("%5s", "Norm");
            }
            
            if(SensorDevice->LowLowMasked == 0)
            {
               printf("%8s", "LowLowM");
            }
            else if(SensorDevice ->LowLowMasked == 1)
            {
               printf("%5s", "LowLowA");
            }
            
            
            if(SensorDevice->LowMasked == 0)
            {
               printf("%8s", "LowM");
            }
            else if(SensorDevice ->LowMasked == 1)
            {
               printf("%5s", "LowA");
            }
            
            if(SensorDevice->HighMasked == 0)
            {
               printf("%8s", "HighM");
            }
            else if(SensorDevice ->IsLowLow == 1)
            {
               printf("%5s", "HighA");
            }
            
            if(SensorDevice->HighHighMasked == 0)
            {
               printf("%10s", "HighHighM");
            }
            else if(SensorDevice ->IsHighHigh == 1)
            {
               printf("%10s", "HighHighA");
            }

            if(SensorDevice->IsInValid == 0)
            {
               printf("%2s","I");
            }
            else if(SensorDevice->IsInValid == 1)
            {
               printf("%2s","V");
            }

            mStlDisplayPlcAlarmState(SensorDevice->AlarmState);
            printf("\n");
         }
         break;
   case E_SDB_PLC_MOTOR_UNITS:
       { 

            MotorDevice = (mStlSplcMotor_t *) &Value;

            if(MotorDevice->IsRunning == 0)
            {
               printf("%10s","Stopped");
            }
            else if(MotorDevice->IsRunning == 1)
            {
               printf("%10s", "Running");
            }
            
            if(MotorDevice->IsTripped == 0)
            {
               printf("%10s","Healthy");
            }
            else if(MotorDevice->IsTripped == 1)
            {
               printf("%10s", "Tripped");
            }

            if(MotorDevice->IsFaulty == 0)
            {
               printf("%10s","Normal");
            }
            else if(MotorDevice->IsFaulty == 1)
            {
               printf("%10s", "Fault");
            }
            
            if(MotorDevice->IsInterlocked == 0)
            {
               printf("%10s","Normal");
            }
            else if(MotorDevice->IsInterlocked == 1)
            {
               printf("%10s", "Interlocked");
            }

            if(MotorDevice->IsMasked == 0)
            {
               printf("%10s","Active");
            }
            else if(MotorDevice->IsMasked == 1)
            {
               printf("%10s", "Masked");
            }

            if(MotorDevice->IsInvalid == 0)
            {
               printf("%3s","V");
            }
            else if(MotorDevice->IsInvalid == 1)
            {
               printf("%3s", "I");
            }
            mStlDisplayPlcAlarmState(MotorDevice->AlarmState);
            printf("\n");
       }
       break;
   case E_SDB_PLC_ACTUATOR_UNITS:
       {
            ActuatorDevice = (mStlSplcActuator_t *) &Value;
            
            if(ActuatorDevice->IsOpened == 0)
            {
               printf("%8s","NotO");
            }
            else if(ActuatorDevice->IsInvalid == 1)
            {
               printf("%8s", "Opened");
            }
            
            if(ActuatorDevice->IsClosed == 0)
            {
               printf("%8s","NotC");
            }
            else if(ActuatorDevice->IsInvalid == 1)
            {
               printf("%8s", "Closed");
            }

            if(ActuatorDevice->InTransit == 0)
            {
               printf("%8s","NotTransit");
            }
            else if(ActuatorDevice->InTransit == 1)
            {
               printf("%8s", "Transit");
            }
                       
            if(ActuatorDevice->IsTripped == 0)
            {
               printf("%10s","Healthy");
            }
            else if(ActuatorDevice->IsTripped == 1)
            {
               printf("%10s", "Tripped");
            }

            if(ActuatorDevice->IsFaulty == 0)
            {
               printf("%10s","Normal");
            }
            else if(ActuatorDevice->IsFaulty == 1)
            {
               printf("%10s", "Fault");
            }
            
            if(ActuatorDevice->IsInterlocked == 0)
            {
               printf("%10s","Normal");
            }
            else if(ActuatorDevice->IsInterlocked == 1)
            {
               printf("%10s", "Interlocked");
            }

            if(ActuatorDevice->IsMasked == 0)
            {
               printf("%8s","Active");
            }
            else if(ActuatorDevice->IsMasked== 1)
            {
               printf("%8s", "Masked");
            }
            
            if(ActuatorDevice->IsInvalid == 0)
            {
               printf("%3s","I");
            }
            else if(ActuatorDevice->IsInvalid == 1)
            {
               printf("3s", "V");
            }

            mStlDisplayPlcAlarmState(ActuatorDevice->AlarmState);
            printf("\n");
       }
       break;
   case E_SDB_PLC_STATUS_UNITS:
       {
            StatusDevice = (mStlSplcPlcStatus_t *) &Value;
            mStlDisplayProccesorMode(StatusDevice->ProccessorMode);
            
            if(StatusDevice->ForcesEnabled == 0)
            {
               printf("%3s","FD");
            }
            else if(StatusDevice->ForcesEnabled == 1)
            {
               printf("3s", "FE");
            }

            if(StatusDevice->ForcesPresent == 0)
            {
               printf("%3s","NF");
            }
            else if(StatusDevice->ForcesPresent == 1)
            {
               printf("3s", "FP");
            }

            if(StatusDevice->IsFaulty == 0)
            {
               printf("%10s","Healthy");
            }
            else if(StatusDevice->IsFaulty == 1)
            {
               printf("%10s", "MajorErr");
            }

            if(StatusDevice->IsProgrammable == 0)
            {
               printf("%3s","AP");
            }
            else if(StatusDevice->IsProgrammable == 1)
            {
               printf("%3s", "AD");
            }
       }
       break;
     


   }
   return(SYS_NOMINAL);
}

/*******
*
*
*
********/
Status_t mStlDisplayProccesorMode(unsigned int ProccessorMode)
{

   switch(ProccessorMode)
   {
      case 0:
         printf("%5s","Rdl");
         break;
      case 1:
         printf("%5s","Rpm");
         break;
      case 3:
         printf("%5s","S");
         break;
      case 6:
         printf("%5s","Rr");
         break;
      case 7:
         printf("%5s","Rtc");
         break;
      case 8:
         printf("%6s","Rtssc");
         break;
      case 9:
         printf("%6s","Rtsst");
         break;
      case 16:
         printf("%%5s","Dip");
         break;
      case 17:
         printf("%5s","Pm");
         break;
      case 27:
         printf("%5s","S");
         break;
      case 30:
         printf("%5s","R");
         break;
      default:
         printf("%5s","Err");
   }

return(SYS_NOMINAL);
}


/*******************************************************************************
** Function Name :
**    mStlDisplayLimitText
**
** Purpose :
**    To display the Text relating to an LimitDevice
**
** Description :
**    Uses a case statement to determine which What limit text to display this
**    also takes into account which limit has been triggered.
**
** Return Type:
**    Status_t
**
**   
** Arguments :
**    
**    mStlSplcLimitDevice_t *LimitDevice
**       A pointer to a limit device structure.           
**
**
** Authors :
**    Dan W. Smith(dws)
**
**
*******************************************************************************/ 
Status_t mStlDisplayLimitText(mStlSplcLimitDevice_t *LimitDevice)
{
   switch(LimitDevice->DisplayText)
   {
      case 0:
         {
            if((LimitDevice->LThreeA == 1) || (LimitDevice->LTwoA == 1))
            {
               printf("%11s", "Clockwise");
            }
            else if((LimitDevice->LThreeB == 1) || (LimitDevice->LTwoB == 1))
            {
               printf("%11s", "Anticwise");
            }
          }
         break;
      case 1:
         {
            if((LimitDevice->LThreeA == 1) || (LimitDevice->LTwoA == 1))
            {
               printf("%11s", "Low");
            }
            else if((LimitDevice->LThreeB == 1) || (LimitDevice->LTwoB == 1))
            {
               printf("%11s", "High");
            }
          }
         break;
      case 2:
         {
            if((LimitDevice->LThreeA == 1) || (LimitDevice->LTwoA == 1))
            {
               printf("%11s", "Utravel");
            }
            else if((LimitDevice->LThreeB == 1) || (LimitDevice->LTwoB == 1))
            {
               printf("%11s", "OTravel");
            }
          }
         break;
      case 3:
         {
            if((LimitDevice->LThreeA == 1) || (LimitDevice->LTwoA == 1))
            {
               printf("%11s", "Zentih");
            }
            else if((LimitDevice->LThreeB == 1) || (LimitDevice->LTwoB == 1))
            {
               printf("%11s", "Horizon");
            }
          }
         break;
       default:
         {
            printf("%11s", "ERROR");
         }
         break;
   }
return(SYS_NOMINAL);
}
/*******************************************************************************
** Function Name :
**    mStlDisplayPlcAlarmState
**
** Purpose :
**    To display the alarm state of the PLC unit
**
** Description :
**    Uses a case statement to determine which alarm state to output
**
** Return Type:
**    Status_t
**
**   
** Arguments :
**    
**    unsigned int AlarmState
**         The numberical value that represents the  mutually exclusive 4 bit field 
**         of the state of the plc device
**
** Authors :
**    Dan W. Smith(dws)
**
**
*******************************************************************************/  
Status_t mStlDisplayPlcAlarmState(unsigned int AlarmState)
{



   switch(AlarmState)
      {
         case M_STL_SPLC_ALARMSTATE_NORMAL:
               {
                  printf(" %4s", "N");
               }
            break;
         case M_STL_SPLC_ALARMSTATE_WARNING:
               {
                  printf(" %4s","W");
               }
         case M_STL_SPLC_ALARMSTATE_STAGE2:
               {
                  printf(" %4s","S2");
               }
            break;
         case M_STL_SPLC_ALARMSTATE_STAGE3:
               {
                  printf(" %4s","S3");
               }           
            break;
         case M_STL_SPLC_ALARMSTATE_STAGE4:
               {
                  printf(" %4s", "S4");
               }
            break;
         default:
               {
                  printf(" %4s", "ERRR");
               }
            break;
      }
 return(SYS_NOMINAL);
 }


/*******************************************************************************
** Function Name :
**    mStlDisplayTwoStateText
**
** Purpose :
**    To display the Text  of the PLC unit
**
** Description :
**    Uses a case statement to determine what text to output this is determined by 
**    the Display text 4 bit field and also by the Single bit DeviceStatus and 
**    DisplayTextSense
**     
**    
**
** Return Type:
**    Status_t
**
**   
** Arguments :
**    unsigned int DisplayText
**       The 4 bit Field that determines what text to display   
**
**    unsigned int DeviceStatus
**        The single bit that determines the DeviceStatus 
**
**   unsigned int DisplayTextSense
**       The single bit that determines the Text sense
**
** Author :
**    Dan W. Smith(dws)
**
**
*******************************************************************************/  
Status_t mStlDisplayTwoStateText(unsigned int DisplayText, unsigned int DeviceStatus, unsigned int DisplayTextSense)
{
 

   switch(DisplayText)
   {   
      case  0:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Safe");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Active");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Active");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Safe");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
               printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 1:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "False");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "True");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "True");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "False");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
               printf( "%10i", DisplayText);
            }
            
         }
         break;                  
      case 2:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Not Ok");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Ok");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Ok");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Not Ok");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;         
      case 3:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Off");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "On");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "On");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Off");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;                  
      case 4:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Stopped");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Running");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Running");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Stopped");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;                  
      case 5:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Closed");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Open");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Open");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Closed");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 6:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Fault");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Healthy");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Healthy");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Fault");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
     case 7:
      {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Standby");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Standby");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 8:    
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Assist");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Assit");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 9:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Manual");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Auto");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Auto");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Manual");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 10:   
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "local");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Remote");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Remote");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Local");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 11:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Invalid");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Valid");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Valid");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Invalid");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 12:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Normal");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "OutRange");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "OutRange");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Normal");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 13:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Normal");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Override");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Override");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Normal");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 14:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Normal");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Alarm");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Alarm");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Normal");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      case 15:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Clear");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Blocked");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Blocked");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Clear");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else
            {
              printf( "%10i", DisplayText);
            }
            
         }
         break;
      default:
         printf("%10i", DisplayText);
   }
return(SYS_NOMINAL);
}
/*******************************************************************************
** Function Name :
**    mStlDisplayFourStateText
**
** Purpose :
**    To display the Text  of the PLC unit
**
** Description :
**    Uses a case statement to determine what text to output this is determined by 
**    the Display text 4 bit field and also by the 2 bit field  DeviceStatus and 
**    single bitDisplayTextSense
**     
**    
**
** Return Type:
**    Status_t
**
**   
** Arguments :
**    unsigned int DisplayText
**       The 4 bit Field that determines what text to display   
**
**    unsigned int DeviceStatus
**        The 2 bit field  that determines the DeviceStatus 
**
**   unsigned int DisplayTextSense
**       The single bit that determines the Text sense
**
** Author :
**    Dan W. Smith(dws)
**
**
*******************************************************************************/ 

Status_t mStlDisplayFourStateText(unsigned int DisplayText, unsigned int DeviceStatus, unsigned int DisplayTextSense)

{
   switch(DisplayText)
   {   
      case  0:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Off");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "On");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "On");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Off");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Healthy");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Fault");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  1:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Stopped");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Running");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Running");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Stopped");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Healthy");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Fault");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  2:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Closed");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Open");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Open");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Closed");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Transit");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Fault");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  3:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Closed");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Open");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Open");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Closed");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Transit");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Override");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  4:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "StandyBy");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "StandBy");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Off");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Invalid");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  5:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty1");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty2");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty2");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty1");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Off");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Invalid");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }break;
      case  6:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty1");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty2");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty2");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty1");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Duty3");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Invalid");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  7:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Assist");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Assist");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Off");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Invalid");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  8:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Assist");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Duty1");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Duty1");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Assist");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Transit");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Fault");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  9:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Manual");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Auto");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Auto");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Manual");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Off");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Invalid");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  10:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Local");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Remote");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Remote");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Local");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Off");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Invalid");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
         break;
      case  11:
         {      
            if( DeviceStatus == 0)
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Normal");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Override");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
            }
            else if(DeviceStatus == 1 )
            {
               if(DisplayTextSense == 0)
               {
                  printf("%10s", "Override");
               }
               else if(DisplayTextSense == 1)
               {
                  printf("%10s", "Normal");
               }
               else
               {
                  printf("%10i", DisplayText);
               }
               
            }
            else if(DeviceStatus == 2)
            {
               printf("10s","Healthy");
            }
            else if(DeviceStatus == 3)
            {
               printf("10s","Fault");
            }
            else
            {
              printf( "%10i", DisplayText); 
            }
         }
      break;
      default:
         printf("%10i", DisplayText);


   }
return(SYS_NOMINAL);
}         

#endif

/* EOF */
