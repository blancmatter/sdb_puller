/*                       
** Module Name:
**    WflMsg.c
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
**    $Id: WflMsg.c,v 0.20 2007/10/05 09:23:33 mjf Exp $
**
** History:
**    $Log: WflMsg.c,v $
**    Revision 0.20  2007/10/05 09:23:33  mjf
**    Creation of new source file from Wfl.c to contain functionality only
**    used in on-mount sub-systems.
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
#include "Wfl.h"

/*
** Local include files
*/

#include "WflPrivate.h"

#ifndef I_WFL_OS_UNSUPPORTED

/* 
** Global variables
*/

#if defined E_WFL_OS_QNX6 

static int mWflPulseCode = _PULSE_CODE_MINAVAIL;
static int mWflChannelId = -1;
static int mWflIPCChannelId = -1;



#else


#endif

#if defined E_WFL_OS_QNX6 

eWflPulseId_t  mWflSndChannel; 
eWflMsg_t     *mWflSndSendMsgPtr;
int            mWflSndSendSize;
int            mWflSndRet;

/*
** Send message with no reply expected from a detached
** thread and place return in module scope variable.
*/
void * mWflMsgSend( void *Args )
{

   mWflSndRet = MsgSend( mWflSndChannel,
                  mWflSndSendMsgPtr,
                  mWflSndSendSize,
                  NULL,
                  0 ); 
   
   return (void *)(&mWflSndRet);
}

#endif

/******************************************************************************
** Function Name:
**    eWflGetChannelId
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Returns the id of the currently open channel.
**
** Description:
**
** Arguments:
**    void
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/
Status_t eWflGetChannelId( int *pChid )
{
 
   Status_t Status = SYS_NOMINAL;

#if defined I_WFL_OS_QNX4  

   /* NO QNX4 CODE NEEDED */

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6 

   *pChid = mWflChannelId;

#endif /* QNX6 specific code */

   return Status;
 
} /* End of eWflGetChannelId() */


/******************************************************************************
** Function Name:
**    eWflCreatePulse
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOIOPRIVITY - unable to gain IO privity
**         E_WFL_NOPCIBIOS   - PCI bios not found
**         E_WFL_NOOSINFO    - unable to read OS info
**
** Purpose:
**    Create a pulse associated with an event.  
**    If EventPtr is NULL, just create a pulse.
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

Status_t eWflCreatePulse( struct sigevent *EventPtr, 
                          eWflPulseId_t   *PulsePtr )
{
 
   Status_t Status = SYS_NOMINAL;

#if defined I_WFL_OS_QNX4  
 
   *PulsePtr = qnx_proxy_attach( 0, 0, 0, -1 );

   if ( EventPtr != (struct sigevent *)(NULL) )
   {
      EventPtr -> sigev_signo = - *PulsePtr;
   }

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6 

   struct sigevent Event;

   if ( mWflChannelId == -1 )
   {
      mWflChannelId = ChannelCreate( E_WFL_THIS_NODE );
   }

   /*
   ** Fill the fields of Event relevant to the pulse.
   ** Store the client-side connection id in sigev_value.sival_int
   ** so that we can determine the source of a delivered pulse.
   */
   Event.sigev_notify   = SIGEV_PULSE;
   Event.sigev_coid     = ConnectAttach( ND_LOCAL_NODE, 
                                         0,
                                         mWflChannelId,
                                        _NTO_SIDE_CHANNEL, 0 );
   Event.sigev_priority = getprio( 0 );
   Event.sigev_code     = E_WFL_PULSECODE_EVENT;
   
   Event.sigev_value.sival_int =  Event.sigev_coid;

   if ( EventPtr != (struct sigevent *)(NULL) )
   {
      *EventPtr = Event;
   }

   *PulsePtr = Event.sigev_coid;
   mWflPulseCode ++;


#endif /* QNX6 specific code */

   if ( *PulsePtr == -1 )
   {
      Status = E_WFL_NOPULSE;
   }

  return Status;
 
} /* End of eWflCreatePulse() */

/******************************************************************************
** Function Name:
**    eWflCheckPulse
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOTPULSE    - not a pulse
**
** Purpose:
**    Check that the connection associated with the given pulse is ok
**    for communication.
**
** Description:
**
** Arguments:
**    eWflPulseId_t PulseId 
**       Pulse Identifier.
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflCheckPulse( eWflPulseId_t PulseId )
{
 
   Status_t Status;

#if defined I_WFL_OS_QNX4  
 
   if ( kill( PulseId, 0 ) == 0 )
   {
      Status = SYS_NOMINAL;
   }
   else
   { 
      Status = E_WFL_NOTPULSE;
   }

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6 

   eWflPulseId_t PulseCopy;
   
   PulseCopy = dup( PulseId );

   if (  PulseCopy == -1 )
   {
      Status = E_WFL_NOTPULSE;
   }
   else
   {
      close( PulseCopy );
      Status = SYS_NOMINAL;
   }

#endif /* QNX6 specific code */

   return Status;

 
} /* End of eWflCheckPulse() */


