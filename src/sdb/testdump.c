/*
** Module Name:
**    testdump.c
**
** Purpose:
**    A test program to dump the contents of an SDB storage file to stdout.
**
** Description:
**    This test program will list the contents of any System Database
**    (SDB) storage file (*.sdb), printing the information to standard
**    output.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testdump.c,v 0.12 2004/04/06 14:02:59 mjf Exp $
**
** History:
**    $Log: testdump.c,v $
**    Revision 0.12  2004/04/06 14:02:59  mjf
**    Fix to return type for 'main'.
**
**    Revision 0.11  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.10  2004/01/16 08:55:04  sxf
**    Minor changes to port to QNX6.
**
**    Revision 0.9  2002/04/18 14:50:12  djm
**    Added the use of the HTI lookup function.
**
**    Revision 0.8  2002/03/22 16:32:10  mjf
**    Run at priority 9 rather than 10.
**
**    Revision 0.7  2001/07/12 12:16:33  mjf
**    Allow specification of the source datum in hex or decimal.
**
**    Revision 0.6  2001/03/16 14:31:44  mjf
**    Additional command-line switches to filter on source and/or datum.
**
**    Revision 0.5  2001/01/23 11:03:05  mjf
**    Addition of use of CLU - note change to command-line args. The program
**    can optionally output the CIL and data ID names.
**
**    Revision 0.4  2000/08/24 11:50:17  djm
**    Converted to make use of the globalised functions in SdbCode.c.
**
**    Revision 0.3  2000/08/22 09:16:10  djm
**    Fix to cope with change to raw format typedef change.
**
**    Revision 0.2  2000/07/21 14:19:55  djm
**    Altered program to cope with the "file start time". Also improved
**    the output format.
**
**    Revision 0.1  2000/07/17 09:41:22  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "TtlSystem.h"
#include "Wfl.h"
#include "TtlConstants.h"
#include "Sdb.h"
#include "SdbPrivate.h"
#include "Cil.h"
#include "Tim.h"
#include "Clu.h"
#include "Log.h"
#include "Hti.h"


/* Application details - normally found in XxxPrivate.h */

#define I_XXX_PROGRAM_NAME   "testdump"
#define I_XXX_PROGRAM_ABOUT  "View the contents of an SDB file"
#define I_XXX_RELEASE_DATE   "22 March 2002"
#define I_XXX_YEAR           "2000-02"
#define I_XXX_MAJOR_VERSION  0
#define I_XXX_MINOR_VERSION  3

/* Common arguments defaults */

#define M_XXX_DFLT_QUIET     FALSE
#define M_XXX_DFLT_VERBOSE   TRUE
#define M_XXX_DFLT_SYSLOG    TRUE
#define M_XXX_DFLT_DEBUG     E_LOG_NOTICE
#define M_XXX_DFLT_PRIORITY  9
#define M_XXX_DFLT_HELP      FALSE
#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_CONFIG    "/opt/ttl/etc/testdump.cfg"
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#define M_XXX_DFLT_ID_PATH   "/opt/ttl/etc" /* Default path for ID tables */
#else
#define M_XXX_DFLT_CONFIG    "/ttl/sw/etc/testdump.cfg"
#define M_XXX_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#define M_XXX_DFLT_ID_PATH   "/ttl/sw/etc" /* Default path for ID tables */
#endif
#define M_XXX_DFLT_LOG       "testdump.txt"
#define M_XXX_DFLT_CIL       "TU0"

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_FILE    0
#define M_XXX_CUSTOM_SOURCE  1
#define M_XXX_CUSTOM_DATUM   2
#define M_XXX_CUSTOM_NAMES   3
#define M_XXX_CUSTOM_IDPATH  4

#define M_XXX_CUSTOM_ARGS    5

/* Definitions */
#define M_XXX_BUFSIZE       16   /* Size of temporary storage buffer */
#define M_XXX_FILE_HDR_SIZE  4   /* Expected size of a storage file header */
#define M_XXX_STRING_LEN   128   /* Maximum size of a string */
#define M_XXX_IDTABLE_STR    "%s %s \"%[^\"]\"" /* Format of ID table entry */
#define M_XXX_IDTABLE_PARAMS 3        /* Number of params in ID table entry */
#define M_XXX_ID_FILE        "IdTable.idx"    /* ID table file */
#define M_XXX_UNIT_FILE      "UnitsTable.idx" /* Units table file */


/* for parsing custom command-line arguments */

