/*
** Header File Name:
**    Clu.h
**
** Purpose:
**    Header file for Command Line Argument package.
**
** Description:
**    Contains the macros, type definitions and function prototypes needed to 
**    use the Command Line Arguments (CLU) package.
**
**    This code has been written to conform to "TTL Coding Standard" v0.6.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** Version:
**    $Id: Clu.h,v 0.15 2000/12/11 14:51:04 mjf Exp $
**
** History:
**    $Log: Clu.h,v $
**    Revision 0.15  2000/12/11 14:51:04  mjf
**    Addition of function to submit the application's version into the SDB.
**    This function will also perform a CIL setup if one has not already been
**    done.
**
**    Revision 0.14  2000/10/31 11:33:44  djm
**    Changes to facilitate port to Linux.
**
**    Revision 0.13  2000/08/16 15:13:22  mjf
**    Call eCluParseArgs with a pointer to a buffer to copy an offending switch
**    into. The pointer can be NULL if the switch is unwanted.
**
**    Revision 0.12  2000/08/16 12:02:02  mjf
**    Remove detection of arguments clashing, i.e. both quiet and verbose.
**
**    Revision 0.11  2000/08/16 11:22:35  mjf
**    Use Status_t as return type for functions, rather than eCluStatus_t.
**
**    Revision 0.10  2000/08/11 14:15:55  mjf
**    Macro E_CIL_IDLEN now available in Cil.h.
**
**    Revision 0.9  2000/07/27 15:27:34  mjf
**    Addition of 'const' qualifier to passed in message strings.
**
**    Revision 0.8  2000/07/27 15:15:03  mjf
**    Fix to comment for argument unsupplied.
**
**    Revision 0.7  2000/07/26 15:38:50  mjf
**    Addition of common parameter to specify alternate CIL map file.
**
**    Revision 0.6  2000/07/26 13:48:59  mjf
**    Precedence now in errors return in CluStatus. Incorporation of use of LOG
**    within CLU - note that CluSignOn will now call eLogSetup. No need to call
**    CLU functions (except CluSetup) with argc, argv anymore.
**
**    Revision 0.5  2000/07/26 09:41:25  mjf
**    Use new Bool_t type from TtlSystem.h for a boolean.
**
**    Revision 0.4  2000/07/26 09:32:44  mjf
**    Addition of enumerated type for checking common arguments. Change of error
**    to be status returned by functions. Change of functions to check custom
**    parameters to accept an argument index, and the addition of two matching
**    functions for common parameters. Addition of elements in argument spec
**    structure to have flag if argument specified, and a pointer to the argument's
**    parameter.
**
**    Revision 0.3  2000/07/25 14:54:46  mjf
**    Year for copyright now implemented as a string. Addition of release date as
**    a string.
**
**    Revision 0.2  2000/07/25 14:33:25  mjf
**    Array of custom arguments now declared within application.
**
**    Revision 0.1  2000/07/25 13:21:09  mjf
**    Initial revision.
**
**
*/

#ifndef CLU_H_DEFINED
#define CLU_H_DEFINED


/*
** Compiler include files 
*/

/* for FILENAME_MAX */

#include <stdio.h>


/*
** System include files
*/

/* for E_CIL_IDLEN */

#include "Cil.h"


/* 
** Macro definitions
*/

/* flags to ignore common arguments */
/*  - note must be 1 << (relevant eCluCommonArg_t below) */

#define E_CLU_IGN_NONE       0x00000000
#define E_CLU_IGN_QUIET      0x00000001
#define E_CLU_IGN_VERBOSE    0x00000002
#define E_CLU_IGN_SYSLOG     0x00000004
#define E_CLU_IGN_NOSYSLOG   0x00000008
#define E_CLU_IGN_DEBUG      0x00000010
#define E_CLU_IGN_PRIORITY   0x00000020
#define E_CLU_IGN_CONFIGFILE 0x00000040
#define E_CLU_IGN_LOGFILE    0x00000080
#define E_CLU_IGN_CIL_NAME   0x00000100
#define E_CLU_IGN_CIL_MAP    0x00000200
#define E_CLU_IGN_HELP       0x00000400

/* end-of-list string for possible arguments */

#define E_CLU_EOL            "\0"

/* string lengths for specification of possible arguments */

#define E_CLU_SWITCH_LEN     24
#define E_CLU_EXPL_LEN       50

/* declare data if this is CLU source, else data is 'extern' */

#ifdef M_CLU_SOURCE
#define E_CLU_EXTERN         
#else
#define E_CLU_EXTERN         extern
#endif
  

