/*
** Module Name:
**    Sdb.c
**
** Purpose:
**    A program to manage storage for telescope engineering data.
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: Sdb.c,v 0.9 2006/06/26 15:26:51 mjf Exp $
**
** History:
**    $Log: Sdb.c,v $
**    Revision 0.9  2006/06/26 15:26:51  mjf
**    Cache time to reduce system calls.
**
**    Revision 0.8  2000/10/16 14:04:19  djm
**    Added check to flag repeated file write failures with the WARNING
**    process state.
**
**    Revision 0.7  2000/09/08 14:28:23  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.6  2000/09/08 12:59:35  djm
**    Changes to safe-state behaviour.
**
**    Revision 0.5  2000/08/31 15:30:05  djm
**    Now uses the "nominal" state, to allow switching between
**    "SAFE" and "OKAY" states on commands from the MCP.
**
**    Revision 0.4  2000/08/30 11:18:36  djm
**    Changed SDB to make use of CLU command line handling.
**    Most of the original functions have been removed from this module
**    and replaced by new functions in the SdbSetup.c module.
**
**    Revision 0.3  2000/06/07 11:51:23  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.2  2000/05/09 11:14:16  djm
**    Corrected CIL address types to match recent CIL changes.
**
**    Revision 0.1  2000/05/09 08:01:15  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define M_SDB_MAIN
#include "TtlSystem.h"
#include "Sdb.h"
#include "SdbPrivate.h"
#include "Cil.h"
#include "Log.h"
#include "Tim.h"


int main(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    Top level function of the SDB.
**
** Description:
**    Top level function of the SDB. It is called on startup of
**    the executable.
**
** Arguments:
**    int argc                 (in)
**       Number of arguments on the command line (including the
**       executable name).
**    char *argv[]             (in)
**       Array of null-terminated character strings containing
**       the command line arguments.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Sep-2000 djm Put in heartbeat timeout.
**    29-Aug-2000 djm Reworked to use with new CLU package.
**    07-Jun-2000 djm Added LOG functions to replace iSdbReport().
**    04-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;        /* Function return status variable */
   eCilMsg_t Msg;          /* CIL message structure */
   Int32_t DelivererId;    /* CIL ID of process that send us the message */
   int NumTimeouts = 0;    /* Counter to see when we are idle */
   eTtlTime_t CurrentTime; /* The current system time */
   eTtlTime_t DiffTime;    /* The difference between sys.time and hbeat rx */



   /* Commence SDB initialisation (This should already be set to INIT) */
   iSdbSetState(SYS_INIT_STATE);


   /* Do program setup */
   Status = iSdbSetup(argc, argv, &Msg);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Setup failure");
      return EXIT_FAILURE;
   }

   /* Print a diagnostic/debugging message */
   iSdbSetState(iSdbNominalState);



   /* Main processing loop */ 
   for(;;)
   {

      /* If required, print a status message to screen */
      if(eCluCommon.Verbose == TRUE) iSdbDisplay();

      /* On each pass, reset the buffer size variable to max.capacity */
      Msg.DataLen = I_SDB_DATASIZE;

      /* Block, waiting for an inbound message */
      Status = eCilReceive(I_SDB_TIMEOUT, &DelivererId, &Msg);

      /* Cache current time. */         
      Status = eTimCacheTime(&CurrentTime);
      if(Status != SYS_NOMINAL) eLogErr(Status, "Unable to get system time");

      /* React to the results of the receive */
      switch(Status)
      {

         case SYS_NOMINAL:
            /* Message received, process it */
            iSdbProcess(DelivererId, &Msg);
            iSdbSetState(iSdbNominalState);
            NumTimeouts = 0;
            break;
         case E_CIL_TIMEOUT:
            /* Timeout on Rx; don't worry about these for the time being */
            NumTimeouts++;
            break;
         default:
            /* Unexpected CIL error */
            /* Increment error count */
            iSdbTaskData[D_SDB_QTY_ERRORS].Value++;
            break;

      }  /* End of switch() */

      /* Check to see if we've received a recent heartbeat */
      Status = eTimDifference(&iSdbHeartBeatTime, &CurrentTime, &DiffTime);
      if(Status != SYS_NOMINAL) eLogErr(Status, "Unable to get delta time");
      if(
         (DiffTime.t_sec > I_SDB_SAFE_AFTER) && 
         (iSdbNominalState != SYS_SAFE_STATE)
      )
      {
         eLogWarning(
            E_SDB_HBEAT_FAIL,
            "No heartbeats have been correctly processed. Going to safe-state"
         );
         iSdbGoSafe();
         iSdbSetState(iSdbNominalState);
      }

      /* Check error counters to see if we need a state change */
      if(iSdbNumWriteFails >= I_SDB_MAX_FILE_ERRS)
      {
         if(iSdbNumWriteFails == I_SDB_MAX_FILE_ERRS)
         {
            /* On the first time through, note the error to the log */
            eLogCrit(
               E_SDB_WRITE_ERR_LIMIT,
               "Still unable to write to file after %d attempts",
               I_SDB_MAX_FILE_ERRS
            );
         }

         /* Set the process state to warning */
         iSdbSetState(SYS_WARN_STATE);

         /* Lock the counter, so we don't get any wrap errors */
         iSdbNumWriteFails = I_SDB_MAX_FILE_ERRS + 1;
      }

   }  /* End of for(;;) */

}  /* End of main() */


/* EOF */
