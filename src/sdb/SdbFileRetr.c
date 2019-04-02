/*
** Module Name:
**    SdbFileRetr.c
**
** Purpose:
**    Functionality to spawn a task to recover data from storage file.
**
** Description:
**    In addition to the RAM maintained databse, the SDB writes data
**    to a "storage file". These storage files are kept on disk and
**    contain a record of the received data. In order to recover this
**    data and serve it to a requesting process, the files must be
**    opened, searched and the formatted reply sent. Unlike the RAM
**    stored database, this operation may take some time, and the 
**    response of the SDB to other requests is compromised. In order
**    to allow lengthy file recoveries to be made, the functions in
**    this module are used to spawn a new process for each request.
**    The new process (the SDB File Recovery task, or SFR) inherits
**    the file request from the parent process (the SDB).
**
**    NOTE: This module contains some QNX specific code. On non-QNX
**          machines, this functionality is ignored (but the remainder
**          of the program should still compile and run).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbFileRetr.c,v 0.10 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbFileRetr.c,v $
**    Revision 0.10  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.9  2004/01/16 08:55:04  sxf
**    Minor changes to port to QNX6.
**
**    Revision 0.8  2001/10/23 09:35:04  mjf
**    Addition of RETRIEVE_L service, to retrieve the last 'n' measurements
**    from disk - like RETRIEVE_F but for the last (not first) measurements.
**
**    Revision 0.7  2001/01/03 13:32:44  mjf
**    Addition of missing error replies.
**
**    Revision 0.6  2000/10/04 11:05:41  djm
**    Fixed problem in error reporting.
**
**    Revision 0.5  2000/09/05 14:17:07  djm
**    Added deliverer ID variable for correct message handling.
**
**    Revision 0.4  2000/08/29 14:35:22  djm
**    Added handling of a sequence number command line argument.
**
**    Revision 0.3  2000/08/29 10:24:13  djm
**    Fixed verbose logging for SFR arguments.
**
**    Revision 0.2  2000/08/22 09:15:25  djm
**    Reworked the argument handling for the spawning of the SFR task.
**
**    Revision 0.1  2000/08/07 15:31:23  djm
**    Initial creation (note, this file contains some non-ANSI code).
**
**
*/

/* Definition for the SdbPrivate.h file to know to intialise a global array */

#define M_SDB_FILERETR


/* Include files */

#include "TtlSystem.h"
#include "Wfl.h"
#include "Log.h"
#include "Sdb.h"
#include "SdbPrivate.h"

#ifdef E_WFL_OS_QNX4
#include <process.h>
#include <sys/types.h>
#include <sys/qnx_glob.h>
#include <string.h>
#endif

#ifdef E_WFL_OS_QNX4

#include <process.h>

#endif


/* Functions */


