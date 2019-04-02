/*******************************************************************************
** Module Name:
**    CctUtil.c
**
** Purpose:
**    Utility functions used by the Computer Control Task.
**
** Description:
**    This source file contains various utility functions used by the CCT. 
**    Functions are provided to manipulate state, output status to the console,   
**    and submit data to the SDB.
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
**    $Id: CctUtil.c,v 0.12 2003/12/17 15:00:41 mjf Exp $
**
** History:
**    $Log: CctUtil.c,v $
**    Revision 0.12  2003/12/17 15:00:41  mjf
**    Allow a command to be accepted with 'NO_UNITS' as well as the
**    existing 'TRUEFALSE_UNITS'.
**
**    Revision 0.11  2002/10/29 15:18:47  mjf
**    Don't check for existence of config files on every heartbeat.
**
**    Revision 0.10  2001/11/07 09:09:22  mjf
**    Take a copy of the incoming CIL message containing the command, in
**    case a heartbeat is processed between the command being received and
**    responded to.
**
**    Revision 0.9  2001/07/17 13:39:57  mjf
**    If all configuration files not available, then go to 'warn' state.
**
**    Revision 0.8  2001/06/19 15:46:19  mjf
**    Correction to log message.
**
**    Revision 0.7  2001/06/13 10:42:33  mjf
**    If a set/get command not received from the MCP, send reply via the MCB.
**
**    Revision 0.6  2001/06/13 09:37:12  mjf
**    Query all the status information and submit it into the SDB on each
**    heartbeat.
**
**    Revision 0.5  2001/05/25 13:30:29  mjf
**    On an operating system shutdown or restart command, send the reply to
**    the command before performing the requested action.
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
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
** System include files
*/

#include "Log.h"
#include "Tim.h"


/*
** Local include files
*/

#include "Cct.h"
#include "CctPrivate.h"


/*******************************************************************************
**
** Function Name:
**    iCctSubmitStatus
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to supervise submission of data into the Status Database.
**
** Description:
**    This function first copies the number of elements to submit into the SDB,
**    and then copies the datums for submission. The data is copied as it is 
**    byte-swapped before transmission using CIL. The sequence number is 
**    incremented, and then the function called to transmit the CIL message 
**    (note that the byte-swapping is done within here).
**
** Arguments:
**    void
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iCctSubmitStatus    ( void )
{
   int      Index;                     /* counter to iterate around datums */
   Status_t Status;                    /* completion status of this function */
   eTtlTime_t CurrentTime;             /* current time-stamp */
   iCctSdbData_t SdbCopy;              /* copy of data for SDB submission */

   /* initialise status */
   Status = SYS_NOMINAL;

   /* initially no elements have been determined to be ready for submission */
   SdbCopy.NumElts = 0;

   /* take a copy of the current time */
   Status = eTimGetTime( &CurrentTime );

   /* iterate around datums for possible submission into the SDB */
   for ( Index = D_MCP_FIRST_USER_DATUM; Index < D_CCT_DATAID_EOL; Index++ )
   {
      /* only submit status to SDB if valid parameters obtained */
      if ( iCctData.Param[ Index ].StatusObtained == TRUE )
      {
         /* take copy of datum to submit to SDB */
         memcpy( &( SdbCopy.Datum[ SdbCopy.NumElts ] ), 
                 &iCctData.Param[ Index ].Datum,
                 sizeof( eSdbDatum_t ) );

         /* add in the timestamp */
         memcpy( &SdbCopy.Datum[ SdbCopy.NumElts ].Msrment.TimeStamp, 
                 &CurrentTime, 
                 sizeof( CurrentTime ) );

         /* increment the number of elements for submission */
         SdbCopy.NumElts++;

         /* clear the flag indicating status for this datum freshly obtained */
         iCctData.Param[ Index ].StatusObtained = FALSE;
      }
   }

   /* if one or more elements found for submission into the SDB */
   if ( SdbCopy.NumElts > 0 )
   {
      /* increment the sequence number for SDB submission message */
      iCctData.SdbSubmissionMsg.SeqNum++;

      /* point to the constructed SDB message to transmit */
      iCctData.SdbSubmissionMsg.DataPtr = &SdbCopy;

      /* determine the length of the SDB submission */
      iCctData.SdbSubmissionMsg.DataLen = sizeof( SdbCopy.NumElts ) 
                                  + ( SdbCopy.NumElts * sizeof( eSdbDatum_t ) );

      /* send message - message body is byte-swapped within this function */
      Status = iCctSendCilMsg( iCctData.SdbSubmissionMsg.DestId, 
                               &iCctData.SdbSubmissionMsg );
   }
   
   return Status;

}


