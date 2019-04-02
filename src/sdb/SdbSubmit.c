/*
** Module Name:
**    SdbSubmit.c
**
** Purpose:
**    A module contains functions for adding data to the database.
**
** Description:
**    This module contains code for adding data to the database
**    maintained by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbSubmit.c,v 0.23 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbSubmit.c,v $
**    Revision 0.23  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.22  2002/01/10 18:48:16  mjf
**    Addition of log messages on a process (other than CHB or MCP) submitting
**    its own proc-state.
**
**    Revision 0.21  2000/12/09 14:58:07  mjf
**    Addition of SUBMIT_1P service - post a submission expecting no reply.
**
**    Revision 0.20  2000/11/21 16:43:58  djm
**    Added call to iSdbStorePrevData() in order to get archive history
**    correct for slowly/occasionally changing data.
**
**    Revision 0.19  2000/11/03 16:07:01  djm
**    Always append new data to the end of the list. This prevents an
**    erroneous point from becoming "permanently" served as the latest
**    datum.
**
**    Revision 0.18  2000/10/16 14:07:40  djm
**    Added increment on global varaible to count failures that
**    occur when trying to write to file.
**
**    Revision 0.17  2000/09/27 11:51:36  djm
**    Added call to store the units to file.
**
**    Revision 0.16  2000/09/08 14:30:28  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.15  2000/09/05 14:20:14  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.14  2000/08/31 15:37:45  djm
**    Modified so as not to write data to file if the SDB is
**    in SAFE state.
**
**    Revision 0.13  2000/07/21 14:42:23  djm
**    Added a #ifdef to allow file storage to be toggled on/off from
**    outside the file. This may need to be replaced with a global
**    variable at some stage, if required. The #ifdef symbolic constant
**    is defined in SdbPrivate.h at this stage.
**
**    Revision 0.12  2000/07/11 17:38:11  djm
**    Added code to record the latest "event" as part of the SDB's
**    data definition structure. This is required by the storage
**    file call, which needs to record the value that has just been
**    submitted to the SDB.
**
**    Revision 0.11  2000/07/10 16:56:20  djm
**    Added call to data storage function (iSdbStore).
**
**    Revision 0.10  2000/06/27 14:52:11  djm
**    Improve and centralise error reporting/registration.
**
**    Revision 0.9  2000/06/07 11:52:55  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.8  2000/05/31 10:51:36  djm
**    Implemented time-sorting for data being added to database linked-list.
**    Now, on submission of data, it will be added into the database linked
**    list in the correct time sequence.
**
**    Revision 0.7  2000/05/31 08:57:11  djm
**    Removed hard-coded linked list manipulation and replaced it with
**    calls the new linked list control functions (from SdbLnkLst.c).
**
**    Revision 0.6  2000/05/18 14:41:54  djm
**    Made mSdbAddData() globally available within the SDB package
**    as iSdbAddData().
**
**    Revision 0.5  2000/05/18 13:21:27  djm
**    Implemented new task-data structure mechanism.
**
**    Revision 0.4  2000/05/17 16:59:13  djm
**    ÿ¡Added calls to facilitate Ack/Err replies to submission requests.
**
**    Revision 0.3  2000/05/17 09:51:09  djm
**    Changes to names/types to match new organisation
**    in SDB header files.
**
**    Revision 0.2  2000/05/12 15:21:36  djm
**    Added ability to take in a limited number of data for each data
**    element definition. The limit as to how many data may be
**    held in memory for each different type of data is determined
**    by I_SDB_HIST_LIMIT (defined in SdbPrivate.h).
**
**    Revision 0.1  2000/05/12 08:57:27  djm
**    Intial creation.
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
#include "Tim.h"
#include "Log.h"
#include "Sdb.h"
#include "SdbPrivate.h"




/* Functions */


