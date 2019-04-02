/*******************************************************************************
** Module Name:
**    AgsUtil.c
**
** Purpose:
**    Utility functions used by the Autoguider Service Task (AGS)..
**
** Description:
**    This source file contains various utility functions used by the AGS. 
**    Functions are provided to manipulate state, output status to the console,
**    and submit data to the SDB.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**
** Author(s):
**    Stephen Foale (sxf).
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: AgsUtil.c,v 0.12 2006/10/03 15:37:43 sxf Exp $
**
** History:
**    $Log: AgsUtil.c,v $
**    Revision 0.12  2006/10/03 15:37:43  sxf
**    Baseline AGS_1_04.
**
**    Revision 0.10  2005/10/04 12:40:53  sxf
**    Brought in line with QNX4 baseline AGS_0_17.
**
**    Revision 0.9  2002/04/10 14:11:00  sxf
**    Check if datum is to be submitted always or only if changed.
**
**    Revision 0.8  2002/01/24 07:52:20  sxf
**    Tidy up debug level messages.
**
**    Revision 0.7  2002/01/23 14:13:09  sxf
**    Allocate memory for Agg command in iAgsAddCmdToQueueStart.
**
**    Revision 0.6  2002/01/21 09:21:14  sxf
**    Added iAgsAddCmdToQueueStart function/
**
**    Revision 0.5  2001/11/09 10:54:34  sxf
**    Minor changes to tidy up code
**
**    Revision 0.4  2001/10/17 15:39:06  sxf
**    First version with basic functionality.
**
**    Revision 0.3  2001/09/25 14:45:49  sxf
**    Initial version with basic functionality.
**
**    Revision 0.2  2001/09/21 14:00:27  sxf
**    Further development.
**
**    Revision 0.1  2001/09/18 14:14:05  sxf
**    Initial version.
**    ,.
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

#include "Log.h"
#include "Tim.h"


/*
** Local include files
*/

#include "Agg.h"
#include "Ags.h"
#include "AgsPrivate.h"