/*******************************************************************************
**
** Function Name:
**    iCctOutputStatus
**
** Return Type:
**    void
**
** Purpose:
**    Outputs the current CCT status to the console.
**
** Description:
**    Outputs a line to the screen detailing progress of the CCT, and the 
**    state and status of current parameters.
**
** Arguments:
**    Bool_t TimeFlag                  (in)
**       TRUE if the time is to be updated.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

void     iCctOutputStatus    ( Bool_t TimeFlag )
{
   Status_t Status;                    /* status of returned functions */
   ProcState_t CctState;               /* copy of CCT state */
   eTtlTime_t CurrentTime;             /* current system time */
                                       /* ASCII representation of time */
   static char TimeBuffer[ E_TIM_BUFFER_LENGTH ] = { "\0" };
   static char * StateString[] =       /* strings for system states */
      { "INVALID",
        "OKAY   ",
        "INIT   ",
        "STANDBY",
        "WARN   ",
        "FAILED ",
        "SAFE   ",
        "OFF    ",
        "TIMEOUT",
        "       " };
 
   /* only output if verbose specified */
   if ( eCluCommon.Verbose == TRUE )
   {
      /* if time is to be updated */
      if ( TimeFlag == TRUE )
      {
         /* get the current time */
         Status = eTimGetTime( &CurrentTime );

         /* convert time into a string */
         Status = eTimToString( &CurrentTime, E_TIM_BUFFER_LENGTH, TimeBuffer );
      }

      /* take copy of CCT state */
      CctState = iCctGetState();
 
      /* print the output */ 
      fprintf( stdout, "\r%s %s State=%s Heartbeats=%d",
               eCluCommon.CilName, TimeBuffer, StateString[ CctState ], 
               iCctData.HeartbeatCount );
      fflush( stdout );
   }

}


/*******************************************************************************
**
** Function Name:
**    iCctSetState
**
** Return Type:
**    void
**
** Purpose:
**    Sets the supplied overall CCT state.
**
** Description:
**    Sets the overall CCT state using the parameter supplied. If the new state 
**    is not safe-state, then a copy of the state is taken for future 
**    restoration when the process is activated after safe-state.
**
** Arguments:
**    ProcState_t CctState             (in)
**       The new state for the CCT to assume.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

void     iCctSetState        ( ProcState_t CctState )
{

   /* if safe-state requested, only set the 'nominal' state */
   if ( CctState == SYS_SAFE_STATE )
   {
      /* assign new nominal state in the data area */
      iCctData.NominalState = SYS_SAFE_STATE;
   }
   /* else if a change in state is requested */
   else if( iCctData.ActualState != CctState )
   {
      /* assign new state in the data area */
      iCctData.ActualState = CctState;
   }

}


/*******************************************************************************
**
** Function Name:
**    iCctRestoreState
**
** Return Type:
**    void
**
** Purpose:
**    Restores the state on ending of safe-state.
**
** Description:
**    When a process receives an activate message, the actual state is restored 
**    to replace safe-state. This is achieved by having two states - the actual
**    state and nominal state. In this case the nominal state is set back to 
**    'nominal'.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

void     iCctRestoreState    ( void )
{

   /* restore nominal state to be SYS_NOMINAL */
   iCctData.NominalState = SYS_NOMINAL;

}


