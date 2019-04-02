/******************************************************************************
** Header File Name:
**    Mbl.h
**
** Purpose:
**    Public header file for the modbus cummications library.
**
** Description:
**    Public header file (.h file) for the MBL package (.c) files.
**
** Traceability to Software Design:
**
** Author(s):
**    Mark Bowman (mkb)
**
** Copyright (C) Telescope Technologies Limited (TTL), 2001
** 
** Version:
**    $Id: Mbl.h,v 0.4 2001/10/16 13:34:09 mkb Exp $
**
** History:
**    $Log: Mbl.h,v $
**    Revision 0.4  2001/10/16 13:34:09  mkb
**    Added status codes relating to inconsitancy in reply message.
**
**    Revision 0.3  2001/10/11 09:30:40  mkb
**    Corrections due to peer review by djm.
**
**    Revision 0.2  2001/10/08 14:39:05  mkb
**    Minor fixes to prototypes.
**
**    Revision 0.1  2001/10/05 15:56:47  mkb
**    First revision.
**
******************************************************************************/


#ifndef MBL_H_DEFINED
#define MBL_H_DEFINED

/* 
** Required include files. 
*/

#include "TtlSystem.h"

/* 
** Type definitions required to use Modbus library. 
*/

typedef int eMblSlave_t;       /* Identifier for a modbus slave device. */
typedef int eMblPort_t;        /* Identifier for a modbus network.      */

typedef enum                   /* Modbus function codes.                */
{                             
   E_MBL_FUNCTION_BOL,                /* Beginning of list.             */
   E_MBL_FUNCTION_READ_INPUT = 0X04,  /* Read analogue input value.     */
   E_MBL_FUNCTION_EOL                 /* End of list.                   */
} eMblFunction_t;

typedef enum                   /* Comms method to modbus devices.       */
{
   E_MBL_COMMS_BOL,            /* Beginning of list.                    */  
   E_MBL_COMMS_RS232_COM1,     /* RS232 comms. Port 1.                  */
   E_MBL_COMMS_RS232_COM2,     /* RS232 comms. Port 2.                  */
   E_MBL_COMMS_RS485,          /* RS484 comms.                          */
   E_MBL_COMMS_EOL             /* End of list.                          */
} eMblCommsDevice_t;
   

/*
** Enumertor lists. 
*/ 

enum eMblStatus_e               /* Returned status codes.               */
{
   E_MBL_GEN_ERR = STATUS_START(MBL),  /* Miscellaneous error.          */
   E_MBL_OPEN_FAIL,             /* Unable to open modbus port.          */
   E_MBL_CLOSE_FAIL,            /* Failure on closing modbus port.      */
   E_MBL_INSUF_DATA,            /* Non-positive amount of data to Tx.   */
   E_MBL_SEND_ERR,              /* Failure to send modbus request.      */
   E_MBL_RECV_ERR,              /* Failure to receive reply to request. */
   E_MBL_RX_TIMEOUT,            /* Time-out while waiting for a reply.  */
   E_MBL_TX_TIMEOUT,            /* Time-out while sending a request.    */
   E_MBL_FLUSH_FAIL,            /* Error flushing input/output buffers. */
   E_MBL_CONFIG_ERR,            /* Error configuring serial device.     */
   E_MBL_TIMEOUT_INIT_ERR,      /* Failed to create timer.              */
   E_MBL_ILLEGAL_FUNCTION,      /* Device does not support requested    */
                                /* function.                            */
   E_MBL_ILLEGAL_DATA_ADDRESS,  /* Requested data address is not        */
                                /* supported by device.                 */
   E_MBL_ILLEGAL_DATA_VALUE,    /* Value in data query is not allowed   */
                                /* for this device.                     */
   E_MBL_SLAVE_DEVICE_FAILURE,  /* Unrecoverable error occurred during  */
                                /* request.                             */
   E_MBL_ACKNOWLEDGE,           /* Long duration request in progress.   */
   E_MBL_SLAVE_DEVICE_BUSY,     /* Device not ready to accept new       */
                                /* request.                             */
   E_MBL_NEGATIVE_ACKNOWLEDGE,  /* Device cannot perform request.       */  
   E_MBL_MEMORY_PARITY_ERROR,   /* Device detected memory parity error. */
   E_MBL_BUFFER_OVERRUN,        /* A message buffer has over-run.       */
   E_MBL_ILLEGAL_SLAVE_ID,      /* Illegal identifier for slave device. */
   E_MBL_UNKNOWN_COMMS_DEVICE,  /* Unknown communication device.        */
   E_MBL_FUNCTION_MISMATCH,     /* Reply contains different function    */
                                /* code to original request.            */

   E_MBL_STATUS_EOL             /* End of list. Do not use as index.    */
};


/*
** Prototpes for exported functions.
*/

/******************************************************************************
** Function Name:
**    eMblSetup
** 
** Purpose:
**    Initialise communications path to Modbus devices.
**
** Return type:
**    Status_t
**       Completion status of setup function - see enum list eMblStatus_e.
**
** Arguments:
**    eMblCommsDevice_t Device        (in)
**       Protocol and device to transmit Modbus requests over.
**    eMblPort_t       *PortPtr       (out)
**       Pointer to an identifier for communications port opened by eMblSetup 
**       function.
**
******************************************************************************/

extern Status_t eMblSetup( 
                          eMblCommsDevice_t Device, 
                          eMblPort_t       *PortPtr
                          ); 


/******************************************************************************
** Function Name:
**    eMblClose
** 
** Purpose:
**    Close the communications path to Modbus devices.
**
** Return type:
**    Status_t
**       Completion status of setup function - see enum list eMblStatus_e.
**
** Arguments:
**    eMblPort_t   Port              (in)
**       An identifier for communications port to close.
**
******************************************************************************/

Status_t eMblClose( 
                   eMblPort_t Port
		   );


/******************************************************************************
** Function Name:
**    eMblRequest
** 
** Purpose:
**    Submit a modbus request to a slave device over a previously defined port.
**
** Return type:
**    Status_t
**       Completion status of request function - see enum list eMblStatus_e.
**
** Arguments:
**    eMblPort_t      Port           (in)
**       Identifier for communications port opened by eMblSetup function.
**    eMblSlave_t     Slave          (in)
**       Slave device to send request to.
**    eMblFunction_t  Function       (in)
**       Code for Modbus function to execute.
**    Int32_t         Address        (in)
**       Data address relating to request.
**    Int32_t         Count          (in)
**       Number of registers request relates to 
**       (ignored if not relevant to function).
**    void           *BufferPtr      (in/out)
**       Buffer for submitted and returned data 
**       (ignored if not relevant to function).
**    Int32_t              *BufferSizePtr  (in/out)
**       (in)  For submitted data - Number of bytes in Buffer.
**             For retreived data - Size of provided buffer. 
**       (out) For submitted data - Number of bytes of data submitted to slave.
**             For retreived data - Number of bytes in Buffer. 
**
******************************************************************************/

extern Status_t eMblRequest( 
                            eMblPort_t     Port, 
                            eMblSlave_t    Slave,
                            eMblFunction_t Function,  
                            Int32_t        Address,
                            Int32_t        Count, 
                            void          *BufferPtr, 
                            Int32_t       *BufferSizePtr
                            ); 


#endif

/******************************************************************************
** End of File: Mbl.h
******************************************************************************/
