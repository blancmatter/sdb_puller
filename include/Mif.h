/*
**
** Module Name:
**       Mif.h
**
** Purpose:
**
** Description:
**      This file provides the public prototypes and definitions
**      for use with the Motor InterFace package.
**
** Authors:
**    JDM:     J. D. Mills (TTL).
**    DJM:     D. J. McKay (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**    $Id: Mif.h,v 0.22 2006/12/07 16:24:42 mjf Exp $
**
** History:
**    $Log: Mif.h,v $
**    Revision 0.22  2006/12/07 16:24:42  mjf
**    Suppress 'incorrect rotator selected' log message when not a rotator
**    sub-system.
**
**    Revision 0.21  2004/06/07 13:36:48  mkb
**    Add rotator selection OIDs.
**
**    Revision 0.20  2004/05/19 08:51:15  mkb
**    Set scheduler priority and policy using OS independant call.
**
**    Revision 0.19  2004/05/06 12:30:38  mkb
**    Minor tidy-up during testing of code port.
**
**    Revision 0.18  2001/11/29 11:27:13  mjf
**    Store the individual digital input bits in the SDB.
**
**    Revision 0.17  2001/10/29 10:57:41  djm
**    Removed motor velocity feedback percentage OID and added
**    new OIDs for the motor torque correction (set by the AMC)
**    and DAC smoothing filter flag.
**
**    Revision 0.16  2001/10/15 09:20:03  djm
**    Added enumerations for the four new OIDs and removed the
**    single obsolete OID (D_MIF_MOTOR_TORQUE_LIMIT).
**
**    Revision 0.15  2001/04/26 14:56:23  djm
**    Added status codes for when the axis or motors are
**    torque limited. Also added OIDs to flag when this
**    is the case.
**
**    Revision 0.14  2001/03/06 10:23:23  jdm
**    New error codes added and line endings corrected.
**
**    Revision 0.13  2001/02/20 11:59:59  jdm
**    SIMULATION_ACTIVE OID removed.
**
**    Revision 0.12  2001/02/08 16:40:04  jdm
**    E_MIF_SERVER_PRI set to 29.
**
**    Revision 0.11  2001/01/31 09:47:03  rdl
**    Addition of #define for Server Priority.
**
**    Revision 0.10  2001/01/08 13:48:30  jdm
**    Simulator OIDs added.
**
**    Revision 0.9  2001/01/02 15:16:50  jdm
**    Package revision OID added.
**
**    Revision 0.8  2000/12/04 17:37:10  jdm
**    Watchdog enable OID added.
**
**    Revision 0.7  2000/11/29 15:23:17  jdm
**    New MifExternal error codes added.
**
**    Revision 0.6  2000/11/23 10:39:45  jdm
**    Simulation mode added.
**
**    Revision 0.5  2000/10/17 15:48:56  jdm
**    Server name changed.
**
**    Revision 0.4  2000/10/10 08:40:41  jdm
**    Shutdown OID added.
**
**    Revision 0.3  2000/10/09 12:34:44  jdm
**    Brake and drive enable OIDs added.
**
**    Revision 0.2  2000/10/06 15:28:09  jdm
**    New OIDs added for multifunction card
**    parameters.
**
**    Revision 0.1  2000/10/05 09:26:59  jdm
**    Initial creation.
**
**
*/

#ifndef _MIF_HEADER_INCLUDED
#define _MIF_HEADER_INCLUDED

/*
** =============================================================================
** Start of include files:
** =============================================================================
*/

#include "TtlSystem.h"  /* Include the package definitions. */

/*
** =============================================================================
** End of include files:
** =============================================================================
*/

/* Code for coping with decl'n/def'n/init'n of global variables */

#ifdef _MIF_SERVER_MAIN
   #define E_MIF_EXTERN
   #define E_MIF_INIT(Initialisation) = Initialisation
#else
   #define E_MIF_EXTERN extern
   #define E_MIF_INIT(Initaliasation)
#endif

/*
** Definition of the number of MIF input bits to store in the SDB.
*/

#define E_MIF_INPUT_BITS     0x20


/*
** =============================================================================
** Start of Type Definitions:
** =============================================================================
*/

