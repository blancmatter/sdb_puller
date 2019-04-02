/*
** Module Name:
**    Clu.c
**
** Purpose:
**    Provides common functions for command-line argument handling.      
**
** Description:
**    Contains functions to set up the use of the Command Line Utilities, and 
**    to parse both command and custom arguments. Also contains functions to 
**    sign on and show the application's usage.
**
**    This code has been written to conform to "TTL Coding Standard" v0.6.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** Version:
**    $Id: Clu.c,v 0.32 2004/01/15 14:51:05 sxf Exp $
**
** History:
**    $Log: Clu.c,v $
**    Revision 0.32  2004/01/15 14:51:05  sxf
**    Ported to Neutrino with minimal changes.
**
**    Revision 0.31  2001/03/13 11:59:14  mjf
**    Do not attempt to reset process priority under VMS.
**
**    Revision 0.30  2001/03/13 11:50:34  mjf
**    CLU ported onto VMS.
**
**    Revision 0.29  2001/02/12 10:26:43  mjf
**    Ensure that SYS_NOMINAL is returned from a CLU sign-on if help has
**    been specified.
**
**    Revision 0.28  2001/01/25 16:36:09  mjf
**    Allow arguments supplied with switches to start with a '-'.
**
**    Revision 0.27  2000/12/12 18:48:05  mjf
**    Removal of function to submit application version into the SDB, and
**    this function moved into a new source file. This does not require an
**    application not using CIL to link the CIL library.
**
**    Revision 0.26  2000/12/11 14:51:04  mjf
**    Addition of function to submit the application's version into the SDB.
**    This function will also perform a CIL setup if one has not already been
**    done.
**
**    Revision 0.25  2000/11/23 10:56:33  mjf
**    Return status of eLogSetup from function eCluSignOn.
**
**    Revision 0.24  2000/11/23 10:28:52  djm
**    Improved instance detection, so that it will correctly handle applications
**    like the ECI and DAT.
**
**    Revision 0.23  2000/11/21 10:19:13  djm
**    eCluSignon now detects whether the application requires specification
**    of an instance name. This allows applications for which multiple copies
**    exist to the be distinct within the syslog (via the LOG pacakge).
**
**    Revision 0.22  2000/11/07 13:21:16  asp
**    More robust programme name extraction for Linux
**
**    Revision 0.21  2000/11/01 15:14:34  asp
**    Further Linux porting.
**
**    Revision 0.20  2000/10/31 11:33:30  djm
**    Changes to facilitate port to Linux.
**
**    Revision 0.19  2000/10/02 14:30:05  mjf
**    Alteration to the number of significant letters required for common args.
**
**    Revision 0.18  2000/08/30 09:04:59  mjf
**    Correction to length of 'strncpy' for CIL name.
**
**    Revision 0.17  2000/08/16 15:13:22  mjf
**    Call eCluParseArgs with a pointer to a buffer to copy an offending switch
**    into. The pointer can be NULL if the switch is unwanted.
**
**    Revision 0.16  2000/08/16 14:38:10  mjf
**    Change to common arguments - 'configfile' requires a 2 character match,
**    and 'cil' is now lower-case and requires a 3 character match.
**
**    Revision 0.15  2000/08/16 14:07:41  mjf
**    Split path before checking correct program name.
**
**    Revision 0.14  2000/08/16 13:48:41  mjf
**    If setup level incorrect, return from functions immediately.
**
**    Revision 0.13  2000/08/16 12:02:02  mjf
**    Remove detection of arguments clashing, i.e. both quiet and verbose.
**
**    Revision 0.12  2000/08/16 11:40:24  mjf
**    Use Status_t and SYS_NOMINAL for return status of functions, rather than
**    eCluStatus_t and E_CLU_NOMINAL.
**
**    Revision 0.11  2000/08/11 15:27:14  mjf
**    New log message at Notice level when application starts. Change of reporting
**    of found arguments now at Info level. Function created to set priority and
**    system call moved into here.
**
**    Revision 0.10  2000/08/09 14:52:15  mjf
**    Correction to function header.
**
**    Revision 0.9  2000/07/27 15:31:06  mjf
**    Addition of 'const' qualifier to passed-in strings.
**
**    Revision 0.8  2000/07/27 15:16:45  mjf
**    Addition of comments in function headers.
**
**    Revision 0.7  2000/07/26 15:38:50  mjf
**    Addition of common parameter to specify alternate CIL map file.
**
**    Revision 0.6  2000/07/26 14:05:05  mjf
**    The SignOn function now moves the process to the requested priority.
**
**    Revision 0.5  2000/07/26 13:48:59  mjf
**    Precedence now in errors return in CluStatus. Incorporation of use of LOG
**    within CLU - note that CluSignOn will now call eLogSetup. No need to call
**    CLU functions (except CluSetup) with argc, argv anymore.
**
**    Revision 0.4  2000/07/26 09:41:25  mjf
**    Use new Bool_t type from TtlSystem.h for a boolean.
**
**    Revision 0.3  2000/07/26 09:32:44  mjf
**    Addition of enumerated type for checking common arguments. Change of error
**    to be status returned by functions. Change of functions to check custom
**    parameters to accept an argument index, and the addition of two matching
**    functions for common parameters. Addition of elements in argument spec
**    structure to have flag if argument specified, and a pointer to the argument's
**    parameter.
**
**    Revision 0.2  2000/07/25 14:54:46  mjf
**    Year for copyright now implemented as a string. Addition of release date as
**    a string.
**
**    Revision 0.1  2000/07/25 13:21:09  mjf
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

#ifdef __VMS

#include <ctype.h>                     /* for 'tolower' */   

#endif


/*
** Put in the OS specific `includes' for process priority setting.
*/

#ifdef __QNX__
#include <sys/sched.h>
#endif

#ifdef __GNUC__
#include <unistd.h>
#endif




/*
** System include files
*/

#include "TtlSystem.h"
#define M_CLU_SOURCE                   /* define so variables get declared */
#include "Clu.h"
#include "Log.h"
#include "Cil.h"
#include "Sdb.h"


