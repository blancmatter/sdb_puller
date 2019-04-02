/*******************************************************************************
** Module Name:
**    CctMain.c
**
** Purpose:
**    Main module of the Computer Monitoring Task.
**
** Description:
**    The Computer Monitoring Task (CCT) is the software process that performs 
**    control and shutdown of the operating system on a computer.
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
**    $Id: CctMain.c,v 0.4 2001/07/25 10:04:28 mjf Exp $
**
** History:
**    $Log: CctMain.c,v $
**    Revision 0.4  2001/07/25 10:04:28  mjf
**    Allow control messages from the TST as well as the MCP.
**
**    Revision 0.3  2001/06/13 10:42:33  mjf
**    If a set/get command not received from the MCP, send reply via the MCB.
**
**    Revision 0.2  2001/03/08 16:54:44  mjf
**    Full functionality for QNX added. Datums only submitted to the SDB
**    when they are re-determined. Query datums are re-determined on a
**    corresponding 'get' command.
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

#define M_CCT_MAIN_C                /* so data area declared, not extern */
#include "Cct.h"
#include "CctPrivate.h"


/*******************************************************************************
** Function Name:
**    main
**
** Purpose:
**    Top level function of the CCT.
**
** Description:
**    Top level function of the CCT. It is called on startup of the executable.
**    It requests initialisation, including the parsing of the command-line, 
**    and then calls the main program loop.
**
** Return Type:
**    int  
**       Returns the completion status of CCT program.
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
**
*******************************************************************************/

int main( int argc, char *argv[] )
{
   Status_t Status;                    /* returned status of functions called */

   /* initial state is initialising */ 
   iCctData.NominalState = SYS_NOMINAL;
   iCctSetState( SYS_INIT_STATE );

   /* perform initialisation passing runtime parameters */
   Status = iCctSetup( argc, argv );

   /* if initialisation has not passed, or help only requested */
   if ( ( ( Status != SYS_NOMINAL ) && ( Status != SYS_WARNING ) )
         || ( eCluCommon.Help == TRUE ) )
   {
      /* exit the CCT */
      iCctShutdown();
   }

   /* monitor the state of the computer */
   Status = iCctMonitorComputer();
   
   /* exit the CCT - note that 'exit' is called within here */
   iCctShutdown();

   return EXIT_SUCCESS;

}


/*******************************************************************************
** Function Name:
**    iCctMonitorComputer
**
** Purpose:
**    Function to loop monitoring and controlling the status of the computer.
**
** Description:
**    This function performs the control and monitoring of the computer and 
**    operating system. It loops communicating with the external device, and is
**    terminated only by a shutdown request handled by the housekeeping.
**
** Return Type:
**    Status_t
**       Returns the status of this function on completion.
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctMonitorComputer ( void )
{
   int      PrevHeartbeatCount;        /* used to check for heartbeat rx'd */
   Status_t Status;                    /* completion status of function */

   /* default state is nominal */
   Status = SYS_NOMINAL;

   /* initially no heartbeats received, for purposes of previous count */
   PrevHeartbeatCount = 0;

   /* start loop - terminated by receipt of shutdown command in housekeeping */
   while( Status != SYS_FAILED_STATE )
   {
      /* if heartbeat has been received, get status from modem */
      if ( PrevHeartbeatCount != iCctData.HeartbeatCount )
      {
         /* assign previous heartbeat count to current count */
         PrevHeartbeatCount = iCctData.HeartbeatCount;

         /* submit version information into the SDB */
         Status = eCluSubmitVersion();

         /* submit any gathered status information to the SDB */
         Status = iCctSubmitStatus();
      }
            
      /* check and process any incoming messages */
      Status = iCctReceiveMessage( I_CCT_CIL_RX_TIMEOUT );
   }

   return Status;

}


