/*******************************************************************************
** Module Name:
**    Ept.h
**
** Purpose:
**    Public header file for Enclosure PLC Task.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    EPT design and implementation document v0.03 28-Sep-2000.
**
** Author(s):
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000, 2001
**
** Version:
**    $Id: Ept.h,v 0.47 2007/07/12 13:56:14 mjf Exp $
**
** History:
**    $Log: Ept.h,v $
**    Revision 0.47  2007/07/12 13:56:14  mjf
**    Conform to revised status data in v0.08 of ICD.
**
**    Revision 0.46  2006/12/14 15:53:47  mjf
**    Comments improved to aid logging clarity.
**
**    Revision 0.45  2002/01/09 15:20:37  mjf
**    Change to names and states for PLC low battery and processor fault.
**
**    Revision 0.44  2002/01/07 11:23:33  mjf
**    Use the telescope definitions in TtlSystem.h.
**
**    Revision 0.43  2002/01/03 10:08:06  mjf
**    Correction to comment.
**
**    Revision 0.42  2001/12/20 09:42:37  mjf
**    Change of 'ACC_CHRG' to 'ACC' datum ID - indicates the accumulator state.
**
**    Revision 0.41  2001/12/20 09:30:30  mjf
**    Addition of 'charged' and 'discharged' data states.
**
**    Revision 0.40  2001/12/19 10:12:30  mjf
**    Moving of definitions from the public to the private header, and the
**    names changed to reflect the correct scope.
**
**    Revision 0.39  2001/12/17 15:22:56  mjf
**    Addition of THIS_APPLICATION datum ID, to determine if the EPT or IET
**    is the one running.
**    Also addition of datum IDs for generic telescope, to allow unambiguous
**    determination of data - e.g. a shutter may be all of closed, halted,
**    and fault.
**
**    Revision 0.38  2001/12/17 09:29:31  mjf
**    Fixing of a few incorrect units in the OID table, and implementation of
**    correct conversion functions for analogue data.
**
**    Revision 0.37  2001/12/14 15:13:19  mjf
**    Changes added for Faulkes telescopes. Data common to Liverpooll and
**    Faulkes is tagged 'generic'.
**
**    Revision 0.36  2001/12/14 10:00:31  mjf
**    Clarification of IUCAA positions as analogue values. Also ShutterState
**    enumeration changed to to EptDataState, as it contains more than just
**    states for shutters.
**
**    Revision 0.35  2001/12/13 15:12:09  mjf
**    Completion of correction units for OIDs.
**
**    Revision 0.34  2001/12/13 10:59:11  mjf
**    Change of OID table format to have a 'default' and 'error' value for
**    each OID. The 'default' will be used when no bits are set in the PLC,
**    and the 'error' will be used on failing to read data from the PLC.
**
**    Revision 0.33  2001/12/12 12:08:12  mjf
**    Addition of NULL state - initial use when accumulators are neither
**    charging nor discharging.
**
**    Revision 0.32  2001/12/12 08:51:16  mjf
**    Changes driven by Liverpool MCS/Enclosure ICD v2.5.
**
**    Revision 0.31  2001/12/11 11:34:17  mjf
**    Change of state priority - STANDBY replaced with SUSPEND.
**
**    Revision 0.30  2001/12/11 09:01:55  mjf
**    Addition of definitions of telescopes and telescope combinations.
**
**    Revision 0.29  2001/07/04 14:45:21  ljs
**    Added and OID for the serial message codes.
**
**    Revision 0.28  2001/06/22 13:18:29  ljs
**    Added new error code for immediate shutdown condition.
**
**    Revision 0.27  2001/06/14 10:36:56  ljs
**    Added new error code, and fault data to DataId list.
**
**    Revision 0.26  2001/06/12 15:14:24  ljs
**    Added list of process state priorities for use in the
**    process state model.
**
**    Revision 0.25  2001/06/06 10:49:12  ljs
**    Added PLC_INVALID error code.
**
**    Revision 0.24  2001/05/24 12:20:08  ljs
**    Added new error codes.
**
**    Revision 0.23  2001/05/22 12:44:49  ljs
**    Added new error for failure to read config file.
**    Added new OID for THIS_TELESCOPE so that the telescope
**    can be set from the config file.
**
**    Revision 0.22  2001/05/18 15:47:37  ljs
**    Added HALTED state and initialised DMD_HALT to FALSE
**    and DMD_STOP to TRUE.
**
**    Revision 0.21  2001/05/18 13:56:40  ljs
**    Changed initial states of demand OIDs to DMD_INVALID_STATE.
**
**    Revision 0.20  2001/05/15 15:26:54  ljs
**    Removed GET and STOP services. Added Changed field to
**    Oid Table. Added new IUCAA only data..
**
**    Revision 0.19  2001/05/15 11:45:15  ljs
**    Added 'Changed' field to OID table. Changed default of BOL
**    and EOL DataIt to match the 'Error' field in the OID table.
**
**    Revision 0.18  2001/05/11 11:02:45  ljs
**    Added conversion functions.
**
**    Revision 0.17  2001/05/10 17:33:16  ljs
**    Includes first version of new OID table.
**
**    Revision 0.16  2001/05/04 10:15:19  ljs
**    Added DMD_INVALID state as in too many places EPT_STATES were
**    being used to set demand values and the demands needed
**    a default value.
**
**    Revision 0.15  2001/04/20 20:04:38  asp
**    New PLC data bits. Also some new states.
**
**    Revision 0.14  2001/02/19 11:29:56  mjf
**    Removal of conditional compilation for Liverpool.
**
**    Revision 0.13  2001/02/16 14:56:47  mjf
**    Addition of GET_1 service.
**
**    Revision 0.12  2001/02/05 14:30:28  asp
**    Common EPT header for IUCCA & Liverpool
**
**
*******************************************************************************/

