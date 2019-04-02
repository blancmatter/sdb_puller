/*
**
** Module Name:
**       Amc.h
**
** Purpose:
**
** Description:
**      This file provides the public prototypes and definitions
**      for use with the node's AMC package.
**
** Authors:
**    JDM:     J. D. Mills (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**    $Id: Amc.h,v 0.41 2006/12/07 10:45:19 mjf Exp $
**
** History:
**    $Log: Amc.h,v $
**    Revision 0.41  2006/12/07 10:45:19  mjf
**    Addition of datum to command a move to the park position (MOVE_PARK_POSITION)
**    and addition of boolean datum (AT_PARK_POSITION) to report when it's there.
**
**    Revision 0.40  2006/10/18 13:45:00  mjf
**    New error code for profiled/actual difference.
**
**    Revision 0.39  2006/10/18 13:33:04  mjf
**    Addition of functionality to monitor the difference between the
**    profiled and actual positions, to attempt to detect a stalled axis.
**    New datum entries for a tolerance, plus the current and maximum
**    difference between the two. Also, name of halting-with-no-VEN state
**    changed to be a general failed state.
**    Addition of function to move to LO or HI L1 limit.
**
**    Revision 0.38  2006/09/04 14:45:01  mjf
**    Public-header (only) brought in line with AMC_0_105 on QNX v4.
**
**    Revision 0.37  2004/06/07 10:34:20  mkb
**    Added error code for timing stutter.
**
**    Revision 0.36  2004/05/19 08:37:12  mkb
**    Set process priorities for QNX4 and QNX6
**
**    Revision 0.35  2004/05/06 12:20:35  mkb
**    Improve diagnostic information.
**    Streamline communications with other processes by
**    remembering connection details.
**
**    Revision 0.34  2003/12/18 12:33:37  sxf
**    Added D_AMC_L1_LIMIT_TOLERANCE
**
**    Revision 0.33  2003/05/09 10:37:34  sxf
**    Change axis velocity feedback gain parameter to D_AMC_AXIS_VEL_GAIN.
**
**    Revision 0.32  2003/05/02 15:05:03  mkb
**    Added support for L2 limit recovery.
**
**    Revision 0.31  2002/12/19 11:04:27  sxf
**    Added Oids for encoded model name and axis velocity feedback
**    frection and torque correction.
**
**    Revision 0.30  2002/04/26 16:06:57  djm
**    Added two new OID identifiers.
**
**    Revision 0.29  2002/02/14 11:31:58  rdl
**    Definition of unique error codes associated with recption of
**    commands and process states.
**
**    Revision 0.28  2002/02/07 12:46:47  djm
**    Added a new error code (E_AMC_VEN_FAILURE) and a place-
**    holder for a new motion state  for the finite state model
**    (E_AMC_STATE_HALTING_WITH_NO_VEN). This latter modification
**    is not really used, but we may require additional functions
**    to be performed in this case, so the state has been added
**    at this stage.
**
**    Revision 0.27  2001/11/30 16:40:06  djm
**    Added new error code (for resetting the motion profiler).
**
**    Revision 0.26  2001/11/08 17:53:40  mjf
**    Reduction in priority of process and TFP trigger proxy, to allow VEN
**    to sample the encoders before AMC samples from VEN.
**
**    Revision 0.25  2001/10/29 11:12:16  djm
**    Added new OIDs.
**
**    Revision 0.24  2001/08/20 10:27:07  mjf
**    Addition of enumeration names for states.
**
**    Revision 0.23  2001/08/08 09:24:48  djm
**    Updated version number.
**
**    Revision 0.22  2001/07/17 09:58:52  djm
**    Corrected comment in Velocity/Acceleration limit OIDs.
**
**    Revision 0.21  2001/03/27 21:56:03  jdm
**    EXTRAPOLATION OIDs removed.
**
**    Revision 0.20  2001/03/27 14:48:05  jdm
**    Invert velocity demand OID removed.
**
**    Revision 0.19  2001/03/26 13:48:12  jdm
**    Limit positions removed (moved to VEN) and
**    velocity demand OID has been removed.
**
**    Revision 0.18  2001/03/06 09:45:16  jdm
**    New error codes added and line endings converted.
**
**    Revision 0.17  2001/02/27 13:00:40  jdm
**    FAILED_TO_CONFIG_SHMEM error added.
**
**    Revision 0.16  2001/02/15 12:04:19  jdm
**    HALTING_OUTSIDE.... states removed.
**
**    Revision 0.15  2001/02/08 16:59:21  jdm
**    Priority set to 29..
**
**    Revision 0.14  2001/02/01 18:15:16  jdm
**    Acceleration and velocity limits added
**    and E_AMC_SERVER_PRI defined.
**
**    Revision 0.13  2001/01/24 14:53:38  jdm
**    Start-logging command added.
**
**    Revision 0.12  2001/01/11 18:05:42  jdm
**    Motion control testing functions added
**    and extrapolation added.
**
**    Revision 0.11  2000/12/21 11:01:06  jdm
**    shutdown and package revision OIDs
**    added.
**
**    Revision 0.10  2000/12/20 19:25:50  jdm
**    Extra error code added:
**    INVALID_CONTROL_MODE.
**
**    Revision 0.9  2000/12/07 14:58:18  jdm
**    Reference to AcnTypedef.h removed.
**
**    Revision 0.8  2000/12/05 16:56:41  jdm
**    Form feeds removed.
**
**    Revision 0.7  2000/12/04 16:26:27  jdm
**    Velocity control OIDs added.
**
**    Revision 0.6  2000/12/04 09:12:58  jdm
**    PID gains and integrator settings added.
**
**    Revision 0.5  2000/11/30 11:08:18  jdm
**    Tracking error OIDs added.
**
**    Revision 0.4  2000/11/29 12:52:16  jdm
**    Preload application period OID added.
**
**    Revision 0.3  2000/11/23 17:40:37  jdm
**    Soft limit functions added.
**
**    Revision 0.2  2000/11/21 11:46:49  jdm
**    PRECONTROLLING changed to DISABLED.
**
**    Revision 0.1  2000/11/16 16:07:45  jdm
**    Initial creation.
**
**
*/

