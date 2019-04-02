/*
** Module Name:
**    SdbMulRetr.c
**
** Purpose:
**    A module contains functions for getting block data from the database.
**
** Description:
**    This module contains code for getting block data from the database
**    maintained by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbMulRetr.c,v 0.10 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbMulRetr.c,v $
**    Revision 0.10  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.9  2006/06/27 16:35:27  mjf
**    Tidy up of logging - no functional changes.
**
**    Revision 0.8  2001/01/03 13:32:44  mjf
**    Addition of missing error replies.
**
**    Revision 0.7  2000/12/04 10:42:17  mjf
**    Enhanced log messages to include CIL name in addition to CIL ID.
**
**    Revision 0.6  2000/09/19 15:50:13  djm
**    Corrected code to correctly set byte ordering.
**
**    Revision 0.5  2000/09/05 14:18:13  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.4  2000/06/27 14:52:02  djm
**    Improve and centralise error reporting/registration.
**
**    Revision 0.3  2000/06/27 08:45:24  djm
**    Added code to prevent response with additional zero-bytes
**    appended to the end of the CIL message.
**
**    Revision 0.2  2000/06/26 16:52:38  djm
**    Got the main functionality of the code implemented and mostly
**    debugged. There is still some remaining work to be done of
**    zero-padded trailing bytes on the end of the return message.
**
**    Revision 0.1  2000/06/26 14:34:42  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TtlSystem.h"
#include "Log.h"
#include "Cil.h"
#include "Tim.h"
#include "Sdb.h"
#include "SdbPrivate.h"


/* Function prototypes */

static Status_t mSdbGetMulData();



/* Functions */


