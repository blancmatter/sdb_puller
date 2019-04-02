/* 
**
**  Module Name : 
**    	Can.h
**
**  Purpose :
**		Public header file for CAN package.
**
**  Description :
**		Used to declare exported data, functions and definitions for the
**		CAM package.
**
**  Authors :
**		PXH : Pete Hall (TTL)
**
**  Version :
**      $Id: Can.h,v 0.16 2001/11/02 10:52:04 mjf Exp $
**
**  History: 
**    	$Log: Can.h,v $
**    	Revision 0.16  2001/11/02 10:52:04  mjf
**    	Re-write to periodically poll for items from the CAN bus. When an item
**    	is polled the first time, it will be routinely retrieved until it
**    	hasn't been polled for in a finite time. Once an item is being
**    	periodically retrieved, any request will be served immediately with the
**    	last retrieved parameter, to increase responsiveness.
**
**    	Revision 0.15  2001/10/08 10:06:34  mjf
**    	Attach using a different name if part of the mirror support sub-system.
**
**    	Revision 0.14  2001/10/02 16:11:49  mjf
**    	Change of server shutdown OID name.
**
**    	Revision 0.13  2001/09/28 16:24:29  mjf
**    	Implementation of relative as well as absolute moves. Removal of
**    	functions to get CTR2 and CTR3, to be implemented using a GetInput
**    	function with the register type specified in the IO number.
**
**    	Revision 0.12  2001/08/28 11:04:58  djm
**    	Removed incorrect comma in enum list.
**
**    	Revision 0.11  2001/08/16 09:29:50  mjf
**    	Alteration of debug levels.
**
**    	Revision 0.10  2001/04/20 11:20:06  pxh
**    	Removed end-client command definitions - these have been moved
**    	into AgenServer.h
**
**    	Revision 0.9  2001/04/12 13:42:26  pxh
**    	Added new status code E_CAN_DEVICE_NOT_FOUND.
**
**    	Revision 0.8  2001/03/15 11:27:56  pxh
**    	Added D_CAN_APP_VERSION OID to list.
**
**    	Revision 0.7  2001/03/06 17:20:52  pxh
**    	Replaced single CAN device Id for ACADS with three for ACADS Red,
**    	Yellow & Blue.
**
**    	Revision 0.6  2001/02/07 16:38:07  pxh
**    	Corrected server path to include executable name.
**
**    	Revision 0.5  2001/02/02 16:51:13  pxh
**    	Removed unwanted OID definitions.
**
**    	Revision 0.4  2001/01/30 15:23:47  pxh
**    	Changed server name and path definitions for compatibility with
**    	OidServer.
**
**    	Revision 0.3  2001/01/30 11:24:20  pxh
**    	Added temporary E_TTL_ENDCLIENT_DISABLE_LYNX command.
**    	Renamed E_CAN_SFR_DEV to E_CAN_SFP_DEV to avoid clash.
**    	Added CAN transmit & receive identifiers for all MicroLynx modules.
**
**    	Revision 0.2  2000/12/22 10:50:53  pxh
**    	Added extra definitions for Lynx group I/O.
**
**    	Revision 0.1  2000/12/16 11:54:00  pxh
**    	Initial revision - tested comms with MicroLynx drive.
**
**
*/


#ifndef _CAN_H_DEFINED
#define _CAN_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_CAN_MAIN
   #define E_CAN_EXTERN
   #define E_CAN_INIT(Initialisation) = Initialisation
#else
   #define E_CAN_EXTERN extern
   #define E_CAN_INIT(Initaliasation)
#endif

/* Required include files */

#include "TtlSystem.h"       /* For Status_t definition                       */


typedef struct
{
   Int32_t CanDevId;
   Int32_t nError;
   Int32_t Param1;
   Int32_t Param2;
   Int32_t Data1;
   Int32_t Data2;
} CanVal_t;

/********/

/*
** Allow two instances of CAN server - the default will be AMN to allow
** backwards compatibility.
*/

#define E_CAN_REG_SERVER_AMN      "E_TTL_AMN_CanServer"
#define E_CAN_REG_SERVER_PMN      "E_TTL_PMN_CanServer"
#define E_CAN_REG_SERVER_NAME     E_CAN_REG_SERVER_AMN
#define E_CAN_SERVER_PROC         "CanServer"
#define E_CAN_SERVER_PATH         "//1/opt/ttl/bin/CanServer"
#define E_CAN_SERVER_PRI          (10)

/*
** Definitions for reading & writing uLynx IO lines.
*/

#define E_CAN_DFLT_IO_GRP    20
#define E_CAN_IO_GRP_WIDTH   10
#define E_CAN_IOx1_MASK      0x01
#define E_CAN_IOx1_SHIFT     0
#define E_CAN_IOx2_MASK      0x02
#define E_CAN_IOx2_SHIFT     1
#define E_CAN_IOx3_MASK      0x04
#define E_CAN_IOx3_SHIFT     2
#define E_CAN_IOx4_MASK      0x08
#define E_CAN_IOx4_SHIFT     3
#define E_CAN_IOx5_MASK      0x10
#define E_CAN_IOx5_SHIFT     4
#define E_CAN_IOx6_MASK      0x20
#define E_CAN_IOx6_SHIFT     5

