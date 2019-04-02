/*******************************************************************************
** Module Name:
**    CctInit.c
**
** Purpose:
**    Initialisation module of the Computer Control Task.
**
** Description:
**    The initialisation module of the CCT parses the command-line arguments, 
**    sets up the initial state of local data, and initialises using the 
**    configuration file.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    -
**
** Author(s):
**    Martyn J. Ford (mjf)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: CctInit.c,v 0.8 2001/08/07 09:21:20 mjf Exp $
**
** History:
**    $Log: CctInit.c,v $
**    Revision 0.8  2001/08/07 09:21:20  mjf
**    Output execution delay in the sign-on message.
**
**    Revision 0.7  2001/08/07 08:31:56  mjf
**    Addition of delay following the execution of a process. Set to 1000 ms
**    by default, may be altered on the command-line.
**
**    Revision 0.6  2001/07/17 13:39:57  mjf
**    Addition of code for '-shutdown' switch.
**
**    Revision 0.5  2001/05/25 13:30:29  mjf
**    On an operating system shutdown or restart command, send the reply to
**    the command before performing the requested action.
**
**    Revision 0.4  2001/03/14 14:57:28  mjf
**    Return improved status from command-line parse.
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
#include "Tim.h"


/*
** Local include files
*/

#include "Cct.h"
#include "CctPrivate.h"