/*
** Local include files
*/

#include "CluPrivate.h"


/* 
** Global variables
*/

/* flag set TRUE when setup has been called */

static iCluSetup_t mCluSetupFlag    = I_CLU_SETUP_AWAIT;

/* Stores the CIL name during eCluSetup. We compare against it in eCluSignOn */
static char mCluDefaultCilName[E_CIL_IDLEN];

/* copy of passed-in argc, argv */

static int mCluArgCount;
static char **mCluArgPtr; 

/* copy of flag denoting which common flags to ignore */

static Uint32_t mCluIgnore = 0;

/* string pointer to program name */

static char   * mCluProgramName;

/* errors found during parsing */

static Status_t mCluStatus;

/* details of common arguments - must be in same order as the ignore flags */

static eCluArgSpec_t mCommonArg[ M_CLU_COMMON_ARGS + 1 ] =
   { { "quiet",             1,
       "Suppresses output of log messages to console",
       FALSE, NULL },
       
     { "verbose",           1, 
       "Outputs log messages to console",
       FALSE, NULL },
       
     { "syslog",            3, 
       "Use 'syslog' in system logging",
       FALSE, NULL },
       
     { "nosyslog",          5, 
       "Supresses use of 'syslog' in system logging",
       FALSE, NULL },
       
     { "debug [level]",     1, 
       "Debug with level specified",
       FALSE, NULL },
       
     { "priority [level]",  3, 
       "Priority to run process at",
       FALSE, NULL },
       
     { "configfile <name>", 3, 
       "Specify alternate configuration file",
       FALSE, NULL },
       
     { "logfile <name>",    3, 
       "Specify name of log file",
       FALSE, NULL },
       
     { "cil <name>",        3, 
       "Specify alternate CIL name",
       FALSE, NULL },
       
     { "map <CIL map>",     3, 
       "Specify alternate CIL map file",
       FALSE, NULL },
       
     { "help",              1, 
       "Display command-line usage and exit",
       FALSE, NULL },
       
     { E_CLU_EOL,           0, 
       E_CLU_EOL,
       FALSE, NULL } };


/*
** Function Name:
**    eCluSetup
**
** Type:
**    Status_t
**       Returns the status of the function on completion (SYS_NOMINAL).
**
** Purpose:
**    Sets up the command-line utility functions for use.
**
** Description:
**    Performs initialisation for command-line utility functions. Accepts a 
**    flag denoting which of the common options to ignore. The flag is one or 
**    more of the E_CLU_IGN_ macros from Clu.h bitwise OR'd together, or 
**    E_CLU_IGN_NONE only if all common arguments are required.
**
** Arguments:
**    int ArgCount                     (in)
**       The number of arguments inherited from the command-line (argc). 
**    char * ArgPtr[]                  (in)
**       Array of pointers to command-line argument strings (argv).
**    Uint32_t IgnoreFlags             (in)
**       Any bits set in this parameter specify which command arguments are to 
**       be ignored for this application.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf Alteration to return 'Status_t' type.
**    14-Jul-2000 mjf Initial creation.
**
*/

Status_t eCluSetup( int ArgCount, char * ArgPtr[], Uint32_t IgnoreFlags )
{
   int      PossibleArg;               /* index to each possible argument */
   eCluArgSpec_t * ArgSpecPtr;         /* pointer to argument details */
   
   /* set flag indicating initial setup has been performed */
   mCluSetupFlag = I_CLU_SETUP_PARSE;

   /* store the argc and argv to be used */
   mCluArgCount = ArgCount;
   mCluArgPtr   = ArgPtr;

   /* store pointer to the program name */
   mCluProgramName = mCluArgPtr[ M_CLU_ARG_PROGNAME ];
   
   /* make copy of switches to ignore */
   mCluIgnore = IgnoreFlags;
   
   /* take module copy of the status */
   mCluStatus = SYS_NOMINAL;

   /* clear present flags and argument parameter pointers, common arguments */
   PossibleArg = 0;
   while ( strcmp( mCommonArg[ PossibleArg ].Switch, E_CLU_EOL ) )
   {
      /* point to current structure */
      ArgSpecPtr = &( mCommonArg[ PossibleArg ] );

      /* set initial conditions */
      ArgSpecPtr->PresentFlag = FALSE;
      ArgSpecPtr->ParamPtr = NULL;

      /* increment index */
      PossibleArg++;
   }

   /* clear present flags and argument parameter pointers, custom arguments */
   PossibleArg = 0;
   while ( strcmp( eCluCustomArg[ PossibleArg ].Switch, E_CLU_EOL ) )
   {
      /* point to current structure */
      ArgSpecPtr = &( eCluCustomArg[ PossibleArg ] );

      /* set initial conditions */
      ArgSpecPtr->PresentFlag = FALSE;
      ArgSpecPtr->ParamPtr = NULL;

      /* increment index */
      PossibleArg++;
   }

   /*
   ** Note the default CIL name. If this changes, assume that we want to
   ** use the new CIL name as an "instance" when calling eLogSetup() in
   ** the eCluSignOn() call. Note that we force the last character to be
   ** be a null, so as not to risk string over-runs.
   */
   strncpy(mCluDefaultCilName, eCluCommon.CilName, E_CIL_IDLEN);
   mCluDefaultCilName[E_CIL_IDLEN - 1] = '\0';
   

   return SYS_NOMINAL;

}


/*
** Function Name:
**    eCluParseArgs
**
** Type:
**    Status_t
**       Returns the status of the function on completion.
**
** Purpose:
**    Parses the command-line arguments.
**
** Description:
**    Parses for the arguments. For common arguments uses the global variable 
**    which expects to initially hold the defaults, and overwrites with any 
**    passed in. For both common and custom arguments, writes into the argument 
**    spec with a flag to denote the argument was passed-in, and a pointer to 
**    any associated parameter. Also compares the passed-in application name 
**    against the expected, and shows the usage if the help switch has been 
**    passed in. Note - requires that eCluSetup has already been called.
**
** Arguments:
**    char * ArgErrorPtr               (out)
**       Pointer to string containing the erroneous switch, or a zero-length 
**       string in the event of 'no error'. If this string is NULL, then the   
**       switch is not passed back. String must be large enough to hold the 
**       switch name (E_CLU_SWITCH_LEN).
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf Return pointer to switch text on an error.
**    16-Aug-2000 mjf Split path up before checking program name.
**    16-Aug-2000 mjf If setup level incorrect, return immediately.
**    16-Aug-2000 mjf Alteration to return 'Status_t' type.
**    14-Jul-2000 mjf Initial creation.
**
*/