/*******************************************************************************
** Function Name:
**    iAgsAddCmdToQueue
**
** Purpose:
**    Add command to end of command queue.
**
** Description:
**    Add the command pointed to by the first argument with the TCS and
**    AGG command bodies given by the second and third arguments.
**
** Return Type:
**    Status_t    
**       Returns the completion status of queuing the command.
**
** Arguments:
**    eCilMsg_t Cmd    (in)
**       Command to be added to the queue.
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t iAgsAddCmdToQueue( iAgsCmd_t     *Cmd,
                            eAgsCmdBody_t *TcsCmdBodyPtr,
                            eAggCmdBody_t *AggCmdBodyPtr )
{

   
   Status_t   Status;       /* Return status. */
   int        CmdCount = 0; /* Count of number of commands on queue. */
   iAgsCmd_t *CmdPtr;       /* Pointer for navigating along the queue. */
   iAgsCmd_t *NextCmdPtr;   /* Pointer to next command on queue. */

   /* Initialise return status. */

   Status = SYS_NOMINAL;

   /*
   ** Start at the top of the queue. 
   */

   NextCmdPtr = iAgsCmdQueuePtr;


   /* 
   ** Work along the queue looking for a NULL "next command" pointer.
   */

   do
   {
      CmdPtr     = NextCmdPtr;

      if ( CmdPtr != (iAgsCmd_t *)(NULL) )
      {
         CmdCount++;
         NextCmdPtr = CmdPtr -> NextCmdPtr;
      }

   } while ( ( CmdCount < I_AGS_LEN_CMDQ ) &&
             ( NextCmdPtr != (iAgsCmd_t *)(NULL) ) );
  
   /*
   ** Check if the queue is already of maximum length.
   */

   if ( CmdCount >= I_AGS_LEN_CMDQ )
   {
      Status = E_AGS_CMDQ_FULL; /* Command queue already full. */
   }
   else /* Command queue not full. */
   {

      /* 
      ** Allocate memory for a new command structure. 
      */

      NextCmdPtr = (iAgsCmd_t *)TTL_MALLOC( sizeof( iAgsCmd_t ) );

      /*
      ** If memory successfully allocated then copy new command into it,
      ** set up pointer to new command and ensure that new command points
      ** to NULL next command. 
      */

      if (NextCmdPtr != (iAgsCmd_t *)(NULL) )
      {
         /*
         ** Copy contents of command into newly allocated memory and
         ** set "next command" pointer of current command. Also allocate 
         ** memory for TCS and AGG commands and copy these into structure.
         */
 
         memcpy( NextCmdPtr, Cmd, sizeof( iAgsCmd_t ) );

         NextCmdPtr -> TcsCmd.DataPtr =
            ( eAgsCmdBody_t *)TTL_MALLOC( sizeof( eAgsCmdBody_t ) );

         if ( NextCmdPtr -> TcsCmd.DataPtr != (eAgsCmdBody_t *)(NULL) )
         {
           memcpy( NextCmdPtr -> TcsCmd.DataPtr, TcsCmdBodyPtr, 
                   sizeof( eAgsCmdBody_t ) );
         }
         else
         {
            Status = E_AGS_CMDQ_ERR;
            eLogErr( Status, 
                     "Unable to allocate memory for Tcs command (body)." );
         }

         NextCmdPtr -> AggCmd.DataPtr = 
            ( eAggCmdBody_t *)TTL_MALLOC( sizeof( eAggCmdBody_t) );

         if ( NextCmdPtr -> AggCmd.DataPtr != (eAggCmdBody_t *)(NULL) )
         { 
            memcpy( NextCmdPtr -> AggCmd.DataPtr, AggCmdBodyPtr,
                    sizeof( eAggCmdBody_t ) );
         }
         else
         {
            Status = E_AGS_CMDQ_ERR;
            eLogErr( Status, 
                     "Unable to allocate memory for Agg command (body)." );
         }     

         /* 
         ** If not the first command in the list then point last command
         ** to next command in list.
         */
 
         if ( CmdCount != 0 )
         { 
            CmdPtr -> NextCmdPtr = NextCmdPtr;
         }

         /*
         ** Set the next command pointer and terminate the linked
         ** list with a NULL pointer.
         */
         
         CmdPtr = NextCmdPtr;
         CmdPtr -> NextCmdPtr = (iAgsCmd_t *)(NULL);

         /*
         ** If this is the first command then set start of list pointer
         ** and send the command to the AGG.
         */

         if ( CmdCount == 0 )
         {
            iAgsCmdQueuePtr = CmdPtr;

            Status = iAgsSendTopCmd();

         }
      }
      else
      {
         Status = E_AGS_CMDQ_ERR;  /* Error allocating memory for command. */
      }
   }

   return Status;

}