/******************************************************************************
** Function Name:
**    eWflCreateTimedPulse
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPULSE     - unable to create pulse
**
** Purpose:
**    Create a timer.  
**
** Description:
**
** Arguments:
**    eWflTimedPulse_t *TimedPulsePtr
**       Timed pulse structure.
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflCreateTimedPulse( eWflTimedPulse_t *TimedPulsePtr )
{

   Status_t          Status;
   struct sigevent   Event;
   
   Status = SYS_NOMINAL;

#if defined I_WFL_OS_QNX4  
 
   TimedPulsePtr -> PulseId = qnx_proxy_attach( 0, 0, 0, -1 );

   Event.sigev_signo = -TimedPulsePtr -> PulseId;

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6 

   if ( mWflChannelId == -1 )
   {
      mWflChannelId = ChannelCreate( E_WFL_THIS_NODE );
   }

   /*
   ** Fill the fields of Event relevant to the pulse.
   ** Store the client-side connection id in sigev_value.sival_int
   ** so that we can determine the source of a delivered pulse.
   */
   Event.sigev_notify   = SIGEV_PULSE;
   Event.sigev_coid     = ConnectAttach( ND_LOCAL_NODE, 
                                     0,
                                     mWflChannelId,
                                     _NTO_SIDE_CHANNEL, 0 );
   Event.sigev_priority = getprio( 0 );
   Event.sigev_code     = E_WFL_PULSECODE_TIMED;

   Event.sigev_value.sival_int =  Event.sigev_coid;

   TimedPulsePtr -> PulseId = Event.sigev_coid;
   

#endif /* QNX6 specific code */
 
   if ( TimedPulsePtr -> PulseId == -1 )
   {
      return E_WFL_NOPULSE;
   }

   Status = eWflTimerCreate( CLOCK_REALTIME, &Event, &(TimedPulsePtr-> TimerId) );

   if ( Status != SYS_NOMINAL )
   {
      return Status;
   }

#if defined E_WFL_OS_QNX6 

   mWflPulseCode ++;

#endif 

   Status = eWflUpdateTimer( TimedPulsePtr );

   return Status;

 
} /* End of eWflCreateTimedPulse() */


/******************************************************************************
** Function Name:
**    eWflPulseOnMqueueEmpty
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOPULSE     - failed to create pulse
** Purpose:
**    Associate an event with a specified queue such that a pulse is
**    delivered on the queue emptying.  
**
** Description:
**
** Arguments:
**    mqd_t            Mqd           - Message queue
**    eWflPulseId_t    PulseIdPtr    - Pulse identifier
**    struct sigevent *EventPtr      - Event associated with pulse
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflPulseOnMqueueEmpty( mqd_t            Mqd,
                                 eWflPulseId_t   *PulseIdPtr,
                                 struct sigevent *EventPtr )
{

   int Ret;    /* Return from mq_notify() */
   
#if defined I_WFL_OS_QNX4  
 
   *PulseIdPtr = qnx_proxy_attach( 0, 0, 0, -1 );

   EventPtr -> sigev_signo = -(*PulseIdPtr);

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6 

   if ( mWflChannelId == -1 )
   {
      mWflChannelId = ChannelCreate( E_WFL_THIS_NODE );
   }

   /*
   ** Fill the fields of Event relevant to the pulse.
   ** Store the client-side connection id in sigev_value.sival_int
   ** so that we can determine the source of a delivered pulse.
   */
   EventPtr -> sigev_notify   = SIGEV_PULSE;
   EventPtr -> sigev_coid     = ConnectAttach( ND_LOCAL_NODE, 
                                     0,
                                     mWflChannelId,
                                     _NTO_SIDE_CHANNEL, 0 );
   EventPtr -> sigev_priority = getprio( 0 );
   EventPtr -> sigev_code     = E_WFL_PULSECODE_MQUEUE;
   
   EventPtr -> sigev_value.sival_int =  EventPtr -> sigev_coid;

#endif /* QNX6 specific code */

   Ret = mq_notify( Mqd, EventPtr );

   if ( Ret == -1 )
   {
      return E_WFL_NOPULSE;
   }

#if defined E_WFL_OS_QNX6 

   *PulseIdPtr = EventPtr -> sigev_coid;
   mWflPulseCode ++;

#endif 

   if ( *PulseIdPtr == -1 )
   {
      return E_WFL_NOPULSE;
   }
   else
   {
      return SYS_NOMINAL;
   }

 
} /* End of eWflPulseOnMqueueEmpty() */

/******************************************************************************
** Function Name:
**    eWflDetachPulse
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NODETACH    - unable to detach pulse successfully
**
** Purpose:
**    Detach a pulse connection.  
**
** Description:
**
** Arguments:
**    eWflPulseId_t Pulse
**       Pulse to detach.
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflDetachPulse( eWflPulseId_t Pulse )
{

   int Ret; 
   
#if defined I_WFL_OS_QNX4  
 
   Ret = qnx_proxy_detach( Pulse );

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6 

   Ret = ConnectDetach( Pulse );

#endif /* QNX6 specific code */

   if ( Ret == -1 )
   {  
      return E_WFL_NODETACH;
   }
   else
   { 
      return SYS_NOMINAL;
   }

} /* End of eWflDetachPulse() */

 
/******************************************************************************
** Function Name:
**    eWflUpdateTimer
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL         - normal successful completion
**         E_WFL_NOTIMERUPDATE - to update timer
**
** Purpose:
**    Update a timer.  
**
** Description:
**
** Arguments:
**    eWflTimedPulse_t *TimedPulsePtr
**       Timed pulse structure.
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflUpdateTimer( eWflTimedPulse_t *TimedPulsePtr ) 
{

   struct itimerspec Itime;

   memset( &Itime, 0, sizeof( Itime ) );

   /*
   ** Populate itimerspec structure. 
   ** NB Qnx6 requires nanosecs field to be less than 1 second.
   */
   Itime.it_value.tv_nsec    = ( TimedPulsePtr -> ValNSecs ) % 1000000000;
   Itime.it_value.tv_sec     =   TimedPulsePtr -> ValSecs + 
                               ( TimedPulsePtr -> ValNSecs ) / 1000000000;

   Itime.it_interval.tv_nsec = ( TimedPulsePtr -> IntNSecs ) % 1000000000;
   Itime.it_interval.tv_sec  =   TimedPulsePtr -> IntSecs + 
                               ( TimedPulsePtr -> IntNSecs ) / 1000000000;

   if ( timer_settime( TimedPulsePtr -> TimerId,
                       0, 
                      &Itime,
                       NULL ) == -1 )
   {
      return E_WFL_NOTIMERUPDATE;
   }

   return SYS_NOMINAL;
 
} /* End of eWflUpdateTimer() */