/*******************************************************************************
** Function Name:
**    iCctSetup
**
** Purpose:
**    Initialisation function of the CCT.
**
** Description:
**    Performs all CCT initialisation. Parses the command-line, signs on, and 
**    initialises using the configuration file.
**
** Return Type:
**    Status_t    
**       Returns the completion status of CCT initialisation.
**
** Arguments:
**    int ArgCount                     (in)
**       Number of arguments on the command-line including executable name.
**    char *ArgPtr[]                   (in)
**       Array of null-terminated character strings containing arguments.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctSetup( int ArgCount, char *ArgPtr[] )
{
   int      Index;                     /* array index into a loop */
   Status_t Status;                    /* status of functions called */
   Status_t TimeStatus;                /* status of time functions */
   Status_t CommandLineStatus;         /* status of init command-line */
   Status_t CilStatus;                 /* status of init CIL */
   Status_t ConfigStatus;              /* status of init config file */
   Status_t CluStatus;                 /* status returned of CLU functions */
   eSdbUnit_t SdbUnits;                /* units for internal datums*/
   eTtlTime_t CurrentTime;             /* current time for timeouts setup */

   /* initial status for this function is SYS_NOMINAL */
   Status = SYS_NOMINAL;

   /* initially no services have been initialised */
   TimeStatus         = SYS_WARNING;
   CommandLineStatus  = SYS_WARNING;
   CilStatus          = SYS_WARNING;
   ConfigStatus       = SYS_WARNING;

   /* data for command-line utilities */
   eCluProgNamePtr    = I_CCT_PROGRAM_NAME;
   eCluProgAboutPtr   = I_CCT_PROGRAM_ABOUT;
   eCluReleaseDatePtr = I_CCT_RELEASE_DATE;
   eCluYearPtr        = I_CCT_YEAR;
   eCluMajorVer       = I_CCT_MAJOR_VERSION;
   eCluMinorVer       = I_CCT_MINOR_VERSION;
  
   /* set defaults for CCT data store */
   iCctData.DelayExe              = I_CCT_DFLT_EXE_DELAY;
   iCctData.HeartbeatCount        = 0;
   iCctData.HeartbeatLossCount    = 0;
   iCctData.ReplySent             = FALSE;

   /* set invalid CIL name */
   strcpy( iCctData.CilName, "" );

   /* set time-out after which safe-state is assumed if no heartbeats */
   iCctData.HeartbeatTimeOut.t_sec  = (Int32_t) I_CCT_LOSS_HB_SEC;
   iCctData.HeartbeatTimeOut.t_nsec = (Int32_t) I_CCT_LOSS_HB_NSEC;

   /* get the current time */
   TimeStatus = eTimGetTime( &CurrentTime );

   /* check time status */
   if ( TimeStatus != SYS_NOMINAL )
   {
      eLogReport( E_LOG_CRIT, TimeStatus, 
                  "Failure performing CCT setup - unable to get system time" );
      return TimeStatus;
   }

   /* set time-stamps for checking for heartbeat time-out */
   TimeStatus = eTimCopy( CurrentTime, &iCctData.LastHeartbeat );
   TimeStatus = eTimSum( &iCctData.LastHeartbeat, &iCctData.HeartbeatTimeOut, 
                         &iCctData.HeartbeatLossTime );

   /* write in the default shutdown config file - may get overwritten */
   strcpy( iCctData.ConfigShutdown, I_CCT_DFLT_SHUTDOWN );

   /* parse the command-line */
   CommandLineStatus = iCctParseCommandLine( ArgCount, ArgPtr );

   /* check status returned */
   if ( CommandLineStatus != SYS_NOMINAL )
   {
      fprintf( stderr, I_CCT_PROGRAM_NAME  
               /* note no comma, uses string concatenation */
               ": <%x> Unable to parse the command-line\n", 
               CommandLineStatus );
      return CommandLineStatus;
   }
         
   /* only continue if help not requested, then initialise services */
   if ( eCluCommon.Help == FALSE )
   {
      /* sign-on - note that LOG is set up within here */
      CluStatus = eCluSignOn( NULL, NULL );

      /* check CLU sign-on is OK */
      if ( CluStatus != SYS_NOMINAL )
      {
         /* attempt to log the error - note LOG setup may have failed though */
         eLogReport( E_LOG_ERR, CluStatus, 
                     "CLU sign-on not completed successfully" );
         /* log the error to 'stderr' - in case LOG setup has failed */
         fprintf( stderr, I_CCT_PROGRAM_NAME  
                  /* note no comma, uses string concatenation */
                  ": <%x> Unable to sign-on program\n",
                  CluStatus );
         return CluStatus;
      }

      /* perform CIL setup and submit version into SDB */
      CilStatus = eCluSubmitVersion();

      /* if an error setting up CIL */
      if ( CilStatus != SYS_NOMINAL )
      {
         /* set invalid CIL name */
         strcpy( iCctData.CilName, I_CCT_INVALID_CILSTR );

         /* log the error */
         eLogReport( E_LOG_CRIT, CilStatus, 
                     "Unable to setup CIL with name '%s' and map '%s'", 
                     eCluCommon.CilName, eCluCommon.CilMap );

         return CilStatus;
      }
      /* else CIL initialised OK */
      else
      {
         /* set CIL name and ID */
         strcpy( iCctData.CilName, eCluCommon.CilName );
         iCctData.CilId = eCilId();
      }

      ConfigStatus = SYS_NOMINAL;

      /* convert the consecutive execution delay into a TTL time type */
      iCctData.TimeDelayExe.t_sec = 
         ( iCctData.DelayExe / ( (Int32_t) E_TTL_MILLISECS_PER_SEC ) );
      iCctData.TimeDelayExe.t_nsec =
         ( iCctData.DelayExe % ( (Int32_t) E_TTL_MILLISECS_PER_SEC ) )
          * ( (Int32_t) ( E_TTL_NANOSECS_PER_SEC / E_TTL_MILLISECS_PER_SEC ) );

      /* initialise data local to the operating system interface */
      Status = iCctOsDataInit();

      /* initialise static parameters internal datums - zero first */
      memset( &iCctData.Param, 0, sizeof( iCctData.Param ) );

      /* set units for SDB submissions */
      SdbUnits = E_SDB_TRUEFALSE_UNITS;
      iCctData.Param[ D_CCT_QRY_OS_AVAILABLE ].Datum.Units = SdbUnits;
      iCctData.Param[ D_CCT_QRY_APP_PREPARED ].Datum.Units = SdbUnits;
      iCctData.Param[ D_CCT_QRY_APP_RUNNING  ].Datum.Units = SdbUnits;
      iCctData.Param[ D_CCT_DMD_APP_START    ].Datum.Units = SdbUnits;
      iCctData.Param[ D_CCT_DMD_APP_KILL     ].Datum.Units = SdbUnits;
      iCctData.Param[ D_CCT_DMD_OS_RESTART   ].Datum.Units = SdbUnits;
      iCctData.Param[ D_CCT_DMD_OS_CLOSE     ].Datum.Units = SdbUnits;

      /* set function handlers for set/get commands */
      iCctData.Param[ D_CCT_QRY_OS_AVAILABLE ].Function 
                                 = ( Status_t ( * )( void ) ) iCctOsAvailable;
      iCctData.Param[ D_CCT_QRY_APP_PREPARED ].Function 
                                 = ( Status_t ( * )( void ) ) iCctComputerReady;
      iCctData.Param[ D_CCT_QRY_APP_RUNNING  ].Function 
                                 = ( Status_t ( * )( void ) ) iCctAppRunning;  
      iCctData.Param[ D_CCT_DMD_APP_START    ].Function 
                                 = ( Status_t ( * )( void ) ) iCctAppStart;    
      iCctData.Param[ D_CCT_DMD_APP_KILL     ].Function 
                                 = ( Status_t ( * )( void ) ) iCctAppKill;
      iCctData.Param[ D_CCT_DMD_OS_RESTART   ].Function 
                                 = ( Status_t ( * )( void ) ) iCctOsRestart; 
      iCctData.Param[ D_CCT_DMD_OS_CLOSE     ].Function 
                                 = ( Status_t ( * )( void ) ) iCctOsClose;

      /* initialise static parameters for each datum in a loop */
      for ( Index = D_CCT_DATAID_BOL; Index < D_CCT_DATAID_EOL; Index++ )
      {
         /* assign the source ID */
         iCctData.Param[ Index ].Datum.SourceId = iCctData.CilId;

         /* assign the datum ID */
         iCctData.Param[ Index ].Datum.DatumId = Index;

         /* assign the modification time */
         memcpy( &iCctData.Param[ Index ].ModificationTime, &CurrentTime,
                 sizeof( CurrentTime ) );

         /* if datum is a query datum, then determine it for first submission */
         if ( ( Index >= E_CCT_QRY_MIN ) && ( Index <= E_CCT_QRY_MAX ) )
         {
            Status = iCctData.Param[ Index ].Function();
         }

         /* set flag to force initial SDB submission, pretend status obtained */
         iCctData.Param[ Index ].StatusObtained = TRUE;
      }

      /* defaults for heartbeat response message */
      iCctData.ChbResponseMsg.SourceId   = iCctData.CilId;
      iCctData.ChbResponseMsg.DestId     = I_CCT_CIL_CHB;
      iCctData.ChbResponseMsg.Class      = E_CIL_RSP_CLASS;
      iCctData.ChbResponseMsg.Service    = E_MCP_HEARTBEAT;
      iCctData.ChbResponseMsg.SeqNum     = 0;
      iCctData.ChbResponseMsg.DataLen    = E_MCP_HEARTBEAT_RSP_LEN;

      /* defaults for MCP message responses */
      iCctData.McpReplyMsg.SourceId      = iCctData.CilId;
      iCctData.McpReplyMsg.DestId        = I_CCT_CIL_PMM;

      /* defaults for command message responses */
      iCctData.CmdReplyMsg.SourceId      = iCctData.CilId;

      /* defaults for single SDB submission command */
      iCctData.SdbSubmissionMsg.SourceId = iCctData.CilId;
      iCctData.SdbSubmissionMsg.DestId   = I_CCT_CIL_SDB;
      iCctData.SdbSubmissionMsg.Class    = E_CIL_CMD_CLASS;
      iCctData.SdbSubmissionMsg.Service  = E_SDB_SUBMIT_1;
      iCctData.SdbSubmissionMsg.SeqNum   = 0;
      iCctData.SdbSubmissionMsg.DataLen  = sizeof( Int32_t ) + 
      ( ( D_CCT_DATAID_EOL - D_MCP_FIRST_USER_DATUM ) * sizeof( eSdbDatum_t ) );
   }

   /* output debug at notice level about runtime parameters */
   if ( strlen( iCctData.ConfigShutdown ) > 0 )
   {
      eLogReport( E_LOG_NOTICE, 0, 
                  "CCT '%s', config '%s', shutdown '%s', execution delay %d ms",
                  eCluCommon.CilName, eCluCommon.ConfigFile, 
                  iCctData.ConfigShutdown, iCctData.DelayExe );
   }
   else
   {
      eLogReport( E_LOG_NOTICE, 0, 
                  "CCT '%s', config '%s', shutdown using pids"
                  ", execution delay %d ms",
                  eCluCommon.CilName, eCluCommon.ConfigFile, 
                  iCctData.DelayExe );
   }

   /* return status - if no error move state from INIT to OKAY */
   Status = SYS_NOMINAL;
   iCctSetState( SYS_OKAY_STATE );
   eLogReport( E_LOG_INFO, 0, "Initialisation completed successfully" );
   
   return Status;

}