Status_t eCluParseArgs( char * ArgErrorPtr )
{
   int      PossibleArg;               /* counter to possible arguments */
   int      CurrentArg;                /* counter to passed-in arguments */
   char     ProgName[I_CLU_MAX_FNAME]; /* program name from command-line */
   Bool_t   ArgMatched;                /* set TRUE if argument matched */
   Bool_t   TestArg;                   /* set TRUE if current arg matched */
   Bool_t   ParamObtained;             /* set TRUE if param obtained with arg */
   char   * ArgParamPtr;               /* pointer to an argument parameter */
   Status_t Status;                    /* status of this function */
   eCluArgSpec_t * ArgSpecPtr;         /* pointer to argument details */

#ifdef __VMS
   int      i;                         /* general counter */
   char     CluName [I_CLU_MAX_FNAME]; /* program name embedded in code */
#endif

   /* check that initial setup has been called */
   if ( mCluSetupFlag < I_CLU_SETUP_PARSE )
   {
      return E_CLU_NO_SETUP;
   }

   /* initially no error */
   Status = SYS_NOMINAL;
   if ( ArgErrorPtr != NULL )
   {
      strcpy( ArgErrorPtr, "" );
   }

   /* increment setup status */
   mCluSetupFlag = I_CLU_SETUP_OK;
   
   /* intially pointer to an argument's parameters is NULL */
   ArgParamPtr = NULL;

   /* get progam name - extract from command-line */
   strcpy( ProgName, "" );

#ifdef __GNUC__
   /* Search for the last directory delimiter character */
   if ( strrchr( mCluProgramName, '/' ) )
   {
      /* Program name is everything after the last '/' character */ 
      strcpy( ProgName, 1 + strrchr( mCluProgramName, '/' ) );
   }
   else
   {
      /* No / delimiter so use the whole name */
      strcpy( ProgName, mCluProgramName ); 
   }
#endif

/* QNX4 specific code so exclude Neutrino */
#ifndef __QNXNTO__
#ifdef __QNX__
   _splitpath( mCluProgramName, NULL, NULL, ProgName, NULL );
#endif
#endif

#ifdef __VMS
   /* Search for the last directory delimiter character */
   if ( strrchr( mCluProgramName, ']' ) )
   {
      /* Program name is everything after the last ']' character */ 
      strcpy( ProgName, 1 + strrchr( mCluProgramName, ']' ) );
   }
   else if ( strrchr( mCluProgramName, ':' ) )
   {
      /* Program name is everything after the last ':' character */ 
      strcpy( ProgName, 1 + strrchr( mCluProgramName, ':' ) );
   }
   else
   {
      /* No / delimiter so use the whole name */
      strcpy( ProgName, mCluProgramName ); 
   }

   /* Strip off any extension */
   if ( strrchr( ProgName, '.' ) )
   {
      *( strrchr( ProgName, '.' ) ) = '\0';
   }
#endif

#ifndef __VMS
   /* compare passed in program name with one embedded in application */
   if ( strcmp( ProgName, eCluProgNamePtr ) != 0 )
   {
      if ( Status < E_CLU_PROGNAME_ERROR ) Status = E_CLU_PROGNAME_ERROR;
   }
#else
   /* VMS is not case-sensitive, convert strings to lower-case */
   strcpy( CluName, eCluProgNamePtr );
   for ( i = 0; i < strlen( CluName ); i++ )
   {
      CluName[ i ] = tolower( CluName[ i ] );
   }
   for ( i = 0; i < strlen( ProgName ); i++ )
   {
      ProgName[ i ] = tolower( ProgName[ i ] );
   }

printf( "Comparing '%s' with '%s'\n", ProgName, CluName );
   /* compare passed in program name with one embedded in application */
   if ( strcmp( ProgName, CluName ) != 0 )
   {
      if ( Status < E_CLU_PROGNAME_ERROR ) Status = E_CLU_PROGNAME_ERROR;
   }
#endif

   /* loop through command line arguments */
   for ( CurrentArg = M_CLU_ARG_FIRST; CurrentArg < mCluArgCount; CurrentArg++ )
   {
      /* check for common arguments - initially start at argument zero */
      PossibleArg = 0;
      
      /* clear flags */
      ArgMatched = FALSE;
      ParamObtained = FALSE;

      /* loop until end of list */
      while ( strcmp( mCommonArg[ PossibleArg ].Switch, E_CLU_EOL ) )
      {
         /* point to appropriate item */
         ArgSpecPtr = &( mCommonArg[ PossibleArg ] );
     
         /* check if a match found for this argument */
         TestArg = mCluMatchArg( CurrentArg, ArgSpecPtr, &ArgParamPtr );

         /* if switch is recognised, then set flag */
         if ( TestArg == TRUE )
         {
            ArgMatched = TRUE;
         }
     
         /* if this flag is not to be ignored, and argument matched */
         if ( ( ( ( 1 << PossibleArg ) & mCluIgnore ) == 0 ) 
               && ( TestArg  == TRUE ) )
         {
            /* set flag in argument spec */
            ArgSpecPtr->PresentFlag = TRUE;

            /* check which common argument has been processed, and assign */
            switch ( PossibleArg )
            {
               case E_CLU_ARG_QUIET : 
                  /* quiet is boolean, opposite of verbose */
                  eCluCommon.Quiet = TRUE;
                  eCluCommon.Verbose = FALSE;
                  break;
                  
               case E_CLU_ARG_VERBOSE : 
                  /* verbose is boolean, opposite of quiet */
                  eCluCommon.Verbose = TRUE;
                  eCluCommon.Quiet = FALSE;
                  break;
                  
               case E_CLU_ARG_SYSLOG : 
                  /* syslog is boolean */
                  eCluCommon.Syslog = TRUE;
                  break;
                  
               case E_CLU_ARG_NOSYSLOG : 
                  /* syslog is boolean */
                  eCluCommon.Syslog = FALSE;
                  break;
                  
               case E_CLU_ARG_DEBUG : 
                  /* debug has integer parameter */
                  if ( ArgParamPtr != NULL )
                  {
                     eCluCommon.DebugLevel = atoi( ArgParamPtr );
                     ParamObtained = TRUE;
                     /* set pointer in argument spec */
                     ArgSpecPtr->ParamPtr = ArgParamPtr;
                     /* increment current argument index over param */
                     CurrentArg++;
                  }
                  /* else no parameter supplied, set status and copy switch */
                  else
                  {
                     if ( Status < E_CLU_PARAM_MISSING ) 
                     {
                        Status = E_CLU_PARAM_MISSING;
                        if ( ArgErrorPtr != NULL )
                        {
                           strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
                        }
                     }
                  } 
                  break;
                  
               case E_CLU_ARG_PRIORITY : 
                  /* priority has integer parameter */
                  if ( ArgParamPtr != NULL )
                  {
                     eCluCommon.Priority = atoi( ArgParamPtr );
                     ParamObtained = TRUE;
                     /* set pointer in argument spec */
                     ArgSpecPtr->ParamPtr = ArgParamPtr;
                     /* increment current argument index over param */
                     CurrentArg++;
                  }
                  /* else no parameter supplied, set status */
                  else
                  {
                     if ( Status < E_CLU_PARAM_MISSING ) 
                     {
                        Status = E_CLU_PARAM_MISSING;
                        if ( ArgErrorPtr != NULL )
                        {
                           strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
                        }
                     }
                  } 
                  break;
                  
               case E_CLU_ARG_CONFIGFILE : 
                  /* configuration file name is a string, copy in */
                  if ( ArgParamPtr != NULL )
                  {
                     strncpy( eCluCommon.ConfigFile, ArgParamPtr, 
                              FILENAME_MAX );
                     eCluCommon.ConfigFile[ FILENAME_MAX - 1 ] = 0;
                     ParamObtained = TRUE;
                     /* set pointer in argument spec */
                     ArgSpecPtr->ParamPtr = ArgParamPtr;
                     /* increment current argument index over param */
                     CurrentArg++;
                  }
                  /* else no parameter supplied, set status */
                  else
                  {
                     if ( Status < E_CLU_PARAM_MISSING ) 
                     {
                        Status = E_CLU_PARAM_MISSING;
                        if ( ArgErrorPtr != NULL )
                        {
                           strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
                        }
                     }
                  } 
                  break;
                  
               case E_CLU_ARG_LOGFILE : 
                  /* log file name is a string, copy in */
                  if ( ArgParamPtr != NULL )
                  {
                     strncpy( eCluCommon.LogFile, ArgParamPtr, 
                              FILENAME_MAX );
                     eCluCommon.LogFile[ FILENAME_MAX - 1 ] = 0;
                     ParamObtained = TRUE;
                     /* set pointer in argument spec */
                     ArgSpecPtr->ParamPtr = ArgParamPtr;
                     /* increment current argument index over param */
                     CurrentArg++;
                  }
                  /* else no parameter supplied, set status */
                  else
                  {
                     if ( Status < E_CLU_PARAM_MISSING ) 
                     {
                        Status = E_CLU_PARAM_MISSING;
                        if ( ArgErrorPtr != NULL )
                        {
                           strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
                        }
                     }
                  } 
                  break;
                  
               case E_CLU_ARG_CIL_NAME : 
                  /* CIL name is a string, copy in */
                  if ( ArgParamPtr != NULL )
                  {
                     strncpy( eCluCommon.CilName, ArgParamPtr, E_CIL_IDLEN );
                     eCluCommon.CilName[ E_CIL_IDLEN - 1 ] = '\0';
                     ParamObtained = TRUE;
                     /* set pointer in argument spec */
                     ArgSpecPtr->ParamPtr = ArgParamPtr;
                     /* increment current argument index over param */
                     CurrentArg++;
                  }
                  /* else no parameter supplied, set status */
                  else
                  {
                     if ( Status < E_CLU_PARAM_MISSING ) 
                     {
                        Status = E_CLU_PARAM_MISSING;
                        if ( ArgErrorPtr != NULL )
                        {
                           strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
                        }
                     }
                  } 
                  break;
                  
               case E_CLU_ARG_CIL_MAP : 
                  /* CIL map is a string, copy in */
                  if ( ArgParamPtr != NULL )
                  {
                     strncpy( eCluCommon.CilMap, ArgParamPtr, 
                              FILENAME_MAX );
                     eCluCommon.CilMap[ FILENAME_MAX - 1 ] = 0;
                     ParamObtained = TRUE;
                     /* set pointer in argument spec */
                     ArgSpecPtr->ParamPtr = ArgParamPtr;
                     /* increment current argument index over param */
                     CurrentArg++;
                  }
                  /* else no parameter supplied, set status */
                  else
                  {
                     if ( Status < E_CLU_PARAM_MISSING ) 
                     {
                        Status = E_CLU_PARAM_MISSING;
                        if ( ArgErrorPtr != NULL )
                        {
                           strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
                        }
                     }
                  } 
                  break;
                  
               case E_CLU_ARG_HELP : 
                  /* set flag to indicate help requested */
                  eCluCommon.Help = TRUE;
                  /* show program usage */
                  eCluShowUsage( NULL, NULL );
                  break;
                  
               default :
                  break;
            }
         }
  
         /* increment counter to next possible argument */
         PossibleArg++;
      }

      /* if argument not matched, see if it is a custom one for info */
      if ( ArgMatched == FALSE )
      {
         /* reset possible argument counter */
         PossibleArg = 0;
         
         /* loop until end of list of possible custom args */
         while ( strcmp( eCluCustomArg[ PossibleArg ].Switch, E_CLU_EOL ) )
         {
            /* point to appropriate argument spec */
            ArgSpecPtr = &( eCluCustomArg[ PossibleArg ] );

            /* test if supplied argument is in the custom list */
            TestArg = mCluMatchArg( CurrentArg, ArgSpecPtr, &ArgParamPtr );
                                
            /* if argument is in custom list, set flag */
            if ( TestArg == TRUE )
            {
               ArgMatched = TRUE;

               /* set flag in argument spec */
               ArgSpecPtr->PresentFlag = TRUE;

               /* need to see if a parameter expected, and if one supplied */
               if ( strchr( ArgSpecPtr->Switch, M_CLU_SWITCH_GAP ) != NULL )
               {
                  /* if a further argument supplied on the command-line */
                  if ( ( CurrentArg + 1 ) < mCluArgCount )
                  {
                     /* set pointer in argument spec */
                     ArgSpecPtr->ParamPtr = ArgParamPtr;
                     /* increment current argument index over the param */
                     CurrentArg++;
                  }
                  /* else no parameter supplied, set status */
                  else
                  {
                     if ( Status < E_CLU_PARAM_MISSING ) 
                     {
                        Status = E_CLU_PARAM_MISSING;
                        if ( ArgErrorPtr != NULL )
                        {
                           strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
                        }
                     }
                  } 
               }
            }
            
            /* increment the possible argument counter */
            PossibleArg++;
         }
      }
      
      /* if argument has not been matched, set status and take copy of switch */
      if ( ArgMatched == FALSE )
      {
         if ( Status < E_CLU_ARG_UNKNOWN ) 
         {
            Status = E_CLU_ARG_UNKNOWN;
            if ( ArgErrorPtr != NULL )
            {
               strcpy( ArgErrorPtr, mCluArgPtr[ CurrentArg ] );
            }
         }
      }
   }

   /* take module copy of the status */
   mCluStatus = Status;

   /* return the determined status */
   return Status;

}


