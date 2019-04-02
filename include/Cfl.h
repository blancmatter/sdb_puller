/* 
**
**  Module Name : 
**    Cfl.h
**
**  Purpose :
**    Public header file for CFL package.
**
**  Description :
**    Used to declare exported data, functions and definitions for the
**    CFL package.
**
**  Authors :
**    MKV : Mark Vernon (TTL)
**
**  Version :
**    $Id: Cfl.h,v 0.6 2001/06/18 08:09:07 mjf Exp $
**
**  History: 
**    $Log: Cfl.h,v $
**    Revision 0.6  2001/06/18 08:09:07  mjf
**    Change of use of AET to be AES (Absolute Encoder Server).
**
**    Revision 0.5  2000/12/13 16:48:32  mkv
**    Updated for baseline release.
**
**    Revision 0.3  2000/11/23 11:22:49  mkv
**    Additional Functionality.
**
**    Revision 0.2  2000/11/20 14:11:57  mkv
**    Updated bit mask Oid table.
**
**    Revision 0.1  2000/11/13 16:42:48  mkv
**    Initial Revision.
**
**
*/


#ifndef _CFL_H_DEFINED
#define _CFL_H_DEFINED

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef M_CFL_MAIN
   #define E_CFL_EXTERN
   #define E_CFL_INIT(Initialisation) = Initialisation
#else
   #define E_CFL_EXTERN extern
   #define E_CFL_INIT(Initaliasation)
#endif

/* Required include files */

#include <limits.h>
#include "TtlSystem.h"                            /* For Status_t definition */
#include "Mif.h"
#include "Ven.h"
#include "Aes.h"
#include "Tfp.h"

#define E_CFL_CMD_DATA_LEN    32

#define E_CFL_REG_SERVER_NAME "E_TTL_AZC_CflServer"
#define E_CFL_SERVER_PROC     "CflServer"
#define E_CFL_SERVER_PATH     "//1/opt/ttl/bin/"
#define E_CFL_SERVER_PRI      10


/*
** Controller Fast Logger Commands
*/
typedef enum eCflCommand_e
{
   I_CFL_COMMAND_BOL                   = 0,   /* Unused - Begining Of List */
   I_CFL_MIF_AXIS_TORQUE_DEMAND        = 1,  
   I_CFL_MIF_FILTERED_MOTOR_1_DAC_BITS = 2,
   I_CFL_MIF_FILTERED_MOTOR_2_DAC_BITS = 3,
   I_CFL_VEN_MOTOR_ENC_1_COUNT         = 4, 
   I_CFL_VEN_MOTOR_ENC_2_COUNT         = 5, 
   I_CFL_VEN_AXIS_ENC_1_COUNT          = 6,
   I_CFL_VEN_AXIS_ENC_2_COUNT          = 7,
   I_CFL_VEN_AXIS_ENC_3_COUNT          = 8,
   I_CFL_VEN_AXIS_ENC_4_COUNT          = 9,
   I_CFL_VEN_AXIS_ENC_1_ABS_POSITION   = 10,
   I_CFL_VEN_AXIS_ENC_2_ABS_POSITION   = 11,
   I_CFL_VEN_AXIS_ENC_3_ABS_POSITION   = 12,
   I_CFL_VEN_AXIS_ENC_4_ABS_POSITION   = 13,
   I_CFL_AES_POSITION                  = 14,
   I_CFL_TFP_CURRENT_TIME              = 15,
   I_CFL_COMMAND_EOL                   = 31,   /* Unused - End Of List */
   I_CFL_COMMAND_MAX                   = 32,
   I_CFL_COMMAND_EMPTY                 = -1
} eCflCommand_t;

/*
** The following default define is a bit representation
** of eCflCommand_t entries required to be logged. 
*/
#define I_CFL_DEF_CMD_STATE            0x0000FFFE

/*
** Fast Log Default timer period
*/
#define I_CFL_TIMER_PERIOD             50000                 /* microseconds */

typedef struct
{
    eCflCommand_t  Command;
    Status_t       Status;
    Int32_t        DataLen;
} eCflHeader_t;

typedef struct
{
    eCflHeader_t  Header;
    char          Data[E_CFL_CMD_DATA_LEN];
} eCflMessage_t;

#define E_CFL_MIN_CMD_LEN    sizeof(eCflHeader_t)
#define E_CFL_MAX_CMD_LEN    sizeof(eCflMessage_t)

typedef struct
{
   Int32_t Value;                  /* Actual value of the global task data   */
   Int32_t Units;                  /* The units of that value (for decoding) */
} eCflTaskData_t;

