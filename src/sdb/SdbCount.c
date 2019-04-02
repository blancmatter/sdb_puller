/*
** Module Name:
**    SdbCount.c
**
** Purpose:
**    Functions for counting the contents of the database.
**
** Description:
**    This module contains code for getting a count of the
**    contents of the database maintained by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbCount.c,v 0.9 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbCount.c,v $
**    Revision 0.9  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.8  2006/06/27 16:35:27  mjf
**    Tidy up of logging - no functional changes.
**
**    Revision 0.7  2001/01/03 13:32:44  mjf
**    Addition of missing error replies.
**
**    Revision 0.6  2000/12/04 10:42:17  mjf
**    Enhanced log messages to include CIL name in addition to CIL ID.
**
**    Revision 0.5  2000/09/05 14:16:45  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.4  2000/06/27 14:51:07  djm
**    Improve and centralise error reporting/registration.
**
**    Revision 0.3  2000/06/26 14:36:42  djm
**    Converted module private function into application-wide scope.
**
**    Revision 0.2  2000/06/23 10:33:34  djm
**    Added iSdbCountMulReq() and associated functions.
**
**    Revision 0.1  2000/06/22 07:57:48  djm
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
#include "Tim.h"
#include "Sdb.h"
#include "SdbPrivate.h"


/* Module function prototypes */
/*
Status_t mSdbCountMulReq(eSdbMulReq_t *MulReqPtr);
*/


/* Functions */


