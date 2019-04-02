/*******************************************************************************
** Module Name:
**    CmtMain.c
**
** Purpose:
**    Main module of the Computer Monitoring Task.
**
** Description:
**    The Computer Monitoring Task (CMT) is the software process that 
**    performs the monitoring of computer resources and motherboard
**    status. 
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.01 1-Nov-2000.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: CmtMain.c,v 1.8 2006/03/21 11:21:56 sxf Exp $
**
** History:
**    $Log: CmtMain.c,v $
**    Revision 1.8  2006/03/21 11:21:56  sxf
**    Added NTP lock functionaity for QNX6, Linux and VMS.
**
**    Revision 1.7  2004/01/13 12:44:37  mkb
**    Check for communications failure during command processing.
**
**    Revision 1.6  2004/01/13 12:34:21  mkb
**    Improve logging of communications failure.
**
**    Revision 1.5  2004/01/13 11:57:27  mkb
**    Shutdown on communications failure.
**
**    Revision 1.4  2003/02/10 16:45:38  sxf
**    Make call to iCmtDetermineState consistent with prototype.
**
**    Revision 1.3  2003/02/06 12:00:20  sxf
**    Added function iCmtDetermineState()
**
**    Revision 1.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 1.1  2000/11/14 17:28:54  asp
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
#include <sys/types.h>


/*
** System include files
*/
#include "Cmt.h"
#include "TtlSystem.h"
#include "Log.h"
#include "Tim.h"


/*
** Local include files
*/

#define M_CMT_MAIN_C         /* So data area defined, not declared extern */
#include "CmtPrivate.h"



/*******************************************************************************
** Function Name:
**    main
**
** Purpose:
**    Top level function of the CMT.
**
** Description:
**    Top level function of the CMT. It is called on startup of the executable.
**    It requests initialisation, including the parsing of the command-line, 
**    and then calls the main program loop.
**
** Return Type:
**    int  
**       Returns the completion status of CMT program.
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
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


int main( int argc, char *argv[] )
{
   Status_t Status;                    /* Returned status of functions called */


   /* Initial state is initialising */ 
   iCmtData.NominalState = SYS_NOMINAL;
   iCmtSetState( SYS_INIT_STATE );

   /* Perform initialisation passing runtime parameters */
   Status = iCmtSetup( argc, argv );

   /* If initialisation has not passed, or help only requested */
   if ( ( Status != SYS_NOMINAL ) || ( eCluCommon.Help == TRUE ) )
   {
      /* Exit the CMT */
      iCmtShutdown();
   }

   /* Monitor the state of the computer */
   Status = iCmtMonitor();
   
   /* Exit the CMT - note that 'exit' is called within here */
   iCmtShutdown();

   return EXIT_SUCCESS;

}  /* End of main() */



/*******************************************************************************
** Function Name:
**    iCmtMonitor
**
** Purpose:
**    Function to loop monitoring the status of the Computer.
**
** Description:
**    This function performs the monitoring of the status of computer.
**    It loops reading the system status and is terminated only by a 
**    shutdown request handled by the housekeeping.
**
** Return Type:
**    Status_t
**       Returns the status of this function on completion.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/


Status_t iCmtMonitor( void )
{
   Status_t Status;                    /* Completion status of function */
   int      PrevHeartbeatCount;        /* Used to check for heartbeat rx'd */


   /* Default state is nominal */
   Status = SYS_NOMINAL;

   /* Initially no heartbeats received, for purposes of previous count */
   PrevHeartbeatCount = 0;

   /* Start loop - terminated by receipt of shutdown command in housekeeping */
   while( Status != SYS_FAILED_STATE )
   {
      /* If heartbeat has been received, get status  */
      if ( PrevHeartbeatCount != iCmtData.HeartbeatCount )
      {
         /* Assign previous heartbeat count to current count */
         PrevHeartbeatCount = iCmtData.HeartbeatCount;

         /* Get status from external device */
         Status = iCmtGetExtStatus();

         iCmtDetermineState();

         /* Submit the gathered status information to the SDB */
         Status = iCmtSubmitStatus();
      }

      /* Check and process any incoming messages */
      if (Status != SYS_FAILED_STATE)
      {
        Status = iCmtReceiveMessage();
      }

   }

   return Status;

}  /* End of iCmtMonitor() */



