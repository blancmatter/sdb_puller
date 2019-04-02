/*
** Module Name:
**    WflRes.c
**
** Purpose:
**    Provides wrappers around functions used in autoguider resource
**    manager.
**
** Description:
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Authors:
**    Martin Norbury (man)
**
** Version:
**    $Id: WflRes.c,v 0.6 2005/06/20 13:48:45 mjf Exp $
**
** History:
**    $Log: WflRes.c,v $
**    Revision 0.6  2005/06/20 13:48:45  mjf
**    First phase of changes to aid porting to Linux. Public header compiles
**    but library not yet ported.
**
**    Revision 0.5  2004/02/20 15:18:07  man
**    Update for checking QNX4 code.
**
**    Revision 0.4  2004/02/17 09:44:05  man
**    Testing on QNX4
**
**    Revision 0.3  2004/02/16 16:40:03  man
**    Updated several functions for use in autoguider
**    resource manager
**
**    Revision 0.2  2004/02/12 16:10:51  man
**    Updated resource manager routines.
**
**    Revision 0.1  2004/02/10 11:46:03  sxf
**    Initial version.
**
*/


/*
** Compiler include files
*/

/*
** System include files
*/

#include <stdio.h>
#include "TtlSystem.h"
#include "Wfl.h"

/*
** Local include files
*/
#include "WflPrivate.h"

/* Global variables */
#if defined E_WFL_OS_QNX4
#endif
#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
static dispatch_t         *mWflDpp = NULL;
static resmgr_context_t   *mWflCtp = NULL;

#endif

#ifndef I_WFL_OS_UNSUPPORTED

/******************************************************************************
** Function Name:
**    eWflDeviceAttach
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NODEVNO     - Error retreiving a device number
**
** Purpose:
**    Obtain a device number to attach the device to.
**
** Description:
**
** Arguments:
**    int *pDevNo   -   Pointer to device number
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflDeviceAttach( int *pDevNo )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   *pDevNo = qnx_device_attach();
   if( *pDevNo == -1 )
      Status = E_WFL_NODEVNO;

   #endif

   #if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
   #endif

   return Status;

}
/******************************************************************************
** Function Name:
**    eWflPrefixAttach
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_NODPP       - Error getting a dispatch handle (QNX6)
**         E_WFL_NOPREFIX    - Error assigning prefix
**
** Purpose:
**    Create the device and associate it with this process.
**
** Description:
**
** Arguments:
**    char *DevName   -    Pointer to name of device to be created "/dev/camera"
**    int   DevNo     -    Device number to associate with device (QNX4)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflPrefixAttach(char *DevName, int DevNo)
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4
   int ret;

   ret = qnx_prefix_attach(DevName, NULL, DevNo);
   if( ret == -1 )
      Status = E_WFL_NOPREFIX;

   #endif

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
   static resmgr_connect_funcs_t    connect_funcs;
   static resmgr_io_funcs_t         io_funcs;
   static iofunc_attr_t             attr;
   resmgr_attr_t        resmgr_attr;
   int                  id;

    
   if( mWflDpp == NULL )
   {
      mWflDpp = dispatch_create();
   }

   /* Check to see if we've got a dispatch handle */
   if( mWflDpp == NULL )
   {
      return E_WFL_NODPP;
   }


    /* initialize resource manager attributes */
    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /* initialize functions for handling messages */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, 
                     _RESMGR_IO_NFUNCS, &io_funcs);

    /* initialize attribute structure used by the device */
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);

    /* attach our device name */
    id = resmgr_attach(mWflDpp,        /* dispatch handle        */
                       &resmgr_attr,   /* resource manager attrs */
                       DevName,        /* device name            */
                       _FTYPE_ANY,     /* open type              */
                       0,              /* flags                  */
                       &connect_funcs, /* connect routines       */
                       &io_funcs,      /* I/O routines           */
                       &attr);         /* handle                 */
    if(id == -1) 
    {
       return E_WFL_NOPREFIX;
    }


