/*
** Module Name:
**    SdbCleanup.c
**
** Purpose:
**    Functions for managing SDB files.
**
** Description:
**    This module contains the functions that are required to
**    cleanup any files that are created by the SDB.
**
**    WARNING: Some of the code in this module may be system specific.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbCleanup.c,v 0.4 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbCleanup.c,v $
**    Revision 0.4  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.3  2002/07/27 11:02:55  mjf
**    Addition of two command-line parameters, to specify the path of the
**    datafile storage, and to specify the number of days after which to
**    clean up SDB files.
**
**    Revision 0.2  2000/10/16 10:49:11  djm
**    Put in a check so that file cleanup is not done when the
**    -nofilestore flag is used.
**
**    Revision 0.1  2000/09/21 10:52:53  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TtlSystem.h"
#include "Tim.h"
#include "Cil.h"
#include "Log.h"
#include "Sdb.h"
#include "SdbPrivate.h"

/* Local function prototypes */


Status_t iSdbCleanup
(
   void
)
{
/*
** Function Name:
**    iSdbCleanup
**
** Type:
**    Status_t
**
** Purpose:
**    Cleanup SDB created files.
**
** Description:
**    Note that this function checks to see if we have the file-storage
**    global variable set. If not, then it wil ignore any operations and
**    return immediately.
**
** Arguments:
**    (none)
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    21-Sep-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;           /* Function return status variables */
   int SysStatus;             /* Return value from the "system()" call */
   char CmdLine[ BUFSIZ ];    /* To construct up command line */


   /* Check if we really want to do this (not necessary for -nofilestore) */
   if(iSdbFileStore != TRUE) return SYS_NOMINAL;


   /* Print a diagnostic file */
   eLogInfo("Cleaning up SDB files");


   /* Check to make sure that a shell is available */
   SysStatus = system(NULL);
   if(SysStatus == 0)
   {
      Status = E_SDB_NO_SHELL;
      eLogErr(Status, "No shell or command handler available");
      return Status;
   }


   /* Sort and remove duplicates of units file */
   sprintf( CmdLine, I_SDB_CLEAN_UNITSFILE_CMD, 
            iSdbDatafilePath, iSdbDatafilePath );
   eLogInfo("Executing: \"%s\"\n", CmdLine);
   SysStatus = system(CmdLine);

   /* Discard old data files */
   sprintf( CmdLine, I_SDB_CLEAN_DATAFILES_CMD, 
            iSdbDatafilePath, iSdbCleanupDays );
   eLogInfo("Executing: \"%s\"\n", CmdLine);
   SysStatus = system(CmdLine);


   return SYS_NOMINAL;

}  /* End of iSdbCleanup() */


/* EOF */