#ifndef EPT_H_DEFINED
#define EPT_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/

#include "TtlSystem.h"
#include "Mcp.h"
#include "Sdb.h"


/*
** Local include files
*/
    

/*
** Macro definitions
*/

/* Size of messages (based on CIL classes) sent when commands received */
#define E_EPT_ACT_LEN_DMD    0
#define E_EPT_RSP_LEN_DMD    0
#define E_EPT_COM_LEN_DMD    sizeof( Int32_t )

/* Number of times to retry interface in event of coms failure. */
#define E_EPT_COMS_RETRIES   5



/*
** Input values are set to this value to indicate un-initialised or error
** It is also used to mark the end of automatically allocated arrays.
*/
#define E_EPT_BAD_VALUE   -1L


/*
** Type definitions
*/

/* Enumerated list of status and error values */

typedef enum eEptStatus_e
{
   E_EPT_GEN_ERR = STATUS_START(EPT),  /* Miscellaneous error */
   E_EPT_ERR_SETUP,          /* 1 Error performing setup */
   E_EPT_ERR_CIL_RX,         /* 2 Unexpected CIL message received */
   E_EPT_ERR_SDB_SUBMIT,     /* 3 Error performing SDB submission */
   E_EPT_SW_SIMULATE,        /* 4 Using software to simulate encl. operation */
   E_EPT_UNKNOWN_SERVICE,    /* 5 Requested service not provided by this task */
   E_EPT_SERVICE_NOT_IMPL,   /* 6 Service not implemented in this version */
   E_EPT_NOT_IMPLEMENTED,    /* 7 General functionality not implemented */
   E_EPT_MSG_LEN_ERR,        /* 8 Incorrect no. bytes in CIL data block */
   E_EPT_UNEXPECTED_SIM,     /* 9 Simulate fn. called without simulate selected */
   E_EPT_UNEXPECTED_MSG,     /* a Unexpected CIL message received */
   E_EPT_INVALID_SHUTTER,    /* b Shutter ID number not recognised */
   E_EPT_INVALID_DATUM,      /* c Datum ID out of range */
   E_EPT_INVALID_COMMAND,    /* d Command not recognised */
   E_EPT_PARSE_ERROR,        /* e Failed to read an ASCII argument token */
   E_EPT_PARAM_RANGE_ERR,    /* f Specified parameter outside allowed range */
   E_EPT_NO_HEARTBEATS,      /* 10 No heartbeats received recently */
   E_EPT_CMD_NOT_PERMITTED,  /* 11 Command not permitted because state not OKAY */
   E_EPT_UNKNOWN_DEMAND,     /* 12 Demanded parameter not known */
   E_EPT_PARAM_SIZE_ERR,     /* 13 A value to be set is not of a supported size */
   E_EPT_NO_TIMED_CMDS,      /* 14 Timed set commands are not supported */
   E_EPT_UNKNOWN_OID,        /* 15 Object ID not recognised */
   E_EPT_OID_READ_ONLY,      /* 16 Attempt made to change a read-only OID value */
   E_EPT_ERR_READ_PKTS,      /* 17 Unable to read no. expected packets to follow */
   E_EPT_ERR_READ_LENGTH,    /* 18 Failed to read the expected length of packet */
   E_EPT_INVALID_LENGTH,     /* 19 Incorrect length of received packet */
   E_EPT_CHKSUM_ERROR,       /* 1a Calculated and reported checksums don't match */
   E_EPT_SER_REPORTED_ERROR, /* 1b Error response received from serial device.*/
   E_EPT_SER_UNEXPECTED_RSP, /* 1c Response received not for command sent.*/

   E_EPT_DMD_PENDING,        /* 1d An operation is already pending */
   E_EPT_WRITE_FAILED,       /* 1e Write to PLC failed */
   E_EPT_READ_FAILED,        /* 1f Read to PLC failed */
   E_EPT_PLC_INVALID,        /* More than one PLC bit is set for the same OID */

   E_EPT_CONFIG_FILE_ERROR,  /* 20 Format of the config file is wrong. */
   E_EPT_STATE_UNKNOWN,      /* Value not in range for this OID. */

   E_EPT_STATUS_EOL          /* 21End of status list marker (do not use) */
} eEptStatus_t;

