/*
** Module Name:
**    SdbList.c
**
** Purpose:
**    Functions for listing the contents of the database.
**
** Description:
**    This module contains code for getting a listing of the
**    contents of the database maintained by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbList.c,v 0.12 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbList.c,v $
**    Revision 0.12  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.11  2001/01/03 13:32:44  mjf
**    Addition of missing error replies.
**
**    Revision 0.10  2000/12/04 10:42:17  mjf
**    Enhanced log messages to include CIL name in addition to CIL ID.
**
**    Revision 0.9  2000/10/19 17:46:30  djm
**    Added sorting of data for the LISTSOURCES and LISTDATA services.
**
**    Revision 0.8  2000/10/16 15:34:20  djm
**    Bug fix to correctly deal with empty lists. Fixes the problem
**    encountered with the "testlist" program.
**
**    Revision 0.7  2000/10/13 14:59:43  djm
**    Put in additional checks for coping with empty data lists.
**
**    Revision 0.6  2000/09/05 14:18:02  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.5  2000/06/27 14:51:41  djm
**    Improve and centralise error reporting/registration.
**
**    Revision 0.4  2000/06/07 11:52:05  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.3  2000/05/22 13:31:19  djm
**    Corrected bug with search algorithm that was causing not all
**    source/datum IDs to be reported.
**
**    Revision 0.2  2000/05/21 15:28:18  djm
**    Added "listdata" function, also fixed numerous bugs in original.
**
**    Revision 0.1  2000/05/19 15:03:44  djm
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

Status_t mSdbSortList(Uint32_t NumData, Int32_t *List);

/* Functions */


Status_t iSdbListSources(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbListSources
**
** Type:
**    Status_t
**
** Purpose:
**    Determines list of source ID's for data stored in database.
**
** Description:
**    Creates a CIL message which contains a list of the source
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
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    07-Jun-2000 djm Replaced error reporting with LOG function calls.
**    22-May-2000 djm Fixed bug with list search algorithm.
**    21-May-2000 djm Fixed problem with network byte ordering.
**    19-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   size_t OutBufSize;        /* Memory size to allocate for buffer */
   Uint32_t NumSrcs;         /* Number of source IDs known */
   char *OutBufPtr;          /* Pointer to output buffer */
   char *TmpBufPtr;          /* Temporary pointer to output buffer */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */
   int Index;                /* Index into the hash-table array */
   int n;                    /* inner loop counter */
   Int32_t SrcList[I_SDB_MAXIDS];        /* List of sources found */
   Int32_t SourceId;         /* Temporary variable for source IDs */
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
        "ListSources message contains unexpected data (%d bytes, 0 expected)",
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

         /* Ignore dummy entries */
         if(DefnPtr->NewestPtr == NULL) continue;

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

   /* Print a diagnostic */
   eLogInfo("%d sources detected in iSdbListSources() call", NumSrcs);

   /* Sort the message data buffer */
   mSdbSortList(NumSrcs, SrcList);

   /* Create a suitably sized message buffer for the counts + sources */
   OutBufSize = sizeof(NumSrcs) + NumSrcs * sizeof(SourceId);
   OutBufPtr = TTL_MALLOC(OutBufSize);
   if(OutBufPtr == NULL)
   {
      Status = E_SDB_MALLOC_FAIL;
      eLogErr(Status, "Failed to allocate memory for ListSources reply message");
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Put the number of sources into the first 32-bit word */
   TmpBufPtr = OutBufPtr;
   NumSrcs = htonl(NumSrcs);
   memcpy(TmpBufPtr, (char *)&NumSrcs, sizeof(NumSrcs));
   TmpBufPtr += sizeof(NumSrcs);


   /* Scan the linked list */
   for(n = 0; SrcList[n] != -1; n++)
   {
      /* Add each source into the message buffer */
      SourceId = htonl(SrcList[n]);
      memcpy(TmpBufPtr, (char *)&SourceId, sizeof(SourceId));
      TmpBufPtr += sizeof(SourceId);
   }


   /* Prepare CIL message for reply */

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
   }


   /* Terminate the function, reporting success */
   TTL_FREE(OutBufPtr);                /* Get rid of temporary data buffer */
   return Status;


}  /* End of iSdbListSources() */




