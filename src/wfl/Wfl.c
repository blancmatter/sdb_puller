/*                       
** Module Name:
**    Wfl.c
**
** Purpose:
**    Provides wrappers around various functions
**
** Description:
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Authors:
**    Steve Foale (sxf)
**
** Version:
**    $Id: Wfl.c,v 0.20 2007/10/05 09:23:33 mjf Exp $
**
** History:
**    $Log: Wfl.c,v $
**    Revision 0.20  2007/10/05 09:23:33  mjf
**    Contents of Wfl.c relating to on-mount software moved into new file
**    WflMsg.c. New function eWflDelay added. Linux porting status :-
**    - most functions ported but some untested
**    - eWflKeyboardHit, eWflDevCtlMsg not yet ported, may not be required
**    - eWflSetClockPeriod, eWflGetClockPeriod probably not relevant
**
**    Revision 0.19  2007/03/12 14:59:53  sxf
**    Minor modifications for building without error under linux.
**
**    Revision 0.18  2007/01/22 10:07:41  sxf
**    No change.
**
**    Revision 0.17  2007/01/11 14:08:43  sxf
**    eWflMsgReceive changed to not block on _IO_DUP messages.
**
**    Revision 0.16  2005/06/20 13:47:44  mjf
**    First phase of changes to aid porting to Linux. Public header compiles
**    but library not yet ported.
**
**    Revision 0.15  2004/11/01 10:36:45  sxf
**    Added devctl() functionality for QNX6 to eWflDevCtlMsg() function.
**
**    Revision 0.14  2004/06/23 14:08:52  asp
**    Added getprio() call to send pulse functions to ensure recipient
**    priority is same as sender.
**
**    Revision 0.13  2004/06/14 07:59:35  sxf
**    Added eWflFileTruncate().
**
**    Revision 0.12  2004/05/12 08:49:48  asp
**    Modified/added eWflSetClockPeriod() & eWflGetCloclPeriod()
**    _PULSE_CODE_COIDDEATH pulse now ignored.
**
**    Revision 0.11  2004/05/07 11:07:36  mjf
**    Addition of interrupt-masking functions and find-connection.
**
**    Revision 0.10  2004/04/27 12:36:34  mkb
**    Added seperate 'Fast' functions to attach a pulse, send a
**    pulse and detach a pulse.
**
**    Revision 0.9  2004/04/20 10:22:46  sxf
**    Fix eWflMsgSendToPid so that error is returned on failure to
**    ConnectAttach().
**
**    Revision 0.8  2004/04/16 10:47:50  sxf
**    Use eWflMsg_t for send/receive prototypes.
**
**    Revision 0.7  2004/04/02 08:08:30  mkb
**    For QNX NameDetach now does nothing because the QNX 6
**    version of name_detach causes a core dump.
**
**    Revision 0.6  2004/03/26 17:03:34  mkb
**    Reverse previous NameDetach `fix`
**
**    Revision 0.5  2004/03/26 16:57:37  mkb
**    Fixed pointer deference in NameDetach.
**
**    Revision 0.4  2004/03/24 10:34:42  sxf
**    Added eWflFindChannel and eWflSendMsgToPid.
**
**    Revision 0.3  2004/03/02 14:09:18  sxf
**    Interim check-in.
**
**    Revision 0.2  2004/02/12 17:03:28  sxf
**    Check in a working version under QNX6.
**
**    Revision 0.1  2004/02/10 11:46:03  sxf
**    Initial version.
**
**
*/


/*
** Compiler include files
*/

#include <signal.h>
#include <time.h>
#include <mqueue.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

/*
** System include files
*/

#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Wfl.h"

/*
** Local include files
*/

#include "WflPrivate.h"

#ifndef I_WFL_OS_UNSUPPORTED