#endif

   return Status;
}
/******************************************************************************
** Function Name:
**    eWflResMgrReceive
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_RESMGERR    - Error allocating the resource manager context
**
** Purpose:
**    Check for messages from client.
**
** Description:
**
** Arguments:
**    int  *pRcvId           - Id of message source
**    int  *pType            - Type of message
**    eWflResMgrMsg_t **pMsg - Pointer to the incoming message
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflResMgrReceive(int *pRcvId,int *pType, eWflResMgrMsg_t **pMsg)
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   *pRcvId = Receive( 0, *pMsg, sizeof( eWflResMgrMsg_t ) );
   if( *pRcvId == -1 )
      Status = E_WFL_RESRECERR ;

   *pType = (*pMsg)->type;

   #endif

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX

   /* start the resource manager message loop */
   mWflCtp = resmgr_block(mWflCtp);

   *pMsg   = mWflCtp;
   *pRcvId = mWflCtp->rcvid;
   *pType  = mWflCtp->msg->type;

#endif

   return Status;
}
/******************************************************************************
** Function Name:
**    eWflResMgrMessAlloc
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_RESMGERR    - Error allocating the resource manager context
**
** Purpose:
**    Allocate memory for incoming resource manager messages.
**
** Description:
**
** Arguments:
**    eWflResMgrMsg_t **pMsg - Pointer to the incoming message
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflResMgrMessAlloc( eWflResMgrMsg_t **pMsg )
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4

   *pMsg = (eWflResMgrMsg_t *) malloc (sizeof(eWflResMgrMsg_t));
   if( *pMsg == NULL )
      Status = E_WFL_RESMGERR;

   #endif

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
   /* allocate a context structure */
   if( mWflCtp == NULL )
   {
      mWflCtp = resmgr_context_alloc(mWflDpp);
      if( mWflCtp == NULL )
         Status = E_WFL_RESMGERR;
   }
#endif

   return Status;
}
/******************************************************************************
** Function Name:
**    eWflResMgrReply
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_RESMGERR    - Error allocating the resource manager context
**
** Purpose:
**    Check for messages from client.
**
** Description:
**
** Arguments:
**    int  RcvId            - Id of message source
**    int  Type             - Type of message
**    eWflResMgrMsg_t *pMsg - Pointer to the incoming message
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflResMgrReply(int RcvId, eWflResMgrMsg_t *pMsg, int ReplySize)
{
   Status_t Status = SYS_NOMINAL;

   #if defined E_WFL_OS_QNX4
   Reply(RcvId,pMsg,ReplySize);
   #endif
  
#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
   int RetStat;

   RetStat = pMsg->status;

   MsgReply(RcvId,RetStat,pMsg,ReplySize);
#endif

   return Status;
}
/******************************************************************************
** Function Name:
**    eWflOpenMessage
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Reads the contents of a waiting message buffer sent by a write command in 
**    client process.
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflOpenMessage( eWflResMgrMsg_t **pMsg, int *ReplySize, int RetStat ) 
{


#if defined I_WFL_OS_QNX4  
   /* Set the reply to EOK */
   (*pMsg)->open_reply.status = RetStat;
   /* Set the reply message size */
   *ReplySize = sizeof( (*pMsg)->open_reply );
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   _RESMGR_STATUS( *pMsg, RetStat );
   *ReplySize = sizeof( eWflResMgrMsg_t );
#endif /* QNX6 specific code */

   return SYS_NOMINAL;
 
} /* End of eWflOpenMessage() */
/******************************************************************************
** Function Name:
**    eWflDupMessage
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Response to a IO_DUP message.
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflDupMessage( eWflResMgrMsg_t **pMsg, int *ReplySize, int RetStat ) 
{


#if defined I_WFL_OS_QNX4  
   /* Set the reply to EOK */
   (*pMsg)->dup_reply.status = RetStat;
   /* Set the reply message size */
   *ReplySize = sizeof( (*pMsg)->dup_reply );
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   _RESMGR_STATUS( *pMsg, RetStat );
   *ReplySize = sizeof( eWflResMgrMsg_t );
