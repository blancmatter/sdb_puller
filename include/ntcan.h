/* ntcan.h
**
**            Copyright (c) 2001 by electronic system design gmbh
**
**  This software is copyrighted by and is the sole property of 
**  esd gmbh.  All rights, title, ownership, or other interests
**  in the software remain the property of esd gmbh. This
**  software may only be used in accordance with the corresponding
**  license agreement.  Any unauthorized use, duplication, transmission,  
**  distribution, or disclosure of this software is expressly forbidden.
**
**  This Copyright notice may not be removed or modified without prior
**  written consent of esd gmbh.
**
**  esd gmbh, reserves the right to modify this software without notice.
**
**  electronic system design gmbh          Tel. +49-511-37298-0
**  Vahrenwalder Str 205                   Fax. +49-511-37298-68 
**  30165 Hannover                         http://www.esd-electronics.com
**  Germany                                slaes@esd-electronics.com
**
**
*/

#ifndef _ntcan_h_
#define _ntcan_h_

#include <stdint.h>

#ifndef EXPORT
#define EXPORT
#endif

#ifndef CALLTYPE
#define CALLTYPE
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*------------------ Defines ------------------------------------------------*/
#define NTCAN_EV_BASE 0x40000000
#define NTCAN_EV_USER 0x40000080
#define NTCAN_EV_LAST 0x400000FF

#define NTCAN_20B_BASE 0x20000000

#define NTCAN_RTR      0x10
#define NTCAN_NO_DATA  0x20

#define NTCAN_EV_CAN_ERROR NTCAN_EV_BASE


/* mode in canOpen() */
#define NTCAN_MODE_LOM          0x00010000 /* mode-mask for listening only mode */
#define NTCAN_MODE_OBJECT       0x10000000 /* mode-mask for flat mode           */
#define NTCAN_MODE_OVERLAPPED   0x20000000 /* reserved for windows, only        */

/* Queue-Size in canOpen() */
#define NTCAN_MAX_TX_QUEUESIZE 0x4000
#define NTCAN_MAX_RX_QUEUESIZE 0x4000

#define NTCAN_NO_QUEUE        -1                 /* Open for Rx object mode        */


/* No baudrate configured (returned from canGetBaudrate() )*/
#define NTCAN_NO_BAUDRATE         0x7FFFFFFF
  
#define NTCAN_ERRNO_BASE             0x00000100
/*------------------- error-codes--------------------------------------------*/
#define NTCAN_SUCCESS                0
#define NTCAN_INVALID_PARAMETER      EINVAL 
#define NTCAN_INVALID_HANDLE         EBADF 
#define NTCAN_NET_NOT_FOUND          ENOENT  
#define NTCAN_INSUFFICIENT_RESOURCES ENOMEM
#define NTCAN_OPERATION_ABORTED      EINTR
#define NTCAN_WRONG_DEVICE_STATE     EPIPE 
#define NTCAN_HANDLE_FORCED_CLOSE    ENOSPC
#define NTCAN_NOT_IMPLEMENTED        ENOTTY
#define NTCAN_NOT_SUPPORTED          ENOSYS

#define NTCAN_RX_TIMEOUT             (NTCAN_ERRNO_BASE +1)
#define NTCAN_TX_TIMEOUT             (NTCAN_ERRNO_BASE +2)

#define NTCAN_TX_ERROR               (NTCAN_ERRNO_BASE +4)
#define NTCAN_CONTR_OFF_BUS          (NTCAN_ERRNO_BASE +5)
#define NTCAN_CONTR_BUSY             (NTCAN_ERRNO_BASE +6)
#define NTCAN_CONTR_WARN             (NTCAN_ERRNO_BASE +7)

#define NTCAN_NO_ID_ENABLED          (NTCAN_ERRNO_BASE +9)
#define NTCAN_ID_ALREADY_ENABLED     (NTCAN_ERRNO_BASE +10)
#define NTCAN_ID_NOT_ENABLED         (NTCAN_ERRNO_BASE +11)
   
