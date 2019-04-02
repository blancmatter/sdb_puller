/*
** Module Name:
**    SdbReport.c
**
** Purpose:
**    A module containing a reporting functions for the SDB.
**
** Description:
**    This module contains the functions that are required to
**    report messages from the Status Database (SDB) 
**    software. It also contains functions for routine printing
**    of the SDB status to the terminal, in the case that it
**    is run interactively.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbReport.c,v 0.9 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbReport.c,v $
**    Revision 0.9  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.8  2002/01/10 17:40:33  mjf
**    Shortening of report string.
**
**    Revision 0.7  2000/09/08 14:30:07  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.6  2000/08/31 15:34:19  djm
**    Minor formatting change to match process name.
**
**    Revision 0.5  2000/06/07 11:57:58  djm
**    Added description for syslog command line switches.
**
**    Revision 0.4  2000/05/18 13:20:09  djm
**    Implmented new task-data structure mechansim.
**
**    Revision 0.3  2000/05/12 15:12:12  djm
**    Changed display code for global counters.
**
**    Revision 0.2  2000/05/12 13:02:09  djm
**    Addition counter in verbose output.
**
**    Revision 0.1  2000/05/09 08:02:17  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdarg.h>          /* For variable arg-list in iSdbReport() */
#include <time.h>            /* Used for iSdbDisplay() */

#include "Sdb.h"
#include "SdbPrivate.h"


void iSdbReport(char *FormatPtr, ...)
{
/*
** Function Name:
**    iSdbReport
**
** Type:
**    void
**
** Purpose:
**    Message reporting function for the SDB.
**
** Description:
**    iSdbReport provides an ecapsulation of the error and message
**    reporting functions for the Status Database (SDB). It
**    contains any TTL defined logging calls or any other method
**    for reporting information as required. It operates as if the
**    argument list was that of the ASNI/ISO-C standard library 
**    call: printf().
**
** Arguments:
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    28-Mar-2000 djm Initial creation (adapted from iMcbReport()).
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */


   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Format and convey report */
   fprintf(stderr, "SDB: ");
   vfprintf(stderr, FormatPtr, Args);
   fprintf(stderr, "\n");

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of iSdbReport() */




void iSdbDisplay()
{
/*
** Function Name:
**    iSdbDisplay
**
** Type:
**    void
**
** Purpose:
**    Print information about the task to console.
**
** Description:
**    This version will not print more than once per second,
**    unless there is a change in state. This will allow the
**    function to be called repeatedly, without risk of a
**    performance hit for repeated (flushed) writes to standard
**    output.
**
** Arguments:
**    (none - the one for the old mAetDispaly() function was...)
**    struct iAetTaskData *TaskData     (in)
**       Structure containing the data to display.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    12-May-2000 djm Added counter for total & history-limited data
**    04-Apr-2000 djm Initial version (adapted from iMcbDisplay()).
**
*/

   /* Local variables */
   static time_t PrevTime;      /* Time of the last update to stdout */
   static Status_t PrevState;   /* Last state reported to stdout */
   static int Counter = 0;      /* Simple counter to see action */
   time_t CurrentTime;          /* Current time to compare to PrevTime */
   char *Activity = "|/-\\";    /* "Wheel" activity indicator */
   char *StateName[] =          /* Set of names to match "ProcState_t" */
   {
      "INVALID",
      "OKAY",
      "INIT",
      "STANDBY",
      "WARN",
      "FAILED",
      "SAFE",
      "OFF",
      "TIMEOUT"
   };



   /* Note the current time, based on the system clock */
   CurrentTime = time(NULL);

   /* Check for a change in time or state. If not, return */
   if(
      ( CurrentTime == PrevTime ) &&
      ( iSdbTaskData[D_SDB_PROC_STATE].Value == PrevState )
   )
   {
      return;
   }



   /* Print the task status information to standard output */
   fprintf
   (
      stdout,
      "\rSdb: %c %-7s  Dat=%d Tot=%d Hld=%d Sub=%d Rtr=%d Htb=%d Err=%d",
      Activity[Counter++],
      StateName[iSdbTaskData[D_SDB_PROC_STATE].Value],
      iSdbTaskData[D_SDB_QTY_DEFNS].Value,
      iSdbTaskData[D_SDB_TOT_VOLATILE_DATA].Value,
      iSdbTaskData[D_SDB_QTY_HISTLIM].Value,
      iSdbTaskData[D_SDB_QTY_SUBMITTED].Value,
      iSdbTaskData[D_SDB_QTY_RETRIEVED].Value,
      iSdbTaskData[D_SDB_QTY_HBEAT].Value,
      iSdbTaskData[D_SDB_QTY_ERRORS].Value
   );

   fflush(stdout);

   /* Keep the activity wheel counter in range */
   if(Counter >= 4) Counter = 0;

   /* Remember what we've just output to screen */
   PrevTime = CurrentTime;
   PrevState = iSdbTaskData[D_SDB_PROC_STATE].Value;


} /* End of iSdbDisplay() */



void iSdbUsage(
   char *ExecNamePtr,
   char *MessagePtr
)
{
/*
** Function Name:
**    iSdbUsage
**
** Type:
**    void
**
** Purpose:
**    Print an error message regarding the correct usage of this
**    program.
**
** Description:
**    The command line arguments (including default behaviours) are
**    defined in SdbPrivate.h.
**
** Arguments:
**    char *ExecNamePtr        (in)
**       Character string containg the name of the executable.
**    char *MessagePtr         (in)
**       A null-terminated character string containg a 
**       diagnostic message to print. This character string is 
**       prefixed by "ERROR: " and is written to stderr. It 
**       should not contain an newline ("\n") character at the
**       end of the string. If this variable MessagePtr is set
**       to NULL, then no error message will be printed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    07-Jun-2000 djm Added -(no)syslog switch descriptions.
**    26-Apr-2000 djm Added "any" CIL ID specification.
**    19-Apr-2000 djm Added CIL test unit usage text.
**    23-Feb-2000 djm Initial version.
**
*/

   /* No local variables */


   /* If we have an associated error message to print, then do so. */
   if(MessagePtr != NULL)
   {
      fprintf(stderr, "ERROR: %s\n", MessagePtr);
   }

   /* Print information on how to use the application */
   fprintf(stderr, "\nUsage: %s [options]\n", ExecNamePtr);
   fprintf(stderr,
      "Options:\n"
      "                  (no option) Normal program operation\n"
      " -help            Print this text and exit\n"
      " -quiet           Suppress output (opposite of -verbose)\n"
      " -cil NAME        Use CIL ID (specified by case-sensitive NAME)\n"
      " -nosyslog        Do not submit message to syslog network log\n"
      " -syslog          Send messages to syslog network based log file\n"
      " -tu#             Use CIL test unit ID TU# (where #=0-9) instead of SDB\n"
      " -verbose         Continuously print task info to console (default)\n\n"
   );

   /* There is no return value */


} /* End of iSdbUsage() */


/* EOF */