/* EPT messages - services offered by the EPT */

typedef enum eEptService_e
{
   E_EPT_HEARTBEAT = E_MCP_HEARTBEAT,  /* Heartbeat command */
   E_EPT_SHUTDOWN  = E_MCP_SHUTDOWN,   /* Shutdown command */
   E_EPT_SAFESTATE = E_MCP_SAFESTATE,  /* Safestate command */
   E_EPT_ACTIVATE  = E_MCP_ACTIVATE,   /* Activate command */

   E_EPT_SET_1     = E_MCP_SET_1           /* Set a single control param */

} eEptService_t;

/*
** These are the states that EPT datum can be set to (excluding analogue)
** Bit values read from the PLC are converted to one of these states 
*/

typedef enum eEptDataState_e
{
   /*
   ** State used to mark analogue data
   */
   E_EPT_ANALOGUE_STATE, /* Analogue, actual value will be submitted to Sdb */ 

   /*
   ** Improper conditions where a state cannot be determined
   */
   E_EPT_INVALID_STATE, /* More than 1 state bit was found to be set */
   E_EPT_UNKNOWN_STATE, /* No PLC state bits were found to be set */
   E_EPT_FAULT_STATE,   /* A fault was detected for this mechanism */

   /*
   ** Proper states - as defined by ICD
   */
   E_EPT_OPENING_STATE, /* Opening */
   E_EPT_CLOSING_STATE, /* Closing */
   E_EPT_MOVING_STATE,  /* Moving  */
   E_EPT_STOPPED_STATE, /* Stopped */
   E_EPT_HALTED_STATE,  /* Halted */
   E_EPT_OPEN_STATE   , /* Open    */
   E_EPT_CLOSED_STATE , /* Closed  */

   /*
   ** Proper states - additional states for other datum
   */ 
   E_EPT_ACTD_STATE,    /* Activated  */
   E_EPT_AV_STATE,      /* Available */
   E_EPT_BAT_STATE,     /* Battery  */
   E_EPT_BLKD_STATE,    /* Blocked */
   E_EPT_CBT_STATE,     /* Circuit breaker tripped */
   E_EPT_CHRG_STATE,    /* Charging */
   E_EPT_CHRGD_STATE,   /* Charged */
   E_EPT_CLR_STATE,     /* Clear */
   E_EPT_DCHRG_STATE,   /* Discharging */
   E_EPT_DCHRGD_STATE,  /* Discharged */
   E_EPT_FLT_STATE,     /* Fault */
   E_EPT_HH_STATE ,     /* High High */
   E_EPT_HIGH_STATE,    /* High */
   E_EPT_HLY_STATE,     /* Healthy */
   E_EPT_HSW_STATE,     /* High Switch */
   E_EPT_LCKD_STATE,    /* Locked */
   E_EPT_LL_STATE ,     /* Low Low */
   E_EPT_LOC_STATE,     /* Local */
   E_EPT_LOW_STATE,     /* Low */
   E_EPT_MA_STATE ,     /* Mis-aligned */
   E_EPT_NAV_STATE,     /* Not Available */
   E_EPT_NOK_STATE,     /* Not OK */
   E_EPT_NORM_STATE,    /* Normal */
   E_EPT_NULL_STATE,    /* Null */
   E_EPT_OFF_STATE,     /* Off */
   E_EPT_OK_STATE ,     /* OK */
   E_EPT_OLT_STATE,     /* Over-load tripped */
   E_EPT_ON_STATE ,     /* On */
   E_EPT_OPRTD_STATE,   /* Operated */
   E_EPT_PROG_STATE,    /* PLC in program mode */
   E_EPT_REMPROG_STATE, /* PLC in remote program mode */
   E_EPT_REMRUN_STATE,  /* PLC in remote run mode */
   E_EPT_RUN_STATE,     /* PLC in running mode */
   E_EPT_REM_STATE,     /* Remote */
   E_EPT_RNG_STATE,     /* Running */
   E_EPT_SAFE_STATE,    /* Safe-state */
   E_EPT_ULKD_STATE,    /* Unlocked */

   /*
   ** Spare states - defined for some datum but should never be set
   */
   E_EPT_SPR_STATE ,    /* Spare state 0 */
   E_EPT_TIMEOUT_STATE, /* Timeout state 1 */
   E_EPT_SPR2_STATE,    /* Spare state 2 */

   E_EPT_STATE_EOL      /* End of list */

} eEptDataState_t;