/* Enumerate this package's error codes. */
typedef enum{
    E_MIF_CARD_NOT_INITIALISED = STATUS_START(MIF),
    E_MIF_UNABLE_TO_INITIALISE_IO_CARD,
    E_MIF_UNABLE_TO_MONITOR_BRAKE_STATUS,
    E_MIF_UNABLE_TO_MONITOR_DRIVE_STATUS,
    E_MIF_UNABLE_TO_MONITOR_INTERLOCK_STATUS,
    E_MIF_UNABLE_TO_MONITOR_L2_LIMIT_STATUS,
    E_MIF_UNABLE_TO_APPLY_TORQUE_TO_MOTORS,
    E_MIF_VALIDATION_FAILURE,
    E_MIF_MESSAGE_NOT_RECOGNISED,
    E_MIF_OID_TABLE_INIT_FAILURE,
    E_MIF_FAILED_TO_CONFIGURE_TIMER,
    E_MIF_FAILED_TO_CONFIGURE_INTERRUPT_PROXY,
    E_MIF_FAILED_TO_SEND_MESSAGE_TO_END_CLIENT,
    E_MIF_UNABLE_TO_TOGGLE_WATCHDOG_OUTPUT,
    E_MIF_UNABLE_TO_STORE_OID_VALUES,
    E_MIF_FAILED_TO_READ_CONFIG_FILE,
    E_MIF_FAILED_TO_SET_OID_NAME_TO_VALUE,

    E_MIF_AXIS_TORQUE_POS_LIMITED,     /* Axis is torque limited (+ve sense) */
    E_MIF_AXIS_TORQUE_NEG_LIMITED,     /* Axis is torque limited (-ve sense) */
    E_MIF_MOTOR1_TORQUE_POS_LIMITED,   /* Motor 1 torque limited (+ve sense) */
    E_MIF_MOTOR1_TORQUE_NEG_LIMITED,   /* Motor 1 torque limited (-ve sense) */
    E_MIF_MOTOR2_TORQUE_POS_LIMITED,   /* Motor 2 torque limited (+ve sense) */
    E_MIF_MOTOR2_TORQUE_NEG_LIMITED,   /* Motor 2 torque limited (-ve sense) */
    
    E_MIF_INCORRECT_ROTATOR,

    E_MIF_SOFTWARE_ERROR = STATUS_START(MIF) + 0xFFFF
}eMifClientStatus_t;

