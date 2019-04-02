/*******************************************************************************
** Module Name:
**    CmtUtil.c
**
** Purpose:
**    Utility functions used by the Computer Monitoring Task.
**
** Description:
**    This source file contains various utility functions used by the CMT. 
**    Functions are provided to manipulate state, output status to the console,
**    and submit data to the SDB.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    CMT design and implementation document v0.01 01-Nov-2000.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: CmtUtil.c,v 1.3 2004/01/13 12:40:34 mkb Exp $
**
** History:
**    $Log: CmtUtil.c,v $
**    Revision 1.3  2004/01/13 12:40:34  mkb
**    Return a FAILED status if serious communications fault occurs.
**
**    Revision 1.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 1.1  2000/11/14 17:31:32  asp
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

#include "Log.h"
#include "Tim.h"


/*
** Local include files
*/

#include "Cmt.h"
#include "CmtPrivate.h"


/*******************************************************************************
** Function Name:
**    iCmtOutputStatus
**
** Purpose:
**    Outputs the current CMT status to the console.
**
** Description:
**    Outputs a line to the screen detailing progress of the CMT, and the 
**    state and status of current parameters.
**
** Return Type:
**    Status_t    
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    Bool_t TimeFlag                  (in)
**       TRUE if the time is to be updated.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


void iCmtOutputStatus ( Bool_t TimeFlag )
{
   Status_t Status;                    /* Status of returned functions */
   ProcState_t CmtState;               /* Copy of CMT state */
   eTtlTime_t CurrentTime;             /* Current system time */
                                       /* ASCII representation of time */
   static char TimeBuffer[ E_TIM_BUFFER_LENGTH ] = { "\0" };
   static char * StateString[] = {     /* Strings for system states */
      "INVALID",
      "OKAY   ",
      "INIT   ",
      "STANDBY",
      "WARN   ",
      "FAILED ",
      "SAFE   ",
      "OFF    ",
      "TIMEOUT",
      "       "
   };
 

   /* Only output if verbose specified */
   if ( eCluCommon.Verbose == TRUE )
   {
      /* If time is to be updated */
      if ( TimeFlag == TRUE )
      {
         /* Get the current time */
         Status = eTimGetTime( &CurrentTime );

         /* Convert time into a string */
         Status = eTimToString( &CurrentTime, E_TIM_BUFFER_LENGTH, TimeBuffer );
      }

      /* Take copy of CMT state */
      CmtState = iCmtGetState();
 
      /* Print the output */ 
      fprintf( stdout, "\r%s %s State=%s Heartbeats=%d",
               eCluCommon.CilName, TimeBuffer, StateString[ CmtState ], 
               iCmtData.HeartbeatCount );
      fflush( stdout );
   }

}  /* End of iCmtOutputStatus() */



/*******************************************************************************
** Function Name:
**    iCmtSafeState
**
** Purpose:
**    This function takes the process to safe-state.
**
** Description:
**    This function performs the actions necessary to take the process to safe 
**    state, and then calls the SetState function accordingly.
**
** Return Type:
**    Status_t    
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/


Status_t iCmtSafeState ( void )
{
   /* Set the safe state */
   iCmtSetState( SYS_SAFE_STATE );

   return SYS_NOMINAL;

}  /* End of iCmtSafeState() */



/*******************************************************************************
** Function Name:
**    iCmtActivate
**
** Purpose:
**    This function re-activates the process after safe-state.
**
** Description:
**    This function performs the actions necessary to reactivate the process 
**    from safe-state, and then calls the RestoreState function accordingly.
**
** Return Type:
**    Status_t    
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtActivate ( void )
{
   /* Set the safe state */
   iCmtRestoreState();

   return SYS_NOMINAL;

}  /* End of iCmtActivate() */



/*******************************************************************************
** Function Name:
**    iCmtSetState
**
** Purpose:
**    Sets the supplied overall CMT state.
**
** Description:
**    Sets the overall CMT state using the parameter supplied. If the new state 
**    is not safe-state, then a copy of the state is taken for future 
**    restoration when the process is activated after safe-state.
**
** Return Type:
**    void
**
** Arguments:
**    ProcState_t CmtState             (in)
**       The new state for the CMT to assume.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

void iCmtSetState ( ProcState_t CmtState )
{

   /* If safe-state requested, only set the 'nominal' state */
   if ( CmtState == SYS_SAFE_STATE )
   {
      /* Assign new nominal state in the data area */
      iCmtData.NominalState = SYS_SAFE_STATE;
   }
   /* Else if a change in state is requested */
   else if( iCmtData.ActualState != CmtState )
   {
      /* Assign new state in the data area */
      iCmtData.ActualState = CmtState;
   }

}  /* End of iCmtSetState() */