/******************************************************************************
** Function Name:
**    eWflSetScheduler
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOSETSCEHD  - failed to set scheduler
**
** Purpose:
**    Set the priority and policy for the current process scheduler.
**
** Description:
**
** Arguments:
**    int    Policy
**    int    Priority
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflSetScheduler( int Policy,
                           int Priority )
{
 
   Status_t Status = SYS_NOMINAL;

#if defined I_WFL_OS_QNX4  

   if ( qnx_scheduler( 0, 0, Policy, Priority, 0 ) == -1 )
   {
      Status = E_WFL_NOSETSCHED;
   }

#else

   struct sched_param SchedParam;

   /*
   ** Initialise to 0
   */
   memset( &SchedParam, 0, sizeof( SchedParam ) );

   SchedParam.sched_priority = Priority;

   if ( sched_setscheduler( 0, Policy, &SchedParam ) == -1 )
   {
      Status = E_WFL_NOSETSCHED;
   }

#endif

   return Status;
 
} /* End of eWflSetScheduler() */


/******************************************************************************
** Function Name:
**    eWflTimerCreate
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOTIMER     - unable to create timer.
**
** Purpose:
**    Create a timer.  
**
** Description:
**
** Arguments:
**    clockid_t        ClockId
**       Clock identifier.
**    struct sigevent *EventPtr
**       Event associated with timer. 
**    timer_t         *TimerIdPtr 
**       Timer identifier.
(( 
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflTimerCreate( clockid_t        ClockId,
                          struct sigevent *EventPtr,
                          timer_t         *TimerIdPtr )
{

#if defined I_WFL_OS_QNX4  

   *TimerIdPtr = timer_create( ClockId, EventPtr );

   if ( *TimerIdPtr == -1 )
   {
      return E_WFL_NOTIMER;
   }

#else

   int TimCreateRet;

   TimCreateRet = timer_create( ClockId, EventPtr, TimerIdPtr );

   if ( TimCreateRet == -1 )
   {
      return E_WFL_NOTIMER;
   }

#endif /* Posix 1003.1 code */

   return SYS_NOMINAL;
 
} /* End of eWflTimerCreate() */


/******************************************************************************
** Function Name:
**    eWflClockPeriod
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Set a clock period
**
** Description:
**
** Arguments:
**    unsigned long NanoSeconds
**       Clock period to set to
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/
Status_t eWflSetClockPeriod( unsigned long NanoSeconds )
{
   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX6 

   struct _clockperiod Period;

   Period.nsec = NanoSeconds;

   if ( -1 == ClockPeriod( CLOCK_REALTIME,
                           &Period, NULL, 0))
   {
      Status = E_WFL_NOSETPERIOD;
   }

#elif defined E_WFL_OS_QNX4

   struct timespec Period;

   Period.tv_sec = 0;
   Period.tv_nsec = NanoSeconds;

   if ( -1 == clock_setres( CLOCK_REALTIME, &Period ) )
   {
      Status = E_WFL_NOSETPERIOD;
   }

#else

   /* Unsupported for other OS */
   Status = E_WFL_NOOSSUPPORT;

#endif

   return Status;
}

/******************************************************************************
** Function Name:
**    eWflGetClockPeriod
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Get the system clock period
**
** Description:
**
** Arguments:
**    unsigned long *NanoSeconds
**       Pointer to retults variable.
**
** Authors:
**    Steve Foale (sxf)
**    Andrzej S Piascik (asp)
**
******************************************************************************/
Status_t eWflGetClockPeriod( unsigned long *NanoSeconds )
{
   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX6 

   struct _clockperiod Period;

   if ( -1 == ClockPeriod( CLOCK_REALTIME,
                           NULL, &Period, 0) )
   {
      Status = E_WFL_NOSETPERIOD;
   }
   else
   {
      *NanoSeconds = Period.nsec;
   }

#elif defined E_WFL_OS_QNX4

   struct timespec Period;

   if ( -1 == clock_getres( CLOCK_REALTIME, &Period ) )
   {
      Status = E_WFL_NOSETPERIOD;
   }
   else
   {
      *NanoSeconds = Period.tv_nsec;      
   }

#else

   /* Unsupported for other OS */
   Status = E_WFL_NOOSSUPPORT;

#endif

   return Status;
}


