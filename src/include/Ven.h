/*
**
** Module Name:
**     Ven.h
**
** Purpose:
**
** Description:
**    This file provides the public prototypes and definitions
**    for use with the virtual encoder package.
**
** Authors:
**   JDM:      J. D. Mills (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**   $Id: Ven.h,v 0.42 2006/10/19 11:26:47 mjf Exp $
**
** History:
**   $Log: Ven.h,v $
**   Revision 0.42  2006/10/19 11:26:47  mjf
**   Addition of error codes for encoder signal status.
**
**   Revision 0.41  2006/06/29 14:29:58  sxf
**   Changes for baseline VEN_1_15.
**
**   Revision 0.40  2005/07/15 10:18:07  mjf
**   Renaming of datum to avoid clashes with AMC datum of the same name.
**
**   Revision 0.39  2005/06/17 12:00:26  mjf
**   Consistent with VEN_0_60.
**
**   Revision 0.38  2005/05/13 13:24:42  mjf
**   Consistent with TQB_0_44.
**
**   Revision 0.37  2004/11/09 10:57:29  mkb
**   First attempt to read linear encoders for secondary mirror tilt.
**
**   Revision 0.36  2004/09/16 11:21:47  sxf
**   Added shared memory error code.
**
**   Revision 0.35  2004/09/07 07:39:17  asp
**   Added D_VEN_AXIS_PARK_POSITION datum.
**
**   Revision 0.34  2004/09/06 07:35:56  asp
**   Implement logging of raw encoder values using D_VEN_START_LOGGING_COMMAND oid.
**
**   Revision 0.33  2004/06/08 10:14:38  asp
**   Modified OIDs to be more consistent with other .cfg files.
**   Added ERA180 encoder ring support.
**   Added TFP stutter check.
**
**   Revision 0.32  2004/05/25 12:51:42  asp
**   Added OID to allow motor and tape encoder starting IK220 port
**
**   Revision 0.31  2004/05/24 13:08:40  asp
**   Added EnDat absolute encoder and port OID entries.
**
**   Revision 0.30  2004/05/19 09:16:46  mkb
**   Set QNX6 process priority
**
**   Revision 0.29  2004/04/27 09:38:45  mkb
**   Removed explicit path to executable.
**
**   Revision 0.28  2004/04/20 15:05:06  asp
**   Port to QNX6.
**   EnDat absolute encoder support added -abs switch.
**   Alternate IK220 port selectable using -tape_a & -tape_b switch.
**   Page mode debug support added using -page switch.
**
**   Revision 0.27  2003/05/30 13:38:25  mkb
**   Added OID to enable encoder recovery.
**
**   Revision 0.26  2003/04/03 11:21:18  mkb
**   Added status code for encoder head that has lost homed state.
**
**   Revision 0.25  2002/07/30 06:18:03  djm
**   Added new OIDs.
**
**   Revision 0.24  2002/04/19 13:05:27  djm
**   Altered the order of the OID list. Also renamed one of the
**   OIDs to better represent the quantity that it labels.
**
**   Revision 0.23  2002/04/09 10:48:15  djm
**   Added new coefficients ready for the implementation of the
**   encoder non-linearity model.
**
**   Revision 0.22  2002/04/04 10:04:09  djm
**   Changed OID name to avoid a clash elsewhere in the system.
**
**   Revision 0.21  2002/04/04 09:11:35  djm
**   Added a new OID (D_VEN_LOGGING_PERIOD), which controls
**   how often encoder data is logged to the normal message
**   logging path.
**
**   Revision 0.20  2002/03/26 12:25:11  djm
**   Added four new Datum Identifiers.
**
**   Revision 0.19  2002/03/05 16:37:31  djm
**   Added a new OID (D_VEN_EXPECTED_ENCODER_MASK) which allows
**   the encoders that are expected to be specified via, say, a
**   configuration file. This is useful for axes which do not
**   use four encoders (the default), such as elevation or
**   cassegrain.
**   Also added some new error codes.
**
**   Revision 0.18  2002/02/28 13:58:17  djm
**   Added new OIDs for signal status changes and the tolerance
**   and difference between linear and absolute encoders.
**
**   Revision 0.17  2002/01/23 17:35:56  djm
**   Added new OIDs for encoder signal amplitude and frequency
**   excess detection.
**
**   Revision 0.16  2001/08/20 10:34:38  mjf
**   Addition of enum name to homing direction.
**
**   Revision 0.15  2001/04/04 18:26:56  jdm
**   StartWrap functions removed.
**
**   Revision 0.14  2001/03/27 11:01:53  jdm
**   Axis Abs_pos_dummy OIDs removed.
**
**   Revision 0.13  2001/03/26 20:02:11  jdm
**   Limit positions added to VEN. Number_of_heads removed,
**   Min and Max diff counts added and negative feedback
**   flags for motor and axis added.
**
**   Revision 0.12  2001/03/06 13:46:45  jdm
**   Axis counts per rev and Motor counts
**   per rev OIDs added.
**
**   Revision 0.11  2001/03/06 11:21:09  jdm
**   New error codes added and line endings converted.
**
**   Revision 0.10  2001/02/15 14:15:00  jdm
**   ABS_ENCODER_OFFSET removed.
**
**   Revision 0.9  2001/02/06 11:42:58  jdm
**   Extra error codes added for Combination function.
**   Encoder mask OID added.
**
**   Revision 0.8  2001/02/01 17:06:14  jdm
**   E_VEN_SERVER_PRI definition added.
**
**   Revision 0.7  2001/02/01 14:25:29  jdm
**   OIDS added for motor positions (in m.a.s.), and
**   overall axis position.
**
**   Revision 0.6  2000/12/12 20:15:56  jdm
**   Added Wrap snapshotting OIDs.
**
**   Revision 0.5  2000/12/11 14:35:52  jdm
**   Wrap value OID added.
**
**   Revision 0.4  2000/12/07 19:07:55  jdm
**   HomingDirection_t made public.
**
**   Revision 0.3  2000/12/07 18:34:00  jdm
**   OIDs added for homing direction.
**
**   Revision 0.2  2000/12/06 09:38:59  jdm
**   Form-feeds removed.
**
**   Revision 0.1  2000/12/06 08:25:40  jdm
**   Initial creation.
**
**
*/

