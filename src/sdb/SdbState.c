/*
** Module Name:
**    SdbState.c
**
** Purpose:
**    A module containing state management functions for the SDB.
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbState.c,v 0.10 2007/07/17 10:20:02 mjf Exp $
**
** History:
**    $Log: SdbState.c,v $
**    Revision 0.10  2007/07/17 10:20:02  mjf
**    Degrade a state of Okay to Warn if MySQL logging is enabled but failing.
**
**    Revision 0.9  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.8  2005/06/20 13:35:31  mjf
**    Minor changes to aid porting to Linux - no functional changes.
**
**    Revision 0.7  2002/01/10 17:42:18  mjf
**    On a safe-state, ensure all volatile data is archived to disk, even if
**    the value is the same as the last value archived.
**
**    Revision 0.6  2000/09/08 14:30:15  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.5  2000/09/08 13:04:13  djm
**    Moved safe-state functionality into a different function.
**
**    Revision 0.4  2000/09/05 14:19:55  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.3  2000/08/31 15:35:23  djm
**    New functions that implement the MCP commands to request that
**    the SDB change state.
**
**    Revision 0.2  2000/05/18 13:20:56  djm
**    Implemented new task-data structure mechanism.
**
**    Revision 0.1  2000/05/09 08:13:30  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdlib.h>

#include "TtlSystem.h"
#include "Sdb.h"
#include "SdbPrivate.h"
#include "Tim.h"
#include "Log.h"


void iSdbSetState(ProcState_t State)
{
/*
** Function Name:
**    iSdbSetState
**
** Type:
**    void
**
** Purpose:
**    State handling function for the SDB.
**
** Description:
**    ...
**
** Arguments:
**    ProcState_t State      (in)
**       New state to which the global state variable shall be set.
**       The type "ProcState_t" is defined in TtlSystem.h.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added pushing of state into database.
**    05-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   eSdbDatum_t Datum;        /* Data element buffer for submission */
   Status_t Status;          /* Return value from called functions */

   /* Don't permit an overall state of OK if MySql logging is failing */
   if ( ( State == SYS_OKAY_STATE ) && ( iSdbSqlLoggingOK == FALSE ) )
   {
      /* Downgrade to Warning */
      State = SYS_WARN_STATE;
   }

   /* Check if we have a change in state */
   if(iSdbTaskData[D_SDB_PROC_STATE].Value == State)
   {
      return;
   }

   /* Remember the state variable in global memory */
   iSdbTaskData[D_SDB_PROC_STATE].Value = State;

   /* Insert the datum into the SDB's database */
   Datum.SourceId = iSdbCilId;
   Datum.DatumId = D_SDB_PROC_STATE;
   Datum.Units = iSdbTaskData[D_SDB_PROC_STATE].Units;
   Datum.Msrment.Value = iSdbTaskData[D_SDB_PROC_STATE].Value;
   Status = eTimGetTime(&(Datum.Msrment.TimeStamp));

   /* Lodge the formatted "datum" structure into the database */
   if(Status == SYS_NOMINAL)
   {
      Status = iSdbAddData(&Datum);
   }

   /* Terminate function and return, ignoring any errors */
   return;

} /* End of iSdbSetState() */




Status_t iSdbSafeState
(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbSafeState
**
** Type:
**    Status_t
**
** Purpose:
**    Change the SDB state from active to safe.
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
**       processing. The contents of this structure may be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Moved safe-state functionality to seperate function.
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    30-Aug-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Function status variable */



   /* Report that we have started working on it */
   Status = iSdbActReply(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogWarning(
         Status, "Error acknowledging starting to go to safe-state"
      );
      return Status;
   }

   /* Actually make the SB safe */
   iSdbGoSafe();

   /* Attempt to report this success to the submitting task */
   Status = iSdbComReply(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogWarning(
         Status, "Error acknowledging successful transfer to safe state"
      );
      return Status;
   }

   /* Terminate function and return */
   eLogNotice(0, "Safe state attained");
   return SYS_NOMINAL;

} /* End of iSdbSafeState() */