/*******************************************************************************
**
** Function Name:
**    iCctGetState
**
** Return Type:
**    ProcState_t
**       Returns the resolved state of this process. The resolved state may 
**       depend on a combination of several states.
**
** Purpose:
**    Determines and returns the resolved state of this process.
**
** Description:
**    The resolved state is determined using the actual and nominal states.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

ProcState_t iCctGetState     ( void )
{
   ProcState_t ResolvedState;          /* determined state for this process */

   /* if the 'nominal state' is 'nominal', return the 'actual state' */
   if ( iCctData.NominalState == SYS_NOMINAL )
   {
      ResolvedState = iCctData.ActualState;
   }
   /* else we are in safe-state, return this */
   else            
   {
      ResolvedState = SYS_SAFE_STATE;
   }

   return ResolvedState;

}


/*******************************************************************************
**
** Function Name:
**    iCctSendCilMsg
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Sends a CIL message to a software process.
**
** Description:
**    Accepts a pointer to a CIL message structure, and sends a CIL message to 
**    the appropriate software process. Also passed in the CIL ID, which is 
**    expected to be valid at this stage. Error reporting is done within this 
**    function.
**
** Arguments:
**    Int32_t CilId                    (in)
**       CIL ID of process to send message to.
**    eCilMsg * CilMsg                 (in/out)
**       Pointer to structure of parameters for CIL message transmission.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iCctSendCilMsg      ( Int32_t CilId, eCilMsg_t * CilMsg )
{
   Status_t Status;                    /* completion status of this function */

   /* initially status is nominal */
   Status = SYS_NOMINAL;

   /* correctly re-order the bytes in the packet body as required */
   if ( CilMsg->DataLen > 0 )
   {
      Status = eCilConvert32bitArray( CilMsg->DataLen, CilMsg->DataPtr );
   }

   eLogReport( E_LOG_DEBUG, 0, 
               "Sending CIL message 0x%x to CIL ID %d '%s', class %x", 
               CilMsg->Service, 
               CilMsg->DestId, eCilNameString( CilMsg->DestId ), 
               CilMsg->Class );

   /* send the message */
   Status = eCilSend( CilId, CilMsg );

   /* check status and report any errors */
   if ( Status != SYS_NOMINAL )
   {
      eLogReport( E_LOG_ERR, Status, 
                  "Error sending CIL message 0x%x to CIL ID %d '%s'",
                  CilMsg->Service, 
                  CilMsg->DestId, eCilNameString( CilMsg->DestId ) ); 
   }

   /* return status of attempt to transmit */
   return Status;

}


/*******************************************************************************
**
** Function Name:
**    iCctProcessHeartbeat
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to process an incoming hearbeat request message.
**
** Description:
**    This function determines the current state of this process, and responds 
**    to the heartbeat message with it. A copy of the passed in time is made to
**    be subsequently used to determine when heartbeats are lost.
**
** Arguments:
**    eTtlTime_t * TimeReceived        (in)
**       Pointer to structure 
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iCctProcessHeartbeat( eTtlTime_t * TimeReceived )
{
   int      Index;                     /* to loop around the query data IDs */
   Status_t Status;                    /* status returned by functions */
   Status_t TimStatus;                 /* status returned by time functions */
   ProcState_t CurrentState;           /* current process state for return */
   ProcState_t NewState;               /* new process state to be set */

   /* initially assume the new required state is OKAY */
   NewState = SYS_OKAY_STATE;

   /* make request to set the new process state */
   iCctSetState( NewState );

   /* respond to heartbeat with our state */
   CurrentState = iCctGetState();

   /* point to return packet and assign sequence number */
   iCctData.ChbResponseMsg.DataPtr = &CurrentState;
   iCctData.ChbResponseMsg.SeqNum = iCctData.RxMsg.SeqNum;

   /* respond to message */
   Status = iCctSendCilMsg( iCctData.ChbResponseMsg.DestId, 
                            &iCctData.ChbResponseMsg );

   /* increment number of heartbeats received */
   iCctData.HeartbeatCount++;

   /* take copy of time to be used as last heartbeat time */
   TimStatus = eTimCopy( *TimeReceived, &iCctData.LastHeartbeat );

   /* time at which heartbeats will be considered lost */
   TimStatus = eTimSum( &iCctData.LastHeartbeat, 
                        &iCctData.HeartbeatTimeOut,
                        &iCctData.HeartbeatLossTime );

   /* loop around the query datums, to submit status data into the SDB */
   for ( Index = E_CCT_QRY_MIN; Index <= E_CCT_QRY_MAX; Index++ )
   {
      Status = iCctData.Param[ Index ].Function();
   }

   /* update on-screen verbose output */
   iCctOutputStatus( TRUE );

   /* return status of CIL transaction */
   return Status;

}


