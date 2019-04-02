/*
** Module Name:
**    SdbRetrieve.c
**
** Purpose:
**    A module contains functions for getting data from the database.
**
** Description:
**    This module contains code for getting data from the database
**    maintained by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbRetrieve.c,v 0.16 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbRetrieve.c,v $
**    Revision 0.16  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.15  2006/06/27 16:35:27  mjf
**    Tidy up of logging - no functional changes.
**
**    Revision 0.14  2001/10/18 10:06:45  mjf
**    Log attempted retrieval of non-existent data at debug level.
**
**    Revision 0.13  2001/01/03 13:32:44  mjf
**    Addition of missing error replies.
**
**    Revision 0.12  2000/12/04 10:42:17  mjf
**    Enhanced log messages to include CIL name in addition to CIL ID.
**
**    Revision 0.11  2000/10/13 15:00:21  djm
**    Add code to create an empty data list (defn header only with no
**    measurement linked list). This will prevent multiple error messages
**    being reported in the event that a monitoring process (e.g. the ECI)
**    repeatedly requests data that has not yet been submitted. The warning
**    message about header existing (but no data) has been lowered in priority
**    to "info", to keep those messages from flooding the log under normal
**    operation (debug level 5).
**
**    Revision 0.10  2000/09/05 14:19:43  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.9  2000/08/29 14:36:18  djm
**    Removed spurious carraige-return in diagnostic messages.
**
**    Revision 0.8  2000/07/21 14:39:53  djm
**    Added RETRIEVE_1R service functionality. This is the ability for
**    the SDB to mark unavailable data as "INVALID", but otherwsie
**    carry on with processing the remainder of the request. This is
**    used for the ECI, where bulk data sets are requested.
**
**    Revision 0.7  2000/06/27 14:52:22  djm
**    Improve and centralise error reporting/registration.
**
**    Revision 0.6  2000/06/20 08:42:08  djm
**    Improved error messages.
**
**    Revision 0.5  2000/06/07 11:52:41  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.4  2000/05/26 07:59:39  djm
**    Minor change to error message reporting.
**
**    Revision 0.3  2000/05/18 13:20:38  djm
**    Implemented new task-data structure mechanism.
**
**    Revision 0.2  2000/05/17 17:00:28  djm
**    Added calls to facilitate Err replies to retrieve requests.
**
**    Revision 0.1  2000/05/17 15:02:50  djm
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
#include "Log.h"
#include "Cil.h"
#include "Sdb.h"
#include "SdbPrivate.h"


/* Function prototypes */

static Status_t mSdbGetData(Int32_t SourceId, eSdbDatum_t *DatumPtr);



/* Functions */


