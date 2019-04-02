/*
**
** Module Name :
**    Sif.h
**
** Purpose :
**    Public header file for the mirror Support Interface.
**
** Description :
**    The Support Interface (SIF) is the software process that performs
**    control and monitoring of the mirror support hardware. 
**
**    Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Imported Definitions :
**
**
** Exported Definitions :
**
**
** Private Definitions :
**
**
** Authors :
**    MKV : Mark Vernon (TTL)
**    
** Version :
**    $Id: Sif.h,v 0.32 2006/12/15 11:45:22 sxf Exp $
**
** History :
**    $Log: Sif.h,v $
**    Revision 0.32  2006/12/15 11:45:22  sxf
**    Add OID D_SIF_LAT_PRELOAD_AMP for lateral preload amplitude (millivolts).
**    Change logging of digital input word to decimal from hex and include entry in header.
**
**    Revision 0.31  2006/12/08 13:44:02  sxf
**    Make voltage offset to apply to lateral support configurable.
**
**    Revision 0.30  2006/11/24 10:57:47  man
**    Added additional OID's and changed the inclinometer noise
**    rejection filter to be the same as that used for load cell
**    noise rejection filtering.
**
**    Revision 0.29  2005/08/16 08:33:19  mkb
**    Define OIDS related to adaptive PID controllers.
**
**    Revision 0.28  2005/08/08 09:15:16  mkb
**    Add controller parameters and filter coefficients to OIDS
**    list. Each axial sector has seperate controller parameters.
**
**    Revision 0.27  2004/08/18 14:52:40  mjf
**    Removal of monitoring of +/- 8v inputs - the output bit toggled to do
**    this was introducing noise on feedbacks.
**    Use implementation of analogue offsets from QNX v4 codebase to replace
**    implementation in this code, after proving on Faulkes 2.
**
**    Revision 0.26  2004/06/02 10:39:10  mkb
**    Added loadcell offset values to OID list.
**
**    Revision 0.25  2004/04/20 09:48:08  mkb
**    First QNX 6 baseline.
**
**    Revision 0.24  2003/04/11 08:27:10  man
**    Modified axial gain to be sector dependent
**    e,g. can now specify a seperate gain for each
**    sector.
**
**    Revision 0.23  2003/04/11 07:51:44  man
**    Included a mirror lifting state, during which
**    Sif is in standby rather than warning.
**
**    Revision 0.22  2002/09/23 13:19:43  man
**    Included additional OID's for inclinometer
**    calibration and axial gain parameters.
**
**    Revision 0.21  2002/05/09 11:22:37  mjf
**    Updated in accordance with Panel 14 I/O schedule as of 01 May 2002.
**
**    Revision 0.20  2001/10/09 13:31:20  mjf
**    Renaming of HALT_DEMAND to STOP_DEMAND, and SIF_STOPPED to STOPPED.
**
**    Revision 0.19  2001/09/24 12:57:20  mjf
**    Change of line endings to be LF-only. No other changes made.
**
**    Revision 0.18  2001/08/23 13:33:02  mjf
**    Change of SIF state enumeration names for HTI-friendliness.
**
**    Revision 0.17  2001/08/23 13:05:06  mjf
**    Change of state enumeration names for HTI friendliness.
**
**    Revision 0.16  2001/08/22 07:16:49  mjf
**    Addition of OID to set a demanded halt condition, and OID to report if
**    movement is currently stopped.
**
**    Revision 0.15  2001/08/20 11:25:44  mjf
**    Change to construction of state enumeration for HTI.
**
**    Revision 0.14  2001/07/11 08:14:01  mjf
**    Tidying up of log messages. Reordering of OID table to match the
**    public header. Remove the watchdog output on a 'disable' command.
**
**    Revision 0.13  2001/07/10 14:35:57  mjf
**    Improvements to public definitions in header file.
**
**    Revision 0.12  2001/06/26 16:14:16  mjf
**    If help requested, terminate. If no I/O cards detected, terminate.
**    Create Sif.log in /opt/ttl/data. Create output file in /opt/ttl/data
**    unless a path is specified.
**
**    Revision 0.11  2001/04/11 13:47:37  mkv
**    Re-Structured Code.
**
**    Revision 0.10  2001/03/28 16:06:59  mkv
**    Part way through a code change to check out a previous baseline.
**
**    Revision 0.9  2001/03/27 09:53:31  mkv
**    North South Lateral Ctrl.
**
**    Revision 0.8  2001/03/14 09:20:05  mkv
**    Controller Design Trials.
**
**    Revision 0.7  2001/03/12 12:21:06  mkv
**    added D_SIF_ALTITUDE.
**
**    Revision 0.6  2001/02/14 09:41:37  mkv
**    Sdb flag and singel submission.
**
**    Revision 0.5  2001/01/29 09:50:20  mkv
**    Axial First Light, Lateral Untested.
**
**    Revision 0.4  2001/01/17 17:28:07  mkv
**    Notch Filter Included.
**
**    Revision 0.3  2001/01/05 10:57:45  mkv
**    Working Test Revision.
**
**    Revision 0.2  2000/12/22 10:41:03  mkv
**    log operating state to text file.
**
**    Revision 0.1  2000/12/08 11:58:04  mkv
**    Initial revision.
**
**
*/
#ifndef SIF_H_DEFINED
#define SIF_H_DEFINED