#ifndef _AMC_HEADER_INCLUDED
#define _AMC_HEADER_INCLUDED

/*
** =============================================================================
** Start of include files:
** =============================================================================
*/

#include "TtlSystem.h"      /* Include the package definitions. */

/*
** =============================================================================
** End of include files:
** =============================================================================
*/

/*
** =============================================================================
** Start of Type Definitions:
** =============================================================================
*/

/* Enumerate this package's error codes. */
typedef enum{
   E_AMC_MESSAGE_NOT_RECOGNISED = STATUS_START(AMC),
   E_AMC_FAILED_TO_CONFIGURE_TIMER,
   E_AMC_OID_TABLE_INIT_FAILURE,
   E_AMC_FAILED_TO_SEND_MESSAGE_TO_END_CLIENT,
   E_AMC_FAILED_TO_CONFIGURE_INTERRUPT_PROXY,
   E_AMC_INVALID_STATE,
   E_AMC_EXPIRED_TARGET_TIME,               /* Not used replaced by E_AMC_TRACK_TIME_EXPIRED - debug */
   E_AMC_IN_CLOCKWISE_LIMIT,
   E_AMC_IN_ANTI_CLOCKWISE_LIMIT,
   E_AMC_INVALID_CONTROL_MODE,
   E_AMC_FAILED_TO_CONFIGURE_LOG_FILE,
   E_AMC_FAILED_TO_SEND_DATA_TO_LOG_FILE,
   E_AMC_DIVIDE_BY_ZERO,                    /* Algorithm would fail to zero denominator. */
   E_AMC_NO_SOLUTION,                       /* No solution may be found for this data. */
   E_AMC_NO_DATA,                           /* No data on which to perform algorithm. */   
   E_AMC_FAILED_TO_CONFIG_SHMEM,            /* Failed to config shmem for logger data. */
   E_AMC_FAILED_TO_READ_CONFIG_FILE,        /* Failed to read supplied config file. */
   E_AMC_FAILED_TO_SET_OID_NAME_TO_VALUE,   /* Failed to set OID name to value. */
   E_AMC_NO_VEN_HOMING_DIRECTION,           /* Couldn't get homing dir'n from VEN */
   E_AMC_PROFILE_RESET,                     /* Motion profile algorithm reset */
   E_AMC_VEN_FAILURE,                       /* The AMC has failed to get valid VEN data */
                             /* 
                             ** The following error codes are returned when the
                             ** package is in an invalid state for the received
                             ** command.
                             */
   E_AMC_HALT_INVALID_STATE,                /* HALT command          */
   E_AMC_MOVE_INVALID_STATE,                /* MOVE command          */
   E_AMC_TRACK_INVALID_STATE,               /* TRACK command         */
   E_AMC_RECOVER_INVALID_STATE,             /* RECOVER command       */
   E_AMC_STARTLOG_INVALID_STATE,            /* START LOGGING command */
   E_AMC_SINUSOID_INVALID_STATE,            /* SINUSOID command      */
   E_AMC_MOTN_TEST_MODE_INVALID_STATE,      /* MOTION TEST command   */
   E_AMC_MOTN_TEST_ENABLE_INVALID_STATE,    /* MOTION TEST ENABLE command */
   
   E_AMC_TRACK_TIME_EXPIRED,                /* Requested time for TRACK command has expired */
   E_AMC_PROFILED_ACTUAL_DIFF,              /* Difference between profiled and actual position */
   E_AMC_TIMING_STUTTER,                    /* Timing stutter. Performance may be degraded. */

   E_AMC_EO_ERROR_LIST = STATUS_START(AMC) + 0xFFFF
}eAmcClientStatus_t;