/*******************************************************************************
**
** Function Name:
**    iCctProcessShutdown
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to process an incoming shutdown request message.
**
** Description:
**    This function forms and transmits an acknowledgement to the shutdown
**    request, and then sets a pending shutdown request.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iCctProcessShutdown( void )
{
   Status_t CilStatus;                 /* status returned by functions */

   /* construct 'acknowledgement' message back */
   iCctData.McpReplyMsg.DestId   = iCctData.RxMsg.SourceId;
   iCctData.McpReplyMsg.Class    = E_CIL_ACK_CLASS;
   iCctData.McpReplyMsg.Service  = iCctData.RxMsg.Service;
   iCctData.McpReplyMsg.SeqNum   = iCctData.RxMsg.SeqNum;
   iCctData.McpReplyMsg.DataPtr  = NULL;
   iCctData.McpReplyMsg.DataLen  = E_MCP_SHUTDOWN_ACK_LEN;

   /* send the acknowledgement message */
   CilStatus = iCctSendCilMsg( iCctData.McpReplyMsg.DestId, 
                               &iCctData.McpReplyMsg );

   /* call the shutdown function */
   iCctShutdown();

   /* return status of CIL transactions */
   return CilStatus;

}


/*******************************************************************************
**
** Function Name:
**    iCctProcessSafeState
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to process an incoming safe-state request message.
**
** Description:
**    This function forms and transmits an action-in-progress message to the 
**    safe-state command, sets the safe-state, then sends an action completed
**    message.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iCctProcessSafeState( void )
{
   Status_t CilStatus;                 /* status returned by CIL functions */
   ProcState_t CurrentState;           /* current process state for return */

   /* respond to heartbeat with our state */
   CurrentState = iCctGetState();

   /* send 'action in progress' message back */ 
   iCctData.McpReplyMsg.DestId   = iCctData.RxMsg.SourceId;
   iCctData.McpReplyMsg.Class    = E_CIL_ACT_CLASS;
   iCctData.McpReplyMsg.Service  = iCctData.RxMsg.Service;
   iCctData.McpReplyMsg.DataPtr  = NULL;
   iCctData.McpReplyMsg.DataLen  = E_MCP_SAFESTATE_ACT_LEN;
   iCctData.McpReplyMsg.SeqNum   = iCctData.RxMsg.SeqNum;

   /* send the message */
   CilStatus = iCctSendCilMsg( iCctData.McpReplyMsg.DestId, 
                               &iCctData.McpReplyMsg );

   /* set the safe state */
   iCctSetState( SYS_SAFE_STATE );

   /* send 'completed' message back */ 
   iCctData.McpReplyMsg.DestId   = iCctData.RxMsg.SourceId;
   iCctData.McpReplyMsg.Class    = E_CIL_COM_CLASS;
   iCctData.McpReplyMsg.Service  = iCctData.RxMsg.Service;
   iCctData.McpReplyMsg.DataPtr  = &CurrentState;
   iCctData.McpReplyMsg.DataLen  = E_MCP_SAFESTATE_COM_LEN;
   iCctData.McpReplyMsg.SeqNum   = iCctData.RxMsg.SeqNum;

   /* send the message */
   CilStatus = iCctSendCilMsg( iCctData.McpReplyMsg.DestId, 
                               &iCctData.McpReplyMsg );

   /* return status of CIL transactions */
   return CilStatus;

}


