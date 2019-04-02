/********************************************************************************
** Module Name:
**    AgsProcess.c
**
** Purpose:
**    Functions for processing incoming commands and system requests.
**
** Description:
**    This source file contains functions used by the AGS to process incoming
**    CIL commands that are specific to itself.  The processing done here is 
**    specific to AGS.
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
**    $Id: AgsProcess.c,v 0.29 2007/07/19 09:33:01 mjf Exp $
**
** History:
**    $Log: AgsProcess.c,v $
**    Revision 0.29  2007/07/19 09:33:01  mjf
**    Allow transition to OKAY state from INIT for new Agg simulate mode.
**
**    Revision 0.28  2007/07/19 09:29:24  sxf
**    Improvement to log message.
**
**    Revision 0.27  2006/10/11 09:41:54  sxf
**    Brought in line with AGS_0_19.
**
**    Revision 0.26  2006/10/03 15:37:43  sxf
**    Baseline AGS_1_04.
**
**    Revision 0.24  2006/01/31 14:31:15  sxf
**    Handle request to go SAFE without creating an error.
**    I n line with AGS_0_18.
**    ,
**
**    Revision 0.23  2005/10/04 12:40:53  sxf
**    Brought in line with QNX4 baseline AGS_0_17.
**
**    Revision 0.22  2004/04/16 09:31:00  mjf
**    Checking in of QNX v6 porting done by MAN.
**
**    Revision 0.21  2003/09/04 08:07:55  sxf
**    Remove unnecessary OID sets in INFO command handler.
**
**    Revision 0.20  2003/05/01 10:52:24  man
**    Pass the frame mean, frame rms and case temperature
**    from the agg to ags.
**
**    Revision 0.19  2003/01/23 09:35:14  man
**    Included two Oid's for CCD case temperature
**    and cooler power level.
**
**    Revision 0.18  2002/05/07 09:39:34  sxf
**    Return meaningful status to TCS on error returned from Agg.
**
**    Revision 0.17  2002/03/26 10:11:34  sxf
**    Report PEAKPIXEL in response to autoguide on command and GUIDEMAG
**    in response to centroid command.
**
**    Revision 0.16  2002/03/18 11:33:23  sxf
**    Integration time returned in response to CENTROID command not AUTOGUIDE ON.
**    ,
**
**    Revision 0.15  2002/03/05 16:38:37  sxf
**    Use process scope function iAgsDelCmdFromQueue to delete commands.
**    ,
**
**    Revision 0.14  2002/02/26 11:39:41  sxf
**    Add function iAgsProcessMonitorCommands to either monitor Agg with an
**    INFO command or both INFO and CENTROID command depending on the passed
**    parameter.
**
**    Revision 0.13  2002/02/05 10:52:08  sxf
**    Response to INFO command now contains temperature of CCD to report to SDB.
**
**    Revision 0.12  2002/02/05 08:21:14  sxf
**    Submit peak pixel in correct units.
**
**    Revision 0.11  2002/01/21 09:19:12  sxf
**    Tidy up debug level log messages.
**
**    Revision 0.10  2001/12/06 11:09:51  sxf
**    Do not attempt to enter WARN state if already in SAFE state.
**
**    Revision 0.9  2001/12/05 09:51:42  sxf
**    Detect if Agg is not responding.
**    Go to FAILED state and report AGSTATE as unknown if so.
**    ,
**
**    Revision 0.8  2001/12/04 15:15:02  sxf
**    Enter failed state if contact with Agg is lost.
**
**    Revision 0.7  2001/12/03 09:15:46  sxf
**    Update state based on reported temperature.
**    ,
**
**    Revision 0.6  2001/11/09 10:52:14  sxf
**    Report AGG state as unknown if a command to AGG has timed out.
**
**    Revision 0.5  2001/11/08 13:53:25  sxf
**    Send CENTROID command if Agg is in INTERACTIVE ON state, not INTERACTIVE WORKING.
**
**    Revision 0.4  2001/10/17 15:39:06  sxf
**    First version with basic functionality.
**
**    Revision 0.3  2001/09/25 14:44:38  sxf
**    Initial version with basic functionality.
**
**    Revision 0.2  2001/09/21 14:00:15  sxf
**    Further development.
**
**    Revision 0.1  2001/09/18 14:11:46  sxf
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

#include <unistd.h>
#include <netinet/in.h>

/*
** System include files
*/

#include "TtlSystem.h"
#include "Log.h"
#include "Tim.h"


/*
** Local include files
*/

#include "Agg.h"
#include "Ags.h"
#include "AgsPrivate.h"


/* Local function prototypes. */

Status_t mAgsReplyToTcs( Status_t );