/*
** Function Name:
**    eCluCommonArgExists
**
** Type:
**    Status_t
**       Returns E_CLU_ARG_SUPPLIED if the requested argument was supplied, 
**       E_CLU_ARG_UNSUPPLIED if it wasn't, or E_CLU_NO_SETUP if the appropriate
**       pre-requisite functions have not been called.
**
** Purpose:
**    Checks the command-line for a specified common argument.
**
** Description:
**    Checks for the existence of whether a specific common argument was 
**    supplied on the command-line. If the argument is expected to have a 
**    parameter, then this may be retrieved with the function 
**    'eCluGetCommonParam'. Note - requires that eCluParseArgs has already been 
**    called.
**
** Arguments:
**    eCluCommonArg_t ArgIndex         (in)
**       Index of common argument to check against, enumerated type.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf If setup level incorrect, return immediately.
**    16-Aug-2000 mjf Alteration to return 'Status_t' type.
**    26-Jul-2000 mjf Initial creation.
**
*/

Status_t eCluCommonArgExists( eCluCommonArg_t ArgIndex )
{
   Status_t ReturnStatus;              /* status to return from function */

   /* check that initial setup has been called */
   if ( mCluSetupFlag < I_CLU_SETUP_OK )
   {
      return E_CLU_NO_SETUP;
   }

   /* check flag for relevant argument */
   if ( mCommonArg[ ArgIndex ].PresentFlag == TRUE )
   {
      /* set flag to indicate it was supplied */
      ReturnStatus = E_CLU_ARG_SUPPLIED;
   }
   /* else argument not supplied */
   else
   {
      /* set flag to indicated it wasn't supplied */
      ReturnStatus = E_CLU_ARG_UNSUPPLIED;
   }
   
   /* return the status of argument (un)matched or no setup */
   return ReturnStatus;

}                               


