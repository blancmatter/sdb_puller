/*******************************************************************************
** Module Name:
**    CctOsInterface.c
**
** Purpose:
**    Module of the Computer Control Task to interface to the operating system.
**
** Description:
**    The module of the CCT which interfaces to the operating system.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    -
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Mark Bowman    (mkb)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: CctOsInterface.c,v 0.18 2007/12/04 15:56:50 mjf Exp $
**
** History:
**    $Log: CctOsInterface.c,v $
**    Revision 0.18  2007/12/04 15:56:50  mjf
**    Tweak to log message.
**
**    Revision 0.17  2007/12/04 14:55:36  mjf
**    If the forked child process is unable to execute the specified program,
**    ensure it terminates rather than remain a duplicate of the parent.
**
**    Revision 0.16  2007/10/17 10:32:54  mjf
**    Fix to log message upon inability to open the shutdown config file.
**
**    Revision 0.15  2004/01/16 09:38:27  sxf
**    Minor changes porting to QNX6.
**
**    Revision 0.14  2003/12/02 14:43:36  sxf
**    Correct pre-processor directive to compile with QNX,VMS and Linux.
**
**    Revision 0.13  2002/10/29 15:30:44  mkb
**    Simplify VMS implementation for stop/start application. Now
**    uses system() rather than SYS$CREPRC()
**
**    Revision 0.12  2001/12/04 14:42:03  mjf
**    Change to QNX variant only - ignore child signal before a 'fork', to
**    ensure that zombie processes are not left after killing them.
**
**    Revision 0.11  2001/11/30 15:22:52  mjf
**    Change of log message levels.
**
**    Revision 0.10  2001/08/07 08:31:56  mjf
**    Addition of delay following the execution of a process. Set to 1000 ms
**    by default, may be altered on the command-line.
**
**    Revision 0.9  2001/07/25 10:04:28  mjf
**    Changes made by MKB to use the CCT to run software on VMS.
**
**    Revision 0.8  2001/07/17 13:39:57  mjf
**    If a shutdown configuration file is supplied, run this on application
**    shutdown rather than killing processes using pids.
**
**    Revision 0.7  2001/05/25 13:30:29  mjf
**    On an operating system shutdown or restart command, send the reply to
**    the command before performing the requested action.
**
**    Revision 0.6  2001/05/21 14:25:20  mjf
**    Check for non-zero pid before attempting a 'kill'.
**
**    Revision 0.5  2001/03/14 14:58:04  mjf
**    Porting to VMS complete.
**
**    Revision 0.4  2001/03/09 15:06:24  mjf
**    Increased POSIX compliance and full support for Linux.
**
**    Revision 0.3  2001/03/08 16:54:44  mjf
**    Full functionality for QNX added. Datums only submitted to the SDB
**    when they are re-determined. Query datums are re-determined on a
**    corresponding 'get' command.
**
**    Revision 0.2  2001/03/07 15:19:52  mjf
**    Submit to the SDB only the datums that have been updated or refreshed.
**
**    Revision 0.1  2001/03/07 12:00:48  mjf
**    Initial revision.
**
**
*******************************************************************************/


/*
** Feature-test macro definitions
*/ 

/* VMS 'kill' with signal 0 validates pid without actually sending a signal */

#ifdef __VMS

#define _POSIX_EXIT  
/*
** Defining __NEW_STARLET enables the program to benefit from better type
** checking for prototypes and structures provided by OpenVMS.
*/
#define __NEW_STARLET 1 

#endif


/*
** Compiler include files
*/

#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

/*
** System include files
*/
#include "Wfl.h"
#include "Log.h"
#include "Tim.h"
#include "Cfu.h"

#ifdef E_WFL_OS_QNX4

#include <process.h>
#include <i86.h>

#endif

#ifdef E_WFL_OS_QNX6
#endif

#ifdef E_WFL_OS_VMS

#include <ints.h>    /* Define integer types.                  */
#include <ssdef.h>   /* System service enumerations.           */
#include <prcdef.h>  /* Process options enumerations.          */
#include <prvdef.h>  /* Priviledge enumerations.               */
#include <descrip.h> /* System service macro definitions.      */
#include <starlet.h> /* Open VMS system services entry points. */