/*******************************************************************************
** Function Name:
**    iAgsProcessCmdQueue
**
** Purpose:
**    Process the command queue on arrival of each CIL message received.
**
** Description:
**    Check for the top message on the AGS queue to have timed out.
**    Search the queue for an INFO command and add one if not found.
**    Search the queue for a CENTROID command and add one if not found. 
**
** Return Type:
**    Status_t
**       Returns the completion status of operation.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t iAgsProcessCmdQueue ( void )
{

   Status_t   Status;
   int Diff;

   eAggCmdBody_t AggCmdBody;

   eTtlTime_t TimeNow;

   /*
   ** Check if there is at least one command on the queue. 
   ** If so then check if has timed out.
   */

   if ( iAgsCmdQueuePtr != (iAgsCmd_t *)(NULL) )
   {
      Status = eTimGetTime( &TimeNow );

      if (Status == SYS_NOMINAL )
      {
         Diff   = eTimCompare( &TimeNow, 
                              &(iAgsCmdQueuePtr -> TimeOut ) );

      }
      else
      {
         Diff = -1;
      }
   }
   else
   { 
      Diff = -1;   /* Indicate that command not timed out. */
   }

   if ( Diff > 0 )
   {

      eLogDebug( "Command to AGG timed out." );

      memcpy( &AggCmdBody, iAgsCmdQueuePtr -> AggCmd.DataPtr, sizeof( eAggCmdBody_t ) ); 
      eLogDebug( "Timed out command is: %d", 
                  AggCmdBody.Command );

      iAgsData.DemandState = SYS_FAILED_STATE;
      iAgsData.AggState = E_AGG_STATE_UNKNOWN;
      Status = iAgsCheckAndSet( D_AGS_AGSTATE, iAgsData.AggState );

      if (Status == E_AGS_UNKNOWN_OID )
      { 
         eLogErr( Status, "Error setting \"D_AGS_AGGSTATE\" Oid" );
      }
      
      if ( iAgsCmdQueuePtr -> TcsCmd.SourceId != ( Int32_t )(NULL) )
      {

         Status = mAgsReplyToTcs( E_AGS_CMD_TIMEOUT );

      }
      else
      {
         Status = SYS_NOMINAL;
      }

      if ( Status != SYS_NOMINAL )
      {
         eLogErr( Status, "Unable to send reply to TCS" );
      }

      Status = iAgsDelCmdFromQueue();   

      if (Status != SYS_NOMINAL)
      {
         eLogErr( Status, "Unable to delete timed out command from queue" );
      }

      Status = iAgsSendTopCmd();

      if ( (Status != SYS_NOMINAL) && (Status != E_AGS_CMDQ_EMPTY) )
      {
         eLogErr( Status, "Unable to send top command on queue." );
      }

   }

   return Status;

} /* End of iAgsProcessCmdQueue. */

/*******************************************************************************
** Function Name:
**    mAgsAddAggCmd
**
** Purpose:
**    Function adds Command to queue  
**
** Description:
**    Initialise a command to send to the AGG and add it to the command
**    queue if there is not already a command of this type on the queue.
**
** Return Type:
**    Status_t
**       Returns the completion status of operation.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t mAgsAddAggCmd ( Int32_t Command )
{

   Status_t      Status;
   iAgsCmd_t     AgsCmd;
   eAggCmdBody_t AggCmdBody;
   eAgsCmdBody_t TcsCmdBody;
   int           Count;

   Status = SYS_NOMINAL;

   if ( ( iAgsSearchCmdQueue( Command ) == FALSE ) ||
        ( Command == E_AGG_CMD_CENTROID          ) )
   {
      AgsCmd.AggCmd.SourceId = iAgsData.CilId;
      AgsCmd.AggCmd.DestId   = iAgsData.AggCilId;
      AgsCmd.AggCmd.Class    = E_CIL_CMD_CLASS;   
      AgsCmd.AggCmd.Service  = E_AGG_CMD;    /* Service = AGG command. */ 
      AgsCmd.AggCmd.SeqNum   = I_AGS_NULL_SEQNUM; /* Command not sent. */

      AggCmdBody.Command     = Command; 
      AggCmdBody.Status      = SYS_NOMINAL;

      for (Count = 0; Count < E_AGG_CMD_BODY_LEN; Count++)
      {
         AggCmdBody.Parameters[ Count ] = 0;
      }

      AgsCmd.AggCmd.DataPtr  = &AggCmdBody;
      AgsCmd.AggCmd.DataLen  = sizeof( eAggCmdBody_t ); 

      AgsCmd.TcsCmd          = iAgsNullTcsCmd;

      Status = iAgsAddCmdToQueue( &AgsCmd, &TcsCmdBody, &AggCmdBody );
   } 

   return Status; 

}