/*******************************************************************************
** Function Name:
**    iCmtRestoreState
**
** Purpose:
**    Restores the state on ending of safe-state.
**
** Description:
**    When a process receives an activate message, the actual state is restored 
**    to replace safe-state. This ius achieved by having two states - the actual
**    state and nominal state. In this case the nominal state is set back to 
**    'nominal'.
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

void iCmtRestoreState ( void )
{

   /* Restore nominal state to be SYS_NOMINAL */
   iCmtData.NominalState = SYS_NOMINAL;

}  /* End of iCmtRestoreState() */



/*******************************************************************************
** Function Name:
**    iCmtGetState
**
** Purpose:
**    Determines and returns the resolved state of this process.
**
** Description:
**    The resolved state is determined using the actual and nominal states.
**
** Return Type:
**    ProcState_t
**       Returns the resolved state of this process. The resolved state may 
**       depend on a combination of several states.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

ProcState_t iCmtGetState ( void )
{
   ProcState_t ResolvedState;          /* Determined state for this process */

   /* If the 'nominal state' is 'nominal', return the 'actual state' */
   if ( iCmtData.NominalState == SYS_NOMINAL )
   {
      ResolvedState = iCmtData.ActualState;
   }
   /* Else we are in safe-state, return this */
   else            
   {
      ResolvedState = SYS_SAFE_STATE;
   }

   return ResolvedState;

}  /* End of iCmtGetState() */



/*******************************************************************************
** Function Name:
**    iCmtSendCilMsg
**
** Purpose:
**    Sends a CIL message to a software process.
**
** Description:
**    AccCMTs a pointer to a CIL message structure, and sends a CIL message to 
**    the appropriate software process. Also passed in the CIL ID, which is 
**    expected to be valid at this stage. Error reporting is done within this 
**    function.
**
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Arguments:
**    Int32_t CilId                    (in)
**       CIL ID of process to send message to.
**    eCilMsg * CilMsg                 (in/out)
**       Pointer to structure of parameters for CIL message transmission.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtSendCilMsg ( Int32_t CilId, eCilMsg_t * CilMsg )
{
   Status_t Status;                    /* Completion status of this function */


   /* Initially status is nominal */
   Status = SYS_NOMINAL;

   /* Correctly re-order the bytes in the packet body as required */
   if ( CilMsg->DataLen > 0 )
   {
      Status = eCilConvert32bitArray( CilMsg->DataLen, CilMsg->DataPtr );
   }

   /* Send the message */
   Status = eCilSend( CilId, CilMsg );

   /* Check status and report any errors */
   if ( Status == E_CIL_NO_MEMORY )
   {
      eLogReport( E_LOG_ERR, Status,
                  "Unable to assign memory for CIL message 0x%x to CIL ID 0x%x",
                  CilMsg->Service, CilMsg->DestId );
   }
   else if ( Status != SYS_NOMINAL )
   {
      eLogReport( E_LOG_ERR, Status,
                  "Error sending CIL message 0x%x to CIL ID 0x%x",
                  CilMsg->Service, CilMsg->DestId );
   }

   /* For memory errors and IP send errors shutdown the CMT. */
   if ( (Status == E_CIL_NO_MEMORY) || (Status == E_CIL_SEND_ERR) )
   {

      Status = SYS_FAILED_STATE;

   }


   /* Return status of attempt to transmit */
   return Status;

}  /* End of iCmtSendCilMsg() */



/*******************************************************************************
** Function Name:
**    iCmtSubmitToSdb
**
** Purpose:
**    Function to make a submission into the Status Database.
**    **** NOT USED *****
**
** Description:
**    This function initially determines the overall data length of the 
**    required CIL message, and assigns this and the other relevant paramters 
**    into the CIL message sructure. The message is re-byte-ordered, and then 
**    a call made to send the CIL message to the SDB.
**
** Return Type:
**    Status_t
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    iCmtSdbData_t * SdbData          (in)
**       Pointer to structure containing the parameters to be submitted to  
**       the SDB.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtSubmitToSdb ( iCmtSdbData_t * SdbData )
{
   int      i;                         /* Loop counter for submissions */
   Status_t Status;                    /* Completion status of this function */
   size_t   DataLen;                   /* Length of submission message */


   /* Initialise status and data-length */
   DataLen = sizeof( SdbData->NumElts );

   /* Loop around the elements to be submitted */
   for ( i = 0; i < SdbData->NumElts; i++ )
   {
      /* Add the size of this element onto the message length */
      DataLen += sizeof( eSdbDatum_t );

      /* Write the submission time in */
      Status = eTimGetTime( &( SdbData->Datum[ i ].Msrment.TimeStamp ) );
   }

   /* Set CIL parameters for transmission of SDB submission */
   iCmtData.SdbSubmissionMsg.DataPtr = SdbData;
   iCmtData.SdbSubmissionMsg.DataLen = DataLen;

   /* Transmit the SDB submission message */
   Status = iCmtSendCilMsg( iCmtData.SdbSubmissionMsg.DestId, 
                            &iCmtData.SdbSubmissionMsg );

   return Status;

}  /* End of iCmtSubmitToSdb() */


