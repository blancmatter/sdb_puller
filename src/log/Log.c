/*
** Module Name:
**    Log.c
**
** Purpose:
**    System message logging functions.
**
** Description:
**    The TTL telescope control software is distributed over multiple
**    processes running over several difference platforms and operating
**    systems. There has already been developed a system for passing
**    engineering data about the system, which is handled via the
**    System Database (SDB). The system logging package (LOG) is used
**    for distributing error messages and sporadic text information to 
**    a central point for storage. It allows any applications to submit
**    such text messages without needing to worry about establishing
**    connections, opening log files, etc..
**
**    There are three possible destinations for messages that are 
**    reported via the LOG package functions. These are a local disk 
**    file, a network based remote disk file and terminal standard 
**    output.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    sxf: Steve Foale (TTL)
**    mjf: Martyn J. Ford (TTL)
**
** Version:
**    $Id: Log.c,v 0.14 2002/09/10 12:50:59 sxf Exp $
**
** History:
**    $Log: Log.c,v $
**    Revision 0.14  2002/09/10 12:50:59  sxf
**    Use vsnprintf() rather than vsprintf() to compose string without
**    overflowing declared memory.
**
**    Revision 0.13  2002/04/10 09:12:24  djm
**    Prevented the segmentation violation caused by eLogDebug()
**    or eLogInfo() being called with a status argument (as
**    required by other functions in the LOG series). In this
**    case, the function will now return, having written a
**    warning message to the normal logging path. This will be
**    written regardless to whether the priority level is set
**    to normally reporting such a message. This is because the
**    warning is reported at the CRITICAL level, as it represents
**    a failure in the logging.
**
**    Revision 0.12  2001/12/05 17:43:15  djm
**    Added use of a facility (E_LOG_FACILITY) code.
**
**    Revision 0.11  2000/12/18 12:13:21  mjf
**    Move the completion of the use of CLU, to complete whether or not a
**    configuration file is used.
**
**    Revision 0.10  2000/12/18 11:12:40  mjf
**    Code added to use optional configuration parameters to specify the
**    logging parameters.
**
**    Revision 0.9  2000/11/23 11:13:33  mjf
**    If host for logging is unresolved, then return E_LOG_HOST_UNKNOWN.
**
**    Revision 0.8  2000/11/21 10:08:33  djm
**    Modified eLogSetup() et al. to include an instance name.
**
**    Revision 0.7  2000/10/04 11:01:11  djm
**    Added connection/fire-and-forget options. Improved address lookup.
**
**    Revision 0.6  2000/08/11 16:30:16  djm
**    Fixed error in comment block for eLogSetup().
**
**    Revision 0.5  2000/08/11 16:19:36  djm
**    Allowed an empty-string to be treated the same as a NULL pointer
**    for the local file argument in eLogSetup().
**
**    Revision 0.4  2000/06/07 16:38:45  djm
**    Added more comments to some function header blocks.
**
**    Revision 0.3  2000/06/06 13:35:31  djm
**    Change name convention for "print-to-net-log" flag.
**
**    Revision 0.2  2000/06/06 12:26:25  djm
**    Added direct UDP code for trasmitting syslog messages.
**
**    Revision 0.1  2000/06/05 11:31:43  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


#ifdef __VMS

#include <time.h>
#include <unixio.h>          /* Prototypes for UNIX emulation functions */
#include <socket.h>          /* TCP UDP header */
#include <in.h>              /* TCP UDP header */
#include <inet.h>            /* TCP UDP header */
#include <netdb.h>

#else

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>      /* TCP UDP headers */
#include <netinet/in.h>      /* TCP UDP headers */
#include <arpa/inet.h>       /* TCP UDP headers */
#include <netdb.h>

#endif


#include "TtlSystem.h"
#include "Log.h"
#include "LogPrivate.h"
#include "LogSnprintf.h"
#include "Cfu.h"


/* Function prototypes */

void mLogProcess(int Priority, Status_t Status,
                 const char *FormatPtr, va_list Args);


/* Functions */