/*******************************************************************************
** Function Name:
**    iAgsReceiveReply
**
** Purpose:
**    Function receives and processes a reply from AGG to a command.
**
** Description:
**    This function processes an incoming command from the AGG. 
**
** Return Type:
**    Status_t
**       Returns the completion status of operation.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t iAgsReceiveReply ( void )
{

   Status_t         Status;
   
   Status_t         TcsReturnStatus; 
   eAggCmdBody_t    AggCmdBody;
   Bool_t           ReplyToTcs;   /* Logical set TRUE if reply sent to TCS. */
   char            *CentroidString;
   char            *ParameterString;

   static char      HashString[] = "#";

   int              ExpPar,       /* Expected number of parameters. */
                    NumPar;       /* Actual number of parameters returned. */

   int              AggState,     /* AGG state. */
                    AggTempState, /* AGG temperature state. */
                    AggPid,       /* AGG Process Id. */
                    AggOnOff,     /* AGG on or off. */
                    AggGwTlx,     /* Guide window top left, X coord. */
                    AggGwTly,     /* Guide window top left, Y coord. */
                    AggGwBrx,     /* Guide window bot right, X coord. */
                    AggGwBry,     /* Guide window bot right, Y coord. */
                    AggItime,     /* Integration time. */                   
                    AggPeak,      /* Peak grey level of guide source. */
                    AggPercPow;   /* Percentage power of cooler. */
     
   float            CentX,        /* X coordinate of centroid. */
                    CentY,        /* Y coordinate of centroid. */
                    CentFWHM,     /* FWHM of centroid. */
                    CentMag,      /* Star magnitude of centroid. */
                    AggTemp,      /* Temperature reported from Agg. */
                    AggCase,      /* Case temperature reported from Agg. */
                    AggFrameMean, /* Frame mean */
                    AggFrameRms;  /* Frame rms */

   /* Initialise return status. */

   Status = SYS_NOMINAL;

   /* 
   ** Copy data from received command into AGG command body structure.
   */
 
   memcpy( &AggCmdBody, iAgsData.RxMsg.DataPtr, sizeof( eAggCmdBody_t ));

   AggCmdBody.Command = ntohl( AggCmdBody.Command );
   AggCmdBody.Status  = ntohl( AggCmdBody.Status );
   
   /*
   ** Verify that response from AGG is of RESPONSE class and has the
   ** same sequence number as the last command sent.
   */
   
   if ( iAgsCmdQueuePtr == ( iAgsCmd_t * )( NULL ) )
   {
      Status = E_AGS_INVREPLY;
      eLogErr( Status, "No outstanding command to correlate reply with" );

   }
   else if ( iAgsData.RxMsg.Class != E_CIL_RSP_CLASS )
   {
      Status = E_AGS_INVREPLY;
      eLogErr( Status, "Message from AGG not of \"RESPONSE\" class." ); 
   }
   else if ( iAgsData.RxMsg.SeqNum != iAgsCmdQueuePtr -> AggCmd.SeqNum )
   { 
      Status = E_AGS_INVREPLY;
      eLogErr( Status, "Message from AGG out of sequence. Out=%d. In=%d.",
               iAgsCmdQueuePtr -> AggCmd.SeqNum, iAgsData.RxMsg.SeqNum );
   } 

   /*
   ** Do not reply to the TCS if an error has been found in the RESPONSE.
   */

   if ( Status != SYS_NOMINAL )
   {
      ReplyToTcs = FALSE;
   }
   
   /*
   ** Reply from INFO command: check number of parameters returned.
   */

   else if ( AggCmdBody.Command == E_AGG_CMD_INFO )  
   { 
      ExpPar = I_AGS_INFO_RSP_PARS;
      NumPar = sscanf(AggCmdBody.Parameters, "%d %d %f %f %d %f %f %d %d", &AggState,
                                                            &AggTempState,
                                                            &AggTemp,
                                                            &AggCase,
                                                            &AggPercPow,
                                                            &AggFrameMean,
                                                            &AggFrameRms,
                                                            &AggItime,
                                                            &AggPid );

      ReplyToTcs = FALSE;

      if ( ExpPar == NumPar )
      {
         iAgsData.AggState     = AggState;
         iAgsData.AggTempState = AggTempState;
         iAgsData.AggTemp      = AggTemp;
         iAgsData.AggCase      = AggCase;
         iAgsData.AggPercPow   = AggPercPow;
         iAgsData.AggFrameMean = AggFrameMean;
         iAgsData.AggFrameRms  = AggFrameRms;
         iAgsData.IntTime      = AggItime;

         Status = iAgsCheckAndSet( D_AGS_AGSTATE, ( Int32_t )( AggState ) );

         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_AGSTATE\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_AGTEMP,
                      ( Int32_t )( AggTemp * E_TTL_MILLI_PER_UNIT ) );

         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_AGTEMP\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_AGCASETEMP,
                      ( Int32_t )( AggCase * E_TTL_MILLI_PER_UNIT ) );

         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_AGCASETEMP\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_AGPERCPOW,
                      ( Int32_t )( AggPercPow * E_TTL_MILLI_PER_UNIT));

         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_AGPERCPOW\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_INTTIME, AggItime );
         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_INTTIME\" Oid" );
         }
         
         Status = iAgsCheckAndSet( D_AGS_AGFRAMEMEAN,
                      ( Int32_t )( AggFrameMean * E_TTL_MILLI_PER_UNIT ) );

         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_AGFRAMEMEAN\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_AGFRAMERMS,
                      ( Int32_t )( AggFrameRms * E_TTL_MILLI_PER_UNIT ) );

         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_AGFRAMERMS\" Oid" );
         }


         iAgsData.AggPid   = AggPid;

         Status = mAgsUpdateState();
      }
      else
      {         
         /*
         ** Invalid reply to INFO indicates something seriously wrong with
         ** Agg process so set states accordingly. 
         */
         Status = iAgsCheckAndSet( D_AGS_AGSTATE, 
                                  ( Int32_t )E_AGG_STATE_UNKNOWN );
         eLogErr(Status,"Incorrect number of parameters returned from INFO request (exp=%d, num=%d)", ExpPar, NumPar);

         iAgsData.DemandState = SYS_FAILED_STATE;

         Status = E_AGS_INVREPLY;
   /*
         eLogErr( Status, "Invalid reply to \"INFO\" from AGG" ); 
   */
      }
   }

   /*
   ** Reply from CENTROID command: check number of parameters returned.
   */

   else if ( AggCmdBody.Command == E_AGG_CMD_CENTROID )
   {
      eLogDebug( "Centroid command response is \"%s\"",
                  AggCmdBody.Parameters );

      ParameterString = (char *)( &AggCmdBody.Parameters );

      while ( ( ( CentroidString = 
                    strtok( (char *)(ParameterString), 
                            (char *)(HashString) ) ) != (char *)(NULL) ) &&
              ( Status == SYS_NOMINAL ) )
      {
         ParameterString = (char *)(NULL);

         ExpPar = I_AGS_CENTROID_RSP_PARS;
         NumPar = sscanf(CentroidString, "%f %f %f %f %d", 
                      &CentX,
                      &CentY,
                      &CentFWHM,
                      &CentMag,
                      &AggItime );
  

         ReplyToTcs = FALSE;

         /*
         ** If expected number of parameters have been read then set the
         ** Oid's assoicated with each data item.
         */

         if ( ExpPar == NumPar )
         {
            Status = iAgsCheckAndSet( D_AGS_CENTROIDX,
                      ( Int32_t )( CentX * E_TTL_MILLI_PER_UNIT ) );

            if (Status == E_AGS_UNKNOWN_OID )
            { 
               eLogErr( Status, "Error setting \"D_AGS_CENTROIDX\" Oid" );
            }

            Status = iAgsCheckAndSet( D_AGS_CENTROIDY,
                      ( Int32_t )( CentY * E_TTL_MILLI_PER_UNIT ) );

            if (Status == E_AGS_UNKNOWN_OID )
            { 
               eLogErr( Status, "Error setting \"D_AGS_CENTROIDY\" Oid" );
            }

            Status = iAgsCheckAndSet( D_AGS_FWHM,
                      ( Int32_t )( CentFWHM * E_TTL_MILLI_PER_UNIT ) );

            if (Status == E_AGS_UNKNOWN_OID )
            { 
               eLogErr( Status, "Error setting \"D_AGS_FWHM\" Oid" );
            }

            Status = iAgsCheckAndSet( D_AGS_GUIDEMAG, 
                      ( Int32_t )( CentMag * E_TTL_MILLI_PER_UNIT ) );

            if (Status == E_AGS_UNKNOWN_OID )
            { 
               eLogErr( Status, "Error setting \"D_AGS_GUIDEMAG\" Oid" );
            }

            Status = iAgsCheckAndSet( D_AGS_INTTIME, AggItime );
            if (Status == E_AGS_UNKNOWN_OID )
            { 
               eLogErr( Status, "Error setting \"D_AGS_INTTIME\" Oid" );
            }

            /* 
            ** Submit OIDs to SDB. 
            */
            Status = iAgsSubmitStatus();
 
         }
         else
         {
            Status = E_AGS_INVREPLY;
            eLogErr( Status, "Invalid reply to \"CENTROID\" from AGG" ); 
         }
      }
   }

   /*
   ** Reply from STARTSESSION, ENDSESSION or CONFIG command -
   ** send reply to TCS indicating command successfully completed.
   */
 
   else if ( ( AggCmdBody.Command == E_AGG_CMD_STARTSESSION ) ||
             ( AggCmdBody.Command == E_AGG_CMD_ENDSESSION   ) ||
             ( AggCmdBody.Command == E_AGG_CMD_CONFIG       ) )
   {
      ReplyToTcs = TRUE;   
   }

   /* 
   ** Reply from AUTOGUIDE command. Check number of parameters returned.
   */

   else if ( AggCmdBody.Command == E_AGG_CMD_AUTOGUIDE )
   {
      ExpPar = I_AGS_AUTOGUIDE_RSP_PARS;
      NumPar = sscanf(AggCmdBody.Parameters, "%d %d %d %d %d %d",
                      &AggOnOff, 
                      &AggGwTlx,
                      &AggGwTly,
                      &AggGwBrx,
                      &AggGwBry,                      
                      &AggPeak );

      if ( NumPar == 1 )
      {
         if ( AggOnOff == E_AGG_OFF )
         {
            ReplyToTcs = TRUE;
         }
      }
      else if (NumPar != ExpPar )
      {
         Status = E_AGS_INVREPLY; 
         eLogErr( Status,
                  "Invalid number of parameters in \"AUTOGUIDE\" response" );
      }

      ReplyToTcs = TRUE;

      /*
      ** If expected number of parameters have been read then set the
      ** Oid's associated with each data item. 
      */
 
      if ( (Status == SYS_NOMINAL ) && (ExpPar == NumPar) )
      {
         Status = iAgsCheckAndSet( D_AGS_WINDOW_TLX, AggGwTlx );
         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_WINDOW_TLX\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_WINDOW_TLY, AggGwTly );
         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_WINDOW_TLY\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_WINDOW_BRX, AggGwBrx );
         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_WINDOW_BRX\" Oid" );
         }

         Status = iAgsCheckAndSet( D_AGS_WINDOW_BRY, AggGwBry );
         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_WINDOW_BRY\" Oid" );
         }
       
         Status = iAgsCheckAndSet( D_AGS_PEAKPIXEL, AggPeak );
         if (Status == E_AGS_UNKNOWN_OID )
         { 
            eLogErr( Status, "Error setting \"D_AGS_PEAKPIXEL\" Oid" );
         }

      }
      else
      {
         Status     = E_AGS_INVREPLY;
         eLogErr ( Status, "Invalid reply to \"AUTOGUIDE\" command." );
         ReplyToTcs = FALSE;
      }
   }

   /* 
   ** Reply from SETLOGPATH command.
   */

   else if ( AggCmdBody.Command == E_AGG_CMD_SETLOGPATH )
   {

      /* Do nothing */
   }


   /* 
   ** Reply to unknown command.
   */

   else 
   {
      ReplyToTcs = FALSE;
      Status = E_AGS_INVREPLY;
      eLogErr( Status, "Unknown command %d in reply from AGG",
               AggCmdBody.Command );
   }

   /* 
   ** Send reply to current command to TCS if required.
   */

   if (ReplyToTcs == TRUE )
   {

      /*
      ** First decide on the return status to the TCS command.
      */
      switch ( AggCmdBody.Status )
      {
         case SYS_NOMINAL:
            TcsReturnStatus = SYS_NOMINAL;
            break;

         case E_AGG_LOOP_STOPPING:
            TcsReturnStatus = E_AGS_LOOP_STOPPING;
            break;

         case E_AGG_LOOP_RUNNING:
            TcsReturnStatus = E_AGS_LOOP_RUNNING;
            break;

         case E_AGG_PROXERR:
            TcsReturnStatus = E_AGS_LOOP_ERROR;
            break;

         case E_AGG_CMD_BAD_FORMAT:
            TcsReturnStatus = E_AGS_BAD_FORMAT;
            break;

         case E_AGG_HW_ERR:
            TcsReturnStatus = E_AGS_HW_ERR;
            break;

         case E_AGG_CMD_NOT_PERMITTED:
            TcsReturnStatus = E_AGS_CMD_NOT_PERMITTED;
            break;

         default:
            TcsReturnStatus = E_AGS_GEN_ERR;
            break;
      }
      
      Status = mAgsReplyToTcs( TcsReturnStatus );

      if ( Status != SYS_NOMINAL )
      {
         eLogCrit( Status, "Unable to send reply to TCS." );

         /* Make request to change process state to FAILED. */
         iAgsData.DemandState = SYS_FAILED_STATE; 
      }
   }

   /*
   ** Reply received so delete top command from queue and send  
   ** the next command from the top of the queue.
   */

   Status = iAgsDelCmdFromQueue();

   if ( Status == SYS_NOMINAL )
   {
      Status = iAgsSendTopCmd();
   }
   else
   {
      eLogCrit( Status, "Unable to delete command from queue." );

      /* Make request to change process state to failed. */
      iAgsData.DemandState = SYS_FAILED_STATE;
   }

   return Status;

}


