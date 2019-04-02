/*
** Module Name:
**    SdbHash.c
**
** Purpose:
**    A module contains functions for hash-table management.
**
** Description:
**    This module contains code for adding entries to/from a
**    hash table. This has table, defined in SdbPrivate.h, is
**    used as an index into the database maintained by the 
**    Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbHash.c,v 0.8 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbHash.c,v $
**    Revision 0.8  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.7  2000/09/27 11:50:52  djm
**    Added units-recorded flah in entry intialisation.
**
**    Revision 0.6  2000/09/08 14:29:14  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.5  2000/06/07 11:51:46  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.4  2000/05/18 13:17:33  djm
**    Removed excess data entry in "DefnPtr" initialisation.
**    Also implemented new task-data structure mechanism.
**
**    Revision 0.3  2000/05/17 09:38:42  djm
**    Changed names and types to match changes in the SDB
**    header files.
**
**    Revision 0.2  2000/05/12 15:10:00  djm
**    Added clearing of the definition structure on creation.
**    Also fixed a memory bug in the allocation of new hash entries.
**
**    Revision 0.1  2000/05/12 08:55:27  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TtlSystem.h"
#include "Sdb.h"
#include "SdbPrivate.h"
#include "Log.h"


/* Global variables */


/* Function prototypes */

size_t mSdbHash(Int32_t SourceId, Int32_t DatumId);



/* Functions */



size_t mSdbHashValue
(
   Int32_t SourceId,
   Int32_t DatumId
)
{
/*
** Function Name:
**    mSdbHashValue
**
** Type:
**    size_t
**
** Purpose:
**    Form a hash value from a Source Id and a Datum Id.
**
** Description:
**    Generate a hash value based on the supplied input parameters.
**    Note that the internal temporary variable is of type "unsigned"
**    deliberately to prevent accidental errors from overflow/negative
**    numbers.
**
** Arguments:
**    Int32_t SourceId       (in)
**       The CIL ID of the source of the data element.
**    Int32_t DatumId        (in)
**       The data element ID number from that source. This, combined
**       with the SourceId will define a unique code for each type
**       of data that may be dealt with.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    11-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   unsigned HashVal;         /* Temporary storage while building hash */

   HashVal = (unsigned) SourceId;
   HashVal += (unsigned) I_SDB_HASHMOD * (unsigned) DatumId;
   return HashVal % I_SDB_HASHSIZE;

} /* End of mSdbHashValue() */




iSdbDefn_t *iSdbHashLookup
(
   Int32_t SourceId,
   Int32_t DatumId
)
{
/*
** Function Name:
**    iSdbHashLookup
**
** Type:
**    iSdbDefn_t*
**
** Purpose:
**    Find an entry in the hash-table.
**
** Description:
**    This function finds the pointer to a hash table entry based
**    on the supplied Source and Data IDs. It returns a pointer to
**    the data element definition structure, or if it can not be
**    found, it returns NULL.
**
** Arguments:
**    Int32_t SourceId       (in)
**       The CIL ID of the source of the data element.
**    Int32_t DatumId        (in)
**       The data element ID number from that source. This, combined
**       with the SourceID will define a unique code for each type
**       of data that may be dealt with.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    07-Jun-2000 djm Added LOG functions for message reporting.
**    11-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   iSdbDefn_t *DefnPtr;      /* Pointer to hash table list */
   unsigned HashVal;


   /* Determine the position to start within the hash table */
   HashVal = mSdbHashValue(SourceId, DatumId);

   /* Cycle through the list for that particular hash entry */
   for
   (
      DefnPtr = iSdbHashTable[HashVal];
      DefnPtr != NULL;
      DefnPtr = DefnPtr->NextHashPtr
   )
   {

      eLogDebug(
         "Checking hash entry Addr=%u (%x<->%x) (%x<->%x)",
         (size_t)(DefnPtr),
         SourceId, DefnPtr->SourceId,
         DatumId, DefnPtr->DatumId
      );

      if
      (
         (SourceId == DefnPtr->SourceId) &&
         (DatumId == DefnPtr->DatumId)
      )
      {
         /* Correct hash entry found */
         return DefnPtr;
      }
   }

   return NULL;

} /* End of iSdbHashLookup() */




iSdbDefn_t *iSdbHashInstall
(
   Int32_t SourceId,
   Int32_t DatumId
)
{
/*
** Function Name:
**    iSdbHashInstall
**
** Type:
**    iSdbDefn_t*
**
** Purpose:
**    Insert a hash entry based on a Source ID and a Datum Id.
**
** Description:
**    This function will install a specified DefnPtr into the hash 
**    table.
**
** Arguments:
**    Int32_t SourceId       (in)
**       The CIL ID of the source of the data element.
**    Int32_t DatumId        (in)
**       The data element ID number from that source. This, combined
**       with the SourceId will define a unique code for each type
**       of data that may be dealt with.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    20-Sep-2000 djm Added initialisation of the UnitsRecorded flag.
**    08-Sep-2000 djm Allowed for renaming of QTY_DEFNS constant.
**    07-Jun-2000 djm Added LOG functions for message reporting.
**    11-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   iSdbDefn_t *DefnPtr;      /* Pointer to hash table list */
   unsigned HashVal;


   /* Allocate memory for hash table entry */
   DefnPtr = (iSdbDefn_t *) TTL_MALLOC(sizeof(iSdbDefn_t));
   if(DefnPtr == NULL)
   {
      eLogCrit(E_SDB_MALLOC_FAIL, "Failed to allocate memory for hash table");
      return NULL;
   }

   /* Determine the hash table key value, based on Source & Datum Ids */
   HashVal = mSdbHashValue(SourceId, DatumId);

   /* Put a pointer in the existing entry to the old entry */
   DefnPtr->NextHashPtr = iSdbHashTable[HashVal];


   /* Replace the hash table entry with the new entry */
   iSdbHashTable[HashVal] = DefnPtr;


   /* Put the search data itself into the structure */
   DefnPtr->SourceId = SourceId;
   DefnPtr->DatumId = DatumId;

   /* Zero all the other elements in the structure */
   DefnPtr->NewestPtr = NULL;
   DefnPtr->OldestPtr = NULL;
   DefnPtr->Units = 0;
   DefnPtr->UnitsRecorded = FALSE;
   DefnPtr->NumData = 0;

   /* Increment the global counter keeping track of how many we have */
   iSdbTaskData[D_SDB_QTY_DEFNS].Value++;

   /* Return as the function value, the address of the new entry */
   eLogDebug("Memory allocated at %u, hash=%d (%x,%x)",
      (size_t)DefnPtr, HashVal, DefnPtr->SourceId, DefnPtr->DatumId);
   return DefnPtr;

} /* End of iSdbHashInstall() */



/*
** OTHER HASH FUNCTIONS TO BE IMPLEMENTED...
*/

/* iSdbHashDelete() */


/* EOF */
