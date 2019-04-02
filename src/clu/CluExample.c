/*
** Module Name:
**    CluExample.c
**
** Purpose:
**    Example of use of Command-Line Utilities (CLU).             
**
** Description:
**    ...
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** Version:
**    $Id: CluExample.c,v 0.5 2000/12/11 14:51:04 mjf Exp $
**
** History:
**    $Log: CluExample.c,v $
**    Revision 0.5  2000/12/11 14:51:04  mjf
**    Addition of function to submit the application's version into the SDB.
**    This function will also perform a CIL setup if one has not already been
**    done.
**
**    Revision 0.4  2000/11/08 11:36:21  mjf
**    Removal of unused variables.
**
**    Revision 0.3  2000/08/16 15:15:42  mjf
**    Use Status_t rather than eCluStatus. When parsing arguments pass pointer
**    to store erroneous switch in. Call 'exit' if help specified.
**
**    Revision 0.2  2000/07/27 15:16:15  mjf
**    Addition of call to eCluSignOn.
**
**    Revision 0.1  2000/07/27 10:37:23  mjf
**    Initial revision.
**
**
*/


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
#include "Log.h"
#include "Clu.h"


/*
** Macro definitions
*/

/* application details - normally found in XxxPrivate.h */

#define I_XXX_PROGRAM_NAME   "CluExample"
#define I_XXX_PROGRAM_ABOUT  "Example of use of Command Line Utilities"
#define I_XXX_RELEASE_DATE   "08 November 2000"
#define I_XXX_YEAR           "2000"
#define I_XXX_MAJOR_VERSION  1
#define I_XXX_MINOR_VERSION  3

/* common arguments defaults */

#define M_XXX_DFLT_QUIET     FALSE
#define M_XXX_DFLT_VERBOSE   TRUE
#define M_XXX_DFLT_SYSLOG    TRUE
#define M_XXX_DFLT_DEBUG     E_LOG_NOTICE
#define M_XXX_DFLT_PRIORITY  12
#define M_XXX_DFLT_HELP      FALSE
#define M_XXX_DFLT_CONFIG    "/opt/ttl/etc/CluExample.cfg"
#define M_XXX_DFLT_LOG       "CluExampleLog.txt"
#define M_XXX_DFLT_CIL       "XXX"
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_BOOLEAN 0
#define M_XXX_CUSTOM_INTEGER 1
#define M_XXX_CUSTOM_TEXT    2

#define M_XXX_CUSTOM_ARGS    3


/*
** Global variables
*/

/* for parsing custom command-line arguments */