/*
** System include files
*/
#include <limits.h>
#include "TtlSystem.h"      /* Include the package definitions. */

/*
** Start of Type Definitions
*/

#define E_SIF_REG_SERVER_NAME "E_TTL_AMN_Sif"
#define E_SIF_SERVER_PROC     "Sif"
#define E_SIF_SERVER_PRI      10

#ifdef E_WFL_OS_QNX4
#define E_SIF_SERVER_PATH     "//1/opt/ttl/bin/Sif"
#else
#define E_SIF_SERVER_PATH     "/ttl/sw/bin/Sif"
#endif

/* Enumerate this package's error codes. */
enum eSifStatus_e 
{
    E_SIF_GEN_ERROR = STATUS_START(SIF),
    E_SIF_INIT_FAIL,
    E_SIF_SHUTDOWN_ERROR,
    E_SIF_INVALID_DATA_LEN,
    E_SIF_INVALID_COMMAND,
    E_SIF_REPLY_FAIL,
    E_SIF_SERVER_STARTUP_FAIL,
    E_SIF_SERVER_SHUTDOWN_FAIL,
    E_SIF_SERVER_NOT_RUNNING,
    E_SIF_SERVER_SEND_FAIL,
    E_SIF_SERIAL_TX_FAIL,
    E_SIF_SERIAL_TXLEN_ERR,
    E_SIF_SERIAL_RX_FAIL,
    E_SIF_SERIAL_CHK_ERR,
    E_SIF_TIMER_SETUP_ERR,
    E_SIF_TIMER_ATTACH_ERR,
    E_SIF_TIMER_CREATE_ERR,
    E_SIF_TIMER_UNDERFLOW_ERR,
    E_SIF_TIMER_OVERFLOW_ERR,
    E_SIF_TIMER_CONFIG_ERR,
    E_SIF_TIMER_CLOSE_ERR,
    E_SIF_SDB_UPDATE_FAIL,
    E_SIF_DEAD_ENDCLIENT,
    E_SIF_MESSAGE_NOT_RECOGNISED,
    E_SIF_UNABLE_TO_INIT_IO_CARD,
    E_SIF_OID_TABLE_INIT_FAILURE,
    E_SIF_CARD_NOT_INITIALISED,
    E_SIF_INVALID_CONTROL_MODE,
    E_SIF_INAVALID_SUPPORT_MECHANISM,
    E_SIF_MIRROR_NOT_AT_ZENITH,
    E_SIF_INVALID_INCLINOMETER,
    E_SIF_CLU_AMPLITUDE_ERR,
    E_SIF_UNABLE_TO_TOGGLE_WATCHDOG_OUTPUT,
    E_SIF_VALVE_FBK_ERROR,
    E_SIF_INVALID_OPERATION_ENABLE_STATE,
    E_SIF_INVALID_PROC_STATE,
    E_SIF_AIR_PRESSURE_NOT_PRESENT,
    E_SIF_SERVO_VALVE_POWER_NOT_PRESENT,
    E_SIF_ALREADY_PRELOADED_LATERAL,
    E_SIF_FAILED_TO_READ_CONFIG_FILE,
    E_SIF_FAILED_TO_SET_OID_NAME_TO_VALUE,

    E_SIF_STATUS_EOL,                  /* End Of List               */
    E_SIF_STATUS_MAX = INT_MAX         /* enum forced to be 4 bytes */
};