/*******************************************************************************
** Function Name:
**    iAgsSendTopCmd
**
** Purpose:
**    Function to send the top command on the Ags command queue to the AGG.
**
** Description:
**    This . 
**
** Return Type:
**    Status_t
**       Returns the completion status of operation.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t iAgsSendTopCmd( void )
{
   Status_t Status;

   Int32_t       Dest;   /* Cil id for destination of command to Agg. */
   eCilMsg_t     CilMsg; /* Temporary structure for outgoing Cil message. */
   eAggCmdBody_t AggCmd; /* temporary structure for outgoing Agg command. */


   /* First check that there is at least one command on the queue. */

   if ( iAgsCmdQueuePtr != ( iAgsCmd_t *)(NULL) )
   {
   
      /* Assign sequence number to outgoing command and increment seqnum. */
      
      iAgsCmdQueuePtr -> AggCmd.SeqNum = iAgsData.AggCmdSeqNum;
      iAgsData.AggCmdSeqNum ++;

      /*
      ** Get destination and timeout for the AGG command.
      */

      Dest = iAgsCmdQueuePtr -> AggCmd.DestId;


      Status = eTimGetTimeOut( I_AGS_AGG_TIMEOUT,
                             &(iAgsCmdQueuePtr -> TimeOut) );
 
      if (Status == SYS_NOMINAL)
      {

         /*
         ** Copy body of outgoing Cil message to temporary CilMsg structure.
         */

         memcpy( &CilMsg,
                 &(iAgsCmdQueuePtr -> AggCmd ),
                 sizeof( eCilMsg_t ) );

         /*
         ** Copy Agg command structure to temporary AggCmd structure.
         */

         memcpy( &AggCmd,
                 iAgsCmdQueuePtr -> AggCmd.DataPtr,
                 sizeof( eAggCmdBody_t ) );

         AggCmd.Command = htonl( AggCmd.Command );
         AggCmd.Status  = htonl( AggCmd.Status );
 
         CilMsg.DataPtr = &AggCmd;
         CilMsg.DataLen = sizeof( eAggCmdBody_t );

         Status = iAgsSendCilMsg( Dest, &CilMsg );
      }
   } 
   else
   {
      Status = E_AGS_CMDQ_EMPTY;
   }

   return Status;   

}  /* End of iAgsSendTopCmd. */