/*******************************************************************************
** Function Name:
**    iAgsAddCmdToQueueStart
**
** Purpose:
**    Add command to start of command queue.
**
** Description:
**    Add the command pointed to by the first argument with the TCS and
**    AGG command bodies given by the second and third arguments.
**
** Return Type:
**    Status_t    
**       Returns the completion status of queuing the command.
**
** Arguments:
**    eCilMsg_t Cmd    (in)
**       Command to be added to the queue.
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t iAgsAddCmdToQueueStart( iAgsCmd_t     *Cmd,
                                 eAgsCmdBody_t *TcsCmdBodyPtr,
                                 eAggCmdBody_t *AggCmdBodyPtr )
{

   
   Status_t   Status;       /* Return status. */
   iAgsCmd_t *NextCmdPtr;   /* Pointer to next command on queue. */
   iAgsCmd_t *NewCmdPtr;    /* Pointer to new command on queue. */

   /* Initialise return status. */

   Status = SYS_NOMINAL;

   eLogDebug( "Adding command to queue start." );

   /*
   ** Start at the top of the queue. 
   */

   NextCmdPtr = iAgsCmdQueuePtr;

   NewCmdPtr  = (iAgsCmd_t *)TTL_MALLOC( sizeof( iAgsCmd_t ) );

   if ( NewCmdPtr != ( iAgsCmd_t *)(NULL) )
   {
     memcpy( NewCmdPtr, Cmd, sizeof( iAgsCmd_t ) );

     NewCmdPtr -> TcsCmd.DataPtr =
           ( eAgsCmdBody_t *)TTL_MALLOC( sizeof( eAgsCmdBody_t ) );

     if ( NewCmdPtr -> TcsCmd.DataPtr != (eAgsCmdBody_t *)(NULL) )
     {
       memcpy( NewCmdPtr -> TcsCmd.DataPtr, TcsCmdBodyPtr, 
               sizeof( eAgsCmdBody_t ) );
     }
     else
     {
       Status = E_AGS_CMDQ_ERR;
       eLogErr( Status, 
                 "Unable to allocate memory for Tcs command (body)." );
     }

     NewCmdPtr -> AggCmd.DataPtr =
           ( eAggCmdBody_t *)TTL_MALLOC( sizeof( eAggCmdBody_t ) );

     if ( NewCmdPtr -> AggCmd.DataPtr != (eAggCmdBody_t *)(NULL) )
     {
       memcpy( NewCmdPtr -> AggCmd.DataPtr, AggCmdBodyPtr, 
               sizeof( eAggCmdBody_t ) );
     }
     else
     {
       Status = E_AGS_CMDQ_ERR;
       eLogErr( Status, 
                 "Unable to allocate memory for Agg command (body)." );
     }
   }

   NewCmdPtr -> NextCmdPtr = NextCmdPtr;

   iAgsCmdQueuePtr = NewCmdPtr;

   Status = iAgsSendTopCmd();


   return Status;

}

/*******************************************************************************
** Function Name:
**    iAgsDelCmdFromQueue
**
** Purpose:
**    Delete command from top of command queue.
**
** Description:
**    Delete the command at the top of the command queue and make the next
**    command the new top element.
**
** Return Type:
**    Status_t    
**       Returns the completion status of the command deletion.
**
** Arguments:
**    None.
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Status_t iAgsDelCmdFromQueue( void )
{

   iAgsCmd_t *NextCmdPtr; 
   Status_t   Status;

   /*
   ** Check if the pointer to the top of the command queue is null 
   ** (ie the queue is already empty).
   */

   if (iAgsCmdQueuePtr != ( iAgsCmd_t * )(NULL) )
   {
      /* Queue not empty, so store pointer to next queue element. */

      NextCmdPtr = iAgsCmdQueuePtr -> NextCmdPtr;

      /* Free up memory allocated for the top command element. */
      
      TTL_FREE( iAgsCmdQueuePtr -> TcsCmd.DataPtr ); 
      TTL_FREE( iAgsCmdQueuePtr -> AggCmd.DataPtr );
      TTL_FREE( iAgsCmdQueuePtr );

      /* Define the new top of queue as the next queue element. */

      iAgsCmdQueuePtr = NextCmdPtr;

      Status = SYS_NOMINAL;         /* successful completion. */
   }
   else
   {
      Status = E_AGS_CMDQ_EMPTY;    /* Command queue is already empty. */
   }

   return Status;      
}

/*******************************************************************************
** Function Name:
**    iAgsSearchCmdQueue
**
** Purpose:
**    Search for a given AGG command in command queue.
**
** Description:
**    
**
** Return Type:
**    Bool_t    
**       Returns whether the command is found in the list (TRUE or FALSE).
**
** Arguments:
**    int Command (in)
**       Command to be searched for in the command queue.
**
** Author(s):
**    Stephen Foale (sxf)
**
*******************************************************************************/