#ifndef _VEN_HEADER_INCLUDED
#define _VEN_HEADER_INCLUDED

/*
** =============================================================================
** Start of include files:
** =============================================================================
*/

#include "TtlSystem.h"     /* Include the package definitions. */

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
   E_VEN_CARD_NOT_INITIALISED = STATUS_START(VEN),
   E_VEN_MESSAGE_NOT_RECOGNISED,
   E_VEN_OID_TABLE_INIT_FAILURE,
   E_VEN_FAILED_TO_CONFIGURE_INTERRUPT_PROXY,
   E_VEN_FAILED_TO_CONFIGURE_ENCODER_CARDS,
   E_VEN_UNABLE_TO_READ_ENCODER_COUNTS,
   E_VEN_UNABLE_TO_STORE_ENCODER_COUNTS,
   E_VEN_FAILED_TO_CONFIGURE_REFRESH_TIMER,
   E_VEN_FAILED_TO_SEND_MESSAGE_TO_END_CLIENT,
   E_VEN_UNABLE_TO_ACCESS_ABSOLUTE_ENCODER,
   E_VEN_UNABLE_TO_DETERMINE_HOMING_DIRECTION,
   E_VEN_ABS_LIN_MISMATCH,     /* Abs/Lin enc values do not correspond. 0x0C. */
   E_VEN_BAD_ABS_ENCODER,      /* Invalid status or value from absolute enc. 0x0D. */
   E_VEN_TOO_MANY_LIN_ENCODER, /* More encoders available than expected. 0x0E. */
   E_VEN_BAD_LIN_ENCODER,      /* Crucial linear encoders have bad status. 0x0F. */
   E_VEN_NO_LIN_ENCODER,       /* No linear encoders available. 0x10. */
   E_VEN_FAILED_TO_READ_CONFIG_FILE,
   E_VEN_FAILED_TO_SET_OID_NAME_TO_VALUE,
   E_VEN_FAILED_TO_ENABLE_CORRECTION_DET,
   E_VEN_FAILED_TO_DISABLE_CORRECTION_DET,
   E_VEN_ENC_DOES_NOT_EXIST,   /* Encoder not present or expected */
   E_VEN_FAILED_SDB_SUBMISSION,/* Failed to submit data to SDB (via REP) */
   E_VEN_HEAD_LOST_HOMING,     /* An encoder head has lost its homed status. */
   E_VEN_TIMING_STUTTER,       /* Timing stutter. Performance may be degraded. */
   E_VEN_SHMEM_ERR,            /* Error declaring or filling shared memory */
   E_VEN_DUMP_FILE_ERR,        /* Error accessing dump file for counts */
   E_VEN_COR_FILE_ERR,         /* Error accessing correction file */
   E_VEN_COR_PROC_ERR,         /* Error processing corrections */
   E_VEN_ENCODER_NOT_LATCHED,  /* Encoder reading is not latched */
   E_VEN_ENCODER_GLITCH,       /* Detected possible encoder glitch */
   E_VEN_DCRM_DIFF_COUNT,      /* Error in DCRM difference count after homing */
   E_VEN_INVALID_ENCODER_ID,   /* Invalid encoder ID supplied */
   E_VEN_SIGNAL_SMALL,         /* Encoder signal strength small */
   E_VEN_SIGNAL_TOO_HIGH,      /* Encoder signal strength too high */
   E_VEN_SIGNAL_TOO_SMALL,     /* Encoder signal strength too small */
   E_VEN_FREQ_EXCEEDED,        /* Encoder signal frequency exceeded */

   E_VEN_SOFTWARE_ERROR = STATUS_START(VEN) + 0xFFFF
}eVenClientStatus_t;

