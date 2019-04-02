/*******************************************************************************
** Module Name:
**    AgsMain.c
**
** Purpose:
**    Main module of the Autoguider Service Task.
**
** Description:
**    The Autoguider Service task provides an interface between the
**    Autoguider guide process and the rest of the telescope.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    Autoguider Control System Detailed Design Document v0.01.
**
** Author(s):
**    Stephen Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**     $Id: AgsMain.c,v 0.15 2007/10/26 11:34:05 mjf Exp $
**
** History:
**    $Log: AgsMain.c,v $
**    Revision 0.15  2007/10/26 11:34:05  mjf
**    Linux-friendly.
**
**    Revision 0.14  2006/10/11 09:41:54  sxf
**    Brought in line with AGS_0_19.
**
**    Revision 0.13  2006/10/03 15:37:43  sxf
**    Baseline AGS_1_04.
**
**    Revision 0.11  2005/10/04 12:40:53  sxf
**    Brought in line with QNX4 baseline AGS_0_17.
**
**    Revision 0.10  2004/04/16 09:31:00  mjf
**    Checking in of QNX v6 porting done by MAN.
**
**    Revision 0.9  2003/09/04 07:53:03  sxf
**    Flush incoming Cil message stack before entering main receive loop
**    in order to stop initial flood of out of sequence heartbeats.
**
**    Revision 0.8  2002/07/05 10:32:17  sxf
**    Fill in the full Agg command structure on going safe.
**
**    Revision 0.7  2002/02/26 11:37:40  sxf
**    Send CENTROID command to Agg on each iteration of the loop.
**    ,
**
**    Revision 0.6  2002/01/23 14:14:46  sxf
**    Correct log message function call.
**
**    Revision 0.4  2002/01/21 09:14:31  sxf
**    Send command to set logging off to Agg on SAFE state request.
**    Also improve log message for unexpected CIL message.
**
**    Revision 0.3  2001/10/17 15:39:06  sxf
**    First version with basic functionality.
**
**    Revision 0.2  2001/09/21 14:00:04  sxf
**    Further development.
**
**    Revision 0.1  2001/09/18 14:09:45  sxf
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
#include <sys/types.h>
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

#define M_AGS_MAIN_C         /* So data area defined, not declared extern */
#include "Ags.h"
#include "AgsPrivate.h"


/*
** Local variable of module scope.
*/

Bool_t mAgsAggAlreadySafe = FALSE;

/*
** Local function prototypes
*/

Status_t mAgsAggGoSafe( void );

/*******************************************************************************
** Function Name:
**    main
**
** Purpose:
**    Top level function of the AGS.
**
** Description:
**    Top level function of the AGS. It is called on startup of the executable.
**    It requests initialisation, including the parsing of the command-line, 
**    and then calls the main program loop.
**
** Return Type:
**    int  
**       Returns the completion status of AGS program.
**
** Arguments:
**    int argc                         (in)
**       Number of arguments on the command-line (including the executable 
**       name).
**    char *argv[]                     (in)
**       Array of null-terminated character strings containing the 
**       command-line arguments.
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

int main( int argc, char *argv[] )
{
   Status_t Status;     /* Returned status of functions called */

   /* Initialise process state, don't use SetState function first time. */ 
   iAgsData.ActualState = SYS_INIT_STATE;

   /* Perform initialisation passing runtime parameters */
   Status = iAgsSetup( argc, argv );

   /* If initialisation has not passed, or help only requested */
   if ( ( Status != SYS_NOMINAL ) || ( eCluCommon.Help == TRUE ) )
   {
      /* Exit the AGS */
      iAgsShutdown();
   }

   /* Monitor the state of the AGG */

   Status = iAgsMonitorAgg();
   
   /* Exit the AGS - note that 'exit' is called within here */
   iAgsShutdown();

   return EXIT_SUCCESS;

}  /* End of main() */