#endif /* QNX6 specific code */

   return SYS_NOMINAL;
 
} /* End of eWflDupMessage() */
/******************************************************************************
** Function Name:
**    eWflCloseMessage
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Response to a IO_CLOSE message.
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflCloseMessage( eWflResMgrMsg_t **pMsg, int *ReplySize, int RetStat ) 
{


#if defined I_WFL_OS_QNX4  
   /* Set the reply to EOK */
   (*pMsg)->close_reply.status = RetStat;
   /* Set the reply message size */
   *ReplySize = sizeof( (*pMsg)->close_reply );
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   _RESMGR_STATUS( *pMsg, RetStat );
   *ReplySize = sizeof( eWflResMgrMsg_t );
#endif /* QNX6 specific code */

   return SYS_NOMINAL;
 
} /* End of eWflCloseMessage() */
/******************************************************************************
** Function Name:
**    eWflWriteReply
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Generates the appropriate reply to a write message
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflWriteReply( eWflResMgrMsg_t **pMsg, int *pReplySize, int Nbytes, int RetStat ) 
{


#if defined I_WFL_OS_QNX4  
   (*pMsg)->write_reply.nbytes = Nbytes;
   (*pMsg)->write_reply.status = RetStat;
   (*pMsg)->write_reply.zero = 0;
   *pReplySize = sizeof((*pMsg)->write_reply);
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   (*pMsg)->status = RetStat;
   *pReplySize = sizeof((*pMsg)->msg->write);
#endif /* QNX6 specific code */

   return SYS_NOMINAL;
 
} /* End of eWflWriteReply() */
/******************************************************************************
** Function Name:
**    eWflReadReply
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**    Generates the appropriate reply to a read message
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflReadReply( eWflResMgrMsg_t **pMsg, int Nbytes, int RetStat ) 
{


#if defined I_WFL_OS_QNX4  
   (*pMsg)->read_reply.nbytes = Nbytes;
   (*pMsg)->read_reply.status = RetStat;
   (*pMsg)->read_reply.zero = 0;
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   (*pMsg)->status = Nbytes;
#endif /* QNX6 specific code */

   return SYS_NOMINAL;
}/* End of eWflReadReply */