/******************************************************************************
** Function Name:
**    eWflReadTermData
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOTERMREAD  - unable to read terminal device
**         E_WFL_NOOSINFO    - function not supported for OS
**
** Purpose:
**    Read data from a terminal device
**
** Description:
**
** Arguments:
**    int   FileDes     - file descriptor associated with terminal device
**    void *BufPtr      - pointer to buffer for data
**    int   Max         - max number of chars to read 
**    int   Min         - min number of chars to wait for
**    int   Time        - max inter char time (1/10 of sec)
**    int   TimeOut     - how long to wait for data (1/10 of sec)
**    int  *NumCharPtr  - pointer to number of chars read
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflReadTermData( int   FileDes,
                           void *BufPtr,
                           int   Max, 
                           int   Min,
                           int   Time,
                           int   TimeOut,
                           int  *NumCharPtr ) 
{
   Status_t Status = SYS_NOMINAL;

#if defined I_WFL_OS_QNX4  

   *NumCharPtr = dev_read( FileDes,
                           BufPtr,
                           Max,
                           Min,
                           Time,
                           TimeOut, 0, 0);

   if ( *NumCharPtr == -1 )
   {
      Status = E_WFL_NOTERMREAD;
   }

#elif defined I_WFL_OS_QNX6

   *NumCharPtr = readcond( FileDes,
                           BufPtr,
                           Max,
                           Min,
                           Time,
                           TimeOut );

   if ( *NumCharPtr == -1 )
   {
      Status = E_WFL_NOTERMREAD;
   }

#elif defined I_WFL_OS_LINUX

   *NumCharPtr = read    ( FileDes,
                           BufPtr,
                           Max );

   if ( *NumCharPtr == -1 )
   {
      Status = E_WFL_NOTERMREAD;
   }

#else

   /* Unsupported for other OS */
   Status = E_WFL_NOOSSUPPORT;

#endif

   return Status;
 
} /* End of eWflReadTermData */


/******************************************************************************
** Function Name:
**    eWflKeyboardHit
**
** Type:
**    Bool_t
**       Returns the status of the function on completion:
**          TRUE  - if the keyboard has been hit
**          FALSE - if the keyboard hasn't been hit
**
** Purpose:
**    Detect if the keyboard has been hit
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Bool_t eWflKeyboardHit( void ) 
{

#if defined I_WFL_OS_QNX4  

   return kbhit();

#elif defined I_WFL_OS_QNX6 || defined I_WFL_OS_VMS

   if ( tcischars( STDIN_FILENO ) > 0 )
   {
      return TRUE;
   }
   else 
   {
      return FALSE;
   }

#else

   /*
   ** To be implemented for Linux.
   */
   return FALSE;

#endif

} /* End of eWflKeyboardHit() */


/******************************************************************************
** Function Name:
**    eWflGetKeyPress
**
** Type:
**    char
**       Returns the last keyboard character pressed:
**
** Purpose:
**    Read the next character from the keyboard without blocking waiting 
**    for a return.
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

char eWflGetKeyPress( void ) 
{

   char KeyPress;               /* Key press detected */
   struct termios Termio;       /* Terminal io settings */
   struct termios TermioCopy;   /* Copy of term io settings for restoring */

   /*
   ** Get current terminal IO settings.
   */
   tcgetattr( STDIN_FILENO, &Termio );

   /*
   ** Copy terminal IO settings
   */
   TermioCopy = Termio;

   /*
   ** Change termio mode
   */
   Termio.c_lflag = Termio.c_lflag || ICANON;

   tcsetattr( STDIN_FILENO, TCSANOW, &Termio );

   /*
   ** Read a characte from stdin
   */
   read ( STDIN_FILENO, &KeyPress, 1 );

   /*
   ** Flush the stdin input buffer 
   */
   tcflush( STDIN_FILENO, TCIFLUSH );

   /*
   ** Return terminal io settings back to original state.
   */
   tcsetattr( STDIN_FILENO, TCSANOW, &TermioCopy );

   return KeyPress;

}  /* End of eWflGetKeyPress( ) */


