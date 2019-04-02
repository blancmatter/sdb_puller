/*******************************************************************************
** Module Name:
**    CmtInit.c
**
** Purpose:
**    Initialisation module of the Computer Monitoring Task.
**
** Description:
**    The initialisation module of the CMT parses the command-line arguments, 
**    sets up the initial state of local data, and initialises using the 
**    configuration file.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.01 1-Nov-2000.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: CmtInit.c,v 1.7 2003/10/07 09:36:25 sxf Exp $
**
** History:
**    $Log: CmtInit.c,v $
**    Revision 1.7  2003/10/07 09:36:25  sxf
**    Move submission of version submission to after Cil setup.
**
**    Revision 1.6  2003/10/07 08:45:03  sxf
**    Submit process version.
**
**    Revision 1.5  2003/02/06 12:02:10  sxf
**    Initialise everything as E_SDB_UNSUPPORTED_UNITS.
**    Read in disk free warning trigger points (optional).
**
**    Revision 1.4  2002/08/01 10:46:41  sxf
**    Added reading of configuration file.
**    ,
**
**    Revision 1.3  2001/03/16 21:29:14  asp
**    Added hardware monitoring
**
**    Revision 1.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 1.1  2000/11/14 17:27:49  asp
**    Initial revision
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
#include "Cfu.h"
#include "Log.h"
#include "Tim.h"


/*
** Local include files
*/

#include "Cmt.h"
#include "CmtPrivate.h"