void iSdbGoSafe(void)
{
/*
** Function Name:
**    iSdbGoSafe
**
** Type:
**    void
**
** Purpose:
**    Change the SDB state from active to safe.
**
** Description:
**    ...
**
** Arguments:
**    (none)
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    08-Jan-2002 mjf Force writing of any volatile data to file before closing.
**    05-Sep-2000 djm Initial creation.
**
*/

   /* Local variables */
   int Index;                /* Loop counter for file array index */
   iSdbDefn_t *DefnPtr;      /* Data element definiton from hash-table */


   /* If we are archiving data to disk */
   if(iSdbFileStore == TRUE)
   {
      /* Loop over the hash-table array */
      for( Index = 0; Index < I_SDB_HASHSIZE; Index++ )
      {
         /* Loop over the hash-entries for the current hash-table index */
         for( DefnPtr = iSdbHashTable[ Index ];
              DefnPtr != NULL;
              DefnPtr = DefnPtr->NextHashPtr )
         {
            /* Ignore dummy entries */
            if( DefnPtr->NewestPtr == NULL )
               continue;

            /* If the last measurement has not been written to disk */
            if ( DefnPtr->ValueRecorded == FALSE )
            {
               /* Store the measurement, forcing a write to disk */
               iSdbStoreData( TRUE, DefnPtr );
            }
         }
      }

      /* Loop around the open files */
      for(Index = 0; Index < I_SDB_MAX_DB_FILES; Index++)
      {
         /* If a file pointer exists (a file is open) */
         if(iSdbDbFileList[Index].FilePtr != NULL)
         {
            /* Close the file (this will also flush unwritten data) */
            fclose(iSdbDbFileList[Index].FilePtr);

            /* Reset all the file parameters, to start again if activated */
            iSdbDbFileList[Index].FilePtr = NULL;
            iSdbDbFileList[Index].StartTime.t_sec = 0;
            iSdbDbFileList[Index].StartTime.t_nsec = 0;
            iSdbDbFileList[Index].LastAccessed.t_sec = 0;
            iSdbDbFileList[Index].LastAccessed.t_nsec = 0;
         }
      }
   }

   /* Change the "Okay-state" to SYS_SAFE_STATE */
   iSdbNominalState = SYS_SAFE_STATE;

   /* Terminate function and return */
   return;

} /* End of iSdbGoSafe() */





Status_t iSdbActivate
(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbActivate
**
** Type:
**    Status_t
**
** Purpose:
**    Change the SDB state from safe state to active.
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
**       processing. The contents of this structure may be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    30-Aug-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Function status variable */

   /* Report that we have started working on it */
   Status = iSdbActReply(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogWarning(
         Status, "Error reporting commencment of activation"
      );
      return Status;
   }


   /* Change the "Okay-state" to SYS_OKAY_STATE */
   iSdbNominalState = SYS_OKAY_STATE;

   /* Attempt to report this success to the submitting task */
   Status = iSdbComReply(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogWarning(Status, "Error acknowledging successful activation");
      return Status;
   }

   /* Terminate function and return */
   eLogNotice(0, "Application fully active");
   return SYS_NOMINAL;

} /* End of iSdbActivate() */



Status_t iSdbShutdown
(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbShutdown
**
** Type:
**    Status_t
**
** Purpose:
**    Terminate the SDB
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
**       processing. The contents of this structure may be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Added deliverer ID for correct message handling.
**    30-Aug-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Function status variable */
   int Index;                /* Loop counter for file array index */

   /* Close all open files */
   if(iSdbFileStore == TRUE)
   {
      for(Index = 0; Index < I_SDB_MAX_DB_FILES; Index++)
      {
         if(iSdbDbFileList[Index].FilePtr != NULL)
         {
            fclose(iSdbDbFileList[Index].FilePtr);
         }
      }
   }


   /* Attempt to report this success to the submitting task */
   Status = iSdbAckReply(DelivererId, MsgPtr);
   if(Status != SYS_NOMINAL)
   {
      eLogWarning(Status, "Error acknowledging successful activation");
      return Status;
   }

   /* Terminate application */
   eLogNotice(0, "Application terminated on shutdown request");
   exit(EXIT_SUCCESS);

   /* Terminate function and return */
   return SYS_WARNING;

} /* End of iSdbShutdown() */







/* EOF */