Status_t iSdbCountSources(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbCountSources
**
** Type:
**    Status_t
**
** Purpose:
**    Counts the number of sources that have submitted data to the database.
**
** Description:
**    Creates a CIL message which contains a count of the source
**    ID's that exist within the database maintained by the SDB.
**    This function assumes that the message has been validated
**    by a higher level. In particular, the message has been checked
**    for valid source, correct destination (namely this program!),
**    and that it is a "command class" message.
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
**    03-Jan-2001 mjf Added sending of missing error replies.
**    05-Sep-2000 djm Added deliverer ID to arguments list.
**    21-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumSrcs;         /* Number of source IDs known */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */
   int Index;                /* Index into the hash-table array */
   int n;                    /* inner loop counter */
   Int32_t SrcList[I_SDB_MAXIDS];        /* List of sources found */
   Bool_t Found;             /* Flag to indicate item found in list */
   iSdbDefn_t *DefnPtr;      /* Data element definiton from hash-table */



   /* Initialise any local variables and the source list */
   NumSrcs = 0;
   for(n = 0; n < I_SDB_MAXIDS; n++)
   {
      SrcList[n] = -1;
   }

   /* Check that there was no associated message */
   if(MsgPtr->DataLen != 0)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr
      (
        Status,
        "CountSources message contains unexpected data (%d bytes, 0 expected)",
         MsgPtr->DataLen
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Loop over the hash-table array */
   for(Index = 0; Index < I_SDB_HASHSIZE; Index++)
   {
      /* Loop over the hash-entries for the current hash-table index */
      for
      (
         DefnPtr = iSdbHashTable[Index];
         DefnPtr != NULL;
         DefnPtr = DefnPtr->NextHashPtr
      )
      {

         /* Scan our list of sources */
         Found = FALSE;
         for(n = 0; SrcList[n] != -1; n++)
         {

            /* If we find that source already, then resume the loops */
            if(SrcList[n] == DefnPtr->SourceId)
            {
               Found = TRUE;
               break;
            }
         }

         /* If we found that we've listed that source already, then resume */
         if(Found == TRUE)
         {
            continue;
         }

         /* Otherwise, put our new source at the end */
         SrcList[n] = DefnPtr->SourceId;

         /* Incr. the counter and check that we haven't reached the limit */
         NumSrcs++;

         if(NumSrcs > I_SDB_MAXIDS)
         {
            Status = E_SDB_GEN_ERR;
            eLogErr(Status, "Overflow of data types. Unable to reply");
            return Status;
         }
      }
   }


   /* Prepare CIL message for reply */

   /* Modify the CIL header, in the first instance, swap the src/dst */
   SwapAddr = MsgPtr->SourceId;
   MsgPtr->SourceId = MsgPtr->DestId;
   MsgPtr->DestId = SwapAddr;

   /* Change the message class */
   MsgPtr->Class = E_CIL_RSP_CLASS;

   /* Append the message data */
   MsgPtr->DataLen = sizeof(NumSrcs);
   NumSrcs = htonl(NumSrcs);
   memcpy(MsgPtr->DataPtr, &NumSrcs, MsgPtr->DataLen);


   /* Actually send the message */
   Status = eCilSend(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Transmission failure");
   }


   /* Terminate the function, reporting status of the eCilSend() call */
   return Status;


}  /* End of iSdbCountSources() */




Status_t iSdbCountData(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbCountData
**
** Type:
**    Status_t
**
** Purpose:
**    Count the number of data points stored for a given source.
**
** Description:
**    ...
**
**    This function assumes that the message has been validated
**    by a higher level. In particular, the message has been checked
**    for valid source, correct destination (namely this program!),
**    and that it is a "command class" message.
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
**    03-Jan-2001 mjf Added sending of missing error replies.
**    05-Sep-2000 djm Added deliverer ID to arguments list.
**    21-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumData;         /* Number of Datum IDs known */
   Int32_t SourceId;         /* ID of the source to get data IDs for */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */
   int Index;                /* Index into the hash-table array */
   int n;                    /* inner loop counter */
   Int32_t DatList[I_SDB_MAXIDS];        /* List of sources found */
   Bool_t Found;             /* Flag to indicate item found in list */
   iSdbDefn_t *DefnPtr;      /* Data element definiton from hash-table */


   /* Initialise any local variables and the source list */
   NumData = 0;
   for(n = 0; n < I_SDB_MAXIDS; n++)
   {
      DatList[n] = -1;
   }

   /* Check that the message length is of the expected size */
   if(MsgPtr->DataLen != sizeof(SourceId))
   {
      Status = E_SDB_TRUNCATED;
      eLogErr
      (
         Status, 
         "CountData message of incorrect length "
         "(%d bytes received, %d expected)",
         MsgPtr->DataLen, sizeof(SourceId)
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Extract from the incoming message the SourceId we're looking for */
   memcpy(&SourceId, (char *)(MsgPtr->DataPtr), sizeof(SourceId));
   SourceId = ntohl(SourceId);

   /* Loop over the hash-table array */
   for(Index = 0; Index < I_SDB_HASHSIZE; Index++)
   {
      /* Loop over the hash-entries for the current hash-table index */
      for
      (
         DefnPtr = iSdbHashTable[Index];
         DefnPtr != NULL;
         DefnPtr = DefnPtr->NextHashPtr
      )
      {
         /* First check that we have an entry with the correct SourceId */
         if(DefnPtr->SourceId != SourceId)
         {
            continue;
         }

         /* Scan our list of sources */
         Found = FALSE;
         for(n = 0; DatList[n] != -1; n++)
         {
            /* If we find that source already, then resume the loops */
            if(DatList[n] == DefnPtr->DatumId)
            {
               Found = TRUE;
               break;
            }
         }

         /* If we found that we've listed that source already, then resume */
         if(Found == TRUE)
         {
            continue;
         }

         /* Otherwise, put our new source at the end */
         DatList[n] = DefnPtr->DatumId;

         /* Incr. the counter and check that we haven't reached the limit */
         NumData++;
         if(NumData > I_SDB_MAXIDS)
         {
            Status = E_SDB_GEN_ERR;
            eLogErr
            (
               Status,
               "Overflow of data types for source 0x%x '%s'. Unable to reply",
               SourceId, eCilNameString( SourceId )
            );
            return Status;
         }
      }
   }


   /* Prepare CIL message for reply */

   /* Modify the CIL header, in the first instance, swap the src/dst */
   SwapAddr = MsgPtr->SourceId;
   MsgPtr->SourceId = MsgPtr->DestId;
   MsgPtr->DestId = SwapAddr;

   /* Change the message class */
   MsgPtr->Class = E_CIL_RSP_CLASS;

   /* Append the message data */
   MsgPtr->DataLen = sizeof(NumData);
   NumData = htonl(NumData);
   memcpy(MsgPtr->DataPtr, &NumData, MsgPtr->DataLen);


   /* Actually send the message */
   Status = eCilSend(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Transmission failure");
   }


   /* Terminate the function, reporting status of the transmit call */
   return Status;

}  /* End of iSdbCountData() */



Status_t iSdbCountMsrments(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbCountMsrments
**
** Type:
**    Status_t
**
** Purpose:
**    Determine the number of measurements that exist in the database
**    for a given source/datum.
**
** Description:
**    If the timestamps are set to zero, then the function will
**    determine the total number of measurements for the source/datum.
**
**    This function assumes that the message has been validated
**    by a higher level. In particular, the message has been checked
**    for valid source, correct destination (namely this program!),
**    and that it is a "command class" message.
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
**    03-Jan-2001 mjf Added sending of missing error replies.
**    05-Sep-2000 djm Added deliverer ID to arguments list.
**    21-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumReqs;         /* Number of requests */
   char *BufPtr;             /* Temporary pointer to data block of message */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */
   size_t BufSize;           /* Memory size to allocate for buffer */
   Uint32_t Req;             /* Request loop counter variable */
   eSdbMulReq_t MulReq;      /* Mulitple request structure */



   /* Does message hold at least enough bytes to hold the number of requests */
   if(MsgPtr->DataLen < sizeof(NumReqs))
   {
      Status = E_SDB_TRUNCATED;
      eLogErr
      (
         Status, 
         "CountMsrments message truncated "
         "(%d bytes received, at least %d were expected)",
         MsgPtr->DataLen, sizeof(NumReqs)
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Assign a temporary pointer to the data block of the message */
   BufPtr = (char *)(MsgPtr->DataPtr);

   /* Extract the number of multi-requests */
   memcpy(&NumReqs, BufPtr, sizeof(NumReqs));
   NumReqs = ntohl(NumReqs);
   BufPtr += sizeof(NumReqs);


   /* Check the message length against the calculated message length */
   /* based on the number of contained data element requests */
   BufSize = sizeof(NumReqs) + (NumReqs * sizeof(eSdbMulReq_t));
   if(MsgPtr->DataLen != BufSize)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr(
          Status,
          "CountMsrments request from source 0x%x '%s' truncated "
          "(%d of %d expected data bytes received)",
          MsgPtr->SourceId, eCilNameString( MsgPtr->SourceId ),
          MsgPtr->DataLen, BufSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* If correct length, convert the byte-order of the entire message */
   eCilConvert32bitArray(MsgPtr->DataLen, MsgPtr->DataPtr);


/* DEBUG - MESSAGE DIAGNOSTIC CODE. LEAVE COMMENTED OUT FOR PRODUCTION */
/*
{
int n;
printf("--- DataLen = %d\n", MsgPtr->DataLen);
printf("Rx (0x%2.2x): ", MsgPtr->SourceId);
for(n = 0; n < MsgPtr->DataLen; n++)
  printf("%2.2x", ((char *)MsgPtr->DataPtr)[n]);
printf("\n");
}
*/


   /* Loop over each multi-request */
   for(Req = 0; Req < NumReqs; Req++)
   {
      /* Extract the next multi-request */
      memcpy(&MulReq, BufPtr, sizeof(MulReq));

      /* Process the multi-request, filling in the NumMsrments field */
      Status = iSdbCountMulReq(&MulReq);
      if(Status != SYS_NOMINAL)
      {
         eLogErr(
            Status,
            "Failure to process MulReq"
         );
         iSdbErrReply(DelivererId, MsgPtr, Status);
         return Status;
      }

      /* Copy the filled structure back into the message */
      memcpy(BufPtr, &MulReq, sizeof(MulReq));

      /* Increment the buffer pointer */
      BufPtr += sizeof(MulReq);
   }

   /* We now have a filled in message. Reformat the header for return */

   /* Modify the CIL header, in the first instance, swap the src/dst */
   SwapAddr = MsgPtr->SourceId;
   MsgPtr->SourceId = MsgPtr->DestId;
   MsgPtr->DestId = SwapAddr;

   /* Change the message class */
   MsgPtr->Class = E_CIL_RSP_CLASS;

   /* Convert the byte order of entire message back to network order again */
   eCilConvert32bitArray(MsgPtr->DataLen, MsgPtr->DataPtr);

   /* Return the filled-in and reformatted message to the requesting task */
   Status = eCilSend(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Transmission failure");
   }

   /* Terminate the function, reporting state of the transmit function */
   return Status;


}  /* End of iSdbCountData() */





Status_t iSdbCountMulReq(
   eSdbMulReq_t *MulReqPtr
)
{
/*
** Function Name:
**    iSdbCountMulReq
**
** Type:
**    Status_t
**
** Purpose:
**    Determine the number of measurements that exist in the database
**    for a given source/datum.
**
** Description:
**    If the timestamps are set to zero, then the function will
**    determine the total number of measurements for the source/datum.
**    This function only processes a single source/datum pair.
**
**    In more detail, this function will take a multiple-request 
**    structure. This will contain a source ID, a datum ID and possibly
**    some timestamps. on input, the NumMsrments field will be irrelevant.
**    What this function will do is to look up the source/datum entry, and
**    determine how many measurments (time/value pairs) exist between the
**    specified time stamps. If both timestamps are zero, then it will
**    return the total number of measurements held in the database for that
**    source/datum definition.
**
** Arguments:
**    eSdbMulReq_t *MulReqPtr     (in/out)
**       A pointer to a Multi-request structure. The NumMsrments field
**       within this structure will be changed according to the notes
**       in the description above.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    21-Jun-2000 djm Initial creation.
**
*/
   /* Local variables */
   Status_t Status;           /* Storage for returning error codes, etc. */
   iSdbDefn_t *DefnPtr;       /* Data element defn extracted from message */
   iSdbEvent_t *EventPtr;     /* Data element event extracted from message */


   /* Start by ensuring that the result is zero in the first instance */
   MulReqPtr->NumMsrments = 0;


   /* Firstly, see if the data already exists in our hash-table */
   DefnPtr = iSdbHashLookup(MulReqPtr->SourceId, MulReqPtr->DatumId);
   if(DefnPtr == NULL)
   {
      Status = E_SDB_UNKNOWN_DEFN;
      printf("\n");
      eLogInfo(
         "Source/datum pair (0x%x '%s',0x%x) not in database"
         " (MulReq count request)",
         MulReqPtr->SourceId, eCilNameString( MulReqPtr->SourceId ), 
         MulReqPtr->DatumId
      );
      return Status;
   }

   /*
   ** If we get this far, then we have a valid MsgDefnPtr
   ** from which to retrieve the data.
   */

   /* Get the pointer to the most recent data element */
   EventPtr = DefnPtr->NewestPtr;

   /* If there isn't one, then no data exists for this definition */
   if(EventPtr == NULL)
   {
      /* Just return with the zero-initialised NumMsrments field */
      return SYS_NOMINAL;
   }

   /* If the timestamps are both zero, just return the total no. measurements */
   if(
      (MulReqPtr->NewestTime.t_sec == 0) &&
      (MulReqPtr->NewestTime.t_nsec == 0) &&
      (MulReqPtr->OldestTime.t_sec == 0) &&
      (MulReqPtr->OldestTime.t_nsec == 0)
   )
   {
      /* Set NumMsrments to total no. measurements stored for that data def'n */
      MulReqPtr->NumMsrments = DefnPtr->NumData;
      return SYS_NOMINAL;
   }

   /* If we still haven't returned, then sift through the data, counting any */
   /* measurements that lie within the specified range of the two timestamps */
   for(
      EventPtr = DefnPtr->NewestPtr;
      EventPtr != NULL;
      EventPtr = EventPtr->PrevPtr
   )
   {

/* DEBUG - DIAGNOSTIC CODE FOR TIME COMPARISONS. DELETE IN PRODUCTION CODE */
/*
{
  char Str[32];

  eTimToString(&EventPtr->TimeStamp, 32, Str);
  printf("Comparing %s", Str);
  eTimToString(&MulReqPtr->OldestTime, 32, Str);
  printf(" against %s", Str);
  eTimToString(&MulReqPtr->NewestTime, 32, Str);
  printf(" and %s\n", Str);
}
*/

      /* Check if the timestamp for this measurement lies within the range */
      if(
         (eTimCompare(&EventPtr->TimeStamp, &MulReqPtr->NewestTime) <= 0) &&
         (eTimCompare(&EventPtr->TimeStamp, &MulReqPtr->OldestTime) >= 0)
      )
      {
         /* Increment the NumMsrments counter */
         MulReqPtr->NumMsrments++;
      }
   }


   /* Terminate the function indicating success */
   return SYS_NOMINAL;

}  /* End of iSdbCountMulReq() */


/* EOF */