Status_t eLogSetup(
   const char *PrefixPtr,
   const char *InstancePtr,
   const char *FileNamePtr,
   int Prio,
   Bool_t PrintNet,
   Bool_t PrintCon
)
{
/*
** Function Name:
**    eLogSetup
**
** Type:
**    Status_t
**
** Purpose:
**    Setup function for system-wide message reporting.
**
** Description:
**    This function will report a user provided message and Status
**    code at a specified priority. Depending on the configuration
**    of the logging for the application (controlled by eLogSetup),
**    this message may be sent to either a local disk log, a UDP
**    accesses network log or to the terminal console. It operates
**    as if the third argument and those that follow were those of
**    the ASNI/ISO-C standard library call: printf().
**
** Arguments:
**    const char *PrefixPtr           (in)
**       This is the name to be associated with the logged messages.
**       This should be the applications three letter acronym (e.g.
**       CMT).
**    const char *InstancePtr         (in)
**       This is the name to be associated with the logged messages
**       for a specific instance of the application. This should
**       normally be the applications three letter CIL address acronym
**       (e.g. CM0), but it may be any other identifier.
**    const char *FileNamePtr         (in)
**       A pointer to a character string that contains the name of 
**       a disk file for the application to use directly. If this 
**       pointer is set to NULL, then no disk log is used.
**    int PrioMask                    (in)
**       The priority mask to use. This determines message of which 
**       priority are sent to the log. Typically, the macro E_LOG_UPTO()
**       is used in conjunction with a priority (e.g. E_LOG_NOTICE). 
**       E_LOG_MASK is used for a single priority. Priority masks may
**       be logically-OR-ed together.
**    Bool_t PrintNet                 (in)
**       If TRUE, then subsequent logging functions will send messages 
**       to the network based syslog.
**    Bool_t PrintCon                 (in)
**       If TRUE, then subsequent logging functions will send messages
**       to the terminal console.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    sxf: Steve Foale (TTL)
**    mjf: Martyn J. Ford (TTL)
**
** History:
**    23-Nov-2000 mjf Check for log destination host name missing.
**    20-Nov-2000 djm Added instance name into the argument list.
**    27-Sep-2000 djm Corrected for remote host operation (djm + sxf)
**    06-Jun-2000 djm Updated the comment header block.
**    05-Jun-2000 djm Reworked to actually open a socket.
**    01-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   struct hostent *HostEntPtr;      /* Host entry - address details */
   struct servent *SrvEntPtr;       /* Service entry */
   struct sockaddr_in *SinPtr;
   static Bool_t FirstTime = TRUE;  /* To detect if we are on the first pass */
   size_t StringLen;                /* Temp. variable for length of strings */
#ifdef I_LOG_CONNECTION_BASED
   int ConnectStatus;               /* Status of the connect() call */
#endif
   Status_t CfuStatus;              /* Status returned by CFU functions */
   Status_t ParamStatus;            /* Parameter status returned by CFU funcs */
   char Buffer  [ I_LOG_MAXSIZE ];  /* String for retrieving parameters */
   char Name    [ I_LOG_MAXSIZE ];  /* String containing service */
   char Protocol[ I_LOG_MAXSIZE ];  /* String containing protocol */
   char Host    [ I_LOG_MAXSIZE ];  /* String containing host */
   int  Port;                       /* Integer containing port */


   /* Do any first time initialisation of variables */
   if(FirstTime == TRUE)
   {
      iLogSkt = -1;
   }

   /* If we don't have a set-up socket, then set one up */
   if(iLogSkt == -1)
   {
      /* Initially assume defaults for setting up logging */
      strcpy( Name,     I_LOG_DEFNAME );
      strcpy( Protocol, I_LOG_DEFPROT );
      strcpy( Host,     I_LOG_DEFHOST );
      Port = I_LOG_DEFPORT;

      /* Use CFU to get any configuration data for LOG */
      CfuStatus = eCfuSetup( I_LOG_DFLT_CFG_FILE );

      /* If the configuration file has been opened OK */
      if ( CfuStatus == SYS_NOMINAL )
      {
         /* Loop until the end of the file */
         do
         {
            /* Retrieve a line from the configuration file */
            CfuStatus = eCfuGetLine( NULL );

            /* If a line retrieved OK */
            if ( CfuStatus == SYS_NOMINAL )
            {
               /* Get the label parameter */
               ParamStatus = eCfuGetNumParam( I_LOG_CFG_PARAM_NAME, Buffer );
               /* If a parameters successfully retrieved */
               if ( ParamStatus == SYS_NOMINAL )
               {
                  /* Attempt to match the name label */
                  if ( strcmp( Buffer, I_LOG_LABEL_SERVICE ) == 0 )
                  {
                     /* Get the associated parameter */
                     ParamStatus = eCfuGetNumParam( I_LOG_CFG_PARAM_ARG, 
                                                    Buffer );
                     /* If parameters succesfully retrieved */
                     if ( ParamStatus == SYS_NOMINAL )
                     {
                        /* Take copy of the parameter */
                        strcpy( Name, Buffer );
                     }
                  }
                  /* Attempt to match the protocol label */
                  if ( strcmp( Buffer, I_LOG_LABEL_PROTOCOL ) == 0 )
                  {
                     /* Get the associated parameter */
                     ParamStatus = eCfuGetNumParam( I_LOG_CFG_PARAM_ARG, 
                                                    Buffer );
                     /* If parameters succesfully retrieved */
                     if ( ParamStatus == SYS_NOMINAL )
                     {
                        /* Take copy of the parameter */
                        strcpy( Protocol, Buffer );
                     }
                  }
                  /* Attempt to match the host label */
                  if ( strcmp( Buffer, I_LOG_LABEL_HOST ) == 0 )
                  {
                     /* Get the associated parameter */
                     ParamStatus = eCfuGetNumParam( I_LOG_CFG_PARAM_ARG, 
                                                    Buffer );
                     /* If parameters succesfully retrieved */
                     if ( ParamStatus == SYS_NOMINAL )
                     {
                        /* Take copy of the parameter */
                        strcpy( Host, Buffer );
                     }
                  }
                  /* Attempt to match the port label */
                  if ( strcmp( Buffer, I_LOG_LABEL_PORT ) == 0 )
                  {
                     /* Get the associated parameter */
                     ParamStatus = eCfuGetNumParam( I_LOG_CFG_PARAM_ARG, 
                                                    Buffer );
                     /* If parameters succesfully retrieved */
                     if ( ParamStatus == SYS_NOMINAL )
                     {
                        /* Take copy of the parameter */
                        Port = atoi( Buffer );
                     }
                  }
               }
            }
         }
         while ( CfuStatus == SYS_NOMINAL );
      }

      /* Complete use of configuration file */
      CfuStatus = eCfuComplete();

      /* Set up the basic address structure */
      SinPtr = (struct sockaddr_in *) &iLogSktAddr;
      SinPtr->sin_family = AF_INET;
      SinPtr->sin_addr.s_addr = htonl (INADDR_ANY);

      /* Check if we can access the server entity */
      SrvEntPtr = getservbyname( Name, Protocol );

      /* If there is one, then fill in the details, otherwise take the default */
      if (SrvEntPtr)
      {
         SinPtr->sin_port = SrvEntPtr->s_port;
      }
      else
      {
         SinPtr->sin_port = htons( Port );
      }

      /* Get host address & put in address structure */
      HostEntPtr = gethostbyname( Host );
      if ( HostEntPtr == NULL )
      {
         return E_LOG_HOST_UNKNOWN;
      }
      SinPtr->sin_addr = *(struct in_addr *) HostEntPtr->h_addr;

      /* Open the socket and connect it */
      iLogSkt = socket (AF_INET, SOCK_DGRAM, 0);
      if(iLogSkt == -1)
      {
         return E_LOG_SOCKET_ERROR;
      }

#ifdef I_LOG_CONNECTION_BASED
      ConnectStatus = connect(iLogSkt, &iLogSktAddr, sizeof(iLogSktAddr));
      if(ConnectStatus == -1)
      {
         return E_LOG_CONNECT_ERROR;
      }
#endif

   }  /* End of if(no socket) */

   /* Make a note of the priority mask */
   iLogPrioMask = Prio;

   /* Record the print flags to global variables */
   iLogPrintCon = PrintCon;
   iLogPrintNet = PrintNet;

   /* Record the prefix to a global variable too */
   if(PrefixPtr == NULL)
   {
      return E_LOG_NO_PREFIX;
   }
   else
   {
      StringLen = strlen(PrefixPtr);
      if(StringLen >= I_LOG_MAXPREFIXLEN)
      {
         return E_LOG_PREFIX_TOO_LONG;
      }
      if(StringLen == 0)
      {
         return E_LOG_ZERO_LENGTH_PREFIX;
      }
      iLogPrefixPtr = TTL_MALLOC(StringLen+1);
      if(iLogPrefixPtr == NULL)
      {
         return E_LOG_OUTOFMEMORY;
      }
      strcpy(iLogPrefixPtr, PrefixPtr);
      iLogPrefixPtr[StringLen] = '\0';
   }

   /* Do the same for the "instance" name (if exists, otherwise set to NULL) */
   if(InstancePtr != NULL)
   {
      StringLen = strlen(InstancePtr);
      if(StringLen >= I_LOG_MAXINSTLEN)
      {
         return E_LOG_INSTANCE_TOO_LONG;
      }
      iLogInstancePtr = TTL_MALLOC(StringLen+1);
      if(iLogInstancePtr == NULL)
      {
         return E_LOG_OUTOFMEMORY;
      }
      strcpy(iLogInstancePtr, InstancePtr);
      iLogInstancePtr[StringLen] = '\0';
   }

   /* Check if the local log file is already open */
   if(iLogFilePtr != NULL)
   {
      /* If it is, close the file */
      fclose(iLogFilePtr);
   }

   /* If we have defined a filename to write to, then open it */
   if(FileNamePtr != NULL)
   {
      if(*FileNamePtr != '\0')
      {
         iLogFilePtr = fopen(FileNamePtr, "a");
         if(iLogFilePtr == NULL)
         {
            return E_LOG_FILENOTOPENED;
         }
      }
   }


   /* Return success */
   FirstTime = FALSE;
   return SYS_NOMINAL;

} /* End of eLogSetup() */