/*******************************************************************************
** Function Name:
**    iCmtProcessHeartbeat
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
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    eTtlTime_t *TimeReceived         (in)
**       Pointer to structure
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtProcessHeartbeat( eTtlTime_t *TimeReceived )
{
   Status_t Status;                    /* Status returned by functions */
   Status_t TimStatus;                 /* Status returned by time functions */
   ProcState_t CurrentState;           /* Current process state for return */

   /* Respond to heartbeat with our state */
   CurrentState = iCmtGetState();

   /* Point to return packet and assign sequence number */
   iCmtData.ChbResponseMsg.DataPtr = &CurrentState;
   iCmtData.ChbResponseMsg.SeqNum = iCmtData.RxMsg.SeqNum;

   /* Respond to message */
   Status = iCmtSendCilMsg( iCmtData.ChbResponseMsg.DestId, 
                            &iCmtData.ChbResponseMsg );

   /* Increment number of heartbeats received */
   iCmtData.HeartbeatCount++;

   /* Take copy of time to be used as last heartbeat time */
   TimStatus = eTimCopy( *TimeReceived, &iCmtData.LastHeartbeat );

   /* Time at which heartbeats will be considered lost */
   TimStatus = eTimSum( &iCmtData.LastHeartbeat, 
                        &iCmtData.HeartbeatTimeOut,
                        &iCmtData.HeartbeatLossTime );

   /* Update on-screen verbose output */
   iCmtOutputStatus( TRUE );

   /* Return status of CIL transaction */
   return Status;

}  /* End of iCmtProcessHeartbeat() */


/*******************************************************************************
** Function Name:
**    iCmtProcessShutdown
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
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtProcessShutdown( void )
{
   Status_t Status;                    /* Status returned by functions */

   /* Send acknowledgement to the command */ 
   iCmtData.McpReplyMsg.Class    = E_CIL_ACK_CLASS;
   iCmtData.McpReplyMsg.Service  = iCmtData.RxMsg.Service;
   iCmtData.McpReplyMsg.DataPtr  = NULL;
   iCmtData.McpReplyMsg.DataLen  = E_MCP_SHUTDOWN_ACK_LEN;
   iCmtData.McpReplyMsg.SeqNum   = iCmtData.RxMsg.SeqNum;

   /* Send the message */
   Status = iCmtSendCilMsg( iCmtData.McpReplyMsg.DestId, 
                            &iCmtData.McpReplyMsg );

   /* Call the shutdown function */
   iCmtShutdown();

   /* Return status of CIL transaction */
   return Status;

}  /* End of iCmtProcessShutdown() */



/*******************************************************************************
** Function Name:
**    iCmtProcessSafeState
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
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtProcessSafeState ( void )
{
   Status_t Status;                    /* Status returned by functions */
   Status_t CilStatus;                 /* Status returned by CIL functions */
   ProcState_t CurrentState;           /* Current process state for return */

   /* Send 'action in progress' message back */ 
   iCmtData.McpReplyMsg.Class    = E_CIL_ACT_CLASS;
   iCmtData.McpReplyMsg.Service  = iCmtData.RxMsg.Service;
   iCmtData.McpReplyMsg.DataPtr  = NULL;
   iCmtData.McpReplyMsg.DataLen  = E_MCP_SAFESTATE_ACT_LEN;
   iCmtData.McpReplyMsg.SeqNum   = iCmtData.RxMsg.SeqNum;

   /* Send the message */
   CilStatus = iCmtSendCilMsg( iCmtData.McpReplyMsg.DestId, 
                               &iCmtData.McpReplyMsg );

   /* Only required action is to set the state */
   Status = iCmtSafeState();
   CurrentState = iCmtGetState();

   /* Send 'command completed' message back */
   iCmtData.McpReplyMsg.Class    = E_CIL_COM_CLASS;
   iCmtData.McpReplyMsg.Service  = iCmtData.RxMsg.Service;
   iCmtData.McpReplyMsg.DataPtr  = &CurrentState;
   iCmtData.McpReplyMsg.DataLen  = E_MCP_SAFESTATE_COM_LEN;
   iCmtData.McpReplyMsg.SeqNum   = iCmtData.RxMsg.SeqNum;

   /* If last transmission was OK, then send this one */
   if ( CilStatus == SYS_NOMINAL )
   {
      CilStatus = iCmtSendCilMsg( iCmtData.McpReplyMsg.DestId, 
                                  &iCmtData.McpReplyMsg );
   }

   /* Return status of CIL transactions */
   return CilStatus;

}  /* End of iCmtProcessSafeState() */



