/*******************************************************************************
** Module Name:
**    AgsReply.c
**
** Purpose:
**    Functions used by the AGS to return messages to client applications.
**
** Description:
**    This source file contains functions used by the Autoguider Service Task
**    to return CIL messages to requesting applications. These functions
**    are not for sending back data, but are for information-type
**    messages like acknowlegdements or errors.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
**
** Traceability to Software Design:
**    Autoguider Control System Detailed Design Document v0.01.
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: AgsReply.c,v 0.3 2001/12/10 11:53:40 sxf Exp $
**
** History:
**    $Log: AgsReply.c,v $
**    Revision 0.3  2001/12/10 11:53:40  sxf
**    Added missing ;
**
**    Revision 0.2  2001/12/10 11:32:09  sxf
**    Fixed potential dereferencing problem in iAgsErrReply.
**
**    Revision 0.1  2001/09/18 14:12:42  sxf
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
#include "Cil.h"
#include "Log.h"
#include "Tim.h"

/*
** Local include files
*/

#include "Ags.h"
#include "AgsPrivate.h"

/*
** Local function prototypes
*/

static Status_t mAgsReply(Bool_t Swap, eCilMsg_t *MsgPtr);



/*******************************************************************************
** Function Name:
**    iAgsAckReply
**
** Purpose:
**    Reply to a CIL message with an acknowledgement.
**
** Description:
**    The ack class has no associated data buffer and is sent back to 
**    the source ofthe message.
**
** Return Type:
**    Status_t
**       Returns the function completion status.
**
** Arguments:
**    eCilMsg_t *MsgPtr      (in)
**       A pointer to a CIL message received by the AGS. We are responding
**       to this message.
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsAckReply( eCilMsg_t *MsgPtr )
{
   /* Local variables */
   Status_t Status;          /* Return value from called functions */


   /* Set the reply message fields. */
   iAgsData.ReplyMsg.SourceId = MsgPtr->SourceId;
   iAgsData.ReplyMsg.DestId   = MsgPtr->DestId;
   iAgsData.ReplyMsg.Class    = E_CIL_ACK_CLASS;
   iAgsData.ReplyMsg.Service  = MsgPtr->Service;
   iAgsData.ReplyMsg.DataPtr  = NULL;
   iAgsData.ReplyMsg.DataLen  = E_MCP_SHUTDOWN_ACK_LEN;
   iAgsData.ReplyMsg.SeqNum   = MsgPtr->SeqNum;

   /* Send the message */
   Status = mAgsReply(TRUE, &(iAgsData.ReplyMsg));

   if (Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"acknowledgement\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iAgsAckReply() */



/*******************************************************************************
** Function Name:
**    iAgsActReply
**
** Purpose:
**    Reply to a CIL message with an "in progress" class.
**
** Description:
**    The act class has no associated data buffer and is sent back to
**    the source of hte message.
**
** Return Type:
**    Status_t
**       Returns the function completion.
**
** Arguments:
**    eCilMsg_t *MsgPtr      (in)
**       A pointer to a CIL message received by the AGS. We are responding 
**       to this message.
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsActReply( eCilMsg_t *MsgPtr )
{
   /* Local variables */
   Status_t Status;          /* Return value from called functions */

   /* Set the reply message fields. */
   iAgsData.ReplyMsg.SourceId = MsgPtr->SourceId;
   iAgsData.ReplyMsg.DestId   = MsgPtr->DestId;
   iAgsData.ReplyMsg.Class    = E_CIL_ACT_CLASS;
   iAgsData.ReplyMsg.Service  = MsgPtr->Service;
   iAgsData.ReplyMsg.DataPtr  = NULL;
   iAgsData.ReplyMsg.DataLen  = E_MCP_SAFESTATE_ACT_LEN;
   iAgsData.ReplyMsg.SeqNum   = MsgPtr->SeqNum;

   /* Send the message */
   Status = mAgsReply(TRUE, &(iAgsData.ReplyMsg));
   if (Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"acting\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iAgsActReply() */


/*******************************************************************************
** Function Name:
**    iAgsComReply
**
** Purpose:
**    Reply to a CIL message with an completed class and procstate.
**
** Description:
**    This function sends a "completed" reply, and includes the current
**    AGS process state as the only item in the CIL data block.
**
** Return Type:
**    Status_t
**       Returns the function completion status.
**
** Arguments:
**    eCilMsg_t *MsgPtr      (in)
**       A pointer to a CIL message received by the AGS. We are responding 
**       to this message.
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsComReply( eCilMsg_t *MsgPtr )
{
   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   ProcState_t CurrentState;        /* Variable containing the state of the AGS */

   /* Set the reply message fields. */
   iAgsData.ReplyMsg.SourceId = MsgPtr->SourceId;
   iAgsData.ReplyMsg.DestId   = MsgPtr->DestId;
   iAgsData.ReplyMsg.Class    = E_CIL_COM_CLASS;
   iAgsData.ReplyMsg.Service  = MsgPtr->Service;
   iAgsData.ReplyMsg.DataLen  = E_MCP_ACTIVATE_COM_LEN;
   iAgsData.ReplyMsg.SeqNum   = MsgPtr->SeqNum;

   /* Copy process state to a local variable and get the byte-order right */
   CurrentState = htonl(iAgsGetState());
   iAgsData.ReplyMsg.DataPtr = &CurrentState;

   /* Send the message. */
   Status = mAgsReply(TRUE, &(iAgsData.ReplyMsg));
   if (Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"completed\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iAgsComReply() */



/*******************************************************************************
** Function Name:
**    iAgsErrReply
**
** Purpose:
**    Reply to a CIL message with an error class indicating a coms error.
**
** Description:
**    The err class is reserved for coms errors. The data buffer of the
**    err class contains the provided error code.
**    Note that error messages are sent directly back to the original
**    source. They do not go via the MCB or any other router process.
**
** Return Type:
**    Status_t
**       Returns the function completion status.
**
** Arguments:
**    eCilMsg_t *MsgPtr      (in)
**       A pointer to a CIL message received by the AGS. We are responding 
**       to this message. 
**    Int32_t ErrCode        (in)
**       An error code (as defined in Ags.h), to be sent with the reply.
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iAgsErrReply(
   eCilMsg_t *MsgPtr,
   Int32_t ErrCode
)
{
   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   static int ErrCodeCopy;   /* Allocate some memory for the reply pointer. */

   /* Set the reply message fields. */
   iAgsData.ReplyMsg.SourceId = MsgPtr->SourceId;
   iAgsData.ReplyMsg.DestId   = MsgPtr->DestId;
   iAgsData.ReplyMsg.Class    = E_CIL_ERR_CLASS;
   iAgsData.ReplyMsg.Service  = MsgPtr->Service;
   iAgsData.ReplyMsg.SeqNum   = MsgPtr->SeqNum;

   /* Add the error code to the message body */
   ErrCodeCopy                = htonl(ErrCode);
   iAgsData.ReplyMsg.DataPtr  = &ErrCodeCopy;
   iAgsData.ReplyMsg.DataLen  = sizeof( ErrCodeCopy );

   Status = mAgsReply(TRUE, &(iAgsData.ReplyMsg));
   if (Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"error\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iAgsErrReply() */



/*******************************************************************************
** Function Name:
**    iAgsSet1Reply
**
** Purpose:
**    Reply to a SET_1 command.
**
** Description:
**    The reply to a SET_1 is always a rsp class and not an err class as
**    err class is reserved for coms errors. Any errors that occured 
**    during the implementation of the SET_1 are reported in the Status
**    field of the eTtlSet_t structure that is contained in the
**    data buffer of the SET_1 message.
**
** Return Type:
**    Status_t
**       Returns the function completion status.
**
** Arguments:
**    eCilMsg_t *MsgPtr                (in)
**       A pointer to a CIL message recieved by the AGS. 
**    eAgsSetMsg_t *OidMsgPtr          (in/out)
**       Pointer to a structure containing the OID to be set and 
**       the set value etc.
**    Int32_t ErrCode                  (in)
**       An error code (as defined in Ags.h), to be sent
**       with the reply. This may be SYS_NOMINAL or an error.
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iAgsSet1Reply(
   eCilMsg_t *MsgPtr,
   eTtlSet_t *OidMsgPtr,
   Int32_t ErrCode )
{
   /* Local variables. */
   Status_t Status;          /* Return value from called functions. */

   /* Set the reply message fields. */
   iAgsData.ReplyMsg.SourceId = MsgPtr->SourceId;
   iAgsData.ReplyMsg.DestId   = MsgPtr->DestId;
   iAgsData.ReplyMsg.Class    = E_CIL_RSP_CLASS;
   iAgsData.ReplyMsg.Service  = MsgPtr->Service;
   iAgsData.ReplyMsg.SeqNum   = MsgPtr->SeqNum;
   
   /*
   ** Send the same structure back, but set the error code and the
   ** current value of the OID into ParamLow, if it is a valid datum.
   */
   OidMsgPtr->Status = ErrCode;
  
   /* If the OID is one that can be set then fill in the current value. */
   if( iAgsOidTable[ OidMsgPtr->DataId ].Oid    == OidMsgPtr->DataId ) 
   {
      OidMsgPtr->ParamLow = iAgsOidTable[ OidMsgPtr->DataId ].Value;
   }

   /* Add the response structure to the message body.  */
   iAgsData.ReplyMsg.DataLen = sizeof(*OidMsgPtr);
   iAgsData.ReplyMsg.DataPtr = OidMsgPtr;

   /*  Sort out the byte-ordering. */
   Status = eCilConvert32bitArray( iAgsData.ReplyMsg.DataLen, 
                                   iAgsData.ReplyMsg.DataPtr );

   /* Send the reply. */
   Status = mAgsReply( TRUE, &(iAgsData.ReplyMsg) );
   if (Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"SET_1\" reply");
      return Status;
   }

   /* If we get this far, return success. */
   return SYS_NOMINAL;

}  /* End of iAgsSetReply() */



/*******************************************************************************
** Function Name:
**    mAgsReply
**
** Purpose:
**    Sends a CIL message reply.
**
** Description:
**    Sends a reply. It swaps the source and destination CIL IDs
**    before sending the message, if the "Swap" variable is set
**    to true.
**    Note that error messages are sent directly back to the original
**    source. They do not go via the MCB or any other router process.
**
** Return Type:
**    Status_t
**       Returns the function completion status.
**
** Arguments:
**    Bool_t Swap      (in)
**       Flag to indicate if the source and destination in the MsgPtr should 
**       be flipped here or not as this may already have been done.
**
**    eCilMsg_t *MsgPtr      (in)
**       A pointer to a CIL message to be sent back as a reply.
**
** Author(s):
**    Derek J. McKay (djm)
**
*******************************************************************************/

static Status_t mAgsReply(
   Bool_t Swap,
   eCilMsg_t *MsgPtr
)
{
   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */

   /* Modify the CIL message */
   if (Swap == TRUE)
   {
      SwapAddr         = MsgPtr->SourceId;
      MsgPtr->SourceId = MsgPtr->DestId;
      MsgPtr->DestId   = SwapAddr;
   }

   /* Send the message and return its status */
   Status = eCilSend(MsgPtr->DestId, MsgPtr);
   return Status;

}  /* End of mAgsReply() */



/*******************************************************************************
** End of File Name: AgsReply.c
*******************************************************************************/