/* Enumerate this package's operating states. */
typedef enum eSifOpStates_e 
{
    E_SIF_SIF_UNINITIALISED = 0,
    E_SIF_SIF_INITIALISE,
    E_SIF_SIF_IDLE,
    E_SIF_SIF_PRELOAD,
    E_SIF_SIF_CENTRE,
    E_SIF_SIF_OPERATE,
    E_SIF_SIF_UNLOAD,
    E_SIF_SIF_TERMINATE,
    E_SIF_SIF_LIFTING,

    E_SIF_SIF_OP_STATE_EOL
} eSifOpStates_t;

/* Enumerate the OID's owned by this package. */
typedef enum esifDataId_e
{
   D_SIF_DATAID_BOL = OID_START(SIF),  /* Offset the OID's by the package ID. */   

   /* Generic Oid's */
   D_SIF_PROC_STATE,                   /* The SIF process state. */    
   D_SIF_APP_VERSION,                  /* Baseline Revision */   
   D_SIF_SHUTDOWN,                     /* Server shutdown command flag. */         
   D_SIF_STATE,                        /* Operating State */
   D_SIF_ENABLE,                       /* Enable / Disable Operation */                    
   D_SIF_ALTITUDE,                     /* Ain  14 Altitude angle in tenths of a degree */
   D_SIF_NORTH_SOUTH_LATITUDE,         /* North South Vector derived from Radial definers */
   D_SIF_EAST_WEST_LATITUDE,           /* East West Vector derived from Radial definers */

   /* Application Specific Oid's */
   D_SIF_REF_V_MONITOR,                /* Ain  00 6.2V Reference Vlotage Monitor */ 
   D_SIF_RED_AXIAL_LOAD,               /* Ain  01 Red Axial Load Cell  */                  
   D_SIF_YEL_AXIAL_LOAD,               /* Ain  02 Yellow Axial Load Cell  */               
   D_SIF_BLU_AXIAL_LOAD,               /* Ain  03 Blue Axial Load Cell  */                
   D_SIF_RED_RADIAL_LOAD,              /* Ain  04 Red Radial Load Cell  */                 
   D_SIF_YEL_RADIAL_LOAD,              /* Ain  05 Yellow Radial Load Cell  */              
   D_SIF_BLU_RADIAL_LOAD,              /* Ain  06 Blue Radial Load Cell  */                
   D_SIF_RED_AXIAL_VALVE_FBK,          /* Ain  07 Red Axial Servo Feedback */                
   D_SIF_YEL_AXIAL_VALVE_FBK,          /* Ain  08 Yellow Axial Servo Feedback */                
   D_SIF_BLU_AXIAL_VALVE_FBK,          /* Ain  09 Blue Axial Servo Feedback */                
   D_SIF_LAT_LOAD_VALVE_FBK,           /* Ain  10 Lateral Load Servo Feedback */                
   D_SIF_LAT_PRELOAD_VALVE_FBK,        /* Ain  11 Lateral PreLoad Servo Feedback */                
   D_SIF_PLUS_8V_MONITOR,              /* Ain  15a +8V Load Cell Monitor */                 
   D_SIF_MINUS_8V_MONITOR,             /* Ain  15b -8V Load Cell Monitor */                 
   D_SIF_REF_VOLTAGE,                  /* Aout 00 6.2V Load Cell Reference Vlotage */     
   D_SIF_RED_AXIAL_VALVE_DRIVE,        /* Aout 01 Red Axial Servo Input Signal */            
   D_SIF_YEL_AXIAL_VALVE_DRIVE,        /* Aout 02 Yellow Axial Servo Input Signal */            
   D_SIF_BLU_AXIAL_VALVE_DRIVE,        /* Aout 03 Blue Axial Servo Input Signal */            
   D_SIF_LAT_LOAD_VALVE_DRIVE,         /* Aout 04 Lateral Load Servo Input Signal */           
   D_SIF_LAT_PRELOAD_VALVE_DRIVE,      /* Aout 05 Lateral PreLoad Servo Input Signal */            
   D_SIF_AIR_PRESSURE_STATE,           /* Din  00 Mirror Cell air Pressure */              
   D_SIF_SERVO_VALVE_POWER_STATE,      /* Din  01 Flag for Servo Valve Power */            
   D_SIF_WATCHDOG1,                    /* Dout 00 Watchdog 1 for Mirror Node */            
   D_SIF_WATCHDOG2,                    /* Dout 01 Watchdog 2 for Mirror Node */            

   D_SIF_STOP_DEMAND,                  /* Write TRUE to stop supporting of mirror */
   D_SIF_STOPPED,                      /* Read TRUE if mirror supporting stopped */

   D_SIF_LAT_2_LOAD_VALVE_FBK,         /* Ain  12 Lateral 2 Load Servo Feedback */                
   D_SIF_LAT_2_PRELOAD_VALVE_FBK,      /* Ain  13 Lateral 2 PreLoad Servo Feedback */                
   D_SIF_LAT_2_LOAD_VALVE_DRIVE,       /* Aout 06 Lateral Load Servo Input Signal */           
   D_SIF_LAT_2_PRELOAD_VALVE_DRIVE,    /* Aout 07 Lateral PreLoad Servo Input Signal */         
   
                                       /* y = mx + c */
   D_SIF_INCLINOMETER_GRADIENT,        /* Inclinometer calibration coefficient (m) */
   D_SIF_INCLINOMETER_INTERCEPT,       /* Inclinometer calibration coefficient (c) */

   D_SIF_RED_AXIAL_GAIN,               /* Mirror support red axial gain */
   D_SIF_YEL_AXIAL_GAIN,               /* Mirror support yel axial gain */
   D_SIF_BLU_AXIAL_GAIN,               /* Mirror support blu axial gain */

   D_SIF_LATERAL_GAIN,                 /* Mirror support lateral gain */

   /* Offsets to apply to analogue input voltages */
   D_SIF_OFFSET_REF_V_MONITOR,            /* Offset for Ain  00 6.2V Reference Vlotage Monitor */ 
   D_SIF_OFFSET_RED_AXIAL_LOAD,           /* Offset for Ain  01 Red Axial Load Cell  */                  
   D_SIF_OFFSET_YEL_AXIAL_LOAD,           /* Offset for Ain  02 Yellow Axial Load Cell  */               
   D_SIF_OFFSET_BLU_AXIAL_LOAD,           /* Offset for Ain  03 Blue Axial Load Cell  */                
   D_SIF_OFFSET_RED_RADIAL_LOAD,          /* Offset for Ain  04 Red Radial Load Cell  */                 
   D_SIF_OFFSET_YEL_RADIAL_LOAD,          /* Offset for Ain  05 Yellow Radial Load Cell  */              
   D_SIF_OFFSET_BLU_RADIAL_LOAD,          /* Offset for Ain  06 Blue Radial Load Cell  */                
   D_SIF_OFFSET_RED_AXIAL_VALVE_FBK,      /* Offset for Ain  07 Red Axial Servo Feedback */                
   D_SIF_OFFSET_YEL_AXIAL_VALVE_FBK,      /* Offset for Ain  08 Yellow Axial Servo Feedback */                
   D_SIF_OFFSET_BLU_AXIAL_VALVE_FBK,      /* Offset for Ain  09 Blue Axial Servo Feedback */                
   D_SIF_OFFSET_LAT_LOAD_VALVE_FBK,       /* Offset for Ain  10 Lateral Load Servo Feedback */                
   D_SIF_OFFSET_LAT_PRELOAD_VALVE_FBK,    /* Offset for Ain  11 Lateral PreLoad Servo Feedback */                
   D_SIF_OFFSET_LAT_2_LOAD_VALVE_FBK,     /* Offset for Ain  12 Lateral 2 Load Servo Feedback */                
   D_SIF_OFFSET_LAT_2_PRELOAD_VALVE_FBK,  /* Offset for Ain  13 Lateral 2 PreLoad Servo Feedback */                

   /* Static Decoupling Matrix */
   D_SIF_SD_CELL_1_1,                  /* Static decoupling matrix cell 1 1 [x10^6] */
   D_SIF_SD_CELL_1_2,                  /* Static decoupling matrix cell 1 2 [x10^6] */
   D_SIF_SD_CELL_1_3,                  /* Static decoupling matrix cell 1 3 [x10^6] */
   D_SIF_SD_CELL_2_1,                  /* Static decoupling matrix cell 2 1 [x10^6] */
   D_SIF_SD_CELL_2_2,                  /* Static decoupling matrix cell 2 2 [x10^6] */
   D_SIF_SD_CELL_2_3,                  /* Static decoupling matrix cell 2 3 [x10^6] */
   D_SIF_SD_CELL_3_1,                  /* Static decoupling matrix cell 3 1 [x10^6] */
   D_SIF_SD_CELL_3_2,                  /* Static decoupling matrix cell 3 2 [x10^6] */
   D_SIF_SD_CELL_3_3,                  /* Static decoupling matrix cell 3 3 [x10^6] */

   /* 0.5Hz Filter coefficients for inclinometer */
   D_SIF_INCLIN_FILTER_COEFF_1,        /* Inclinometer filter coefficient 1 [x10^6] */
   D_SIF_INCLIN_FILTER_COEFF_2,        /* Inclinometer filter coefficient 2 [x10^6] */

   /* Loadcell noise rejection filter */
   D_SIF_NOISE_FILTER_IN_T0,           /* Loadcell noise rejection filter input coefficient 0 [x10^6] */  
   D_SIF_NOISE_FILTER_IN_T1,           /* Loadcell noise rejection filter input coefficient 1 [x10^6] */
   D_SIF_NOISE_FILTER_OUT,             /* Loadcell noise rejection filter output coefficient  [x10^6] */ 

   /* Axial control model coefficients. */
   D_SIF_RED_AX_CTRL_T1,               /* Axial control coefficient Red 1 [x10^6] */
   D_SIF_RED_AX_CTRL_T2,               /* Axial control coefficient Red 2 [x10^6] */   
   D_SIF_RED_AX_CTRL_T3,               /* Axial control coefficient Red 3 [x10^6] */   
   D_SIF_RED_AX_CTRL_T4,               /* Axial control coefficient Red 4 [x10^6] */   
                           
   D_SIF_YEL_AX_CTRL_T1,               /* Axial control coefficient Yellow 1 [x10^6] */    
   D_SIF_YEL_AX_CTRL_T2,               /* Axial control coefficient Yellow 2 [x10^6] */    
   D_SIF_YEL_AX_CTRL_T3,               /* Axial control coefficient Yellow 3 [x10^6] */    
   D_SIF_YEL_AX_CTRL_T4,               /* Axial control coefficient Yellow 4 [x10^6] */    
                           
   D_SIF_BLU_AX_CTRL_T1,               /* Axial control coefficient Blue 1 [x10^6] */    
   D_SIF_BLU_AX_CTRL_T2,               /* Axial control coefficient Blue 2 [x10^6] */    
   D_SIF_BLU_AX_CTRL_T3,               /* Axial control coefficient Blue 3 [x10^6] */    
   D_SIF_BLU_AX_CTRL_T4,               /* Axial control coefficient Blue 4 [x10^6] */    
                           
   D_SIF_RED_AX_INPUT_T0,              /* Load cell feedback coefficient Red 0 [x10^6] */
   D_SIF_RED_AX_INPUT_T1,              /* Load cell feedback coefficient Red 1 [x10^6] */  
   D_SIF_RED_AX_INPUT_T2,              /* Load cell feedback coefficient Red 2 [x10^6] */
   D_SIF_RED_AX_INPUT_T3,              /* Load cell feedback coefficient Red 3 [x10^6] */
   D_SIF_RED_AX_INPUT_T4,              /* Load cell feedback coefficient Red 4 [x10^6] */
   D_SIF_RED_AX_INPUT_T5,              /* Load cell feedback coefficient Red 5 [x10^6] */
                           
   D_SIF_YEL_AX_INPUT_T0,              /* Load cell feedback coefficient Yellow 0 [x10^6] */  
   D_SIF_YEL_AX_INPUT_T1,              /* Load cell feedback coefficient Yellow 1 [x10^6] */  
   D_SIF_YEL_AX_INPUT_T2,              /* Load cell feedback coefficient Yellow 2 [x10^6] */  
   D_SIF_YEL_AX_INPUT_T3,              /* Load cell feedback coefficient Yellow 3 [x10^6] */  
   D_SIF_YEL_AX_INPUT_T4,              /* Load cell feedback coefficient Yellow 4 [x10^6] */  
   D_SIF_YEL_AX_INPUT_T5,              /* Load cell feedback coefficient Yellow 5 [x10^6] */  
                           
   D_SIF_BLU_AX_INPUT_T0,              /* Load cell feedback coefficient Blue 0 [x10^6] */  
   D_SIF_BLU_AX_INPUT_T1,              /* Load cell feedback coefficient Blue 1 [x10^6] */  
   D_SIF_BLU_AX_INPUT_T2,              /* Load cell feedback coefficient Blue 2 [x10^6] */  
   D_SIF_BLU_AX_INPUT_T3,              /* Load cell feedback coefficient Blue 3 [x10^6] */  
   D_SIF_BLU_AX_INPUT_T4,              /* Load cell feedback coefficient Blue 4 [x10^6] */  
   D_SIF_BLU_AX_INPUT_T5,              /* Load cell feedback coefficient Blue 5 [x10^6] */  
          
   /* Lateral control model coefficients. */              
   D_SIF_LAT_CTRL_T0,                  /* Lateral control model coefficients 0 [x10^6] */
   D_SIF_LAT_CTRL_T1,                  /* Lateral control model coefficients 1 [x10^6] */  
   D_SIF_LAT_CTRL_T2,                  /* Lateral control model coefficients 2 [x10^6] */  

   /* Adaptive PID controller parameters. */
   D_SIF_ADAPTIVE_PID_ENABLE,          /* Enable adaptive PID controller.      */
   D_SIF_RED_ADAP_CTRL,                /* Adaptive PID Control coefficient Red */ 
   D_SIF_RED_PROP_GAIN_MIN,            /* Proportional gain minimum Red        */
   D_SIF_RED_PROP_GAIN_MAX,            /* Proportional gain maximum Red        */
   D_SIF_RED_INT_GAIN_MIN,             /* Integral gain minimum Red            */
   D_SIF_RED_INT_GAIN_MAX,             /* Integral gain maximum Red            */
   D_SIF_RED_DERIV_GAIN,               /* Derivative gain fixed Red            */
   D_SIF_RED_LEARNING,                 /* Learning rate Red                    */
   D_SIF_RED_ERR_THRES,                /* Error treshold for learning Red      */
   D_SIF_YEL_ADAP_CTRL,                /* Adaptive PID Control coefficient Yel */ 
   D_SIF_YEL_PROP_GAIN_MIN,            /* Proportional gain minimum Yel        */
   D_SIF_YEL_PROP_GAIN_MAX,            /* Proportional gain maximum Yel        */
   D_SIF_YEL_INT_GAIN_MIN,             /* Integral gain minimum Yel            */
   D_SIF_YEL_INT_GAIN_MAX,             /* Integral gain maximum Yel            */
   D_SIF_YEL_DERIV_GAIN,               /* Derivative gain fixed Yel            */
   D_SIF_YEL_LEARNING,                 /* Learning rate Yel                    */
   D_SIF_YEL_ERR_THRES,                /* Error treshold for learning Yel      */
   D_SIF_BLU_ADAP_CTRL,                /* Adaptive PID Control coefficient Blu */
   D_SIF_BLU_PROP_GAIN_MIN,            /* Proportional gain minimum Blu        */
   D_SIF_BLU_PROP_GAIN_MAX,            /* Proportional gain maximum Blu        */
   D_SIF_BLU_INT_GAIN_MIN,             /* Integral gain minimum Blu            */
   D_SIF_BLU_INT_GAIN_MAX,             /* Integral gain maximum Blu            */
   D_SIF_BLU_DERIV_GAIN,               /* Derivative gain fixed Blu            */
   D_SIF_BLU_LEARNING,                 /* Learning rate Blu                    */
   D_SIF_BLU_ERR_THRES,                /* Error treshold for learning Blu      */

   /* Inclinometer noise rejection filter coefficients */
   D_SIF_INCLIN_FILTER_IN_T0,     /* Inclinometer noise rejection filter input coefficient 0 [x10^6] */  
   D_SIF_INCLIN_FILTER_IN_T1,     /* Inclinometer noise rejection filter input coefficient 1 [x10^6] */     
   D_SIF_INCLIN_FILTER_OUT,       /* Inclinometer noise rejection filter output coefficient  [x10^6] */ 

   D_SIF_LAT_FEED_FWD,            /* Voltage offset (mV) to apply to lateral support to support */
   D_SIF_LAT_PRELOAD_AMP,         /* Lateral preload amplitude */
                        
   D_SIF_DATAID_EOL                    /* End of list marker. */                   

} eSifDataId_t;

/* Define Support Mechanisms */
#define ASM      0x0040       /* _MKV these may need adding to system package Id's */
#define LSM      0x0041       /* depending on how code pans out, poss split */

#endif  /* SIF_H_DEFINED */                                                    

/*******************************************************************************
** End of File Name: Sif.h
*******************************************************************************/