Status_t iSdbFileRetr(
   Int32_t DelivererId,
   eCilMsg_t *MsgPtr,
   Bool_t LastData
)
{
/*
** Function Name:
**    iSdbFileRetr
**
** Type:
**    Status_t
**
** Purpose:
**    Spawn a process to recover data from an SDB storage file.
**
** Description:
**    ...
**
**    Note: This function contains some QNX specific code.
**
** Arguments:
**    Int32_t DelivererId    (in)
**       CIL ID code (definitions in Cil.h) of the process that sent
**       the CIL message (MsgPtr) to the SDB.
**    eCilMsg_t *MsgPtr      (in)
**       A pointer to a CIL message sent to the SDB, that requires 
**       processing.
**    Bool_t LastData        (in)
**       Flag indicating whether the last data in the time range is to
**       be retrieved, otherwise the first data will be obtained.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-Oct-2001 mjf Added support to retrieve first or last data.
**    03-Jan-2001 mjf Added sending of missing error replies.
**    05-Sep-2000 djm Added deliverer ID to arguments list.
**    16-Aug-2000 djm Modified to handle new SFR executable.
**    18-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */

   /* Do not include the function body we are not doing file recovery */
#ifdef I_SDB_SFR_USE

#if defined E_WFL_OS_QNX4 || defined E_WFL_OS_QNX6
   int SpawnStatus;          /* Function return from QNX spawn call */
#if 0
   char ArgBuf[I_SDB_SFR_ARGBUFSIZE];
#endif
   char *ArgBufPtr;          /* Temp pointer to argument string buffer */
   int Ctr;
   int Arg;                  /* Loop counter for aguments for SFR */
#endif


   /* Check message length */
   if(MsgPtr->DataLen != sizeof(eSdbMulReq_t))
   {
      Status = E_SDB_TRUNCATED;
      eLogErr(
         Status,
         "File retrieve request from source 0x%x "
         "of incorrect length (%d expected, %d received)",
         MsgPtr->SourceId, sizeof(eSdbMulReq_t), MsgPtr->DataLen
      );
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }


#if defined E_WFL_OS_QNX4 || defined E_WFL_OS_QNX6

   /* Put the message into an ACSII buffer, with the data hex-encoded */

   /* Set any global flags that control spawned process behaviour */
/*   qnx_spawn_options.priority = I_SDB_SFR_PRIORITY; */
#if defined E_WFL_OS_QNX4

   qnx_spawn_options.flags = _SPAWN_NOZOMBIE;

#endif

   /*
   ** Loop through the arguments, and replace the value arguments that
   ** follow any of the "custom" arguments with the correct values. The
   ** ones that are defined in the SdbPrivate.h file are used only as
   ** placeholders, to ensure that sufficient memory is allowed for the
   ** dynamically set values (done here).
   */
   for(Arg = 0; iSdbSfrArgv[Arg] != NULL; Arg++)
   {
      eLogInfo("Checking arg.%d (=%s)", Arg, iSdbSfrArgv[Arg]);

      /*
      ** Check to see if we have any switch that needs to have the
      ** argument that follows altered. Note the implied string
      ** concatenation between the "-" switch marker and the "E_SFR_XXX"
      ** symbolic constant (which doesn't have the "-" included).
      */ 
      if(strcmp(iSdbSfrArgv[Arg], "-" E_SFR_REQUESTSTR) == 0)
      {
         ArgBufPtr = iSdbSfrArgv[Arg+1];
         for(Ctr = 0; Ctr < MsgPtr->DataLen; Ctr++)
         {
            sprintf(ArgBufPtr, "%2.2x", ((char *)(MsgPtr->DataPtr))[Ctr]);
            ArgBufPtr += 2;
         }
         ArgBufPtr = '\0';
      }
      else if(strcmp(iSdbSfrArgv[Arg], "-" E_SFR_SENDTOSTR) == 0)
      {
         sprintf(iSdbSfrArgv[Arg+1], "%d", MsgPtr->SourceId);
      }
      else if(strcmp(iSdbSfrArgv[Arg], "-" E_SFR_SEQNUMSTR) == 0)
      {
         sprintf(iSdbSfrArgv[Arg+1], "%d", MsgPtr->SeqNum);
      }
      else if(strcmp(iSdbSfrArgv[Arg], "-" E_SFR_LASTSTR) == 0)
      {
         sprintf(iSdbSfrArgv[Arg+1], "%d", LastData);
      }

   }  /* End of for-loop, checking the arguments */

   /* Spawn the process, with the hex-encoded-data buffer as the argument */
   SpawnStatus = spawnv(P_NOWAIT, I_SDB_SFR_PATHNAME, iSdbSfrArgv);
   if(SpawnStatus == -1)
   {
      Status = E_SDB_SFR_SPAWN_FAIL;
      iSdbErrReply(DelivererId, MsgPtr, Status);
      return Status;
   }

   /* Return */
   return SYS_NOMINAL;

#else

   /* Reply with an error indicating that the file retrieval is not available */
   Status = E_SDB_SFR_NOT_AVAIL;
   eLogErr(
      Status,
      "Data recovery from a file has not been implemented on this system"
   );
   iSdbErrReply(DelivererId, MsgPtr, Status);
   return Status;

#endif


#else   /* #ifdef I_SDB_SFR_USE */
   Status = E_SDB_SFR_NOT_AVAIL;
   eLogErr(
      Status,
      "Data recovery from a file has not been enabled in this version"
   );
   iSdbErrReply(MsgPtr, Status);
   return Status;
#endif  /* #ifdef I_SDB_SFR_USE */


}  /* End of iSdbFileRetr() */




/* EOF */