Status_t iSdbSubmit(
   Int32_t DelivererId,
   Bool_t  RespondFlag,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbSubmit
**
** Type:
**    Status_t
**
** Purpose:
**    Extract data from a CIL message into the SDB database.
**
** Description:
**    Extracts all relevant data from an incoming CIL message and
**    puts it into the SDB's linked list for data storage.
**    This function assumes that the message has been validated
**    by a higher level. In particular, the message has been checked
**    for valid source, correct destination (namely this program!),
**    and that it is a "command class" message.
**
** Arguments:
**    Int32_t DelivererId    (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr) to the SDB.
**    Bool_t RespondFlag     (in)
**       Flag set TRUE if a response (success or fail) is to be sent to the
**       submitting process.
**    eCilMsg_t *MsgPtr      (in/out)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing. The contents of this structure may be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    09-Dec-2000 mjf Addition of flag denoting whether to respond.
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    07-Jun-2000 djm Added LOG functions for error reporting.
**    15-May-2000 djm Change names/types to match header file changes.
**    12-May-2000 djm Minor bug-fixes.
**    11-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumElts;         /* Number of data elements in message */
   Uint32_t Elt;             /* Element counter */
   eSdbDatum_t Datum;        /* Data element extracted from message */
   char *BufPtr;             /* Temporary pointer to data block of message */



   /* Check message length */
   if(MsgPtr->DataLen < sizeof(NumElts))
   {
      Status = E_SDB_TRUNCATED;
      eLogErr(
         Status,
         "Submit message truncated (%d bytes only)",
         MsgPtr->DataLen
      );
      if (RespondFlag == TRUE)
      {
         iSdbErrReply(DelivererId, MsgPtr, Status);
      }
      return Status;
   }

   /* Assign a temporary pointer to the data block of the message */
   BufPtr = (char *)(MsgPtr->DataPtr);


   /* Extract the number of data submitted from the message */
   memcpy(&NumElts, BufPtr, sizeof(NumElts));
   NumElts = ntohl(NumElts);
   BufPtr += sizeof(NumElts);


   /* Check the message length against the calculated message length */
   /* based on the number of contained data elements */
   if(MsgPtr->DataLen < (sizeof(NumElts) + (NumElts * sizeof(eSdbDatum_t))) )
   {
      eLogErr(
          Status,
          "Submit data truncated (%d of %d expected data bytes received)",
          (MsgPtr->DataLen - sizeof(NumElts)),
          (NumElts * sizeof(eSdbDatum_t))
      );
      if (RespondFlag == TRUE)
      {
         iSdbErrReply(DelivererId, MsgPtr, Status);
      }
      return Status;
   }   

   /* Loop over all data elements until they have all been read in */
   for(Elt = 0; Elt < NumElts; Elt++)
   {

      /* Extract all the components of one data element */
      memcpy(&Datum, BufPtr, sizeof(Datum));
      BufPtr += sizeof(Datum);

      /* Convert from network to hardware byte order */
      Datum.SourceId = ntohl(Datum.SourceId);
      Datum.DatumId = ntohl(Datum.DatumId);
      Datum.Units = ntohl(Datum.Units);
      Datum.Msrment.Value = ntohl(Datum.Msrment.Value);
      Datum.Msrment.TimeStamp.t_sec = ntohl(Datum.Msrment.TimeStamp.t_sec);
      Datum.Msrment.TimeStamp.t_nsec = ntohl(Datum.Msrment.TimeStamp.t_nsec);

      /* Print out some diagnostics */
      eLogDebug("----------------------------------------------");
      eLogDebug("Buf   = %u", (size_t)BufPtr);
/*
      {
      int n;
      for(n = 0; n < sizeof(eSdbDatum_t); n++)
         printf("%2.2x ", BufPtr[n-sizeof(eSdbDatum_t)]);
      }
      printf("\n");
*/
      eLogDebug("SrcID = %x", Datum.SourceId);
      eLogDebug("DatID = %x", Datum.DatumId);
      eLogDebug("Units = %x", Datum.Units);
      eLogDebug("TS.s  = %x", Datum.Msrment.TimeStamp.t_sec);
      eLogDebug("TS.ns = %x", Datum.Msrment.TimeStamp.t_nsec);
      eLogDebug("Value = %x", Datum.Msrment.Value);
      eLogDebug("----------------------------------------------");

      /* Temp log message to trap processes submitting their own proc-state */
      if ( Datum.DatumId == D_MCP_PROC_STATE )
      {
         if ( ( DelivererId == Datum.SourceId ) 
               && ( Datum.SourceId != E_CIL_CHB )
               && ( Datum.SourceId != E_CIL_MCP ) )
         {
            if ( ( DelivererId > E_CIL_BOL ) && ( DelivererId < E_CIL_EOL ) )
            {
               eLogNotice( 0, "Process '%s' submitted its own process state",
                           eCilNameString( DelivererId ) );
            }
         }
      }


      /* Put the data into the database linked-list/hash-table */
      Status = iSdbAddData(&Datum);
      if(Status != SYS_NOMINAL)
      {
         eLogErr(Status, "Failed to add data");
         if (RespondFlag == TRUE)
         {
            iSdbErrReply(DelivererId, MsgPtr, Status);
         }
         return Status;
      }

   }  /* End for(`each element') */

   /* If we get this far, then all the data was successfully submitted */

   /* Attempt to report this success to the submitting task */
   if (RespondFlag == TRUE)
   {
      Status = iSdbAckReply(DelivererId, MsgPtr);
      if(Status != SYS_NOMINAL)
      {
         eLogWarning(Status, "Error acknowledging successful submission");
         return Status;
      }
   }


   /* Terminate the function, reporting success */
   return SYS_NOMINAL;


} /* End of iSdbSubmit() */