Status_t iSdbRetrieve(
   Bool_t Robust,
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbRetrieve
**
** Type:
**    Status_t
**
** Purpose:
**    Extract data from the SDB database into a CIL message.
**
** Description:
**    Fills a CIL message with data from the status database.
**    This function assumes that the message has been validated
**    by a higher level. In particular, the message has been checked
**    for valid source, correct destination (namely this program!),
**    and that it is a "command class" message.
**
** Arguments:
**    Bool_t Robust          (in)
**       This flag will control the behaviour of the function in the
**       event that requested data is not in the SDB. If "Robust" is
**       true, the SDB will flag the returned data as "not-found" (by
**       setting "invalid" in the units field, and carry on to the
**       next datum request. This puts the onus on the requesting
**       process to sort out if there is any "missing" data. Note that
**       only the units field is adjusted; the Source/Datum ID numbers
**       are preserved to allow the requesting process to identify
**       the missing data.
**    Int32_t DelivererI    (in)
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
**    05-Sep-2000 djm Added DelivererId for correct message response.
**    20-Jun-2000 djm Added better error reporting.
**    16-May-2000 djm Changed for new export data structures.
**    15-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumElts;         /* Number of data elements in message */
   Uint32_t TmpNumElts;      /* Temp var w/ No. data elements in message */
   Uint32_t Elt;             /* Element counter */
   eSdbSngReq_t SngReq;      /* Data element request extracted from message */
   eSdbDatum_t Datum;        /* Data element to send back */
   char *InBufPtr;           /* Temporary pointer to data block of message */
   char *OutBufPtr;          /* Pointer to output buffer */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */
   size_t InBufSize;         /* Memory size to allocate for buffer */
   size_t OutBufSize;        /* Memory size to allocate for buffer */



   /* Check message length */
   if(MsgPtr->DataLen < sizeof(NumElts))
   {
      Status = E_SDB_TRUNCATED;
      eLogErr(
         Status,
         "Retrieve request from source 0x%x '%s' truncated (%d bytes only)",
         MsgPtr->SourceId, eCilNameString( MsgPtr->SourceId ), MsgPtr->DataLen
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Assign a temporary pointer to the data block of the message */
   InBufPtr = (char *)(MsgPtr->DataPtr);

   /* Extract the number of data to be retrieved from the message */
   memcpy(&NumElts, InBufPtr, sizeof(NumElts));
   TmpNumElts = NumElts;  /* <-- Remember network ordered version for later */
   NumElts = ntohl(NumElts);
   InBufPtr += sizeof(NumElts);


   /* Check the message length against the calculated message length */
   /* based on the number of contained data element requests */
   InBufSize = sizeof(NumElts) + (NumElts * sizeof(eSdbSngReq_t));
   if(MsgPtr->DataLen != InBufSize)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr(
          Status,
          "Retrieve request from source 0x%x '%s' truncated "
          "(%d of %d expected data bytes received)",
          MsgPtr->SourceId, eCilNameString( MsgPtr->SourceId ), 
          MsgPtr->DataLen, InBufSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Allocate some space for the output message */
   OutBufSize = sizeof(NumElts) + NumElts * sizeof(eSdbDatum_t);
   OutBufPtr = TTL_MALLOC(OutBufSize);
   if(OutBufPtr == NULL)
   {
      Status = E_SDB_MALLOC_FAIL;
      eLogCrit(
         Status,
         "Failed to allocate memory (%d bytes) for reply message",
         OutBufSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Copy over the number of elements (expected to be returned) */
   memcpy(OutBufPtr, (char *)&TmpNumElts, sizeof(TmpNumElts));



   /* Loop over all data elements until they have all been read in */
   for(Elt = 0; Elt < NumElts; Elt++)
   {

      /* Extract all the components of one data element */
      memcpy(&SngReq, InBufPtr, sizeof(SngReq));
      InBufPtr += sizeof(SngReq);

      /* Convert from network to hardware byte order */
      SngReq.SourceId = ntohl(SngReq.SourceId);
      SngReq.DatumId = ntohl(SngReq.DatumId);

      /* Start building the datum to be sent back */
      Datum.SourceId = SngReq.SourceId;
      Datum.DatumId = SngReq.DatumId;

      /* Use a call to extract other data from the hash-table */
      Status = mSdbGetData(MsgPtr->SourceId, &Datum);
      if(Status != SYS_NOMINAL)
      {
         /*
         ** This block handles the case of "missing" data in the SDB.
         ** Depending on whether or not we are operating in "robust"
         ** mode we may either:
         **    a) bail out immediately and return an error to the 
         **       calling process, or,
         **    b) set the units flag to "invalid" and carry on. The
         **       user must then look for the invalid units field
         **       in the data stream to trap the error.
         */
         if(Robust == TRUE)
         {
            Datum.Units = E_SDB_INVALID_UNITS;
         }
         else
         {
            TTL_FREE(OutBufPtr);          /* Free up our output buffer */
            iSdbErrReply(DelivererId, MsgPtr, Status);
            return Status;
         }
      }

      /* Reconvert into network byte-order */
      Datum.SourceId = htonl(Datum.SourceId);
      Datum.DatumId = htonl(Datum.DatumId);
      Datum.Units = htonl(Datum.Units);
      Datum.Msrment.Value = htonl(Datum.Msrment.Value);
      Datum.Msrment.TimeStamp.t_sec = htonl(Datum.Msrment.TimeStamp.t_sec);
      Datum.Msrment.TimeStamp.t_nsec = htonl(Datum.Msrment.TimeStamp.t_nsec);

      /* Put the data into the transmit buffer */
      memcpy((char *)(OutBufPtr+Elt*sizeof(Datum)+sizeof(NumElts)),
             (char *)&Datum, sizeof(Datum));


   }  /* End for(`each element') */

   /*
   ** Send the CIL message reply
   */


   /* Modify the CIL header, in the first instance, swap the src/dst */
   SwapAddr = MsgPtr->SourceId;
   MsgPtr->SourceId = MsgPtr->DestId;
   MsgPtr->DestId = SwapAddr;

   /* Change the message class */
   MsgPtr->Class = E_CIL_RSP_CLASS;

   /* Append the message data */
   MsgPtr->DataLen = OutBufSize;
   memcpy(MsgPtr->DataPtr, OutBufPtr, OutBufSize);

   /* Actually send the message */
   Status = eCilSend(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Transmission failure");
      return Status;
   }

   /* Get rid of temporary data buffer */
   TTL_FREE(OutBufPtr);

   /* Terminate the function, reporting success */
   return SYS_NOMINAL;


} /* End of iSdbRetrieve() */






Status_t mSdbGetData(
   Int32_t SourceId,
   eSdbDatum_t *DatumPtr
)
{
/*
** Function Name:
**    iSdbGetData
**
** Type:
**    Status_t
**
** Purpose:
**    Get data from the hash-table/linked-list
**
** Description:
**    This function only returns the latest data from the list.
**
** Arguments:
**    Int32_t SourceId       (in)
**      CIL ID of the task that has requested the retrieval of the
**      data from the database. This if for reporting errors only.
**    eSdbDatum_t *DatumPtr  (in)
**       ...
**    
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    12-May-2000 djm Added datum storage.
**    11-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;           /* Storage for returning error codes, etc. */
   iSdbDefn_t *DefnPtr;       /* Data element defn extracted from message */
   iSdbEvent_t *EventPtr;     /* Data element event extracted from message */


   /* Firstly, see if the data already exists in our hash-table */
   DefnPtr = iSdbHashLookup(DatumPtr->SourceId, DatumPtr->DatumId);
   if(DefnPtr == NULL)
   {
      Status = E_SDB_UNKNOWN_DEFN;
      eLogInfo(
         "Source/datum pair (0x%x '%s', 0x%x) not in database "
         "(requested by 0x%x '%s')",
         DatumPtr->SourceId, eCilNameString( DatumPtr->SourceId ), 
         DatumPtr->DatumId, SourceId, eCilNameString( SourceId )
      );

      /*
      ** We now set up a single hash entry. This will prevent this error
      ** occurring over and over for subsequent submissions. Note that this
      ** setup will not require writing to file.
      */

      DefnPtr = iSdbHashInstall(DatumPtr->SourceId, DatumPtr->DatumId);
      if(DefnPtr == NULL)
      {
         eLogCrit(
            Status, "Error creating holding hash-entry (%x '%s', %x)",
            DatumPtr->SourceId, eCilNameString( DatumPtr->SourceId ), 
            DatumPtr->DatumId
         );
      }

      return Status;
   }


   /*
   ** If we get this far, then we have a valid MsgDefnPtr 
   ** from which to retrieve the data.
   */

   /* Get the pointer to the most recent data element */
   EventPtr = DefnPtr->NewestPtr;
   if(EventPtr == NULL)
   {
      Status = E_SDB_NO_VALUES;
      eLogReport(
         E_LOG_DEBUG, Status, 
         "Data definition for (0x%x '%s', 0x%x) available, "
         "but no values present",
         DatumPtr->SourceId, eCilNameString( DatumPtr->SourceId ), 
         DatumPtr->DatumId
      );
      return Status;
   }

   /* Fill out the MsgDatum structure with all required */
   DatumPtr->Units = DefnPtr->Units;
   memcpy(&(DatumPtr->Msrment.TimeStamp),
      &(EventPtr->TimeStamp), sizeof(DatumPtr->Msrment.TimeStamp));
   DatumPtr->Msrment.Value = EventPtr->Value;


   /* Print some diagnostics */

/*
printf("*** DEFINITION ***\n");
printf("  SourceID=%d\n", DefnPtr->SourceId); 
printf("   DatumID=%d\n", DefnPtr->DatumId); 
printf("      Type=%d\n", DefnPtr->Type); 
printf("     Units=%d\n", DefnPtr->Units); 
printf("   NumData=%d\n", DefnPtr->NumData); 
printf("*** LATEST DATA ***\n");
printf("     Value=%d\n", DatumPtr->Msrment.Value);
printf(" Timestamp=%d.%9.9d\n",
   DatumPtr->Msrment.TimeStamp.t_sec, DatumPtr->Msrment.TimeStamp.t_nsec);
*/

   /* Return sucess, if we manage to get this far */
   return SYS_NOMINAL;

}  /* End of iSdbGetData() */


/* EOF */
