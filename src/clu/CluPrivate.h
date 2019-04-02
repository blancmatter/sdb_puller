/*
** Header File Name:
**    CluPrivate.h
**
** Purpose:
**    Private header file for Command Line Argument package.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v0.6.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** Version:
**    $Id: CluPrivate.h,v 0.14 2005/06/17 09:35:10 mjf Exp $
**
** History:
**    $Log: CluPrivate.h,v $
**    Revision 0.14  2005/06/17 09:35:10  mjf
**    Use 'stderr' as default stream rather than 'stdout'.
**
**    Revision 0.13  2004/01/15 14:51:05  sxf
**    Ported to Neutrino with minimal changes.
**
**    Revision 0.12  2000/12/11 14:51:04  mjf
**    Addition of function to submit the application's version into the SDB.
**    This function will also perform a CIL setup if one has not already been
**    done.
**
**    Revision 0.11  2000/10/31 11:33:55  djm
**    Changes to facilitate port to Linux.
**
**    Revision 0.10  2000/08/16 12:02:02  mjf
**    Remove detection of arguments clashing, i.e. both quiet and verbose.
**
**    Revision 0.9  2000/08/16 11:31:10  mjf
**    Change of enum to be internal rather than module. Removal of trailing
**    carriage-returns for strings sent to LOG.
**
**    Revision 0.8  2000/08/11 15:26:26  mjf
**    Addition of strings for starting application and reporting log status
**    error. Also prototype for function to set priority.
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
**    Revision 0.3  2000/07/26 09:30:08  mjf
**    Addition of use of enumerated type for stages of setup. Also minor change
**    to program display string.
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

#ifndef CLU_PRIVATE_H_DEFINED
#define CLU_PRIVATE_H_DEFINED

/*
** Compiler include files
*/

#include <stdio.h>        /* For _MAX_FNAME */


/*
** Type definitions
*/

/* progress of CLU */

typedef enum
{
   I_CLU_SETUP_AWAIT,                  /* awaiting CLU setup */
   I_CLU_SETUP_PARSE,                  /* awaiting CLU parse arguments */
   I_CLU_SETUP_OK                      /* arguments have been parsed */
} iCluSetup_t;

/* structure for single SDB submissions */

typedef struct
{
   Uint32_t NumElts;                   /* elements for submission to SDB */
   eSdbDatum_t Datum;                  /* datum to be submitted to the SDB */
} iCluSdbData_t;
      

/* 
** Macro definitions
*/

/* number of common arguments */

#define M_CLU_COMMON_ARGS    11

/* macro to convert major and minor versions into milli-version */

#define M_CLU_GET_MVERSION( Major, Minor ) ( ( Major * 1000 ) + ( Minor * 10 ) )

/* CIL ID of SDB for application version submission */

#define M_CLU_CIL_SDB        E_CIL_SDB

/* number of repeated version submissions sent to SDB */

#define M_CLU_SDB_SUBMITS    3

/* default stream if none specified */

#define M_CLU_DEFAULT_STREAM stderr

/* character to start a command-line switch */

#define M_CLU_SWITCH_CHAR    '-'

/* character used to separate a parameter and an argument */

#define M_CLU_SWITCH_GAP     ' '

/* string to display program name, major and minor version, release date */

#define M_CLU_PROG_DISPLAY   "\n%s v%d.%02d, %s\n"

/* about string */

#define M_CLU_ABOUT_STR      "%s\n"

/* copyright string, with year as integer */

#define M_CLU_COPYRIGHT_STR  "Copyright Telescope Technologies Limited %s\n"

/* strings to display parameters actually being used */

#define M_CLU_STR_COMMON     "Common arguments :" 
#define M_CLU_STR_COMMON_A   \
                     "   quiet %d, verbose %d, syslog %d, debug %d, priority %d"
#define M_CLU_STR_COMMON_B   "   config file '%s'"
#define M_CLU_STR_COMMON_C   "   log file '%s'"
#define M_CLU_STR_COMMON_D   "   CIL name '%s'"
#define M_CLU_STR_COMMON_E   "   CIL map  '%s'"

#define M_CLU_STR_CUSTOM     "Custom arguments :"
#define M_CLU_STR_CUSTOM_ARG "   %s"
#define M_CLU_STR_CUSTOM_PRM "   %s '%s'"

#define M_CLU_STR_NO_ARG     "<unspecified>"

#define M_CLU_STR_END_ARGS   "End of arguments"

/* strings to report errors during parsing */

#define M_CLU_STR_PROGNAME   "Error in detected program name"
#define M_CLU_STR_UNKNOWN    "Unrecognised command-line argument"
#define M_CLU_STR_PARAM_MISS "Missing parameter expected with argument"

#define M_CLU_STR_START_APP  "Starting application '%s'"
#define M_CLU_STR_LOG        "Error %X reported by LOG setup"
#define M_CLU_STR_PRIO       "Unable to set process to requested priority %d"

/* string used to display option usage */

#define M_CLU_OPTION_DISPLAY "   %c%-24s %-50s\n"

/* command-line argument positions */

#define M_CLU_ARG_PROGNAME   0
#define M_CLU_ARG_FIRST      1

/* level to report LCU errors to LOG at */

#define M_CLU_LOG_ERR_LEVEL  E_LOG_ERR

/* The maximum number of characters in the program name */

#ifdef __QNX__
#ifdef __QNXNTO__    /* Neutrino */
#define I_CLU_MAX_FNAME  FILENAME_MAX
#else                /* QNX4 */
#define I_CLU_MAX_FNAME  _MAX_FNAME
#endif
#else
#define I_CLU_MAX_FNAME  64
#endif

/*
** Type definitions
*/


/* 
** Function prototypes
*/

static Status_t mCluSetPriority
                             ( int Priority );
static Bool_t mCluMatchArg   ( int CurrentArg, eCluArgSpec_t * ArgSpecPtr, 
                               char ** ArgParam );


#endif

/*
** EOF
*/
