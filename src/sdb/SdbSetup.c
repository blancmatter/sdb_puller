/*
** Module Name:
**    SdbSetup.c
**
** Purpose:
**    A module containing SDB initialisation functions.
**
** Description:
**    This module contains the functions that are required to
**    initalise the Status Database (SDB) software.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbSetup.c,v 0.14 2007/07/17 10:20:02 mjf Exp $
**
** History:
**    $Log: SdbSetup.c,v $
**    Revision 0.14  2007/07/17 10:20:02  mjf
**    Add -port and -host command line switches to define destination
**    of optional UDP packets containing Sdb data for logging to external
**    database.
**
**    Revision 0.13  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.12  2006/06/26 15:53:02  mjf
**    Cache time to reduce system calls.
**
**    Revision 0.11  2005/06/20 13:35:31  mjf
**    Minor changes to aid porting to Linux - no functional changes.
**
**    Revision 0.10  2002/07/27 11:02:55  mjf
**    Addition of two command-line parameters, to specify the path of the
**    datafile storage, and to specify the number of days after which to
**    clean up SDB files.
**
**    Revision 0.9  2000/12/11 16:17:21  mjf
**    Use CLU to perform CIL setup and submit version into the SDB.
**
**    Revision 0.8  2000/10/12 14:46:16  mjf
**    Perform 'eCilSetup' with specified CIL map, rather than hard-coded.
**
**    Revision 0.7  2000/09/27 11:52:18  djm
**    Added call to cleanup function.
**
**    Revision 0.6  2000/09/08 13:03:00  djm
**    Added last-received-heartbeat-time initialisation.
**
**    Revision 0.5  2000/09/01 09:35:23  djm
**    Added code to initialise the file list array. Also improved
**    the eCluSignOn() function arguments.
**
**    Revision 0.4  2000/08/30 11:20:34  djm
**    Fixed a trivial syntac error.
**
**    Revision 0.3  2000/08/30 11:17:25  djm
**    Implemented CLU package calls for command line argument handling.
**
**    Revision 0.2  2000/06/07 11:52:24  djm
**    Added LOG functions for error/message reporting.
**
**    Revision 0.1  2000/05/09 08:12:41  djm
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

static Status_t mSdbGetFileStore( void );



Status_t iSdbSetup
(
   int argc,
   char *argv[],
   eCilMsg_t *MsgPtr
)
{
/*
** Function Name:
**    iSdbSetup
**
** Type:
**    Status_t
**
** Purpose:
**    Main setup function for the SDB.
**
** Description:
**    Top level setup function of the SDB. It is called on startup
**    from main() and contains all the necessary calls to be able
**    to start processing.
**
** Arguments:
**    int argc                 (in)
**       Number of arguments on the command line (including the
**       executable name).
**    char *argv[]             (in)
**       Array of null-terminated character strings containing
**       the command line arguments.
**    eCilMsg_t *MsgPtr      (in/out)
**       Pointer to a CIL message structure. During setup, a block
**       of memory is allocated for this message. The size of this
**       memory block is defined in the SdbPrivate.h header file.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    21-Sep-2000 djm Added clean-up of the units file (if used)
**    05-Sep-2000 djm Added initialisation of heartbeat time.
**    29-Aug-2000 djm Rewritten to use the CLU package.
**    07-Jun-2000 djm Added LOG function for error reporting.
**    05-May-2000 djm Initial creation (adapted from iSdbSetup()).
**
*/

   /* Local variables */
   Status_t Status;           /* Function return status variables */
   int Index;                 /* Loop counter for file list initialisation */

   /*
   ** Insert the default values for command line arguments
   */

   eCluProgNamePtr           = I_SDB_PROGRAM_NAME;
   eCluProgAboutPtr          = I_SDB_PROGRAM_ABOUT;
   eCluReleaseDatePtr        = I_SDB_RELEASE_DATE;
   eCluYearPtr               = I_SDB_YEAR;
   eCluMajorVer              = I_SDB_MAJOR_VERSION;
   eCluMinorVer              = I_SDB_MINOR_VERSION;

   eCluCommon.Quiet          = I_SDB_DFLT_QUIET;
   eCluCommon.Verbose        = I_SDB_DFLT_VERBOSE;
   eCluCommon.Syslog         = I_SDB_DFLT_SYSLOG;
   eCluCommon.DebugLevel     = I_SDB_DFLT_DEBUG;
   eCluCommon.Priority       = I_SDB_DFLT_PRIORITY;
   eCluCommon.Help           = I_SDB_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, I_SDB_DFLT_LOG );
   strcpy( eCluCommon.CilName,    I_SDB_DFLT_CIL );
   strcpy( eCluCommon.CilMap,     I_SDB_DFLT_CIL_MAP );


   /*
   ** Handle the command line arguments using the CLU package calls
   */

   Status = eCluSetup(argc, argv, E_CLU_IGN_CONFIGFILE);
   if(Status != SYS_NOMINAL)
   {
      fprintf(
         stderr,
         I_SDB_PROGRAM_NAME   /* <-- NB: No comma, uses string concatenation */
         ": <%x> Failed to setup command line argument parsing\n",
         Status
      );
      return Status;
   }


   Status = eCluParseArgs(NULL);
   if(Status != SYS_NOMINAL)
   {
      fprintf(
         stderr,
         I_SDB_PROGRAM_NAME   /* <-- NB: No comma, uses string concatenation */
         ": <%x> Unable to parse arguments\n",
         Status
      );
      return Status;
   }


   if(eCluCommon.Help == TRUE)
   {
      exit(EXIT_SUCCESS);
   }


   Status = eCluSignOn(NULL, I_SDB_PROGRAM_TITLE);
   if(Status != SYS_NOMINAL)
   {
      fprintf(
         stderr,
         I_SDB_PROGRAM_NAME   /* <-- NB: No comma, uses string concatenation */
         ": <%x> Unable to sign-on program\n",
         Status
      );
      return Status;
   }


   /* Recover the -filestore command line argument */
   Status = mSdbGetFileStore();
   if(Status != SYS_NOMINAL)
   {
      eLogErr(
         Status, "Error parsing the flag -%s/-%s",
         eCluCustomArg[I_SDB_CUSTOM_FILESTORE].Switch,
         eCluCustomArg[I_SDB_CUSTOM_NOFILESTORE].Switch
      );
      return Status;
   }


   /* Check for the specification of an alternative datafile path */
   if ( eCluCustomArgExists( I_SDB_CUSTOM_DATAPATH ) == E_CLU_ARG_SUPPLIED )
   {
      /* Take copy of the alternative path */
      strncpy( iSdbDatafilePath, eCluGetCustomParam( I_SDB_CUSTOM_DATAPATH ), 
               I_SDB_MAX_FILENAME - 1 );
      iSdbDatafilePath[ I_SDB_MAX_FILENAME - 1 ] = '\0';
      /* If the last character of the string is not '/', then add one */
      if ( iSdbDatafilePath[ strlen( iSdbDatafilePath ) - 1 ] 
            != I_SDB_PATH_SEPARATOR )
      {
         /* Add a string termination one character beyond the current one */
         iSdbDatafilePath[ strlen( iSdbDatafilePath ) + 1 ] = '\0';
         /* Add the character onto the end of the string */
         iSdbDatafilePath[ strlen( iSdbDatafilePath ) ] = I_SDB_PATH_SEPARATOR;
      }
      eLogNotice( 0, "SDB file storage specified in '%s'", iSdbDatafilePath );
   }
   /* else store files in the default place */
   else
   {
      /* Take copy of the default path */
      strcpy( iSdbDatafilePath, I_SDB_DB_FILE_PATH );
      eLogInfo( "Default SDB file storage in '%s'", iSdbDatafilePath );
   }

   /* Check for the specification of file cleanup days */
   if ( eCluCustomArgExists( I_SDB_CUSTOM_CLEANUP ) == E_CLU_ARG_SUPPLIED )
   {
      /* Get the supplied parameter and store */
      iSdbCleanupDays = strtol( eCluGetCustomParam( I_SDB_CUSTOM_CLEANUP ), 
                                0, 0 );
   }

   /* Default to not sending to an SQL database. */
   iSdbSendToSql = FALSE;

   /* Check for mysql host argument */
   if( eCluCustomArgExists( I_SDB_CUSTOM_MYSQLHOST ) == E_CLU_ARG_SUPPLIED )
   {
      /* mysql host has been specified */
      strcpy(iSdbMySqlHost, eCluGetCustomParam( I_SDB_CUSTOM_MYSQLHOST ));
      eLogNotice( 0, "MySql host specified `%s`", iSdbMySqlHost);
      iSdbSendToSql = TRUE;
   }
   else
   {
      strcpy(iSdbMySqlHost, I_SDB_DFLT_MYSQLHOST);
      eLogInfo( "Default MySql host `%s`",iSdbMySqlHost);
   }

   /* Check for mysql port argument */
   if( eCluCustomArgExists( I_SDB_CUSTOM_MYSQLPORT ) == E_CLU_ARG_SUPPLIED )
   {
      /* mysql port has been specified */
      strcpy(iSdbMySqlPort, eCluGetCustomParam( I_SDB_CUSTOM_MYSQLPORT ));
      eLogNotice( 0, "MySql port specified `%s`", iSdbMySqlPort);
      iSdbSendToSql = TRUE;
   }
   else
   {
      strcpy(iSdbMySqlPort, I_SDB_DFLT_MYSQLPORT);
      eLogInfo( "Default MySql port `%s`",iSdbMySqlPort);
   }

   /* Submit version into the SDB - CIL communications is set up in here */
   Status = eCluSubmitVersion();

   /* Check the returned status */
   if( Status != SYS_NOMINAL )
   {
      eLogErr(
         Status, "Unable to setup CIL comms and submit SDB version"
      );
      return Status;
   }

   /* Take copy of the local CIL ID to use */
   iSdbCilId = eCilId();


   /* Allocate some space for message contents */
   MsgPtr->DataPtr = TTL_MALLOC(I_SDB_DATASIZE);
   if(MsgPtr->DataPtr == NULL)
   {
      Status = E_SDB_GEN_ERR;
      eLogCrit(Status, "Insufficient memory for message storage");
      return Status;
   }

   /*
   ** Initialise the file list
   */

   for(Index = 0; Index < I_SDB_MAX_DB_FILES; Index++)
   {
      iSdbDbFileList[Index].FilePtr = NULL;
      iSdbDbFileList[Index].StartTime.t_sec = 0;
      iSdbDbFileList[Index].StartTime.t_nsec = 0;
      iSdbDbFileList[Index].LastAccessed.t_sec = 0;
      iSdbDbFileList[Index].LastAccessed.t_nsec = 0;
   }

   /*
   ** Do any pre-start clean-up.
   */

   Status = iSdbCleanup();
   if(Status != SYS_NOMINAL)
   {
      /* Write a warning, but don't return out of the function */
      eLogWarning(Status, "Problem encountered during clean-up");
   }

   /*
   ** Initialise the time of last received heartbeat
   */

   Status = eTimCacheTime(&iSdbHeartBeatTime);
   if(Status != SYS_NOMINAL)
   {
      eLogCrit(Status, "Error initialising last heartbeat time");
      return Status;
   }


   /* If we get this far, then we've completed succesfully. */
   return SYS_NOMINAL;

} /* End of iSdbSetup() */







static Status_t mSdbGetFileStore( void )
{
/*
** Function Name:
**    mSdbGetFileStore
**
** Type:
**    Status_t
**
** Purpose:
**    Extract the filestore flag from the command line argument.
**
** Description:
**    Determine from the command line arguments whether or not file storage
**    is required and put the result into a global variable.
**
** Arguments:
**    (none)
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    29-Aug-2000 djm Initial creation.
**
*/
   /* No local variable */


   if (eCluCustomArgExists( I_SDB_CUSTOM_FILESTORE ) == E_CLU_ARG_SUPPLIED )
   {
      iSdbFileStore = TRUE;
      eLogInfo("Setting SDB file storage ON");
   }

   if (eCluCustomArgExists( I_SDB_CUSTOM_NOFILESTORE ) == E_CLU_ARG_SUPPLIED )
   {
      iSdbFileStore = FALSE;
      eLogInfo("Setting SDB file storage OFF");
   }


   return SYS_NOMINAL;

}  /* End of mSdbGetFileStore() */


/* EOF */

