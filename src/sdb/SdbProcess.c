/*
** Module Name:
**    SdbProcess.c
**
** Purpose:
**    A module contains message processing functions for the SDB.
**
** Description:
**    This module contains code for processing messages received
**    by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbProcess.c,v 0.24 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbProcess.c,v $
**    Revision 0.24  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.23  2001/10/23 09:35:04  mjf
**    Addition of RETRIEVE_L service, to retrieve the last 'n' measurements
**    from disk - like RETRIEVE_F but for the last (not first) measurements.
**
**    Revision 0.22  2001/10/12 11:24:58  mjf
**    Addition of a 'CLEAR_1' service, which will allow the originator of the
**    message to selectively clear their own datum entries. The message takes
**    the same form as the 'CLEAR_S' service, but the data to clear are
**    specified rather than the data to be perserved.
**
**    Revision 0.21  2001/09/14 11:08:01  djm
**    Added call to clear data from a particular source.
**
**    Revision 0.20  2001/01/03 13:32:44  mjf
**    Addition of missing error replies.
**
**    Revision 0.19  2000/12/09 14:58:07  mjf
**    Addition of SUBMIT_1P service - post a submission expecting no reply.
**
**    Revision 0.18  2000/10/04 11:07:34  djm
**    Fixed incorrect error message.
**
**    Revision 0.17  2000/09/27 11:51:12  djm
**    Improved error reporting.
**
**    Revision 0.16  2000/09/08 14:29:31  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.15  2000/09/05 14:19:01  djm
**    Added deliverer ID to command function calls. Also centralised
**    command/error counting into this function.
**
**    Revision 0.14  2000/08/31 15:32:00  djm
**    Added handling for the MCP commands ACTIVATE, SAFE-STATE
**    and SHUTDOWN.
**
**    Revision 0.13  2000/08/30 11:18:04  djm
**    Minor change to cope with the new command line arguments.
**
**    Revision 0.12  2000/07/21 14:19:00  djm
**    Added the calls for the RETRIEVE_F and RETRIEVE_1R services.
**
**    Revision 0.11  2000/06/27 14:52:39  djm
**    Improve the error handling by added statu return checking
**    on all "service processing" functions. Centralised the
**    error registraction counter to this function, thus removing
**    additional checking/counting requirements in the lower
**    level functions. Also improved the header comments.
**
**    Revision 0.10  2000/06/26 14:36:05  djm
**    Added function call for block-/multi-requests.
**
**    Revision 0.9  2000/06/23 10:34:08  djm
**    Added calls to COUNT- service functions.
**
**    Revision 0.8  2000/06/20 08:41:44  djm
**    Fixed incorrect type for CIL address.
**
**    Revision 0.7  2000/06/07 11:52:12  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.6  2000/05/22 13:30:20  djm
**    Added some diagnostic code (commented out).
**
**    Revision 0.5  2000/05/21 15:29:09  djm
**    Added calls to the SDB's "list" service functions.
**
**    Revision 0.4  2000/05/18 13:19:32  djm
**    Implemented new task-data structure mechanism.
**
**    Revision 0.3  2000/05/17 09:44:49  djm
**    Minor change to service types.
**
**    Revision 0.2  2000/05/12 08:13:00  djm
**    Added function calls for data submission and heartbeat handling.
**
**    Revision 0.1  2000/05/09 08:16:54  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "Log.h"
#include "Sdb.h"
#include "SdbPrivate.h"



Status_t iSdbProcess(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbProcess
**
** Type:
**    Status_t
**
** Purpose:
**    Message processing function for the SDB.
**
** Description:
**    This function is the main processing distribution function
**    for the SDB. It takes as its arguments the received parameters
**    from a CIL receive call (eCilReceive) that is called by the
**    function that calls this one. These parameters specify the
**    contents of the CIL message and the ID of the process that sent
**    it to the SDB (the original source is contained in the details
**    of the CIL message header).
**
**    This function does a first pass at basic CIL message chacking.
**    It tests that the soruce and destination are valid/correct and
**    that the class is of type "command" (the SDB is a pure server,
**    and thus ignores other message classes).
**
**    Although not yet implemented, the command authorisation will be
**    done (or called) from this function.
**
**    Once validated and authorised, the CIL message is passed 
**    (according to its service type) onto the appropriate function
**    for detailed parsing, the resultant action and response.
**
** Arguments:
**    Int32_t DelivererId    (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr) to the SDB.
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing. The contents of this structure may be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-Oct-2001 mjf Addition of handling of 'RETRIEVE_L' service.
**    12-Oct-2001 mjf Addition of handling of 'CLEAR_1' service.
**    10-Sep-2001 djm Added call to clear command.
**    03-Jan-2001 mjf Added sending of missing error replies.
**    09-Dec-2000 mjf Addition of submit-by-post - no response sent back.
**    04-Oct-2000 djm Fixed error in error message.
**    21-Sep-2000 djm Added DelivererId to incorrect destination message.
**    05-Sep-2000 djm Added DelivererId to most function calls.
**    05-Sep-2000 djm New method for handling command counting
**    29-Aug-2000 djm Changed the CIL ID variable that is used.
**    18-Jul-2000 djm Added RETRIEVE_F and RETRIEVE_1R service.
**    27-Jun-2000 djm Filled in the description comment in header.
**    27-Jun-2000 djm Changed the error handling slightly.
**    23-Jun-2000 djm Added multi-request service call.
**    21-Jun-2000 djm Added "count" services.
**    20-Jun-2000 djm Corrected incorrect CIL address type.
**    07-Jun-2000 djm Added LOG functions for error reporting.
**    16-May-2000 djm Minor change to service handling.
**    05-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Function return value */
   int QtyIndex;             /* Which counter to increment */


   eLogDebug(" Dx=%u, Tx=%u, Rx=%u",
         DelivererId, MsgPtr->SourceId, MsgPtr->DestId);