Status_t iSdbAddData(
   eSdbDatum_t *DatumPtr
)
{
/*
** Function Name:
**    iSdbAddData
**
** Type:
**    Status_t
**
** Purpose:
**    Add/update data in the hash-table/linked-list
**
** Description:
**    ...
**
** Arguments:
**    eSdbDatum_t *DatumPtr         (in)
**       ...
**    
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    07-Jul-2000 djm Slight change for data reporting.
**    06-Jul-2000 djm Added call to write data to file.
**    07-Jun-2000 djm Added LOG functions for error reporting.
**    15-May-2000 djm Change names/types to match header file changes.
**    12-May-2000 djm Added datum storage.
**    11-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;           /* Function return variable */
   iSdbDefn_t *DefnPtr;       /* Data element defn extracted from message */
   iSdbEvent_t *EventPtr;     /* Data element event extracted from message */

#ifdef M_SDB_SORTED_LIST_INSERTION
   iSdbEvent_t *TmpEventPtr;  /* Temp ptr to an event (pseudo-loop counter) */
   Bool_t InsertFlag;         /* Flag indicating that an event has been added */
#endif



   /* Firstly, see if the data already exists in our hash-table */
   DefnPtr = iSdbHashLookup(DatumPtr->SourceId, DatumPtr->DatumId);
   if(DefnPtr == NULL)
   {
      /* Print a diagnostic message saying that a new entry has been added */
      eLogInfo(
         "Creating a new hash-table entry (%x,%x)", 
         DatumPtr->SourceId, DatumPtr->DatumId
      );

      /* We don't have this one yet. Create a new hash-table entry for it */
      DefnPtr = iSdbHashInstall(DatumPtr->SourceId, DatumPtr->DatumId);
      if(DefnPtr == NULL)
      {
         /* Failed to create new hash-table entry. Bail out. */
         Status = E_SDB_GEN_ERR;
         eLogErr(Status, "Failed to create new hash-table entry");
         return Status;
      }

   }

   /*
   ** If we get this far, then we will have a valid MsgDefnPtr in which
   ** to insert the new data we have received.
   */

   /* Add data */

   /* Update any header details */
   DefnPtr->Units = DatumPtr->Units;


   /* Create a new SdbDatum (not MsgDatum) */
   EventPtr = (iSdbEvent_t *) TTL_MALLOC(sizeof(iSdbEvent_t));
   if(EventPtr == NULL)
   {
      Status = E_SDB_MALLOC_FAIL;
      eLogCrit(Status, "Failed to allocate memory for new datum");
      return Status;
   }
   

   /* Copy in the details and initialise the pointers to other structures */
   memcpy(&(EventPtr->TimeStamp),
      &(DatumPtr->Msrment.TimeStamp), sizeof(DatumPtr->Msrment.TimeStamp));
   EventPtr->Value = DatumPtr->Msrment.Value;
   EventPtr->NextPtr = NULL;
   EventPtr->PrevPtr = NULL;


   /*
   ** Create the relevant links.
   */

   /* If we are about to reach a history limit, incr. "limited" counter */
   if((DefnPtr->NumData+1) == I_SDB_HIST_LIMIT)
   {
      iSdbTaskData[D_SDB_QTY_HISTLIM].Value++;
   }

   /* Check that we haven't exceeded our history limit (E_SDB_HIST_LIMIT) */
   /* If we have, then delete the oldest data point we have */
   while(DefnPtr->NumData >= I_SDB_HIST_LIMIT)
   {
      /* Remove the oldest item in the linked list */
      Status = iSdbLnkLstClip(DefnPtr);
      if(Status != SYS_NOMINAL) return Status;
   }

   /* Note the new datum as the "latest" one we've received (this is */
   /* used for storage of the data to file, which is done later) */
   DefnPtr->LastSubPtr = EventPtr;

   /* Is it the first one to be received? */
   if(DefnPtr->NewestPtr == NULL)
   {
      /* ... if YES, then create a new entry */
      Status = iSdbLnkLstCreate(DefnPtr, EventPtr);
      if(Status != SYS_NOMINAL)
      {
         eLogErr(Status, "Error trying to create new linked-list entry");
         return Status;
      }
   }
   else
   {

#ifdef M_SDB_SORTED_LIST_INSERTION

      /* If no, sort out where the new entry needs to be placed in the list */

      /* First check if it goes at the start of the list */
      if(eTimCompare(&EventPtr->TimeStamp, &DefnPtr->NewestPtr->TimeStamp) >= 0)
      {
         Status = iSdbLnkLstAppend(DefnPtr, EventPtr);
         if(Status != SYS_NOMINAL)
         {
            eLogErr(Status, "Error trying to append new linked-list entry");
            return Status;
         }
      }
      else
      {
         /* Otherwise, we go through the list looking for a suitable place */
         InsertFlag = FALSE;
         for(
            TmpEventPtr = DefnPtr->NewestPtr->PrevPtr;
            TmpEventPtr != NULL;
            TmpEventPtr = TmpEventPtr->PrevPtr
         )
         {
            if(eTimCompare(&EventPtr->TimeStamp, &TmpEventPtr->TimeStamp) >= 0)
            {
               Status = iSdbLnkLstInsert(DefnPtr, TmpEventPtr, EventPtr);
               if(Status != SYS_NOMINAL)
               {
                  eLogErr(
                     Status,
                     "Error trying to insert new linked-list entry"
                  );
                  return Status;
               }
               InsertFlag = TRUE;
               break;
            }
         }  /* End of for() */

         /*If we get to the end of this without adding the data, put it at end */
         if(InsertFlag != TRUE)
         {
            Status = iSdbLnkLstPrepend(DefnPtr, EventPtr);
            if(Status != SYS_NOMINAL)
            {
               eLogErr(Status, "Error trying to prepend new linked-list entry");
               return Status;
            }
         }

      }  /* End of if(later than any existing data) */

#else

      /* Append the data to the end of the list */
      Status = iSdbLnkLstAppend(DefnPtr, EventPtr);
      if(Status != SYS_NOMINAL)
      {
         eLogErr(Status, "Error trying to append new linked-list entry");
         return Status;
      }

#endif


   }  /* End of if(first received) */


   /*
   ** Put the data into the SDB's storage files.
   */

#ifdef I_SDB_SFR_USE

   /* Firstly, only hit the files if we are in "OKAY" state */
   if(iSdbNominalState == SYS_OKAY_STATE)
   {
      /* Then, only do it, if we actually want file storage */
      if(iSdbFileStore == TRUE)
      {

         /*
         ** Increment the error counter. If we get to the end of this section
         ** then we'll reset it to zero. Otherwise, it'll remain incremented.
         ** If this happens enough, the variable will continue to increment,
         ** exceed a threshold and then it will be flagged as an error/warning.
         */
         iSdbNumWriteFails++;

         /* The first part is to record the units */
         Status = iSdbStoreUnits(DefnPtr);
         if(Status != SYS_NOMINAL)
         {
            eLogErr(Status, "Failed to write units data to file");
            return Status;
         }

         /* Dump the previous value, if necessary */
         Status = iSdbStorePrevData(DefnPtr);
         if(Status != SYS_NOMINAL)
         {
            eLogErr(Status, "Failed to write previous data to file");
            return Status;
         }

         /* Finally, actually dump the data itself */
         Status = iSdbStoreData(FALSE, DefnPtr);
         if(Status != SYS_NOMINAL)
         {
            eLogErr(Status, "Failed to write data to file");
            return Status;
         }
      }

      /* If we get here without returning an error, reset the failure counter */
      iSdbNumWriteFails = 0;
   }
#endif

   /* Print some diagnostics */
   eLogDebug("*** DEFINITION ***");
   eLogDebug("  SourceID=%d", DefnPtr->SourceId); 
   eLogDebug("   DatumID=%d", DefnPtr->DatumId); 
   eLogDebug("     Units=%d", DefnPtr->Units); 
   eLogDebug("   NumData=%d", DefnPtr->NumData); 
   if(EventPtr != NULL)
   {
      eLogDebug("*** LATEST DATA ***");
      eLogDebug("     Value=%d", DefnPtr->NewestPtr->Value);
      eLogDebug(" Timestamp=%d.%9.9d",
                DefnPtr->NewestPtr->TimeStamp.t_sec, 
                DefnPtr->NewestPtr->TimeStamp.t_nsec);
   }


   /* Return sucess, if we manage to get this far */
   return SYS_NOMINAL;

}  /* End of iSdbAddData() */


/* EOF */