/*******************************************************************************
** Function Name:
**    mAgsReplyToTcs
**
** Purpose:
**    Function to send a reply to the TCS based on the command currently
**    at the top of the queue.
**
** Description:
**    This function swaps the source and destination IDs for the TCS
**    command at the top of the queue and then sends the reply to the TCS. 
**
** Return Type:
**    Status_t
**       Returns the completion status of operation.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t mAgsReplyToTcs( Status_t TcsReturnStatus )
{
   Status_t       Status;
   eAgsCmdBody_t *TcsCmdBody;
   Int32_t        Dest;

   /*
   ** Copy the TCS return status to the command body for returning.
   */
   TcsCmdBody = iAgsCmdQueuePtr -> TcsCmd.DataPtr;
   TcsCmdBody -> Status = TcsReturnStatus;

   /*
   ** Swap source and destination Id.
   */
   Dest = iAgsCmdQueuePtr -> TcsCmd.SourceId;
   Status = SYS_NOMINAL;
   iAgsCmdQueuePtr -> TcsCmd.SourceId   = iAgsCmdQueuePtr -> TcsCmd.DestId;
   iAgsCmdQueuePtr -> TcsCmd.DestId = Dest;
   iAgsCmdQueuePtr -> TcsCmd.Class    = E_CIL_RSP_CLASS;
 
   Status = iAgsSendCilMsg( Dest, &iAgsCmdQueuePtr -> TcsCmd );

   return Status;   

}  /* End of mAgsReplyToTcs. */