typedef enum eCflStatus_e                  
{
    E_CFL_GEN_ERROR = STATUS_START(CFL),
    E_CFL_INIT_FAIL,
    E_CFL_SHUTDOWN_ERROR,
    E_CFL_INVALID_DATA_LEN,
    E_CFL_INVALID_COMMAND,
    E_CFL_INVALID_PARAMETER,
    E_CFL_REPLY_FAIL,
    E_CFL_SERVER_STARTUP_FAIL,
    E_CFL_SERVER_SHUTDOWN_FAIL,
    E_CFL_SERVER_NOT_RUNNING,
    E_CFL_SERVER_SEND_FAIL,
    E_CFL_TIMER_ATTACH_ERR,
    E_CFL_TIMER_CREATE_ERR,
    E_CFL_TIMER_UNDERFLOW_ERR,
    E_CFL_TIMER_OVERFLOW_ERR,
    E_CFL_TIMER_CONFIG_ERR,
    E_CFL_TIMER_CLOSE_ERR,
    E_CFL_SDB_UPDATE_FAIL,
    E_CFL_CMD_TABLE_FULL,
    E_CFL_OID_NOT_IN_TABLE,
    E_CFL_NUM_OUTSIDE_TABLE,
    E_CFL_ALREADY_FAST_LOGGING,
    E_CFL_OID_ALREADY_IN_TABLE,
    E_CFL_DEAD_ENDCLIENT,

    E_CFL_UNINITIALISED,                /* Used to update the Oid Log State */
    E_CFL_INITIALISING,
    E_CFL_WAITING_TO_FAST_LOG,
    E_CFL_FAST_LOGGING,
    E_CFL_BUFFER_FILLED,                /* Local status - dont update Sdb   */
    E_CFL_UPDATING_SDB,                 /* Local status - dont update Sdb   */
    E_CFL_SHUT_DOWN,
    E_CFL_STATUS_EOL,
    E_CFL_STATUS_MAX_VALUE = INT_MAX
} eCflStatus_t;

/*
** Package OID list.
*/

typedef enum eCflDataId_e
{
    D_CFL_DATAID_BOL = OID_START(CFL),  /* Offset OIDs by package Id         */
    D_CFL_PROC_STATE,                   /* CflServer process state           */
    D_CFL_OPERATING_STATE,              /* CflServer Operating State         */
    D_CFL_SERVER_SHUTDOWN,              /* CflServer process state           */
    D_CFL_LOG_TRIGGER,                  /* CflServer */
    D_CFL_UPDATE_PERIOD,                /* CflServer update value rate       */
    D_CFL_ADD_CUSTOM_OID,  
    D_CFL_REM_CUSTOM_OID,
    D_CFL_SET_FASTLOG_ON,
    D_CFL_SET_FASTLOG_OFF,
    D_CFL_GET_FASTLOG_BY_OID,
    D_CFL_GET_FASTLOG_BY_NUM,
    D_CFL_REINIT_OID_TABLE,
    D_CFL_CMD_TABLE_NUM_00,
    D_CFL_CMD_TABLE_NUM_01,
    D_CFL_CMD_TABLE_NUM_02,
    D_CFL_CMD_TABLE_NUM_03,
    D_CFL_CMD_TABLE_NUM_04,
    D_CFL_CMD_TABLE_NUM_05,
    D_CFL_CMD_TABLE_NUM_06,
    D_CFL_CMD_TABLE_NUM_07,
    D_CFL_CMD_TABLE_NUM_08,
    D_CFL_CMD_TABLE_NUM_09,
    D_CFL_CMD_TABLE_NUM_10,
    D_CFL_CMD_TABLE_NUM_11,
    D_CFL_CMD_TABLE_NUM_12,
    D_CFL_CMD_TABLE_NUM_13,
    D_CFL_CMD_TABLE_NUM_14,
    D_CFL_CMD_TABLE_NUM_15,
    D_CFL_CMD_TABLE_NUM_16,
    D_CFL_CMD_TABLE_NUM_17,
    D_CFL_CMD_TABLE_NUM_18,
    D_CFL_CMD_TABLE_NUM_19,
    D_CFL_CMD_TABLE_NUM_20,
    D_CFL_CMD_TABLE_NUM_21,
    D_CFL_CMD_TABLE_NUM_22,
    D_CFL_CMD_TABLE_NUM_23,
    D_CFL_CMD_TABLE_NUM_24,
    D_CFL_CMD_TABLE_NUM_25,
    D_CFL_CMD_TABLE_NUM_26,
    D_CFL_CMD_TABLE_NUM_27,
    D_CFL_CMD_TABLE_NUM_28,
    D_CFL_CMD_TABLE_NUM_29,
    D_CFL_CMD_TABLE_NUM_30,
    D_CFL_CMD_TABLE_NUM_31,
    D_CFL_DATAID_EOL                    /* End-of-list marker                */
} eCflDataId_t;

/*
** External function prototypes
*/

#endif

/*
** EOF
*/