/*******************************************************************************
** Function Name:
**    iCctParseCommandLine
**
** Purpose:
**    Parses the command-line of the CCT.
**
** Description:
**    Initially sets default command-line options, and uses the CLU package to 
**    parse common arguments, then specifically check for custom arguments. 
**
** Return Type:
**    Status_t
**       Returns the result of this function - SYS_NOMINAL if no error 
**       otherwise or SYS_WARNING if problem in command-line parameters.
**
** Arguments:
**    int ArgCount                     (in)
**       Number of arguments on the command-line including executable name.
**    char *ArgPtr[]                   (in)
**       Array of null-terminated character strings containing arguments.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctParseCommandLine( int ArgCount, char *ArgPtr[] )
{
   char   * ParamPtr;                  /* pointer to parameter of an argument */
   Status_t CluStatus;                 /* returned status of CLU functions */

   /* pointer to argument parameter initially NULL */
   ParamPtr = NULL;

   /* set default parameters before parsing command-line */
   eCluCommon.Quiet             = I_CCT_DFLT_QUIET;
   eCluCommon.Verbose           = I_CCT_DFLT_VERBOSE;
   eCluCommon.Syslog            = I_CCT_DFLT_SYSLOG;
   eCluCommon.DebugLevel        = I_CCT_DFLT_DEBUG;
   eCluCommon.Priority          = I_CCT_DFLT_PRIORITY;
   eCluCommon.Help              = I_CCT_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, I_CCT_DFLT_CONFIG  );
   strcpy( eCluCommon.LogFile,    I_CCT_DFLT_LOG     );
   strcpy( eCluCommon.CilName,    I_CCT_DFLT_CIL     );
   strcpy( eCluCommon.CilMap,     I_CCT_DFLT_CIL_MAP );

   /* register with the command-line utilities (CLU), ignore no common flags */
   CluStatus = eCluSetup( ArgCount, ArgPtr, E_CLU_IGN_NONE );
   
   /* parse the incoming command-line for common arguments */
   if ( CluStatus == SYS_NOMINAL )
   {
      CluStatus = eCluParseArgs( NULL );
   }

   /* check for presence of configuration file for application shutdown */
   if ( eCluCustomArgExists( I_CCT_ARG_SHUTDOWN ) == E_CLU_ARG_SUPPLIED )
   {
      /* get pointer to parameter */
      ParamPtr = eCluGetCustomParam( I_CCT_ARG_SHUTDOWN );
      /* if parameter supplied */
      if ( ParamPtr != NULL )
      {
         /* copy from string into data area */
         strncpy( iCctData.ConfigShutdown, ParamPtr, E_CFU_STRING_LEN );
         iCctData.ConfigShutdown[ E_CFU_STRING_LEN - 1 ] = 0;
      }
   }

   /* check for presence of execution delay */
   if ( eCluCustomArgExists( I_CCT_ARG_EXE_DELAY ) == E_CLU_ARG_SUPPLIED )
   {
      /* get pointer to parameter */
      ParamPtr = eCluGetCustomParam( I_CCT_ARG_EXE_DELAY );
      /* if parameter supplied */
      if ( ParamPtr != NULL )
      {
         /* convert from string into data area */
         iCctData.DelayExe = strtol( ParamPtr, NULL, 0 );
      }
   }

   /* return the CLU status */
   return CluStatus;

}