void eLogReport
(
   int Priority,
   Status_t Status,
   const char *FormatPtr,
   ...
)
{
/*
** Function Name:
**    eLogReport
**
** Type:
**    void
**
** Purpose:
**    Generic program message reporting function.
**
** Description:
**    This function will report a user provided message and Status
**    code at a specified priority. Depending on the configuration
**    of the logging for the application (controlled by eLogSetup),
**    this message may be sent to either a local disk log, a UDP 
**    accesses network log or to the terminal console. It operates
**    as if the third argument and those that follow were those of
**    the ASNI/ISO-C standard library call: printf().
**
** Arguments:
**    int Priority                    (in)
**       A priority to be associated with the message being sent.
**    Status_t Status                 (in)
**       A TTL Status code.
**    const char *FormatPtr           (in)
**       A format string which takes the same format as that in 
**       the standard library function printf(). The variable
**       arguments that follow match these, the same as the printf()
**       case. Note that there is no need to add a trailing newline 
**       (\n) character; this is done automatically by the function.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    06-Jun-2000 djm Updated the comment header block.
**    01-Jun-2000 djm Modified to use setup parameters.
**    31-May-2000 djm Added actual syslog() call.
**    03-Apr-2000 djm Adapted from McbReport.c
**    28-Mar-2000 djm Initial creation.
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */


   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Call the generic function to do the work */
   mLogProcess(Priority, Status, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogReport() */



void eLogDebug
(
   const char *FormatPtr, ...
)
{
/*
** Function Name:
**    eLogDebug
**
** Type:
**    void
**
** Purpose:
**    Report a message with DEBUG priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup();
**
** Arguments:
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/


   /* Local variables */
   va_list Args;             /* Variable length argument list */


   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Check for a NULL pointer */
   if(FormatPtr == NULL)
   {
      /* Report the fact that this error has occurred */
      mLogProcess(
         E_LOG_CRIT, E_LOG_NULL_FORMAT_PTR,
         "eLogDebug() called with a NULL format pointer "
         "(message aborted)", 0
      );

      /* Return now, in oder to prevent a segmentation fault */
      return;
   }

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_DEBUG, I_LOG_NOSTATUS, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogDebug() */


void eLogInfo
(
  const char *FormatPtr, ...
)
{
/*
** Function Name:
**    eLogInfo
**
** Type:
**    void
**
** Purpose:
**    Report a message with INFO priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup();
**
** Arguments:
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/


   /* Local variables */
   va_list Args;             /* Variable length argument list */


   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Check for a NULL pointer */
   if(FormatPtr == NULL)
   {
      /* Report the fact that this error has occurred */
      mLogProcess(
         E_LOG_CRIT, E_LOG_NULL_FORMAT_PTR,
         "eLogInfo() called with a NULL format pointer "
         "(message aborted)", 0
      );

      /* Return now, in oder to prevent a segmentation fault */
      return;
   }

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_INFO, I_LOG_NOSTATUS, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogInfo() */



void eLogNotice
(
   Status_t Status,
   const char *FormatPtr,
   ...
)
{
/*
** Function Name:
**    eLogNotice
**
** Type:
**    void
**
** Purpose:
**    Report a message with NOTICE priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup(). The Status code is
**    explicitly provided by the caller.
**
** Arguments:
**    Status_t Status        (in)
**       Status code.
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */

   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_NOTICE, Status, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogNotice() */


void eLogWarning
(
   Status_t Status,
   const char *FormatPtr,
   ...
)
{
/*
** Function Name:
**    eLogWarning
**
** Type:
**    void
**
** Purpose:
**    Report a message with WARNING priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup(). The Status code is
**    explicitly provided by the caller.
**
** Arguments:
**    Status_t Status        (in)
**       Status code.
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */

   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_WARNING, Status, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogWarning() */


void eLogErr
(
   Status_t Status,
   const char *FormatPtr,
   ...
)
{
/*
** Function Name:
**    eLogErr
**
** Type:
**    void
**
** Purpose:
**    Report a message with ERROR priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup(). The Status code is
**    explicitly provided by the caller.
**
** Arguments:
**    Status_t Status        (in)
**       Status code.
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */

   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_ERR, Status, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogErr() */


void eLogCrit
(
   Status_t Status,
   const char *FormatPtr,
   ...
)
{
/*
** Function Name:
**    eLogCrit
**
** Type:
**    void
**
** Purpose:
**    Report a message with CRIT priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup(). The Status code is
**    explicitly provided by the caller.
**
** Arguments:
**    Status_t Status        (in)
**       Status code.
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */

   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_CRIT, Status, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogCrit() */


void eLogAlert
(
   Status_t Status,
   const char *FormatPtr,
   ...
)
{
/*
** Function Name:
**    eLogAlert
**
** Type:
**    void
**
** Purpose:
**    Report a message with ALERT priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup(). The Status code is
**    explicitly provided by the caller.
**
** Arguments:
**    Status_t Status        (in)
**       Status code.
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */

   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_ALERT, Status, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogAlert() */


void eLogEmerg
(
   Status_t Status,
   const char *FormatPtr,
   ...
)
{
/*
** Function Name:
**    eLogEmerg
**
** Type:
**    void
**
** Purpose:
**    Report a message with EMERG priority.
**
** Description:
**    This function submits a message to the relevant logging 
**    locations as set-up by eLogSetup(). The Status code is
**    explicitly provided by the caller.
**
** Arguments:
**    Status_t Status        (in)
**       Status code.
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    01-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   va_list Args;             /* Variable length argument list */

   /* Initialise variable argument list */
   va_start(Args, FormatPtr);

   /* Call the generic function to do the work */
   mLogProcess(E_LOG_EMERG, Status, FormatPtr, Args);

   /* Clean up and terminate function */
   va_end(Args);
   return;


} /* End of eLogEmerg() */






void mLogProcess
(
   int Priority,
   Status_t Status,
   const char *FormatPtr,
   va_list Args
)
{
/*
** Function Name:
**    mLogProcess
**
** Type:
**    void
**
** Purpose:
**    Core of message reporting system.
**
** Description:
**    ...
**
** Arguments:
**    int Priority           (in)
**       Message priority. It ranges from E_LOG_DEBUG to E_LOG_EMERG.
**    char *FormatPtr        (in)
**       Argument list as per "printf()".
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Dec-2001 djm Added facility code to generated messages.
**    20-Nov-2000 djm Added instance name into output string.
**    02-Jun-2000 djm Converted from syslog to eCilSendRaw calls.
**    01-Jun-2000 djm Extracted core from eLogReport to form this function.
**    01-Jun-2000 djm Modified to use setup parameters.
**    31-May-2000 djm Added actual syslog() call.
**    03-Apr-2000 djm Adapted from McbReport.c to form this eLogReport.
**    28-Mar-2000 djm Initial creation (as McbReport).
**
*/

   /* Local variables */
   char LogString[I_LOG_MAXSIZE];      /* Character string to hold output */
   char *LogStringPtr;                 /* Location pointer */
   char *PostPrefixPtr;                /* Location pointer */
   time_t CurrentTime;                 /* Storage for system time */
   size_t Size;                        /* Size of remaining string */


   /* First, check if it is a relevant priority to send, bail out if not */
   if(!(E_LOG_MASK(Priority) & iLogPrioMask))
   {
      return;
   }

   /* Check to make sure the priority is valid */
   if((Priority > E_LOG_DEBUG) || (Priority < E_LOG_EMERG))
   {
      printf("LOG: Invalid message priority (%d)\n", Priority);
      return;
   }


   /* Set up a pointer to our output string */
   LogStringPtr = LogString;

   /* Do the syslog format first (note the addition of the facility) */
   time(&CurrentTime);   
   sprintf(
      LogStringPtr, "<%d>%.15s ",
      (Priority | E_LOG_FACILITY),
      ctime(&CurrentTime)+4
   );
   while(*LogStringPtr != '\0') LogStringPtr++;

   /* Writing the program prefix */
   sprintf(LogStringPtr, "%s", iLogPrefixPtr);
   PostPrefixPtr = LogStringPtr;
   while(*LogStringPtr != '\0') LogStringPtr++;

   /* Writing the program instance name */
   if(iLogInstancePtr != NULL)
   {
      sprintf(LogStringPtr, "[%s]", iLogInstancePtr);
      while(*LogStringPtr != '\0') LogStringPtr++;
   }

   /* Writing the separator */
   sprintf(LogStringPtr, ": ");
   while(*LogStringPtr != '\0') LogStringPtr++;

   /* Now write the Status code */
   if(Status != (Status_t)(NULL))
   {
      sprintf(LogStringPtr, "<%x> ", Status);
      while(*LogStringPtr != '\0') LogStringPtr++;
   }

   /* Calculate the size of the remaining string */
   Size = I_LOG_MAXSIZE - strlen(LogString);

   if ( Size > 0 ) 
   {
      /* Now actually write in the supplied message */
      vsnprintf(LogStringPtr, Size, FormatPtr, Args);
   }

   /* Check if we are sending message to the system log */
   if(iLogPrintNet == TRUE)
   {
#ifdef I_LOG_CONNECTION_BASED
      send(iLogSkt, LogString, strlen(LogString), 0);
#else
      sendto(
         iLogSkt, LogString, strlen(LogString), 0,
         (struct sockaddr *)&iLogSktAddr, (int)sizeof(iLogSktAddr)
      );
#endif
   }

   /* If required, put the (entire) output to screen */
   if(iLogPrintCon == TRUE)
   {
      printf("%s\n", PostPrefixPtr);
   }

   /* Finally, if we are keeping a local log, then write that as well */
   if(iLogFilePtr != NULL)
   {
      fprintf(iLogFilePtr, "%s\n", LogString);
   }

}  /* End of mLogProcess() */


/* EOF */