/* 
** These are the enumerated values that commands can be set to
*/

typedef enum eEptDmdState_e
{
   E_EPT_DMD_DISABLE_STATE = 0,               /* Generic disable      */ 
   E_EPT_DMD_ENABLE_STATE  = 1,               /* Generic enable       */
   E_EPT_DMD_THIRD_STATE   = 2,               /* Generic third state  */
   E_EPT_DMD_FOURTH_STATE  = 3,               /* Generic fourth state */ 
   E_EPT_DMD_INVALID_STATE = 4,               /* Generic fourth state */ 

   E_EPT_DMD_ACTD_STATE    = E_EPT_DMD_ENABLE_STATE,  /* Activated  */
   E_EPT_DMD_SAFE_STATE    = E_EPT_DMD_DISABLE_STATE, /* Safe state */

   E_EPT_DMD_UNLOCK_STATE  = E_EPT_DMD_ENABLE_STATE,  /* Unlocked */
   E_EPT_DMD_LOCK_STATE    = E_EPT_DMD_DISABLE_STATE, /* Locked */

   E_EPT_DMD_OPEN_STATE    = E_EPT_DMD_ENABLE_STATE,  /* Open */
   E_EPT_DMD_CLOSE_STATE   = E_EPT_DMD_DISABLE_STATE, /* Close */
   E_EPT_DMD_STOP_STATE    = E_EPT_DMD_THIRD_STATE,   /* Stop */
   E_EPT_DMD_POS_STATE     = E_EPT_DMD_FOURTH_STATE,  /* Move to position */

   E_EPT_DMD_RESET_STATE   = E_EPT_DMD_ENABLE_STATE   /* Reset */
} eEptDmdState_t;

/* Global data for submission to the Status Database (SDB) */