/*******************************************************************************
** Function Name:
**    iAgsMonitorAgg
**
** Purpose:
**
** Description:
**
** Return Type:
**    Status_t
**       Returns the status of this function on completion.
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

Status_t iAgsMonitorAgg ( void )
{
   Status_t Status;                    /* Completion status of function */

   /* Default state is nominal */
   Status = SYS_NOMINAL;

   /*
   ** Flush incoming Cil message stack.
   */
   eCilFlush();

   /* Start loop - terminated by receipt of shutdown command in housekeeping */
   while( Status != SYS_FAILED_STATE )
   {
      if ( kill( iAgsData.AggPid, 0 ) != 0 )
      {
         iAgsData.AggState = E_AGG_STATE_NONRECERR;
      }

      /* Receive any CIL messages */
      Status = iAgsReceiveMessage();

      /* If we need to have data submitted to the SDB, then do so */
      if (iAgsData.SdbSubmitReq == TRUE)
      {
         /* Submit the current data set to the database */
         Status = iAgsSubmitStatus();

         if ( Status != SYS_NOMINAL )
         {
           eLogWarning( Status, "Unable to submit information to SDB" );
         }
      }

      /* Do we need to goto, or are we at, safe state ? */
      Status = iAgsCheckSafeState(); 

   }  /* End of while() loop */

   return Status;

}  /* End of iAgsMonitorAgg() */