/*******************************************************************************
** Function Name:
**    iAgsReceiveCommand
**
** Purpose:
**    Function passes an incoming command to AGG command queue.
**
** Description:
**    This function responds to an incoming command from the TCS. 
**
** Return Type:
**    Status_t
**       Returns the completion status of communciations operation.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t iAgsReceiveCommand ( void )
{

   /* Local variables */

   int                ParLen;         /* Length of params in command body. */
   eAgsCmdBody_t      TcsCmdBody;     /* Body of TCS command (received). */
   eAggCmdBody_t      AggCmdBody;     /* Body of AGG command (to be sent). */ 
   iAgsCmd_t          CmdOut;         /* CIL message to be placed on queue. */
   Status_t           Status;         /* Return status. */

   Status = SYS_NOMINAL;              /* Initialise Status. */

   /*
   ** Copy received TCS command to CmdOut for later pushing on to queue.
   */

   memcpy( &CmdOut.TcsCmd, &iAgsData.RxMsg, sizeof( eCilMsg_t ) );

   /* 
   ** Copy data from received command into TCS command body structure.
   ** Also translate from network to hardware byte order.
   */

   memcpy( &TcsCmdBody, iAgsData.RxMsg.DataPtr, sizeof( eAgsCmdBody_t ));

   TcsCmdBody.Command = ntohl( TcsCmdBody.Command );
   TcsCmdBody.Status  = ntohl( TcsCmdBody.Status );
   TcsCmdBody.Param1  = ntohl( TcsCmdBody.Param1 );
   TcsCmdBody.Param2  = ntohl( TcsCmdBody.Param2 );

   /* Form basic structure of the outgoing AGG command. */

   CmdOut.AggCmd.SourceId = iAgsData.CilId;
   CmdOut.AggCmd.DestId   = iAgsData.AggCilId;
   CmdOut.AggCmd.Class    = E_CIL_CMD_CLASS;   
   CmdOut.AggCmd.Service  = E_AGG_CMD;         /* Service = AGG command. */
   CmdOut.AggCmd.SeqNum   = I_AGS_NULL_SEQNUM; /* Command not sent. */
 
   /*
   ** Form the body of the outgoing AGG command depending on the 
   ** TCS command that has been received. 
   */
 
   switch ( TcsCmdBody.Command )
   {
      case ( E_AGS_AUTOGUIDE_ON_BRIGHTEST ):
 
         AggCmdBody.Command       = E_AGG_CMD_AUTOGUIDE;
         AggCmdBody.Status        = SYS_NOMINAL;

         ParLen = sprintf( AggCmdBody.Parameters, "%d %d", E_AGG_ON,
                                                  E_AGG_BRIGHTEST );

         eLogDebug( "Received autoguide on brightest cmd" );

         break;
 
      case ( E_AGS_AUTOGUIDE_ON_RANK ):

         AggCmdBody.Command       = E_AGG_CMD_AUTOGUIDE;
         AggCmdBody.Status        = SYS_NOMINAL;

         ParLen = sprintf( AggCmdBody.Parameters, "%d %d %d", E_AGG_ON,
                           E_AGG_RANK,
                           TcsCmdBody.Param1 );

         eLogDebug( "Received autoguide on rank cmd" );

         break;

      case ( E_AGS_AUTOGUIDE_ON_RANGE ):

         AggCmdBody.Command       = E_AGG_CMD_AUTOGUIDE;
         AggCmdBody.Status        = SYS_NOMINAL;

         ParLen = sprintf( AggCmdBody.Parameters, "%d %d %lf %lf", E_AGG_ON,
                           E_AGG_RANGE,
                 (double)( TcsCmdBody.Param1 ) * I_AGS_PER_MILLI_UNITS,
                 (double)( TcsCmdBody.Param2 ) * I_AGS_PER_MILLI_UNITS );

         eLogDebug( "Received autoguide on range cmd" );

         break;
 
      case ( E_AGS_AUTOGUIDE_ON_PIXEL ):

         AggCmdBody.Command       = E_AGG_CMD_AUTOGUIDE;
         AggCmdBody.Status        = SYS_NOMINAL;

         ParLen = sprintf( AggCmdBody.Parameters, "%d %d %lf %lf", 
                           E_AGG_ON,
                           E_AGG_PIXEL,
                 (double)( TcsCmdBody.Param1 ) * I_AGS_PER_MILLI_UNITS,
                 (double)( TcsCmdBody.Param2 ) * I_AGS_PER_MILLI_UNITS );

         eLogDebug( "Received autoguide on pixel cmd" );

         break;

      case ( E_AGS_AUTOGUIDE_OFF ):
 
         AggCmdBody.Command       = E_AGG_CMD_AUTOGUIDE;
         AggCmdBody.Status        = SYS_NOMINAL;

         ParLen = sprintf( AggCmdBody.Parameters, "%d", E_AGG_OFF );

         eLogDebug( "Received autoguide off cmd" );

         break;
 
      case ( E_AGS_CONF_EXP_TIME ):

         AggCmdBody.Command       = E_AGG_CMD_CONFIG;
         AggCmdBody.Status        = SYS_NOMINAL;
         
         ParLen = sprintf( AggCmdBody.Parameters, "%d %d", 
                           E_AGG_CONF_EXPTIME, 
                           TcsCmdBody.Param1 );

         eLogDebug( "Received conf exp time cmd" );

         break;

      case ( E_AGS_CONF_FRAME_RATE ):

         AggCmdBody.Command       = E_AGG_CMD_CONFIG;
         AggCmdBody.Status        = SYS_NOMINAL;
         
         ParLen = sprintf( AggCmdBody.Parameters, "%d %d", 
                           E_AGG_CONF_FRATE, 
                           TcsCmdBody.Param1 );

         eLogDebug( "Received conf frame rate cmd" );

         break;

      case ( E_AGS_CONF_FRAME_AVERAGE ):

         AggCmdBody.Command       = E_AGG_CMD_CONFIG;
         AggCmdBody.Status        = SYS_NOMINAL;
         
         ParLen = sprintf( AggCmdBody.Parameters, "%d %d", 
                           E_AGG_CONF_FAVERAGE, 
                           TcsCmdBody.Param1 );

         eLogDebug( "Received conf frame average cmd" );

         break;

      case ( E_AGS_CONF_CALIB ):

         AggCmdBody.Command       = E_AGG_CMD_CONFIG;
         AggCmdBody.Status        = SYS_NOMINAL;
         
         ParLen = sprintf( AggCmdBody.Parameters, "%d %d %d", 
                           E_AGG_CONF_CALIB, 
                           TcsCmdBody.Param1,
                           TcsCmdBody.Param2 );

         eLogDebug( "Received conf calib cmd" );

         break;
 
      case( E_AGS_START_SESSION ):
         
         ParLen = 0;
         AggCmdBody.Command       = E_AGG_CMD_STARTSESSION;
         AggCmdBody.Status        = SYS_NOMINAL;
         ParLen = strlen( strcpy( AggCmdBody.Parameters, "" ) );

         eLogDebug( "Received start session cmd" );

         break;

      case ( E_AGS_END_SESSION ):

         ParLen = 0;
         AggCmdBody.Command       = E_AGG_CMD_ENDSESSION;
         AggCmdBody.Status        = SYS_NOMINAL;
         ParLen = strlen( strcpy( AggCmdBody.Parameters, "" ) );

         eLogDebug( "Received end session cmd" );

         break;

      case ( E_AGS_LOGGING ):

         ParLen = 0;
         AggCmdBody.Command       = E_AGG_CMD_SETLOGPATH;
         AggCmdBody.Status        = SYS_NOMINAL;

         if ( TcsCmdBody.Param1 == E_AGS_KEYWORD_ON )
         {
           ParLen = sprintf( AggCmdBody.Parameters, "%d %s",                          
                             E_AGG_ON, E_AGG_LOGPATH );
         }
         else if ( TcsCmdBody.Param2 == E_AGS_KEYWORD_OFF )
         {
           ParLen = sprintf( AggCmdBody.Parameters, "%d",                          
                             E_AGG_OFF );
         }

         eLogDebug( "Received logging command" );

         break;
      default:

         eLogDebug( "Received unknown command" );

         Status = E_AGS_INV_CMD;
         break;

   }

   /* 
   ** If no other errors have occurred then check that the number of
   ** parameters written was correct and add the fully formed AGG command
   ** to the queue.
   */

   if ( Status == SYS_NOMINAL) 
   {
      if ( ParLen >= 0 )
      {

         Status = iAgsAddCmdToQueue( &CmdOut, 
                                     &TcsCmdBody,
                                     &AggCmdBody );
      }
      else
      {

         eLogDebug( "Error in TCS command - sending error reply." );

         CmdOut.TcsCmd.Class = E_CIL_ERR_CLASS;
         Status = iAgsErrReply( &(CmdOut.TcsCmd), Status );
         if ( Status == SYS_NOMINAL ) 
         {
            Status = E_AGS_BAD_CMD;
         }
      }
   }

   return Status;

} /* End of iAgsReceiveCommand() */