/* Enumerate the OID's owned by this package. */
typedef enum eMifDataId_e{
    D_MIF_DATAID_BOL = OID_START(MIF),  /* Offset the OID's by the package ID. */   /* 0000 */
    D_MIF_PROC_STATE,                   /* The motor interface process state. */    /* 0001 */
    D_MIF_APP_VERSION,                  /* MIF package revision. */                 /* 0002 */
    D_MIF_SERVER_SHUTDOWN,              /* Server shutdown command flag. */         /* 0003 */

    D_MIF_AXIS_TORQUE_DEMAND,           /* Axis torque demand. */                   /* 0004 */
    D_MIF_AXIS_TORQUE_LIMIT,            /* Axis torque limit. */                    /* 0005 */
    D_MIF_MOTOR_FULL_PRELOAD_TORQUE,    /* Idle motor torque when at full preload *//* 0006 */
    D_MIF_MOTOR_PRELOAD_PERCENTAGE,     /* Actual motor preload percentage. */      /* 0007 */
    D_MIF_MOTOR_PRELOAD_TORQUE,         /* Actual motor preload torque. */          /* 0008 */
    D_MIF_AMPLIFIER_GAIN,               /* Amplifier gain. */                       /* 0009 */
    D_MIF_MOTOR_GAIN,                   /* Motor gain. */                           /* 000A */
    D_MIF_DAC_GAIN,                     /* DAC gain. */                             /* 000B */
    D_MIF_DAC_OFFSET,                   /* DAC offset. */                           /* 000C */
    D_MIF_CLAMPED_AXIS_TORQUE_DEMAND,   /* Clamped axis torque demand. */           /* 000D */
    D_MIF_CLAMPED_MOTOR_1_TORQUE_DEMAND,/* Clamped motor 1 torque demand. */        /* 000E */
    D_MIF_CLAMPED_MOTOR_2_TORQUE_DEMAND,/* Clamped motor 2 torque demand. */        /* 000F */
    D_MIF_UNFILTERED_MOTOR_1_DAC_BITS,  /* Unfiltered motor 1 DAC bits. */          /* 0010 */
    D_MIF_UNFILTERED_MOTOR_2_DAC_BITS,  /* Unfiltered motor 2 DAC bits. */          /* 0011 */
    D_MIF_FILTERED_MOTOR_1_DAC_BITS,    /* Filtered motor 1 DAC bits. */            /* 0012 */
    D_MIF_FILTERED_MOTOR_2_DAC_BITS,    /* Filtered motor 2 DAC bits. */            /* 0013 */
    D_MIF_DIGITAL_INPUT_BITS,           /* Digital input bit-word. */               /* 0014 */
    D_MIF_DIGITAL_OUTPUT_BITS,          /* Digital output bit-word. */              /* 0015 */
    D_MIF_PLC_INTERLOCK_STATUS,         /* PLC interlock status. */                 /* 0016 */
    D_MIF_L2_LIMIT_STATUS,              /* PLC L2 limit status. */                  /* 0017 */
    D_MIF_MOTOR_1_MEASURED_CURRENT,     /* Motor 1 measured current. */             /* 0018 */
    D_MIF_MOTOR_2_MEASURED_CURRENT,     /* Motor 2 measured current. */             /* 0019 */
    D_MIF_MOTOR_1_MEASURED_TORQUE,      /* Motor 1 measured torque. */              /* 001A */
    D_MIF_MOTOR_2_MEASURED_TORQUE,      /* Motor 2 measured torque. */              /* 001B */
    D_MIF_BRAKE_RELEASE_ENABLE,         /* Brake release demand flag. */            /* 001C */
    D_MIF_BRAKE_STATUS_ON,              /* Brake status flag from PLC. */           /* 001D */
    D_MIF_DRIVE_ENABLE,                 /* Drive enable demand flag. */             /* 001E */
    D_MIF_DRIVE_ENABLE_STATUS_ON,       /* Drive enable status from PLC. */         /* 001F */
    D_MIF_MOTOR_1_DEMAND_CURRENT,       /* Motor 1 demanded current. */             /* 0020 */
    D_MIF_MOTOR_2_DEMAND_CURRENT,       /* Motor 2 demanded current. */             /* 0021 */  
    D_MIF_WATCHDOG_OUTPUT_ENABLE,       /* Enable the watchdog output. */           /* 0022 */
    D_MIF_SIM_AXIS_POSN_LSWORD,         /* Simulated axis position - LSWord. */     /* 0023 */
    D_MIF_SIM_AXIS_POSN_MSWORD,         /* Simulated axis position - MSWord. */     /* 0024 */
    D_MIF_SIM_MOT1_POSN_LSWORD,         /* Simulated motor 1 position - LSWord. */  /* 0025 */
    D_MIF_SIM_MOT1_POSN_MSWORD,         /* Simulated motor 1 position - MSWord. */  /* 0026 */
    D_MIF_SIM_MOT2_POSN_LSWORD,         /* Simulated motor 2 position - LSWord. */  /* 0027 */
    D_MIF_SIM_MOT2_POSN_MSWORD,         /* Simulated motor 2 position - MSWord. */  /* 0028 */
    D_MIF_AXIS_TORQUE_CLAMP_FLAG,       /* True if Ax torque demand is clamped */   /* 0029 */
    D_MIF_MOT1_TORQUE_CLAMP_FLAG,       /* True if M1 torque demand is clamped */   /* 002A */
    D_MIF_MOT2_TORQUE_CLAMP_FLAG,       /* True if M2 torque demand is clamped */   /* 002B */
    D_MIF_MOTOR_TORQUE_CORRECTION,      /* Correction based on velocity feedback */ /* 002C */
    D_MIF_MOTOR_TORQUE_MAX_LIMIT,       /* Motor torque maximum limit. */           /* 002D */
    D_MIF_MOTOR_TORQUE_MIN_LIMIT,       /* Motor torque minimum limit. */           /* 002E */

    D_MIF_DAC_SMOOTHING_FILTER_ENABLE,  /* DAC-bits smoothing filter */             /* 002F */
    D_MIF_ROTATOR_SELECTED,             /* Correct rotator selected. */             /* 0030 */

    D_MIF_INPUT_BIT_BASE,               /* Base for digital input bits */           /* 0031 */

    D_MIF_DATAID_EOL = D_MIF_INPUT_BIT_BASE + E_MIF_INPUT_BITS                /* End of list marker. */
}eMifDataId_t;

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

/* Define this server's name-server name and the name of its executeable. */
#define E_MIF_REG_SERVER_NAME   "E_TTL_MIF"
#define E_MIF_SERVER_PROC       "Mif"
#define E_MIF_SERVER_PATH       ""

#ifdef  E_WFL_OS_QNX6
#define E_MIF_SERVER_PRI                 55
#endif
#ifdef  E_WFL_OS_QNX4
#define E_MIF_SERVER_PRI                 25
#endif


#endif  /* _MIF_HEADER_INCLUDED */                                                    

/*
** =============================================================================
** End of header:   Mif.h
** =============================================================================
*/