/******************************************************************************
** Function Name:
**    eWflReceivePulse
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOTPULSE    - message received was not a pulse
**         E_WFL_PULSERCVERR - error in pulse receive
**
** Purpose:
**    Block waiting on a pulse
**
** Description:
**
** Arguments:
**    eWflPulseId_t *PulseIdPtr
**       Pulse identifier
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflReceivePulse( eWflPulseId_t *PulseIdPtr ) 
{

   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX6 
   int MsgReturn;
   
   struct _msg_info Info;

   eWflMsg_t Pulse;

   if ( mWflChannelId == -1 ) 
   {
      return E_WFL_NOPULSE;
   }


   MsgReturn = MsgReceive( mWflChannelId, &Pulse, sizeof( Pulse ), NULL );
   if( MsgReturn == -1 )
   {
      Status = E_WFL_PULSERCVERR;
   }
   else if ( MsgReturn == 0 )
   {
      *PulseIdPtr = Pulse.Hdr.value.sival_int;
   }
   else 
   {
      Status = E_WFL_NOTPULSE;
   }

#elif defined E_WFL_OS_QNX4
   
   Int32_t Msg;

   *PulseIdPtr = Receive( 0, &Msg, sizeof( Msg ) );

   if ( *PulseIdPtr == -1 )
   {
      Status = E_WFL_PULSERCVERR;
   }

#endif

   return Status;
 
} /* End of eWflReceivePulse() */


/******************************************************************************
** Function Name:
**    eWflNameAttach
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOATTACH    - unable to attach name
**
** Purpose:
**    Register a name in the namespace and create a channel 
**    for interprocess communications
**
** Description:
**
** Arguments:
**    const char       * PathPtr    - Name to register
**    eWflNameAttach_t * AttachPtr  - Attach point
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflNameAttach( const char       * PathPtr,
                         eWflNameAttach_t * AttachPtr )
{

   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX4

   int Ret;

   Ret = qnx_name_attach( 0, PathPtr );

   if ( Ret == -1 )
   {
      Status = E_WFL_NOATTACH;
   }

#endif

#if defined E_WFL_OS_QNX6 

   eWflNameAttach_t *TmpAttachPtr;

   TmpAttachPtr = name_attach( NULL, PathPtr, NULL );

   *AttachPtr = *TmpAttachPtr;

   if ( AttachPtr -> chid == 0 )
   {
      Status = E_WFL_NOATTACH;
   }
   
   mWflChannelId = AttachPtr -> chid;

#endif

   return Status;

} /* End of eWflNameAttach() */

/******************************************************************************
** Function Name:
**    eWflNameDetach
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NODETACH    - unable to detach name
**
** Purpose:
**    Remove a name from the namespace and destroy the
**    communications channel.
**    NameDetach not currently supported in Wfl under QNX6.
**
** Description:
**
** Arguments:
**    eWflNameAttach_t *AttachPtr - Attach point
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflNameDetach( eWflNameAttach_t *AttachPtr ) 
{

   Status_t Status = SYS_NOMINAL;
   int      Ret    = 0;

#if defined E_WFL_OS_QNX4

   Ret = qnx_name_detach( NULL, *AttachPtr );

#endif

#if defined E_WFL_OS_QNX6

/*
** This name detach causes a core dump. Name detach happens automatically when
** a process performs an exit.
**
Ret = name_detach( AttachPtr, NULL );
**
*/
   
#endif

   if ( Ret != 0 )
   { 
      Status = E_WFL_NODETACH;
   }

   return Status;

} /* End of eWflNameDetach() */

/******************************************************************************
** Function Name:
**    eWflNameOpen
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NONAMEOPEN  - unable to open name
**
** Purpose:
**    Open a name for a server connection
**
** Description:
**
** Arguments:
**    const char    * NamePtr    - Name to open
**    eWflPulseId_t * PulseIdPtr - Connection id for pulse
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflNameOpen( const char    * NamePtr,
                       eWflPulseId_t * PulseIdPtr ) 
{

   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX4

   *PulseIdPtr = qnx_name_locate( NULL, NamePtr, NULL, NULL );

#endif

#if defined E_WFL_OS_QNX6 

   *PulseIdPtr = name_open( NamePtr, NULL );
   
#endif

   if ( *PulseIdPtr == -1 )
   { 
      Status = E_WFL_NONAMEOPEN;
   }

   return Status;

} /* End of eWflNameOpen() */