eCluArgSpec_t eCluCustomArg[ M_XXX_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                   FALSE, NULL */ 
{ { "boolean",         1, "can be used for boolean flag", FALSE, NULL },
  { "integer [param]", 3, "used to pass in an integer",   FALSE, NULL },
  { "text <string>",   4, "used to pass in a string",     FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                      FALSE, NULL }
};


/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    Main and only function for CLU example code.
**
** Description:
**    Initialises the global variables about the application as required by CLU,
**    then assigns the defaults for the common parameters. Sets up CLU, then 
**    parses the command-line arguments. Finally checks for the existence of 
**    the existence of some arguments, and their parameters if appropriate.
**
** Arguments:
**    int argc                         (in)
**       Number of arguments on the command-line including executable name.
**    char *argv[]                     (in)
**       Array of null-terminated character strings containing arguments.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    27-Jul-2000 mjf Initial creation.
**
*/

int main ( int argc, char *argv[] )
{
   Status_t CluStatus;                 /* status returned of CLU functions */
   char     Switch[ E_CLU_SWITCH_LEN ];/* to report error parsing a switch */

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
   CluStatus = eCluSetup( argc, argv, E_CLU_IGN_CIL_MAP );
   
   /* if setup was successful */
   if ( CluStatus == SYS_NOMINAL )
   {
      /* parse the incoming command-line */
      CluStatus = eCluParseArgs( Switch );

      /* print out returned CLU status */
      switch ( CluStatus )
      {
         case SYS_NOMINAL :
            break;
         case E_CLU_ARG_SUPPLIED :
            printf( "Status E_CLU_ARG_SUPPLIED\n" );
            break;
         case E_CLU_ARG_UNSUPPLIED :
            printf( "Status E_CLU_ARG_UNSUPPLIED\n" );
            break;
         case E_CLU_PARAM_MISSING :
            printf( "Status E_CLU_PARAM_MISSING - switch '%s'\n", Switch );
            break;
         case E_CLU_ARG_UNKNOWN :
            printf( "Status E_CLU_ARG_UNKNOWN - switch '%s'\n", Switch );
            break;
         case E_CLU_PROGNAME_ERROR :
            printf( "Status E_CLU_PROGNAME_ERROR\n" );
            break;
         case E_CLU_NO_SETUP :
            printf( "Status E_CLU_NO_SETUP\n" );
            break;
         default :
            break;
      }
   }

   /* if help specified, then terminate */
   if ( eCluCommonArgExists( E_CLU_ARG_HELP ) == E_CLU_ARG_SUPPLIED )
   {
      exit( EXIT_SUCCESS );
   }

   /* sign on */
   eCluSignOn( NULL, "Starting CLU example" );

   /* call CIL setup and submit version into SDB */
   CluStatus = eCluSubmitVersion();

   /* output blank line */
   printf( "\n" );

   /* print out requested debug level from the structure */
   printf( "Debug level from eCluCommon %d\n", eCluCommon.DebugLevel );
   /* print out requested debug level using the functions */
   if ( eCluCommonArgExists( E_CLU_ARG_DEBUG ) == E_CLU_ARG_SUPPLIED )
   {
      printf( "Debug argument specified  : " );
      /* check if a parameter was supplied */
      if ( eCluGetCommonParam( E_CLU_ARG_DEBUG ) != NULL )
      {
         printf( "'%s'\n", eCluGetCommonParam( E_CLU_ARG_DEBUG ) );
      }
      else
      {
         printf( "unsupplied\n" );
      }
   }

   /* print out requested boolean (custom arg) using the functions */
   printf( "Custom 'boolean' argument : " );
   if ( eCluCustomArgExists( M_XXX_CUSTOM_BOOLEAN ) == E_CLU_ARG_SUPPLIED )
   {
      printf( "supplied\n" );
   }
   else
   {
      printf( "unsupplied\n" );
   }
   /* print out requested integer (custom arg) using the functions */
   printf( "Custom 'integer' argument : " );
   if ( eCluCustomArgExists( M_XXX_CUSTOM_INTEGER ) == E_CLU_ARG_SUPPLIED )
   {
      /* check if a parameter was supplied */
      if ( eCluGetCustomParam( M_XXX_CUSTOM_INTEGER ) != NULL )
      {
         printf( "'%s' (%d)\n", 
                 eCluGetCustomParam( M_XXX_CUSTOM_INTEGER ), 
                 atoi( eCluGetCustomParam( M_XXX_CUSTOM_INTEGER ) ) );
      }
      else
      {
         printf( "parameter not supplied\n" );
      }
   }
   else
   {
      printf( "argument not supplied\n" );
   }

   /* print out requested text string (custom arg) using the functions */
   printf( "Custom 'text' argument    : " );
   if ( eCluCustomArgExists( M_XXX_CUSTOM_TEXT ) == E_CLU_ARG_SUPPLIED )
   {
      /* check if a parameter was supplied */
      if ( eCluGetCustomParam( M_XXX_CUSTOM_TEXT ) != NULL )
      {
         printf( "'%s'\n", eCluGetCustomParam( M_XXX_CUSTOM_TEXT ) );
      }
      else
      {
         printf( "parameter not supplied\n" );
      }
   }
   else
   {
      printf( "argument not supplied\n" );
   }

   return 0;

}


/*
** EOF
*/