/*******************************************************************************
** Function Name:
**    iAgsReceiveMessage
**
** Purpose:
**    Function to receive and process messages for the AGS.
**
** Description:
**    This function receives any incoming CIL messages and processes them 
**    accordingly. Heartbeats and received and responded to, MCP messages are 
**    received, processed and responded to. An ackowledgement to an SDB 
**    submission may be received, and this is checked for an error. 
**    Commands from the TCS and responses from the AGG are also handled. This
**    function also checks that a heartbeat has been received within the 
**    required period - if not then safe-state is assumed by this process.
**
** Return Type:
**    Status_t
**       Returns the status of this function on completion.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iAgsReceiveMessage( void )
{
   char InBuffer[ I_AGS_CIL_DATALEN ]; /* Buffer for incoming CIL messages */ 
   Status_t Status;                    /* Status of functions called */
   Int32_t  CilId;                     /* CIL ID of deliverer of message */
   Int32_t  SourceId;                  /* CIL ID of originator of message */
   Int32_t  SwapWord;                  /* Word used to swap byte-ordering */
   eTtlTime_t CurrentTime;             /* Time at which message received */

   /* Set default status */
   Status = SYS_NOMINAL;


   /* Take copy of current time */
   Status = eTimGetTime( &CurrentTime );

   /* Check for incoming CIL messages */
   iAgsData.RxMsg.DataPtr = &InBuffer;
   iAgsData.RxMsg.DataLen = sizeof( InBuffer );
   Status = eCilReceive( I_AGS_CIL_RX_TIMEOUT, &CilId, &iAgsData.RxMsg );

   /* 
   ** Normally submit to SDB every loop, except when the message received
   ** is the response to the last SDB submission.
   */
   iAgsData.SdbSubmitReq = TRUE;


   /* If an error occurred that was not timeout, then return it */
   if ( ( Status != SYS_NOMINAL ) && ( Status != E_CIL_TIMEOUT ) )
   {
      eLogReport( E_LOG_ERR, Status, "CIL message receive error" );
      return Status;
   }

   /* If a message received, process it */
   if ( Status == SYS_NOMINAL )
   {

      /* Take copy of source ID - checked for validity by the CIL function */
      SourceId = iAgsData.RxMsg.SourceId;

      if ( iAgsData.RxMsg.SourceId == iAgsData.AggCilId )
      {
         /* If message of response class then process AGG reply. */
         if ( iAgsData.RxMsg.Class == E_CIL_RSP_CLASS ) 
         {              
            /* Process the reply from the AGG. */
            Status = iAgsReceiveReply();
         }
         else
         {
            Status = E_AGS_UNEXPECTED_MSG;
         }

      }
      else
      {
         /* Check the source id */
         switch ( iAgsData.RxMsg.SourceId )
         {
            /* Message from CHB. */
            case I_AGS_CIL_CHB:

               /* Check that the service and class are valid */
               if ( ( iAgsData.RxMsg.Service == E_MCP_HEARTBEAT ) 
                  && ( iAgsData.RxMsg.Class == E_CIL_CMD_CLASS ) )
               {
                  /* Process the heartbeat message */
                  Status = iAgsProcessHeartbeat( &CurrentTime );
               }
               /* Else unexpected message */
               else
               {
                  Status = E_AGS_UNEXPECTED_MSG;
               }


               break;
    
            /* Message from MCP. */
            case I_AGS_CIL_MCP:

               /* If message of command class then process state change request. */
               if ( iAgsData.RxMsg.Class == E_CIL_CMD_CLASS ) 
               {
                  /* Process the state change request message */
                  Status = iAgsRequestStateChange();
               }
               else
               {
                  Status = E_AGS_UNEXPECTED_MSG;
               }
               break;

            /* Message from TCS. */
            case I_AGS_CIL_TCS:

               eLogDebug( "Received message from TCS." );


               /* If message of command class then process TCS command. */
               if ( iAgsData.RxMsg.Class == E_CIL_CMD_CLASS ) 
               {
                  /* Process the TCS command. */
                  Status = iAgsReceiveCommand();
               }
               else
               {
                  Status = E_AGS_UNEXPECTED_MSG;
               }
               break;

            /* Message from SDB. */
            case I_AGS_CIL_SDB:
       
               /* Should be response to SDB submit service */
               if ( iAgsData.RxMsg.Service == E_SDB_SUBMIT_1 )
               {
                  /* Check the message class */
                  switch ( iAgsData.RxMsg.Class )
                  {
                     /* Error responses sent back to AGS */
                     case E_CIL_ERR_CLASS :

                        /* Get incoming word, will be in network order */
                        memcpy( &SwapWord, iAgsData.RxMsg.DataPtr, 
                                sizeof( Int32_t ) );
                        /* Error returned by SDB submission */
                        eLogReport( E_LOG_ERR, ntohl( SwapWord ), 
                                    "Error returned by SDB submission" );
                        Status = E_AGS_GEN_ERR;

                        break;

                     /* Successful SDB submission */
                     case E_CIL_ACK_CLASS :
 
                        /* Don't submit again this loop. */
                        iAgsData.SdbSubmitReq = FALSE;
 
                        break;

                     default:
                        Status = E_AGS_UNEXPECTED_MSG;
                        break;
                  }
               }
               break;

            /* Default is an error */
            default :
               eLogErr(0, "Message received from unknown source, %d", iAgsData.RxMsg.SourceId );
               Status = E_AGS_UNKNOWN_SOURCE;
               break;
         }
      }

      /* If message is unexpected */
      if ( ( Status == E_AGS_UNEXPECTED_MSG ) ||
           ( Status == E_AGS_UNKNOWN_SOURCE) ) 
      {
         eLogErr(
            Status, 
            "Unexpected CIL msg rx'd, src=%s, deliverer %s "
            "class=0x%x, service=0x%x",
            eCilNameString( iAgsData.RxMsg.SourceId ),
            eCilNameString( CilId ), iAgsData.RxMsg.Class, 
            iAgsData.RxMsg.Service
         );
#if 0
         iAgsErrReply(&(iAgsData.RxMsg), Status);
#endif
      }
   }

   if (Status == E_CIL_TIMEOUT)
   {
     /*
     ** Send monitoring command (CENTROID) to Agg.
     */
     Status = iAgsProcessMonitorCommands( I_AGS_CENTROID_ONLY );

     /* 
     ** We have received no message. We need to check to see if
     ** we are missing heartbeats because if we are we need to
     ** go safe. See how long it has been since the last heartbeat.
     */
     if ((CurrentTime.t_sec - iAgsData.LastHeartbeatTime.t_sec) >= 
          I_AGS_LOSS_HB_SEC)
     {
        /* 
        ** We have not received a heartbeat in a while so set the
        ** missing heartbeat flag.
        */ 
        /* Set the flag that requests safestate due to loss of heartbeats. */
        iAgsData.DemandState = SYS_SAFE_STATE;

        /* Report the message on the change NoRecentHBeats --> FALSE */
        eLogNotice(
            E_AGS_NO_HEARTBEATS, "Heartbeats lost - going to safe-state"
        );

        /* Assign count at which heartbeats were lost */ 
        iAgsData.HeartbeatLossCount = iAgsData.HeartbeatCount;

        /* 
        ** Reset the heartbeat loss time to the current time, this will 
        ** prevent us from immediately dectecting the same problem again. 
        */
        
        iAgsData.LastHeartbeatTime.t_sec = CurrentTime.t_sec;

      }

   }
 
   /* Process any outstanding state change requests for the AGS. */

   Status = iAgsProcessStateChange();

   Status = iAgsProcessCmdQueue();

   /* Do not return an error just because the command queue was empty. */
   if ( Status == E_AGS_CMDQ_EMPTY )
   {
     Status = SYS_NOMINAL;
   }

   return Status;

}  /* End of iAgsReceiveMessage() */