/*******************************************************************************
** Function Name:
**    iCmtSetup
**
** Purpose:
**    Initialisation function of the Computer Monitoring Task.
**
** Description:
**    Performs all CMT initialisation. Parses the command-line, signs on, and 
**    initialises using the configuration file.
**
** Return Type:
**    Status_t    
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    int ArgCount                     (in)
**       Number of arguments on the command-line including executable name.
**    char *ArgPtr[]                   (in)
**       Array of null-terminated character strings containing arguments.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iCmtSetup( int ArgCount, char *ArgPtr[] )
{
   int      Index;                     /* Array index into a loop */
   Status_t Status;                    /* Returned status of this function */
   eTtlTime_t CurrentTime;             /* Current time for timeouts setup */

   /* Initial status for this function is SYS_NOMINAL */
   Status = SYS_NOMINAL;

   /* Data for command-line utilities */
   eCluProgNamePtr    = I_CMT_PROGRAM_NAME;
   eCluProgAboutPtr   = I_CMT_PROGRAM_ABOUT;
   eCluReleaseDatePtr = I_CMT_RELEASE_DATE;
   eCluYearPtr        = I_CMT_YEAR;
   eCluMajorVer       = I_CMT_MAJOR_VERSION;
   eCluMinorVer       = I_CMT_MINOR_VERSION;
  
   /* Set defaults for CMT data store */
   iCmtData.HeartbeatCount     = 0;
   iCmtData.HeartbeatLossCount = 0;
   iCmtData.StatusRequestCount = 0;

   /* Set time-out after which safe-state is assumed if no heartbeats */
   iCmtData.HeartbeatTimeOut.t_sec  = (Int32_t) I_CMT_LOSS_HB_SEC;
   iCmtData.HeartbeatTimeOut.t_nsec = (Int32_t) I_CMT_LOSS_HB_NSEC;

   /* Get the current time */
   Status = eTimGetTime( &CurrentTime );
   if(Status != SYS_NOMINAL)
   {
      printf(I_CMT_PROGRAM_NAME ": <%x>Error getting system time\n", Status);
      return Status;
   }

   /* Set time-stamps for checking for heartbeat time-out */
   Status = eTimCopy( CurrentTime, &iCmtData.LastHeartbeat );
   Status = eTimSum( &iCmtData.LastHeartbeat, &iCmtData.HeartbeatTimeOut, 
                     &iCmtData.HeartbeatLossTime );

   /* Parse the command-line */
   Status = iCmtParseCommandLine( ArgCount, ArgPtr );
   if ( Status != SYS_NOMINAL )
   {
      printf(I_CMT_PROGRAM_NAME ": <%x> Error parsing command-line\n", Status);
      return Status;
   }
         
   /* Only continue if help not requested */
   if ( eCluCommon.Help == TRUE )
   {
      /* Help message requested. We exit at this point */
      return SYS_NOMINAL;
   }      

   /* Sign-on - note that LOG is set up within here */
   Status = eCluSignOn( NULL, NULL );
   if(Status != SYS_NOMINAL)
   {
      eLogCrit( Status, "Unable to sign the program on" );
      return Status;
   }
   
   /* Get the CIL ID using the name given */
   Status = eCilLookup( eCluCommon.CilMap, eCluCommon.CilName, 
                        &iCmtData.CilId );
   if ( Status != SYS_NOMINAL )
   {
      /* If not found log the error */
      eLogCrit( Status, 
                "Invalid CIL name '%s' specified for CMT", 
                eCluCommon.CilName );
      return Status;
   }

   /* Setup the CIL communications port */
   Status = eCilSetup( eCluCommon.CilMap, iCmtData.CilId );
   /* Check status returned */
   if ( Status != SYS_NOMINAL )
   {
      switch ( Status )
      {
         case E_CIL_MAP_NOT_FOUND : 
            eLogCrit( Status, 
                      "Error CMT unable to setup CIL with map '%s'", 
                      eCluCommon.CilMap );
            break;

         case E_CIL_SOCKET_FAIL : 
         case E_CIL_ADDRINUSE : 
            eLogCrit( Status, "Error CMT CIL socket or ID already in use" );
            break;

         case E_CIL_ADDRNOTAVAIL :
            eLogCrit( Status, "Error binding CMT CIL socket to IP address" );
            break;

         default :

            eLogCrit( Status, "Error setting up CIL communications" );
            break;
      }
      return Status;
   }

   /* Submit application version - note that CIL is set up within here */
   Status = eCluSubmitVersion();
   if ( Status != SYS_NOMINAL )
   {
      eLogCrit( Status, "Unable to submit application version" ); 
      return Status;
   }

   /* Prepare configuration data from the file */
   Status = iCmtProcessConfig();
   if ( Status != SYS_NOMINAL )
   {
      eLogWarning( Status, "Unable to process CMT configuration file" );
      /* Note that we do not exit on this one. Just note it, that's all */
   }

   /* Initialise static parameters for SDB submission - zero first */
   memset( &iCmtData.SdbParams, 0, sizeof( iCmtData.SdbParams ) );
   
   /* Set element count for SDB submission - always submit all elements */
   iCmtData.SdbParams.NumElts = D_CMT_DATAID_EOL - D_MCP_PROC_STATE;

   /* Initialise static parameters for each datum in a loop */
   for ( Index = D_CMT_DATAID_BOL; Index < D_CMT_DATAID_EOL; Index++ )
   {
      /* Assign the source and datum ID */
      iCmtData.SdbParams.Datum[ Index ].SourceId = iCmtData.CilId;
      iCmtData.SdbParams.Datum[ Index ].DatumId  = Index;
      iCmtData.SdbParams.Datum[ Index ].Units    = E_SDB_UNSUPPORTED_UNITS;
      iCmtData.SdbDatumChanged[ Index ]          = TRUE;
      /*
              iCmtSdbDatumType[ Index ]; 
      */
   }

   iCmtData.SdbParams.Datum[ D_CMT_PROC_STATE  ].Msrment.Value =
      SYS_OKAY_STATE;
      
   iCmtData.SdbParams.Datum[ D_CMT_AUTH_STATE  ].Msrment.Value = 
      E_MCP_AUTH_NONE;
      
   iCmtData.SdbParams.Datum[ D_CMT_SYS_REQUEST ].Msrment.Value =
       0;
  
   /* Initialise the version number */
   iCmtData.SdbParams.Datum[ D_CMT_APP_VERSION ].Msrment.Value =
       (I_CMT_MAJOR_VERSION * I_CMT_MVERSIONS_PER_VERSION) + 
        I_CMT_MINOR_VERSION;
        
   /* Defaults for heartbeat response message */
   iCmtData.ChbResponseMsg.SourceId   = iCmtData.CilId;
   iCmtData.ChbResponseMsg.DestId     = I_CMT_CIL_CHB;
   iCmtData.ChbResponseMsg.Class      = E_CIL_RSP_CLASS;
   iCmtData.ChbResponseMsg.Service    = E_MCP_HEARTBEAT;
   iCmtData.ChbResponseMsg.SeqNum     = 0;
   iCmtData.ChbResponseMsg.DataLen    = E_MCP_HEARTBEAT_RSP_LEN;

   /* Defaults for MCP message responses */
   iCmtData.McpReplyMsg.SourceId      = iCmtData.CilId;
   iCmtData.McpReplyMsg.DestId        = I_CMT_CIL_PMM;

   /* Defaults for command message responses */
   iCmtData.CmdReplyMsg.SourceId      = iCmtData.CilId;

   /* Defaults for message containing current status parameters */
   /* DEBUG - THIS WILL PROBABLY NOT BE USED */
   iCmtData.StatusMsg.SourceId        = iCmtData.CilId;
   iCmtData.StatusMsg.DestId          = I_CMT_CIL_PMM;
   iCmtData.StatusMsg.Class           = E_CIL_RSP_CLASS;
   iCmtData.StatusMsg.Service         = 0;
   iCmtData.StatusMsg.SeqNum          = 0;
   iCmtData.StatusMsg.DataLen         = sizeof( Int32_t );

   /* Defaults for single SDB submission command */
   iCmtData.SdbSubmissionMsg.SourceId = iCmtData.CilId;
   iCmtData.SdbSubmissionMsg.DestId   = I_CMT_CIL_SDB;
   iCmtData.SdbSubmissionMsg.Class    = E_CIL_CMD_CLASS;
   iCmtData.SdbSubmissionMsg.Service  = E_SDB_SUBMIT_1;
   iCmtData.SdbSubmissionMsg.SeqNum   = 0;
   iCmtData.SdbSubmissionMsg.DataLen  = sizeof( Int32_t ) + 
   ( ( D_CMT_DATAID_EOL - D_MCP_PROC_STATE ) * sizeof( eSdbDatum_t ) );

   /*
   ** Detect hardware monitoring chip
   */

   eLogDebug( "Detecting hardware" );
   iCmtDetectHardware();
   eLogDebug( "Hardware detected OK" );

   /* If we get this far, then all is well, and we may return with success */
   iCmtSetState( SYS_OKAY_STATE );
   eLogDebug("CMT setup complete");
   return SYS_NOMINAL;

}  /* End of iCmtSetup() */