/******************************************************************************
** Function Name:
**    eWflWriteMessInfo
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Return the size of the incoming message.
**
** Description:
**
** Arguments:
**    eWflResMgrMsg_t *pMsg   -  Incoming resource message
**    int             *pSize  -  Size of incoming message
**    int             *pOffset-  Offset to message buffer 
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflWriteMessInfo( eWflResMgrMsg_t *pMsg, int *pSize, int *pOffset ) 
{


#if defined I_WFL_OS_QNX4  
   *pSize = (int)pMsg->write.nbytes;
   *pOffset = sizeof(pMsg->write) - sizeof(pMsg->write.data);
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   io_write_t *pWrite;

   pWrite = &(pMsg->msg->write);

   *pSize   = (pWrite->i.nbytes);
   *pOffset = sizeof(pWrite->i);
#endif /* QNX6 specific code */

   return SYS_NOMINAL;
 
} /* End of eWflWriteMessInfo() */
/******************************************************************************
** Function Name:
**    eWflReadMessInfo
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Return the size of the incoming message.
**
** Description:
**
** Arguments:
**    eWflResMgrMsg_t *pMsg   -  Incoming resource message
**    int             *pSize  -  Size of incoming message
**    int             *pOffset-  Offset to message buffer 
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflReadMessInfo( eWflResMgrMsg_t *pMsg, int *pSize, int *pOffset ) 
{


#if defined I_WFL_OS_QNX4  
   *pSize = (int)pMsg->read.nbytes;
   *pOffset = sizeof(pMsg->read) - sizeof(pMsg->read.data);
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   io_read_t *pRead;

   pRead = &(pMsg->msg->read);

   *pSize   = (pRead->i.nbytes);
   *pOffset = sizeof(pRead->i);
#endif /* QNX6 specific code */

   return SYS_NOMINAL;
 
} /* End of eWflReadMessInfo() */
/******************************************************************************
** Function Name:
**    eWflIoCtlInfo
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Return the info related to the IO request
**
** Description:
**
** Arguments:
**    eWflResMgrMsg_t *pMsg   -  Incoming resource message
**    int             *pRetSize  -  Size of the devctl/qioctl message
**    int             *pInOff    -  Pointer to the input message offset
**    int             *pOutOff   -  Pointer to the output message offset
**    int             *pRequest  -  The devctl/qioctl request
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflIoCtlMessInfo( eWflResMgrMsg_t *pMsg, 
                            int *pRetSize, 
                            int *pInOff, 
                            int *pOutOff, 
                            int *pRequest ) 
{


#if defined I_WFL_OS_QNX4  
   *pRetSize = sizeof(pMsg->qioctl_reply);
   *pRequest = pMsg->qioctl.request;

   *pInOff  = sizeof(pMsg->qioctl) - sizeof(pMsg->qioctl.data);
   *pOutOff = sizeof(pMsg->qioctl_reply) - sizeof(pMsg->qioctl_reply.data);
#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
   io_devctl_t *pDevctl;

   pDevctl = &(pMsg->msg->devctl);

  *pRequest = pDevctl->i.dcmd;
  *pRetSize = sizeof( eWflResMgrMsg_t );

  *pInOff  = sizeof( pDevctl->i );
  *pOutOff = sizeof( pDevctl->o );


#endif /* QNX6 specific code */

   return SYS_NOMINAL;
 
} /* End of eWflIoCtlMessInfo() */
/******************************************************************************
** Function Name:
**    eWflIoCtlReply
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**
** Purpose:
**       Set the reply to the qioctl/devctl message
**
** Description:
**
** Arguments:
**    eWflResMgrMsg_t *pMsg    -  Incoming resource message
**    int              RetVal  -  Return value in qioctl/devctl message
**    int              RetStat -  Return status in devctl/qioctl request
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflIoctlReply( eWflResMgrMsg_t *pMsg, int RetVal, int RetStat ) 
{
   #if defined E_WFL_OS_QNX4
   pMsg->qioctl_reply.status = RetStat;
   pMsg->qioctl_reply.ret_val = RetVal;
   #endif

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
   io_devctl_t *pdevctl;
   
   pMsg->status = RetStat;

   pdevctl = &(pMsg->msg->devctl);
   pdevctl->o.ret_val = RetVal;
#endif

   return SYS_NOMINAL;
}
/******************************************************************************
** Function Name:
**    eWflReadMessage
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_MSGREADERR  - Error reading message
**
** Purpose:
**    Reads the contents of a waiting message buffer sent by a write command in 
**    client process.
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflReadMessage( int pid, void *msg, int offset, int nbytes )
{
   Status_t Status = SYS_NOMINAL;
   int Ret;


#if defined I_WFL_OS_QNX4  

   /* Begin QNX4 specific code */
   Ret = Readmsg( (pid_t) pid, (unsigned) offset, msg, (unsigned) nbytes);

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
  
   /* Begin QNX6 specific code */
   Ret = MsgRead( (int) pid, msg, (int) nbytes, (int) offset);

#endif /* QNX6 specific code */

   if( Ret == -1 )
      Status = E_WFL_MSGREADERR;

   return Status;
 
} /* End of eWflReadMessage() */
/******************************************************************************
** Function Name:
**    eWflWriteMessage
**
** Type:
**    Status_t
**       Returns the status of the function on completion:
**         SYS_NOMINAL       - normal successful completion
**         E_WFL_MSGWRITERR  - Error writing message
**
** Purpose:
**    Write the contents of a message buffer to client process
**
** Description:
**
** Arguments:
**    None (void)
**
** Authors:
**    Martin Norbury (man)
**
******************************************************************************/
Status_t eWflWriteMessage( int pid, void *msg, int offset, int nbytes )
{
   Status_t Status = SYS_NOMINAL;
   int Ret;


#if defined I_WFL_OS_QNX4  

   /* Begin QNX4 specific code */
   Ret = Writemsg( (pid_t) pid, (unsigned) offset, msg, (unsigned) nbytes);

#endif /* QNX4 specific code */

#if defined I_WFL_OS_QNX6
  
   /* Begin QNX6 specific code */
   Ret = MsgWrite( (int) pid, msg, (int) nbytes, (int) offset);

#endif /* QNX6 specific code */

   if( Ret == -1 )
      Status = E_WFL_MSGWRITERR;

   return Status;
 
} /* End of eWflWriteMessage() */

#endif /* End of unsupported */
/*
** EOF
*/
