/*******************************************************************************
** Module Name:
**    AgsInit.c
**
** Purpose:
**    Initialisation module of the Services PLC Task.
**
** Description:
**    The initialisation module of the AGS parses the command-line arguments, 
**    sets up the initial state of local data, and initialises using the 
**    configuration file.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: AgsInit.c,v 0.7 2005/10/04 12:40:53 sxf Exp $
**
** History:
**    $Log: AgsInit.c,v $
**    Revision 0.7  2005/10/04 12:40:53  sxf
**    Brought in line with QNX4 baseline AGS_0_17.
**
**    Revision 0.6  2002/01/21 09:11:26  sxf
**    Ensure Ags goes to OKAY state after initialisation complete.
**
**    Revision 0.5  2001/12/13 15:18:30  sxf
**    Initialise the time stamps for all data in data table.
**
**    Revision 0.4  2001/12/06 11:14:26  sxf
**    Remove unused variables.
**
**    Revision 0.3  2001/12/06 10:34:01  sxf
**    Remove code to check for Ags.cfg init file.
**    ,
**
**    Revision 0.2  2001/10/17 15:39:06  sxf
**    First version with basic functionality.
**
**    Revision 0.1  2001/09/18 14:09:05  sxf
**    Initial version.
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
#include "Hti.h"

/*
** Local include files
*/

#include "Ags.h"
#include "AgsPrivate.h"

/* Module functions. */
Status_t mAgsSetOid ( Int32_t DatumId,
                      Int32_t Value );