/******************************************************************************
** Function Name:
**    eWflNameClose
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NONAMECLOSE - unable to close name for server connect
**
** Purpose:
**    Close the connection obtained with the eWflNameOpen() call
**
** Description:
**
** Arguments:
**    eWflPulseId_t PulseId - Connection id for pulse
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflNameClose( eWflPulseId_t PulseId )
{

   Status_t Status = SYS_NOMINAL;
   int      Ret;

#if defined E_WFL_OS_QNX4

   Ret = 0;

#endif

#if defined E_WFL_OS_QNX6

   Ret = name_close( PulseId );
   
#endif

   if ( Ret == -1 )
   { 
      Status = E_WFL_NONAMECLOSE;
   }

   return Status;

} /* End of eWflNameClose() */

/******************************************************************************
** Function Name:
**    eWflMsgReceive
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_MSGRCVERR   - error in message receive
**
** Purpose:
**    Receive a QNX message or pulse.
**
** Description:
**
** Arguments:
**    eWflMsg_t        *MsgPtr     - Returned message
**    int               Size       - size of returned message
**    eWflPulseId_t    *RecvIdPtr  - Pulse connection id of sender
**    eWflPulseId_t    *ReplIdPtr  - Pulse connection id for reply
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflMsgReceive( eWflMsg_t        *MsgPtr,
                         int               Size,
                         eWflPulseId_t    *RecvIdPtr,
                         eWflPulseId_t    *ReplIdPtr )
{

   Status_t Status = SYS_NOMINAL;
   eWflMsg_t        *PulseRcvPtr;


#if defined E_WFL_OS_QNX6 

   struct _msg_info Info;

#endif

   PulseRcvPtr = (eWflMsg_t *)(MsgPtr);

#if defined E_WFL_OS_QNX4

   *RecvIdPtr = Receive( NULL, MsgPtr, Size );
   
   PulseRcvPtr -> PulseId = *RecvIdPtr;
   *ReplIdPtr = *RecvIdPtr;

#endif

#if defined E_WFL_OS_QNX6 
   /* Check to see if we've got a channel ID */
   if( mWflChannelId == -1 )
      return E_WFL_NOATTACH;

   *RecvIdPtr = MsgReceive( mWflChannelId, 
                              MsgPtr, 
                              Size, 
                              NULL );

   /*
   ** Handle any pulses received
   */
   if ( *RecvIdPtr == 0) 
   {

      switch ( PulseRcvPtr -> Hdr.code)
      {
         case _PULSE_CODE_DISCONNECT:
            /*
            ** A client disconnected all its connections (called
            ** name_close() for each name_open() of our name) or
            ** terminated
            */

            ConnectDetach( PulseRcvPtr -> Hdr.scoid );
            break;

         case _PULSE_CODE_UNBLOCK:
            /*
            ** REPLY blocked client wants to unblock (was hit by
            ** a signal or timed out).  It's up to you if you
            ** reply now or later.
            */
            break;

         case _PULSE_CODE_COIDDEATH:
            /*
            ** REPLY blocked client wants to unblock (was hit by
            ** a signal or timed out).  It's up to you if you
            ** reply now or later.
            */
            break;
    
         default:
            /*
            ** A pulse sent by one of your processes or a
            ** _PULSE_CODE_COIDDEATH or _PULSE_CODE_THREADDEATH
            ** from the kernel?
            */
            *ReplIdPtr = PulseRcvPtr -> Hdr.scoid;
            PulseRcvPtr -> PulseId = PulseRcvPtr -> Hdr.value.sival_int;
            break;

      } /* End of switch */
   }

   /*
   ** If a pulse was received, get the server connection id 
   ** passed as the pulse payload.
   */
   if ( *RecvIdPtr == 0 )
   {
      *RecvIdPtr = PulseRcvPtr -> Hdr.value.sival_int;
   }
   else if ( ( PulseRcvPtr -> Hdr.type >= _IO_BASE ) &&
             ( PulseRcvPtr -> Hdr.type <= _IO_MAX ) )
   {
      MsgError( *RecvIdPtr, ENOSYS);
      *RecvIdPtr = -1;
   }
   else
   {
      MsgInfo( *RecvIdPtr, &Info );
         
      *ReplIdPtr = *RecvIdPtr;
      *RecvIdPtr = Info.coid;
   }

   if ( *RecvIdPtr == -1 )
   { 
      Status = E_WFL_MSGRCVERR;
   }
#endif

   return Status;

} /* End of eWflMsgReceive() */

/******************************************************************************
** Function Name:
**    eWflMsgSendToPid
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Block waiting on a pulse
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

Status_t eWflMsgSendToPid( pid_t          Pid, 
                           int            Chid,
                           const eWflMsg_t *SendMsgPtr,
                           eWflMsg_t     *ReplyMsgPtr,
                           int            SendSize,
                           int            ReplySize )
{

   Status_t Status = SYS_NOMINAL;
   int      Ret;

#if defined E_WFL_OS_QNX4

   Ret = Send( Pid,
               SendMsgPtr,
               ReplyMsgPtr,
               SendSize,
               ReplySize );

#endif

#if defined E_WFL_OS_QNX6 
   int Coid;

   Coid = ConnectAttach( 0, Pid, Chid, _NTO_SIDE_CHANNEL, 0 );

   if ( Coid != -1 )
   {
      Ret = MsgSend( Coid,
                     SendMsgPtr,
                     SendSize,
                     ReplyMsgPtr,
                     ReplySize );
 
      if ( Ret != -1 )
      {
         Ret = ConnectDetach( Coid );
      }
   }
   else
   {
      Ret = -1;
   }
   
#endif

   if ( Ret == -1 )
   { 
      Status = E_WFL_MSGSNDERR;
   }

   return Status;

} /* End of eWflMsgSendToPid() */