/*******************************************************************************
** Function Name:
**    iCmtProcessActivate
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
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtProcessActivate ( void )
{
   Status_t Status;                    /* Status returned by functions */
   Status_t CilStatus;                 /* Status returned by CIL functions */
   ProcState_t CurrentState;           /* Current process state for return */

   /* Send 'action in progress' message back */ 
   iCmtData.McpReplyMsg.Class    = E_CIL_ACT_CLASS;
   iCmtData.McpReplyMsg.Service  = iCmtData.RxMsg.Service;
   iCmtData.McpReplyMsg.DataPtr  = NULL;
   iCmtData.McpReplyMsg.DataLen  = E_MCP_ACTIVATE_ACT_LEN;
   iCmtData.McpReplyMsg.SeqNum   = iCmtData.RxMsg.SeqNum;

   /* Send the message */
   CilStatus = iCmtSendCilMsg( iCmtData.McpReplyMsg.DestId, 
                               &iCmtData.McpReplyMsg );

   /* Re-activate the process */    
   Status = iCmtActivate();
   CurrentState = iCmtGetState();

   /* Send 'command completed' message back */
   iCmtData.McpReplyMsg.Class    = E_CIL_COM_CLASS;
   iCmtData.McpReplyMsg.Service  = iCmtData.RxMsg.Service;
   iCmtData.McpReplyMsg.DataPtr  = &CurrentState;
   iCmtData.McpReplyMsg.DataLen  = E_MCP_ACTIVATE_COM_LEN;
   iCmtData.McpReplyMsg.SeqNum   = iCmtData.RxMsg.SeqNum;

   /* If last transmission was OK, then send this one */
   if ( CilStatus == SYS_NOMINAL )
   {
      CilStatus = iCmtSendCilMsg( iCmtData.McpReplyMsg.DestId, 
                                  &iCmtData.McpReplyMsg );
   }

   /* Return status of CIL transactions */
   return CilStatus;

}  /* End of iCmtProcessActivate() */



/*******************************************************************************
** Function Name:
**    iCmtProcessCommand
**
** Purpose:
**    Function to respond to an incoming command.
**
** Description:
**    This function responds to an incoming command. It performs the required 
**    processing and sends the required responses to the controlling process.
**
** Return Type:
**    Status_t
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtProcessCommand ( void )
{
   Status_t Status;                    /* Status returned by functions */
   Int32_t  AttainedStatus;            /* Required status when attained */

   /* Send 'action in progress' message back */ 
   iCmtData.CmdReplyMsg.Class    = E_CIL_ACT_CLASS;
   iCmtData.CmdReplyMsg.Service  = iCmtData.RxMsg.Service;
   iCmtData.CmdReplyMsg.DestId   = iCmtData.RxMsg.SourceId;
   iCmtData.CmdReplyMsg.DataPtr  = E_CMT_ACT_LEN_CMD;
   iCmtData.CmdReplyMsg.DataLen  = 0;
   iCmtData.CmdReplyMsg.SeqNum   = iCmtData.RxMsg.SeqNum;

   /* Send the message */
   Status = iCmtSendCilMsg( iCmtData.CmdReplyMsg.DestId, 
                            &iCmtData.CmdReplyMsg );

   /* Call function to perform the requested command */
   switch ( iCmtData.RxMsg.Service )
   {
      default :
         break;

   }  /* End of switch() */

   /* Send 'command completed' message back */
   iCmtData.CmdReplyMsg.Class    = E_CIL_COM_CLASS;
   iCmtData.CmdReplyMsg.Service  = iCmtData.RxMsg.Service;
   iCmtData.CmdReplyMsg.DestId   = iCmtData.RxMsg.SourceId;
   iCmtData.CmdReplyMsg.DataPtr  = &AttainedStatus;
   iCmtData.CmdReplyMsg.DataLen  = E_CMT_COM_LEN_CMD;
   iCmtData.CmdReplyMsg.SeqNum   = iCmtData.RxMsg.SeqNum;

   /* If last transmission was OK, then send this one */
   if ( Status == SYS_NOMINAL )
   {
      Status = iCmtSendCilMsg( iCmtData.CmdReplyMsg.DestId, 
                               &iCmtData.CmdReplyMsg );
   }

   /* Return status of CIL transactions */
   return Status;

} /* End of iCmtProcessCommand() */


/*******************************************************************************
** End of File Name: CmtUtil.c
*******************************************************************************/
