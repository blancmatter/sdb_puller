/*
** Module Name:
**    SdbHeartbeat.c
**
** Purpose:
**    A module with code to process messages received from the CHB.
**    functions.
**
** Description:
**    This module contains code for processing messages received
**    from the continuous heartbeat (CHB) task..
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbHeartbeat.c,v 0.14 2006/06/26 15:34:22 mjf Exp $
**
** History:
**    $Log: SdbHeartbeat.c,v $
**    Revision 0.14  2006/06/26 15:34:22  mjf
**    Remove redundant checks.
**    Remove duplicate attempts to submit code version number.
**
**    Revision 0.13  2003/10/07 09:21:29  sxf
**    Submit SDB version everyheartbeat.
**
**    Revision 0.12  2000/12/04 10:42:17  mjf
**    Enhanced log messages to include CIL name in addition to CIL ID.
**
**    Revision 0.11  2000/09/08 14:29:57  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.10  2000/09/08 13:02:36  djm
**    Added recording of heartbeat receipt time.
**
**    Revision 0.9  2000/09/05 14:17:35  djm
**    Alteration to heartbeat counting. Better message checking.
**
**    Revision 0.8  2000/06/27 14:52:31  djm
**    Improve and centralise error reporting/registration.
**
**    Revision 0.7  2000/06/07 11:51:55  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.6  2000/05/26 08:00:19  djm
**    Changed htons to htonl for returned state and also force the
**    message data body size to the state-size, rather than just
**    adding the size to the existing one (for added robustness).
**
**    Revision 0.5  2000/05/18 14:41:07  djm
**    Added automatic submission of SDB data to its own database
**    as a response to receipt of a heartbeat message. This is
**    done after the heartbeat reply, in case of error.
**
**    Revision 0.4  2000/05/18 13:18:37  djm
**    Implemented new task-data structure mechanism.
**
**    Revision 0.3  2000/05/17 15:00:13  djm
**    Changed process state message data from a 16 to a 32 bit word.
**
**    Revision 0.2  2000/05/17 09:39:22  djm
**    Minor change to abstract the CIL ID of the heartbeat source.
**
**    Revision 0.1  2000/05/12 08:56:34  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "TtlSystem.h"
#include "Cil.h"
#include "Log.h"
#include "Tim.h"
#include "Sdb.h"
#include "SdbPrivate.h"



/* Functions */


Status_t iSdbHeartbeat(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbHeartbeat
**
** Type:
**    Status_t
**
** Purpose:
**    Processing function for CHB messages
**
** Description:
**    ...
**
** Arguments:
**    Uint32_t DelivererID   (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr)..
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message that requires processing. The 
**       contents of this structure are changed as the source and
**       destination are swapped and the process state is put in
**       the data block.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    08-Sep-2000 djm Added new global definition enumeration names.
**    05-Sep-2000 djm Added recording of heartbeat receipt time.
**    07-Jun-2000 djm Added LOG functions for error reporting.
**    16-May-2000 djm Changed symbolic constant on HBEAT source CIL ID.
**    09-May-2000 djm Initial creation.
**
*/

   /* No local variables */
   Int32_t SwapAddr;         /* Address variable for swapping Src/Dst */
   Int32_t State;            /* State of the process */
   Status_t Status;          /* Returned value from called functions */


   /* Check to make sure the heartbeat has come from the authorised source */
   if( (MsgPtr->SourceId != I_SDB_HBEAT_ID) || (DelivererId != I_SDB_HBEAT_ID) )
   {
      Status = E_SDB_WRONG_SRC;
      eLogErr(
         Status,
         "Heartbeat received from non-CHB source (0x%x '%s' via 0x%x '%s')",
         MsgPtr->SourceId, eCilNameString( MsgPtr->SourceId ),
         DelivererId, eCilNameString( DelivererId )
      );
      return Status;
   }


   /* Check for correct message length */
   if(MsgPtr->DataLen != 0)
   {
      Status = E_SDB_GEN_ERR;
      eLogErr(
         Status,
         "Heartbeat command of incorrect size data (%d)",
         MsgPtr->DataLen
      );
      return Status;
   }


   /*
   ** NOW RESTRUCTURE THE MESSAGE FOR THE REPLY...
   */

   /* Swap source and destination fields */
   SwapAddr = MsgPtr->SourceId;
   MsgPtr->SourceId = MsgPtr->DestId;
   MsgPtr->DestId = SwapAddr;

   /* Add the process state to the end */
   State = htonl(iSdbTaskData[D_SDB_PROC_STATE].Value);
   MsgPtr->DataLen = sizeof(State);
   memcpy(((char *)(MsgPtr->DataPtr)), &State, sizeof(State));

   /* Indicate that we are responding to the heartbeat request */
   MsgPtr->Class = E_CIL_RSP_CLASS;
   

   /*
   ** NOW RETURN THE MESSAGE TO THE SENDER...
   */


   /* Return message */
   Status = eCilSend(MsgPtr->DestId, MsgPtr);

   /* Check status of the reply, increment appropriate global counter */
   if(Status != SYS_NOMINAL)
   {
      /*
      ** If there was a failure, we make a note of it with the LOG
      ** package and by incrementing the SDB error counter. Note
      ** that we DO NOT return at this point because, even though
      ** the reply failed, we are able to push on and at least
      ** attempt to store our own internal data in the publically
      ** available linked-list database.
      */
      iSdbTaskData[D_SDB_QTY_ERRORS].Value++;
      eLogErr(Status, "Error attempting to send heartbeat reply");
   }


   /*
   ** FINALLY, SUBMIT OUR GLOBAL DATA TO THE DATABASE FOR ALL TO ACCESS...
   */

   Status = iSdbAutoSubmit();
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Failure to auto-submit SDB data");
      return Status;
   }
   

   /* Make a note in a global variable of the heartbeat receipt time. */
   /* This will be used for safe-state determination in the main loop */
   Status = eTimGetTime(&iSdbHeartBeatTime);
   if(Status != SYS_NOMINAL)
   {
      eLogErr(Status, "Unable to record system time for heartbeat time stamp");
      return Status;
   }   


   /* Terminate the function, reporting the submission status. */
   return SYS_NOMINAL;


} /* End of iSdbHeartbeat() */


/* EOF */