/******************************************************************************
** Function Name:
**    eWflMsgSend
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Block waiting on a pulse
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

Status_t eWflMsgSend( eWflPulseId_t  Coid, 
                      const eWflMsg_t *SendMsgPtr,
                      int            SendSize,
                      eWflMsg_t     *ReplyMsgPtr,
                      int            ReplySize )
{

   Status_t Status = SYS_NOMINAL;
   int      Ret;

#if defined E_WFL_OS_QNX4

   Ret = Send( Coid,
               SendMsgPtr,
               ReplyMsgPtr,
               SendSize,
               ReplySize );

#endif

#if defined E_WFL_OS_QNX6 

   Ret = MsgSend( Coid,
                  SendMsgPtr,
                  SendSize,
                  ReplyMsgPtr,
                  ReplySize ); 
   
#endif

   if ( Ret == -1 )
   { 

      Status = E_WFL_MSGSNDERR;
   }

   return Status;

} /* End of eWflMsgSend() */


/******************************************************************************
** Function Name:
**    eWflSndMsgToPid
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_MSGSNDERR   - error sending message
**         E_WFL_NODETACH    - unable to detach pulse after sending
**
** Purpose:
**    Send a message to a given pid without blocking
**
** Description:
**
** Arguments:
**    pid_t          Pid         - Process id to send message to
**    const void    *SendMsgPtr  - Pointer to message
**    int            SendSize    - Size of message
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflSndMsgToPid ( pid_t          Pid, 
                           eWflMsg_t     *SendMsgPtr,
                           int            SendSize )
{

   Status_t Status = SYS_NOMINAL;
   int      Ret;

#if defined E_WFL_OS_QNX4
   
   pid_t Proxy;
   eWflMsg_t *MsgPtr;

   MsgPtr = (eWflMsg_t *)(SendMsgPtr);

   Proxy = qnx_proxy_attach( Pid, (char *)(SendMsgPtr), SendSize, -1 );

   if ( Proxy != -1 )
   {
      Ret = Trigger( Proxy );
      if ( Ret == -1 )
      {
         Status = E_WFL_MSGSNDERR;
      }
   }
   else
   { 
      Status = E_WFL_NOATTACH;
   }

   if ( Status == SYS_NOMINAL )
   {
      if ( qnx_proxy_detach( Proxy ) == -1 )
      {
            Status = E_WFL_NODETACH;
      }
   }
   else
   {
      Status = SYS_NOMINAL;
   }

#endif

#if defined E_WFL_OS_QNX6 

   pthread_attr_t Attr;
   int            Coid;
   eWflMsg_t     *MsgPtr;


   if ( mWflChannelId == -1 )
   {
      mWflChannelId = ChannelCreate( 0 );      
   }




   Coid = ConnectAttach( 0, Pid, mWflChannelId, _NTO_SIDE_CHANNEL, 0 );


   MsgPtr = (eWflMsg_t *)(SendMsgPtr);
   MsgPtr -> PulseId = Coid;
   MsgPtr -> Hdr.value.sival_int = Coid;
   MsgPtr -> Hdr.code = E_WFL_PULSECODE_INTERPROCESS;

   mWflSndChannel     = Coid;
   mWflSndSendMsgPtr  = (eWflMsg_t *)(SendMsgPtr);
   mWflSndSendSize    = SendSize;

   pthread_attr_init( &Attr );
   pthread_attr_setdetachstate(
      &Attr, PTHREAD_CREATE_DETACHED );
   pthread_create( NULL, &Attr, mWflMsgSend, NULL );

   Ret = ConnectDetach( Coid );

   if ( mWflSndRet == -1 )
   {
      Ret = mWflSndRet;
   }

   /* 
   ** Check for error
   */
   if ( (Ret == -1)  )
   { 
      Status = E_WFL_MSGSNDERR;
   }

#endif

   return Status;

} /* End of eWflMsgSendNoBlock() */

/******************************************************************************
** Function Name:
**    eWflMsgReply
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_MSGREPLYERR - error replying to message
**
** Purpose:
**    Reply to a message
**
** Description:
**
** Arguments:
**    eWflPulseId_t RcvId        - Connection to reply on 
**    const void   *ReplyMsgPtr  - Reply message
**    int           ReplySize    - Reply message size
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/

Status_t eWflMsgReply( eWflPulseId_t RcvId, 
                       const eWflMsg_t *ReplyMsgPtr,
                       int           ReplySize )
{

   Status_t Status = SYS_NOMINAL;
   int      Ret;

#if defined E_WFL_OS_QNX4

   Ret = Reply( RcvId, ReplyMsgPtr, ReplySize );

#endif

#if defined E_WFL_OS_QNX6

   Ret = MsgReply( RcvId, NULL, ReplyMsgPtr, ReplySize );
  
#endif

   if ( Ret == -1 )
   { 

      Status = E_WFL_MSGREPLYERR;
   }

   return Status;

} /* End of eWflMsgReply() */