/*******************************************************************************
**
** Function Name:
**    iCctProcessActivate
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to process an incoming activate request message.
**
** Description:
**    This function forms and transmits an action-in-progress message to the 
**    activate command, sets the safe-state, then sends an action completed
**    message.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iCctProcessActivate( void )
{
   Status_t CilStatus;                 /* status returned by CIL functions */
   ProcState_t CurrentState;           /* current process state for return */

   /* respond to heartbeat with our state */
   CurrentState = iCctGetState();

   /* send 'action in progress' message back */ 
   iCctData.McpReplyMsg.DestId   = iCctData.RxMsg.SourceId;
   iCctData.McpReplyMsg.Class    = E_CIL_ACT_CLASS;
   iCctData.McpReplyMsg.Service  = iCctData.RxMsg.Service;
   iCctData.McpReplyMsg.DataPtr  = NULL;
   iCctData.McpReplyMsg.DataLen  = E_MCP_ACTIVATE_ACT_LEN;
   iCctData.McpReplyMsg.SeqNum   = iCctData.RxMsg.SeqNum;

   /* send the message */
   CilStatus = iCctSendCilMsg( iCctData.McpReplyMsg.DestId, 
                               &iCctData.McpReplyMsg );

   /* restore nominal state to be SYS_NOMINAL */
   iCctData.NominalState = SYS_NOMINAL;

   /* send 'completed' message back */ 
   iCctData.McpReplyMsg.DestId   = iCctData.RxMsg.SourceId;
   iCctData.McpReplyMsg.Class    = E_CIL_COM_CLASS;
   iCctData.McpReplyMsg.Service  = iCctData.RxMsg.Service;
   iCctData.McpReplyMsg.DataPtr  = &CurrentState;
   iCctData.McpReplyMsg.DataLen  = E_MCP_ACTIVATE_COM_LEN;
   iCctData.McpReplyMsg.SeqNum   = iCctData.RxMsg.SeqNum;

   /* send the message */
   CilStatus = iCctSendCilMsg( iCctData.McpReplyMsg.DestId, 
                               &iCctData.McpReplyMsg );

   /* return status of CIL transactions */
   return CilStatus;

}