/*******************************************************************************
** Function Name:
**    iAgsShutdown
**
** Purpose:
**    Shutdown function for Ags.
**
** Description:
**    This function shuts down the AGS. A message is logged noting the shutdown,
**    and then the program is exited.
**
** Return Type:
**    void  
**
** Arguments:
**    none
**
** Author(s):
**    Steve Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

void iAgsShutdown ( void )
{
   /* Perform any last minute tidying up of AGG (ie send END_SESSION ???) */
   /* Function call(s) here... */
   
   /* Log the shutdown */
   eLogNotice(0, "Application terminating");

   /* Call the exit function, with success */
   exit( EXIT_SUCCESS );

}  /* End of iAgsShutdown() */

/*******************************************************************************
** Function Name:
**    iAgsCheckSafeState
**
** Purpose:
**    Function to check if AGS needs to go to safe state or has attained it.
**
** Description:
**    This function checks if the AGS has been requested to go to safe state
**    or, if requested, whether or not it has attained it. The reason for
**    splitting this functionality off into a separate function is that the
**    AGG may not immediately achieve safe state, because, for exmple, the
**    autoguider may be axquiring an imager, which can take a long time.
**    requires the enclosure to be closed. From the fully open position, 
**    Thus, the checks are made periodically by calling this function,
**    and the rest of the SPT remains responsive to further input. 
**
** Return Type:
**    Status_t
**       Returns the status of this function on completion.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iAgsCheckSafeState ( void )
{
   /* Local variables */
   Status_t Status;                    /* Status of functions called */

   /* Set the status to nominal */
   Status = SYS_NOMINAL;


   /* Check if a request has been made to get the AGS to safe-state */
   if( iAgsData.SafeStateReq == TRUE )
   {
      /* Have we reached a safe state yet ? */
      if( mAgsAggAlreadySafe == TRUE)
      {
         /* If we are safe, then we can set the state to safe */
         iAgsSetState(SYS_SAFE_STATE);

         /* 
         ** Tell the MCP that we have completed going safe. 
         ** The Source and Dest will be flipped by the reply. 
         */
         iAgsData.ReplyMsg.SourceId   = E_CIL_MCP; 
         iAgsData.ReplyMsg.DestId     = iAgsData.CilId;

         Status = iAgsComReply( &(iAgsData.ReplyMsg) );
         

         /* Reset the safe-request flag */
         iAgsData.SafeStateReq = FALSE;
         eLogDebug("Safety check: Safe attained");
      }
      /* Are we attempting to go safe ?, if not start now. */
      else
      {
         Status = mAgsAggGoSafe();

         /*
         ** Once the safe state command is sent, set the mAgsAggAlreadySafe 
         ** flag to ensure that it is not sent again.
         */
         mAgsAggAlreadySafe = TRUE;

         eLogDebug("Safety check: Safe requested");
      }
   }

   /* Return the status of the last call */
   return Status;

}  /* End of iAgsCheckSafeState() */

/*******************************************************************************
** Function Name:
**    mAgsAggGoSafe
**
** Purpose:
**    Sends command to AGG to make it safe.
**
** Description:
**    Turn AGG logging off. 
**
** Return Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**
*******************************************************************************/


Status_t mAgsAggGoSafe  ( void )
{
   Status_t Status;       
   iAgsCmd_t SafeMessage;
   eAgsCmdBody_t TcsCmd = { 0, 0, 0, 0 };
   eAggCmdBody_t AggCmd;

   Status = SYS_NOMINAL;

   /* Form SET LOGGING OFF command in Agg command structure. */
   SafeMessage.AggCmd.SourceId = iAgsData.CilId;
   SafeMessage.AggCmd.DestId   = iAgsData.AggCilId;
   SafeMessage.AggCmd.Class    = E_CIL_CMD_CLASS;   
   SafeMessage.AggCmd.Service  = E_AGG_CMD;    /* Service = AGG command. */ 
   SafeMessage.AggCmd.SeqNum   = I_AGS_NULL_SEQNUM; /* Command not sent. */
   AggCmd.Command = E_AGG_CMD_SETLOGPATH;
   AggCmd.Status  = SYS_NOMINAL;
   sprintf( AggCmd.Parameters, "%d", E_AGG_OFF );

   Status = iAgsAddCmdToQueueStart( &SafeMessage,  &TcsCmd, &AggCmd );

   eLogDebug( "Added safe state command to start of queue" );

   if ( Status != SYS_NOMINAL )
   {
     eLogErr( Status,
              "Unable to send SET LOG OFF command to Agg for SAFE STATE." );
   }

   eLogDebug( "Acted on SAFE state request, returning 0X%X");

   return Status;

}  /* End of mAgsAggGoSafe */


/*******************************************************************************
** End of File Name: AgsMain.c
*******************************************************************************/