/*******************************************************************************
** Function Name:
**    iCmtParseCommandLine
**
** Purpose:
**    Parses the command-line of the CMT.
**
** Description:
**    Initially sets default command-line options, and uses the CLU package to 
**    parse common arguments, then specifically check for custom arguments. 
**
** Return Type:
**    Status_t
**       Returns the result of this function - SYS_NOMINAL if no error 
**       otherwise and error code if there is a problem in command-line
**       parameters.
**
** Arguments:
**    int ArgCount                     (in)
**       Number of arguments on the command-line including executable name.
**    char *ArgPtr[]                   (in)
**       Array of null-terminated character strings containing arguments.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iCmtParseCommandLine( int ArgCount, char *ArgPtr[] )
{
   char *ParamPtr;     /* Pointer to parameter of an argument */
   Status_t Status;    /* Return value of functions */


   /* Initial status for this function is SYS_NOMINAL */
   Status = SYS_NOMINAL;
   
   /* Pointer to argument parameter initially NULL */
   ParamPtr = NULL;

   /* Set default parameters before parsing command-line */
   eCluCommon.Quiet             = I_CMT_DFLT_QUIET;
   eCluCommon.Verbose           = I_CMT_DFLT_VERBOSE;
   eCluCommon.Syslog            = I_CMT_DFLT_SYSLOG;
   eCluCommon.DebugLevel        = I_CMT_DFLT_DEBUG;
   eCluCommon.Priority          = I_CMT_DFLT_PRIORITY;
   eCluCommon.Help              = I_CMT_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, I_CMT_DFLT_CONFIG  );
   strcpy( eCluCommon.LogFile,    I_CMT_DFLT_LOG     );
   strcpy( eCluCommon.CilName,    I_CMT_DFLT_CIL     );
   strcpy( eCluCommon.CilMap,     I_CMT_DFLT_CIL_MAP );

   /* Register with the command-line utilities (CLU), ignore no common flags */
   Status = eCluSetup( ArgCount, ArgPtr, E_CLU_IGN_NONE );
   
   /* Parse the incoming command-line for common arguments */
   if ( Status == SYS_NOMINAL )
   {
      Status = eCluParseArgs( NULL );

/*
** No longer have Port argument.
*/

#if __SXF__
      if ( eCluCustomArgExists( I_CMT_ARG_PORT ) )
      {
         /* Get pointer to parameter */
         ParamPtr = eCluGetCustomParam( I_CMT_ARG_PORT );
         /* If parameter supplied */
         if ( ParamPtr != NULL )
         {
            /* Copy from string into data area */
            iCmtData.Port = (int) strtol( ParamPtr, NULL, 0 );
         }
      }
#endif

   }

   return Status;

}  /* End of iCmtParseCommandLine() */