/* Enumerate the OID's owned by this package. */
typedef enum eAmcDataId_e{
   D_AMC_DATAID_BOL = OID_START(AMC),  /* Offset the OID's by the package ID. */       /* 0000 */
   D_AMC_PROC_STATE,                   /* The AMC process state. */                    /* 0001 */
   D_AMC_APP_VERSION,                  /* TFP package revision. */                     /* 0002 */
   D_AMC_SERVER_SHUTDOWN,              /* Server shutdown command flag. */             /* 0003 */

   D_AMC_STATE,                        /* The current FSM state. */                    /* 0004 */
   D_AMC_MOVE_ENABLE,                  /* The Enable this movement function. */        /* 0005 */
   D_AMC_AXIS_MOVE_FAILURE,            /* Failed while moving axis. */                 /* 0006 */
   D_AMC_MOVE_COMMAND,                 /* Trigger a MOVE command. */                   /* 0007 */
   D_AMC_TRACK_COMMAND,                /* Trigger a TRACK command. */                  /* 0008 */
   D_AMC_HALT_COMMAND,                 /* Trigger a HALT command. */                   /* 0009 */
   D_AMC_TARGET_TIME,                  /* The target time for a new TRACK command.*/   /* 000A */
   D_AMC_TARGET_TIME_DUMMY,            /* The target time for a new TRACK command.*/   /* 000B */
   D_AMC_TARGET_POSITION,              /* The target position for a new TRK or MV.*/   /* 000C */
   D_AMC_WORKING_TARGET_TIME,          /* Snapshot of current target time (nS). */     /* 000D */
   D_AMC_WORKING_TARGET_TIME_DUMMY,    /* Snapshot of current target time (Secs. */    /* 000E */
   D_AMC_WORKING_TARGET_POSITION,      /* Snapshot of current target position. */      /* 000F */
   D_AMC_SINUSOID_COMMAND,             /* Trigger a SINUSOID command. */               /* 0010 */
   D_AMC_SINUSOID_AMPLITUDE,           /* Amplitude for new SINE command. */           /* 0011 */
   D_AMC_SINUSOID_FREQUENCY,           /* Frequency for new SINE command. */           /* 0012 */
   D_AMC_AXIS_POSITION_DEMAND,         /* Position demand to the axis. */              /* 0013 */
   D_AMC_SIMULATION_ACTIVE,            /* Enable SIMULATION mode. */                   /* 0014 */
   D_AMC_CLOCKWISE_LIMIT_ACTIVE,       /* The clockwise limit status. */               /* 0015 */
   D_AMC_ACW_LIMIT_ACTIVE,             /* The anti-clockwise limit status. */          /* 0016 */
   D_AMC_DISTANCE_TO_CW_LIMIT,         /* The distance to the clockwise L1 limit. */   /* 0017 */
   D_AMC_DISTANCE_TO_ACW_LIMIT,        /* The distance to the anti-cw L1 limit. */     /* 0018 */
   D_AMC_RECOVER_COMMAND,              /* Trigger a recovery from a limit. */          /* 0019 */
   D_AMC_LIMIT_RECOVERY_DISTANCE,      /* Distance to recover beyond a limit. */       /* 001A */
   D_AMC_PRELOAD_APPLICATION_PERIOD,   /* Duration to apply preload over. */           /* 001B */
   D_AMC_PEAK_TRACKING_ERROR,          /* Control loop peak tracking error. */         /* 001C */
   D_AMC_RMS_TRACKING_ERROR,           /* Control loop RMS tracking error. */          /* 001D */
   D_AMC_AXIS_POSITION,                /* The current axis position. */                /* 001E */
   D_AMC_PID_P1_GAIN,                  /* PID controller P1 gain x 1e9. */             /* 001F */
   D_AMC_PID_P2_GAIN,                  /* PID controller P2 gain x 1e9. */             /* 0020 */
   D_AMC_PID_K1_GAIN,                  /* PID controller K1 gain x 1e9. */             /* 0021 */
   D_AMC_INTEGRATOR_WIND_UP_LIMIT,     /* PID controller integrator limit value. */    /* 0022 */
   D_AMC_INTEGRATOR_LIMIT_REACHED,     /* PID controller integrator limit status. */   /* 0023 */
   D_AMC_HOMING_VELOCITY,              /* Homing velocity magnitude. */                /* 0024 */
   D_AMC_MAX_HOMING_DURATION,          /* Maximum homing duration before timeout. */   /* 0025 */
   D_AMC_CURRENT_AXIS_VELOCITY,        /* The current axis velocity in as/s. */        /* 0026 */
   D_AMC_ENABLE_MOTION_TESTING,        /* Enable motion control test mode. */          /* 0027 */
   D_AMC_MOTION_CONTROL_TEST_MODE,     /* The mode of operation for testing. */        /* 0028 */
   D_AMC_START_LOGGING_COMMAND,        /* Start logging data to log file. */           /* 0029 */
   D_AMC_VELOCITY_LIMIT,               /* Axis velocity limit - in as/s. */            /* 002A */
   D_AMC_ACCELERATION_LIMIT,           /* Acceleration limit - in as/s^2. */           /* 002B */
   D_AMC_VELOCITY_CONTROL_DC_GAIN,     /* DC loop gain in velocity control - integer.*//* 002C */

   D_AMC_MOTOR_1_VELOCITY,             /* Motor1 velocity in as/s */                   /* 002D */
   D_AMC_MOTOR_2_VELOCITY,             /* Motor2 velocity in as/s */                   /* 002E */
   D_AMC_MOTOR_VELOCITY_SUM,           /* Motor1 + Motor2 velocity */                  /* 002F */
   D_AMC_MOTOR_VELOCITY_DIFF,          /* Motor1 - Motor2 velocity */                  /* 0030 */

   D_AMC_MTR_VEL_FEEDBACK_FRACTION,    /* MtrVelDiff scaling for Trq correction */     /* 0031 */
   D_AMC_MTR_VEL_TRQ_CORRECTION,       /* Torque correction factor */                  /* 0032 */

   D_AMC_SERVO_CTRL_ALGORITHM_TYPE,    /* Servo algorithm used (PID, HNF, etc.) */     /* 0033 */
   D_AMC_SERVO_CTRL_ALGORITHM_VER,     /* Servo algorithm version-number/ID */         /* 0034 */

   D_AMC_INTERRUPT_STUTTER_LIMIT,      /* Threshold for detecting interrupt repeats */ /* 0035 */
   D_AMC_INTERRUPT_STUTTER_IGNORE,     /* Flag to ingoring stuttered interrupts */     /* 0036 */

   D_AMC_HNF_MODEL_ID_1,               /* First siz chars of H-infinity model name */  /* 0037 */
   D_AMC_HNF_MODEL_ID_2,               /* Last six chars of H-infinity model name */   /* 0038 */

   D_AMC_AXIS_VEL_GAIN,                /* Axis vel feedback gain */                    /* 0039 */
   D_AMC_AXIS_VEL_TRQ_CORRECTION,      /* Torque correction for axis vel feedback  */  /* 003A */

   D_AMC_RECOVER_L2_LIMIT,             /* Recover from an L2 limit.                */  /* 003B */
   D_AMC_RECOVER_L2_VELOCITY,          /* Velocity to use during L2 limit recovery.*/  /* 003C */
   D_AMC_RECOVER_L2_DURATION,          /* Duration of L2 limit recovery.           */  /* 003D */

   D_AMC_MOVE_RE_ENABLE,               /* Have another go at homing.         */        /* 003E */

   D_AMC_L1_LIMIT_TOLERANCE,           /* Tolerance for dropping out of L1 limit */    /* 003F */

   D_AMC_PROFILER_TOLERANCE,           /* Tolerance for halting axis on profiler */    /* 0040 */
   D_AMC_PROFILER_DIFF,                /* Difference between profiled and actual */    /* 0041 */
   D_AMC_MAX_PROFILER_DIFF,            /* Max difference between profile and actual */ /* 0042 */

   D_AMC_MOVE_LO,                      /* Move to lower L1 limit */                    /* 0043 */
   D_AMC_MOVE_HI,                      /* Move to upper L1 limit */                    /* 0044 */

   D_AMC_MOVE_PARK_POSITION,           /* Move to park position */                     /* 0045 */
   D_AMC_AT_PARK_POSITION,             /* Flag to denote at park position */           /* 0046 */

   D_AMC_DATAID_EOL                    /* End of list marker. */
}eAmcDataId_t;