/*******************************************************************************
** Function Name:
**    iAgsProcessHeartbeat
**
** Purpose:
**    Function to process an incoming hearbeat request message.
**
**
** Description:
**    This function determines the current state of this process, and responds 
**    to the heartbeat message with it. A copy of the passed in time is made to
**    be subsequently used to determine when heartbeats are lost.
**
** Return Type:
**    Status_t
**       Returns the completion status of AGS heart beat processing.
**
** Arguments:
**    eTtlTime_t *TimeReceived         (in)
**       Pointer to structure
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsProcessHeartbeat( eTtlTime_t *TimeReceived )
{
   Status_t    Status;                 /* Status returned by functions */
   Status_t    TimStatus;              /* Status returned by time functions */
   ProcState_t ReportState;            /* Current process state for return */


   /* Respond to heartbeat with our state */
   ReportState = iAgsGetState();

   /* Point to return packet and assign sequence number */
   iAgsData.ChbResponseMsg.DataPtr = &ReportState;
   iAgsData.ChbResponseMsg.SeqNum  = iAgsData.RxMsg.SeqNum;

   /* Respond to message */
   Status = iAgsSendCilMsg( iAgsData.ChbResponseMsg.DestId, 
                            &iAgsData.ChbResponseMsg );

   /* Increment number of heartbeats received */
   iAgsData.HeartbeatCount++;

   /* Take copy of time to be used as last heartbeat time */
   TimStatus = eTimCopy( *TimeReceived, &iAgsData.LastHeartbeatTime );

   /* Update on-screen verbose output */
   iAgsOutputStatus( TRUE );

   /* Send INFO and CENTROID commands to monitor Agg. */
   Status = iAgsProcessMonitorCommands( I_AGS_CENTROID_INFO );

   return Status;

}  /* End of iAgsProcessHeartbeat() */

/*******************************************************************************
** Function Name:
**    iAgsAddMonitorCommands
**
** Purpose:
**    Function to add monitoring commands (INFO and CENTROID) to
**    the Agg command queue.
**
** Description:
**    Check the queue and add an INFO command if one does not already exist on the queue.
**    Also add a CENTROID command each time this function is called.
**
** Return Type:
**    Status_t
**       Program exits before return.
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale.
**
*******************************************************************************/

Status_t iAgsProcessMonitorCommands( iAgsMonitorCmds_t Commands )
{

   Status_t    Status = SYS_NOMINAL;     /* Return status. */

   if ( Commands == I_AGS_CENTROID_INFO )
   {
     Status = mAgsAddAggCmd( E_AGG_CMD_INFO );
   }

   if ( Status != SYS_NOMINAL )
   {
      eLogWarning( Status, "Unable to add INFO command to queue." );
   } 

   if ( ( iAgsData.AggState == E_AGG_STATE_GUIDEONBRIGHT ) ||
        ( iAgsData.AggState == E_AGG_STATE_GUIDEONRANGE  ) ||
        ( iAgsData.AggState == E_AGG_STATE_GUIDEONRANK   ) ||
        ( iAgsData.AggState == E_AGG_STATE_GUIDEONPIXEL  ) ||
        ( iAgsData.AggState == E_AGG_STATE_INTON       ) )
   {
      Status = mAgsAddAggCmd( E_AGG_CMD_CENTROID );
      if ( Status != SYS_NOMINAL )
      {
         eLogWarning( Status, "Unable to add CENTROID command to queue." );
      } 
   }

   return Status;
}



