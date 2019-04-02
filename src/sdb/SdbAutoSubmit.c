/*
** Module Name:
**    SdbAutoSubmit.c
**
** Purpose:
**    A module containing a submitting SDB global data into 
**    its own database.
**
** Description:
**    In addition to fielding data from other tasks about the
**    telescope system, the SDB itself generates data that is
**    of interest to other tasks. In particular, it is possible
**    for a human to interrogate the contents of the database
**    and to see the database configuration parameters. The
**    function in this module facilitates this auto-submission
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbAutoSubmit.c,v 0.8 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbAutoSubmit.c,v $
**    Revision 0.8  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.7  2000/11/03 16:05:35  djm
**    Changed the code so that the SDB doesn't submit to itself, its own
**    ProcState.
**
**    Revision 0.6  2000/09/08 14:29:02  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.5  2000/08/30 11:15:31  djm
**    Change to cope with the new command line arguments.
**
**    Revision 0.4  2000/06/07 10:48:32  djm
**    Changed incorrect comment.
**
**    Revision 0.3  2000/05/31 10:51:09  djm
**    Changed code to make use of TIM package functions.
**
**    Revision 0.2  2000/05/23 14:59:53  djm
**    Used clock_gettime() function to give sub-second resolution
**    on SDB timestamps.
**
**    Revision 0.1  2000/05/18 14:39:48  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <time.h>

#include "Sdb.h"
#include "SdbPrivate.h"
#include "Tim.h"


Status_t iSdbAutoSubmit(void)
{
/*
** Function Name:
**    iSdbAutoSubmit
**
** Type:
**    Status_t
**
** Purpose:
**    Stores SDB global data into its own database.
**
** Description:
**    This function takes all the global data within the SDB and
**    creates data structures that are suitable for calling the
**    iSdbAddData() function, defined in SdbSubmit.c.
**
** Arguments:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    02-Nov-2000 djm Don't report  common data.
**    08-Sep-2000 djm Adjusted for the new global data enumeration.
**    29-Aug-2000 djm Changed CIL ID number variable.
**    31-May-2000 djm Converted to use TIM package for timestamps.
**    23-May-2000 djm Added clock_gettime call for timestamps.
**    18-May-2000 djm Initial creation.
**
*/

   /* Local variables */
   eSdbDatum_t Datum;        /* Data element buffer for submission */
   Status_t Status;          /* Return value from called functions */
   int Item;                 /* Loop counter for global task data items */


   /* Get the timestamp to associate with all the data */
   Status = eTimGetTime(&(Datum.Msrment.TimeStamp));
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }

   /* Loop over each element in the global data array */
   for(Item = D_MCP_FIRST_USER_DATUM; Item < D_SDB_DATAID_EOL; Item++)
   {
      /* Format-up "datum" structure (except TimeStamp, which is common) */
      Datum.SourceId = iSdbCilId;
      Datum.DatumId = Item;
      Datum.Units = iSdbTaskData[Item].Units;
      Datum.Msrment.Value = iSdbTaskData[Item].Value;

      /* Lodge the formatted "datum" structure into the database */
      Status = iSdbAddData(&Datum);
      if(Status != SYS_NOMINAL)
      {
         return Status;
      }

   }  /* End of for-loop */


   /* If we get this far, terminate the function reporting success. */
   return SYS_NOMINAL;

} /* End of iSdbAutoSubmit() */




/* EOF */