/*
** Type definitions
*/

/* enumerated type for indexing common arguments */

typedef enum
{
   E_CLU_ARG_QUIET = 0,
   E_CLU_ARG_VERBOSE,
   E_CLU_ARG_SYSLOG,
   E_CLU_ARG_NOSYSLOG,
   E_CLU_ARG_DEBUG,
   E_CLU_ARG_PRIORITY,
   E_CLU_ARG_CONFIGFILE,
   E_CLU_ARG_LOGFILE,
   E_CLU_ARG_CIL_NAME,
   E_CLU_ARG_CIL_MAP,
   E_CLU_ARG_HELP
} eCluCommonArg_t;

/* enumerated type for return values from Clu functions */

typedef enum {
   E_CLU_NO_ERR = STATUS_START( CLU ), /* no error - unused */
   E_CLU_ARG_SUPPLIED,                 /* specified argument supplied */
   E_CLU_ARG_UNSUPPLIED,               /* specified argument not supplied */
   E_CLU_PARAM_MISSING,                /* missing parameter expected with arg */
   E_CLU_ARG_UNKNOWN,                  /* unrecognised command line argument */
   E_CLU_PROGNAME_ERROR,               /* inconsistency in program name */
   E_CLU_NO_SETUP,                     /* setup has not been performed */
   E_CLU_PRIO_ERR,                     /* error setting process priority */
   E_CLU_NO_SETPRIO                    /* priority control not implemented */
} eCluStatus_t;

/* structure for argument specification */

typedef struct
{
                                       /* switch in full, check max length */
   char   Switch[ E_CLU_SWITCH_LEN + 1 ];
   int    MatchLetters;                /* letters required to achieve a match */
                                       /* explanation, check length */
   char   Explanation[ E_CLU_EXPL_LEN + 1 ];
   Bool_t PresentFlag;                 /* flag set TRUE if argument present */
   char   *ParamPtr;                   /* supplied argument parameter pointer */
} eCluArgSpec_t;

/* structure for default arguments */

typedef struct
{
   Bool_t   Quiet;                     /* TRUE for quiet, opposes verbose */
   Bool_t   Verbose;                   /* TRUE for verbose, opposes quiet */
   Bool_t   Syslog;                    /* TRUE to use syslog */
   Uint32_t DebugLevel;                /* as defined in Log.h */
   Uint32_t Priority;                  /* priority for process to run at */
   Bool_t   Help;                      /* TRUE if help requested */
   char     ConfigFile[ FILENAME_MAX ];/* alternate configuration file */
   char     LogFile   [ FILENAME_MAX ];/* alternate log file */
   char     CilName   [ E_CIL_IDLEN ]; /* points to alternate CIL string */
   char     CilMap    [ FILENAME_MAX ];/* alternate CIL map file */
} eCluCommon_t;


/*
** Variables - external to application but declared within CLU
*/ 

/* CLU data provided by application */

E_CLU_EXTERN char * eCluProgNamePtr;   /* expected name of program */
E_CLU_EXTERN char * eCluProgAboutPtr;  /* information about the program */
E_CLU_EXTERN char * eCluYearPtr;       /* year of production of program */
E_CLU_EXTERN char * eCluReleaseDatePtr;/* release date of software version */
E_CLU_EXTERN Uint32_t eCluMajorVer;    /* program major version */
E_CLU_EXTERN Uint32_t eCluMinorVer;    /* program minor version */
E_CLU_EXTERN eCluCommon_t eCluCommon;  /* structure of common arguments */

/* pointer to array of custom arguments - declared within the application */

#ifdef M_CLU_SOURCE
extern       eCluArgSpec_t eCluCustomArg[];
#endif


/* 
** Function prototypes
*/

Status_t eCluSetup           ( int ArgCount, char *ArgPtr[], 
                               Uint32_t IgnoreFlags );
Status_t eCluParseArgs       ( char * ArgErrorPtr );
Status_t eCluCommonArgExists ( eCluCommonArg_t ArgIndex );
char   * eCluGetCommonParam  ( eCluCommonArg_t ArgIndex );
Status_t eCluCustomArgExists ( int ArgIndex );
char   * eCluGetCustomParam  ( int ArgIndex );
Status_t eCluSignOn          ( FILE * StreamPtr, const char * MessageString );
Status_t eCluSubmitVersion   ( void );
Status_t eCluShowUsage       ( FILE * StreamPtr, const char * MessageString );


#endif

/*
** EOF
*/