#endif


/*
** Local include files
*/
#include "Cct.h"
#include "CctPrivate.h"
#include "CctOsInterface.h"


/*
** Variable declarations
*/ 

/* array of pointers for argument array - assume maximum size */

static char * mCctArgV[ E_CFU_STRING_LEN ];

/* number of processes started */

static int      mCctProcessCount;

/* array of process IDs for processes started */

static pid_t    mCctProcessPid[ M_CCT_MAX_PIDS ];


/*******************************************************************************
** Function Name:
**    iCctOsDataInit
**
** Purpose:
**    Function to initialise data for the operating system interfacing.
**
** Description:
**    This function is called during initialisation to initialise and data that
**    is static to this module.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctOsDataInit      ( void )
{
   int      Index;                     /* index into array */

   /* set the initial number of processes spawned */
   mCctProcessCount = 0;

   /* loop around the array containing process IDs */ 
   for ( Index = 0; Index < M_CCT_MAX_PIDS; Index++ )
   {
      /* assign invalid pid */
      mCctProcessPid[ Index ] = M_CCT_PID_INVALID;
   }

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctOsAvailable
**
** Purpose:
**    Function to determine if the operating system is available.
**
** Description:
**    This function is called in response to a 'get' command requesting whether
**    the operating system is available. This status is determined, and 
**    assigned into the internal parameters. The flag is set to ensure that 
**    this datum is submitted into the SDB on the next submission cycle.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctOsAvailable     ( void )
{
   Bool_t   OsAvailable;               /* flag TRUE if operating system up */  

   /* set initial status of flag */
   OsAvailable = E_SYS_FALSE;

   /*
   ** determine if the operating system is running
   */

   /* if this code is running, then the operating system must be running */
   OsAvailable = E_SYS_TRUE;


   /* log debug at info level */
   eLogReport( E_LOG_DEBUG, 0, "OS available        : %d", OsAvailable );

   /* set status in internal parameters */
   iCctData.Param[ D_CCT_QRY_OS_AVAILABLE ].Datum.Msrment.Value = OsAvailable;

   /* set flag to ensure this is reported into the SDB next time */
   iCctData.Param[ D_CCT_QRY_OS_AVAILABLE ].StatusObtained = TRUE;

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctComputerReady
**
** Purpose:
**    Function to determine if the computer is ready to run the application 
**    software.
**
** Description:
**    This function is called in response to a 'get' command requesting whether
**    the computer is ready to run the application. This status is determined, 
**    and assigned into the internal parameters. The flag is set to ensure that 
**    this datum is submitted into the SDB on the next submission cycle.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctComputerReady   ( void )
{
   Bool_t   ComputerReady;             /* flag TRUE is computer is prepared */  

   /* set initial status of flag */
   ComputerReady = E_SYS_FALSE;

   /*
   ** determine if the computer is ready for the application
   */

   /* if this code is running, then the computer is prepared */
   ComputerReady = E_SYS_TRUE;


   /* log debug at info level */
   eLogReport( E_LOG_DEBUG, 0, "Computer ready      : %d", ComputerReady );

   /* set status in internal parameters */
   iCctData.Param[ D_CCT_QRY_APP_PREPARED ].Datum.Msrment.Value = ComputerReady;

   /* set flag to ensure this is reported into the SDB next time */
   iCctData.Param[ D_CCT_QRY_APP_PREPARED ].StatusObtained = TRUE;

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctAppRunning
**
** Purpose:
**    Function to determine if the application is running on the computer. 
**
** Description:
**    This function is called in response to a 'get' command requesting whether
**    the software application is running on the computer. This status is 
**    determined, and assigned into the internal parameters. The flag is set to
**    ensure that this datum is submitted into the SDB on the next submission 
**    cycle.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctAppRunning      ( void )
{
   Bool_t   AppRunning;                /* flag TRUE if application is running */  
   int      Index;                     /* index to array of pids */

   /* set initial status of flag */
   AppRunning = E_SYS_FALSE;

   /*
   ** determine if the application is running
   */

   /* loop through the registered process id s*/
   for ( Index = 0; Index < mCctProcessCount; Index++ )
   {
      /* if any of the registered pids is running */
      if ( kill( mCctProcessPid[ Index ], M_CCT_SIGNAL_NONE ) 
              != M_CCT_OS_INVALID )
      {
         /* assume software is running */
         AppRunning = E_SYS_TRUE;

         /* log debug at debug level */
         eLogReport( E_LOG_DEBUG, 0, "Process pid %d running", 
                     mCctProcessPid[ Index ] );
      }
      /* else this process is not running */
      else
      {
         /* log debug at debug level */
         eLogReport( E_LOG_DEBUG, 0, "Process pid %d not running", 
                     mCctProcessPid[ Index ] );
      }
   }


   /* log debug at info level */
   eLogReport( E_LOG_DEBUG, 0, "Application running : %d", AppRunning );

   /* set status in internal parameters */
   iCctData.Param[ D_CCT_QRY_APP_RUNNING  ].Datum.Msrment.Value = AppRunning;

   /* set flag to ensure this is reported into the SDB next time */
   iCctData.Param[ D_CCT_QRY_APP_RUNNING  ].StatusObtained = TRUE;

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctAppStart
**
** Purpose:
**    Function to start the application software on the computer.
**
** Description:
**    This function is called in response to a 'set' command requesting the 
**    starting of the application software. The command-line(s) to start the 
**    application will be read from a configuration file. The demand flag will 
**    then be cleared and the flag set to ensure that this datum is submitted 
**    into the SDB on the next submission cycle.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctAppStart        ( void )
{
   Status_t Status;                    /* completion status of this function */

   /*
   ** read in configuration file and start application software
   */

   Status = mCctRunConfigFile( eCluCommon.ConfigFile, &mCctProcessCount, 
                               mCctProcessPid );


   /* clear the demand in the internal parameters */
   iCctData.Param[ D_CCT_DMD_APP_START    ].Datum.Msrment.Value = E_SYS_FALSE;

   /* set flag to ensure this is reported into the SDB next time */
   iCctData.Param[ D_CCT_DMD_APP_START    ].StatusObtained = TRUE;

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctAppKill 
**
** Purpose:
**    Function to kill the application software on the computer.
**
** Description:
**    This function is called in response to a 'set' command requesting the 
**    killing of the application software. This function will have been 
**    requested if the application has failed to respond to a shutdown message,
**    and it will then be necessary to kill it using the operating system. The 
**    demand flag will then be cleared and the flag set to ensure that this 
**    datum is submitted into the SDB on the next submission cycle.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctAppKill         ( void )
{
   int      Index;                     /* index to array of pids */
   int      ShutProcessCount;          /* number of processes run in shutdown */
   pid_t    ShutProcessPid             /* pids of commands in shutdown file */
               [ M_CCT_MAX_PIDS ];
   Status_t Status;                    /* completion status of this function */


   /*
   ** if a shutdown configuration has been supplied, then run the commands in 
   ** it, otherwise kill the application software using the operating system
   */

   /* assign initial conditions */
   Status = SYS_NOMINAL;
   ShutProcessCount = 0;

   /* if a shutdown configuration file has been supplied */
   if ( strlen( iCctData.ConfigShutdown ) > 0 )
   {
      /* run the commands in the configuration file */
      Status = mCctRunConfigFile( iCctData.ConfigShutdown, &ShutProcessCount,
                                  ShutProcessPid );
   }
   /* else kill application software using the pids held */
   else
   {
      /* loop through the registered process ids */
      for ( Index = 0; Index < mCctProcessCount; Index++ )
      {
         /* if the process is running (ie pid is not -1), and pid is non-zero */
         if ( ( mCctProcessPid[ Index ] != M_CCT_PID_INVALID ) 
               && ( mCctProcessPid[ Index ] != 0 ) )
         {
            /* if the process actually still exists */
            if ( kill( mCctProcessPid[ Index ], M_CCT_SIGNAL_NONE ) 
                  != M_CCT_OS_INVALID )
            {
               /* send kill message to the process */
               if ( kill( mCctProcessPid[ Index ], SIGKILL ) 
                     != M_CCT_PID_INVALID )
               {
                  /* log the kill at notice level */
                  eLogReport( E_LOG_NOTICE, 0, "Killed process with pid '%d'", 
                              mCctProcessPid[ Index ] );
               }
               /* else the kill was unsuccessful */
               else
               {
                  /* if kill has been unsuccessful, log the error */
                  eLogReport( E_LOG_ERR, E_CCT_ERR_APP_KILL,
                              "Unable to kill process with pid '%d'", 
                              mCctProcessPid[ Index ] );
               }
            }
            else
            { 
               /* log message about the process no longer exists */
               eLogReport( E_LOG_INFO, 0, 
                           "Requested kill of process with pid '%d', " 
                           "process no longer exists", 
                           mCctProcessPid[ Index ] );
            }

            /* set pid for this process to invalid */
            mCctProcessPid[ Index ] = M_CCT_PID_INVALID;
         }
      }
   }

   /* clear the number of processes running */
   mCctProcessCount = 0;


   /* clear the demand in the internal parameters */
   iCctData.Param[ D_CCT_DMD_APP_KILL     ].Datum.Msrment.Value = E_SYS_FALSE;

   /* set flag to ensure this is reported into the SDB next time */
   iCctData.Param[ D_CCT_DMD_APP_KILL     ].StatusObtained = TRUE;

   return Status;

}


/*******************************************************************************
** Function Name:
**    iCctOsRestart
**
** Purpose:
**    Function to shutdown the operating system on the computer, with an auto-
**    restart.
**
** Description:
**    This function is called when it is necessary to shut down the operating 
**    system on the computer, and perform an auto-restart of the operating 
**    system. It will be expected that any application software on the computer
**    will have already been shut down or killed at this point. The demand flag
**    will then be cleared and the flag set to ensure that this datum is 
**    submitted into the SDB on the next submission cycle.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctOsRestart       ( void )
{


   /* 
   ** close down the operating system with auto-restart
   */

   /* log the restart at notice level */
   eLogReport( E_LOG_NOTICE, 0, "Closing operating system with auto-restart" );

   /* send reply to command before the operating system is closed down */
   iCctSetReply( &iCctData.RxMsg, SYS_NOMINAL );
   iCctData.ReplySent = TRUE;

   /* shutdown operating system with restart */
   if ( system( M_CCT_CMD_RESTART ) == M_CCT_OS_INVALID )
   {
      /* the shutdown has been unsuccessful, log the error */
      eLogReport( E_LOG_ERR, E_CCT_ERR_OS_RESTART, 
                  "Unable to close operating system for auto-restart" );
   }

   /* clear the demand in the internal parameters */
   iCctData.Param[ D_CCT_DMD_OS_RESTART   ].Datum.Msrment.Value = E_SYS_FALSE;

   /* set flag to ensure this is reported into the SDB next time */
   iCctData.Param[ D_CCT_DMD_OS_RESTART   ].StatusObtained = TRUE;

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctOsClose
**
** Purpose:
**    Function to shutdown the operating system on the computer, with no auto-
**    restart.
**
** Description:
**    This function is called when it is necessary to shut down the operating 
**    system on the computer with no auto-restart. It will be expected that any
**    application software on the computer will have already been shut down or 
**    killed at this point. The demand flag will then be cleared and the flag 
**    set to ensure that this datum is submitted into the SDB on the next 
**    submission cycle.
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctOsClose         ( void )
{


   /*
   ** close down the operating system with no restart
   */

   /* log the closure at notice level */
   eLogReport( E_LOG_NOTICE, 0, "Closing operating system down" );

   /* send reply to command before the operating system is closed down */
   iCctSetReply( &iCctData.RxMsg, SYS_NOMINAL );
   iCctData.ReplySent = TRUE;

   /* shutdown operating system with no restart */
   if ( system( M_CCT_CMD_CLOSE ) == M_CCT_OS_INVALID )
   {
      /* the shutdown has been unsuccessful, log the error */
      eLogReport( E_LOG_ERR, E_CCT_ERR_OS_CLOSE, 
                  "Unable to close operating system down" );
   }


   /* clear the demand in the internal parameters */
   iCctData.Param[ D_CCT_DMD_OS_CLOSE     ].Datum.Msrment.Value = E_SYS_FALSE;

   /* set flag to ensure this is reported into the SDB next time */
   iCctData.Param[ D_CCT_DMD_OS_CLOSE     ].StatusObtained = TRUE;

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    mCctRunConfigFile
**
** Purpose:
**    Runs the commands in a configuration file.
**
** Description:
**    Accepts the name of a configuration file, and runs the commands in it. The
**    number of processes (commands) is returned, and the pids returned by each 
**    execution are stored in array for any required re-use.
**    Each line of the config file is assumed to be a program image under QNX
**    or a DCL script under VMS.
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Arguments:
**    char * ConfigFile                (in)
**       The name of the configuration file containing the commands to run.
**    int * ProcessCount               (out)
**       The pointer to the number of processes executed by this function.
**    pid_t * ProcessPid               (out)
**       Array of pids obtained when executing the processes in the config file.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Mark Bowman    (mkb)
**
*******************************************************************************/

Status_t mCctRunConfigFile   ( char * ConfigFile, int * ProcessCount, 
                               pid_t * ProcessPid )
{
   int      ExecStatus;                /* status returned by exec function */
   Status_t Status;                    /* general status */
   Status_t CfuStatus;                 /* status returned of CFU functions */
   Status_t TimStatus;                 /* status returned of TIM functions */
   eTtlTime_t CurrentTime;             /* copy of the actual time taken */
   eTtlTime_t ContinueTime;            /* time when delay complete */
   char     Buffer[ E_CFU_STRING_LEN ];/* buffer used to retrieve text into */
   char     Copy  [ E_CFU_STRING_LEN ];/* copy of command-line */               


   /* register with the configuration file utilities (CFU) */
   CfuStatus = eCfuSetup( ConfigFile );
   
   /* if setup was successful, read lines from file */
   if ( CfuStatus == SYS_NOMINAL )
   {
      /* loop until no lines left to read */
      do
      {
         /* retrieve a line from the configuration file */
         CfuStatus = eCfuGetLine( Buffer );

         /* if line retrieved OK */
         if ( CfuStatus == SYS_NOMINAL )
         {
            /* check maximum number of process to start */
            if ( *ProcessCount < ( M_CCT_MAX_PIDS - 1 ) )
            {
               /* take copy of the command-line */
               strcpy( Copy, Buffer );

               /* split arguments up - pointed to by static array 'mCctArgV' */
               mCctSplitArgs( Buffer );

#ifndef __VMS

#ifdef __QNX__
               /* ignore child signal to avoid leaving zombie processes */
               signal( SIGCHLD, SIG_IGN );
#endif
               /* fork a new instance of this process, saving the pid */
               ProcessPid[ *ProcessCount ] = fork();

               /* switch depending on the fork status */
               switch ( ProcessPid[ *ProcessCount ] )
               {
                  /* parent process - fork unsuccessful */
                  case M_CCT_OS_INVALID :

                     /* log the error */
                     eLogReport( E_LOG_ERR, E_CCT_ERR_APP_START, 
                                 "Unable to fork process" );
                     break;

                  /* this is now the child process */
                  case 0 :

                     /* attempt to replace the child with the new process */
                     ExecStatus = execv( mCctArgV[ M_CCT_ARG_PROGNAME ], 
                                         mCctArgV );
                     /* if the new process wasn't executed, we need to close */
                     if ( ExecStatus == M_CCT_OS_INVALID )
                     {
                        /* delay to ensure order of parent/child log messages */
                        sleep( 1 );

                        /* log an error */
                        eLogReport( E_LOG_ERR, E_CCT_ERR_APP_START, 
                                    "Unable to execute process '%s', errno %d", 
                                    Copy, errno );
                        /* exit with failure */
                        exit( EXIT_FAILURE );
                     }
                     /* else process was excuted - should never get here */
                     else
                     {
                        /* should never get here as new program running */
                     }
                     break;

                  /* parent process - we have forked a child process */
                  default :

                     /* log fork and execution at info status */
                     eLogReport( E_LOG_NOTICE, 0, 
                                 "Process %d created to execute '%s'",
                                 ProcessPid[ *ProcessCount ], Copy );

                     /* increment the process count */
                     ( *ProcessCount )++;
                     break;
               }
                
#else

               /* Send line from configuration file to DCL CLI. */
               ExecStatus = system(Copy);


               /* log execution at info status */
               eLogReport( E_LOG_NOTICE, 0, "Executed '%s'",
                           Copy);

               /* check if execution was successful */
               if ( ExecStatus != M_CCT_OS_INVALID )
               {
                  /* increment the process count */
                  ( *ProcessCount )++;
               }
               /* else execution was unsuccessful */
               else
               {
                  eLogReport( E_LOG_ERR, E_CCT_ERR_APP_START, 
                              "Unable to execute process '%s', error %d", 
                              Copy, ExecStatus );
               }

#endif

               /* get copy of the current time */ 
               TimStatus = eTimGetTime( &CurrentTime );

               /* determine time after which delay will be complete */
               TimStatus = eTimSum( &CurrentTime, &iCctData.TimeDelayExe, 
                                    &ContinueTime );

               /* delay after executing the process until the completion time */
               do
               {
                  /* check for and handle any incoming messages */
                  Status = iCctReceiveMessage( I_CCT_CIL_RX_TIMEOUT );

                  /* get the current time */
                  TimStatus = eTimGetTime( &CurrentTime );
               }
               while ( eTimCompare( &ContinueTime, &CurrentTime ) 
                        == E_TIM_TIMEA_GT_TIMEB );
              
            }
            /* else maximum number of process already reached */
            else
            {
               eLogReport( E_LOG_ERR, E_CCT_ERR_APP_COUNT, 
                           "Maximum number of processes already started" );
            }
         }
      }
      while ( CfuStatus == SYS_NOMINAL );
   }
   /* else problem opening file */
   else
   {
      eLogReport( E_LOG_ERR, E_CCT_ERR_CONFIG_FILE, 
                  "Unable to open configuration file '%s'", 
                  ConfigFile );
      return CfuStatus;
   }

   /* complete use of configuration file */
   CfuStatus = eCfuComplete();

   return CfuStatus;

}


/*******************************************************************************
** Function Name:
**    mCctSplitArgs
**
** Purpose:
**    Splits command-line arguments for a spawned process into 'argv'.
**
** Description:
**    Accepts the command-line for a spawned process as a single string, and 
**    extracts the arguments into the vector format associated with 'argv'. The 
**    memory for the vector of string pointers is statically allocated, local 
**    to this module. The argument vector list uses a NULL pointer as an end 
**    marker. 
**
** Return Type:
**    char **
**       Returns pointer to array of pointers to strings.
**
** Arguments:
**    char * CommandPtr                (in/out)
**       Original command-line - note that this is likely to be modified, so a 
**       copy should be submitted.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

void     mCctSplitArgs       ( char * CommandPtr )
{
   int      ArgCount;                  /* counter to arguments found on line */
   char   * CharPtr;                   /* pointer to a character string */

   /* initially assigning argument number zero */
   ArgCount = 0;

   /* find first argument, terminated by a space */
   CharPtr = strtok( CommandPtr, M_CCT_STR_SPACE );

   /* loop while arguments are present */
   while ( CharPtr != NULL )
   {
      /* assign pointer argument in pointer array */
      if ( ArgCount < ( E_CFU_STRING_LEN - 1 ) )
      {
         mCctArgV[ ArgCount++ ] = CharPtr;
      }

      /* check for next argument */
      CharPtr = strtok( NULL, M_CCT_STR_SPACE );
   }

   /* set end-of-argument marker as NULL-pointer */
   mCctArgV[ ArgCount ] = NULL;

}


/*
** EOF
*/
