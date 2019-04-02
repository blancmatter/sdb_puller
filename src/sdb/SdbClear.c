/*
** Module Name:
**    SdbClear.c
**
** Purpose:
**    Functions for clearing data out of the database
**
** Description:
**    This module contains code for clearing parts of the 
**    database maintained by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    mjf: Martyn J. Ford (TTL)
**
** Version:
**    $Id: SdbClear.c,v 0.5 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbClear.c,v $
**    Revision 0.5  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.4  2002/01/10 17:41:02  mjf
**    Upon a requested 'clear', ensure the last submission has been archived
**    to disk, even if the value is the same as the last archived one.
**
**    Revision 0.3  2001/10/18 10:07:20  mjf
**    Tidying of debug levels for log messages.
**
**    Revision 0.2  2001/10/12 11:24:58  mjf
**    Addition of a 'CLEAR_1' service, which will allow the originator of the
**    message to selectively clear their own datum entries. The message takes
**    the same form as the 'CLEAR_S' service, but the data to clear are
**    specified rather than the data to be perserved.
**
**    Revision 0.1  2001/09/14 11:03:05  djm
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



/* Local function prototypes */


/* Functions */


Status_t iSdbClearSource(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbClearSource
**
** Type:
**    Status_t
**
** Purpose:
**    Clears all data from a specified source.
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
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumData;         /* Number of Datum IDs  */
   int Index;                /* Index into the hash-table array */
   int n;                    /* inner loop counter */
   Int32_t DatList[I_SDB_MAXIDS];        /* List of Datum IDs to ignore */
   iSdbDefn_t *DefnPtr;      /* Data element definiton from hash-table */
   char *BufPtr;             /* Temporary pointer to data block of message */
   Int32_t SourceId;         /* Source ID of data to be cleared */
   iSdbEvent_t *EventPtr;
   iSdbEvent_t *TmpEventPtr;
   size_t ExpectedSize;
   Bool_t Found;
   int ClearCount = 0;

   /* Initialise status return variable */
   Status = SYS_ERROR_NON_FATAL;

   /* Initialise array of DatumIDs to ignore during clear */
   NumData = 0;
   for(n = 0; n < I_SDB_MAXIDS; n++)
   {
      DatList[n] = -1;
   }

   /* Check message length */
   ExpectedSize = sizeof(SourceId) + sizeof(NumData);
   if(MsgPtr->DataLen < ExpectedSize)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr
      (
        Status,
        "ClearSource message requires at least a sepcified source "
        "(%d bytes, %d expected)",
         MsgPtr->DataLen,
         ExpectedSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Assign a temporary pointer to the data block of the message */
   BufPtr = (char *)(MsgPtr->DataPtr);

   /* Extract the SourceID to be cleared */
   memcpy(&SourceId, BufPtr, sizeof(SourceId));
   SourceId = ntohl(SourceId);
   BufPtr += sizeof(SourceId);

   /* Check that we are premitted to clear data from this source */
   if((SourceId != MsgPtr->SourceId) && (MsgPtr->SourceId != E_CIL_MCP))
   {
      Status = E_SDB_NOT_AUTH;
      eLogErr
      (
        Status,
        "%s not permitted to clear source data of %s",
        eCilNameString(MsgPtr->SourceId), eCilNameString(SourceId)
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Extract the number of data submitted from the message */
   memcpy(&NumData, BufPtr, sizeof(NumData));
   NumData = ntohl(NumData);
   BufPtr += sizeof(NumData);
   eLogDebug("Ignoring %d data", NumData);

   /* Now that we know the number of Datum IDs that we will be ignoring, */
   /* we will check again (more carefully) the size of the message */
   ExpectedSize = sizeof(SourceId) + sizeof(NumData) + NumData * sizeof(Int32_t);
   if(MsgPtr->DataLen != ExpectedSize)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr
      (
        Status,
        "ClearSource message not of expected size "
        "(%d bytes, %d expected)",
         MsgPtr->DataLen,
         ExpectedSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Extract list of Datum IDs which'll be ignored,even if SourceID matches */
   for(n = 0; n < NumData; n++)
   {
      /* Extract all the components of one data element */
      memcpy(&DatList[n], BufPtr, sizeof(DatList[n]));
      DatList[n] = ntohl(DatList[n]);
      BufPtr += sizeof(DatList[n]);
   }

   for(n = 0; n < I_SDB_MAXIDS; n++)
   {
      if(DatList[n] != -1)
      {
         eLogDebug("Datum to ignore = 0x%x", DatList[n]);
      }
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

         /* Ignore dummy entries */
         if(DefnPtr->NewestPtr == NULL) continue;

         /* Ignore irrelevant sources */
         if(DefnPtr->SourceId != SourceId) continue;

         /* Ignore any specified Data */
         Found = FALSE;
         for(n = 0; n < I_SDB_MAXIDS; n++)
         {
            if(DatList[n] == DefnPtr->DatumId)
            {
               Found = TRUE;
               break;
            }
         }

         if(Found == TRUE) continue;

         /* If we get this far, then we are okay to clear the data */
         eLogInfo("Clearing data (%s,0x%x)",
            eCilNameString(DefnPtr->SourceId), DefnPtr->DatumId);
         ClearCount++;

         /* Ensure datum is flushed to disk (if writing to disk & not safe) */
         if ( ( iSdbFileStore == TRUE ) 
               && ( iSdbNominalState != SYS_SAFE_STATE ) )
         {
            /* If the last measurement has not been written to disk */
            if ( DefnPtr->ValueRecorded == FALSE )
            {
               /* Store the measurement, forcing a write to disk */
               iSdbStoreData( TRUE, DefnPtr );
            }
         }

         /* Wind through to the end of the data */
         EventPtr = DefnPtr->NewestPtr;
         while(EventPtr->NextPtr != NULL)
         {
            eLogDebug("Winding forward (%s,0x%x)",
               eCilNameString(DefnPtr->SourceId), DefnPtr->DatumId);
            EventPtr = EventPtr->NextPtr;
         }

         if(DefnPtr->NumData >= I_SDB_HIST_LIMIT)
         {
            iSdbTaskData[D_SDB_QTY_HISTLIM].Value--;
         }

         /* Backtrack through the data, deleting as we go */
         while(EventPtr != NULL)
         {
            eLogDebug("Clearing event from (%s,0x%x)",
               eCilNameString(DefnPtr->SourceId), DefnPtr->DatumId);
            TmpEventPtr = EventPtr->PrevPtr;
            TTL_FREE(EventPtr);
            EventPtr = TmpEventPtr;
            iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value--;
            DefnPtr->NumData--;
         }
         DefnPtr->NewestPtr = NULL;
         DefnPtr->OldestPtr = NULL;
         DefnPtr->LastSubPtr = NULL;
         DefnPtr->ValueRecorded = FALSE;
         if(DefnPtr->NumData != 0)
         {
            Status = SYS_ERROR_NON_FATAL;
            eLogErr(Status, "Error clearing data (possible memory leak)"
               " NumData = %d", DefnPtr->NumData);
         }

      }  /* End of loop hash-entries at a particular index */

   }  /* End of loop over the entire hash-table */


   /* Print a diagnostic */
   if ( ClearCount > 0 )
   {
      eLogInfo("Clearing operation completed");
   }
   Status = SYS_NOMINAL;


   if(Status != SYS_NOMINAL)
   {
      eLogErr(Status, "Failed to clear data for source %s",
         eCilNameString(SourceId));
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Attempt to report this success to the submitting task */
   Status = iSdbAckReply(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogWarning(Status, "Error acknowledging successful clear operation");
      return Status;
   }

   return Status;

}  /* End of iSdbClearSource() */


/*******************************************************************************
**
** Function Name:
**    iSdbClearSource
**
** Type:
**    Status_t    
**       Returns the completion status of this function.
**
** Purpose:
**    Clears specified data entries from the database.
**
** Description:
**    This function will be called by a client when the client decides that it
**    is not possible to submit valid data into the SDB. It enables consumers 
**    of the data to subsequently determine that no data is available, rather
**    than stale data be available.
**
** Arguments:
**    Int32_t DelivererId              (in)
**       CIL ID code (definitions in Cil.h) of the process that sent the CIL 
**       message (MsgPtr) to the SDB.
**    eCilMsg_t *MsgPtr                (in/out)
**       A pointer to a CIL message sent to the SDB, that requires processing. 
**       The contents of this structure will be changed.
**
** Authors:
**    djm : Derek J. McKay (TTL)
**    mjf : Martyn J. Ford (TTL)
**
*******************************************************************************/

Status_t iSdbClearData( Int32_t DelivererId, eCilMsg_t *MsgPtr )
{
   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Uint32_t NumData;         /* Number of Datum IDs  */
   int Index;                /* Index into the hash-table array */
   int n;                    /* inner loop counter */
   Int32_t DatList[I_SDB_MAXIDS];        /* List of Datum IDs to ignore */
   iSdbDefn_t *DefnPtr;      /* Data element definiton from hash-table */
   char *BufPtr;             /* Temporary pointer to data block of message */
   Int32_t SourceId;         /* Source ID of data to be cleared */
   iSdbEvent_t *EventPtr;
   iSdbEvent_t *TmpEventPtr;
   size_t ExpectedSize;
   Bool_t Found;
   int ClearCount = 0;

   /* Initialise status return variable */
   Status = SYS_ERROR_NON_FATAL;

   /* Initialise array of DatumIDs to clear */
   NumData = 0;
   for(n = 0; n < I_SDB_MAXIDS; n++)
   {
      DatList[n] = -1;
   }

   /* Check message length */
   ExpectedSize = sizeof(SourceId) + sizeof(NumData);
   if(MsgPtr->DataLen < ExpectedSize)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr
      (
        Status,
        "Clear data message requires at least a sepcified source "
        "(%d bytes, %d expected)",
         MsgPtr->DataLen,
         ExpectedSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Assign a temporary pointer to the data block of the message */
   BufPtr = (char *)(MsgPtr->DataPtr);

   /* Extract the SourceID from which to clear */
   memcpy(&SourceId, BufPtr, sizeof(SourceId));
   SourceId = ntohl(SourceId);
   BufPtr += sizeof(SourceId);

   /* Check that we are premitted to clear data from this source */
   if((SourceId != MsgPtr->SourceId) && (MsgPtr->SourceId != E_CIL_MCP))
   {
      Status = E_SDB_NOT_AUTH;
      eLogErr
      (
        Status,
        "%s not permitted to clear source data of %s",
        eCilNameString(MsgPtr->SourceId), eCilNameString(SourceId)
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Extract the number of data submitted from the message */
   memcpy(&NumData, BufPtr, sizeof(NumData));
   NumData = ntohl(NumData);
   BufPtr += sizeof(NumData);
   eLogDebug("Clearing %d data", NumData);

   /* Now that we know the number of Datum IDs that we will be clearing, */
   /* we will check again (more carefully) the size of the message */
   ExpectedSize = sizeof(SourceId) + sizeof(NumData) 
                   + ( NumData * sizeof(Int32_t) );
   if(MsgPtr->DataLen != ExpectedSize)
   {
      Status = E_SDB_TRUNCATED;
      eLogErr
      (
        Status,
        "Clear data message not of expected size "
        "(%d bytes, %d expected)",
         MsgPtr->DataLen,
         ExpectedSize
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Extract list of Datum IDs which'll be cleared */
   for(n = 0; n < NumData; n++)
   {
      /* Extract all the components of one data element */
      memcpy(&DatList[n], BufPtr, sizeof(DatList[n]));
      DatList[n] = ntohl(DatList[n]);
      BufPtr += sizeof(DatList[n]);
   }

   for(n = 0; n < I_SDB_MAXIDS; n++)
   {
      if(DatList[n] != -1)
      {
         eLogDebug("Datum to clear = 0x%x", DatList[n]);
      }
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

         /* Ignore dummy entries */
         if(DefnPtr->NewestPtr == NULL) continue;

         /* Ignore irrelevant sources */
         if(DefnPtr->SourceId != SourceId) continue;

         /* Clear only specified Data */
         Found = FALSE;
         for(n = 0; n < I_SDB_MAXIDS; n++)
         {
            if(DatList[n] == DefnPtr->DatumId)
            {
               Found = TRUE;
               break;
            }
         }

         if(Found == FALSE) continue;

         /* If we get this far, then we are okay to clear the data */
         eLogInfo("Clearing data (%s,0x%x)",
            eCilNameString(DefnPtr->SourceId), DefnPtr->DatumId);
         ClearCount++;

         /* Wind through to the end of the data */
         EventPtr = DefnPtr->NewestPtr;
         while(EventPtr->NextPtr != NULL)
         {
            eLogDebug("Winding forward (%s,0x%x)",
               eCilNameString(DefnPtr->SourceId), DefnPtr->DatumId);
            EventPtr = EventPtr->NextPtr;
         }

         if(DefnPtr->NumData >= I_SDB_HIST_LIMIT)
         {
            iSdbTaskData[D_SDB_QTY_HISTLIM].Value--;
         }

         /* Backtrack through the data, deleting as we go */
         while(EventPtr != NULL)
         {
            eLogDebug("Clearing event from (%s,0x%x)",
               eCilNameString(DefnPtr->SourceId), DefnPtr->DatumId);
            TmpEventPtr = EventPtr->PrevPtr;
            TTL_FREE(EventPtr);
            EventPtr = TmpEventPtr;
            iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value--;
            DefnPtr->NumData--;
         }
         DefnPtr->NewestPtr = NULL;
         DefnPtr->OldestPtr = NULL;
         DefnPtr->LastSubPtr = NULL;
         DefnPtr->ValueRecorded = FALSE;
         if(DefnPtr->NumData != 0)
         {
            Status = SYS_ERROR_NON_FATAL;
            eLogErr(Status, "Error clearing data (possible memory leak)"
               " NumData = %d", DefnPtr->NumData);
         }

      }  /* End of loop hash-entries at a particular index */

   }  /* End of loop over the entire hash-table */


   /* Print a diagnostic */
   if ( ClearCount > 0 )
   {
      eLogInfo("Clearing data operation completed");
   }
   Status = SYS_NOMINAL;


   if(Status != SYS_NOMINAL)
   {
      eLogErr(Status, "Failed to clear data for source %s",
         eCilNameString(SourceId));
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Attempt to report this success to the submitting task */
   Status = iSdbAckReply(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogWarning(Status, "Error acknowledging successful clear operation");
      return Status;
   }

   return Status;

}  /* End of iSdbClearData() */


/* EOF */