Status_t iSdbListData(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbListData
**
** Type:
**    Status_t
**
** Purpose:
**    Determines list of datum ID's for data from a particular
**    source ID stored in database.
**
** Description:
**    Creates a CIL message which contains a list of the datum
**    ID's that exist within the database maintained by the SDB.
**    The list is only the ID's for a particular, specified source.
**    It is possible to have determined the sources that have data
**    lodged in the SDB, using the iSdbListSources() call.
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
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    07-Jun-2000 djm Replaced error reporting with LOG function calls.
**    22-May-2000 djm Fixed potential problem with search algorithm.
**    21-May-2000 djm Fixed problem with network byte ordering.
**    19-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   size_t OutBufSize;        /* Memory size to allocate for buffer */
   Uint32_t NumData;         /* Number of Datum IDs known */
   Int32_t SourceId;         /* ID of the source to get data IDs for */
   char *InBufPtr;           /* Pointer to input buffer */
   char *OutBufPtr;          /* Pointer to output buffer */
   char *TmpBufPtr;          /* Temporary pointer to output buffer */
   Int32_t SwapAddr;         /* Temporary variable for swapping addresses */
   int Index;                /* Index into the hash-table array */
   int n;                    /* inner loop counter */
   Int32_t DatList[I_SDB_MAXIDS];        /* List of sources found */
   Bool_t Found;             /* Flag to indicate item found in list */
   iSdbDefn_t *DefnPtr;      /* Data element definiton from hash-table */
   Int32_t DatumId;          /* Temporary variable for Datum IDs */


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
         "ListData message of incorrect length "
         "(%d bytes received, %d expected)",
         MsgPtr->DataLen, sizeof(SourceId)
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


   /* Extract from the incoming message the SourceId we're looking for */
   InBufPtr = (char *)(MsgPtr->DataPtr);
   memcpy(&SourceId, InBufPtr, sizeof(SourceId));
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

         /* Ignore dummy entries */
         if(DefnPtr->NewestPtr == NULL) continue;

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


   /* Create a suitably sized message buffer for the counts + sources */
   OutBufSize = sizeof(NumData) + NumData * sizeof(DatumId);
   OutBufPtr = TTL_MALLOC(OutBufSize);
   if(OutBufPtr == NULL)
   {
      Status = E_SDB_MALLOC_FAIL;
      eLogErr(Status, "Failed to allocate memory for ListData reply message");
      iSdbErrReply(DelivererId, MsgPtr, E_SDB_TRUNCATED);
      return Status;
   }

   /* Sort the message data buffer */
   mSdbSortList(NumData, DatList);

   /* Put the number of sources into the first 32-bit word */
   TmpBufPtr = OutBufPtr;
   NumData = htonl(NumData);
   memcpy(TmpBufPtr, (char *)&NumData, sizeof(NumData));
   TmpBufPtr += sizeof(NumData);


   /* Scan the linked list */
   for(n = 0; DatList[n] != -1; n++)
   {
      /* Add each source into the message buffer */
      DatumId = htonl(DatList[n]);
      memcpy(TmpBufPtr, (char *)&DatumId, sizeof(DatumId));
      TmpBufPtr += sizeof(DatumId);
   }


   /* Prepare CIL message for reply */

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
   }


   /* Terminate the function, reporting success */
   TTL_FREE(OutBufPtr);                /* Get rid of temporary data buffer */
   return Status;


}  /* End of iSdbListData() */


int mSdbIdCompare(
   const void *Item1,
   const void *Item2
)
{
/*
** Function Name:
**    mSdbIdCompare
**
** Type:
**    int
**
** Purpose:
**    Comparison function used by qsort() in iSdbSortList().
**
** Description:
**    ...
**
** Arguments:
**    const void *Item1      (in)
**       First item to compare.
**    const void *Item2      (in)
**       Second item to compare.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    19-Oct-2000 djm Initial creation.
**
*/

   if( *((Int32_t *)(Item1)) < *((Int32_t *)(Item2)) )
   {
      return -1;
   }
   if( *((Int32_t *)(Item1)) > *((Int32_t *)(Item2)) )
   {
      return 1;
   }
   else
   {
      return 0;
   }

}  /* End of mSdbIdCompare() */


Status_t mSdbSortList(
   Uint32_t NumData,
   Int32_t *List
)
{
/*
** Function Name:
**    mSdbSortList
**
** Type:
**    Status_t
**
** Purpose:
**    Orders a list of data.
**
** Description:
**    ...
**
** Arguments:
**    Uint32_t NumData       (in)
**       Number of items in the list.
**    Int32_t *List          (in/out)
**       Pointer to the list of data.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    19-Oct-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   int Id;                   /* Loop counter */

   /* Some diagnostic code first */
   for(Id = 0; Id < NumData; Id++)
   {
      eLogDebug("ID %d = %4.4x\n", Id, *(List+Id));
   }

   /* Sort the data */
   qsort( (void *)List, NumData, sizeof(*List), mSdbIdCompare );

   /* Return the success status */
   Status = SYS_NOMINAL;
   return Status;
}  /* End of mSdbSortList() */

/* EOF */