/*
** Function Name:
**    eCluGetCommonParam
**
** Type:
**    char *  
**       Returns pointer to parameter string, or NULL if none supplied.
**
** Purpose:
**    Gets the parameter associated with a specified common argument.
**
** Description:
**    Checks for the existence of whether a parameter for a specific common 
**    argument was supplied on the command-line. If the parameter is supplied, a
**    pointer to it is returned, else NULL is returned if the parameter isn't 
**    supplied. It would be sensible to use function 'eCluCheckCommonArg' to 
**    determine that the argument has been supplied before using this function.
**    Note - requires that eCluParseArgs has already been called.
**
** Arguments:
**    eCluCommonArg_t ArgIndex         (in)
**       Index of common argument to check against, enumerated type.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf If setup level incorrect, return immediately.
**    26-Jul-2000 mjf Initial creation.
**
*/

char * eCluGetCommonParam( eCluCommonArg_t ArgIndex )
{
   char   * ParamFoundPtr;             /* pointer to parameter for return */

   /* check that initial setup has been called */
   if ( mCluSetupFlag < I_CLU_SETUP_OK )
   {
      return NULL;
   }

   /* initially no parameter to point to */
   ParamFoundPtr = NULL;

   /* check relevant argument was specified */
   if ( mCommonArg[ ArgIndex ].PresentFlag == TRUE )
   {
      /* set return pointer to the parameter supplied */
      ParamFoundPtr = mCommonArg[ ArgIndex ].ParamPtr;
   }
   
   /* return pointer to argument, NULL if none found */
   return ParamFoundPtr;

}                               


/*
** Function Name:
**    eCluCustomArgExists
**
** Type:
**    Status_t
**       Returns E_CLU_ARG_SUPPLIED if the requested argument was supplied, 
**       E_CLU_ARG_UNSUPPLIED if it wasn't, or E_CLU_NO_SETUP if the appropriate
**       pre-requisite functions have not been called.
**
** Purpose:
**    Checks the command-line for a specified custom argument.
**
** Description:
**    Checks for the existence of whether a specific custom argument was 
**    supplied on the command-line. If the argument is expected to have a 
**    parameter, then this may be retrieved with the function 
**    'eCluGetCustomParam'. Note - requires that eCluParseArgs has already been 
**    called.
**
** Arguments:
**    int ArgIndex                     (in)
**       Index of custom argument to check against.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf If setup level incorrect, return immediately.
**    16-Aug-2000 mjf Alteration to return 'Status_t' type.
**    14-Jul-2000 mjf Initial creation.
**
*/