/*******************************************************************************
** Function Name:
**    iAgsProcessShutdown
**
** Purpose:
**    Function to process an incoming shutdown request message.
**
** Description:
**    This function forms and transmits an acknowledgement to the shutdown
**    request, and then calls the shutdown function for this process.
**
** Return Type:
**    Status_t
**       Program exits before return.
**
** Arguments:
**    none
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsProcessShutdown( void )
{
   Status_t Status;                    /* Status returned by functions */

   /* Reply to this system request. */ 
   Status = iAgsAckReply(&(iAgsData.RxMsg));

   /* Call the shutdown function */
   iAgsShutdown();

   /* Return status of CIL transaction */
   return Status;

}  /* End of iAgsProcessShutdown() */



/*******************************************************************************
** Function Name:
**    iAgsProcessSafeState
**
** Purpose:
**    Function to process an incoming safe-state request message.
**
** Description:
**    This function forms and transmits an action-in-progress message to the 
**    safe-state request, sets safe-state, and then forms and transmits an 
**    action-completed message with the attained state appended.
**
** Return Type:
**    Status_t
**       Returns the completion status of Safe state request.
**
** Arguments:
**    none
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsProcessSafeState ( void )
{
   Status_t Status;                    /* Status returned by functions */
   Int32_t  Oid;      /* For loop index. */

   /* Reply to this system request. */
   Status = iAgsActReply(&(iAgsData.RxMsg));

   /* 
   ** Set a flag saying that we need to go safe. All the work of going 
   ** safe is done in the CheckSafeState function that is run at the end  
   ** of the main loop, we just set the flag here.
   */
   iAgsData.SafeStateReq = TRUE;

   /* Remember sequence no, of safestate cmd (latest one overwrites others) */
   iAgsData.SafeStateSeqNum = iAgsData.RxMsg.SeqNum;

   /* 
   ** Clear the 'Changed' field of the OidTable to force re-submission
   ** of everything. This ensures that if the PMM is restarted that
   ** all the data in the SDB is current. Don't set BOL and EOL
   */
   for (Oid = D_AGS_DATAID_BOL+1; Oid < D_AGS_DATAID_EOL; Oid++)
       iAgsOidTable[ Oid ].Changed = TRUE; 

   /* Return status of CIL transactions */
   return Status;

}  /* End of iAgsProcessSafeState() */



/*******************************************************************************
** Function Name:
**    iAgsProcessActivate
**
** Purpose:
**    Function to process an incoming activate request message.
**
** Description:
**    This function forms and transmits an action-in-progress message to the 
**    activate request, restores the previous state, and then forms and 
**    transmits an action-completed message with the attained state appended.
**
** Return Type:
**    Status_t
**       Returns the completion status of AGS initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsProcessActivate ( void )
{
   Status_t    Status;                 /* Status returned by functions */
   Status_t    CilStatus;              /* Status returned by CIL functions */
   Int32_t     Oid;                    /* For loop index. */

   /* Reply to this system request. */
   Status = iAgsActReply(&(iAgsData.RxMsg));

   /* Clear the safe state */
   iAgsRestoreState();

   /* Update state to what it would have been prior to SAFE */
   mAgsUpdateState();

   /* Clear any safe-state request flags */
   iAgsData.SafeStateReq = FALSE;

   /* If last transmission was OK, then send this one */
   if ( Status == SYS_NOMINAL )
   {
      CilStatus = iAgsComReply(&(iAgsData.RxMsg));
   }

   /* 
   ** Clear the 'Changed' field of the OidTable to force re-submission
   ** of everything. This ensures that if the PMM is restarted that
   ** all the data in the SDB is current. Don't set BOL and EOL
   */
   for (Oid = D_AGS_DATAID_BOL+1; Oid < D_AGS_DATAID_EOL; Oid++)
       iAgsOidTable[ Oid ].Changed = TRUE; 

   /* Return status of CIL transactions */
   return CilStatus;


}  /* End of iAgsProcessActivate() */


Status_t iAgsProcessStateChange( void )
{

   Status_t  Status;     /* Return status. */

   Status = SYS_NOMINAL;

   return Status;

}

/*******************************************************************************
** Function Name:
**    mAgsUpdateState
**
** Purpose:
**    Function to update the overall Autoguider Service state based on the
**    reported Agg system and temperature states.
**
** Description:
**
** Return Type:
**    Status_t
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t mAgsUpdateState( void )
{

  /*
  ** Set demand state to FAILED if Agg is reporting that it is in a 
  ** non-recoverable error state.
  */
  if ( iAgsData.AggState == E_AGG_STATE_NONRECERR )
  {
     if ( iAgsData.DemandState != SYS_SAFE_STATE )
     {
        iAgsData.DemandState = iAgsData.ActualState = SYS_FAILED_STATE;
     }
      
     return SYS_NOMINAL;
  }

  /* 
  ** Set demand state to WARN if Agg is reporting that it is not at
  ** temperature (overridden by SAFE state).
  ** Otherwise, if system currently in WARN state but Agg is reporting
  ** that it is at temperature then request a change to OKAY.
  */

  if ( iAgsData.AggTempState != E_AGG_TSTATE_AT_TEMP )
  {
     if ( iAgsData.DemandState != SYS_SAFE_STATE )
     {
        iAgsData.DemandState = SYS_WARN_STATE;
     }
  }
  else if ( ( iAgsGetState() == SYS_WARN_STATE ) ||
            ( iAgsGetState() == SYS_INIT_STATE ) )
  {
     iAgsData.ActualState = iAgsData.DemandState = SYS_OKAY_STATE;
  }

  return SYS_NOMINAL;
}


/*******************************************************************************
** End of File Name: AgsProcess.c
*******************************************************************************/