#define NTCAN_INVALID_FIRMWARE       (NTCAN_ERRNO_BASE +13)
#define NTCAN_MESSAGE_LOST           (NTCAN_ERRNO_BASE +14)
#define NTCAN_INVALID_HARDWARE       (NTCAN_ERRNO_BASE +15)
#define NTCAN_PENDING_WRITE          (NTCAN_ERRNO_BASE +16)
#define NTCAN_PENDING_READ           (NTCAN_ERRNO_BASE +17)
#define NTCAN_INVALID_DRIVER         (NTCAN_ERRNO_BASE +18)

#define NTCAN_SOCK_CONN_TIMEOUT      (NTCAN_ERRNO_BASE + 0x80)
#define NTCAN_SOCK_CMD_TIMEOUT       (NTCAN_ERRNO_BASE + 0x81)
#define NTCAN_SOCK_HOST_NOT_FOUND    (NTCAN_ERRNO_BASE + 0x82)  /* gethostbyname() failed */

typedef int NTCAN_RESULT;
typedef int OVERLAPPED;
typedef int NTCAN_HANDLE;


typedef struct
{
  int32_t id ;             /* can-id                                   */
  uint8_t len ;            /* length of message: 0-8                   */
  uint8_t msg_lost ;       /* count of lost rx-messages                */
  uint8_t reserved[2] ;    /* reserved                                 */
  uint8_t data[8] ;        /* 8 data-bytes                             */
} CMSG;


typedef struct
{
  int32_t evid;           /* event-id: possible range:EV_BASE...EV_LAST */
  uint8_t len;            /* length of message: 0-8                     */
  uint8_t reserved[3];    /* reserved                                   */
  union
  {
    uint8_t  c[8];
    uint16_t s[4];
    uint32_t  l[2];
  } evdata ;
} EVMSG;


typedef struct
{
  int32_t id;                /* can-id                                   */
  uint8_t len;               /* length of message: 0-8                   */
  uint8_t msg_lost ;         /* count of lost rx-messages                */
  uint8_t reserved[2] ;      /* reserved                                 */
  uint8_t data[8] ;          /* 8 data-bytes                             */
  uint64_t timestamp ;       /* time stamp of this message               */
} CMSG_T;

typedef struct
{
  uint16_t hardware ;
  uint16_t firmware ;
  uint16_t driver ;
  uint16_t dll ;
  uint32_t  boardstatus ;
  uint8_t  boardid[14] ;
  uint16_t features ;
} CAN_IF_STATUS;

typedef struct
{
  int32_t id;
  int32_t flags;
  uint64_t time_start;
  uint64_t time_interval;
  uint8_t reserved[8];
} CSCHED;



#define NTCAN_FEATURE_FULL_CAN       (1<<0)   /*! Marks Full-CAN-controller, used by nucleus for flat-mode,... */
#define NTCAN_FEATURE_CAN_20B        (1<<1)   /*! CAN 2.OB support                                             */
#define NTCAN_FEATURE_LOM            (1<<2)   /*! feature/mode flag for listening-only-mode                    */
#define NTCAN_FEATURE_CYCLIC_TX      (1<<3)   /*!                                                              */
#define NTCAN_FEATURE_OBJECT_MODE    (1<<4)   /*! DIFFERS FROM MODE FLAG: flat mode                            */
#define NTCAN_FEATURE_TIMESTAMP      (1<<5)   /*! feature flag for timestamping                                */ 


#define NTCAN_STATUS_OK              0       /*! can Status = OK                                           */ 
#define NTCAN_STATUS_WARN            (1<<0)  /*! can status = controller warn                              */
#define NTCAN_STATUS_BUSOFF          (1<<1)  /*! can status = controller off bus                           */
#define NTCAN_STATUS_WRONG_FIRMWARE  (1<<2)  /*! Wrong firmware version (disables CAN-functionality)       */
#define NTCAN_STATUS_WRONG_HARDWARE  (1<<3)  /*! Wrong hardware version (disables CAN-functionality)       */