Status_t eCluCustomArgExists( int ArgIndex )
{
   Status_t ReturnStatus;              /* status to return from function */

   /* check that initial setup has been called */
   if ( mCluSetupFlag < I_CLU_SETUP_OK )
   {
      return E_CLU_NO_SETUP;
   }

   /* check flag for relevant argument */
   if ( eCluCustomArg[ ArgIndex ].PresentFlag == TRUE )
   {
      /* set flag to indicate it was supplied */
      ReturnStatus = E_CLU_ARG_SUPPLIED;
   }
   /* else argument not supplied */
   else
   {
      /* set flag to indicated it wasn't supplied */
      ReturnStatus = E_CLU_ARG_UNSUPPLIED;
   }
   
   /* return the status of argument (un)matched or no setup */
   return ReturnStatus;

}                               


/*
** Function Name:
**    eCluGetCustomParam
**
** Type:
**    char *  
**       Returns pointer to parameter string, or NULL if none supplied.
**
** Purpose:
**    Gets the parameter associated with a specified custom argument.
**
** Description:
**    Checks for the existence of whether a parameter for a specific custom 
**    argument was supplied on the command-line. If the parameter is supplied, a
**    pointer to it is returned, else NULL is returned if the parameter isn't 
**    supplied. It would be sensible to use function 'eCluCheckCustomArg' to 
**    determine that the argument has been supplied before using this function.
**    Note - requires that eCluParseArgs has already been called.
**
** Arguments:
**    int ArgIndex                     (in)
**       Index of custom argument to check against.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf If setup level incorrect, return immediately.
**    18-Jul-2000 mjf Initial creation.
**
*/

char * eCluGetCustomParam( int ArgIndex )
{
   char   * ParamFoundPtr;             /* pointer to parameter for return */

   /* check that initial setup has been called */
   if ( mCluSetupFlag < I_CLU_SETUP_OK )
   {
      return NULL;
   }

   /* initially no parameter to point to */
   ParamFoundPtr = NULL;

   /* check relevant argument was specified */
   if ( eCluCustomArg[ ArgIndex ].PresentFlag == TRUE )
   {
      /* set return pointer to the parameter supplied */
      ParamFoundPtr = eCluCustomArg[ ArgIndex ].ParamPtr;
   }
   
   /* return pointer to argument, NULL if none found */
   return ParamFoundPtr;

}                               


/*
** Function Name:
**    eCluSignOn
**
** Type:
**    Status_t
**       Returns the status of the function on completion.
**
** Purpose:
**    Outputs the sign-on string to specified stream.
**
** Description:
**    Outputs the sign-on string, and constructs this in a common format 
**    including the program name, company name, copyright message and version.
**    An optional supplied message may be output after this. This function also 
**    calls 'eLogSetup', set the process priority as specified, log any errors 
**    generated by the CLU, and logs the arguments being used by the 
**    application. Note - requires that eCluParseArgs has already been called.
**
**    The function also makes use of the global variable mCluDefaultCilName.
**    This is used to determine whether or not there is a specific instance
**    of the application. The variable is set to contain the CilName string
**    in the CLU common structure during eCluSetup(). If the common structure
**    CIL name element is changed between then and this function, then
**    this function will ensure that eLogSetup() is called with the new CIL 
**    name as the instance name.
**    Otherwise it is set to NULL, and no CIL instance is printed by the LOG
**    package.
**
** Arguments:
**    FILE * StreamPtr                 (in)
**       Pointer to output stream to be used. If this is NULL, then 'stdout' is 
**       assumed.
**    const char * MessageString       (in)
**       String pointing to optional message. If this is NULL then no message 
**       is displayed.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    12-Feb-2001 mjf Return SYS_NOMINAL if help requested.
**    20-Nov-2000 djm Added instance name to eLogSetup() call.
**    16-Aug-2000 mjf If setup level incorrect, return immediately.
**    16-Aug-2000 mjf Correction to status level checking.
**    16-Aug-2000 mjf Alteration to return 'Status_t' type.
**    11-Aug-2000 mjf Addition of 'starting application' message.
**    11-Aug-2000 mjf Alteration of priority of signing on messages.
**    14-Jul-2000 mjf Initial creation.
**
*/