Bool_t iAgsSearchCmdQueue( int Command )
{

   
   int            CmdCount = 0; /* Count of number of commands on queue. */
   iAgsCmd_t     *CmdPtr;       /* Pointer for navigating along the queue. */ 
   iAgsCmd_t     *NextCmdPtr;   /* Pointer to next command in the list. */
   eAggCmdBody_t  AggCmd;       /* Autoguider command. */
   Bool_t         CmdFound = FALSE;

   /*
   ** Start at the top of the queue. 
   */

   NextCmdPtr = iAgsCmdQueuePtr;

   /* 
   ** Work along the queue looking for the "Command" (passed in) 
   ** or a NULL "next command" pointer.
   */

   do
   {
      CmdPtr     = NextCmdPtr;

      if ( CmdPtr != (iAgsCmd_t *)(NULL) )
      {
         CmdCount++;
         NextCmdPtr = CmdPtr -> NextCmdPtr;

         memcpy( &AggCmd, 
                  CmdPtr -> AggCmd.DataPtr,
                  sizeof( eAggCmdBody_t ) );

          if ( AggCmd.Command == Command )
         {
            CmdFound = TRUE;
         }
         else
         {
            CmdFound = FALSE;
         }
      }

   } while ( ( CmdCount    < I_AGS_LEN_CMDQ      ) &&
             ( NextCmdPtr != (iAgsCmd_t *)(NULL) ) &&
             ( CmdFound   == FALSE               ) ); 

   return CmdFound;

}

/*******************************************************************************
** Function Name:
**    iAgsOutputStatus
**
** Purpose:
**    Outputs the current AGS status to the console.
**
** Description:
**    Outputs a line to the screen detailing progress of the AGS, and the 
**    state and status of current parameters.
**
** Return Type:
**    Status_t    
**       Returns the completion status of AGS initialisation.
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


void iAgsOutputStatus ( Bool_t TimeFlag )
{
   Status_t    Status;                 /* Status of returned functions */
   ProcState_t ReportState;            /* Reported state of AGS */
   eTtlTime_t  CurrentTime;            /* Current system time */
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

      /* Take copy of AGS state */
      ReportState = iAgsGetState();
 
      /* Print the output */ 
      fprintf( stdout, "\r%s %s Rep. State=%s Act. State=%s H.beats=%d",
               eCluCommon.CilName, TimeBuffer, 
               StateString[ ReportState ], StateString[ iAgsData.ActualState ], 
               iAgsData.HeartbeatCount );
      fflush( stdout );
   }

}  /* End of iAgsOutputStatus() */

/*******************************************************************************
** Function Name:
**    iAgsRequestStateChange
**
** Purpose:
**    Request a change in the overall AGS state.
**
** Description:
**
**
** Return Type:
**    void
**
** Arguments:
**    ProcState_t AgsState             (in)
**       The requested state for the AGS to assume.
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsRequestStateChange( void )
{

   Status_t  Status;          /* Return status. */

   switch ( iAgsData.RxMsg.Service )
   {
      case E_MCP_SHUTDOWN:

         Status = iAgsProcessShutdown();
         break;

      case E_MCP_ACTIVATE:

         Status = iAgsProcessActivate();
         break;


      case E_MCP_SAFESTATE:

         Status = iAgsProcessSafeState();   
         break;

      default:
 
         Status = E_AGS_INV_STATE;

         iAgsErrReply( &iAgsData.RxMsg, Status );                    

         eLogErr( Status,
                  "Unexpected CIL msg rx'd, src = 0x%x, delivered by 0x%x "
                  "class = 0x%x, service = 0x%x",
                  iAgsData.RxMsg.SourceId, 
                  iAgsData.RxMsg.DestId,
                  iAgsData.RxMsg.Class,
                  iAgsData.RxMsg.Service );
         break;

   }

   return Status;
}