/*
 * IOCTL codes for canIoctl()
 */
#define NTCAN_IOCTL_FLUSH_RX_FIFO         0x0001   /* Flush Rx FIFO                  */
#define NTCAN_IOCTL_GET_RX_MSG_COUNT      0x0002   /* Ret # CMSG in Rx FIFO          */
#define NTCAN_IOCTL_GET_RX_TIMEOUT        0x0003   /* Ret configured Rx tout         */
#define NTCAN_IOCTL_GET_TX_TIMEOUT        0x0004   /* Ret configured Tx tout         */
#define NTCAN_IOCTL_SET_20B_HND_FILTER    0x0005   /* Configure 20B filter           */
#define NTCAN_IOCTL_GET_SERIAL            0x0006   /* Get HW serial number           */
#define NTCAN_IOCTL_GET_TIMESTAMP_FREQ    0x0007   /* Get timestamp frequency in Hz  */
#define NTCAN_IOCTL_GET_TIMESTAMP         0x0008   /* Get timestamp counter          */
#define NTCAN_IOCTL_ABORT_RX              0x0009   /* Abort pending Rx I/O           */
#define NTCAN_IOCTL_ABORT_TX              0x000a   /* Abort pending Tx I/O           */
#define NTCAN_IOCTL_SET_RX_TIMEOUT        0x000b   /* Change Rx timeout              */
#define NTCAN_IOCTL_SET_TX_TIMEOUT        0x000c   /* Change Rx timeout              */
#define NTCAN_IOCTL_TX_OBJ_CREATE         0x000d   /* Create obj, arg->CMSG          */
#define NTCAN_IOCTL_TX_OBJ_AUTOANSWER_ON  0x000e   /* Switch autoanswer on,arg->CMSG */
#define NTCAN_IOCTL_TX_OBJ_AUTOANSWER_OFF 0x000f   /* Switch autoanswer off,arg->CMSG*/
#define NTCAN_IOCTL_TX_OBJ_UPDATE         0x0010   /* update  obj, arg->CMSG         */ 
#define NTCAN_IOCTL_TX_OBJ_DESTROY        0x0011   /* Destroy obj, arg->id           */
#define NTCAN_IOCTL_TX_OBJ_SCHEDULE       0x0012   /* Set sched. for obj,arg->CSCHED */
#define NTCAN_IOCTL_TX_OBJ_SCHEDULE_START 0x0013   /* Start scheduling for handle    */
#define NTCAN_IOCTL_TX_OBJ_SCHEDULE_STOP  0x0014   /* Stop scheduling for handle     */

/*...prototypes..............................................................*/
EXPORT NTCAN_RESULT CALLTYPE canSetBaudrate(NTCAN_HANDLE handle,/* nt-handle       */
			  uint32_t baud );              /* baudrate-constant */

/*...prototypes..............................................................*/
EXPORT NTCAN_RESULT CALLTYPE canGetBaudrate(NTCAN_HANDLE handle,/* nt-handle       */
			  uint32_t *baud );      /* Out: baudrate            */
/*...........................................................................*/
EXPORT NTCAN_RESULT CALLTYPE canOpen(             
	        int32_t  net,               /* net number                    */
		uint32_t flags,             /*                               */
                int32_t  txqueuesize,       /* nr of entries in message queue*/
                int32_t  rxqueuesize,       /* nr of entries in message queue*/
                int32_t  txtimeout,         /* tx-timeout in miliseconds     */
                int32_t  rxtimeout,         /* rx-timeout in miliseconds     */
		NTCAN_HANDLE   *handle );         /* out: nt-handle                */