/*******************************************************************************
**
** Function Name:
**    iCctProcessCommand
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Purpose:
**    Function to respond to an incoming command. The command may be a SET_1 or
**    a GET_1.
**
** Description:
**    This function handles an incoming SET_1 or GET_1 command. The parameters
**    of the command are checked to be valid. For a SET_1 command the requested
**    datum assignment is made, if valid. Both a SET_1 and GET_1 command have 
**    the same response.
**
** Arguments:
**    None
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iCctProcessCommand  ( void )
{
   Int32_t  DataLen;                   /* length of incoming command */
   Status_t Status;                    /* function status variable */
   eCilMsg_t   MsgIn;                  /* copy of incoming CIL message */
   eTtlSet_t * SetCmdPtr;              /* pointer to incoming set command */

   /* take copy of incoming CIL message */
   memcpy( &MsgIn, &iCctData.RxMsg, sizeof( MsgIn ) );

   /* take copy of incoming data length */
   DataLen = MsgIn.DataLen;

   /* overlay the set command structure onto the incoming message */
   SetCmdPtr = ( eTtlSet_t * ) MsgIn.DataPtr;

   /* set the default status to SYS_NOMINAL */
   Status = SYS_NOMINAL;

   /* check the length of the incoming set/get command */
   if ( DataLen != sizeof( eTtlSet_t ) )
   {
      /* handle error in message length */
      Status = E_CCT_ERR_COMMAND_LEN;
      eLogReport( E_LOG_ERR, Status, 
                  "Invalid length of incoming command (%d expecting %d)",
                  DataLen, sizeof( eTtlSet_t ) );
      /* attempt to report error to the originating task */
      iCctSetReply( &MsgIn, Status );
      return Status;
   }

   /* process the case of a 'get' command here */
   if ( MsgIn.Service == E_CCT_GET_1 )
   {
      /* check for a query datum */
      if ( ( SetCmdPtr->DataId >= E_CCT_QRY_MIN ) 
            && ( SetCmdPtr->DataId <= E_CCT_QRY_MAX ) )
      {
         /* call function to get the relevant piece of status information */
         Status = iCctData.Param[ SetCmdPtr->DataId ].Function();
         /* valid 'get' command has same response as a successful 'set' */
         Status = SYS_NOMINAL;
         iCctSetReply( &MsgIn, Status );
         return Status;
      }
      /* else check for a demand datum */
      else if ( ( SetCmdPtr->DataId >= E_CCT_DMD_MIN ) 
               && ( SetCmdPtr->DataId <= E_CCT_DMD_MAX ) )
      {
         /* valid 'get' command has same response as a successful 'set' */
         Status = SYS_NOMINAL;
         iCctSetReply( &MsgIn, Status );
         return Status;
      }
      /* else datum not within range */
      else
      {
         /* report the error */
         Status = E_CCT_ERR_COMMAND_DATUM;
         eLogReport( E_LOG_ERR, Status, 
                     "Get command requested using invalid datum (%d)", 
                     SetCmdPtr->DataId );
         /* attempt to report error to the originating task */
         iCctSetReply( &MsgIn, Status );
         return Status;
      }
   }
   
   /* check the status of the 'set' command */
   if ( SetCmdPtr->Status != SYS_NOMINAL )
   {
      /* Handle error in set command status */
      Status = E_CCT_ERR_COMMAND;
      eLogReport( E_LOG_ERR, Status, 
                  "Set command contains invalid status (%d)", 
                  SetCmdPtr->Status );
      /* attempt to report error to the originating task */
      iCctSetReply( &MsgIn, Status );
      return Status;
   }

   /* check the number of bytes in the 'set' command parameter */
   if ( SetCmdPtr->NumBytes != sizeof( Int32_t ) )
   {
      /* handle error in set command parameter length */
      Status = E_CCT_ERR_COMMAND;
      eLogReport( E_LOG_ERR, Status, 
                  "Set command contains invalid parameter length (%d)", 
                  SetCmdPtr->NumBytes );
      /* attempt to report error to the originating task */
      iCctSetReply( &MsgIn, Status );
      return Status;
   }

   /* check the 'set' command time-stamp - only immediate commands accepted */
   if ( ( SetCmdPtr->TimeStamp.t_sec != 0 ) 
         || ( SetCmdPtr->TimeStamp.t_nsec != 0 ) )
   {
      /* handle non-immediate command parameter length */
      Status = E_CCT_ERR_COMMAND;
      eLogReport( E_LOG_ERR, Status, 
                  "Set command with non-immediate time-stamp rejected" );
      /* attempt to report error to the originating task */
      iCctSetReply( &MsgIn, Status );
      return Status;
   }

   /* check the datum is within range */
   if ( ( SetCmdPtr->DataId >= E_CCT_DMD_MIN ) 
         && ( SetCmdPtr->DataId <= E_CCT_DMD_MAX ) )
   {
      /* check the units to be correct */
      if ( ( SetCmdPtr->Units != E_SDB_TRUEFALSE_UNITS ) 
            && ( SetCmdPtr->Units != E_SDB_NO_UNITS ) )
      {
         /* incorrect units */
         Status = E_CCT_ERR_COMMAND;
         eLogReport( E_LOG_ERR, Status, 
                     "Set command contains incorrect units (%d)",
                     SetCmdPtr->Units );
         /* attempt to report error to the originating task */
         iCctSetReply( &MsgIn, Status );
         return Status;
      }

      /* check the value to be within range */
      if ( ( SetCmdPtr->ParamLow != E_SYS_FALSE ) 
            && ( SetCmdPtr->ParamLow != E_SYS_TRUE ) )
      {
         /* invalid value */
         Status = E_CCT_ERR_COMMAND;
         eLogReport( E_LOG_ERR, Status, 
                     "Invalid requested demand flag %d", SetCmdPtr->ParamLow );
         /* sttempt to report error to the originating task */
         iCctSetReply( &MsgIn, Status );
         return Status;
      }
         
      /* set the new demand */
      iCctData.Param[ SetCmdPtr->DataId ].Datum.Msrment.Value 
         = SetCmdPtr->ParamLow;
      eTimGetTime( &iCctData.Param[ SetCmdPtr->DataId ].ModificationTime );

      /* indicate that the reply to the command has not been sent */
      iCctData.ReplySent = FALSE;

      /* call the function to attend to the demand */
      Status = iCctData.Param[ SetCmdPtr->DataId ].Function();

      /* if the reply not sent from within the function */
      if ( iCctData.ReplySent == FALSE )
      {
         /* attempt to report acknowledgement to the originating task */
         Status = SYS_NOMINAL;
         iCctSetReply( &MsgIn, Status );
      }

      /* make additional submission of CCT data to SDB */
      Status = iCctSubmitStatus();
      if( Status != SYS_NOMINAL )
      {
         eLogCrit( Status, "Failure to submit CCT data" );
         return Status;
      }
   }
   /* else datum is out of range */
   else
   {
      /* report the error */
      Status = E_CCT_ERR_COMMAND_DATUM;
      eLogReport( E_LOG_ERR, Status, 
                  "Set command requested using invalid datum (%d)", 
                  SetCmdPtr->DataId );
      /* attempt to report error to the originating task */
      iCctSetReply( &MsgIn, Status );
      return Status;
   }

   /* if got this far, then return SYS_NOMINAL */
   return SYS_NOMINAL;

}