/******************************************************************************
** Function Name:
**    eWflHintAttach
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOIRQATTACH - unable to attach interrupt handler
**
** Purpose:
**       Attach an interrupt line to a handler function.
**
** Description:
**
** Arguments:
**       int   Intr    -  Interrupt line to attach handler function to.
**       void *handler -  Pointer to handler function.
**       int  *IdPtr   -  Interrupt id number
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflHintAttach( int  Intr,
                         void *handler, 
                         int  *IdPtr) 
{
   Status_t Status = SYS_NOMINAL;

#if defined I_WFL_OS_QNX4  

   *IdPtr = qnx_hint_attach( Intr, handler, FP_SEG(IdPtr) );
   if( *IdPtr == -1 )
      Status = E_WFL_NOIRQATTACH;

#endif /* End of QNX4 code */
         
#if defined I_WFL_OS_QNX6
   *IdPtr = InterruptAttach( _NTO_INTR_CLASS_EXTERNAL | Intr, handler, NULL, 0, 
            _NTO_INTR_FLAGS_PROCESS );
   if( *IdPtr == -1 )
      Status = E_WFL_NOIRQATTACH; 

#endif /* End of QNX6 code */

   return Status; 

}  /* End of eWflHintAttach() */
   
/******************************************************************************
** Function Name:
**    eWflDisableInterrupt
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Disable interrupts (for use in an interrupt service routine).
**
** Description:
**
** Arguments:
**       void
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/
Status_t eWflDisableInterrupt( void ) 
{

#ifdef E_WFL_OS_QNX4

    _disable();

#endif

#if defined E_WFL_OS_QNX6 

   InterruptDisable( );

#endif

   return SYS_NOMINAL;

}

/******************************************************************************
** Function Name:
**    eWflEnableInterrupt
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Enable interrupts (for use in an interrupt service routine).
**
** Description:
**
** Arguments:
**       void
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/
Status_t eWflEnableInterrupt( void ) 
{

#ifdef E_WFL_OS_QNX4

    _enable();

#endif

#if defined E_WFL_OS_QNX6 

   InterruptEnable( );

#endif

   return SYS_NOMINAL;
}

/******************************************************************************
** Function Name:
**    eWflUnmaskInterrupt
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Unmask an interrupt (for use in an interrupt service routine).
**
** Description:
**
** Arguments:
**       int Intr - interrupt to unmask.
**       int Id   - interrupt ID (returned by InterruptAttach)
**
** Authors:
**    Mark Bowman (mkb)
**
******************************************************************************/
Status_t eWflUnmaskInterrupt( int Intr, int Id ) 
{
   int Status;

#ifdef E_WFL_OS_QNX4
    qnx_hint_mask( Intr, 1 )
#endif

#if defined E_WFL_OS_QNX6 
    InterruptUnmask( Intr, Id );
#endif

   return SYS_NOMINAL;

}

/******************************************************************************
** Function Name:
**    eWflMaskInterrupt
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Mask an interrupt (for use in an interrupt service routine).
**
** Description:
**
** Arguments:
**       int Intr - interrupt to mask.
**       int Id   - interrupt ID (returned by InterruptAttach)
**
** Authors:
**    Mark Bowman (mkb)
**
******************************************************************************/
Status_t eWflMaskInterrupt( int Intr, int Id ) 
{
   int Status;

#ifdef E_WFL_OS_QNX4
    qnx_hint_mask( Intr, 2 )
#endif

#if defined E_WFL_OS_QNX6 
    InterruptMask( Intr, Id );
#endif

   return SYS_NOMINAL;

}


/******************************************************************************
** Function Name:
**    eWflHintDetach
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOIRQDETACH - unable to detach interrupt handler
**
** Purpose:
**       Detach an interrupt handler by ID
**
** Description:
**
** Arguments:
**       int  Id   -  Interrupt id number
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/
Status_t eWflHintDetach( int Id ) 
{
   Status_t Status = SYS_NOMINAL;
   int      Ret;

#if defined I_WFL_OS_QNX4  

   Ret = qnx_hint_detach( Id );
   if( Ret == -1 )
      Status = E_WFL_NOIRQDETACH;

#endif /* End of QNX4 code */
         
#if defined I_WFL_OS_QNX6

   Ret = InterruptDetach( Id );
   if( Ret == -1 )
      Status = E_WFL_NOIRQDETACH; 

#endif /* End of QNX6 code */

   return Status; 

}  /* End of eWflHintDetach() */

/******************************************************************************
** Function Name:
**    eWflSetProxPulse
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Obtain a proxy or pulse to trigger when an interrupt is received.
**
** Description:
**
** Arguments:
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflSetProxPulse( eWflProxPulsId_t *ProxPulsPtr, 
                           struct sigevent  *PulseEventPtr,
                           eWflPulseId_t    *ProxyPidPtr )
{
   Status_t Status = SYS_NOMINAL;

#if defined I_WFL_OS_QNX4  

   *ProxPulsPtr = *ProxyPidPtr;

#endif /* End of QNX4 code */

#if defined I_WFL_OS_QNX6

   *ProxPulsPtr = PulseEventPtr;

#endif /* End of QNX6 code */

   return Status; 

}  /* End of eWflHintAttach() */
/******************************************************************************
** Function Name:
**    eWflCreateChannel
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOATTACH    - Can't create a channel
**
** Purpose:
**       Opens a channel for QNX6 IPC  
**
** Description:
**
** Arguments:
**       void
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflCreateChannel( void )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   #endif

#if defined E_WFL_OS_QNX6 

   if( mWflChannelId == -1 )
   {
      mWflChannelId = ChannelCreate( E_WFL_THIS_NODE );
   }

   if( mWflChannelId == -1 )
      return E_WFL_NOATTACH;

#endif
    
   return Status; 

}  /* End of eWflCreateChannel */