/*...........................................................................*/
EXPORT NTCAN_RESULT   CALLTYPE canClose( /* ret NTCAN_OK, if success         */
                NTCAN_HANDLE        handle );  /* handle                           */
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE      canIdAdd( /* ret NTCAN_OK, if success   */
                NTCAN_HANDLE        handle,    /* read handle                      */
                int32_t       id  );     /* can identifier                   */
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canIdDelete( /* ret 0, if success            */
                NTCAN_HANDLE        handle,    /* read handle                      */
                int32_t       id  );     /* can identifier                   */
/*...........................................................................*/
EXPORT NTCAN_RESULT   CALLTYPE canGetOverlappedResult(
                NTCAN_HANDLE        handle,    /* handle                           */
		OVERLAPPED    *ovrlppd,  /* overlapped-structure             */
		int32_t       *len,      /* out: cnt of available CMSG-Buffer*/
		int           bWait );   /* FALSE =>do not wait, else wait   */        
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canTake( /* ret 0, if success                */
	        NTCAN_HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                int32_t       *len );    /* out: size of CMSG-Buffer         */
                                         /* in:  count of read messages      */
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canRead( /* ret 0, if success                */
                NTCAN_HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                int32_t       *len,      /* out: size of CMSG-Buffer         */ 
                                         /* in:  count of read messages      */
                OVERLAPPED    *ovrlppd); /* NULL or overlapped-structure     */
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canSend( /* ret 0, if success                */
                NTCAN_HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                int32_t       *len );    /* size of CMSG-Buffer              */ 
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canWrite(/* ret 0, if success                */
                NTCAN_HANDLE        handle,    /* handle                           */
                CMSG          *cmsg,     /* ptr to data-buffer               */
                int32_t       *len,      /* size of CMSG-Buffer              */ 
                OVERLAPPED    *ovrlppd); /* NULL or overlapped-structure     */
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canReadEvent(
                NTCAN_HANDLE        handle,    /* handle                           */
                EVMSG         *evmsg,    /* ptr to event-msg-buffer          */
                OVERLAPPED    *ovrlppd); /* NULL or overlapped-structure     */
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canSendEvent(
                NTCAN_HANDLE        handle,    /* handle                           */
                EVMSG         *evmsg );  /* ptr to event-msg-buffer          */
/*...........................................................................*/
EXPORT NTCAN_RESULT    CALLTYPE canStatus(
                NTCAN_HANDLE        handle,    /* handle                           */
                CAN_IF_STATUS *cstat );  /* ptr to can-status-buffer         */
/*...........................................................................*/
EXPORT NTCAN_RESULT CALLTYPE canIoctl(
                NTCAN_HANDLE        handle,    /* handle                           */
		uint32_t      ulCmd,     /* Command                          */
		void *        pArg );    /* Ptr to command specific argument */
/*...........................................................................*/
EXPORT NTCAN_RESULT CALLTYPE canTakeT(
                NTCAN_HANDLE        handle,    /* In: handle                       */
                CMSG_T        *cmsg_t,   /* In: ptr to receive buffer        */
                int32_t       *len );    /* out: size of CMSG_T-Buffer       */
                                         /* in:  count of read messages      */
/*...........................................................................*/
EXPORT NTCAN_RESULT CALLTYPE canReadT(
                NTCAN_HANDLE        handle,    /* In: handle                       */
                CMSG_T        *cmsg_t,   /* Int ptr to receive buffer        */
                int32_t       *len,      /* out: size of CMSG_T-Buffer       */
                                         /* in:  count of read messages      */
                OVERLAPPED    *ovrlppd); /* In: always NULL                  */
/*...........................................................................*/
EXPORT NTCAN_RESULT CALLTYPE canSendT(   /* ret 0, if success                */
                NTCAN_HANDLE        handle,    /* handle                           */
                CMSG_T        *cmsg_t,   /* ptr to data-buffer               */
                int32_t       *len );    /* size of CMSG-Buffer              */ 


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _ntcan_h_ */

