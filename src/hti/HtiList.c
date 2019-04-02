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

#define I_XXX_PROGRAM_NAME   "HtiList"
#define I_XXX_PROGRAM_ABOUT  "HTI Datum List Utility"
#define I_XXX_RELEASE_DATE   "29 August 2007"
#define I_XXX_YEAR           "2004-07"
#define I_XXX_MAJOR_VERSION  0
#define I_XXX_MINOR_VERSION  2

/* common arguments defaults */

#define M_XXX_DFLT_QUIET     FALSE
#define M_XXX_DFLT_VERBOSE   TRUE
#define M_XXX_DFLT_SYSLOG    TRUE
#define M_XXX_DFLT_DEBUG     E_LOG_NOTICE
#define M_XXX_DFLT_PRIORITY  10
#define M_XXX_DFLT_HELP      FALSE
#define M_XXX_DFLT_CONFIG    "HtiTest.cfg"
#define M_XXX_DFLT_LOG       ""
#define M_XXX_DFLT_CIL       "TU1"
#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#else
#define M_XXX_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#endif

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_PATH    0

#define M_XXX_CUSTOM_ARGS    1

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


/*
** Global variables
*/

/* for parsing custom command-line arguments */

eCluArgSpec_t eCluCustomArg[ M_XXX_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                   FALSE, NULL */ 
{ { "path <path>",     4, "Specify path of lookup table", FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                      FALSE, NULL }
};


int main ( int argc, char *argv[] )
{
   Status_t Status;                    /* status returned from functions */
   Status_t HtiStatus;                 /* status returned from HTI functions */
   char     Path[ M_XXX_STR_LEN ];     /* path for lookup table */
   char     LookupFile[ M_XXX_STR_LEN ];  /* path/name of lookup table */
   eTtlTime_t StartTime;               /* starting time */
   eTtlTime_t EndTime;                 /* end time */
   eTtlTime_t ElapsedTime;             /* time taken to parse file */
   char     DatumName[ E_CFU_STRING_LEN ]; /* datum read from file */
   Int32_t  CilId;
   Int32_t  DatumId;
   FILE *   FilePtr;

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
   strcpy( Path,                  M_XXX_DFLT_PATH    );

   /* register with the command-line utilities (CLU), ignore some */
   Status = eCluSetup( argc, argv, E_CLU_IGN_CIL_MAP | E_CLU_IGN_CONFIGFILE );

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

   /* attempt to determine the CIL ID */
   Status = eCilLookup( eCluCommon.CilMap, eCluCommon.CilName, &CilId );

   /* check for alternative path supplied */
   if ( eCluCustomArgExists( M_XXX_CUSTOM_PATH ) == E_CLU_ARG_SUPPLIED )
   {
      /* check if a parameter was supplied */
      if ( eCluGetCustomParam( M_XXX_CUSTOM_PATH ) != NULL )
      {
         strcpy( Path, eCluGetCustomParam( M_XXX_CUSTOM_PATH ) );
      }
   }

   /* construct ID table name with path */
   sprintf( LookupFile, "%s/%s", Path, M_XXX_ID_TABLE );
   eLogNotice( 0, "Using lookup table '%s' and CIL '%s'", 
               LookupFile, eCilNameString( CilId ) );

   /* check that ID table exists */
   FilePtr = fopen( LookupFile, "r" );
   if ( FilePtr != NULL )
   {
      fclose( FilePtr );
   }
   else
   {
      eLogNotice( 0, "Unable to open lookup table '%s'", LookupFile );
      exit ( 0 );
   }

   /* take copy of start time */
   eTimGetTime( &StartTime );

   /* loop around looking up datum labels */
   DatumId = D_MCP_FIRST_USER_DATUM;
   do
   {
      /* attempt to look up the datum name */
      HtiStatus = eHtiGetLabel( LookupFile, CilId, DatumId, DatumName );

      /* if label looked up OK */
      if ( HtiStatus == SYS_NOMINAL )
      {
         /* log the result */
         eLogNotice( 0, "0x%06x %s", DatumId, DatumName );

         /* increment the datum Id */
         DatumId++;
      }
   }
   while ( HtiStatus == SYS_NOMINAL );

   /* take copy of end time */
   eTimGetTime( &EndTime );

   /* calculate the elapsed time */
   eTimDifference( &StartTime, &EndTime, &ElapsedTime );

   /* log output of time taken */
   eLogNotice( 0, "Max datum 0x%06x, time taken %d seconds", 
               DatumId - 1, ElapsedTime.t_sec );

   return 0;

}


/*
** EOF
*/