/* Enumerate the OID's owned by this package. */
typedef enum eVenDataId_e{
   D_VEN_DATAID_BOL = OID_START(VEN),  /* Offset the OID's by the package ID. */ /* 0000 */
   D_VEN_PROC_STATE,                   /* The VEN process state. */              /* 0001 */
   D_VEN_APP_VERSION,                  /* VEN package revision. */               /* 0002 */
   D_VEN_SERVER_SHUTDOWN,              /* Server shutdown command flag. */       /* 0003 */

   D_VEN_MOTOR_ENC_1_COUNT,            /* Motor 1 encoder raw count. */          /* 0004 */
   D_VEN_MOTOR_ENC_1_COUNT_DUMMY,      /* Motor 1 encoder dummy upper word. */   /* 0005 */
   D_VEN_MOTOR_ENC_1_STATE,            /* Motor 1 encoder state. */              /* 0006 */
   D_VEN_MOTOR_ENC_2_COUNT,            /* Motor 2 encoder raw count. */          /* 0007 */
   D_VEN_MOTOR_ENC_2_COUNT_DUMMY,      /* Motor 2 encoder dummy upper word. */   /* 0008 */
   D_VEN_MOTOR_ENC_2_STATE,            /* Motor 2 encoder state. */              /* 0009 */
   D_VEN_MOTOR_ENC_1_ABS_POS,          /* Motor encoder 1 absolute position. */  /* 000A */
   D_VEN_MOTOR_ENC_1_ABS_POS_DUMMY,    /* Motor encoder 1 absolute pos. MSWord.*//* 000B */
   D_VEN_MOTOR_ENC_2_ABS_POS,          /* Motor encoder 2 absolute position. */  /* 000C */
   D_VEN_MOTOR_ENC_2_ABS_POS_DUMMY,    /* Motor encoder 2 absolute pos. MSWord.*//* 000D */
   D_VEN_AXIS_ENC_1_COUNT,             /* Axis encoder 1 raw count. */           /* 000E */
   D_VEN_AXIS_ENC_1_COUNT_DUMMY,       /* Axis encoder 1 dummy upper word. */    /* 000F */
   D_VEN_AXIS_ENC_1_STATE,             /* Axis encoder 1 state. */               /* 0010 */
   D_VEN_AXIS_ENC_1_REF_STATE,         /* Axis encoder 1 reference state. */     /* 0011 */
   D_VEN_AXIS_ENC_2_COUNT,             /* Axis encoder 2 raw count. */           /* 0012 */
   D_VEN_AXIS_ENC_2_COUNT_DUMMY,       /* Axis encoder 2 dummy upper word. */    /* 0013 */
   D_VEN_AXIS_ENC_2_STATE,             /* Axis encoder 2 state. */               /* 0014 */
   D_VEN_AXIS_ENC_2_REF_STATE,         /* Axis encoder 2 reference state. */     /* 0015 */
   D_VEN_AXIS_ENC_3_COUNT,             /* Axis encoder 3 raw count. */           /* 0016 */
   D_VEN_AXIS_ENC_3_COUNT_DUMMY,       /* Axis encoder 3 dummy upper word. */    /* 0017 */
   D_VEN_AXIS_ENC_3_STATE,             /* Axis encoder 3 state. */               /* 0018 */
   D_VEN_AXIS_ENC_3_REF_STATE,         /* Axis encoder 3 reference state. */     /* 0019 */
   D_VEN_AXIS_ENC_4_COUNT,             /* Axis encoder 4 raw count. */           /* 001A */
   D_VEN_AXIS_ENC_4_COUNT_DUMMY,       /* Axis encoder 4 dummy upper word. */    /* 001B */
   D_VEN_AXIS_ENC_4_STATE,             /* Axis encoder 4 state. */               /* 001C */
   D_VEN_AXIS_ENC_4_REF_STATE,         /* Axis encoder 4 reference state. */     /* 001D */
   D_VEN_AXIS_ENC_1_OFFSET,            /* Axis encoder 1 counter offset. */      /* 001E */
   D_VEN_AXIS_ENC_1_OFFSET_DUMMY,      /* Axis encoder 1 offset dummy word. */   /* 001F */
   D_VEN_AXIS_ENC_2_OFFSET,            /* Axis encoder 2 counter offset. */      /* 0020 */
   D_VEN_AXIS_ENC_2_OFFSET_DUMMY,      /* Axis encoder 2 offset dummy word. */   /* 0021 */
   D_VEN_AXIS_ENC_3_OFFSET,            /* Axis encoder 3 counter offset. */      /* 0022 */
   D_VEN_AXIS_ENC_3_OFFSET_DUMMY,      /* Axis encoder 3 offset dummy word. */   /* 0023 */
   D_VEN_AXIS_ENC_4_OFFSET,            /* Axis encoder 4 counter offset. */      /* 0024 */
   D_VEN_AXIS_ENC_4_OFFSET_DUMMY,      /* Axis encoder 4 offset dummy word. */   /* 0025 */
   D_VEN_AXIS_ENC_1_ABS_POSITION,      /* Axis encoder 1 absolute position. */   /* 0026 */
   D_VEN_AXIS_ENC_2_ABS_POSITION,      /* Axis encoder 2 absolute position. */   /* 0027 */
   D_VEN_AXIS_ENC_3_ABS_POSITION,      /* Axis encoder 3 absolute position. */   /* 0028 */
   D_VEN_AXIS_ENC_4_ABS_POSITION,      /* Axis encoder 4 absolute position. */   /* 0029 */
   D_VEN_AXIS_POS,                     /* Our final measurement of axis pos. */  /* 002A */
   D_VEN_AXIS_POS_DUMMY,               /* Final axis pos. measurment MSWord. */  /* 002B */
   D_VEN_HOME_AXIS_ENCODER_1,          /* Start axis encoder 1 homing. */        /* 002C */
   D_VEN_HOME_AXIS_ENCODER_2,          /* Start axis encoder 2 homing. */        /* 002D */
   D_VEN_HOME_AXIS_ENCODER_3,          /* Start axis encoder 3 homing. */        /* 002E */
   D_VEN_HOME_AXIS_ENCODER_4,          /* Start axis encoder 4 homing. */        /* 002F */
   D_VEN_ABS_ENCODER_POSITION,         /* Absolute encoder position. */          /* 0030 */
   D_VEN_ABS_ENCODER_STATUS,           /* Absolute encoder status. */            /* 0031 */
   D_VEN_JOIN_POSITION,                /* Join position */                       /* 0032 */
   D_VEN_HOMING_DIRECTION,             /* Direction to home the axis. */         /* 0033 */
   D_VEN_HEAD_1_POSITION,              /* Head 1 position w.r.t. abs-enc zero. *//* 0034 */
   D_VEN_HEAD_2_POSITION,              /* Head 2 position w.r.t. abs-enc zero. *//* 0035 */
   D_VEN_HEAD_3_POSITION,              /* Head 3 position w.r.t. abs-enc zero. *//* 0036 */
   D_VEN_HEAD_4_POSITION,              /* Head 4 position w.r.t. abs-enc zero. *//* 0037 */
   D_VEN_USED_ENCODER_MASK,            /* Mask showing which encoders are used.*//* 0038 */
   D_VEN_AXIS_COUNTS_PER_REV,          /* No. of axis encoder counts per rev.*/  /* 0039 */
   D_VEN_MOTOR_COUNTS_PER_REV,         /* No. of motor encoder counts per rev.*/ /* 003A */
   D_VEN_MIN_TAPE_DIFF_COUNT,          /* Minimum tape difference count value. *//* 003B */
   D_VEN_MAX_TAPE_DIFF_COUNT,          /* Maximum tape difference count value. *//* 003C */
   D_VEN_AXIS_ENCODER_NEG_FEEDBACK,    /* Enable inversion of axis feedback. */  /* 003D */
   D_VEN_MOTOR_ENCODER_NEG_FEEDBACK,   /* Enable inversion of motor feedback. */ /* 003E */
   D_VEN_CLOCKWISE_L1_LIMIT_POSITION,  /* Clockwise L1 limit position. */        /* 003F */
   D_VEN_ANTI_CW_L1_LIMIT_POSITION,    /* Anti-clockwise L1 limit position. */   /* 0040 */
   D_VEN_AXIS_ENC_1_MIN_AMPL_STATUS,   /* Head 1 min. signal amplitude status */ /* 0041 */
   D_VEN_AXIS_ENC_1_MAX_AMPL_STATUS,   /* Head 1 max. signal amplitude status */ /* 0042 */
   D_VEN_AXIS_ENC_1_CUR_AMPL_STATUS,   /* Head 1 cur. signal amplitude status */ /* 0043 */
   D_VEN_AXIS_ENC_1_FREQ_EXCEEDED,     /* Head 1 signal frequency exceeded */    /* 0044 */
   D_VEN_AXIS_ENC_2_MIN_AMPL_STATUS,   /* Head 2 min. signal amplitude status */ /* 0045 */
   D_VEN_AXIS_ENC_2_MAX_AMPL_STATUS,   /* Head 2 max. signal amplitude status */ /* 0046 */
   D_VEN_AXIS_ENC_2_CUR_AMPL_STATUS,   /* Head 2 cur. signal amplitude status */ /* 0047 */
   D_VEN_AXIS_ENC_2_FREQ_EXCEEDED,     /* Head 2 signal frequency exceeded */    /* 0048 */
   D_VEN_AXIS_ENC_3_MIN_AMPL_STATUS,   /* Head 3 min. signal amplitude status */ /* 0049 */
   D_VEN_AXIS_ENC_3_MAX_AMPL_STATUS,   /* Head 3 max. signal amplitude status */ /* 004a */
   D_VEN_AXIS_ENC_3_CUR_AMPL_STATUS,   /* Head 3 cur. signal amplitude status */ /* 004b */
   D_VEN_AXIS_ENC_3_FREQ_EXCEEDED,     /* Head 3 signal frequency exceeded */    /* 004c */
   D_VEN_AXIS_ENC_4_MIN_AMPL_STATUS,   /* Head 4 min. signal amplitude status */ /* 004d */
   D_VEN_AXIS_ENC_4_MAX_AMPL_STATUS,   /* Head 4 max. signal amplitude status */ /* 004e */
   D_VEN_AXIS_ENC_4_CUR_AMPL_STATUS,   /* Head 4 cur. signal amplitude status */ /* 004f */
   D_VEN_AXIS_ENC_4_FREQ_EXCEEDED,     /* Head 4 signal frequency exceeded */    /* 0050 */
   D_VEN_AXIS_ENC_1_MIN_AMPL_CHANGE,   /* Head 1 min.sig.ampl. status changes */ /* 0051 */
   D_VEN_AXIS_ENC_1_MAX_AMPL_CHANGE,   /* Head 1 max.sig.ampl. status changes */ /* 0052 */
   D_VEN_AXIS_ENC_1_CUR_AMPL_CHANGE,   /* Head 1 cur.sig.ampl. status changes */ /* 0053 */
   D_VEN_AXIS_ENC_2_MIN_AMPL_CHANGE,   /* Head 2 min.sig.ampl. status changes */ /* 0054 */
   D_VEN_AXIS_ENC_2_MAX_AMPL_CHANGE,   /* Head 2 max.sig.ampl. status changes */ /* 0055 */
   D_VEN_AXIS_ENC_2_CUR_AMPL_CHANGE,   /* Head 2 cur.sig.ampl. status changes */ /* 0056 */
   D_VEN_AXIS_ENC_3_MIN_AMPL_CHANGE,   /* Head 3 min.sig.ampl. status changes */ /* 0057 */
   D_VEN_AXIS_ENC_3_MAX_AMPL_CHANGE,   /* Head 3 max.sig.ampl. status changes */ /* 0058 */
   D_VEN_AXIS_ENC_3_CUR_AMPL_CHANGE,   /* Head 3 cur.sig.ampl. status changes */ /* 0059 */
   D_VEN_AXIS_ENC_4_MIN_AMPL_CHANGE,   /* Head 4 min.sig.ampl. status changes */ /* 005a */
   D_VEN_AXIS_ENC_4_MAX_AMPL_CHANGE,   /* Head 4 max.sig.ampl. status changes */ /* 005b */
   D_VEN_AXIS_ENC_4_CUR_AMPL_CHANGE,   /* Head 4 cur.sig.ampl. status changes */ /* 005c */
   D_VEN_ABS_LIN_ENC_TOLERANCE,        /* Max.allowed deviation between encs. */ /* 005d */
   D_VEN_ABS_LIN_ENC_DEVIATION,        /* Max.deviation between lin/abs encs. */ /* 005e */
   D_VEN_EXPECTED_ENCODER_MASK,        /* Number of encoders installed */        /* 005f */
   D_VEN_AXIS_ENC_1_STATUS_BITS,       /* Status bits from HHE encoder head 1 */ /* 0060 */
   D_VEN_AXIS_ENC_2_STATUS_BITS,       /* Status bits from HHE encoder head 2 */ /* 0061 */
   D_VEN_AXIS_ENC_3_STATUS_BITS,       /* Status bits from HHE encoder head 3 */ /* 0062 */
   D_VEN_AXIS_ENC_4_STATUS_BITS,       /* Status bits from HHE encoder head 4 */ /* 0063 */
   D_VEN_ENC_LOGGING_PERIOD,           /* No.cycles at which enc.vals logged */  /* 0064 */

   D_VEN_ENL_ENABLE_CORRECTION,        /* Enable enc.non-linearity correct'ns */ /* 0065 */
   D_VEN_ENL_RMS_ENC_1_SHORT,          /* Short term RMS from mean for E1, mas*/ /* 0066 */
   D_VEN_ENL_RMS_ENC_2_SHORT,          /* Short term RMS from mean for E1, mas*/ /* 0067 */
   D_VEN_ENL_RMS_ENC_3_SHORT,          /* Short term RMS from mean for E1, mas*/ /* 0068 */
   D_VEN_ENL_RMS_ENC_4_SHORT,          /* Short term RMS from mean for E1, mas*/ /* 0069 */
   D_VEN_ENL_RMS_ENC_1_MED,            /* Med term RMS from mean for E1, mas*/   /* 006A */
   D_VEN_ENL_RMS_ENC_2_MED,            /* Med term RMS from mean for E1, mas*/   /* 006B */
   D_VEN_ENL_RMS_ENC_3_MED,            /* Med term RMS from mean for E1, mas*/   /* 006C */
   D_VEN_ENL_RMS_ENC_4_MED,            /* Med term RMS from mean for E1, mas*/   /* 006D */
   D_VEN_ENL_RMS_ENC_1_LONG,           /* Long term RMS from mean for E1, mas*/  /* 006E */
   D_VEN_ENL_RMS_ENC_2_LONG,           /* Long term RMS from mean for E1, mas*/  /* 006F */
   D_VEN_ENL_RMS_ENC_3_LONG,           /* Long term RMS from mean for E1, mas*/  /* 0070 */
   D_VEN_ENL_RMS_ENC_4_LONG,           /* Long term RMS from mean for E1, mas*/  /* 0071 */
   D_VEN_ENL_SHORTTERM_SEC,            /* Time for short term ENL RMS, sec  */   /* 0072 */
   D_VEN_ENL_MEDTERM_SEC,              /* Time for medium term ENL RMS, sec */   /* 0073 */
   D_VEN_ENL_LONGTERM_SEC,             /* Time for long term ENL RMS, sec */     /* 0074 */
   D_VEN_ENL_CORRECTION1,              /* Reserved for future use */             /* 0075 */
   D_VEN_ENL_CORRECTION2,              /* Reserved for future use */             /* 0076 */
   D_VEN_ENL_CORRECTION3,              /* Reserved for future use */             /* 0077 */
   D_VEN_ENL_CORRECTION4,              /* Reserved for future use */             /* 0078 */
   D_VEN_ENL_RESERVED1,                /* Reserved for future use */             /* 0079 */
   D_VEN_ENL_RESERVED2,                /* Reserved for future use */             /* 007A */
   D_VEN_ENL_RESERVED3,                /* Reserved for future use */             /* 007B */
   D_VEN_ENL_RESERVED4,                /* Reserved for future use */             /* 007C */
   D_VEN_ENL_RESERVED5,                /* Reserved for future use */             /* 007D */
   D_VEN_ENL_RESERVED6,                /* Reserved for future use */             /* 007E */
   D_VEN_ENL_RESERVED7,                /* Reserved for future use */             /* 007F */
   D_VEN_ENL_RESERVED8,                /* Reserved for future use */             /* 0080 */
   D_VEN_ENL_RESERVED9,                /* Reserved for future use */             /* 0081 */
   D_VEN_ENL_RESERVED10,               /* Reserved for future use */             /* 0082 */

   D_VEN_AXIS_HOME_ATTRACT_POSITION,   /* Axis pos'n towards which to home */    /* 0083 */
   D_VEN_AXIS_HOME_REPULSE_POSITION,   /* Axis pos'n away from which to home */  /* 0084 */

   D_VEN_ENCODER_AUTO_RECOVER,         /* Automatically recover lost encoders */ /* 0085 */

   D_VEN_AXIS_PARK_POSITION,           /* Preferred parked position for axis */  /* 0086 */
   D_VEN_START_ENC_LOG,                /* AMC-style fast logging to dump file */ /* 0087 */

   D_VEN_HHE_ABS_ENC_COUNTS_PER_REV,   /* No. of abs. encoder counts per rev. */ /* 0088 */
   D_VEN_HHE_ABS_ENC_NEG_FEEDBACK,     /* Enable inversion of motor feedback. */ /* 0089 */

   D_VEN_HHE_ABS_ENC_POSITION,         /* Hhe Absolute Encoder Position. */      /* 008A */
   D_VEN_HHE_ABS_ENC_OFFSET,           /* User defined offset. */                /* 008B */
   D_VEN_HHE_ABS_ENC_ORIGIN,           /* Position of arbitrary 0 origin. */     /* 008C */
   D_VEN_HHE_ABS_ENC_PINION_TEETH,     /* No. of teeth on pinion. */             /* 008D */
   D_VEN_HHE_ABS_ENC_SPUR_TEETH,       /* No. of teeth on spur gear. */          /* 008E */

   D_VEN_HHE_ABS_ENC_TYPE,             /* Encoder type. */                       /* 008F */
   D_VEN_HHE_ABS_ENC_PERIOD,           /* Encoder period. */                     /* 0090 */
   D_VEN_HHE_ABS_ENC_STEP,             /* Steps per turn. */                     /* 0091 */
   D_VEN_HHE_ABS_ENC_TURNS,            /* Resolvable turns. */                   /* 0092 */ 
   D_VEN_HHE_ABS_ENC_REFDIST,          /* Distance to reference mark. */         /* 0093 */
   D_VEN_HHE_ABS_ENC_CNTDIR,           /* Direction to centre position. */       /* 0094 */

   D_VEN_HHE_ABS_ENC_ENABLED,          /* Use the EnDat Absolute encoder */      /* 0095 */
   D_VEN_HHE_ABS_ENC_PORT,             /* IK220 port used for absolute encoder*/ /* 0096 */

   D_VEN_HHE_TAPE_ENC_1_PORT,          /* IK220 port connected to tape enc. 1 */ /* 0097 */
   D_VEN_HHE_TAPE_ENC_3_PORT,          /* IK220 port connected to tape enc. 2 */ /* 0098 */
   D_VEN_HHE_MOTOR_1_PORT,             /* IK220 port connected to motor enc.  */ /* 0099 */

   D_VEN_USE_ERA180_ENCODER,           /* ERA180 encoder used (for Nasymth)   */ /* 009A */
   D_VEN_INTERRUPT_STUTTER_IGNORE,     /* Ignore TFP pulses if timestamp not unique. *//* 009B */

   D_VEN_ENL_STATE,                    /* State of ENL collection/compensation */  /* 009C */
   D_VEN_ENL_AXISREFVEL,               /* Reference velocity of axis           */  /* 009D */
   D_VEN_ENL_BLENDTICKS,               /* Velocity change before switch tables */  /* 009E */

   D_VEN_LIN_ENC_MASK,                 /* Mask of linear encoders to use.      */  /* 009F */
   D_VEN_LIN_ENC_ENABLE,               /* Enable linear encoders.              */  /* 00A0 */
   D_VEN_LIN_ENC_HOME,                 /* Start linear encoders homing.        */  /* 00A1 */
   D_VEN_LIN_ENC_RESET,                /* Use current counts for offset.       */  /* 00A2 */
   D_VEN_LIN_ENC_STATE,                /* Combined linear encoders state.      */  /* 00A3 */
   D_VEN_LIN_ENC_TOLERANCE,            /* Tolerance to sanity-check readings.  */  /* 00A4 */
   D_VEN_LIN_ENC_1_PORT,               /* IK220 port connected to linear enc. 1*/  /* 00A5 */
   D_VEN_LIN_ENC_2_PORT,               /* IK220 port connected to linear enc. 2*/  /* 00A6 */
   D_VEN_LIN_ENC_3_PORT,               /* IK220 port connected to linear enc. 3*/  /* 00A7 */
   D_VEN_LIN_ENC_1_STATUS_BITS,        /* Status bits for linear encoder 1.    */  /* 00A8 */
   D_VEN_LIN_ENC_2_STATUS_BITS,        /* Status bits for linear encoder 2.    */  /* 00A9 */
   D_VEN_LIN_ENC_3_STATUS_BITS,        /* Status bits for linear encoder 3.    */  /* 00AA */
   D_VEN_LIN_ENC_1_STATE,              /* Lin enc 1 state.                     */  /* 00AB */
   D_VEN_LIN_ENC_2_STATE,              /* Lin enc 2 state.                     */  /* 00AC */
   D_VEN_LIN_ENC_3_STATE,              /* Lin enc 3 state.                     */  /* 00AD */
   D_VEN_LIN_ENC_1_COUNT_PER_MM,       /* Lin enc 1 scaling.                   */  /* 00AE */
   D_VEN_LIN_ENC_2_COUNT_PER_MM,       /* Lin enc 2 scaling.                   */  /* 00AF */
   D_VEN_LIN_ENC_3_COUNT_PER_MM,       /* Lin enc 3 scaling.                   */  /* 00B0 */
   D_VEN_LIN_ENC_1_OFFSET,             /* Lin enc 1 counter offset.            */  /* 00B1 */
   D_VEN_LIN_ENC_2_OFFSET,             /* Lin enc 2 counter offset.            */  /* 00B2 */
   D_VEN_LIN_ENC_3_OFFSET,             /* Lin enc 3 counter offset.            */  /* 00B3 */
   D_VEN_LIN_ENC_1_ABS_COUNT,          /* Lin enc 1 absolute (raw) counts.     */  /* 00B4 */
   D_VEN_LIN_ENC_2_ABS_COUNT,          /* Lin enc 2 absolute (raw) counts.     */  /* 00B5 */
   D_VEN_LIN_ENC_3_ABS_COUNT,          /* Lin enc 3 absolute (raw) counts.     */  /* 00B6 */
   D_VEN_LIN_ENC_1_ABS_POS,            /* Lin enc 1 absolute position.         */  /* 00B7 */
   D_VEN_LIN_ENC_2_ABS_POS,            /* Lin enc 2 absolute position.         */  /* 00B8 */
   D_VEN_LIN_ENC_3_ABS_POS,            /* Lin enc 3 absolute position.         */  /* 00B9 */
   D_VEN_LIN_ENC_1_REL_COUNT,          /* Lin enc 1 relative (offset) counts.  */  /* 00BA */
   D_VEN_LIN_ENC_2_REL_COUNT,          /* Lin enc 2 relative (offset) counts.  */  /* 00BB */
   D_VEN_LIN_ENC_3_REL_COUNT,          /* Lin enc 3 relative (offset) counts.  */  /* 00BC */
   D_VEN_LIN_ENC_1_REL_POS,            /* Lin enc 1 relative (offset) position.*/  /* 00BD */
   D_VEN_LIN_ENC_2_REL_POS,            /* Lin enc 2 relative (offset) position.*/  /* 00BE */
   D_VEN_LIN_ENC_3_REL_POS,            /* Lin enc 3 relative (offset) position.*/  /* 00BF */

   D_VEN_AXIS_ENC_1_COMPOFF0,          /* Axis 1 comp value offset0            */  /* 00C0 */
   D_VEN_AXIS_ENC_1_COMPOFF90,         /* Axis 1 comp value offset90           */  /* 00C1 */
   D_VEN_AXIS_ENC_1_COMPPHASE0,        /* Axis 1 comp value phase0             */  /* 00C2 */
   D_VEN_AXIS_ENC_1_COMPPHASE90,       /* Axis 1 comp value phase90            */  /* 00C3 */
   D_VEN_AXIS_ENC_1_COMPSYMM0,         /* Axis 1 comp value symmetry0          */  /* 00C4 */
   D_VEN_AXIS_ENC_1_COMPSYMM90,        /* Axis 1 comp value symmetry90         */  /* 00C5 */
   D_VEN_AXIS_ENC_1_COMPFLAG1,         /* Axis 1 comp value flag1              */  /* 00C6 */
   D_VEN_AXIS_ENC_1_COMPFLAG2,         /* Axis 1 comp value flag2              */  /* 00C7 */

   D_VEN_AXIS_ENC_2_COMPOFF0,          /* Axis 1 comp value offset0            */  /* 00C8 */
   D_VEN_AXIS_ENC_2_COMPOFF90,         /* Axis 1 comp value offset90           */  /* 00C9 */
   D_VEN_AXIS_ENC_2_COMPPHASE0,        /* Axis 1 comp value phase0             */  /* 00CA */
   D_VEN_AXIS_ENC_2_COMPPHASE90,       /* Axis 1 comp value phase90            */  /* 00CB */
   D_VEN_AXIS_ENC_2_COMPSYMM0,         /* Axis 1 comp value symmetry0          */  /* 00CC */
   D_VEN_AXIS_ENC_2_COMPSYMM90,        /* Axis 1 comp value symmetry90         */  /* 00CD */
   D_VEN_AXIS_ENC_2_COMPFLAG1,         /* Axis 1 comp value flag1              */  /* 00CE */
   D_VEN_AXIS_ENC_2_COMPFLAG2,         /* Axis 1 comp value flag2              */  /* 00CF */

   D_VEN_AXIS_ENC_3_COMPOFF0,          /* Axis 1 comp value offset0            */  /* 00D0 */ 
   D_VEN_AXIS_ENC_3_COMPOFF90,         /* Axis 1 comp value offset90           */  /* 00D1 */ 
   D_VEN_AXIS_ENC_3_COMPPHASE0,        /* Axis 1 comp value phase0             */  /* 00D2 */ 
   D_VEN_AXIS_ENC_3_COMPPHASE90,       /* Axis 1 comp value phase90            */  /* 00D3 */ 
   D_VEN_AXIS_ENC_3_COMPSYMM0,         /* Axis 1 comp value symmetry0          */  /* 00D4 */ 
   D_VEN_AXIS_ENC_3_COMPSYMM90,        /* Axis 1 comp value symmetry90         */  /* 00D5 */ 
   D_VEN_AXIS_ENC_3_COMPFLAG1,         /* Axis 1 comp value flag1              */  /* 00D6 */ 
   D_VEN_AXIS_ENC_3_COMPFLAG2,         /* Axis 1 comp value flag2              */  /* 00D7 */ 
                                                                                              
   D_VEN_AXIS_ENC_4_COMPOFF0,          /* Axis 1 comp value offset0            */  /* 00D8 */ 
   D_VEN_AXIS_ENC_4_COMPOFF90,         /* Axis 1 comp value offset90           */  /* 00D9 */ 
   D_VEN_AXIS_ENC_4_COMPPHASE0,        /* Axis 1 comp value phase0             */  /* 00DA */ 
   D_VEN_AXIS_ENC_4_COMPPHASE90,       /* Axis 1 comp value phase90            */  /* 00DB */ 
   D_VEN_AXIS_ENC_4_COMPSYMM0,         /* Axis 1 comp value symmetry0          */  /* 00DC */ 
   D_VEN_AXIS_ENC_4_COMPSYMM90,        /* Axis 1 comp value symmetry90         */  /* 00DD */ 
   D_VEN_AXIS_ENC_4_COMPFLAG1,         /* Axis 1 comp value flag1              */  /* 00DE */ 
   D_VEN_AXIS_ENC_4_COMPFLAG2,         /* Axis 1 comp value flag2              */  /* 00DF */ 

   D_VEN_AXIS_ENC_1_TALLY_NONNORMAL,      /* Axis 1 tally normal signals          */  /* 00E0 */
   D_VEN_AXIS_ENC_1_TALLY_SMALL,       /* Axis 1 tally small signals           */  /* 00E1 */
   D_VEN_AXIS_ENC_1_TALLY_TOO_HIGH,    /* Axis 1 tally too high signals        */  /* 00E2 */
   D_VEN_AXIS_ENC_1_TALLY_TOO_SMALL,   /* Axis 1 tally too small signals       */  /* 00E3 */ 
   D_VEN_AXIS_ENC_1_TALLY_FREQ,        /* Axis 1 tally signal freq exceeded    */  /* 00E4 */

   D_VEN_AXIS_ENC_2_TALLY_NONNORMAL,      /* Axis 2 tally normal signals          */  /* 00E5 */
   D_VEN_AXIS_ENC_2_TALLY_SMALL,       /* Axis 2 tally small signals           */  /* 00E6 */
   D_VEN_AXIS_ENC_2_TALLY_TOO_HIGH,    /* Axis 2 tally too high signals        */  /* 00E7 */
   D_VEN_AXIS_ENC_2_TALLY_TOO_SMALL,   /* Axis 2 tally too small signals       */  /* 00E8 */ 
   D_VEN_AXIS_ENC_2_TALLY_FREQ,        /* Axis 2 tally signal freq exceeded    */  /* 00E9 */

   D_VEN_AXIS_ENC_3_TALLY_NONNORMAL,      /* Axis 3 tally normal signals          */  /* 00EA */
   D_VEN_AXIS_ENC_3_TALLY_SMALL,       /* Axis 3 tally small signals           */  /* 00EB */
   D_VEN_AXIS_ENC_3_TALLY_TOO_HIGH,    /* Axis 3 tally too high signals        */  /* 00EC */
   D_VEN_AXIS_ENC_3_TALLY_TOO_SMALL,   /* Axis 3 tally too small signals       */  /* 00ED */ 
   D_VEN_AXIS_ENC_3_TALLY_FREQ,        /* Axis 3 tally signal freq exceeded    */  /* 00EE */

   D_VEN_AXIS_ENC_4_TALLY_NONNORMAL,      /* Axis 4 tally normal signals          */  /* 00EF */
   D_VEN_AXIS_ENC_4_TALLY_SMALL,       /* Axis 4 tally small signals           */  /* 00F0 */
   D_VEN_AXIS_ENC_4_TALLY_TOO_HIGH,    /* Axis 4 tally too high signals        */  /* 00F1 */
   D_VEN_AXIS_ENC_4_TALLY_TOO_SMALL,   /* Axis 4 tally too small signals       */  /* 00F2 */ 
   D_VEN_AXIS_ENC_4_TALLY_FREQ,        /* Axis 4 tally signal freq exceeded    */  /* 00F3 */

   D_VEN_AXIS_ENC_1_COMBINED_STATE,    /* Combined summary state, axis enc 1   */  /* 00F4 */
   D_VEN_AXIS_ENC_2_COMBINED_STATE,    /* Combined summary state, axis enc 2   */  /* 00F5 */
   D_VEN_AXIS_ENC_3_COMBINED_STATE,    /* Combined summary state, axis enc 3   */  /* 00F6 */
   D_VEN_AXIS_ENC_4_COMBINED_STATE,    /* Combined summary state, axis enc 4   */  /* 00F7 */
         
   D_VEN_DATAID_EOL                    /* End of list marker */

}  eVenDataId_t;