/******************************************************************************
** Function Name:
**    eWflSendPulse
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOATTACH    - Can't create a channel
**
** Purpose:
**       Send a pulse to a server  
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
Status_t  eWflSendPulse( pid_t Pid, int Channel )
{
   Status_t Status = SYS_NOMINAL;


   #if defined E_WFL_OS_QNX4
   pid_t ProxyPid;

   /* Create a proxy to server */
   ProxyPid = qnx_proxy_attach(Pid,NULL,0,-1);
   if( ProxyPid == -1 )
      return E_WFL_NOATTACH;
 
   /* Send a message to the server */
   if( Trigger( ProxyPid ) == -1 )
      return E_WFL_PULSESNDERR;
 
   /* Release server proxy */
   if( qnx_proxy_detach( ProxyPid ) == -1 )
      return E_WFL_NODETACH;
 
   #endif 

#if defined E_WFL_OS_QNX6 
   int Coid;

   Coid = ConnectAttach( 0, Pid, Channel, _NTO_SIDE_CHANNEL, 0 );

   if( Coid == -1 )
   {
      return E_WFL_NOATTACH;
   }

   if( MsgSendPulse(Coid, getprio(0), E_WFL_PULSECODE_INTERPROCESS, Coid ) == -1 )
   {
      return E_WFL_PULSESNDERR;
   }

   /* Make sure we have detached connection */
   if( ConnectDetach( Coid ) == -1 )
      return E_WFL_NODETACH;

#endif

   return Status;
}

/******************************************************************************
** Function Name:
**    eWflAttchPulseFast
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOATTACH    - Can't create a channel
**
** Purpose:
**       Attach a pulse and return an identifier to allow fast pulsing.  
**
** Description:
**
** Arguments:
**       int  *Coid     -  Pulse Id
**       int   Channel  -  Channel id of the server
**       pid_t Pid      -  Pid of the server
**
** Authors:
**    Mark Bowman (mkb)
**
******************************************************************************/
Status_t  eWflAttachPulseFast( pid_t Pid, int Channel, int *CoidPtr )
{
   Status_t Status = SYS_NOMINAL;


   #if defined E_WFL_OS_QNX4
   pid_t ProxyPid;
   int *CoidPtr;

   /* Create a proxy to server */
   ProxyPid = qnx_proxy_attach(Pid,NULL,0,-1);
   if( ProxyPid == -1 )
      return E_WFL_NOATTACH;
 
   *CoidPtr = (int) ProxyPid;

   #endif 

#if defined E_WFL_OS_QNX6 

   *CoidPtr = ConnectAttach( 0, Pid, Channel, _NTO_SIDE_CHANNEL, 0 );

   if( *CoidPtr == -1 )
   {
      return E_WFL_NOATTACH;
   }

#endif

   return Status;
}

/******************************************************************************
** Function Name:
**    eWflSendPulseFast
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_PULSESNDERR - Can't send pulse
**
** Purpose:
**       Send a pulse to a server without attach and dettach.  
**
** Description:
**
** Arguments:
**       int   Coid  -  Pulse id  returned by 
**
** Authors:
**    Mark Bowman (mkb)
**
******************************************************************************/
Status_t  eWflSendPulseFast( int Coid )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   /* Send a message to the server */
   if( Trigger( (pid_t) Coid ) == -1 )
      return E_WFL_PULSESNDERR;
 
   #endif 

#if defined E_WFL_OS_QNX6 

   if( MsgSendPulse(Coid, getprio(0), E_WFL_PULSECODE_INTERPROCESS, Coid ) == -1 )
   {
      return E_WFL_PULSESNDERR;
   }

#endif

   return Status;
}

/******************************************************************************
** Function Name:
**    eWflDetachPulseFast
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NODETACH    - Can't create a channel
**
** Purpose:
**       Dettach a pulse previously setup using eWflAttachPulseFast  
**
** Description:
**
** Arguments:
**       int   Coid  -  Id of pulse
**
** Authors:
**    Mark Bowman (mkb)
**
******************************************************************************/
Status_t  eWflDetachPulseFast( int Coid )
{
   Status_t Status = SYS_NOMINAL;
 
   #if defined E_WFL_OS_QNX4

   /* Release server proxy */
   if( qnx_proxy_detach( (pid_t) Coid ) == -1 )
      return E_WFL_NODETACH;
 
   #endif 

#if defined E_WFL_OS_QNX6 

   /* Make sure we have detached connection */
   if( ConnectDetach( Coid ) == -1 )
      return E_WFL_NODETACH;

#endif

   return Status;
}

/******************************************************************************
** Function Name:
**    eWflFindConnection
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOATTACH    - Can't create a channel
**
** Purpose:
**       Find an open channel in Pid.
**
** Description:
**
** Arguments:
**       pid_t Pid           - Pid of the server
**       int  *Chid          - Channel id of the server
**       eWflPulseId_t *Coid - Connection id of the server
**
** Authors:
**    Steve Foale (sxf)
**
******************************************************************************/
Status_t  eWflFindConnection( pid_t Pid, int *Chid, eWflPulseId_t *Coid )
{
   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX4

   *Chid = 0;
   *Coid = Pid;
   return Status; 

#endif 

#if defined E_WFL_OS_QNX6 
   
   int Channel = 0;

   do
   {
      Channel ++;
      *Coid = ConnectAttach( 0, Pid, Channel, _NTO_SIDE_CHANNEL, 0 );
   } while (( *Coid == -1 ) && ( Channel <= E_WFL_MAX_CHANNELS ));

   if ( *Coid == -1 ) 
   {
      *Chid = -1;
      Status = E_WFL_NOCHANNEL;
   }
   else
   {
     *Chid = Channel;
   }

   return Status;

#endif

} /* End of eWflFindConnection() */