/*******************************************************************************
** Function Name:
**    iCctReceiveMessage
**
** Purpose:
**    Function to receive and process messages for the CCT.
**
** Description:
**    This function receives any incoming CIL messages and processes them 
**    accordingly. Heartbeats and received and responded to, MCP messages are 
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
**    int Timeout                      (in)
**       The timeout (in mSec) to wait for an incoming CIL message.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t iCctReceiveMessage  ( int Timeout )
{
                                       /* buffer for incoming CIL messages */ 
   char     InBuffer[ I_CCT_CIL_DATALEN ];
   Status_t Status;                    /* status of functions called */
   Int32_t  CilId;                     /* CIL ID of deliverer of message */
   Int32_t  SourceId;                  /* CIL ID of originator of message */
   Int32_t  DataWord;                  /* word copied from packet body */
   Bool_t   UnexpectedMessage;         /* set TRUE if message is unexpected */
   eTtlTime_t CurrentTime;             /* time at which housekeeping done */

   /* set default status */
   Status = SYS_NOMINAL;
   UnexpectedMessage = FALSE;

   /* take copy of current time */
   Status = eTimGetTime( &CurrentTime );

   /* check for incoming CIL messages */
   iCctData.RxMsg.DataPtr = &InBuffer;
   iCctData.RxMsg.DataLen = sizeof( InBuffer );
   Status = eCilReceive( Timeout, &CilId, &iCctData.RxMsg );

   /* if an error occurred that was not timeout, then return it */
   if ( ( Status != SYS_NOMINAL ) && ( Status != E_CIL_TIMEOUT ) )
   {
      eLogReport( E_LOG_ERR, Status, "CIL message receive error" );
      return Status;
   }

   /* if a message received, process it */
   if ( Status == SYS_NOMINAL )
   {
      /* take copy of source ID - checked for validity by the CIL function */
      SourceId = iCctData.RxMsg.SourceId;

      /* convert the message body into host byte-order */
      if ( iCctData.RxMsg.DataLen > 0 )
      {
         Status = eCilConvert32bitArray( iCctData.RxMsg.DataLen, 
                                         iCctData.RxMsg.DataPtr );
      }

      /* log debug for message received */
      eLogReport( E_LOG_DEBUG, 0, 
                  "CIL message received, src %d '%s', cls 0x%x, svc 0x%x",
                  iCctData.RxMsg.SourceId, 
                  eCilNameString( iCctData.RxMsg.SourceId ), 
                  iCctData.RxMsg.Class, iCctData.RxMsg.Service );

      /* check the service number */
      switch ( iCctData.RxMsg.Service )
      {
         /* heartbeat message */
         case E_CCT_HEARTBEAT :

            /* check the source and class are valid */
            if ( ( SourceId == I_CCT_CIL_CHB ) 
                  && ( iCctData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* process the heartbeat message */
               Status = iCctProcessHeartbeat( &CurrentTime );
            }
            /* else unexpected message */
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* shutdown requested */
         case E_CCT_SHUTDOWN :

            /* only accepted from PMM, and must be a command */
            if ( ( SourceId == I_CCT_CIL_PMM ) 
                  && ( iCctData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* process the shutdown message */
               Status = iCctProcessShutdown();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* safe-state requested */
         case E_CCT_SAFESTATE :
   
            /* only accepted from PMM, and must be a command */
            if ( ( SourceId == I_CCT_CIL_PMM ) 
                  && ( iCctData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* process the safe-state message */
               Status = iCctProcessSafeState();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* awaken from safe-state */
         case E_CCT_ACTIVATE :
  
            /* only accepted from PMM, and must be a command */
            if ( ( SourceId == I_CCT_CIL_PMM ) 
                  && ( iCctData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* send 'action in progress' message back */
               Status = iCctProcessActivate();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* reply to SDB submission */
         case E_SDB_SUBMIT_1 :

            /* should be sourced by SDB */
            if ( SourceId == I_CCT_CIL_SDB )
            {
               /* check the message class */
               switch ( iCctData.RxMsg.Class )
               {
                  /* error responses sent back to CCT */
                  case E_CIL_ERR_CLASS :

                     /* get incoming word */
                     memcpy( &DataWord, iCctData.RxMsg.DataPtr, 
                             sizeof( Int32_t ) );
                     /* error returned by SDB submission */
                     eLogReport( E_LOG_ERR, DataWord, 
                                 "Error returned by SDB submission" );
                     break;

                  /* successful SDB submission */
                  case E_CIL_ACK_CLASS :
 
                     /* nothing to do */
                     break;

                  default :

                     UnexpectedMessage = TRUE;
                     break;
               }
            }
            break;

         /* set command */
         case E_CCT_SET_1 :

            /* only accepted from PMM, and must be a command */
            if ( ( ( SourceId == I_CCT_CIL_CONTROL ) 
                    || ( SourceId == I_CCT_CIL_ALTCONTROL ) )
                  && ( iCctData.RxMsg.Class == E_CIL_CMD_CLASS ) )
            {
               /* call function to process an incoming command */
               Status = iCctProcessCommand();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* get command */
         case E_CCT_GET_1 :

            /* must be a command */
            if ( iCctData.RxMsg.Class == E_CIL_CMD_CLASS )
            {
               /* call function to process an incoming command */
               Status = iCctProcessCommand();
            }
            else
            {
               UnexpectedMessage = TRUE;
            }
            break;

         /* default is an error */
         default :

            UnexpectedMessage = TRUE;
            break;
      }

      /* if message is unexpected */
      if ( UnexpectedMessage == TRUE )
      {
         eLogReport( E_LOG_ERR, E_CCT_ERR_CIL_RX, 
                     "Unexpected CIL msg rx, src %d '%s', cls 0x%x, svc 0x%x",
                     iCctData.RxMsg.SourceId, 
                     eCilNameString( iCctData.RxMsg.SourceId ), 
                     iCctData.RxMsg.Class, iCctData.RxMsg.Service );

      }
   }

   /* check if heartbeats have been lost, since last time loss was detected */
   if ( iCctData.HeartbeatLossCount != iCctData.HeartbeatCount )
   {
      if ( eTimCompare( &CurrentTime, &iCctData.HeartbeatLossTime ) 
            == E_TIM_TIMEA_GT_TIMEB )
      {
         /* need to assume safe-state */
         iCctSetState( SYS_SAFE_STATE );

         /* assign count at which hwartbeats were lost */ 
         iCctData.HeartbeatLossCount = iCctData.HeartbeatCount;
         eLogReport( E_LOG_NOTICE, E_CCT_ERR_HEARTBEATS, 
                     "Heartbeats lost - assumed safe-state" );
         iCctOutputStatus( FALSE );
      }
   }

   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctShutdown
**
** Purpose:
**    Shutdown function for CCT.
**
** Description:
**    This function shuts down the CCT. A message is logged noting the shutdown,
**    and then the program is exited.
**
** Return Type:
**    void
**
** Arguments:
**    None
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

void iCctShutdown            ( void )
{
   Status_t Status;                    /* status of functions called */

   /* set default status */
   Status = SYS_NOMINAL;

   /* log the shutdown */
   eLogReport( E_LOG_NOTICE, 0, "Application terminating" );

   /* call the exit function, with success */
   exit( EXIT_SUCCESS );

}


/*
** EOF
*/