/*******************************************************************************
** Function Name:
**    iCctSetReply
**
** Purpose:
**    Reply to a 'set' or 'get' command.
**
** Description:
**    This function replies to a 'set' or 'get' command, as both commands have
**    the same reply. The message class is assigned dependent on the supplied
**    status code, and the default reply parameters written into the reply 
**    message. The data is then written in, together with the last modification
**    time of the datum. Finally the source and destination of the message is 
**    swapped and the reply returned.
**
** Return Type:
**    void
**
** Arguments:
**    eCilMsg_t *MsgPtr                (in/out)
**       A pointer to a CIL message sent to a CCT, that has been processed. The
**       contents of this structure will be changed.
**    Int32_t StatusCode               (in)
**       A status code (as defined in Cct.h), to be appended and sent with the 
**       reply. This may be SYS_NOMINAL, in which case the CIL message class is 
**       RSP, rather than ERR.
**
** Author(s):
**    Derek J. McKay (djm)
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

void     iCctSetReply( eCilMsg_t * MsgPtr, Int32_t StatusCode )
{
   Int32_t SwapAddr;                   /* temp variable for swapping */
   Int32_t TxCilId;                    /* CIL ID to transmit reply to */
   Status_t Status;                    /* return value from called functions */
   eTtlSet_t * SetPtr;                 /* pointer to set command structure */

   /* overlay the set command structure onto the data */
   SetPtr = ( eTtlSet_t * ) MsgPtr->DataPtr;

   /* change the message class */
   if( StatusCode == SYS_NOMINAL )
   {
      MsgPtr->Class = E_CIL_RSP_CLASS;
   }
   else
   {
      MsgPtr->Class = E_CIL_ERR_CLASS;
   }

   /* fill in the default details */
   SetPtr->Status = StatusCode;
   SetPtr->NumBytes = sizeof( Int32_t );
   SetPtr->ParamHigh = 0;
   SetPtr->ParamLow = 0;
   SetPtr->TimeStamp.t_sec = 0;
   SetPtr->TimeStamp.t_nsec = 0;
   SetPtr->Units = E_SDB_INVALID_UNITS;

   /* if the datum ID really exists, then fill in the values */
   if ( ( SetPtr->DataId >= D_MCP_FIRST_USER_DATUM ) 
         && ( SetPtr->DataId < D_CCT_DATAID_EOL ) )
   {
      SetPtr->ParamLow = iCctData.Param[ SetPtr->DataId ].Datum.Msrment.Value;
      SetPtr->Units    = iCctData.Param[ SetPtr->DataId ].Datum.Units;
      memcpy( &( SetPtr->TimeStamp ), 
              &iCctData.Param[ SetPtr->DataId ].ModificationTime,
              sizeof( eTtlTime_t ) );
   }

   /* confirm the message length */
   MsgPtr->DataLen = sizeof( *SetPtr );

   /* swap the message source and destination */
   SwapAddr = MsgPtr->SourceId;
   MsgPtr->SourceId = MsgPtr->DestId;
   MsgPtr->DestId = SwapAddr;

   /* if the command originated by the MCP, send back directly */
   if ( MsgPtr->DestId == I_CCT_CIL_CONTROL )
   {
      TxCilId = MsgPtr->DestId;
   }
   /* else send back via the MCB */
   else
   {
      TxCilId = I_CCT_CIL_MCB;
   }

   /* send the message and return its status */
   Status = iCctSendCilMsg( TxCilId, MsgPtr );

}


/*
** EOF
*/