Status_t eCluSignOn( FILE * StreamPtr, const char * MessageString )
{
   int      PossibleArg;               /* counter to possible arguments */
   Status_t LogStatus;                 /* status of LOG functions */   
   Status_t Status;                    /* status of this function */
   eCluArgSpec_t * ArgSpecPtr;         /* pointer to argument details */
   char * InstanceNamePtr;             /* pointer to the instance name */

   /* check that initial setup has been called */
   if ( mCluSetupFlag < I_CLU_SETUP_OK )
   {
      return E_CLU_NO_SETUP;
   }

   Status = SYS_NOMINAL;

   /* if no stream specified, assume default */
   if ( StreamPtr == NULL )
   {
      StreamPtr = M_CLU_DEFAULT_STREAM;
   }

   /* if verbose specified, output the information */
   if ( eCluCommon.Verbose )
   {
      /* output the program name and version */
      fprintf( StreamPtr, M_CLU_PROG_DISPLAY, eCluProgNamePtr, 
               eCluMajorVer, eCluMinorVer, eCluReleaseDatePtr );
     
      /* output the program about information */
      fprintf( StreamPtr, M_CLU_ABOUT_STR, eCluProgAboutPtr );
     
      /* output the copyright message */
      fprintf( StreamPtr, M_CLU_COPYRIGHT_STR, eCluYearPtr );
     
      /* output a blank line */
      fprintf( StreamPtr, "\n" );
   }

   /* do the following only if help not requested, ie program to continue */
   if ( eCluCommon.Help == FALSE )
   {
      /* Determine if there is an instance name */
      if(strcmp(mCluDefaultCilName, eCluCommon.CilName) != 0)
      {
         InstanceNamePtr = eCluCommon.CilName;
      }
      else
      {
         InstanceNamePtr = NULL;
      }

      /* initialise LOG with passed-in or default parameters */
      LogStatus = eLogSetup( eCluProgNamePtr, InstanceNamePtr,
                             eCluCommon.LogFile, 
                             E_LOG_UPTO( eCluCommon.DebugLevel ), 
                             eCluCommon.Syslog, eCluCommon.Verbose );

      /* output 'starting application', even if log setup has failed */
      eLogReport( E_LOG_NOTICE, 0, M_CLU_STR_START_APP, eCluProgNamePtr );

      /* check that log setup is OK - if error then log it !!! */
      /* eg if problem with log file, error appears on screen  */
      if ( LogStatus != SYS_NOMINAL )
      {
         eLogReport( M_CLU_LOG_ERR_LEVEL, LogStatus, M_CLU_STR_LOG, LogStatus );
      }

#ifndef __VMS
      /* move this process to the requested priority */
      if ( mCluSetPriority( eCluCommon.Priority ) != SYS_NOMINAL )
      {
         /* error occurred attempting to set priority */
         eLogReport( M_CLU_LOG_ERR_LEVEL, 0, M_CLU_STR_PRIO, 
                     eCluCommon.Priority );
      }
#endif

      /* if error has been detected, then output it and report it to LOG */
      switch ( mCluStatus )
      {
         case E_CLU_PROGNAME_ERROR :
            eLogReport( M_CLU_LOG_ERR_LEVEL, 0, M_CLU_STR_PROGNAME );
            break;
      
         case E_CLU_ARG_UNKNOWN :
            eLogReport( M_CLU_LOG_ERR_LEVEL, 0, M_CLU_STR_UNKNOWN );
            break;
      
         case E_CLU_PARAM_MISSING :
            eLogReport( M_CLU_LOG_ERR_LEVEL, 0, M_CLU_STR_PARAM_MISS );
            break;
      
         default :
            break;
      }

      /* output common parameters being used */
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_COMMON );
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_COMMON_A, 
                  eCluCommon.Quiet, eCluCommon.Verbose, eCluCommon.Syslog, 
                  eCluCommon.DebugLevel, eCluCommon.Priority );
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_COMMON_B, eCluCommon.ConfigFile );
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_COMMON_C, eCluCommon.LogFile );
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_COMMON_D, eCluCommon.CilName );
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_COMMON_E, eCluCommon.CilMap );

      /* output the custom parameters being used - start at zero */
      PossibleArg = 0;
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_CUSTOM );
         
      /* loop until end of list */
      while ( strcmp( eCluCustomArg[ PossibleArg ].Switch, E_CLU_EOL ) )
      {
         /* point to appropriate item */
         ArgSpecPtr = &( eCluCustomArg[ PossibleArg ] );

         /* if argument was specified */
         if ( ArgSpecPtr->PresentFlag )
         {
            /* if argument has no parameter */
            if ( strchr( ArgSpecPtr->Switch, M_CLU_SWITCH_GAP ) == NULL )
            {
               eLogReport( E_LOG_INFO, 0, M_CLU_STR_CUSTOM_ARG, 
                           ArgSpecPtr->Switch );
            }
            /* else argument has a parameter and it is not NULL */
            else if ( ArgSpecPtr->ParamPtr != NULL )
            {
               eLogReport( E_LOG_INFO, 0, M_CLU_STR_CUSTOM_PRM,
                           ArgSpecPtr->Switch, ArgSpecPtr->ParamPtr );
            }
            /* else argument has parameter and it was not specified */
            else
            {
               eLogReport( E_LOG_INFO, 0, M_CLU_STR_CUSTOM_PRM,
                           ArgSpecPtr->Switch, M_CLU_STR_NO_ARG );
            }
         }

         /* increment index */
         PossibleArg++;
      }

      /* output message for end of arguments */
      eLogReport( E_LOG_INFO, 0, M_CLU_STR_END_ARGS );

      /* if message string supplied, then output it */
      if ( MessageString != NULL )
      {
         eLogReport( E_LOG_INFO, 0, MessageString );
      }
   }
   /* else help has been requested */
   else
   {
      /* return SYS_NOMINAL - log setup not been performed */
      return SYS_NOMINAL;
   }

   /* return status of the log setup - any CLU errors already returned */
   return LogStatus;

}


/*
** Function Name:
**    eCluShowUsage
**
** Type:
**    Status_t
**       Returns the status of the function on completion.
**
** Purpose:
**    Outputs the usage string for command-line arguments to specified stream.
**
** Description:
**    Outputs the command-line arguments' usage in a common format. The common 
**    arguments appear first unless they are to be ignored, followed by custom 
**    parameters. An optional supplied message may be output after this. Note -
**    requires that eCluParseArgs has already been called.
**
** Arguments:
**    FILE * StreamPtr                 (in)
**       Pointer to output stream to be used. If this is NULL, then 'stdout' is 
**       assumed.
**    const char * MessageString       (in)
**       String pointing to optional message. If this is NULL then no message 
**       is displayed.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf If setup level incorrect, return immediately.
**    16-Aug-2000 mjf Correction to status level checking.
**    16-Aug-2000 mjf Alteration to return 'Status_t' type.
**    14-Jul-2000 mjf Initial creation.
**
*/