/******************************************************************************
** Function Name:
**    eWflFindChannel
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NOATTACH    - Can't create a channel
**
** Purpose:
**       Find an open channel in Pid.
**
** Description:
**
** Arguments:
**       pid_t Pid   -  Pid of the server
**       int  *Chid  -  Channel id of the server
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t  eWflFindChannel( pid_t Pid, int *Chid )
{
   Status_t Status = SYS_NOMINAL;

#if defined E_WFL_OS_QNX4

   *Chid = 0;
   return Status; 

#endif 

#if defined E_WFL_OS_QNX6 

   int Coid;
   int Channel = 0;

   do
   {
      Channel ++;
      Coid = ConnectAttach( 0, Pid, Channel, _NTO_SIDE_CHANNEL, 0 );
   } while (( Coid == -1 ) && ( Channel <= E_WFL_MAX_CHANNELS ));

   if ( Coid == -1 ) 
   {
      *Chid = -1;
      Status = E_WFL_NOCHANNEL;
   }
   else
   {
     *Chid = Channel;
     ConnectDetach( Coid );
   }

   return Status;

#endif

} /* End of eWflFindChannel() */


/******************************************************************************
** Function Name:
**    eWflWriteMsg
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Write a message to another process.  
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
Status_t eWflWriteMsg( int       rcvid,
                       unsigned  offset,
                       void     *msg,
                       unsigned  nbytes )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4
   
   if( Writemsg( (pid_t) rcvid, offset, msg, nbytes ) == -1 )
      Status = E_WFL_MSGWRITERR;

   #endif 

#if defined E_WFL_OS_QNX6 

   if( MsgWrite( rcvid, msg, nbytes, offset ) == -1 )
      Status = E_WFL_MSGWRITERR;

#endif

   return Status;

}  /* End of eWflWriteMsg() */

/******************************************************************************
** Function Name:
**    eWflCMsgReceive
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Write a message to another process.  
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
Status_t eWflCMsgReceive( pid_t *pid,
                          void *msg,
                          unsigned  nbytes )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   *pid = Creceive( 0, msg, nbytes );

   if( *pid == -1 )
      Status = E_WFL_MSGRCVERR;

   #endif 

#if defined E_WFL_OS_QNX6 
   int rcvid;
   struct sigevent event;

   if( mWflChannelId == -1 )
      mWflChannelId = ChannelCreate( E_WFL_THIS_NODE );

   /* Set a signal up to unblock the receive immediately */
   event.sigev_notify = SIGEV_UNBLOCK;

   TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, &event, NULL, NULL );

   rcvid = MsgReceive( mWflChannelId, msg, nbytes, NULL );

   if (rcvid == -1 )
      Status = E_WFL_MSGRCVERR;


#endif

   return Status;

}  /* End of eWflCMsgReceive() */

/******************************************************************************
** Function Name:
**    eWflShmOpen
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Create a shared memory object
**
** Description:
**
** Arguments:
**       const char *name  - Name of the shared memory object
**       int         oflag - Flag to control read write access 
**       mode_t      mode  - Permission bits for memory object
**       int        *fd    - Memory file descriptor
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflShmOpen( const char *name,
                      int         oflag,
                      mode_t      mode,
                      int        *fd )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   *fd = shm_open( name, oflag, mode );
   if( *fd == -1 )
      return E_WFL_SHMERR;

   #endif 

#if defined E_WFL_OS_QNX6 
   char NameBuffer[E_WFL_MAX_BUFF];

   /* Check that we're not going to overfill the buffer */
   if( strlen(name) >= (E_WFL_MAX_BUFF -1 ))
      return E_WFL_SHMERR;

   /* Insert a forward slash (needed by QNX6) */
   strcpy(NameBuffer,"/");
   strcat(NameBuffer,name);

   *fd = shm_open( NameBuffer, oflag, mode );
   if( *fd == -1 )
      return E_WFL_SHMERR;

#endif

   return Status;

}  /* End of eWflShmOpen() */

/******************************************************************************
** Function Name:
**    eWflShmClose
**
** Type:
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Close a shared memory object
**
** Description:
**
** Arguments:
**       const char *name  - Name of the shared memory object
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflShmClose( const char *name )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   shm_unlink( name );

   #endif 

#if defined E_WFL_OS_QNX6 
   char NameBuffer[E_WFL_MAX_BUFF];

   /* Check that we're not going to overfill the buffer */
   if( strlen(name) >= (E_WFL_MAX_BUFF -1 ))
      return E_WFL_SHMERR;

   /* Insert a forward slash (needed by QNX6) */
   strcpy(NameBuffer,"/");
   strcat(NameBuffer,name);

   shm_unlink( NameBuffer );

#endif

   return Status;

}  /* End of eWflShmClose() */


#endif /* Operating system unsupported */ 


/*
** EOF
*/
