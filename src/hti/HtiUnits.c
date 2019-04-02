/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
** System include files
*/

#include "TtlSystem.h"
#include "Cfu.h"
#include "Clu.h"
#include "Hti.h"
#include "Log.h"
#include "Tim.h"
#include "Mcp.h"


/*
** Macro definitions
*/

/* application details */

#define I_XXX_PROGRAM_NAME   "HtiUnits"
#define I_XXX_PROGRAM_ABOUT  "HTI SDB Units Conversion"
#define I_XXX_RELEASE_DATE   "29 August 2007"
#define I_XXX_YEAR           "2007"
#define I_XXX_MAJOR_VERSION  0
#define I_XXX_MINOR_VERSION  1

/* common arguments defaults */

#define M_XXX_DFLT_QUIET     FALSE
#define M_XXX_DFLT_VERBOSE   TRUE
#define M_XXX_DFLT_SYSLOG    TRUE
#define M_XXX_DFLT_DEBUG     E_LOG_NOTICE
#define M_XXX_DFLT_PRIORITY  10
#define M_XXX_DFLT_HELP      FALSE
#define M_XXX_DFLT_CONFIG    "HtiUnits.cfg"
#define M_XXX_DFLT_LOG       ""
#define M_XXX_DFLT_CIL       "TU1"
#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#else
#define M_XXX_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#endif

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_ARGS    3

/* default infile and outfile */

#define M_XXX_DFLT_INFILE   "/opt/ttl/data/SdbUnits.dat"
#define M_XXX_DFLT_OUTFILE  "/opt/ttl/data/SdbUnits.txt"

/* custom argument defaults */

#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_PATH     "/opt/ttl/etc"
#else
#define M_XXX_DFLT_PATH     "/ttl/sw/etc"
#endif

/* name of lookup table */

#define M_XXX_ID_TABLE      "IdTable.idx"

/* length of an internal string */

#define M_XXX_STR_LEN       128

/* number of items on a line in the SDB units file */

#define M_XXX_LINE_ITEMS    3

/* command-line to sort input file */

#define I_XXX_SORT_CMD      "sort -u -o %s %s"


/*
** For parsing custom command-line arguments
*/

enum mXxxCluArgs_e
{
   M_XXX_CUSTOM_INFILE = 0,
   M_XXX_CUSTOM_OUTFILE,
   M_XXX_CUSTOM_PATH
};

#define M_XXX_SWITCH_INFILE  "infile <units file>"
#define M_XXX_EXPL_INFILE    "SDB units file (default /opt/ttl/data/SdbUnits.dat)"
#define M_XXX_SWITCH_OUTFILE "outfile <text file>"
#define M_XXX_EXPL_OUTFILE   "Output file (default /opt/ttl/data/SdbUnits.txt)" 
#define M_XXX_SWITCH_PATH    "path <path>"
#define M_XXX_EXPL_PATH      "Specify path of lookup table" 

/* Number of characters needed to be matched for custom arguements */
#define M_XXX_CUSTOM_MATCH 2

eCluArgSpec_t eCluCustomArg[]
 = { 
     { M_XXX_SWITCH_INFILE,  M_XXX_CUSTOM_MATCH, M_XXX_EXPL_INFILE,  FALSE, NULL },
     { M_XXX_SWITCH_OUTFILE, M_XXX_CUSTOM_MATCH, M_XXX_EXPL_OUTFILE, FALSE, NULL },
     { M_XXX_SWITCH_PATH,    M_XXX_CUSTOM_MATCH, M_XXX_EXPL_PATH,    FALSE, NULL },
     { E_CLU_EOL,            0,                  E_CLU_EOL,          FALSE, NULL }
};


