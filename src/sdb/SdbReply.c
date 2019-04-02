/*
** Module Name:
**    SdbReply.c
**
** Purpose:
**    A module contains functions for simple replies to other tasks..
**
** Description:
**    This module contains functions for replying to other tasks that
**    have submitted/requested data to/from the SDB. These functions
**    are not for sending back data, but are for information-type
**    messages like acknowlegdements or errors.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbReply.c,v 0.8 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbReply.c,v $
**    Revision 0.8  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.7  2001/01/03 11:41:24  mjf
**    Fix to typo in error message.
**
**    Revision 0.6  2000/10/04 11:08:16  djm
**    Fixed incorrect byte-ordering in the error reply code.
**
**    Revision 0.5  2000/09/08 14:29:40  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.4  2000/09/05 14:19:33  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.3  2000/08/31 15:32:56  djm
**    Added new reply functions for the MCP commands.
**
**    Revision 0.2  2000/06/07 11:52:34  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.1  2000/05/17 14:37:04  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "TtlSystem.h"
#include "Cil.h"
#include "Log.h"
#include "Sdb.h"
#include "SdbPrivate.h"


/* Local functiojn prototypes */

static Status_t mSdbReply(Bool_t Swap, Int32_t DelivererId, eCilMsg_t *MsgPtr);


/* Functions */


Status_t iSdbAckReply(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbAckReply
**
** Type:
**    Status_t
**
** Purpose:
**    Reply to a CIL message with an acknowledgement.
**
** Description:
**    ...
**
** Arguments:
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing. The contents of this structure will be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    07-Jun-2000 djm Added LOG functions for error reporting.
**    17-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */

   /* Change the message class */
   MsgPtr->Class = E_CIL_ACK_CLASS;

   /* Ensure no "message body" data is sent */
   MsgPtr->DataLen = 0;

   /* Send the message */
   Status = mSdbReply(TRUE, DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"acknowledgement\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iSdbAckReply() */


Status_t iSdbActReply(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbActReply
**
** Type:
**    Status_t
**
** Purpose:
**    Reply to a CIL message with an "in progress" class.
**
** Description:
**    ...
**
** Arguments:
**    Int32_t DelivererId    (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr) to the SDB.
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing. The contents of this structure will be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    31-Aug-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */

   /* Change the message class */
   MsgPtr->Class = E_CIL_ACT_CLASS;

   /* Ensure no "message body" data is sent */
   MsgPtr->DataLen = 0;

   /* Send the message */
   Status = mSdbReply(TRUE, DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"acting\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iSdbActReply() */



Status_t iSdbComReply(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbComReply
**
** Type:
**    Status_t
**
** Purpose:
**    Reply to a CIL message with an completed class and procstate.
**
** Description:
**    This function sends a "completed" reply, and includes the current
**    SDB process state as the only item in the CIL data block.
**
** Arguments:
**    Int32_t DelivererId    (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr) to the SDB.
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing. The contents of this structure will be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    31-Aug-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   ProcState_t State;        /* Variable containing the state of the SDB */

   /* Change the message class */
   MsgPtr->Class = E_CIL_COM_CLASS;

   /* Copy process state to a local variable and get the byte-order right */
   State = htonl(iSdbTaskData[D_SDB_PROC_STATE].Value);

   /* Formulate the data body */
   memcpy((void *)MsgPtr->DataPtr, (void *)&State, sizeof(State));
   MsgPtr->DataLen = sizeof(iSdbTaskData[D_SDB_PROC_STATE].Value);

   /* Send the message (don't swap the addresses, the ACT would've done that */
   Status = mSdbReply(FALSE, DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"completed\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iSdbComReply() */




Status_t iSdbErrReply(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr,
   Int32_t ErrCode
)
{
/*
** Function Name:
**    iSdbErrReply
**
** Type:
**    Status_t
**
** Purpose:
**    Reply to a CIL message with an error.
**
** Description:
**    Extracts all relevant data from the provided CIL message and
**    modifies the relevant entries before using eCilSend to return
**    it to its original source. It appends the provided error code.
**
** Arguments:
**    Int32_t DelivererId    (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr) to the SDB.
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing. The contents of this structure will be changed.
**    Int32_t ErrCode        (in)
**       An error code (as defined in Sdb.h), to be appended and sent
**       with the reply.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    07-Jun-2000 djm Added LOG functions for error reporting.
**    17-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */


   /* Change the message class */
   MsgPtr->Class = E_CIL_ERR_CLASS;

   /* Add the error code to the message body */
   ErrCode = htonl(ErrCode);
   MsgPtr->DataLen = sizeof(ErrCode);
   memcpy(MsgPtr->DataPtr, &ErrCode, sizeof(ErrCode));

   Status = mSdbReply(TRUE, DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to send \"error\" reply");
      return Status;
   }

   /* If we get this far, then return sucess */
   return SYS_NOMINAL;

}  /* End of iSdbErrReply() */








static Status_t mSdbReply(
   Bool_t Swap,
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    mSdbReply
**
** Type:
**    Status_t
**
** Purpose:
**    Sends a CIL message reply.
**
** Description:
**    Sends a reply. It swaps the source and destination CIL IDs
**    before sending the message, if the "Swap" variable is set
**    to true.
**
** Arguments:
**    Bool_t Swap            (in)
**       If TRUE, then the function will swap the source and
**       destination variables.
**    Int32_t DelivererId    (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr) to the SDB.
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing. The contents of this structure will be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    31-Aug-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */

   /* Modify the CIL message */
   if(Swap == TRUE)
   {
      SwapAddr = MsgPtr->SourceId;
      MsgPtr->SourceId = MsgPtr->DestId;
      MsgPtr->DestId = SwapAddr;
   }

   /* Send the message and return its status */
   Status = eCilSend(DelivererId, MsgPtr);
   return Status;

}  /* End of mSdbReply() */



/* EOF */