/*
{
   int n;

   printf("\nRx: Src=%x Dst=%x Del=%x Srv=%x Cls=%x\n",
      MsgPtr->SourceId,
      MsgPtr->DestId,
      DelivererId,
      MsgPtr->Service,
      MsgPtr->Class
   );
   for(n = 0; n < MsgPtr->DataLen; n++)
      printf("%2.2x ", ((char *)MsgPtr->DataPtr)[n]);
   printf("\n");
}
*/


   /* Make sure source is known */
   if
   (
      (MsgPtr->SourceId >= E_CIL_EOL) ||
      (MsgPtr->SourceId <= E_CIL_BOL)
   )
   {
      iSdbTaskData[D_SDB_QTY_ERRORS].Value++;
      Status = E_SDB_SRC_UNKNOWN;
      eLogErr(
         Status,
         "CIL message with unrecognised source (0x%x) received by SDB",
         MsgPtr->SourceId
      );
      return Status;
   }

   /* Make sure that destination is us! */
   if(MsgPtr->DestId != iSdbCilId)
   {
      iSdbTaskData[D_SDB_QTY_ERRORS].Value++;
      Status = E_SDB_WRONG_DST;
      eLogErr(
         Status,
         "CIL message with incorrect destination (0x%x) "
         " received from 0x%x by SDB",
         MsgPtr->DestId, DelivererId
      );
      return Status;
   }

   /* As we are a strict server, then rejecet any non-command-class msgs */
   if(MsgPtr->Class != E_CIL_CMD_CLASS)
   {
      iSdbTaskData[D_SDB_QTY_ERRORS].Value++;
      Status = E_SDB_NOT_COMMAND;
      eLogErr(
         Status, "Non-command class (0x%x) received by SDB", MsgPtr->Class
      );
      return Status;
   }


   /* If we get this far, then process the message */

   /* Set a "catch-all" error message in case one is not set */
   Status = SYS_WARNING;
   QtyIndex = D_SDB_QTY_ERRORS;

   /* Call a different function, based on the CIL service */
   switch(MsgPtr->Service)
   {
      case E_SDB_HEARTBEAT:
         QtyIndex = D_SDB_QTY_HBEAT;
         Status = iSdbHeartbeat(DelivererId, MsgPtr);
         break;
      case E_SDB_SUBMIT_1:
         QtyIndex = D_SDB_QTY_SUBMITTED;
         Status = iSdbSubmit(DelivererId, TRUE, MsgPtr);
         break;
      case E_SDB_SUBMIT_1P:
         QtyIndex = D_SDB_QTY_SUBMITTED;
         Status = iSdbSubmit(DelivererId, FALSE, MsgPtr);
         break;
      case E_SDB_RETRIEVE_1:
         QtyIndex = D_SDB_QTY_RETRIEVED;
         Status = iSdbRetrieve(FALSE, DelivererId, MsgPtr);
         break;
      case E_SDB_RETRIEVE_1R:
         QtyIndex = D_SDB_QTY_RETRIEVED;
         Status = iSdbRetrieve(TRUE, DelivererId, MsgPtr);
         break;
      case E_SDB_RETRIEVE_N:
         QtyIndex = D_SDB_QTY_RETRIEVED;
         Status = iSdbMulRetr(DelivererId, MsgPtr);
         break;
      case E_SDB_RETRIEVE_F:
         QtyIndex = D_SDB_QTY_RETRIEVED;
         Status = iSdbFileRetr(DelivererId, MsgPtr, FALSE);
         break;
      case E_SDB_RETRIEVE_L:
         QtyIndex = D_SDB_QTY_RETRIEVED;
         Status = iSdbFileRetr(DelivererId, MsgPtr, TRUE);
         break;
      case E_SDB_CLEAR_S:
         QtyIndex = D_SDB_QTY_MISC;
         Status = iSdbClearSource(DelivererId, MsgPtr);
         break;         
      case E_SDB_CLEAR_1:
         QtyIndex = D_SDB_QTY_MISC;
         Status = iSdbClearData(DelivererId, MsgPtr);
         break;         
      case E_SDB_COUNTSOURCES:
         QtyIndex = D_SDB_QTY_COUNT;
         Status = iSdbCountSources(DelivererId, MsgPtr);
         break;         
      case E_SDB_COUNTDATA:
         QtyIndex = D_SDB_QTY_COUNT;
         Status = iSdbCountData(DelivererId, MsgPtr);
         break;         
      case E_SDB_COUNTMSRMENTS:
         QtyIndex = D_SDB_QTY_COUNT;
         Status = iSdbCountMsrments(DelivererId, MsgPtr);
         break;         
      case E_SDB_LISTSOURCES:
         QtyIndex = D_SDB_QTY_LIST;
         Status = iSdbListSources(DelivererId, MsgPtr);
         break;         
      case E_SDB_LISTDATA:
         QtyIndex = D_SDB_QTY_LIST;
         Status = iSdbListData(DelivererId, MsgPtr);
         break;
      case E_SDB_PURGE:
         Status = E_SDB_GEN_ERR;
         eLogWarning(Status, "Purge command not implemented");
         break;
      case E_SDB_SHUTDOWN:
         QtyIndex = D_SDB_QTY_MISC;
         Status = iSdbShutdown(DelivererId, MsgPtr);
         break;
      case E_SDB_SAFESTATE:
         QtyIndex = D_SDB_QTY_MISC;
         Status = iSdbSafeState(DelivererId, MsgPtr);
         break;
      case E_SDB_ACTIVATE:
         QtyIndex = D_SDB_QTY_MISC;
         Status = iSdbActivate(DelivererId, MsgPtr);
         break;
      default:
         Status = E_SDB_UNKNOWN_SERVICE;
         eLogErr(
            Status,
            "Unknown service (0x%x) requested of SDB",
            MsgPtr->Service
         );
         iSdbErrReply(DelivererId, MsgPtr, Status);
         break;
   }  /* End of switch() */


   /* Check for errors, increment the counter if there are any */
   if(Status != SYS_NOMINAL)
   {
      iSdbTaskData[D_SDB_QTY_ERRORS].Value++;
   }
   else
   {
      iSdbTaskData[QtyIndex].Value++;
   }

   /* Terminate the function, reporting success */
   return Status;


}  /* End of iSdbProcess() */


/* EOF */