Status_t eCluShowUsage( FILE * StreamPtr, const char * MessageString )
{
   int      PossibleArg;               /* counter for possible arguments */
   Status_t Status;                    /* local status of this function */
   eCluArgSpec_t * ArgSpecPtr;         /* pointer to argument details */
   
   /* check that initial setup has been called */
   if ( mCluSetupFlag < I_CLU_SETUP_OK )
   {
      return E_CLU_NO_SETUP;
   }

   /* initially no errors */   
   Status = SYS_NOMINAL;

   /* if no stream specified, assume default */
   if ( StreamPtr == NULL )
   {
      StreamPtr = M_CLU_DEFAULT_STREAM;
   }

   /* sign on before displaying usage */
   Status = eCluSignOn( StreamPtr, NULL );

   /* display usage of program name and options */
   fprintf( StreamPtr, "Usage : %s {-options} :-\n", eCluProgNamePtr );
   
   /* first display common arguments starting at zero */
   PossibleArg = 0;
   
   /* loop around possible arguments */
   while ( strcmp( mCommonArg[ PossibleArg ].Switch, E_CLU_EOL ) )
   {
      /* point to current structure */
      ArgSpecPtr = &( mCommonArg[ PossibleArg ] );
   
      /* if this flag is not to be ignored */
      if ( ( ( 1 << PossibleArg ) & mCluIgnore ) == 0 )
      {
         /* display the argument details */
         fprintf( StreamPtr, M_CLU_OPTION_DISPLAY, M_CLU_SWITCH_CHAR,
                  ArgSpecPtr->Switch, ArgSpecPtr->Explanation );
      }
   
      /* increment the counter */
      PossibleArg++;
   }
   
   /* next display custom arguments starting at zero */
   PossibleArg = 0;
   
   /* loop around possible arguments */
   while ( strcmp( eCluCustomArg[ PossibleArg ].Switch, E_CLU_EOL ) )
   {
      /* point to current structure */
      ArgSpecPtr = &( eCluCustomArg[ PossibleArg ] );
  
      /* display the argument details */
      fprintf( StreamPtr, M_CLU_OPTION_DISPLAY, M_CLU_SWITCH_CHAR,
               ArgSpecPtr->Switch, ArgSpecPtr->Explanation );

      /* increment the counter */
      PossibleArg++;
   }

   /* display blank line */
   fprintf( StreamPtr, "\n" );

   /* output the message, if any */
   if ( MessageString != NULL )
   {
      fprintf( StreamPtr, "\n%s\n\n", MessageString );
   }

   /* return the determined status */
   return Status;

}


/*
** Function Name:
**    mCluSetPriority
**
** Type:
**    Status_t 
**       Returns SYS_NOMINAL is successful, else SYS_WARNING.
**
** Purpose:
**    Resets the priority for the current process.
**
** Description:
**    Calls a system call to reset the process priority. It is possible that  
**    this may fail, for example due to lack of priviledge.
**
** Arguments:
**    int Priority                     (in)
**       The required priority for this process.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    16-Aug-2000 mjf Alteration to return 'Status_t' type.
**    11-Aug-2000 mjf Initial creation.
**
*/

static Status_t mCluSetPriority( int Priority )
{
   Status_t Status;                    /* completion status of this function */

   /* initial status is  "nominal" */
   Status = SYS_NOMINAL;

#ifdef __QNX__
   /* call system function to set priority, pid 0 indicates this process */ 
   if ( setprio( 0, Priority ) == -1 )
   {
      Status = E_CLU_PRIO_ERR;
   }
#endif

#ifdef __GNUC__
   /* Use "nice" function call to change the priority (NB: reverse sense) */
   if( nice(Priority) == -1 )
   {
      Status = E_CLU_PRIO_ERR;
   }
#endif

#ifdef __VMS
   /* Set the status to an error indicating that it is not implemented */
   Status = E_CLU_NO_SETPRIO;
#endif

   return Status;

}


/*
** Function Name:
**    mCluMatchArg
**
** Type:
**    Bool_t   
**       Returns TRUE is argument found, else FALSE.
**
** Purpose:
**    Checks for a command-line argument, and optional result.
**
** Description:
**    Checks for the existence of a specified command-line argument against a 
**    passed in argument. If the argument is expected with a parameter then 
**    this is also extracted if provided.
**
** Arguments:
**    int CurrentArg                   (in)
**       The number of the passed-in arguments currently being checked.
**    eCluArgSpec_t * ArgSpecPtr       (in)
**       Pointer to the structure of argument details with which to check 
**       against the passed-in argument.
**    char ** ArgResult                (out)
**       Pointer to pointer to the parameter associated with an argument. This 
**       is set to NULL if there is no argument or it isn't present.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    14-Jul-2000 mjf Initial creation.
**
*/

static Bool_t mCluMatchArg( int CurrentArg, eCluArgSpec_t * ArgSpecPtr, 
                            char ** ArgParam ) 
{
   Bool_t   ArgMatched;                /* flag set TRUE if argument present */
   int      SwitchLength;              /* length of passed-in switch string */
   int      CompareLength;             /* number of letters for comparison */

   /* initially argument has not been matched and there is no parameter found */
   ArgMatched = FALSE;
   *ArgParam = NULL;

   /* if the first character is the switch character */
   if ( mCluArgPtr[ CurrentArg ][ 0 ] == M_CLU_SWITCH_CHAR )
   {
      /* switch length is simply length of string after first char */
      SwitchLength = strlen( &mCluArgPtr[ CurrentArg ][ 1 ] );
   
      /* if length of switch greater than minimum number to match, use this */ 
      if ( SwitchLength > ArgSpecPtr->MatchLetters )
      {
         CompareLength = SwitchLength;
      }
      /* else use the minimum number of characters required to match */
      else
      {
         CompareLength = ArgSpecPtr->MatchLetters;
      }
   
      /* if the first required number of letters match */
      if ( strncmp( &mCluArgPtr[ CurrentArg ][ 1 ], ArgSpecPtr->Switch, 
                    CompareLength ) == 0 )
      {
         /* set flag to indicate an argument has been matched */
         ArgMatched = TRUE;
         /* check for space in switch, to see if a parameter expected */
         if ( strchr( ArgSpecPtr->Switch, M_CLU_SWITCH_GAP ) != NULL )
         {
            /* if there is a further argument supplied on the command-line */
            if ( ( CurrentArg + 1 ) < mCluArgCount )
            {
               /* point to the next argument and assume it is the param */
               *ArgParam = mCluArgPtr[ CurrentArg + 1 ];
            }
         }
      }
   }
 
   /* return flag indicating if an argument has been matched */
   return ArgMatched;
 
}


/*
** EOF
*/