/*******************************************************************************
** Function Name:
**    iCmtReceiveMessage
**
** Purpose:
**    Function to receive and process messages for the CMT.
**
** Description:
**    This function receives any incoming CIL messages and processes them 
**    accordingly. Heartbeats are received and responded to, MCP messages are 
**    received, processed and responded to. An ackowledgement to an SDB 
**    submission may be received, and this is checked for an error. This 
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
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iCmtReceiveMessage( void )
{
   char InBuffer[ I_CMT_CIL_DATALEN ]; /* Buffer for incoming CIL messages */ 
   Status_t Status;                    /* Status of functions called */
   Int32_t  CilId;                     /* CIL ID of deliverer of message */
   Int32_t  SourceId;                  /* CIL ID of originator of message */
   Int32_t  SwapWord;                  /* Word used to swap byte-ordering */
   Bool_t   UnexpectedMessage;         /* Set TRUE if message is unexpected */
   eTtlTime_t CurrentTime;             /* Time at which housekeeping done */


   /* Set default status */
   Status = SYS_NOMINAL;
   UnexpectedMessage = FALSE;

   /* Take copy of current time */
   Status = eTimGetTime( &CurrentTime );

   /* Check for incoming CIL messages */
   iCmtData.RxMsg.DataPtr = &InBuffer;
   iCmtData.RxMsg.DataLen = sizeof( InBuffer );
   Status = eCilReceive( I_CMT_CIL_RX_TIMEOUT, &CilId, &iCmtData.RxMsg );


   /* Check for serious errors. The process will go into FAILED state and  */
   /* wait to be recovered. This also prevent repeated errors being logged */
   /* and the main loop from running flat-out.                             */
   if (Status == E_CIL_NO_SETUP)
   {

      eLogReport( E_LOG_ERR, Status, "Communications not configured before use." );

      Status = SYS_FAILED_STATE;
      iCmtSetState( Status );
      
      return Status;

   }
   if (Status == E_CIL_NO_MEMORY)
   {

      eLogReport( E_LOG_ERR, Status, 
      "Unable to allocate memory for incoming message." );

      Status = SYS_FAILED_STATE;
      iCmtSetState( Status );
      
      return Status;

   }
   if (Status == E_CIL_RECV_ERR)
   {

      eLogReport( E_LOG_ERR, Status, "Low level communications failure." );

      Status = SYS_FAILED_STATE;
      iCmtSetState( Status );
      
      return Status;

   }
   /* If an error occurred that was not timeout, then return it */
   else if ( ( Status != SYS_NOMINAL ) && ( Status != E_CIL_TIMEOUT ) )
   {
      eLogReport( E_LOG_ERR, Status, "Received CIL message unreadable." );
      return Status;
   }



   /* If a message received, process it */
   if ( Status == SYS_NOMINAL )
   {
      /* Take copy of source ID - checked for validity by the CIL function */
      SourceId = iCmtData.RxMsg.SourceId;

      /* Log debug for message received */
      eLogReport( E_LOG_DEBUG, 0, 
                  "CIL message received, source 0x%x, class 0x%x, service 0x%x",
                  iCmtData.RxMsg.SourceId, iCmtData.RxMsg.Class, 
                  iCmtData.RxMsg.Service );

      /* Check the service number */
      switch ( iCmtData.RxMsg.Service )
      {
         /* Heartbeat message */
         case E_MCP_HEARTBEAT :

            /* Check the source and class are valid */
            if ( ( SourceId == I_CMT_CIL_CHB ) 
                  && ( iCmtData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* Process the heartbeat message */
               Status = iCmtProcessHeartbeat( &CurrentTime );
            }
            /* Else unexpected message */
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* Shutdown requested */
         case E_MCP_SHUTDOWN :

            /* Only accepted from PMM, and must be a command */
            if ( ( SourceId == I_CMT_CIL_PMM ) 
                  && ( iCmtData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* Process the shutdown message */
               Status = iCmtProcessShutdown();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* Safe-state requested */
         case E_MCP_SAFESTATE :
   
            /* Only accept3ed from PMM, and must be a command */
            if ( ( SourceId == I_CMT_CIL_PMM ) 
                  && ( iCmtData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* Process the safe-state message */
               Status = iCmtProcessSafeState();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* Awaken from safe-state */
         case E_MCP_ACTIVATE :
  
            /* Only accepted from PMM, and must be a command */
            if ( ( SourceId == I_CMT_CIL_PMM ) 
                  && ( iCmtData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* Set process state back to active */
               Status = iCmtProcessActivate();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* Reply to SDB submission */
         case E_SDB_SUBMIT_1 :

            /* Should be sourced by SDB */
            if ( SourceId == I_CMT_CIL_SDB )
            {
               /* Check the message class */
               switch ( iCmtData.RxMsg.Class )
               {
                  /* Error responses sent back to CMT */
                  case E_CIL_ERR_CLASS :

                     /* Get incoming word, will be in network order */
                     memcpy( &SwapWord, iCmtData.RxMsg.DataPtr, 
                             sizeof( Int32_t ) );
                     eCilConvert32bitArray( sizeof( SwapWord ), &SwapWord );  

                     /* Error returned by SDB submission */
                     eLogReport( E_LOG_ERR, SwapWord, 
                                 "Error returned by SDB submission" );
                     break;

                  /* Successful SDB submission */
                  case E_CIL_ACK_CLASS :
 
                     /* Nothing to do */
                     break;

                  default :

                     UnexpectedMessage = TRUE;
                     break;
               }
            }
            break;

         /* Default is an error */
         default :

            UnexpectedMessage = TRUE;
            break;
      }

      /* Check for communications failure. */
      if (Status == SYS_FAILED_STATE)
      {

         eLogReport(
            E_LOG_ERR, E_CMT_ERR_CIL_RX, 
            "Communications failure while processing incoming command."
         );
         return Status;

      }

      /* If message is unexpected */
      if ( UnexpectedMessage == TRUE )
      {
         eLogReport(
            E_LOG_ERR, E_CMT_ERR_CIL_RX, 
            "Unexpected CIL msg rx, src=0x%x, delivered by 0x%x "
            "class=0x%x, service=0x%x",
            iCmtData.RxMsg.SourceId, CilId, iCmtData.RxMsg.Class, 
            iCmtData.RxMsg.Service
         );

      }
   }

   /* Check if heartbeats have been lost, since last time loss was detected */
   if ( iCmtData.HeartbeatLossCount != iCmtData.HeartbeatCount )
   {
      if ( eTimCompare( &CurrentTime, &iCmtData.HeartbeatLossTime ) 
            == E_TIM_TIMEA_GT_TIMEB )
      {
         /* Need to assume safe-state */
         Status = iCmtSafeState();
         /* Assign count at which heartbeats were lost */ 
         iCmtData.HeartbeatLossCount = iCmtData.HeartbeatCount;
         eLogReport( E_LOG_NOTICE, E_CMT_ERR_HEARTBEATS, 
                     "Heartbeats lost - assumed safe-state" );
         iCmtOutputStatus( FALSE );
      }
   }

   return SYS_NOMINAL;

}  /* End of iCmtReceiveMessage() */



/*******************************************************************************
** Function Name:
**    iCmtShutdown
**
** Purpose:
**    Shutdown function for CMT.
**
** Description:
**    This function shuts down the CMT. A message is logged noting the shutdown,
**    and then the program is exited.
**
** Return Type:
**    void  
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


void iCmtShutdown( void )
{
   /* Log the shutdown */
   eLogNotice(0, "Application terminating");

   /* Call the exit function, with success */
   exit( EXIT_SUCCESS );

}  /* End of iCmtShutdown() */

/*******************************************************************************
** Function Name:
**    iCmtDetermineState
**
** Purpose:
**    Determine current state for CMT
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
**    Steve Foale (sxf)
**
*******************************************************************************/

void iCmtDetermineState( void )
{

   Bool_t InWarn;

   /*
   ** Determine if monitored parameters should make the CMT
   ** enter WARN state.
   */
   if ( iCmtData.DiskWarn || iCmtData.NtpWarn )
   {
      InWarn = TRUE;
   }
   else
   { 
      InWarn = FALSE;
   }

   /*
   ** If the CMT is in WARN or OKAY state then use InWarn to
   ** determine overall CMT state. If not in one of these states
   ** then either something has failed, CMT is in SAFE state etc.
   */
   if ( ( iCmtData.ActualState == SYS_WARN_STATE ) ||
        ( iCmtData.ActualState == SYS_OKAY_STATE ) )
   {

      if ( InWarn == TRUE )
      {
         iCmtData.ActualState = SYS_WARN_STATE;
      }
      else
      {
         iCmtData.ActualState = SYS_OKAY_STATE;
      }
   }

}  /* End of iCmtDetermineState() */



/*******************************************************************************
** End of File Name: CmtMain.c
*******************************************************************************/