/* Enumerate the values of HomingDirection */
typedef  enum eVenDirection_e {
   E_VEN_HOMING_DIRECTION_INVALID,     /* Invalid homing direction */
   E_VEN_HOMING_CLOCKWISE,             /* Clockwise homing direction */
   E_VEN_HOMING_ANTI_CLOCKWISE         /* Anti-clockwise homing direction */
}  eVenDirection_t;

/* Enumerate ENL states */

typedef enum eVenEnlState_e
{
   E_VEN_ENL_NOMINAL,                  /* Nominal ENL compensation state */
   E_VEN_ENL_TO_COLLECT,               /* Waiting to collect ENL counts  */
   E_VEN_ENL_COLLECTING,               /* Collecting ENL counts */
   E_VEN_ENL_COLLECTED,                /* Completed collecting ENL counts */
   E_VEN_ENL_TO_APPLY,                 /* ENL compensations are to be applied */
   E_VEN_ENL_APPLYING,                 /* Applying ENL compensations */
   E_VEN_ENL_READING,                  /* Reading in ENL compensation table */
   E_VEN_ENL_COLLECT_ERROR,            /* Error collecting ENL counts */
   E_VEN_ENL_APPLY_ERROR               /* Error applying ENL compensations */
} eVenEnlState_t;