/******************************************************************************
** Function Name:
**    eWflDevCtlMsg
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOATTACH    - Can't create a channel
**
** Purpose:
**       Sends a device control command message to a resource driver.  
**
** Description:
**
** Arguments:
**       int   Chid  -  Channel id of the server
**       pid_t Pid   -  Pid of the server
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflDevCtlMsg(int filedes, 
                       unsigned request, 
                       void *sbuf, 
                       unsigned sbytes, 
                       void *rbuf, 
                       unsigned rbytes )
{
   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX4
   
   if( qnx_ioctl( filedes, request, sbuf, sbytes, rbuf, rbytes) == -1 )
      Status = E_WFL_DEVCTLERR;

#elif defined E_WFL_OS_QNX6

#define MAX_SIZE 16384

   static char *DataPtr = NULL;
   static int   Size;

   /*
   ** In/out buffer size is max(sbytes, rbytes)
   */ 
   if ( sbytes > rbytes ) 
   {
      Size = sbytes;
   }
   else
   {
      Size = rbytes;
   }

   /*
   ** Limit size of buffer.
   */
   if ( Size > MAX_SIZE )
   {
      Size = MAX_SIZE;
   }

   /* 
   ** Declare memory for in/out buffer.
   */
   if ( DataPtr == NULL )
   {
      DataPtr = (char *)malloc( MAX_SIZE );
   }

   /*
   ** If output buffer exists, fill in/out buffer.
   */
   if ( sbuf != NULL )
   {
      if ( sbytes < MAX_SIZE )
      {
         memcpy( DataPtr, sbuf, sbytes );
      }
      else
      {
         memcpy( DataPtr, sbuf, MAX_SIZE );
      }
   }

   /*
   ** Send device control command.
   */
   if ( devctl( filedes, request, DataPtr, MAX_SIZE, NULL ) != EOK )
       Status = E_WFL_DEVCTLERR;   
   
   /* 
   ** Copy data into read buffer.
   */
   if ( rbuf != NULL )
   {
      if ( rbytes < MAX_SIZE )
      {
         memcpy( rbuf, DataPtr, rbytes );
      }
      else
      {
         memcpy( rbuf, DataPtr, MAX_SIZE );
      }

   }

#else

   /* Unsupported for other OS */
   Status = E_WFL_NOOSSUPPORT;

#endif

   return Status;

}  /* End of eWflDevCtlMsg() */


/******************************************************************************
** Function Name:
**    eWflFileTruncate
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Truncate file to a given size
**
** Description:
**
** Arguments:
**       const char *name  - Name of the shared memory object
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/
Status_t eWflFileTruncate( int   FileDesc,
                           off_t Size )
{
   Status_t Status = SYS_NOMINAL;

   int Return;

#if defined E_WFL_OS_QNX4

   Return = ltrunc( FileDesc, Size, SEEK_SET );

#else

   Return = ftruncate( FileDesc, Size );

#endif

   if ( Return == -1 )
   {
      Status = E_WFL_FILEERR;
   }

   return Status;

} /* End of eWflFileTruncate() */


/*******************************************************************************
** Function Name:
**    eWflDelay
**
** Purpose:
**    Implements a delay, replacing the QNX function 'Delay'.
**
** Description:
**    Uses the function 'nanosleep' to implement the QNX function 'Delay'.
**
** Return Type:
**    Returns the status of the function on completion:
**    - SYS_NOMINAL - normal successful completion
**    - E_WFL_INTERRUPTED - function was interrupted
**
** Arguments:
**    int MilliSeconds                 (in)
**       The length of time to delay for, in milli-seconds.
**
** Author(s):
**    Martyn J. Ford (mjf)
**
*******************************************************************************/

Status_t eWflDelay ( int MilliSeconds )
{
   struct timespec Period;

   /* Construct the timespec */
   Period.tv_sec = 0;
   Period.tv_nsec = MilliSeconds * E_TTL_NANOSECS_PER_MILLISEC;

#if defined E_WFL_OS_QNX4 || defined E_WFL_OS_QNX6

   delay( MilliSeconds );

   return SYS_NOMINAL;

#else

   /* Have a sleep */
   if ( nanosleep( &Period, NULL ) == 0 )
   {
      /* Sleep elapsed OK */
      return SYS_NOMINAL;
   }
   else
   {
      /* Sleep was interrupted */
      return E_WFL_INTERRUPTED;
   }

#endif

} 


#endif /* Operating system unsupported */ 


/*
** EOF
*/