/* Enumerate the states in this package's FSM. */
typedef enum eAmcStates_e {
   E_AMC_STATE_DISABLED,
   E_AMC_STATE_APPLYING_PRELOAD,
   E_AMC_STATE_HOMING_HEADS,
   E_AMC_STATE_AXIS_HALTED,
   E_AMC_STATE_MOVING,
   E_AMC_STATE_TRACKING,
   E_AMC_STATE_APPLYING_SINUSOID,
   E_AMC_STATE_HALTING_AXIS,
   E_AMC_STATE_EXIT,
   E_AMC_STATE_HALTING_INSIDE_L1_CW_LIMIT,
   E_AMC_STATE_HALTED_INSIDE_L1_CW_LIMIT,
   E_AMC_STATE_RECOVERING_FROM_L1_CW_LIMIT,
   E_AMC_STATE_HALTING_INSIDE_L1_ACW_LIMIT,
   E_AMC_STATE_HALTED_INSIDE_L1_ACW_LIMIT,
   E_AMC_STATE_RECOVERING_FROM_L1_ACW_LIMIT,
   E_AMC_STATE_MOTION_TESTING,
   E_AMC_STATE_STOPPING_FAILED,
   E_AMC_STATE_RECOVER_L2_LIMIT
} eAmcStates_t;

/* 
** Enumerate the control modes of operation.
*/
typedef enum eAmcControlMode_e {
   E_AMC_INIT_MODE,
   E_AMC_TORQUE_MODE,
   E_AMC_VELOCITY_MODE,
   E_AMC_POSITION_MODE,

   E_AMC_CONTROL_MODE_EOL  /* End of list marker. */
}  eAmcControlMode_t;

/*
** =============================================================================
** End of Type Definitions:
** =============================================================================
*/


/*
** =============================================================================
** Start of Specific Definitions:
** =============================================================================
*/

/* Define this server's name-server name and the name of its executable. */
#define E_AMC_REG_SERVER_NAME   "E_TTL_AMC"
#define E_AMC_SERVER_PROC       "Amc"
#define E_AMC_SERVER_PATH       ""

#ifdef  E_WFL_OS_QNX6
#define E_AMC_SERVER_PRI                 50
#endif
#ifdef  E_WFL_OS_QNX4
#define E_AMC_SERVER_PRI                 20
#endif

#endif  /* _AMC_HEADER_INCLUDED */                                                    

/*
** =============================================================================
** End of header:   Amc.h
** =============================================================================
*/