/*******************************************************************************
** Function Name:
**    iAgsSetup
**
** Purpose:
**    Initialisation function of the Autoguider Service Task.
**
** Description:
**    Performs all AGS initialisation. Parses the command-line, signs on, and 
**    initialises using the configuration file.
**
** Return Type:
**    Status_t    
**       Returns the completion status of AGS initialisation.
**
** Arguments:
**    int ArgCount                     (in)
**       Number of arguments on the command-line including executable name.
**    char *ArgPtr[]                   (in)
**       Array of null-terminated character strings containing arguments.
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsSetup( int ArgCount, char *ArgPtr[] )
{
   Status_t   Status;                  /* Returned status of this function */
   eTtlTime_t CurrentTime;             /* Current time for timeouts setup */
   int        Index;                   /* Index over OIDs. */

   /* Initial status for this function is SYS_NOMINAL */
   Status = SYS_NOMINAL;

   /* Data for command-line utilities */
   eCluProgNamePtr    = I_AGS_PROGRAM_NAME;
   eCluProgAboutPtr   = I_AGS_PROGRAM_ABOUT;
   eCluReleaseDatePtr = I_AGS_RELEASE_DATE;
   eCluYearPtr        = I_AGS_YEAR;
   eCluMajorVer       = I_AGS_MAJOR_VERSION;
   eCluMinorVer       = I_AGS_MINOR_VERSION;
 
   /* Initialise command queue pointer to NULL. */
   iAgsCmdQueuePtr = ( iAgsCmd_t  *)(NULL);
 
   /* Set defaults for AGS data store, process state has been set already. */
   iAgsData.DemandState = SYS_INIT_STATE; /* No safe state request yet. */
   iAgsData.HeartbeatCount     = 0;  /* No heartbeats yet. */
   iAgsData.HeartbeatLossCount = 0;  /* No heartbeats yet. */
   iAgsData.SdbSubmitReq = TRUE;     /* Do submit to the SDB first loop. */
   iAgsData.AggPid       = 0;        /* Before AGG reports its PID set to 0. */
   iAgsData.AggCmdSeqNum = I_AGS_NULL_SEQNUM; /* Initialise sequence no */

   /* Get the current time */
   Status = eTimGetTime( &CurrentTime );
   if(Status != SYS_NOMINAL)
   {
      printf(I_AGS_PROGRAM_NAME ": <%x>Error getting system time\n", Status);
      return Status;
   }

   /* Set time-stamps for checking for heartbeat time-out */
   Status = eTimCopy( CurrentTime, &iAgsData.LastHeartbeatTime );

   /* Parse the command-line */
   Status = iAgsParseCommandLine( ArgCount, ArgPtr );
   if ( Status != SYS_NOMINAL )
   {
      printf(I_AGS_PROGRAM_NAME ": <%x> Error parsing command-line\n", Status);
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
                        &iAgsData.CilId );
   if ( Status != SYS_NOMINAL )
   {
      /* If not found log the error */
      eLogCrit( Status, 
                "Invalid CIL name '%s' specified for AGS", 
                eCluCommon.CilName );
      return Status;
   }

   /* Setup the CIL communications port */
   Status = eCilSetup( eCluCommon.CilMap, iAgsData.CilId );
   /* Check status returned */
   if ( Status != SYS_NOMINAL )
   {
      switch ( Status )
      {
         case E_CIL_MAP_NOT_FOUND: 
            eLogCrit( Status, 
                      "Error AGS unable to setup CIL with map '%s'", 
                      eCluCommon.CilMap );
            break;

         case E_CIL_SOCKET_FAIL: 
         case E_CIL_ADDRINUSE: 
            eLogCrit( Status, "Error AGS CIL socket or ID already in use" );
            break;

         case E_CIL_ADDRNOTAVAIL:
            eLogCrit( Status, "Error binding AGS CIL socket to IP address" );
            break;

         default :
            eLogCrit( Status, "Error setting up CIL communications" );
            break;
      }
      return Status;
   }

   /* Set the initial time-stamp for data in the table */
   for ( Index = D_AGS_DATAID_BOL; Index < D_AGS_DATAID_EOL; Index++ )
   {
      /* Copy the determined 'current time' into the table */
      memcpy( &iAgsOidTable[ Index ].TimeStamp, &CurrentTime, 
              sizeof( CurrentTime ) );
   }

   /* Prepare configuration data from the file */
   Status = iAgsProcessConfig();
   if ( Status != SYS_NOMINAL )
   {
      eLogInfo("Unable to process AGS configuration file");
      /* Note that we do not exit on this one. Just note it, that's all */
   }

   /* Defaults for heartbeat response message. */
   iAgsData.ChbResponseMsg.SourceId   = iAgsData.CilId;
   iAgsData.ChbResponseMsg.DestId     = I_AGS_CIL_CHB;
   iAgsData.ChbResponseMsg.Class      = E_CIL_RSP_CLASS;
   iAgsData.ChbResponseMsg.Service    = E_MCP_HEARTBEAT;
   iAgsData.ChbResponseMsg.SeqNum     = 0;
   iAgsData.ChbResponseMsg.DataLen    = E_MCP_HEARTBEAT_RSP_LEN;

   /* Defaults for generic reply message. */
   iAgsData.ReplyMsg.SourceId         = iAgsData.CilId;
   iAgsData.ReplyMsg.DestId           = I_AGS_CIL_MCP;
   iAgsData.ReplyMsg.Class            = E_CIL_COM_CLASS;
   iAgsData.ReplyMsg.Service          = 0;
   iAgsData.ReplyMsg.SeqNum           = 0;
   iAgsData.ReplyMsg.DataLen          = sizeof( Int32_t );

   /* Defaults for single SDB submission command. */
   iAgsData.SdbSubmissionMsg.SourceId = iAgsData.CilId;
   iAgsData.SdbSubmissionMsg.DestId   = I_AGS_CIL_SDB;
   iAgsData.SdbSubmissionMsg.Class    = E_CIL_CMD_CLASS;
   iAgsData.SdbSubmissionMsg.Service  = E_SDB_SUBMIT_1;
   iAgsData.SdbSubmissionMsg.SeqNum   = 0;
   iAgsData.SdbSubmissionMsg.DataLen  = sizeof( Int32_t ) + 
   ( ( D_AGS_DATAID_EOL - D_MCP_FIRST_USER_DATUM ) * sizeof( eSdbDatum_t ) );


   /* If we get this far, then all is well, and we may return with success */
   iAgsSetState( SYS_INIT_STATE );

   eLogDebug("AGS setup complete");

   return SYS_NOMINAL;

}  /* End of iAgsSetup() */