/*******************************************************************************
** Function Name:
**    iCmtProcessConfig
**
** Purpose:
**    Processes to configuration file for the CMT.
**
** Description:
**    Initialises data from the configuration file. Reads the configuration 
**    file to initialise the CMT parameters.
**
** Return Type:
**    Status_t
**       Returns the result of this function - SYS_NOMINAL if no error 
**       otherwise and error code if there is a problem.
**
** Arguments:
**    int ArgCount                     (in)
**       Number of arguments on the command-line including executable name.
**    char *ArgPtr[]                   (in)
**       Array of null-terminated character strings containing arguments.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iCmtProcessConfig( void )
{
   char              Line[ E_CFU_STRING_LEN ];   /* Line read in from file. */
   char              KeyWord[ E_CFU_STRING_LEN ];  /* Keyword part of line. */
   char              Value[ E_CFU_STRING_LEN ];      /* Value part of line. */
   char              PerCent[ E_CFU_STRING_LEN ]; /* Percent free threshold */
   int               Percentage;         /* Percent free threshold (number) */
   int               ItemFound;                   /* Indicate if item found */
   int               i;                                    /* Loop counter. */
   Bool_t            ChipFound;    /* Boolean to stop loop when chip found. */
   Status_t Status;                    /* Status of this function to return */

   /* Initially status is nominal */
   Status = SYS_NOMINAL;

   iCmtData.NumDisksToMonitor = 0;
   iCmtData.HwMonitorChip     = I_CMT_HWMON_AUTO;

   /* Attempt to setup use of the configuration file */
   Status = eCfuSetup( eCluCommon.ConfigFile );
   if(Status != SYS_NOMINAL)
   {

      eLogErr( Status,  
               "Error CMT unable to open configuration file '%s'", 
               eCluCommon.ConfigFile );
      return Status;

   }

   /* If we get this far, then the configuration file setup was okay */

   /* Loop reading a line until end of config file */
   while ( eCfuGetLine( Line ) == SYS_NOMINAL )
   {
     /* Parse for each keyword in turn */
     if ( ( eCfuGetParam( KeyWord ) == SYS_NOMINAL ) &&
          ( eCfuGetParam( Value )   == SYS_NOMINAL ) )
     {

       ItemFound = 0;
       
       if ( strncmp( KeyWord, 
                     "DISK",
                     strlen( KeyWord ) ) == 0  )
       {
          ItemFound = 1;

          eLogDebug("Found item with value \"%s\"", Value );

          if ( iCmtData.NumDisksToMonitor < I_CMT_DISK_MAX )
          {
             strncpy( iCmtData.DisksToMonitor[ iCmtData.NumDisksToMonitor ],
                      Value,
                      I_CMT_MAX_FILENAME_LEN );

             
             /*
             ** Initially set warning threshold for this disk to 0%
             */
             iCmtData.DiskFreeWarn[ iCmtData.NumDisksToMonitor ] = 0;

             /*
             ** If additional parameter on line then set disk free percent
             */
             if ( eCfuGetParam( PerCent ) == SYS_NOMINAL )
             {
                if ( sscanf( PerCent, "%d", &Percentage ) == 1 )
                {
                   if ( ( Percentage >= 0 ) && ( Percentage <= 100 ) )
                   {
                      iCmtData.DiskFreeWarn[ iCmtData.NumDisksToMonitor ] =
                          Percentage * 1000;
                   }
                   else 
                   {
                      eLogNotice( E_CMT_ERR_SETUP, "Config file error: \"%s\"",
                                  Line );
                   }
                }
                else
                {
                   eLogNotice( E_CMT_ERR_SETUP, "Config file error \"%s\"", 
                               Line );
                }
             }

             iCmtData.NumDisksToMonitor ++;

          }
          else
          {
             eLogInfo( 
               "Error in config file. Too many disks defined." );
          }

          eLogDebug( "Monitoring %d disks", iCmtData.NumDisksToMonitor );

       }

       if ( strncmp( KeyWord, 
                     "HWMON",
                     strlen( KeyWord ) ) == 0  )
       {
          ItemFound = 1;
    
          i = I_CMT_HWMON_BOL + 1;
          ChipFound = FALSE;

          do
          {
             if (strncmp( Value,
                          iCmtHwMonChipNames[ i ],
                          strlen( Value ) ) == 0 )
             {
                eLogDebug("Selecting hardware monitoring chip %s", 
                          iCmtHwMonChipNames[i] );

                iCmtData.HwMonitorChip = i;
                ChipFound = TRUE;
             }

             i++;

          }  while ( ( i < I_CMT_HWMON_EOL ) && ( ChipFound == FALSE ) );
          
          if ( ChipFound == FALSE )
          {

             eLogInfo( "Error in config file. Unknown HW monitor chip." );

          }
          else
          {
             eLogInfo( "HW monitor chip is %s", 
                       iCmtHwMonChipNames[ iCmtData.HwMonitorChip ] );
          }
       }

       if ( strncmp( KeyWord, 
                     "HWMETHOD",
                     strlen( KeyWord ) ) == 0  )
       {
          ItemFound = 1;
    
          if (strncmp( Value,
                       I_CMT_HWMETHOD_NAME_IOPORT,
                       strlen( Value ) ) == 0 )
          {
             eLogDebug("Using IO port method for hardware monitoring." );
             iCmtData.HwMonitorMethod = I_CMT_HWMETHOD_IOPORT;
          }
          else if (strncmp( Value,
                       I_CMT_HWMETHOD_NAME_SMBUS,
                       strlen( Value ) ) == 0 )
          {
             eLogDebug("Using SMBUS for hardware monitoring." );
             iCmtData.HwMonitorMethod = I_CMT_HWMETHOD_SMBUS;
          }
          else if (strncmp( Value,
                       I_CMT_HWMETHOD_NAME_NONE,
                       strlen( Value ) ) == 0 )
          {
             eLogDebug("No hardware monitoring." );
             iCmtData.HwMonitorMethod = I_CMT_HWMETHOD_NONE;
          }
          else
          {
             eLogInfo( "Error in config file. Unknown HW monitor method." );
          }

       }

       if ( ItemFound == 0 )
       {
         eLogErr( E_CMT_BADPARAM, 
                  "Configuration item not recognised, \"%s\"", KeyWord );
       }
     }
     else
     {
       eLogInfo( "Ignoring line %s", Line );
     }

   }


   return Status;

}  /* End of iCmtProcessConfig() */



/*******************************************************************************
** End of File Name: CmtInit.c
*******************************************************************************/