Status_t iSdbMulRetr(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbMulRetr
**
** Type:
**    Status_t
**
** Purpose:
**    Extract block data from the SDB database into a CIL message.
**
** Description:
**    Fills a CIL message with data from the status database.
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
**       Essentially, this routine will fill in the gaps within this
**       message.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    03-Jan-2001 mjf Added sending of missing error replies.
**    19-Sep-2000 djm Fixed bug with network byte ordering.
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    27-Jun-2000 djm Added error count increments on errors.
**    27-Jun-2000 djm Stripped excess zeros from return message.
**    26-Jun-2000 djm Got working.
**    23-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumReqs;         /* Number of requests that are being made */
   size_t InBufSize;         /* Expected size of input buffer */
   size_t OutBufSize;        /* Memory size to allocate for buffer */
   size_t BlockSize;         /* The size of a block that has been retrieved */
   char *InBufPtr;           /* Temporary pointer to data block of message */
   char *OutBufPtr;          /* Pointer to output buffer */
   char *MarkerPtr;          /* Temp. pointer to remember a place in an array */
   Uint32_t NumMsrments;     /* Temp. storage for number of measurements */
   Uint32_t TotNumMsrments;  /* Total number of measurements to be retrieved */
   Uint32_t Req;             /* Request loop counter */
   eSdbMulReq_t MulReq;      /* Holding structure for a multi-request */
   Int32_t SwapAddr;         /* Temporary variable for swapping src/dst IDs */


   /* Convert the entire message from network byte order into hardware order */
   eCilConvert32bitArray(MsgPtr->DataLen, (char *)(MsgPtr->DataPtr));


   /* Check message for sufficient length to hold at least the no. requests */
   if(MsgPtr->DataLen < sizeof(NumReqs))
   {
      Status = E_SDB_TRUNCATED;
      eLogErr(
         Status,
         "Retrieve multi-request from source 0x%x '%s' "
         "truncated (%d bytes only)",
         MsgPtr->SourceId, eCilNameString( MsgPtr->SourceId ), MsgPtr->DataLen
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Assign a temporary pointer to the data block of the message */
   InBufPtr = (char *)(MsgPtr->DataPtr);

   /* Extract the number of data to be retrieved from the message */
   memcpy(&NumReqs, InBufPtr, sizeof(NumReqs));
   InBufPtr += sizeof(NumReqs);


   /* Check that we have enough bytes to extract all the requests */
   /* based on the number of contained data element requests */
   InBufSize = sizeof(NumReqs) + (NumReqs * sizeof(eSdbMulReq_t));
   if(MsgPtr->DataLen != InBufSize)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr(
          Status,
          "Requests of retrieve multi-request from source 0x%x '%s' truncated "
          "(%d of %d expected data bytes received)",
          MsgPtr->SourceId, eCilNameString( MsgPtr->SourceId ), 
          MsgPtr->DataLen, InBufSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Reset the total measurement count (=T) to zero */
   TotNumMsrments = 0;

   /* Remember our place in the input buffer */
   MarkerPtr = InBufPtr;

   /* Loop through each request: */
   for(Req = 0; Req < NumReqs; Req++)
   {
      /* Extract the next multi-request */
      memcpy(&MulReq, InBufPtr+Req*sizeof(MulReq), sizeof(MulReq));

      /* Remember the number of measurements (this is the limit) */
      NumMsrments = MulReq.NumMsrments;

      /* Get the number of measurements for this request */
      Status = iSdbCountMulReq(&MulReq);
      if(Status != SYS_NOMINAL)
      {
         eLogWarning(Status, "Problem determining number of measurements");
         iSdbErrReply(DelivererId, MsgPtr, Status);
         return Status;
      }

      /*
      ** Compare the number of measurements against the original
      ** If either the specified NumMsrments was set to zero (don't care)
      ** or there is less data than what we asked for, then set the
      ** "data-to-get" to the number of points that were returned as
      ** "available", otherwise we use the number that was requested
      ** by the "user".
      */
      if((NumMsrments == 0) || (MulReq.NumMsrments < NumMsrments))
      {
         NumMsrments = MulReq.NumMsrments;
      }

      /* Add the number of requests to the total measurement count (T) */
      TotNumMsrments += NumMsrments;
   }


   /* Calculate the return message size: N block headers and T measurements */
   OutBufSize = sizeof(NumReqs) + 
                NumReqs * ( sizeof(eSdbBlock_t) - sizeof(eSdbMsrment_t *) ) +
                TotNumMsrments * sizeof(eSdbMsrment_t);

   /* Check that we have sufficient space to send the return message */
   if(OutBufSize > I_SDB_DATASIZE)
   {
      Status = E_SDB_BUFFER_OVERFLOW;
      eLogErr(
         Status,
         "Too much data (%d bytes) for return buffer (%d byte) capacity",
         OutBufSize, I_SDB_DATASIZE
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Allocate a return CIL message buffer with enough space */
   /* ... or at least make use of the existing one! */
   OutBufPtr = TTL_MALLOC(OutBufSize);
   if(OutBufPtr == NULL)
   {
      Status = E_SDB_MALLOC_FAIL;
      eLogCrit(
         Status,
         "Failed to allocate memory (%d bytes) for reply block message",
         OutBufSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Make a copy of the pointer to the output buffer */
   MarkerPtr = OutBufPtr;

   /* Copy over the number of elements (expected to be returned) */
   memcpy(OutBufPtr, (char *)&NumReqs, sizeof(NumReqs));
   MarkerPtr += sizeof(NumReqs);


   /* Loop though each request again: */
   for(Req = 0; Req < NumReqs; Req++)
   {
      /* Extract the next multi-request */
      memcpy(&MulReq, InBufPtr+Req*sizeof(MulReq), sizeof(MulReq));

      /* Create a block for this request */
      Status = iSdbGetBlock(&MulReq, MarkerPtr, &BlockSize);
      if(Status != SYS_NOMINAL)
      {
         eLogWarning(Status, "Problem retrieving block data");
         iSdbErrReply(DelivererId, MsgPtr, Status);
         TTL_FREE(OutBufPtr);
         return Status;
      }

      /* Increment the pointer, to skip over the block */
      MarkerPtr += BlockSize;

   }


   /* Modify the CIL header, in the first instance, swap the src/dst */
   SwapAddr = MsgPtr->SourceId;
   MsgPtr->SourceId = MsgPtr->DestId;
   MsgPtr->DestId = SwapAddr;

   /* Change the message class */
   MsgPtr->Class = E_CIL_RSP_CLASS;

   /* Append the message data */
   MsgPtr->DataLen = OutBufSize;
   memcpy(MsgPtr->DataPtr, OutBufPtr, OutBufSize);


   /* Convert the entire message from hardware back to network byte order */
   eCilConvert32bitArray(MsgPtr->DataLen, MsgPtr->DataPtr);

   /* Actually send the message */
   Status = eCilSend(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Transmission failure");
   }


   /* Terminate function, indicating final success/fail of eCilSend() */
   TTL_FREE(OutBufPtr);
   return Status;


}  /* End of iSdbMulRetr() */



Status_t iSdbGetBlock(
   eSdbMulReq_t *MulReqPtr,
   char *BufPtr,
   size_t *BufLenPtr
)
{
/*
** Function Name:
**    iSdbGetBlock
**
** Type:
**    Status_t
**
** Purpose:
**    Puts a block of data into a character array.
**
** Description:
**    This function uses as specified multiple-request to put data
**    into a character array. It is assumed that sufficient memory has
**    already been allocated.
**
**    If the timestamps are set to zero, then the function will
**    return up to the specified number of measurements for the 
**    source/datum (starting with the most recent data).
**    This function only processes a single source/datum pair.
**
**    In more detail, this function will take a multiple-request
**    structure. This will contain a source ID, a datum ID and possibly
**    some timestamps. on input, the NumMsrments field will be irrelevant.
**    What this function will do is to look up the source/datum entry, and
**    create a sequence of data that is put into a character array.
**
** Arguments:
**    eSdbMulReq_t *MulReqPtr     (in)
**       A pointer to a Multi-request structure. The NumMsrments field
**       within this structure will indicate the maximum amount of data
**       to return.
**    char *BufPtr                (in)
**       A pointer to a character array that will contain the return 
**       message.
**    size_t &BufLenPtr           (in)
**       A pointer to a size of the returned block. This may be used to
**       advise the calling function how far to advance the pointer to
**       BufPtr, in order for a second call to start writing in the 
**       correct location.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    26-Jun-2000 djm Got working.
**    26-Jun-2000 djm Initial creation.
**
*/
   /* Local variables */
   Status_t Status;           /* Storage for returning error codes, etc. */
   iSdbDefn_t *DefnPtr;       /* Data element defn extracted from message */
   iSdbEvent_t *EventPtr;     /* Data element event extracted from message */
   char *NumMsrmentsFieldPtr; /* Temporary marker for the NumMsrments field */
   Bool_t TimeCheck;          /* Whether or not to check on time */
   Uint32_t NumMsrments;      /* Number of measurements recorded */



   /* Reset the length of the returned data */
   *BufLenPtr = 0;


   /* Firstly, see if the data already exists in our hash-table */
   DefnPtr = iSdbHashLookup(MulReqPtr->SourceId, MulReqPtr->DatumId);
   if(DefnPtr == NULL)
   {
      Status = E_SDB_UNKNOWN_DEFN;
      printf("\n");
      eLogInfo(
         "Source/datum pair (0x%x '%s', 0x%x) not in database"
         " (Block request)",
         MulReqPtr->SourceId, eCilNameString( MulReqPtr->SourceId ), 
         MulReqPtr->DatumId
      );
      return Status;
   }


   /*
   ** If we get this far, then we have a valid MsgDefnPtr
   ** from which to retrieve the data.
   */

   /* Setup the header */
   memcpy(BufPtr, &DefnPtr->SourceId, sizeof(DefnPtr->SourceId));
   BufPtr += sizeof(DefnPtr->SourceId);
   *BufLenPtr += sizeof(DefnPtr->SourceId);
   memcpy(BufPtr, &DefnPtr->DatumId, sizeof(DefnPtr->DatumId));
   BufPtr += sizeof(DefnPtr->DatumId);
   *BufLenPtr += sizeof(DefnPtr->DatumId);
   memcpy(BufPtr, &DefnPtr->Units, sizeof(DefnPtr->Units));
   BufPtr += sizeof(DefnPtr->Units);
   *BufLenPtr += sizeof(DefnPtr->Units);

   /* Memorize where the counter is, and initialise it to zero */
   NumMsrmentsFieldPtr = BufPtr;
   NumMsrments = 0;
   memcpy(BufPtr, BufLenPtr, sizeof(*BufLenPtr));
   BufPtr += sizeof(NumMsrments);
   *BufLenPtr += sizeof(NumMsrments);


   /* Get the pointer to the most recent data element */
   EventPtr = DefnPtr->NewestPtr;

   /* If there isn't one, then no data exists for this definition */
   if(EventPtr == NULL)
   {
      /* Return an empty block */
      return SYS_NOMINAL;
   }

   /* Set a flag indicating as default that we are not checking times */
   TimeCheck = FALSE;

   /* If any timestamp is not zero, skip over measurements until start */
   if(
      (MulReqPtr->NewestTime.t_sec != 0) ||
      (MulReqPtr->NewestTime.t_nsec != 0) ||
      (MulReqPtr->OldestTime.t_sec != 0) ||
      (MulReqPtr->OldestTime.t_nsec != 0)
   )
   {
      /* Save a flag for later use */
      TimeCheck = TRUE;

      /* Skip over measurements */
      for(
         ;
         EventPtr != NULL;
         EventPtr = EventPtr->PrevPtr
      )
      {
         /* If we within range, bail out */
         if(eTimCompare(&EventPtr->TimeStamp, &MulReqPtr->NewestTime) <= 0)
         {
            break;
         }

#if 0
         else
         {
            /* DEBUG SECTION ONLY */
            char Str[32];

            eTimToString(&EventPtr->TimeStamp, 32, Str);
            printf("Skipping... TS = %s, ", Str);
            eTimToString(&MulReqPtr->OldestTime, 32, Str);
            printf("OT = %s, ", Str);
            eTimToString(&MulReqPtr->NewestTime, 32, Str);
            printf("NT = %s\n", Str);

         }
#endif

      }
   }




   /* Loop getting data until we run out of data, exceed the number of */
   /* points to get or exceed the time range                           */
   for(
      ;
      EventPtr != NULL;
      EventPtr = EventPtr->PrevPtr
   )
   {

      /* If event time < oldest spec time), then bail out */
      if(TimeCheck == TRUE)
      {

         /* Check if the timestamp for this measurement lies within the range */
         if(eTimCompare(&EventPtr->TimeStamp, &MulReqPtr->OldestTime) <= 0)
         {
            break;
         }
      }

      /* Add the data */
      memcpy(BufPtr, &EventPtr->TimeStamp, sizeof(EventPtr->TimeStamp));
      BufPtr += sizeof(EventPtr->TimeStamp);
      memcpy(BufPtr, &EventPtr->Value, sizeof(EventPtr->Value));
      BufPtr += sizeof(EventPtr->Value);

      /* Increment the amount of data we have */
      NumMsrments++;
      *BufLenPtr += sizeof(eSdbMsrment_t);

      /* Check we have not reached capacity */
      if(NumMsrments >= MulReqPtr->NumMsrments)
      {
         break;
      }

   } 

   /* Copy the "number of measurements" into the buffer */
   memcpy(NumMsrmentsFieldPtr, &NumMsrments, sizeof(NumMsrments));

   /* Terminate function */
   return SYS_NOMINAL;


}  /* End of iSdbGetBlock() */


/* EOF */