/*******************************************************************************
** Function Name:
**    iAgsParseCommandLine
**
** Purpose:
**    Parses the command-line of the AGS.
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
**    Stephen Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iAgsParseCommandLine( int ArgCount, char *ArgPtr[] )
{
   char *ParamPtr;     /* Pointer to parameter of an argument */
   Status_t Status;    /* Return value of functions */
   char AggCilName[ 10 ]; /* Cil name for the guide process */
   Int32_t Id;         /* Temporary cil id */

   /* Initial status for this function is SYS_NOMINAL */
   Status = SYS_NOMINAL;
   
   /* Pointer to argument parameter initially NULL */
   ParamPtr = NULL;

   /* Set default parameters before parsing command-line */
   eCluCommon.Quiet             = I_AGS_DFLT_QUIET;
   eCluCommon.Verbose           = I_AGS_DFLT_VERBOSE;
   eCluCommon.Syslog            = I_AGS_DFLT_SYSLOG;
   eCluCommon.DebugLevel        = I_AGS_DFLT_DEBUG;
   eCluCommon.Priority          = I_AGS_DFLT_PRIORITY;
   eCluCommon.Help              = I_AGS_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, I_AGS_DFLT_CONFIG  );
   strcpy( eCluCommon.LogFile,    I_AGS_DFLT_LOG     );
   strcpy( eCluCommon.CilName,    I_AGS_DFLT_CIL     );
   strcpy( eCluCommon.CilMap,     I_AGS_DFLT_CIL_MAP );

   /* Register with the command-line utilities (CLU), ignore no common flags */
   Status = eCluSetup( ArgCount, ArgPtr, E_CLU_IGN_NONE );

   /* Parse the incoming command-line for common arguments */
   if ( Status == SYS_NOMINAL )
   {
      Status = eCluParseArgs( NULL );
   }

   /*
   ** Parse command line for custom arguments.
   */
   iAgsData.AggCilId = I_AGS_CIL_AGG;

   if ( eCluCustomArgExists( I_AGS_ARG_GUIDE ) == E_CLU_ARG_SUPPLIED )
   {
      /* Get pointer to parameter */
      ParamPtr = eCluGetCustomParam( I_AGS_ARG_GUIDE );

      /* If parameter supplied */
      if ( ParamPtr != NULL )
      {
         /* Copy from string into data area */
         strncpy( AggCilName, ParamPtr, 4 );
      }
    
      Status = eCilLookup( eCluCommon.CilMap, AggCilName, &Id );
      
      if ( Status == SYS_NOMINAL )
      {
         iAgsData.AggCilId = Id;
      }
      else
      {
         eLogErr( 0,
           "Unable to lookup CIL name \"%s\", reverting to default guide process AGG", 
                  AggCilName );
      }                   
   }

   return Status;

}  /* End of iAgsParseCommandLine() */



/*******************************************************************************
** Function Name:
**    iAgsProcessConfig
**
** Purpose:
**    Processes to configuration file for the AGS.
**
** Description:
**    Ags currently has no configuration file so this routine is
**    just a shell.
**
** Return Type:
**    Status_t
**       Returns the result of this function - SYS_NOMINAL if no error 
**       otherwise and error code if there is a problem.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iAgsProcessConfig( void )
{
   Status_t Status;                    /* Status of this function to return */

   /* Initially status is nominal */
   Status = SYS_NOMINAL;


   return(Status);


}  /* End of iAgsProcessConfig() */





/*******************************************************************************
** Function Name:
**    mAgsSetOid
**
** Purpose:
**    Sets the value of a DatumId in the OID table.
**
** Description:
**    The DatumId is the index into the OID table, set the value at 
**    that index and update the timestamp.
**
** Return Type:
**    Status_t
**       Returns the function completion status.
**
** Arguments:
**    Int32_t DatumId            (in)
**       Index of element in distributed data array.
**    Int32_t Value              (in)
**       Value to which the datum should be set.
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t mAgsSetOid
(
   Int32_t DatumId,
   Int32_t Value
)
{
   /* Local variables */
   Status_t Status;          /* Status returned by functions */


   /* Check that the specified index wont cause a memory fault */
   if( (DatumId <= D_AGS_DATAID_BOL) || (DatumId >= D_AGS_DATAID_EOL) )
   {
      return E_AGS_INVALID_DATUM;
   }

   /* Check to see if the value has changed, and mark it changed. */
   iAgsCheckAndSet(DatumId, Value);

   /* Set a timestamp, associated with this datum */
   Status = eTimGetTime(&iAgsOidTable[DatumId].TimeStamp);

   /* Return the status of the time stamp setting */
   return Status;

}  /* End of mAgsSetOid() */



/*******************************************************************************
** End of File Name: AgsInit.c
*******************************************************************************/