/* Enumerate Linear Encoder states */

typedef enum eVenLinEncState_e
{
   E_VEN_LIN_HOMED,                    /* Linear encoder homed */
   E_VEN_LIN_RESET_ABS,                /* Encoder homed and reset */
   E_VEN_LIN_RESET_REL,                /* Encoder reset */
   E_VEN_LIN_RANGE,                    /* Inter-encoder tolerance exceeded */
   E_VEN_LIN_ENABLED,                  /* Encoder enabled for use */
   E_VEN_LIN_DISABLED,                 /* Encoder disabled from use */
   E_VEN_LIN_ERROR,                    /* Encoder has encountered error */
   E_VEN_LIN_HOMING                    /* Encoder is homing */
} eVenLinEncState_t;


typedef struct eVenCombinedState_s
{
   /* Low 4 bits contain HHE state */
   unsigned int      HheEncoderState : 4;
   /* Bit 4 set if encoder in expected mask */
   unsigned int      Expected        : 1;
   /* Bit 5 set if encoder in used mask */
   unsigned int      Used            : 1;

}  eVenCombinedState_t;


/* Hhe Absolute Encoder */
typedef struct {
   Int32_t Offset;                     /* Axis angle at reference point     */
   Int32_t Origin;                     /* Encoder count at reference point  */ 
   Int32_t PinionTeeth;                /* No. of teeth on pinion            */
   Int32_t SpurTeeth;                  /* No. of teeth on spur wheel        */
   Int32_t Turns;                      /* AES Legacy - not sure what it does*/
   Bool_t Invert;                      /* Used to invert counting direction */ 
} eVenHheAbsEncConfig_t; 

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
#define E_VEN_REG_SERVER_NAME "E_TTL_VEN"
#define E_VEN_SERVER_PROC     "Ven"
#define E_VEN_SERVER_PATH     ""

#ifdef  E_WFL_OS_QNX6
#define E_VEN_SERVER_PRI                 55
#endif
#ifdef  E_WFL_OS_QNX4
#define E_VEN_SERVER_PRI                 25
#endif

#endif   /* _VEN_HEADER_INCLUDED */

/*
** =============================================================================
** End of header: Ven.h
** =============================================================================
*/