/*******************************************************************************
** Function Name:
**    iAgsSetState
**
** Purpose:
**    Sets the supplied overall AGS state.
**
** Description:
**    Sets the overall AGS state using the parameter supplied. If the new state 
**    is not safe-state, then a copy of the state is taken for future 
**    restoration when the process is activated after safe-state.
**    This function determines the process state model of the Ags by
**    using the priorities that have been assigned to the process states.
**
** Return Type:
**    void
**
** Arguments:
**    ProcState_t AgsState             (in)
**       The new state for the AGS to assume.
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

void iAgsSetState( ProcState_t AgsState )
{
   /* Local variables. */
   Int32_t Priority;    /* For loop index to loop through procstate list. */
   Int32_t CurrentPriority; /* Current proority of the actual state. */
   Int32_t NewPriority;     /* Priority of state to be set. */

   /* If safe-state requested, set the demand state. */
   if ( AgsState == SYS_SAFE_STATE )
   {
      /* Assign new nominal state in the data area */
      iAgsData.DemandState = SYS_SAFE_STATE;
      return;
   }
  
   /* 
   ** If not safe state request then we need to set the actual state.
   ** Set the actual state, only if the new state is of higher
   ** priority than the current state. For example we wouldn't want
   ** to over write a Failed with a Warn.
   */

   for (Priority = 0; Priority < E_AGS_NUM_POS_STATES; Priority++)
   {
      if( eAgsStatePriority[ Priority ] == iAgsData.ActualState )
      {
          CurrentPriority = Priority;
      }
      if( eAgsStatePriority[ Priority ] == AgsState )
      {
          NewPriority = Priority;
      }
   }

   if ( NewPriority > CurrentPriority )
   {
      /* Assign new state in the data area */
      iAgsData.ActualState = AgsState;
   }

   return;

}  /* End of iAgsSetState() */



/*******************************************************************************
** Function Name:
**    iAgsRestoreState
**
** Purpose:
**    Restores the state on ending of safe-state.
**
** Description:
**    When a process receives an activate message, the actual state is restored 
**    to replace safe-state. This is achieved by having two states - the actual
**    state and a reported state. In this case the report state is set back to 
**    actual.
**
** Return Type:
**    void
**
** Arguments:
**    none
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

void iAgsRestoreState ( void )
{
   /* Restore Request state to OKAY now that we are out of SAFE State. */
   iAgsData.DemandState = SYS_OKAY_STATE;

}  /* End of iAgsRestoreState() */


/*******************************************************************************
** Function Name:
**    iAgsRecoverState
**
** Purpose:
**    Recovers the process state from a fault state back to OKAY.
**
** Description:
**    In the case that a fault (WARN, STANDBY, FAILED) fixes itself 
**    without the Ags having to be restarted, we use this function to
**    recover the process state back to OKAY. 
**
** Return Type:
**    void
**
** Arguments:
**    none
**
** Author(s):
**    Lorna J. Slater (ljs)
**
*******************************************************************************/

void iAgsRecoverState ( void )
{
   /* Recover Actual state to OKAY now that fault is fixed. */
   iAgsData.ActualState = SYS_OKAY_STATE;

   return;

}  /* End of iAgsRecoverState() */

/*******************************************************************************
** Function Name:
**    iAgsGetState
**
** Purpose:
**    Determines and returns the process state.
**
** Description:
**    The process state is determined using the Reported and Actual states.
**    This function determines the process state model of the Ags by
**    using the priorities that have been assigned to the process states.
**
** Return Type:
**    ProcState_t
**       Returns the process state of this process.
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

ProcState_t iAgsGetState ( void )
{
   /* Local variables. */
   Int32_t Priority;      /* For loop index to loop through procstate list. */
   Int32_t RequestPriority; /* Priority of current request state. */
   Int32_t ActualPriority;  /* Priority of current actual state. */ 

   /* 
   ** The process state will be whichever state has the higher priority
   ** the request state or the actual state. 
   */ 
      
   for (Priority = 0; Priority < E_AGS_NUM_POS_STATES; Priority++)
   {
      if( eAgsStatePriority[ Priority ] == iAgsData.ActualState )
      {
          ActualPriority = Priority;
      }
      if( eAgsStatePriority[ Priority ] == iAgsData.DemandState )
      {
          RequestPriority = Priority;
      }
   }

   if ( ActualPriority > RequestPriority )
   {
      return(iAgsData.ActualState);
   }
   else
   {
      return(iAgsData.DemandState);
   }

}  /* End of iAgsGetState() */