int main ( int argc, char *argv[] )
{
   Status_t Status;                    /* status returned from functions */
   Status_t CfuStatus;                 /* status returned from CFU functions */
   char     Path[ M_XXX_STR_LEN ];     /* path for lookup table */
   char     LookupFile[ M_XXX_STR_LEN ];  /* path/name of lookup table */
   char     Infile[ M_XXX_STR_LEN ];   /* path/name of input file */
   char     Outfile[ M_XXX_STR_LEN ];  /* path/name of output file */
   char     Tmpfile[ M_XXX_STR_LEN ];  /* path/name of temporary file */
   char     SortCommand[ M_XXX_STR_LEN ];
   int      SysStatus;                 /* for operating system commands */
   char     Buffer[ E_CFU_STRING_LEN ];/* line read from file */
   Int32_t  CilId;
   Int32_t  DatumId;
   Int32_t  UnitsId;
   Int32_t  Count;
   char     DatumStr[ M_XXX_STR_LEN ];
   char     UnitsStr[ M_XXX_STR_LEN ];
   char     DescrStr[ M_XXX_STR_LEN ];
   FILE   * FilePtr;

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
   strcpy( Infile,                M_XXX_DFLT_INFILE  );
   strcpy( Outfile,               M_XXX_DFLT_OUTFILE );
   strcpy( Path,                  M_XXX_DFLT_PATH    );

   /* register with the command-line utilities (CLU), ignore some */
   Status = eCluSetup( argc, argv, 
               E_CLU_IGN_CIL_NAME | E_CLU_IGN_CIL_MAP | E_CLU_IGN_CONFIGFILE );

   /* if setup was successful */
   if ( Status == SYS_NOMINAL )
   {
      /* parse the incoming command-line */
      Status = eCluParseArgs( NULL );
   }

   /* if error parsing command-line arguments, report and exit */
   if ( Status != SYS_NOMINAL )
   {
      printf( I_XXX_PROGRAM_NAME ": <%x> Error parsing command-line\n", Status );
      return Status;
   }
   else
   {
      /* If help-only requested, go no further */
      if ( eCluCommon.Help == TRUE )
      {
         exit ( 0 );
      }
   }

   /* sign-on - note that LOG is set up within here */
   Status = eCluSignOn( NULL, NULL );
   if( Status != SYS_NOMINAL )
   {
      eLogCrit( Status, "Unable to sign the program on" );
      return Status;
   }

   /* check for alternative path supplied */
   if ( eCluCustomArgExists( M_XXX_CUSTOM_PATH ) == E_CLU_ARG_SUPPLIED )
   {
      /* check if a parameter was supplied */
      if ( eCluGetCustomParam( M_XXX_CUSTOM_PATH ) != NULL )
      {
         strcpy( Path, eCluGetCustomParam( M_XXX_CUSTOM_PATH ) );
      }
   }

   /* check for alternative infile supplied */
   if ( eCluCustomArgExists( M_XXX_CUSTOM_INFILE ) == E_CLU_ARG_SUPPLIED )
   {
      /* check if a parameter was supplied */
      if ( eCluGetCustomParam( M_XXX_CUSTOM_INFILE ) != NULL )
      {
         strcpy( Infile, eCluGetCustomParam( M_XXX_CUSTOM_INFILE ) );
      }
   }

   /* check for alternative outfile supplied */
   if ( eCluCustomArgExists( M_XXX_CUSTOM_OUTFILE ) == E_CLU_ARG_SUPPLIED )
   {
      /* check if a parameter was supplied */
      if ( eCluGetCustomParam( M_XXX_CUSTOM_OUTFILE ) != NULL )
      {
         strcpy( Outfile, eCluGetCustomParam( M_XXX_CUSTOM_OUTFILE ) );
      }
   }

   /* construct ID table name with path */
   sprintf( LookupFile, "%s/%s", Path, M_XXX_ID_TABLE );
   eLogNotice( 0, "Using lookup table '%s'", LookupFile );

   /* check that ID table exists */
   FilePtr = fopen( LookupFile, "r" );
   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }
   else
   {
      eLogErr( SYS_WARNING, "Unable to open lookup table '%s'", LookupFile );
      exit ( 0 );
   }

   /* sort the input file prior to processing */
   if ( tmpnam( Tmpfile ) == NULL )
   {
      /* unable to create temporary file */
      eLogErr( SYS_WARNING, "Unable to create temporary file" );
      exit ( 0 );
   }
   else
   {
      /* sort the input file into a temporary file */
      sprintf( SortCommand, I_XXX_SORT_CMD, Tmpfile, Infile );
      eLogInfo( "Sorting input file '%s'", SortCommand );
      SysStatus = system( SortCommand );
   }

   /* attempt to open the (sorted) SDB units file */
   CfuStatus = eCfuSetup( Tmpfile );

   /* if not opened, report error and exit */
   if ( CfuStatus != SYS_NOMINAL )
   {
      eLogErr( CfuStatus, "Unable to open (temporary) input file '%s'", Tmpfile );
      exit ( 0 );
   }

   /* attempt to open the output file */
   FilePtr = fopen( Outfile, "w" );
   
   /* if not opened, report error and exit */
   if ( FilePtr == NULL )
   {
      eLogErr( SYS_WARNING, "Unable to open output file '%s'", Outfile );
      exit ( 0 );
   }

   eLogNotice( 0, "Processing '%s' -> '%s'", Infile, Outfile );

   /* zero the count of entries retrieved */
   Count = 0;

   /* loop around looking up datum units */
   do
   {
      /* attempt to read a line from the input file */
      if ( CfuStatus == SYS_NOMINAL )
      {
         CfuStatus = eCfuGetLine( Buffer );
      }

      /* attempt to read the first parameter from the line */
      if ( CfuStatus == SYS_NOMINAL )
      {
         /* parse the line, in format 'SourceId DatumId Units' */
         if ( M_XXX_LINE_ITEMS != sscanf( Buffer, "%x %x %x", 
                                           &CilId, &DatumId, &UnitsId ) )
         {
            eLogErr( SYS_WARNING, "Unable to parse line '%s'", Buffer );
         }
         else
         {
            /* write a line to the output text file */
            fprintf( FilePtr, "%s %-32s %-24s \"%s\"\n", 
                     eCilNameString( CilId), 
                     eHtiGetDataString( CilId, DatumId, DatumStr ),
                     eHtiGetUnitString( UnitsId, UnitsStr ),
                     eHtiGetOidDescriptionString( CilId, DatumId, DescrStr )
                     );

            /* write information to the screen */
            eLogInfo( "%s %s %s", 
                      eCilNameString( CilId), 
                      eHtiGetDataString( CilId, DatumId, DatumStr ),
                      eHtiGetUnitString( UnitsId, UnitsStr ) );

            /* increment the count of entries reteived */
            Count++;
         }
      }
   }
   while ( CfuStatus == SYS_NOMINAL );

   /* complete use of the open file */
   eCfuComplete();

   /* remove the temporary file */
   if ( remove( Tmpfile ) != 0 )
   {
      /* unable to remove the temporary file */
      eLogErr( SYS_WARNING, "Unable to remove temporary file '%s'", Tmpfile );
   }

   /* output usage results */
   eLogNotice( 0, "Processed %d entries", Count );

   return 0;

}


/*
** EOF
*/