eCluArgSpec_t eCluCustomArg[ M_XXX_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                       FALSE, NULL */ 
{ { "file <filename>", 1, "Name of SDB file to read",         FALSE, NULL },
  { "source <name>",   3, "Only recover data from CIL name",  FALSE, NULL },
  { "datum [ID]",      3, "Only recover data from datum ID",  FALSE, NULL },
  { "names",           3, "Display datum ID and units names", FALSE, NULL },
  { "idpath <path>",   3, "Alternate path for names index",   FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                          FALSE, NULL }
};

/* Other global variables */

Int32_t SingleSource;     /* Display only one source (otherwise E_CIL_BOL) */
int SingleDatum;          /* Display only one datum (otherwise 0) */
Bool_t DisplayNames;      /* Use look-up table to display identifier names */
char IdPath[ M_XXX_STRING_LEN ]; /* Path for ID lookup tables */
char CilName[ E_CIL_EOL ] [ E_CIL_IDLEN ];   /* Storage for CIL names */


/* Function prototypes */

char *   mXxxGetIdName       ( char * IndexFilename, Int32_t Process, 
                               Int32_t Value, char * Buffer );


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
**    void
**
** Purpose:
**    Top level function of the "testdump" program.
**
** Description:
**    Top level function of the "SDB file dump". It is called on 
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
**    17-Jul-2000 djm Added block reading.
**    14-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   unsigned Datum;           /* Temp variable for datum to display */
   FILE *FilePtr;            /* Pointer to an SDB storage file */
   eSdbRawFmt_t Store;       /* Structure for storage format */
   char Buf[M_XXX_BUFSIZE];  /* Dummy buffer */
   size_t NumBytes = 0;      /* Number of bytes read from a fread() */
   size_t TotBytes = 0;      /* Total number of bytes read from file */
   size_t NumRecords = 0;    /* Number of record read back from file */
   size_t TotRecords = 0;    /* Total number of record read back from file */
   eTtlTime_t TimeStamp;     /* Derived timestamp of datum being extracted */
   eTtlTime_t TimeOffset;    /* Offset of datum from file start time*/ 
   eTtlTime_t TimeHour;      /* Timestamp of file start time */
   char TimeStr[E_TIM_BUFFER_LENGTH];   /* String to hold time output */
   eSdbSngReq_t Req;         /* Data defintion for temporary storage */
   Status_t Status;          /* Return value from called functions */
   Status_t CluStatus;       /* Return value from called CLU functions */
   Int32_t Id;               /* For initialising the CIL names */
   char     Switch[ E_CLU_SWITCH_LEN ];/* to report error parsing a switch */
   char DatumStr[ M_XXX_STRING_LEN ]; /* Datum string for display */

   /* defaults */
   SingleSource = E_CIL_BOL; 
   SingleDatum  = 0;
   DisplayNames = FALSE;
   strcpy( IdPath, M_XXX_DFLT_ID_PATH );

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

   /* register with the command-line utilities (CLU), ignore all except help */
   CluStatus = eCluSetup( argc, argv, 
                          E_CLU_IGN_QUIET | E_CLU_IGN_VERBOSE 
                           | E_CLU_IGN_SYSLOG | E_CLU_IGN_NOSYSLOG
                           | E_CLU_IGN_DEBUG | E_CLU_IGN_PRIORITY
                           | E_CLU_IGN_CONFIGFILE | E_CLU_IGN_LOGFILE 
                           | E_CLU_IGN_CIL_NAME );

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
            /* exit the program */
            exit( EXIT_SUCCESS );
         }

         /* if filename unspecified */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_FILE ) != E_CLU_ARG_SUPPLIED )
         {
            /* output the usage */
            eCluShowUsage( NULL, NULL );
            /* exit the program */
            exit( EXIT_SUCCESS );
         }

         /* check for existence of source */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_SOURCE ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to look it up */
            Status = eCilLookup( eCluCommon.CilMap, 
                                 eCluGetCustomParam( M_XXX_CUSTOM_SOURCE ), 
                                 &Id );
            if( Status != SYS_NOMINAL )
            {
               printf( "CIL name \"%s\" not recognised\n", 
                       eCluGetCustomParam( M_XXX_CUSTOM_SOURCE ) );
               exit( EXIT_FAILURE );
            }
            /* If we get this far, alls well. We'll try to use this CIL ID */
            SingleSource = Id;
            printf( "Using a single Source ID %d (= 0x%2.2x, %s)\n",
                    Id, Id, 
                    eCluGetCustomParam( M_XXX_CUSTOM_SOURCE ) );
         }

         /* check for existence of datum */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_DATUM ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to recover it */
            Datum = strtol( eCluGetCustomParam( M_XXX_CUSTOM_DATUM ), NULL, 0 );
            SingleDatum = Datum;
            printf( "Using a single datum ID %d (= 0x%2.2x)\n", Datum, Datum );
         }

         /* check for existence of names */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_NAMES ) == E_CLU_ARG_SUPPLIED )
         {
            DisplayNames = TRUE;
         }

         /* check for existence of ID path */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_IDPATH ) == E_CLU_ARG_SUPPLIED )
         {
            strcpy( IdPath, eCluGetCustomParam( M_XXX_CUSTOM_IDPATH) );
         }
      }
      /* else command-line not parsed successfully */
      else
      {
         eCluShowUsage( NULL, NULL );
         printf( "Error parsing the command-line switch '%s'\n", Switch );
         /* exit the program */
         exit( EXIT_SUCCESS );
      }
   }
   /* else command-line not parsed successfully */
   else
   {
      printf( "Error parsing the command-line\n" );
      /* exit the program */
      exit( EXIT_SUCCESS );
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

   /* If names to be displayed */
   if ( DisplayNames == TRUE )
   {
      /* Fill in array of CIL names for rapid access */
      strcpy( CilName[ 0 ], "???" );
      for( Id = E_CIL_BOL + 1; Id < E_CIL_EOL; Id++ )
      {
         Status = eCilName( eCluCommon.CilMap, Id, E_CIL_IDLEN, CilName[ Id ] );
         if( Status != SYS_NOMINAL )
         {
            printf( "Error getting CIL name for Id=0x%x\n", Id );
            exit(EXIT_FAILURE);
         }
      }
   }

   /* Open the file for reading */
   FilePtr = fopen(eCluGetCustomParam( M_XXX_CUSTOM_FILE ), "rb");
   if(FilePtr == NULL)
   {
      printf("Error: unable to open file '%s'\n", 
             eCluGetCustomParam( M_XXX_CUSTOM_FILE ));
      eCluShowUsage( NULL, NULL );
      exit( EXIT_FAILURE );
   }
   
   /* Read the header */
   NumBytes = fread(Buf, sizeof(char), M_XXX_FILE_HDR_SIZE, FilePtr);
   if(NumBytes != M_XXX_FILE_HDR_SIZE)
   {
      printf("Error: only %d bytes read from header\n", NumBytes);
      exit( EXIT_FAILURE );
   }
   TotBytes += NumBytes;


   /* Get the file "time-stamp", from which all other times are offset. */
   NumRecords = fread(&(TimeHour.t_sec), sizeof(TimeHour.t_sec), 1, FilePtr);
   if(NumRecords != 1)
   {
      printf("Error: couldn't get file timestamp\n");
      exit( EXIT_FAILURE );
   }
   TotBytes += NumRecords * sizeof(TimeHour.t_sec);
   TimeHour.t_nsec = 0;


   /* Loop through printing all the data */
   NumRecords = 1;
   while(NumRecords != 0)
   {
      NumRecords = fread(&Store, sizeof(Store), 1, FilePtr);
      if(NumRecords == 1)
      {
         TotRecords++;
         TotBytes += NumRecords * sizeof(Store);
      }


      /* Convert the raw storage format into "SDB" components */
      Status = eSdbStoreIdDecode(&(Store.Code), &Req);

      TimeOffset.t_sec = Store.TimeOffset / E_TTL_MICROSECS_PER_SEC;
      TimeOffset.t_nsec = (Store.TimeOffset % (long)E_TTL_MICROSECS_PER_SEC) *
                          ((long)E_TTL_NANOSECS_PER_SEC / (long)E_TTL_MICROSECS_PER_SEC);
      Status = eTimSum(&TimeHour, &TimeOffset, &TimeStamp);
      Status = eTimToString(&TimeStamp, E_TIM_BUFFER_LENGTH, TimeStr);

      /* If no source filter, or source filter matches */
      if ( ( SingleSource == E_CIL_BOL ) || ( SingleSource == Req.SourceId ) )
      {
         /* If no datum filter, or datum filter matches */
         if ( ( SingleDatum == 0 ) || ( SingleDatum == Req.DatumId ) )
         {
            /* Print the details to standard output */
            if ( DisplayNames == FALSE )
            {
               printf(
                  "%s (0x%2.2x,0x%2.2x) = %8.8x %d\n",
                  TimeStr, Req.SourceId, Req.DatumId, Store.Value, Store.Value
               );
            }
            else
            {
               printf(
                  "%s (%-3.3s, %-18.18s) = %8.8x %d\n",
                  TimeStr, CilName[ Req.SourceId ], 
                  eHtiGetDataString( Req.SourceId, Req.DatumId, DatumStr ),
                  Store.Value, Store.Value
               );

/*
               printf(
                  "%s (%-3.3s, %-18.18s) = %8.8x %d\n",
                  TimeStr, CilName[ Req.SourceId ], 
                  mXxxGetIdName( M_XXX_ID_FILE, Req.SourceId, 
                                 Req.DatumId, DatumStr ),
                  Store.Value, Store.Value
               );
*/
            }
         }
      }

   }

   /* Print out any file statistics */
   printf("%d records read from file %s\n", TotRecords, 
          eCluGetCustomParam( M_XXX_CUSTOM_FILE ));
   printf("%d bytes total\n", TotBytes);

   /* Close the file and terminate the program */
   fclose(FilePtr);
   exit( EXIT_SUCCESS );

}  /* End of main() */


