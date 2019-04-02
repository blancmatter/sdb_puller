/*
** Module Name:
**    SdbLnkLst.c
**
** Purpose:
**    A module with functions for manipulating linked list entries.
**
** Description:
**    This module contains code for manipulating the linked list that
**    forms the database maintained by the Status Database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbLnkLst.c,v 0.3 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbLnkLst.c,v $
**    Revision 0.3  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.2  2000/09/08 14:29:24  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.1  2000/05/30 18:56:29  djm
**    Initial creation.
**
**
*/


/* Include files */
#include <stdlib.h>

#include "TtlSystem.h"
#include "Sdb.h"
#include "SdbPrivate.h"




/* Functions */


Status_t iSdbLnkLstCreate(
   iSdbDefn_t *DefnPtr,
   iSdbEvent_t *EventPtr
)
{
/*
** Function Name:
**    iSdbLnkLstCreate
**
** Type:
**    Status_t
**
** Purpose:
**    Create a new linked list.
**
** Description:
**    Starts a new linked list with entry *EventPtr, for the definition
**    *DefnPtr.
**
** Arguments:
**    eSdbDefn_t *DefnPtr    (in/out)
**       A pointer to a SDB definition. This defines the linked list
**       "location" to use.
**    eSdbEvent_t *EventPtr  (in/out)
**       This is the item to put in as the first entry in the linked list.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    08-Sep-2000 djm Changed name of global data enumeration.
**    30-May-2000 djm Initial creation.
**
*/

   /* No local variables */


   /* First, check that the linked list really is empty */
   if((DefnPtr->NewestPtr != NULL) || (DefnPtr->OldestPtr != NULL))
   {
      return E_SDB_LL_NOTEMPTY;
   }

   /* Then check that we have a valid entry to add */
   if(EventPtr == NULL)
   {
      return E_SDB_LL_NULLENTRY;
   }


   /* Link the oldest and newest Defn items to the event */
   DefnPtr->OldestPtr = EventPtr;
   DefnPtr->NewestPtr = EventPtr;

   /* Increment the data counters */
   DefnPtr->NumData++;
   iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value++;


   /* Return success */
   return SYS_NOMINAL;

}  /* End of eSdbLnkLstCreate() */




Status_t iSdbLnkLstAppend(
   iSdbDefn_t *DefnPtr,
   iSdbEvent_t *EventPtr
)
{
/*
** Function Name:
**    iSdbLnkLstAppend
**
** Type:
**    Status_t
**
** Purpose:
**    Create a new entry after last entry in the linked list.
**
** Description:
**    ...
**
** Arguments:
**    eSdbDefn_t *DefnPtr    (in/out)
**       A pointer to a SDB definition. This defines the linked list
**       to use.
**    eSdbEvent_t *EventPtr  (in/out)
**       This is the item to put after the latest entry in the linked
**       list.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    08-Sep-2000 djm Changed name of global data enumeration.
**    30-May-2000 djm Initial creation.
**
*/

   /* No local variables */


   /* First, check that the linked list really isn't empty */
   if((DefnPtr->NewestPtr == NULL) || (DefnPtr->OldestPtr == NULL))
   {
      return E_SDB_LL_EMPTY;
   }

   /* Then check that we have a valid entry to add */
   if(EventPtr == NULL)
   {
      return E_SDB_LL_NULLENTRY;
   }


   /* Change the pointers on the previously most recent event */
   DefnPtr->NewestPtr->NextPtr = EventPtr;
   EventPtr->PrevPtr = DefnPtr->NewestPtr;

   /* Then set the pointer to the newest data to our new one */
   DefnPtr->NewestPtr = EventPtr;

   /* Increment the data counters */
   DefnPtr->NumData++;
   iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value++;


   /* Return success */
   return SYS_NOMINAL;

}  /* End of eSdbLnkLstAppend() */



Status_t iSdbLnkLstPrepend(
   iSdbDefn_t *DefnPtr,
   iSdbEvent_t *EventPtr
)
{
/*
** Function Name:
**    iSdbLnkLstPrepend
**
** Type:
**    Status_t
**
** Purpose:
**    Create a new entry before first entry in the linked list.
**
** Description:
**    ...
**
** Arguments:
**    eSdbDefn_t *DefnPtr    (in/out)
**       A pointer to a SDB definition. This defines the linked list
**       to use.
**    eSdbEvent_t *EventPtr  (in/out)
**       This is the item to put before the first entry in the linked
**       list.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    08-Sep-2000 djm Changed name of global data enumeration.
**    30-May-2000 djm Initial creation.
**
*/

   /* No local variables */


   /* First, check that the linked list really isn't empty */
   if((DefnPtr->NewestPtr == NULL) || (DefnPtr->OldestPtr == NULL))
   {
      return E_SDB_LL_EMPTY;
   }

   /* Then check that we have a valid entry to add */
   if(EventPtr == NULL)
   {
      return E_SDB_LL_NULLENTRY;
   }


   /* Change the pointers on the previously oldest event */
   DefnPtr->OldestPtr->PrevPtr = EventPtr;
   EventPtr->NextPtr = DefnPtr->OldestPtr;

   /* Then set the pointer to the oldest data to our new one */
   DefnPtr->OldestPtr = EventPtr;

   /* Increment the data counters */
   DefnPtr->NumData++;
   iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value++;


   /* Return success */
   return SYS_NOMINAL;

}  /* End of eSdbLnkLstPrepend() */