/*
** Definitions for accessing registers other than IO. Note that IO numbers less
** than 10 are not used, so use these numbers to identify other registers.
*/

#define E_CAN_REG_CTR1       1
#define E_CAN_REG_CTR2       2
#define E_CAN_REG_CTR3       3

/*
** Definitions for a move command, to specify absolute or relative.
*/

typedef enum eCanMoveType_e
{
   E_CAN_MOVE_ABSOLUTE = 0,
   E_CAN_MOVE_RELATIVE
} eCanMoveType_t;

typedef struct
{
   Int32_t Value;    /* Actual value of the global task data   */
   Int32_t Units;    /* The units of that value (for decoding) */
} eCanTaskData_t;

typedef enum eCanDevId_e
{
   E_CAN_BOL_DEV,

   E_CAN_ACMR_DEV,
   E_CAN_ACMY_DEV,
   E_CAN_ACMB_DEV,
   E_CAN_SMF_DEV,
   E_CAN_AGD_DEV,
   E_CAN_AGF_DEV,
   E_CAN_SFD_DEV,
   E_CAN_SFP_DEV,

   E_CAN_EOL_DEV
} eCanDevId_t;

#define E_CAN_ACMR_LYNX_TX_ID   0x7C1
#define E_CAN_ACMR_LYNX_RX_ID   0x7C0
#define E_CAN_ACMY_LYNX_TX_ID   0x7C3
#define E_CAN_ACMY_LYNX_RX_ID   0x7C2
#define E_CAN_ACMB_LYNX_TX_ID   0x7C5
#define E_CAN_ACMB_LYNX_RX_ID   0x7C4
#define E_CAN_SMF_LYNX_TX_ID    0x7D1
#define E_CAN_SMF_LYNX_RX_ID    0x7D0
#define E_CAN_AGD_LYNX_TX_ID    0x7A1
#define E_CAN_AGD_LYNX_RX_ID    0x7A0
#define E_CAN_AGF_LYNX_TX_ID    0x7A3
#define E_CAN_AGF_LYNX_RX_ID    0x7A2
#define E_CAN_SFD_LYNX_TX_ID    0x7B1
#define E_CAN_SFD_LYNX_RX_ID    0x7B0
#define E_CAN_SFP_LYNX_TX_ID    0x7B3
#define E_CAN_SFP_LYNX_RX_ID    0x7B2


enum eCanStatus
{
    E_CAN_GEN_ERROR = STATUS_START(CAN),
    E_CAN_INIT_FAIL,
    E_CAN_SHUTDOWN_ERROR,
    E_CAN_OPEN_FAIL,
    E_CAN_CLOSE_FAIL,
    E_CAN_READ_FAIL,
    E_CAN_SEND_FAIL,
    E_CAN_BAUD_FAIL,
    E_CAN_BAUDRATE_ERR,
    E_CAN_TX_ERROR,
    E_CAN_RX_ERROR,
	E_CAN_INVALID_DATA_LEN,
	E_CAN_INVALID_COMMAND,
	E_CAN_INVALID_RESPONSE,
    E_CAN_REPLY_FAIL,
    E_CAN_TIMER_SETUP_ERR,
    E_CAN_TIMER_CLOSE_ERR,
    E_CAN_INVALID_STATE,
    E_CAN_UNEXPECTED_REPLY,
    E_CAN_DEVID_OUT_OF_RANGE,
    E_CAN_DEVID_ALLOC,
    E_CAN_DEVID_NOT_ALLOC,
    E_CAN_DEVICE_NOT_FOUND,
    E_CAN_SEND_ERR,
    E_CAN_RECEIVE_ERR,
    E_CAN_IO_GROUP_REQ_ERR,
    E_CAN_IO_LINE_REQ_ERR,
    E_CAN_POLL_TYPE_ERR
};

/*
** Package OID list.
*/

typedef enum eCanDataId_e
{
   D_CAN_DATAID_BOL = OID_START(CAN),  /* Offset OIDs by package Id      */

   D_CAN_PROC_STATE,                   /* CanServer process state        */
   D_CAN_APP_VERSION,                  /* Software revision number       */
   D_CAN_SHUTDOWN,                     /* CanServer shutdown flag        */

   D_CAN_DATAID_EOL                    /* End-of-list marker             */
} eCanDataId_t;

typedef enum
{
   E_CAN_STATE_DISABLED = STATUS_START(CAN),
   E_CAN_STATE_HOMING,
   E_CAN_STATE_READY,
   E_CAN_STATE_MOVING,
   E_CAN_STATE_ERROR
} eCanStates_t;

/*
** External function prototypes
*/

#endif

/*
** EOF
*/