/*******************************************************************************
** Function Name:
**    iCctProcessConfig
**
** Purpose:
**    Processes to configuration file for the CCT.
**
** Description:
**    Initialises data from the configuration file. Reads the configuration 
**    file to initialise the CCT parameters.
**
** Return Type:
**    Status_t
**       Returns the result of this function - SYS_NOMINAL if no error 
**       otherwise or SYS_WARNING if a problem found.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctProcessConfig( void )
{
   char     Buffer[ E_CFU_STRING_LEN ];/* line or param read from config file */
   Status_t Status;                    /* status of this function to return */
   Status_t ConfigFileStatus;          /* status of config file functions */

   /* initially status is nominal */
   Status = SYS_NOMINAL;

   /* attempt to setup use of the configuration file */
   ConfigFileStatus = eCfuSetup( eCluCommon.ConfigFile );

   /* if configuration file setup OK */
   if ( ConfigFileStatus == SYS_NOMINAL )
   {
      /* loop reading a line until end of config file */
      do
      {
         /* attempt to read a line from the file */
         ConfigFileStatus = eCfuGetLine( Buffer );

         /* if a line read OK */
         if ( ConfigFileStatus == SYS_NOMINAL )
         {


         }
      }
      while( ConfigFileStatus == SYS_NOMINAL );
   }
   /* else unable to open configuration file */
   else
   {
      /* report error to LOG */
      eLogReport( E_LOG_ERR, ConfigFileStatus,  
                  "Error CCT unable to open configuration file '%s'", 
                  eCluCommon.ConfigFile );
   }
   
   return Status;

}


/*
** EOF
*/