/*******************************************************************************
** Function Name:
**    iAgsSendCilMsg
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
** Return Type:
**    Status_t
**       Returns the completion status of this function.
**
** Arguments:
**    Int32_t CilId                    (in)
**       CIL ID of process to send message to.
**    eCilMsg *CilMsgPtr               (in/out)
**       Pointer to structure of parameters for CIL message transmission.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsSendCilMsg ( Int32_t CilId, eCilMsg_t *CilMsgPtr )
{
   Status_t Status;                    /* Completion status of this function */


   /* Initially status is nominal */
   Status = SYS_NOMINAL;


   /* 
   ** Correctly re-order the bytes in the packet body as required 
   ** - note that for outgoing messages to the AGG the byte ordering
   ** has already been corrected.
   */ 

   if ( ( CilMsgPtr->DataLen > 0 ) && ( CilId != iAgsData.AggCilId ) )
   {
      Status = eCilConvert32bitArray( CilMsgPtr->DataLen, CilMsgPtr->DataPtr );
   }
 
   /* Send the message */
 
  Status = eCilSend( CilId, CilMsgPtr );

   /* Check status and report any errors */
   if ( Status != SYS_NOMINAL )
   {
      eLogReport( E_LOG_ERR, Status,
                  "Error sending CIL message 0x%x to CIL ID 0x%x",
                  CilMsgPtr->Service, CilMsgPtr->DestId );
   }

   /* Return status of attempt to transmit */
   return Status;

}  /* End of iAgsSendCilMsg() */



/*******************************************************************************
** Function Name:
**    iAgsCheckAndSet
**
** Purpose:
**    To set the Oid in the OidTable and the Changed flag if appropriate.
**
** Description:
**    This function sets the Oid to the new value and sets the Changed
**    flag if the value has changed. Finally, it returns the previous
**    value of this Oid. 
**
** Return Type:
**    Int32_t
**       Returns the Previous value of the DatumId.
**
** Arguments:
**    eAgsDataId_t DatumId (in)
**       The Oid that is to be checked and set.
**
**    Int32_t SetValue  (in)
**       The new value to be set for the DatumId.
**
** Author(s):
**    Lorna J. Slater (ljs)
**
*******************************************************************************/

Int32_t iAgsCheckAndSet( eAgsDataId_t DatumId, Int32_t SetValue )
{
   Int32_t PreviousValue;
   eTtlTime_t TimeNow;

   eTimGetTime( &TimeNow );

   /* Double check that the DatumId is in range (should be done already). */
   if ( (DatumId > D_AGS_DATAID_BOL) && (DatumId < D_AGS_DATAID_EOL) )
   {

      PreviousValue = iAgsOidTable[ DatumId ].Value;

      /* If the value is being changed set the changed flag. */
      if ( iAgsOidTable[ DatumId ].Value != SetValue) 
      {
         iAgsOidTable[ DatumId ].Changed = TRUE;
      }
      /* Check if datum is to be submitted every time, not just when changed. */
      else if ( ( DatumId == D_AGS_GUIDEMAG ) ||
                ( DatumId == D_AGS_CENTROIDX ) ||
                ( DatumId == D_AGS_CENTROIDY ) ||
                ( DatumId == D_AGS_FWHM ) ||
                ( DatumId == D_AGS_INTTIME ) )
      {
         iAgsOidTable[ DatumId ].Changed = TRUE;
      }

      iAgsOidTable[ DatumId ].Value     = SetValue;
      iAgsOidTable[ DatumId ].TimeStamp = TimeNow;

      return( SYS_NOMINAL ); 
   }
   else
   {
      /* Return an error. */
      return(E_AGS_UNKNOWN_OID);
   }

} /* End of iAgsCheckAndSet */


/*******************************************************************************
** End of File Name: AgsUtil.c
*******************************************************************************/