/*******************************************************************************
** Function Name:
**    mXxxGetIdName
**
** Purpose:
**    Function to determine an identifier name as a string, using an owning 
**    process and a numeric ID.
**
** Description:
**    ...
**
** Return Type:
**    char * 
**       Returns a pointer to the supplied buffer to hold the string.
**
** Arguments:
**    char * IndexFilename             (in)
**       The filename of the index file to be used.
**    Int32_t Process                  (in)
**       The process ID for the value to be resolved.
**    Int32_t Value                    (in)
**       The value to be resolved into a string.
**    char * Buffer                    (in/out)
**       Pointer to buffer in which to write the resolved ID name.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

char *   mXxxGetIdName       ( char * IndexFilename, Int32_t Process, 
                               Int32_t Value, char * Buffer )
{
   int      Count;                     /* number of strings read from file */
   Int32_t  ValueRead;                 /* value read from line of file */
   Bool_t   Matched;                   /* set TRUE when match found in file */
   char     IdFile                     /* ID filename with path */
               [ M_XXX_STRING_LEN ];
   char     LineBuffer                 /* buffer for line read from ID file */
               [ M_XXX_STRING_LEN ];
   char     ProcessStr                 /* string containing process */
               [ M_XXX_STRING_LEN ];
   char     ValueStr                   /* string containing value */
               [ M_XXX_STRING_LEN ];
   char     LabelStr                   /* string containing label */
               [ M_XXX_STRING_LEN ];
   FILE   * FileId;                    /* pointer to file */

   /* initial conditions */
   Matched = FALSE;

   /* construct ID filename with path */
   sprintf( IdFile, "%s/%s", IdPath, IndexFilename );

   /* attempt to match a label in the file */
   FileId = fopen( IdFile, "r" );

   /* if file opened successfully */
   if ( FileId != NULL )
   {
      /* read lines looking for a match, until one found or EOF */
      do
      {
         /* read in a line from the file */
         if ( fgets( LineBuffer, sizeof( LineBuffer ), FileId ) != NULL )
         {
            /* line read in OK, convert arguments */
            Count = fscanf( FileId, M_XXX_IDTABLE_STR, 
                            ProcessStr, ValueStr, LabelStr );

            /* check for required number of strings found */
            if ( Count == M_XXX_IDTABLE_PARAMS )
            {
               /* convert the associated value */
               ValueRead = strtol( ValueStr, NULL, 0 );

               /* if process and value match */
               if ( ( strcmp( ProcessStr, CilName[ Process ] ) == 0 ) 
                     && ( ValueRead == Value ) )
               {
                  /* set flag to indicate label matched */
                  Matched = TRUE;

                  /* take copy of the label */
                  strcpy( Buffer, LabelStr );
               }
            }
         }
         /* else line not read correctly */
         else
         {
            /* set count to EOF to exit loop */
            Count = EOF;
         }
      }
      while ( ( Matched == FALSE ) && ( Count != EOF ) );
 
      /* close the file */
      fclose ( FileId );
   }

   /* if value not matched, then write number into the string */
   if ( Matched == FALSE )
   {
      sprintf( Buffer, "%4.4x", Value );
   }

   return Buffer;

}


/* EOF */
