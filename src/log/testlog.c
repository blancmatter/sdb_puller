/*
** Module Name:
**    testlog.c
**
** Purpose:
**    A test program to check logging function design.
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testlog.c,v 0.4 2002/04/10 09:18:46 djm Exp $
**
** History:
**    $Log: testlog.c,v $
**    Revision 0.4  2002/04/10 09:18:46  djm
**    Added some experimental function calls to test whether the
**    eLogDebug() and eLogInfo() functions can cope with a NULL
**    pointer to the format string (which is the most common of
**    the usage errors of these functions).
**
**    Revision 0.3  2000/11/21 10:09:42  djm
**    Updated code to test the new eLogSetup() API.
**
**    Revision 0.2  2000/06/06 12:27:16  djm
**    Removed debugging code.
**
**    Revision 0.1  2000/06/05 11:33:18  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>         /* For main() only */
#include <syslog.h>         /* For syslog */

#include "TtlSystem.h"
#include "Log.h"
/*
#include "Cil.h"
*/

/* Symbolic constants */

/*
#define MY_MAP  "/opt/ttl/src/cil/Cil.map"
#define MY_ID   E_CIL_TU4
*/

int main(int argc, char *argv[])
{
/*
** Function Name:
**    main
**
** Type:
**    void
**
** Purpose:
**    Top level function for test program.
**
** Description:
**    This function is a test stub for testing a generic TTL
**    program message reporting function.
**
** Arguments:
**    (not used)
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Adapted from prototype code.
**    01-Jun-2000 djm Added eLogSetup() calls.
**    03-Apr-2000 djm Adapted from McbReport.c.
**    28-Mar-2000 djm Initial creation.
**
*/
   /* Local variables */
   int Prio;
   Status_t Status;

   /* Set up LOG parameters */
/*
   Status = eCilSetup(MY_MAP, MY_ID);
   if(Status != SYS_NOMINAL)
   {
      printf("Failed to complete CIL setup\n");
      exit( EXIT_FAILURE );
   }
  */
    
   Status = eLogSetup("Sys", "S01", "./tmp.log", 
                      E_LOG_UPTO(E_LOG_INFO), TRUE, TRUE);
   if(Status != SYS_NOMINAL)
   {
      printf("Failed to complete LOG setup\n");
      exit( EXIT_FAILURE );
   }

   /* Make some log calls */

   Status = 20;

   eLogReport(1, Status, "====================================");

   for(Prio = E_LOG_EMERG-1; Prio <= E_LOG_DEBUG; Prio++)
   {
      eLogReport(Prio, Status, "Test of eLogReport() with Prio=%d", Prio);
   }

   eLogDebug("Test of eLogDebug() (no status supplied)");
   eLogInfo("Test of eLogInfo() (no status supplied)");
   eLogNotice(Status, "Test of eLogNotice() with Status=%d", Status);
   eLogWarning(Status, "Test of eLogWarning() with Status=%d", Status);
   eLogErr(Status, "Test of eLogErr() with Status=%d", Status);
   eLogCrit(Status, "Test of eLogCrit() with Status=%d", Status);
   eLogAlert(Status, "Test of eLogAlert() with Status=%d", Status);
   eLogEmerg(Status, "Test of eLogEmerg() with Status=%d", Status);

   eLogReport(1, Status, "====================================");
   eLogReport(1, Status, "Testing error handling... ");
   eLogDebug(0, "Test of eLogDebug() (with incorrectly supplied status)");
   eLogInfo(0, "Test of eLogInfo() (with incorrectly supplied status)");


/*
   THE FOLLOWING SYSLOG MESSAGE LEVELS ARE AVAILABLE...

   eLogDebug(F,...);           7
   eLogInfo(F,...);            6
   eLogNotice(S,F,...);        5
   eLogWarning(S,F,...);       4
   eLogError(S,F,...);         3
   eLogCrit(S,F,...);          2
   eLogAlert(S,F,...);         1
   eLogEmerg(S,F,...);         0
*/

   return EXIT_SUCCESS;

}  /* End of main() */

/* EOF */