typedef enum eEptDataId_e
{
   D_EPT_DATAID_BOL = 0,     /* Begining of data list */

   /*
   ** Process specific data submitted to SDB
   */
   D_EPT_PROC_STATE  = D_MCP_PROC_STATE,       /* Process state  */
   D_EPT_AUTH_STATE  = D_MCP_AUTH_STATE,       /* Authorisation state */
   D_EPT_SYS_REQUEST = D_MCP_SYS_REQUEST,      /* System requests made to MCP*/
   D_EPT_APP_VERSION = D_MCP_APP_VERSION,      /* Application version number */

   D_EPT_THIS_APPLICATION = D_MCP_FIRST_USER_DATUM, /* Application package ID */
   D_EPT_THIS_TELESCOPE  ,                     /* Identity of this telescope */
   D_EPT_SW_SIMULATE     ,                     /* Software simulate mode */

   /* Data values retrieved from PLC and submitted to SDB */
   D_EPT_AP_STATE        , /* Aperture Mechanism State */
   D_EPT_AP_HALTED       , /* Aperture Halted */
   D_EPT_AP_FAULT        , /* Aperture Fault */
   D_EPT_AP_UNAVAIL      , /* Aperture Not Available */
   D_EPT_S1_STATE        , /* Shutter 1 Mechanism State */
   D_EPT_S1_HALTED       , /* Shutter 1 Halted */
   D_EPT_S1_FAULT        , /* Shutter 1 Fault */
   D_EPT_S1_LAG_ERROR    , /* Shutter 1 Lag Error */
   D_EPT_S2_STATE        , /* Shutter 2 Mechanism State */
   D_EPT_S2_HALTED       , /* Shutter 2 Halted */
   D_EPT_S2_FAULT        , /* Shutter 2 Fault */
   D_EPT_S2_LAG_ERROR    , /* Shutter 2 Lag Error */
   D_EPT_S1_POS          , /* Shutter 1 Actual Position */
   D_EPT_S2_POS          , /* Shutter 2 Actual Position */
   D_EPT_S1_CL_POS       , /* Shutter 1 Closed Limit (decimal value) */
   D_EPT_S2_CL_POS       , /* Shutter 2 Closed Limit (decimal value) */
   D_EPT_S1_OP_POS       , /* Shutter 1 Open Limit (decimal value) */
   D_EPT_S2_OP_POS       , /* Shutter 2 Open Limit (decimal value) */

   D_EPT_TOP_LCK         , /* Top Locks State */
   D_EPT_TOP_LCK_FAULT   , /* Top Locks Fault */
   D_EPT_TOP_LCK_SPR     , /* Top Locks Spare */
   D_EPT_S1_LCK          , /* Shutter 1 Locks State */
   D_EPT_S1_LCK_FAULT    , /* Shutter 1 Locks Fault */
   D_EPT_S1_LCK_SPR      , /* Shutter 1 Locks Spare */
   D_EPT_S2_LCK          , /* Shutter 2 Locks State */
   D_EPT_S2_LCK_FAULT    , /* Shutter 2 Locks Fault */
   D_EPT_S2_LCK_SPR      , /* Shutter 2 Locks Spare */
   D_EPT_S1_R1           , /* Shutter 1 Ram 1 Misalignment */
   D_EPT_S1_R2           , /* Shutter 1 Ram 2 Misalignment */
   D_EPT_S2_R3           , /* Shutter 2 Ram 3 Misalignment */
   D_EPT_S2_R4           , /* Shutter 2 Ram 4 Misalignment */
   D_EPT_HYD_PMP         , /* Hydraulic Pump State */
   D_EPT_HYD_PMP_FAULT   , /* Hydraulic Pump Fault */
   D_EPT_HYD_PMP_SPR     , /* Hydraulic Pump Spare */
   D_EPT_HYD_PMP_OLT     , /* Hydraulic Pump O/L Tripped */
   D_EPT_HYD_PMP_CBT     , /* Hydraulic Pump MCB Tripped */
   D_EPT_HTK_LVL         , /* Hydraulic Tank Level State */
   D_EPT_HTK_LVL_FAULT   , /* Hydraulic Tank Level Fault */
   D_EPT_HTK_LVL_SPR     , /* Hydraulic Tank Level Spare */
   D_EPT_HTK_TEMP        , /* Hydraulic Tank Temperature State */
   D_EPT_HTK_TEMP_FAULT  , /* Hydraulic Tank Temperature Fault */
   D_EPT_HTK_TEMP_SPR    , /* Hydraulic Tank Temperature Spare */
   D_EPT_MCP_ALARM       , /* MCP Alarm Active */
   D_EPT_MCP_ENGAGED     , /* MCP Engaged */
   D_EPT_WD_BYPASSED     , /* Watchdog Key Bypassed */
   D_EPT_ENC_SH1_1_AA    , /* Encoder Card Fault SH1-1 */
   D_EPT_ENC_SH1_2_AA    , /* Encoder Card Fault SH1-2 */
   D_EPT_ENC_SH2_1_AA    , /* Encoder Card Fault SH2-1 */
   D_EPT_ENC_SH2_2_AA    , /* Encoder Card Fault SH2-2 */
   D_EPT_OVL_CLOSED      , /* Any Closed Limit Switch Active */
   D_EPT_OVL_OPEN        , /* Any Open Limit Switch Active */
   D_EPT_FLT1            , /* Filter 1 Pressure Switch State */        
   D_EPT_FLT1_FAULT      , /* Filter 1 Pressure Switch Fault */        
   D_EPT_FLT1_SPR        , /* Filter 1 Pressure Switch Spare */
   D_EPT_FLT2            , /* Filter 2 Pressure Switch State */        
   D_EPT_FLT2_FAULT      , /* Filter 2 Pressure Switch Fault */        
   D_EPT_FLT2_SPR        , /* Filter 2 Pressure Switch Spare */
   D_EPT_R1_UNLK_PRES_FLT, /* Ram 1 Unlock Pressure Failure */
   D_EPT_R2_UNLK_PRES_FLT, /* Ram 2 Unlock Pressure Failure */
   D_EPT_R3_UNLK_PRES_FLT, /* Ram 3 Unlock Pressure Failure */
   D_EPT_R4_UNLK_PRES_FLT, /* Ram 4 Unlock Pressure Failure */
   D_EPT_MOVEMENT_TIMEOUT, /* Movement Timeout */
   D_EPT_MCBS            , /* Miniature Circuit Breakers State */
   D_EPT_ILOCK_STOP      , /* Stop Interlock Asserted */
   D_EPT_ILOCK_CLOSE     , /* Close Interlock Asserted */
   D_EPT_ESTOP           , /* Enclosure Emergency Stop  */
   D_EPT_MPCBS           , /* Motor Protection Circuit Breakers */
   D_EPT_KEYSW           , /* Local/Remote Selector Switch  */
   D_EPT_EPLC            , /* Enclosure PLC State */
   D_EPT_PLC_WARN_STATE  , /* PLC in WARN State */
   D_EPT_PLC_S2_FLT_STATE,/* PLC in stage 2 fault State */
   D_EPT_PLC_S3_FLT_STATE,/* PLC in stage 3 fault State */
   D_EPT_PLC_S4_FLT_STATE,/* PLC in stage 4 fault State */
   D_EPT_PLC_WDOG        , /* Enclosure PLC Watchdog Healthy */
   D_EPT_MAIN_PWR        , /* Enclosure Main Power Healthy */
   D_EPT_R1_SLP          , /* Ram 1 Seal Pressure Switch State */
   D_EPT_R1_SLP_FAULT    , /* Ram 1 Seal Pressure Switch Fault */
   D_EPT_R1_SLP_UNAVAIL  , /* Ram 1 Seal Pressure Switch Not Available */
   D_EPT_R2_SLP          , /* Ram 2 Seal Pressure Switch State */
   D_EPT_R2_SLP_FAULT    , /* Ram 2 Seal Pressure Switch Fault */
   D_EPT_R2_SLP_UNAVAIL  , /* Ram 2 Seal Pressure Switch Not Available */
   D_EPT_R3_SLP          , /* Ram 3 Seal Pressure Switch State */
   D_EPT_R3_SLP_FAULT    , /* Ram 3 Seal Pressure Switch Fault */
   D_EPT_R3_SLP_UNAVAIL  , /* Ram 3 Seal Pressure Switch Not Available */
   D_EPT_R4_SLP          , /* Ram 4 Seal Pressure Switch State */
   D_EPT_R4_SLP_FAULT    , /* Ram 4 Seal Pressure Switch Fault */
   D_EPT_R4_SLP_UNAVAIL  , /* Ram 4 Seal Pressure Switch Not Available */
   D_EPT_QTY_PMP_STRTS   , /* Number of Hydraulic Pump Starts in the Hour */
   D_EPT_QTY_PMP_HOURS   , /* Number of Hydraulic Pump Run Hours */
   D_EPT_RAM1_PRES       , /* Ram No 1 Pressure (analogue) */
   D_EPT_RAM2_PRES       , /* Ram No 2 Pressure (analogue) */
   D_EPT_RAM3_PRES       , /* Ram No 3 Pressure (analogue) */
   D_EPT_RAM4_PRES       , /* Ram No 4 Pressure (analogue) */
   D_EPT_HTK_ACT_TEMP    , /* Hydraulic Tank Temperature (analogue) */
   D_EPT_R1_PRES_TL      , /* Ram 1 Pressure Trip Level State */ 
   D_EPT_R1_PRES_TL_FAIL , /* Ram 1 Pressure Trip Level Sensor Failed */
   D_EPT_R2_PRES_TL      , /* Ram 2 Pressure Trip Level State */ 
   D_EPT_R2_PRES_TL_FAIL , /* Ram 2 Pressure Trip Level Sensor Failed */
   D_EPT_R3_PRES_TL      , /* Ram 3 Pressure Trip Level State */ 
   D_EPT_R3_PRES_TL_FAIL , /* Ram 3 Pressure Trip Level Sensor Failed */
   D_EPT_R4_PRES_TL      , /* Ram 4 Pressure Trip Level State */ 
   D_EPT_R4_PRES_TL_FAIL , /* Ram 4 Pressure Trip Level Sensor Failed */
   D_EPT_PANEL_TEMP     , /* Enclosure Panel Temperature */
   D_EPT_MAIN_HYD_PRES  , /* Main Hydraulic Pressure */
   D_EPT_S1_R1_ENC      , /* Shutter 1 Ram 1 Encoder Reading */
   D_EPT_S1_R2_ENC      , /* Shutter 1 Ram 2 Encoder Reading */
   D_EPT_S2_R3_ENC      , /* Shutter 2 Ram 3 Encoder Reading */
   D_EPT_S2_R4_ENC      , /* Shutter 2 Ram 4 Encoder Reading */
   D_EPT_S1_ENC_MIS     , /* Shutter 1 Encoder Misalignment Value */
   D_EPT_S2_ENC_MIS     , /* Shutter 2 Encoder Misalignment Value */
   D_EPT_PLC_LOW_BAT    , /* PLC Low Battery Alarm */
   D_EPT_PLC_PROC_FAULT , /* PLC Processor Fault Alarm */
   D_EPT_PLC_MODE       , /* PLC in Remote Program Mode */
   D_EPT_PROG_VERS_MANT , /* Program Version Number (Major Part) */
   D_EPT_PROG_VERS_REM  , /* Program Version Number (Minor Part) */

   D_EPT_DC_PMP         , /* Backup DC Pump State */
   D_EPT_DC_PMP_FAULT   , /* Backup DC Pump Fault */
   D_EPT_DC_PMP_PRES    , /* Backup DC Pump Low Pressure */
   D_EPT_DC_PMP_OLT     , /* Backup DC Pump O/L Tripped */
   D_EPT_DC_PMP_CBT     , /* Backup DC Pump MCB Tripped */
   D_EPT_FLT_PMP        , /* Off-Line Filter Pump State */
   D_EPT_FLT_PMP_FAULT  , /* Off-Line Filter Pump Fault */
   D_EPT_FLT_PMP_SPR    , /* Off-Line Filter Pump Spare */
   D_EPT_FLT_PMP_OLT    , /* Off-Line Filter Pump O/L Tripped */
   D_EPT_FLT_PMP_CBT    , /* Off-Line Filter Pump MCB Tripped */
   D_EPT_DC_PMP_UPS     , /* DC Pump UPS */
   D_EPT_PLC_UPS        , /* PLC UPS */
   D_EPT_OHTG_1         , /* Oil Tank Heater 1 State */
   D_EPT_OHTG_1_FAULT   , /* Oil Tank Heater 1 Fault */
   D_EPT_OHTG_1_CBT     , /* Oil Tank Heater 1 Overheating Switch Tripped */
   D_EPT_OHTG_2         , /* Oil Tank Heater 2 State */
   D_EPT_OHTG_2_FAULT   , /* Oil Tank Heater 2 Fault */
   D_EPT_OHTG_2_CBT     , /* Oil Tank Heater 2 Overheating Switch Tripped */
   D_EPT_PSU            , /* Power Supply Units */
   D_EPT_QTY_DC_PMP_HR  , /* Backup DC Pump Run Hours */
   D_EPT_QTY_FLT_PMP_HR , /* Off-Line Filter Pump Run Hours */

   /* Dome-only data */
   D_EPT_DOME_STATE     , /* Overall dome state */
   D_EPT_AZM_POS        , /* Dome azimuth angle (mas) */
   D_EPT_ALT_POS        , /* Dome altitude angle (mas) */
   D_EPT_AZM_LOW_POS    , /* Minimum dome azimuth angle */
   D_EPT_AZM_HIGH_POS   , /* Maximum dome azimuth angle */
   D_EPT_ALT_LOW_POS    , /* Minimum dome altitude angle */
   D_EPT_ALT_HIGH_POS   , /* Maximum dome altitude angle */
   D_EPT_SER_MSG_CODE   , /* Numerical code from serial device */

   /*
   ** SET_1 Demands that the EPT will action and respond to 
   ** These demands will be sumbitted to the SDB as they are made
   */
   D_EPT_DMD_AP_STATE   , /* Aperture Demand */
   D_EPT_DMD_S1_STATE   , /* Shutter 1 Demand */
   D_EPT_DMD_S2_STATE   , /* Shutter 2 Demand */
   D_EPT_DMD_AP_POS     , /* Aperture Position Demand */
   D_EPT_DMD_S1_POS     , /* Shutter 1 Position Demand */
   D_EPT_DMD_S2_POS     , /* Shutter 2 Position Demand */
   D_EPT_DMD_HALT       , /* Halt Motion Demand */
   D_EPT_DMD_STOP       , /* Prevent Opening Demand */

   /* Liverpool-only datums */
   D_EPT_DMD_ALL_LCK    , /* All Locks  */
   D_EPT_DMD_TOP_LCK    , /* Top Locks  */
   D_EPT_DMD_S1_LCK     , /* Shutter 1 Locks  */
   D_EPT_DMD_S2_LCK     , /* Shutter 2 Locks  */
   D_EPT_DMD_OHTG       , /* Switch Oil Tank Heating On/Off */
   D_EPT_DMD_HYD        , /* Switch Hydraulic System (Prime) On/Off */
   D_EPT_DMD_DCHRG_ACC  , /* Discharge Accumulators */
   D_EPT_DMD_FLT        , /* Fault Reset */
   D_EPT_DMD_ESTOP      , /* Emergency Stop Reset */
   D_EPT_DMD_SAFE       , /* Switch Enclosure PLC into Safe State */

   /* Dome-only data */
   D_EPT_DMD_DOME_AZM   , /* Dome Azimuth Angle (mas) Demand */
   D_EPT_DMD_DOME_ALT   , /* Dome Altitude Angle (mas) Demand */

   /*
   ** This command is internal to EPT process and should not be
   ** requested by any other process
   */
   D_EPT_DMD_QRY_STATUS , /* Query Status */

   D_EPT_DATAID_EOL      /* End of list marker */
} eEptDataId_t;


#endif

/*******************************************************************************
** End of File Name: Ept.h
*******************************************************************************/