Status_t iSdbLnkLstInsert(
   iSdbDefn_t *DefnPtr,
   iSdbEvent_t *PrevEventPtr,
   iSdbEvent_t *NewEventPtr
)
{
/*
** Function Name:
**    iSdbLnkLstInsert
**
** Type:
**    Status_t
**
** Purpose:
**    Create a new entry after the specified entry in the middle of the list.
**
** Description:
**    This function creates a new linked list entry, after the specified
**    entry, but before the one that previously followed it. That is, if
**    there is an entry already following the one we are inserting after,
**    then use this function. Otherwise, iSdbLnkLstAppend() should be used.
**
** Arguments:
**    eSdbDefn_t *DefnPtr              (in/out)
**       A pointer to a SDB definition. This defines the linked list
**       to use.
**    eSdbEvent_t *PrevEventPtr        (in/out)
**       This is the item in the linked list after which to put the
**       new entry.
**    eSdbEvent_t *NewEventPtr         (in/out)
**       This is the item to put after the specified entry in the linked
**       list.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    08-Sep-2000 djm Changed name of global data enumeration.
**    30-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   iSdbEvent_t *NextEventPtr; /* Temporary event pointer */


   /* First, check that the linked list really isn't empty */
   if((DefnPtr->NewestPtr == NULL) || (DefnPtr->OldestPtr == NULL))
   {
      return E_SDB_LL_EMPTY;
   }

   /* Check we're adding after a valid entry */
   if(PrevEventPtr == NULL)
   {
      return E_SDB_LL_NULLENTRY;
   }

   /* Check that the entry we'll insert after has a following entry anyway */
   if(PrevEventPtr->NextPtr == NULL)
   {
      return E_SDB_LL_ENDOFLIST;
   }

   /* Then check that we have a valid entry to add */
   if(NewEventPtr == NULL)
   {
      return E_SDB_LL_NULLENTRY;
   }


   /* Get a pointer the one that follows */
   NextEventPtr = PrevEventPtr->NextPtr;


   /* Correct the pointer on the previous entry */
   PrevEventPtr->NextPtr = NewEventPtr;

   /* Correct the pointer on the one that followed it */
   NextEventPtr->PrevPtr = NewEventPtr;

   /* Put in the correct links on our new entry */
   NewEventPtr->NextPtr = NextEventPtr;
   NewEventPtr->PrevPtr = PrevEventPtr;

   /* Increment the data counters */
   DefnPtr->NumData++;
   iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value++;


   /* Return success */
   return SYS_NOMINAL;

}  /* End of eSdbLnkLstInsert() */




Status_t iSdbLnkLstClip(
   iSdbDefn_t *DefnPtr
)
{
/*
** Function Name:
**    iSdbLnkLstClip
**
** Type:
**    Status_t
**
** Purpose:
**    Remove the oldest entry from a linked list.
**
** Description:
**    This function removes the oldest entry from a linked list,
**    referred to by the DefnPtr. Note that this function only works
**    if there are two or more entries in the linked list. I.e. it
**    will not work if it totally empties the list.
**
** Arguments:
**    eSdbDefn_t *DefnPtr    (in/out)
**       A pointer to a SDB definition. This defines the linked list
**       to clip.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    08-Sep-2000 djm Changed name of global data enumeration.
**    30-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   iSdbEvent_t *TmpEventPtr; /* Temporary event pointer */


   /* First, check that the linked list really isn't empty */
   if((DefnPtr->NewestPtr == NULL) || (DefnPtr->OldestPtr == NULL))
   {
      return E_SDB_LL_EMPTY;
   }


   /* Record what the second oldest data pointer was */
   TmpEventPtr = DefnPtr->OldestPtr->NextPtr;

   /* Adjust all the refering pointers */
   DefnPtr->OldestPtr->NextPtr->PrevPtr = NULL;      /* ! */

   /* Delete oldest entry */
   TTL_FREE(DefnPtr->OldestPtr);

   /* Re-record the pointer to the "new" oldest point */
   DefnPtr->OldestPtr = TmpEventPtr;

   /* Decrement the counter with how much data is in that list */
   DefnPtr->NumData--;

   /* Decrement the global variable with the total number of stored values */
   iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value--;


   /* Return success */
   return SYS_NOMINAL;

}  /* End of eSdbLnkLstClip() */




/* EOF */
