/********************************************************************************
** Module Name:
**    Spt.h
**
** Purpose:
**    Public header file for Services PLC Task.
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    SPT design and implementation document v0.01 05-Jan-2001.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: Spt.h,v 0.57 2007/12/04 10:11:14 mjf Exp $
**
** History:
**    $Log: Spt.h,v $
**    Revision 0.57  2007/12/04 10:11:14  mjf
**    Mirror cover actuators must have Ops A & B, even if unused.
**
**    Revision 0.56  2007/11/09 15:15:26  mjf
**    Addition of individual mirror cover sensors to T2-010 SPLC ICD.
**
**    Revision 0.55  2007/07/17 14:28:09  mjf
**    Re-use spare temeprature sensors for the difference between oil and
**    ambient and for the optional well ambient temperature sensor.
**    Addition of power status for Basebox Power Switch and Chiller.
**    Re-use Pump Room and IT Rooms lights commands for Basebox Power Switch
**    and Chiller Power feed power respectively.
**
**    Revision 0.54  2007/03/07 16:16:36  mjf
**    Add 'BatteryLow' bit into PLC Status register. Rename Safe state to
**    Inactive, Active state to Activated and tweak colours for ECI display.
**
**    Revision 0.53  2007/01/24 13:59:40  mjf
**    Comments clarify identificatinn of HLPC pressure transmitters.
**
**    Revision 0.52  2006/12/14 15:49:45  mjf
**    Addition of 5 x 2 trapped key Bc positions.
**
**    Revision 0.51  2006/12/11 15:04:53  mjf
**    Use T2-408 comments for commands rather than (unused) T2-010 ones.
**
**    Revision 0.50  2006/10/11 21:37:49  mjf
**    Re-use redundant PLC ladder version data for alarm table and spares,
**    in T2-010 SPLC.
**
**    Revision 0.49  2006/10/11 21:28:13  mjf
**    Re-use PLC Program File versions for SPLC watchdog and healthy tallies,
**    in T2-010 SPLC.
**
**    Revision 0.48  2006/10/11 14:25:38  mjf
**    Display E-stop status correctly.
**
**    Revision 0.47  2006/10/03 11:23:54  sxf
**    Change % to percent to avoid confusing Htg.
**
**    Revision 0.46  2004/10/06 11:05:15  mjf
**    Notation changed for display of actuator closed. Datum descriptions added.
**
**    Revision 0.45  2004/09/20 10:58:30  mjf
**    Addition of datum entries for Panel 55 MCB and temperature monitoring.
**
**    Revision 0.44  2004/06/25 14:14:47  mjf
**    Compliance to SPLC/MCS ICD v0.13.
**
**    Revision 0.43  2004/06/10 18:14:26  mjf
**    Minor textual changes for HPLC comms loss and trapped key status.
**
**    Revision 0.42  2004/05/20 09:20:38  mjf
**    Implementation of re-used commands for T2-408 SPLC as an enumerated
**    list to facilitate HTI lookup of datum name.
**
**    Revision 0.41  2004/05/14 14:22:58  mjf
**    Addition of status data retrieved from T2-408 PLC, in accordance with
**    ICD v0.11 (except using N70:10 for combined E-stop not enclosure
**    doors).
**
**    Revision 0.40  2004/05/12 14:22:09  mjf
**    Addition of datum entries for additional data in T2-408 ICD v0.11.
**
**    Revision 0.39  2004/05/06 08:51:49  mjf
**    Addition of functionality for T2-408 Services PLC command interface.
**    Remains backwardly-compatible with previous Spt.h.
**
**    Revision 0.38  2003/12/11 09:57:13  mjf
**    Change of ordering for PLC System Status shutdowns, and enumeration
**    for these added.
**
**    Revision 0.37  2003/12/10 11:48:07  mjf
**    Addition of PLC System Status for Controlled Shutdown, Immediate
**    Shutdown and E-Stop Shutdown.
**
**    Revision 0.36  2003/11/26 12:09:27  mjf
**    Addition of enumeration of PLC alarm levels, and addition of new
**    datum D_SPT_PLC_ALARM_LEVEL to store overall alarm level.
**
**    Revision 0.35  2003/09/18 11:21:20  mjf
**    Minor changes from Services PLC ICD v3.4. Four commands removed, and
**    four commands added. Datum slots reused to maintain backwards
**    compatibility.
**
**    Revision 0.34  2003/08/08 09:15:10  mjf
**    Correction to interpretation of PLC system status.
**
**    Revision 0.33  2003/08/06 15:21:02  mjf
**    Addition of PLC system status.
**
**    Revision 0.32  2003/05/30 10:05:36  mjf
**    Change of wording of 'DeviceStatus' to 'ProgramType'.
**
**    Revision 0.31  2003/05/23 13:48:42  mjf
**    Removal of 'Display Sense' for PLC status data. Addition of text for
**    PLC Status. Renaming of DMD_FLT to DMD_FLT_RESET, and replacing of
**    DMD_ESTOP with DMD_AXIS_RESET. Addition of version status and numbers
**    for HMI and PFs 02-47.
**
**    Revision 0.30  2003/05/13 11:29:22  mjf
**    Additions to Sensor device - Out of Limits, Out of Range and Out of
**    Limits Mask.
**
**    Revision 0.29  2003/05/07 12:08:19  mjf
**    Addition of 'disconnected' bits for motors.
**
**    Revision 0.28  2003/05/03 13:31:59  mjf
**    Addition of some more data, from ICD v3.2 (revised).
**
**    Revision 0.27  2003/05/02 17:56:03  mjf
**    Some device text information (for colours) added for testing. May
**    need thorough revision later.
**
**    Revision 0.26  2003/04/24 11:28:37  mjf
**    Corrections to specifying device text and (colour) coding thereof.
**
**    Revision 0.25  2003/04/11 15:50:27  mjf
**    Initial attempt at encoding output display properties for use by
**    client applications. The display properties particular for each
**    item have yet to be assigned.
**
**    Revision 0.24  2003/04/11 12:30:03  mjf
**    Removal of spurious comma in text definitions.
**
**    Revision 0.23  2003/04/11 11:07:53  mjf
**    Updated in line with Services PLC v3.2 (subject to addition of overall
**    states x 4, and revision numbers packed into a single word).
**
**    Revision 0.22  2003/04/10 12:34:01  mjf
**    Updated in accordance with the changes (but not additions) in Services
**    PLC ICD 3.1, plus additions of overall states for Oil System, Air
**    System, Limits & Interlocks, and Enclosure.
**
**    Revision 0.21  2002/09/12 15:16:43  mjf
**    Correction to some datum names.
**
**    Revision 0.20  2002/09/12 14:54:22  mjf
**    Addition of general device type for alarm detection.
**
**    Revision 0.19  2002/08/13 17:00:26  mjf
**    Correction to PLC safe and activate demands.
**
**    Revision 0.18  2002/08/13 13:08:31  mjf
**    Addition of type for actuators.
**
**    Revision 0.17  2002/08/13 08:07:53  mjf
**    Make bit fields unsigned integers rather than signed.
**
**    Revision 0.16  2002/08/12 17:46:12  mjf
**    Implementation of lookup tables to export text to display.
**
**    Revision 0.15  2002/08/06 18:37:39  mjf
**    Initial overhaul in line with Services PLC ICD v3.0.
**
**    Revision 0.14  2002/01/24 10:34:26  mjf
**    Addition of error on SDB submission.
**
**    Revision 0.13  2002/01/10 09:08:40  mjf
**    Change to handling of PLC low battery and processor state.
**
**    Revision 0.12  2002/01/09 09:50:21  mjf
**    Change driven by Services PLC ICD v2.8.
**
**    Revision 0.11  2002/01/07 17:17:00  mjf
**    Extensive changes driven by the recent changes to the EPT (sister) code.
**    Changes to datum ID list : 'this application' and 'simulate' added, and
**    the demands reordered to be after the status data. Capability for
**    software simulation of Services PLC added. Improved handling of PLC
**    status data obtained. Datum IDs in the configuration file no longer
**    require the 'D_SPT_' prefix. SPT process states implemented as macros.
**    Obtain status data from PLC on each incoming heartbeat. OID table now
**    contains an 'error' state (if unable to read from PLC), and a 'default'
**    state (if no bit is set in the PLC) for state type data. Fix to
**    error response messages implemented. Use the definitions for telescopes
**    in 'TtlSystem.h'.
**
**    Revision 0.10  2001/06/22 13:30:09  ljs
**    Added error code for immediate shutdown condition.
**
**    Revision 0.9  2001/06/20 13:14:13  ljs
**    Added UPS OID.
**
**    Revision 0.8  2001/06/20 08:18:05  ljs
**    Removed homed commands, added a few missing OIDs.
**
**    Revision 0.7  2001/06/14 10:31:01  ljs
**    Added new error messages.
**
**    Revision 0.6  2001/06/12 15:19:34  ljs
**    Added list of process state priorities for use in
**    process state modelling.
**
**    Revision 0.5  2001/06/11 12:45:59  ljs
**    Added DatumIds per MCS/SPLC?ICD v 2.6.
**
**    Revision 0.4  2001/06/08 11:31:56  ljs
**    Updated to resemble EPT, all changes implemented but not
**    yet tested. Implemented UNSUPPORTED_UNITS, tidied code,
**    process SAFE and ACTIVATE correctly add config file handling
**    and new Df1Write function.
**
**    Revision 0.3  2001/02/12 09:16:43  asp
**    Demands now submitted to SDB
**
**    Revision 0.2  2001/01/10 13:42:11  asp
**    Read/Write failures added. CMD changed to DMD. Typo's corrected.
**
**    Revision 0.1  2001/01/05 15:07:05  asp
**    Initial Version
**
**
*******************************************************************************/


#ifndef SPT_H_DEFINED
#define SPT_H_DEFINED


/*
** Compiler include files
*/


/*
** System include files
*/

#include "TtlSystem.h"
#include "Mcp.h"
#include "Sdb.h"


/****************************************************************************** 
** 
** The SPT is used to interface to the T2-010 Services PLC. The SPI is compiled
** from the SPT code-base with the definition '__SPI__' to produce an SPI 
** executable to interface to the T2-408 Services PLC (note there is one public 
** header 'Spt.h' which is identical for both).
**
** The intention is to allow backward-compatibility for applications 
** interfacing to the SPT/SPI for the common elements of the interface. To 
** achieve this there are a small number of SPT commands that have been re-used
** for the SPI, but these are commands that were not used in the T2-010 PLC :-
** - Switch Trace Heating On/Off     -> Select Cassegrain rotator
** - Switch Scavenge Pumpset On/Off  -> Select Nasmyth Plus rotator
** - Switch Remote I/O Panel On/Off  -> Select Nasmyth Minus rotator
** - Switch Cryotiger 1 Power On/Off -> Switch Azimuth Brake Power On/Off
** - Switch Cryotiger 2 Power On/Off -> Switch Mirror Restraint Power On/Off
** - Switch Basebox Pressure Sensors On/Off -> HPLC Mute Alarm
** All other commands are in the same position in the datum list, hence 
** allowing back-compatibility.
** 
** The status data retrieval...
**
**
**
**
******************************************************************************/ 


/*
** Macro definitions
*/

/* This definition declares the device text name look-up table :-             */
/*  #define E_SPT_DEVICE_TEXT_DECLARE                                         */
/* This definition declares the device text name look-up table as 'extern' :- */
/*  #define E_SPT_DEVICE_TEXT_EXTERN                                          */

/* Size of messages (based on CIL classes) sent when commands received */

#define E_SPT_ACT_LEN_DMD    0
#define E_SPT_RSP_LEN_DMD    0
#define E_SPT_COM_LEN_DMD    sizeof( Int32_t )

/* Number of times to retry interface in event of coms failure. */

#define E_SPT_COMS_RETRIES   5

/* Bounds for indexing into text look-up tables. */

#define E_SPT_TEXT_ROWS      32
#define E_SPT_TEXT_COLS      4

/* Length of string for state name text. */

#define E_SPT_TEXT_LEN       32


/*
** Type definitions
*/

/* Enumeration of PLC alarm levels */

typedef enum eSptAlarmLevel_e
{
   E_SPT_ALARM_NONE    = 0,  /* No alarms */
   E_SPT_ALARM_WARNING = 1,  /* Warning */
   E_SPT_ALARM_STAGE_2 = 2,  /* Stage 2 alarm */
   E_SPT_ALARM_STAGE_3 = 3,  /* Stage 3 alarm */
   E_SPT_ALARM_STAGE_4 = 4   /* Stage 4 alarm */
} eSptAlarmLevel_t;

/* Enumeration of PLC System Status */

typedef enum eSptSysStatus_e
{
   E_SPT_SYS_SAFE      = 0,  /* Safe State */
   E_SPT_SYS_ACTIVE       ,  /* Active State */
   E_SPT_SYS_PWR_UP       ,  /* Powering Up */
   E_SPT_SYS_PWR_DOWN     ,  /* Powering Down */
   E_SPT_SYS_PWR_RUNNING  ,  /* Power Running */
   E_SPT_SYS_HYD_START    ,  /* Hydrostatics Starting */
   E_SPT_SYS_HYD_STOP     ,  /* Hydrostatics Stopping */
   E_SPT_SYS_HYD_RUNNING  ,  /* Hydrostatics Running */
   E_SPT_SYS_OBS_START    ,  /* Observations Starting */
   E_SPT_SYS_OBS_STOP     ,  /* Observations Stopping */
   E_SPT_SYS_READY        ,  /* System Ready */
   E_SPT_SYS_CONTR_SHUT   ,  /* Controlled Shutdown */
   E_SPT_SYS_IMMED_SHUT   ,  /* Immediate Shutdown */
   E_SPT_SYS_ESTOP_SHUT   ,  /* E-Stop Shutdown */
   E_SPT_SYS_ERROR           /* System Error */
} eSptSysStatus_t;

/* Enumeration of HPLC System Status */

typedef enum eSptHplcStatus_e
{
   E_SPT_HPLC_SAFE     = 0,  /* Safe State */
   E_SPT_HPLC_ACTIVE      ,  /* Active State */
   E_SPT_HPLC_COOLING     ,  /* Cooling Unit Running */
   E_SPT_HPLC_CONDITIONING,  /* Conditioning Oil Tank */
   E_SPT_HPLC_PRESSURISING,  /* Pressurising Oil Lines */
   E_SPT_HPLC_PREPARING   ,  /* Preparing Bearing System */
   E_SPT_HPLC_BEARINGS_OK ,  /* Bearing System OK */
   E_SPT_HPLC_ERROR          /* System Error */
} eSptHplcStatus_t;

/* Enumeration for the methods of indexing device text */

typedef enum eSptTextIndex_e
{
   E_SPT_INDEX_BOL,          /* Beginning-of-list */

   E_SPT_INDEX_TEXT_VALUE,   /* Index using text and value reported */
   E_SPT_INDEX_TEXT_BIT,     /* Index using text and bit number */
   E_SPT_INDEX_BIT_VALUE,    /* Index using bit and value reported */
   E_SPT_INDEX_VALUE_BIT,    /* Index using value and bit number */
   E_SPT_INDEX_VALUE_TEXT,   /* Index using value and text */

   E_SPT_INDEX_EOL           /* End-of-list */
} eSptTextIndex_t;

/* Enumeration for the methods of displaying device text */

typedef enum eSptTextOutput_e
{
   E_SPT_OP_BOL,             /* Beginning-of-list */

   E_SPT_OP_NO_DATA,    
   E_SPT_OP_INACTIVE,   
   E_SPT_OP_OFF,        
   E_SPT_OP_INIT,       
   E_SPT_OP_OKAY,       
   E_SPT_OP_SAFE,       
   E_SPT_OP_NEUTRAL,    
   E_SPT_OP_NOTE,       
   E_SPT_OP_ACTION,     
   E_SPT_OP_SUBNOM,     
   E_SPT_OP_WARN,       
   E_SPT_OP_STANDBY,    
   E_SPT_OP_FAULT,      
   E_SPT_OP_FAILED,     
   E_SPT_OP_DANGER,     

   E_SPT_OP_EOL              /* End-of-list */
} eSptTextOutput_t;

/* Type for general device - for alarm warning status only */

typedef struct
{
   unsigned _ReservedA       : 12;
   unsigned Warning          : 1;
   unsigned Stage2Alarm      : 1;
   unsigned Stage3Alarm      : 1;
   unsigned Stage4Alarm      : 1;
} eSptDeviceGeneral_t;

/* Type for 2-state device */

typedef struct
{
   unsigned DeviceStatus     : 1;
   unsigned _ReservedA       : 1;
   unsigned Masked           : 1;
   unsigned _ReservedB       : 1;
   unsigned DisplayText      : 5;
   unsigned _ReservedC       : 3;
   unsigned Warning          : 1;
   unsigned Stage2Alarm      : 1;
   unsigned Stage3Alarm      : 1;
   unsigned Stage4Alarm      : 1;
} eSptDevice2State_t;

/* Type for 4-state device */

typedef struct
{
   unsigned DeviceStatus     : 2;
   unsigned Masked           : 1;
   unsigned _ReservedA       : 1;
   unsigned DisplayText      : 4;
   unsigned _ReservedB       : 4;
   unsigned Warning          : 1;
   unsigned Stage2Alarm      : 1;
   unsigned Stage3Alarm      : 1;
   unsigned Stage4Alarm      : 1;
} eSptDevice4State_t;       

/* Type for limit device */

typedef struct
{
   unsigned L3A              : 1;
   unsigned L2A              : 1;
   unsigned L2B              : 1;
   unsigned L3B              : 1;
   unsigned L3AMasked        : 1;
   unsigned L2AMasked        : 1;
   unsigned L2BMasked        : 1;
   unsigned L3BMasked        : 1;
   unsigned DisplayText      : 2;
   unsigned _ReservedA       : 2;
   unsigned Warning          : 1;
   unsigned Stage2Alarm      : 1;
   unsigned Stage3Alarm      : 1;
   unsigned Stage4Alarm      : 1;
} eSptDeviceLimit_t;

/* Type for sensor device */
                          
typedef struct            
{                         
   unsigned LowLow           : 1;
   unsigned Low              : 1;
   unsigned High             : 1;
   unsigned HighHigh         : 1;
   unsigned OutOfLimits      : 1;
   unsigned OutOfRange       : 1;
   unsigned LowLowMasked     : 1;
   unsigned LowMasked        : 1;
   unsigned HighMasked       : 1;
   unsigned HighHighMasked   : 1;
   unsigned OutOfLimitsMask  : 1;
   unsigned Invalid          : 1;
   unsigned Warning          : 1;
   unsigned Stage2Alarm      : 1;
   unsigned Stage3Alarm      : 1;
   unsigned Stage4Alarm      : 1;
} eSptDeviceSensor_t;

/* Type for motor device */

typedef struct
{
   unsigned Running          : 1;
   unsigned Tripped          : 1;
   unsigned Fault            : 1;
   unsigned Reversed         : 1;
   unsigned Interlocked      : 1;
   unsigned Disconnected     : 1;
   unsigned _ReservedA       : 4;
   unsigned Masked           : 1;
   unsigned Invalid          : 1;
   unsigned Warning          : 1;
   unsigned Stage2Alarm      : 1;
   unsigned Stage3Alarm      : 1;
   unsigned Stage4Alarm      : 1;
} eSptDeviceMotor_t;      

/* Type for actuator device */

typedef struct
{
   unsigned Opened           : 1;
   unsigned Closed           : 1;
   unsigned Transit          : 1;
   unsigned Tripped          : 1;
   unsigned Fault            : 1;
   unsigned Interlocked      : 1;
   unsigned _ReservedA       : 4;
   unsigned Masked           : 1;
   unsigned Invalid          : 1;
   unsigned Warning          : 1;
   unsigned Stage2Alarm      : 1;
   unsigned Stage3Alarm      : 1;
   unsigned Stage4Alarm      : 1;
} eSptDeviceActuator_t;      

/* Type for PLC Revision Status */

typedef struct
{
   unsigned ProgramType      : 2;
   unsigned InUse            : 1;
   unsigned Enabled          : 1;
   unsigned CodeCategory     : 3;
   unsigned _ReservedA       : 9;
} eSptDevicePLCRevStat_t;

/* Type for PLC Revision Number */

typedef struct
{
   unsigned MinorRevision    : 8;
   unsigned MajorRevision    : 8;
} eSptDevicePLCRevNum_t;

/* Type for PLC status */

typedef struct
{
   unsigned ProcessorMode    : 5;
   unsigned ForcesEnabled    : 1;
   unsigned ForcesPresent    : 1;
   unsigned ReplaceBattery   : 1;
   unsigned OngoingBattery   : 1;
   unsigned _ReservedA       : 4;
   unsigned Fault            : 1;
   unsigned ProgAccessDenied : 1;
   unsigned BatteryLow       : 1;
} eSptDevicePLCStatus_t;

/* Type for PLC System Status */

typedef struct
{
   unsigned PLCSystemStatus;
} eSptDevicePLCSysStat_t;

/* Type for specifying device-type characteristics */

typedef struct
{
   char     DeviceText[ E_SPT_TEXT_LEN ];
   eSptTextOutput_t DeviceOutput;
} eSptDeviceTypeSpec_t;

/* Type for defining display text for device types */

typedef struct
{
   eSptTextIndex_t TextIndex;
   eSptDeviceTypeSpec_t Spec[ E_SPT_TEXT_ROWS ][ E_SPT_TEXT_COLS ];
} eSptDeviceSpec_t;


/*
** Variables Declarations
*/

/* Text for displaying Services PLC states - optionally included */

#ifdef E_SPT_DEVICE_TEXT_DECLARE

eSptDeviceSpec_t eSptDeviceSpec[ E_SDB_PLC_EOL_UNITS - E_SDB_PLC_BOL_UNITS ] =
{ /* BOL */
  { E_SPT_INDEX_BOL,
    { { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 1 - 2-state device */
  { E_SPT_INDEX_TEXT_VALUE,
    { { "Inactive"      , E_SPT_OP_OFF     , "Activated"     , E_SPT_OP_STANDBY , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "False"         , E_SPT_OP_OFF     , "True"          , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Not OK"        , E_SPT_OP_FAULT   , "OK"            , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Off"           , E_SPT_OP_OFF     , "On"            , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Stopped"       , E_SPT_OP_OFF     , "Running"       , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Closed"        , E_SPT_OP_OFF     , "Open"          , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Fault"         , E_SPT_OP_FAULT   , "Healthy"       , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Standby"       , E_SPT_OP_STANDBY , "Duty"          , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Assist"        , E_SPT_OP_STANDBY , "Duty"          , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Manual"        , E_SPT_OP_STANDBY , "Auto"          , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Local"         , E_SPT_OP_NOTE    , "Remote"        , E_SPT_OP_ACTION  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Invalid"       , E_SPT_OP_INACTIVE, "Valid"         , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_SUBNOM  , "Out-of-range"  , E_SPT_OP_FAILED  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_SUBNOM  , "Override"      , E_SPT_OP_NOTE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_SUBNOM  , "Alarm"         , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Clear"         , E_SPT_OP_SUBNOM  , "Blocked"       , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Operated"      , E_SPT_OP_SUBNOM  , "Healthy"       , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Fault"         , E_SPT_OP_FAULT   , "Normal"        , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_SUBNOM  , "Fault"         , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_SUBNOM  , "Low"           , E_SPT_OP_WARN    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_SUBNOM  , "High"          , E_SPT_OP_WARN    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Tripped"       , E_SPT_OP_FAULT   , "Healthy"       , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "In Service"    , E_SPT_OP_SUBNOM  , "Blocked"       , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Operated"      , E_SPT_OP_DANGER  , "Healthy"       , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Operated"      , E_SPT_OP_DANGER  , "Operated"      , E_SPT_OP_DANGER  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 2 - 4-state device */
  { E_SPT_INDEX_TEXT_VALUE,
    { { "Off"           , E_SPT_OP_OFF     , "On"            , E_SPT_OP_SUBNOM  , "Healthy"       , E_SPT_OP_OKAY    , "Fault"         , E_SPT_OP_FAULT },
      { "Stopped"       , E_SPT_OP_OFF     , "Running"       , E_SPT_OP_SUBNOM  , "Healthy"       , E_SPT_OP_OKAY    , "Fault"         , E_SPT_OP_FAULT },
      { "Closed"        , E_SPT_OP_OFF     , "Opened"        , E_SPT_OP_SUBNOM  , "Transit"       , E_SPT_OP_ACTION  , "Fault"         , E_SPT_OP_FAULT },
      { "Closed"        , E_SPT_OP_OFF     , "Opened"        , E_SPT_OP_SUBNOM  , "Transit"       , E_SPT_OP_ACTION  , "Override"      , E_SPT_OP_NOTE },
      { "Standby"       , E_SPT_OP_STANDBY , "Duty"          , E_SPT_OP_NEUTRAL , "Off"           , E_SPT_OP_OFF     , "Invalid"       , E_SPT_OP_INACTIVE },
      { "Duty 1"        , E_SPT_OP_NEUTRAL , "Duty 2"        , E_SPT_OP_NEUTRAL , "Off"           , E_SPT_OP_OFF     , "Invalid"       , E_SPT_OP_INACTIVE },
      { "Free"          , E_SPT_OP_NEUTRAL , "Trapped"       , E_SPT_OP_SUBNOM  , "Override"      , E_SPT_OP_NOTE    , "Invalid"       , E_SPT_OP_INACTIVE },
      { "Assist"        , E_SPT_OP_NOTE    , "Duty"          , E_SPT_OP_NEUTRAL , "Off"           , E_SPT_OP_OFF     , "Invalid"       , E_SPT_OP_INACTIVE },
      { "Assist"        , E_SPT_OP_NOTE    , "Duty 1"        , E_SPT_OP_NEUTRAL , "Duty 2"        , E_SPT_OP_NEUTRAL , "Invalid"       , E_SPT_OP_INACTIVE },
      { "Manual"        , E_SPT_OP_NOTE    , "Auto"          , E_SPT_OP_SUBNOM  , "Off"           , E_SPT_OP_OFF     , "Invalid"       , E_SPT_OP_INACTIVE },
      { "Local"         , E_SPT_OP_NOTE    , "Remote"        , E_SPT_OP_ACTION  , "Off"           , E_SPT_OP_OFF     , "Invalid"       , E_SPT_OP_INACTIVE },
      { "Normal"        , E_SPT_OP_SUBNOM  , "Override"      , E_SPT_OP_NOTE    , "Healthy"       , E_SPT_OP_OKAY    , "Fault"         , E_SPT_OP_FAULT },
      { "Connected"     , E_SPT_OP_SUBNOM  , "Disconnected"  , E_SPT_OP_INACTIVE, "Healthy"       , E_SPT_OP_OKAY    , "Tripped"       , E_SPT_OP_FAULT },
      { "Normal"        , E_SPT_OP_SUBNOM  , "High"          , E_SPT_OP_NEUTRAL , "Low"           , E_SPT_OP_NOTE    , "Out of Limits" , E_SPT_OP_FAULT },
      { "Healthy"       , E_SPT_OP_OKAY    , "Valid"         , E_SPT_OP_SUBNOM  , "Out of Range"  , E_SPT_OP_FAILED  , "Operated"      , E_SPT_OP_SUBNOM },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 3 - Limit device */
  { E_SPT_INDEX_TEXT_BIT,
    { { "Clockwise L3"  , E_SPT_OP_STANDBY , "Clockwise L2"  , E_SPT_OP_WARN    , "Anticl/wise L2", E_SPT_OP_WARN    , "Anticl/wise L3", E_SPT_OP_STANDBY },
      { "Low L3"        , E_SPT_OP_STANDBY , "Low L2"        , E_SPT_OP_WARN    , "High L2"       , E_SPT_OP_WARN    , "High L3"       , E_SPT_OP_STANDBY },
      { "Undertravel L3", E_SPT_OP_STANDBY , "Undertravel L2", E_SPT_OP_WARN    , "Overtravel L2" , E_SPT_OP_WARN    , "Overtravel L3 ", E_SPT_OP_STANDBY },
      { "Zenith L3"     , E_SPT_OP_STANDBY , "Zenith L2"     , E_SPT_OP_WARN    , "Horizon L2"    , E_SPT_OP_WARN    , "Horizon L3"    , E_SPT_OP_STANDBY },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 4 - Sensor */
  { E_SPT_INDEX_BIT_VALUE,
    { { "Normal"        , E_SPT_OP_OKAY    , "Low-low"       , E_SPT_OP_WARN    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "Low"           , E_SPT_OP_WARN    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "High"          , E_SPT_OP_WARN    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "High-high"     , E_SPT_OP_WARN    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "Out of Limits" , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "Out of Range"  , E_SPT_OP_FAILED  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 5 - Motor */
  { E_SPT_INDEX_BIT_VALUE,
    { { "Stopped"       , E_SPT_OP_NEUTRAL , "Running"       , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Healthy"       , E_SPT_OP_OKAY    , "Tripped"       , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "Fault"         , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Forward"       , E_SPT_OP_NOTE    , "Reverse"       , E_SPT_OP_NEUTRAL , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "Interlocked"   , E_SPT_OP_STANDBY , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Connected"     , E_SPT_OP_OKAY    , "Disconnected"  , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 6 - Actuator */
  { E_SPT_INDEX_BIT_VALUE,
    { { "Not opened"    , E_SPT_OP_NEUTRAL , "Opened"        , E_SPT_OP_NOTE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Not closed"    , E_SPT_OP_NEUTRAL , "Closed"        , E_SPT_OP_NEUTRAL , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Not in-transit", E_SPT_OP_NEUTRAL , "In-transit"    , E_SPT_OP_ACTION  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Healthy"       , E_SPT_OP_OKAY    , "Tripped"       , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "Fault"         , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Normal"        , E_SPT_OP_OKAY    , "Interlocked"   , E_SPT_OP_STANDBY , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 7 - PLC Status */
  { E_SPT_INDEX_VALUE_TEXT,
    { { "Remote download" , E_SPT_OP_NOTE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Remote program"  , E_SPT_OP_ACTION  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Suspend/Idle"    , E_SPT_OP_OFF     , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Remote run"      , E_SPT_OP_WARN    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Remote test cont", E_SPT_OP_STANDBY , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Remote test scan", E_SPT_OP_STANDBY , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Remote test step", E_SPT_OP_STANDBY , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Download"        , E_SPT_OP_INIT    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Program"         , E_SPT_OP_INIT    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Suspend/Idle"    , E_SPT_OP_OFF     , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Run"             , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "invalid"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 8 - PLC Version */
  {  E_SPT_INDEX_VALUE_TEXT,
    { { "Main Ladder"   , E_SPT_OP_NOTE    , "Sub Ladder"    , E_SPT_OP_NEUTRAL , "HMI"           , E_SPT_OP_STANDBY , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  },
      { ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""             , E_SPT_OP_NO_DATA  }
    }
  },
  /* Type 9 - PLC System Status */
  { E_SPT_INDEX_VALUE_TEXT,
    { { "Inactive State"       , E_SPT_OP_OFF     , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Activated State"      , E_SPT_OP_STANDBY , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Powering Up"          , E_SPT_OP_INIT    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Powering Down"        , E_SPT_OP_NOTE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Power Running"        , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Hydrostatics Starting", E_SPT_OP_INIT    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Hydrostatics Stopping", E_SPT_OP_NOTE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Hydrostatics Running" , E_SPT_OP_NEUTRAL , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Observations Starting", E_SPT_OP_INIT    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Observations Stopping", E_SPT_OP_NOTE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "System Ready"         , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Controlled Shutdown"  , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Immediate Shutdown"   , E_SPT_OP_FAILED  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "E-Stop Shutdown"      , E_SPT_OP_DANGER  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "System Error"         , E_SPT_OP_FAULT   , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 10 - HPLC System Status */
  { E_SPT_INDEX_VALUE_TEXT,
    { { "Safe State"           , E_SPT_OP_SAFE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Active State"         , E_SPT_OP_INACTIVE, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Cooling Unit Running" , E_SPT_OP_SUBNOM  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Conditioning Tank"    , E_SPT_OP_NEUTRAL , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Pressurising Lines"   , E_SPT_OP_NOTE    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Preparing Bearings"   , E_SPT_OP_ACTION  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Bearing System OK"    , E_SPT_OP_OKAY    , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { "Communication Failure", E_SPT_OP_FAILED  , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 11 - spare */
  { E_SPT_INDEX_VALUE_TEXT,
    { { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 12 - spare */
  { E_SPT_INDEX_VALUE_TEXT,
    { { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 13 - spare */
  { E_SPT_INDEX_VALUE_TEXT,
    { { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  },
  /* Type 14 - spare */
  { E_SPT_INDEX_VALUE_TEXT,
    { { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA },
      { ""              , E_SPT_OP_NO_DATA, ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA , ""              , E_SPT_OP_NO_DATA }
    }
  }
};

#endif

#ifdef E_SPT_DEVICE_TEXT_EXTERN

extern eSptDeviceSpec_t eSptDeviceSpec[ E_SDB_PLC_EOL_UNITS - E_SDB_PLC_BOL_UNITS ];

#endif


/* Enumerated list of status and error values */
typedef enum eSptStatus_e
{
   E_SPT_GEN_ERR = STATUS_START(SPT),  /* Miscellaneous error */

   E_SPT_ERR_SETUP,          /* Error performing setup */
   E_SPT_UNKNOWN_SERVICE,    /* Requested service not provided by this task */
   E_SPT_SERVICE_NOT_IMPL,   /* Service not implemented in this version */
   E_SPT_NOT_IMPLEMENTED,    /* General functionality not implemented */
   E_SPT_MSG_LEN_ERR,        /* Incorrect no. bytes in CIL data block */
   E_SPT_UNEXPECTED_MSG,     /* Unexpected CIL message received */
   E_SPT_PARAM_RANGE_ERR,    /* Specified parameter outside allowed range */
   E_SPT_NO_HEARTBEATS,      /* No heartbeats received recently */
   E_SPT_DMD_NOT_PERMITTED,  /* Command not permitted because state not OKAY */
   E_SPT_PARAM_SIZE_ERR,     /* A value to be set is not of a supported size */
   E_SPT_NO_TIMED_DMDS,      /* Timed set commands are not supported */
   E_SPT_UNKNOWN_OID,        /* Object ID not recognised */
   E_SPT_DMD_PENDING,        /* An operation is already pending */
   E_SPT_WRITE_FAILED,       /* Write to PLC failed */
   E_SPT_READ_FAILED,        /* Read to PLC failed */
   E_SPT_PLC_INVALID,        /* More than one bit set in PLC invalid state. */
   E_SPT_CONFIG_FILE_ERROR,  /* Error found in Config file format. */
   E_SPT_INVALID_DATUM,      /* Datum is not in rage of eSptDataId_t. */
   E_SPT_NO_TIMED_CMDS,      /* Set 1 commands must have 0 timestamps. */
   E_SPT_UNKNOWN_DEMAND,     /* Received Demand Oid is unknown/invalid. */
   E_SPT_PARSE_ERROR,        /* Parsing of string failed; incorrect format. */
   E_SPT_UNKNOWN_STATE,      /* Value not in range for this OID. */
   E_SPT_IMMED_SDOWN,        /* Immediate shutdown requested, HW problem. */
   E_SPT_SW_SIMULATE,        /* Using software to simulate PLC operation */
   E_SPT_UNEXPECTED_SIM,     /* Simulate fn. called without simulate selected */
   E_SPT_ERR_SDB_SUBMIT,     /* Error performing SDB submission */

   E_SPT_STATUS_EOL          /* End of status list marker (do not use) */
} eSptStatus_t;


/* 
** These are the enumerated values that commands can be set to
** For the SPT these are bit values and are Clear (Disabled) or Set (Enabled)
*/
typedef enum eSptDmdState_e
{
   E_SPT_DISABLE          = 0,              /* Generic disable */ 
   E_SPT_ENABLE           = 1,              /* Generic enable  */
   E_SPT_DMD_ANALOG       = 3,              /* Invalid demand state  */
   E_SPT_DMD_STOP         = 4,              /* Invalid demand state  */
   E_SPT_DMD_STS_INVALID  = 5,              /* Invalid demand state  */

   E_SPT_DMD_STS_ACTD    = E_SPT_DISABLE,   /* Activated  */
   E_SPT_DMD_STS_SAFE    = E_SPT_ENABLE,    /* Safe state */

   E_SPT_DMD_STS_ON      = E_SPT_ENABLE,    /* On */
   E_SPT_DMD_STS_OFF     = E_SPT_DISABLE,   /* Off */

   E_SPT_DMD_STS_START   = E_SPT_ENABLE,    /* Start */
   E_SPT_DMD_STS_STOP    = E_SPT_DISABLE,   /* Stop */

   E_SPT_DMD_STS_OPN     = E_SPT_ENABLE,    /* Open */
   E_SPT_DMD_STS_CLS     = E_SPT_DISABLE,   /* Close */

   E_SPT_DMD_STS_RESET   = E_SPT_ENABLE     /* Reset */
} eSptDmdState_t;

/*
** These are the states that SPT datum can be set to (excluding analogue)
** Bit values read from the PLC are converted to one of these states 
*/
typedef enum eSptDataState_e
{
   /*
   ** State used to mark analogue data
   */
   E_SPT_STS_ANALOGUE,  /* Analogue - actual value will be submitted to Sdb  */ 

   /*
   ** Improper conditions where a state cannot be determined
   */
   E_SPT_STS_UNKNOWN,   /* No PLC state bits were found to be set */
   E_SPT_STS_AMBIGUOUS, /* More than 1 state bit was found to be set */

   /* 2-state devices. */
   E_SPT_STS_SAFE = 0,  /* Safe State */
   E_SPT_STS_ACTD = 1,  /* Activated */
   E_SPT_STS_OFF  = 0,  /* Off */
   E_SPT_STS_ON   = 1,  /* On */
   E_SPT_STS_LOC  = 0,  /* Local */
   E_SPT_STS_REM  = 1,  /* Remote */

   /* 4-state devices, actuators. */
   E_SPT_STS_CLS  = 0,  /* Closed */
   E_SPT_STS_OPN  = 1 , /* Open */
   E_SPT_STS_TRN  = 2 , /* Transit */
   E_SPT_STS_FLT  = 3 , /* Fault */

   E_SPT_STS_EOL

} eSptDataState_t;


/* SPT messages - services offered by the SPT */
typedef enum eSptService_e
{
   E_SPT_HEARTBEAT = E_MCP_HEARTBEAT,  /* Heartbeat command */
   E_SPT_SHUTDOWN  = E_MCP_SHUTDOWN,   /* Shutdown command */
   E_SPT_SAFESTATE = E_MCP_SAFESTATE,  /* Safestate command */
   E_SPT_ACTIVATE  = E_MCP_ACTIVATE,   /* Activate command */

   E_SPT_SET_1     = E_MCP_SET_1       /* Set a single control param */

} eSptService_t;



/* Global data for submission to the Status Database (SDB) */
typedef enum eSptDataId_e
{
   D_SPT_DATAID_BOL = 0,     /* Begining of data list */

   D_SPT_PROC_STATE          /* Process state datum */
      = D_MCP_PROC_STATE,
   D_SPT_AUTH_STATE          /* Requested/granted authorisation state */
      = D_MCP_AUTH_STATE,
   D_SPT_SYS_REQUEST         /* System requests made to MCP */
      = D_MCP_SYS_REQUEST,
   D_SPT_APP_VERSION         /* Application version number */
      = D_MCP_APP_VERSION,

   D_SPT_THIS_APPLICATION    /* Application package ID */
      = D_MCP_FIRST_USER_DATUM, 
   D_SPT_THIS_TELESCOPE   ,  /* Identity of this telescope */
   D_SPT_SW_SIMULATE      ,  /* Software simulate mode */

   /*
   ** SET_1 Demands that the SPT will action and respond to 
   ** These demands will be sumbitted to the SDB as they are made
   */
   D_SPT_DMD_BOL          ,  /* Beginning of demand list */

   D_SPT_DMD_OBS          ,  /* Observing Start/Stop */
   D_SPT_DMD_SAFE         ,  /* Switch Services PLC into Safe/Activated */
   D_SPT_DMD_HYD_SYS      ,  /* Full Hydraulic System Start/Stop */
   D_SPT_DMD_AZ_HYD       ,  /* Azimuth Hydraulic System Start/Stop */
   D_SPT_DMD_EL_HYD       ,  /* Elevation Hydraulic System Start/Stop */
   D_SPT_DMD_AIR_CMP      ,  /* Air Compressor On/Off */
   D_SPT_DMD_SW_THTG      ,  /* Select Cassegrain Rotator */
/* In SPI - D_SPT_DMD_SEL_CAS      ,     Select Cassegrain Rotator */
   D_SPT_DMD_SW_AGD       ,  /* Switch A & G Box Drives Power On/Off */
   D_SPT_DMD_SW_ALL_LTG   ,  /* Switch All Lights On/Off */
   D_SPT_DMD_SW_ENC_LTG   ,  /* Switch Enclosure Lights On/Off */
   D_SPT_DMD_SW_BBS_PWR   ,  /* Switch Basebox Power Switch On/Off */
   D_SPT_DMD_SW_WLA_LTG   ,  /* Switch Well Area Lights On/Off */
   D_SPT_DMD_SW_CHL_PWR   ,  /* Switch Chiller Power On/Off */
   D_SPT_DMD_MUTE_ALARM   ,  /* MCS Mute Alarm */
   D_SPT_DMD_SW_ALL_PWR   ,  /* Switch All Power On/Off */
   D_SPT_DMD_SW_PMC_PWR   ,  /* Switch Primary Mirror Cell Power On/Off */
   D_SPT_DMD_SW_MCO_PWR   ,  /* Switch Mirror Cover Power On/Off */
   D_SPT_DMD_SW_ACC_PWR   ,  /* Switch Autoguider Computer Power On/Off */
   D_SPT_DMD_SW_AZC_PWR   ,  /* Switch Azimuth Computer Power On/Off */
   D_SPT_DMD_SW_ELC_PWR   ,  /* Switch Elevation Computer Power On/Off */
   D_SPT_DMD_SW_CAC_PWR   ,  /* Switch Cassegrain Computer Power On/Off */
   D_SPT_DMD_SW_SCV_PWR   ,  /* Select Nasmyth Plus Rotator */
/* In SPI - D_SPT_DMD_SEL_NASP     ,     Select Nasmyth Plus Rotator */
   D_SPT_DMD_SW_RIO_PWR   ,  /* Select Nasmyth Minus Rotator */
/* In SPI - D_SPT_DMD_SEL_NASM     ,     Select Nasmyth Minus Rotator */
   D_SPT_DMD_SW_AIRCON    ,  /* Switch Ventilation Power On/Off */
   D_SPT_DMD_SW_NDS_PWR   ,  /* Switch Nasmyth Drive On/Off */
   D_SPT_DMD_SW_NWS_PWR   ,  /* Switch Nasmyth Wrap On/Off */
   D_SPT_DMD_SW_BBX_PWR   ,  /* Switch Basebox Power On/Off  */
   D_SPT_DMD_SW_CR1_PWR   ,  /* Switch Azimuth Brake Power On/Off */
/* In SPI - D_SPT_DMD_SW_AZB_PWR   ,     Switch Azimuth Brake Power On/Off */
   D_SPT_DMD_SW_AXD_PWR   ,  /* Switch Axis Drives Power On/Off */
   D_SPT_DMD_SW_SVR_PWR   ,  /* Switch Servo Valve Power On/Off */
   D_SPT_DMD_MIR_COV      ,  /* Mirror Cover Open/Close */
   D_SPT_DMD_FLT_RESET    ,  /* Fault Reset */
   D_SPT_DMD_AXIS_RESET   ,  /* Axis Reset */
   D_SPT_DMD_SW_SEC_PWR   ,  /* Switch Secondary Mirror Power On/Off */
   D_SPT_DMD_SW_TMP_PWR   ,  /* Switch Motor Temperature Panel Power On/Off */
   D_SPT_DMD_SW_CR2_PWR   ,  /* Switch Mirror Restraint Power On/Off */
/* In SPI - D_SPT_DMD_SW_PMR_PWR   ,     Switch Mirror Restraint Power On/Off */
   D_SPT_DMD_SW_BBL_PWR   ,  /* Switch Basebox Low Voltage Power On/Off */
   D_SPT_DMD_SW_BBF_PWR   ,  /* Switch Amplifier Fans On/Off */
   D_SPT_DMD_SW_BBP_PWR   ,  /* HPLC Mute Alarm */
/* In SPI - D_SPT_DMD_HPLC_MUTE    ,     HPLC Mute Alarm */

   /*
   ** These commands are internal to SPT process and should not be
   ** requested by any other process
   */
   D_SPT_DMD_QRY_STATUS   ,  /* Query Status */

   D_SPT_DMD_EOL          ,  /* End of demand list */

   /*
   ** Data values submitted to the SDB
   */
   D_SPT_STATUS_BOL       ,  /* Beginning of status list */

   D_SPT_AZM_WD           ,  /* Azimuth Node Watchdog */
   D_SPT_ALT_WD           ,  /* Altitude Node Watchdog */
   D_SPT_CAS_WD           ,  /* Cassegrain Node Watchdog */
   D_SPT_AMN_WD           ,  /* Auxiliary Mechanisms Node Watchdog */
   D_SPT_HYD_F1           ,  /* Hydrostatic Filter No 1 Pressure Switch */
   D_SPT_HYD_F2           ,  /* Hydrostatic Filter No 2 Pressure Switch */
   D_SPT_FA               ,  /* Fire Alarm */
   D_SPT_SA               ,  /* Security Alarm */
   D_SPT_DR_ILK           ,  /* Door Interlock Override */
   D_SPT_ITR_OUT_DR       ,  /* IT Room Outer Door  */
   D_SPT_PRM_OUT_DR       ,  /* Pump Room Outer Door */
   D_SPT_PER_GT           ,  /* Perimeter Gates */
   D_SPT_LTG_LOC_OR       ,  /* Lighting Override */
   D_SPT_LTG_PRM          ,  /* Pump Room Lights */
   D_SPT_LTG_ENC          ,  /* Enclosure Lights */
   D_SPT_LTG_WLA          ,  /* Well Area Lights */
   D_SPT_LTG_ITR          ,  /* IT Room Lights */
   D_SPT_ESTOP_CTRS       ,  /* Emergency Stop Contactors */
   D_SPT_SPLC_CONTROL     ,  /* Services Panel Control Source */
   D_SPT_SPLC_MODE        ,  /* Services Panel Control Mode */
   D_SPT_SPLC_STATE       ,  /* Services Panel General Status */
   D_SPT_SPLC_UPS         ,  /* Services PLC UPS */
   D_SPT_OIL_SYS          ,  /* Oil System */
   D_SPT_AIR_SYS          ,  /* Compressed Air System */ 
   D_SPT_LIM_ILOCK        ,  /* Limits & Interlocks */
   D_SPT_ENCL             ,  /* Enclosure */

   D_SPT_PWR_AZM          ,  /* Azimuth Computer Power */  
   D_SPT_PWR_ALT          ,  /* Altitude Computer Power */
   D_SPT_PWR_CAS          ,  /* Cassegrain Computer Power */  
   D_SPT_PWR_AMN          ,  /* Auxiliary Mechanism Node Power */
   D_SPT_PWR_NDS          ,  /* Nasmyth Drive-Side Power */
   D_SPT_PWR_NWS          ,  /* Nasmyth Wrap-Side Power */
   D_SPT_PWR_ACC          ,  /* Autoguider Computer Power */
   D_SPT_PWR_AZM_SW       ,  /* Azimuth Switchgear Power */
   D_SPT_PWR_ALT_SW       ,  /* Altitude Switchgear Power */
   D_SPT_PWR_CAS_SW       ,  /* Cassegrain Switchgear Power */
   D_SPT_CSE_SPLY         ,  /* Customer Instrument Supply */
   D_SPT_ENC_DRS          ,  /* Enclosure Doors */
   D_SPT_ENC_RSD          ,  /* Enclosure Roller Shutter Door */
   D_SPT_HTG_ENC_SEAL     ,  /* Enclosure Seals Trace Heating */
   D_SPT_MODE             ,  /* Mode Selector Switch */
   D_SPT_PWR_AG           ,  /* A & G Box Drives Power */
   D_SPT_PWR_AXIS         ,  /* Axis Drives Power */
   D_SPT_PWR_CRY1         ,  /* Cryotiger No 1 Power */
   D_SPT_PWR_CRY2         ,  /* Cryotiger No 2 Power */
   D_SPT_PWR_LV           ,  /* Basebox Low Voltage Power */
   D_SPT_PWR_MAIN         ,  /* Services Main Power */
   D_SPT_PWR_MCO          ,  /* Mirror Cover Power */
   D_SPT_PWR_MV           ,  /* Basebox Medium Voltage Power */
   D_SPT_PWR_OFM          ,  /* Off-Mount Power */
   D_SPT_PWR_ONM          ,  /* On-Mount Power */
   D_SPT_PWR_PMC          ,  /* Primary Mirror Cell Power */
   D_SPT_PWR_PRES         ,  /* Basebox Pressure Sensors Power */
   D_SPT_PWR_SEC          ,  /* Secondary Mirror Power */
   D_SPT_PWR_SRV          ,  /* Servo Valve Power */
   D_SPT_PWR_TEMP         ,  /* Motor Temperature Panel Power */

   D_SPT_AZM_LIMITS       ,  /* Azimuth Limits */
   D_SPT_ALT_LIMITS       ,  /* Altitude Limits */
   D_SPT_CAS_LIMITS       ,  /* Cassegrain Limits */

   D_SPT_AZM_M1_TEMP      ,  /* Azimuth Motor 1 Temperature Status */
   D_SPT_AZM_M1_TEMP_VAL  ,  /* Azimuth Motor 1 Temperature Value */
   D_SPT_AZM_M2_TEMP      ,  /* Azimuth Motor 2 Temperature Status */
   D_SPT_AZM_M2_TEMP_VAL  ,  /* Azimuth Motor 2 Temperature Value */ 
   D_SPT_ALT_M1_TEMP      ,  /* Altitude Motor 1 Temperature Status */
   D_SPT_ALT_M1_TEMP_VAL  ,  /* Altitude Motor 1 Temperature Value */ 
   D_SPT_ALT_M2_TEMP      ,  /* Altitude Motor 2 Temperature Status */
   D_SPT_ALT_M2_TEMP_VAL  ,  /* Altitude Motor 2 Temperature Value */ 
   D_SPT_CAS_M1_TEMP      ,  /* Cassegrain Motor 1 Temperature Status */
   D_SPT_CAS_M1_TEMP_VAL  ,  /* Cassegrain Motor 1 Temperature Value */ 
   D_SPT_CAS_M2_TEMP      ,  /* Cassegrain Motor 2 Temperature Status */
   D_SPT_CAS_M2_TEMP_VAL  ,  /* Cassegrain Motor 2 Temperature Value */ 
   D_SPT_AMP_RK_TEMP      ,  /* Amplifier Rack Temperature Status */
   D_SPT_AMP_RK_TEMP_VAL  ,  /* Amplifier Rack Temperature Value */
   D_SPT_OIL_AMB_TEMP     ,  /* Oil & Ambient Temperature Difference Status */
   D_SPT_OIL_AMB_TEMP_VAL ,  /* Oil & Ambient Temperature Difference Value */ 
   D_SPT_WELL_AMB_TEMP    ,  /* Well Ambient Temperature Status */
   D_SPT_WELL_AMB_TEMP_VAL,  /* Well Ambient Temperature Value */ 
   D_SPT_SPARE_4_TEMP     ,  /* Spare 4 Temperature Status */
   D_SPT_SPARE_4_TEMP_VAL ,  /* Spare 4 Temperature Value */ 
   D_SPT_AMN_TEMP         ,  /* Auxiliary Mechanism Node Temperature Status */
   D_SPT_AMN_TEMP_VAL     ,  /* Auxiliary Mechanism Node Temperature Value */
   D_SPT_CHK_PNL_TEMP     ,  /* Choke Panel Temperature Status */
   D_SPT_CHK_PNL_TEMP_VAL ,  /* Choke Panel Temperature Value */ 
   D_SPT_AG_TEMP          ,  /* A & G Drives Temperature Status */
   D_SPT_AG_TEMP_VAL      ,  /* A & G Drives Temperature Value */ 
   D_SPT_SPARE_8_TEMP     ,  /* Spare 8 Temperature Status */
   D_SPT_SPARE_8_TEMP_VAL ,  /* Spare 8 Temperature Value */ 
   D_SPT_ENC_AMB_TEMP     ,  /* Enclosure Ambient Temperature Status */
   D_SPT_ENC_AMB_TEMP_VAL ,  /* Enclosure Ambient Temperature Value */
   D_SPT_HTG_TT1_TEMP     ,  /* Trace Heating Sensor 1 Temperature Status */
   D_SPT_HTG_TT1_TEMP_VAL ,  /* Trace Heating Sensor 1 Temperature Value */ 
   D_SPT_HTG_TT2_TEMP     ,  /* Trace Heating Sensor 2 Temperature Status */
   D_SPT_HTG_TT2_TEMP_VAL ,  /* Trace Heating Sensor 2 Temperature Value */ 
   D_SPT_HTG_TT3_TEMP     ,  /* Trace Heating Sensor 3 Temperature Status */
   D_SPT_HTG_TT3_TEMP_VAL ,  /* Trace Heating Sensor 3 Temperature Value */ 
   D_SPT_HTG_TT4_TEMP     ,  /* Trace Heating Sensor 4 Temperature Status */
   D_SPT_HTG_TT4_TEMP_VAL ,  /* Trace Heating Sensor 4 Temperature Value */ 
   D_SPT_HTG_TT5_TEMP     ,  /* Trace Heating Sensor 5 Temperature Status */
   D_SPT_HTG_TT5_TEMP_VAL ,  /* Trace Heating Sensor 5 Temperature Value */ 
   D_SPT_HTG_TT6_TEMP     ,  /* Trace Heating Sensor 6 Temperature Status */
   D_SPT_HTG_TT6_TEMP_VAL ,  /* Trace Heating Sensor 6 Temperature Value */ 
   D_SPT_HYD_CHL_TEMP     ,  /* Hydrostatic Chiller Oil Temperature Status */
   D_SPT_HYD_CHL_TEMP_VAL ,  /* Hydrostatic Chiller Oil Temperature Value */ 
   D_SPT_HYD_FT1_FLOW     ,  /* Oil Flow Transmitter 1 Flow Status */
   D_SPT_HYD_FT1_FLOW_VAL ,  /* Oil Flow Transmitter 1 Flow Value */ 
   D_SPT_HYD_FT2_FLOW     ,  /* Oil Flow Transmitter 2 Flow Status */
   D_SPT_HYD_FT2_FLOW_VAL ,  /* Oil Flow Transmitter 2 Flow Value */ 
   D_SPT_HYD_FT3_FLOW     ,  /* Oil Flow Transmitter 3 Flow Status */
   D_SPT_HYD_FT3_FLOW_VAL ,  /* Oil Flow Transmitter 3 Flow Value */ 
   D_SPT_HYD_FT4_FLOW     ,  /* Oil Flow Transmitter 4 Flow Status */
   D_SPT_HYD_FT4_FLOW_VAL ,  /* Oil Flow Transmitter 4 Flow Value */ 
   D_SPT_HYD_MAIN_LVL     ,  /* Main Hydraulic Pump Level Status */
   D_SPT_HYD_MAIN_LVL_VAL ,  /* Main Hydraulic Pump Level Value */ 
   D_SPT_HYD_PT1A_PRES    ,  /* Oil Pressure Transmitter PT1A Status */
   D_SPT_HYD_PT1A_PRES_VAL,  /* Oil Pressure Transmitter PT1A Value */ 
   D_SPT_HYD_PT1B_PRES    ,  /* Oil Pressure Transmitter PT1B Status */
   D_SPT_HYD_PT1B_PRES_VAL,  /* Oil Pressure Transmitter PT1B Value */ 
   D_SPT_HYD_PT1C_PRES    ,  /* Oil Pressure Transmitter PT1C Status */
   D_SPT_HYD_PT1C_PRES_VAL,  /* Oil Pressure Transmitter PT1C Value */ 
   D_SPT_HYD_PT1D_PRES    ,  /* Oil Pressure Transmitter PT1D Status */
   D_SPT_HYD_PT1D_PRES_VAL,  /* Oil Pressure Transmitter PT1D Value */ 
   D_SPT_HYD_PT2A_PRES    ,  /* Oil Pressure Transmitter PT2A Status */
   D_SPT_HYD_PT2A_PRES_VAL,  /* Oil Pressure Transmitter PT2A Value */ 
   D_SPT_HYD_PT2B_PRES    ,  /* Oil Pressure Transmitter PT2B Status */
   D_SPT_HYD_PT2B_PRES_VAL,  /* Oil Pressure Transmitter PT2B Value */ 
   D_SPT_HYD_PT3A_PRES    ,  /* Oil Pressure Transmitter PT3A Status */
   D_SPT_HYD_PT3A_PRES_VAL,  /* Oil Pressure Transmitter PT3A Value */ 
   D_SPT_HYD_PT3B_PRES    ,  /* Oil Pressure Transmitter PT3B Status */
   D_SPT_HYD_PT3B_PRES_VAL,  /* Oil Pressure Transmitter PT3B Value */ 
   D_SPT_HYD_PT3C_PRES    ,  /* Oil Pressure Transmitter PT3C Status */
   D_SPT_HYD_PT3C_PRES_VAL,  /* Oil Pressure Transmitter PT3C Value */ 
   D_SPT_HYD_PT4A_PRES    ,  /* Oil Pressure Transmitter PT4A Status */
   D_SPT_HYD_PT4A_PRES_VAL,  /* Oil Pressure Transmitter PT4A Value */ 
   D_SPT_HYD_PT4B_PRES    ,  /* Oil Pressure Transmitter PT4B Status */
   D_SPT_HYD_PT4B_PRES_VAL,  /* Oil Pressure Transmitter PT4B Value */ 
   D_SPT_HYD_PT4C_PRES    ,  /* Oil Pressure Transmitter PT4C Status */
   D_SPT_HYD_PT4C_PRES_VAL,  /* Oil Pressure Transmitter PT4C Value */ 
   D_SPT_HYD_SCV_LVL      ,  /* Scavenge Oil Level Status */
   D_SPT_HYD_SCV_LVL_VAL  ,  /* Scavenge Oil Level Value */ 
   D_SPT_HYD_OIL_TEMP     ,  /* Hydrostatic Oil Temperature Status */
   D_SPT_HYD_OIL_TEMP_VAL ,  /* Hydrostatic Oil Temperature Value */ 
   D_SPT_HYD_TRY_LVL      ,  /* Azimuth Ring Oil Tray Level Status */
   D_SPT_HYD_TRY_LVL_VAL  ,  /* Azimuth Ring Oil Tray Level Value */
   D_SPT_IT_HUMD          ,  /* IT Rack Humidity Status */
   D_SPT_IT_HUMD_VAL      ,  /* IT Rack Humidity Value */ 
   D_SPT_IT_RM_TEMP       ,  /* IT Room Temperature Status */
   D_SPT_IT_RM_TEMP_VAL   ,  /* IT Room Temperature Value */ 
   D_SPT_IT_TEMP          ,  /* IT Rack Temperature Status */
   D_SPT_IT_TEMP_VAL      ,  /* IT Rack Temperature Value */ 
   D_SPT_MCO_PRES         ,  /* Mirror Cover Air Pressure Status */
   D_SPT_MCO_PRES_VAL     ,  /* Mirror Cover Air Pressure Value */ 
   D_SPT_PMC_PRES         ,  /* Primary Mirror Cell Air Pressure Status */
   D_SPT_PMC_PRES_VAL     ,  /* Primary Mirror Cell Air Pressure Value */ 
   D_SPT_PRM_TEMP         ,  /* Pump Room Temperature Status */
   D_SPT_PRM_TEMP_VAL     ,  /* Pump Room Temperature Value */ 

   D_SPT_AZM_M1           ,  /* Azimuth Motor 1 Status */
   D_SPT_AZM_M1_HRS       ,  /* Azimuth Motor 1 Running Hours */
   D_SPT_AZM_M2           ,  /* Azimuth Motor 2 Status */       
   D_SPT_AZM_M2_HRS       ,  /* Azimuth Motor 2 Running Hours */
   D_SPT_ALT_M1           ,  /* Altitude Motor 1 Status */       
   D_SPT_ALT_M1_HRS       ,  /* Altitude Motor 1 Running Hours */
   D_SPT_ALT_M2           ,  /* Altitude Motor 2 Status */       
   D_SPT_ALT_M2_HRS       ,  /* Altitude Motor 2 Running Hours */
   D_SPT_CAS_M1           ,  /* Cassegrain Motor 1 Status */       
   D_SPT_CAS_M1_HRS       ,  /* Cassegrain Motor 1 Running Hours */
   D_SPT_CAS_M2           ,  /* Cassegrain Motor 2 Status */       
   D_SPT_CAS_M2_HRS       ,  /* Cassegrain Motor 2 Running Hours */
   D_SPT_PRM_FAN          ,  /* Pump Room Fan Status */       
   D_SPT_PRM_FAN_HRS      ,  /* Pump Room Fan Running Hours */
   D_SPT_AIR_COMP         ,  /* Air Compressor Status */       
   D_SPT_AIR_COMP_HRS     ,  /* Air Compressor Running Hours */
   D_SPT_HYD_PMP          ,  /* Main Hydraulic Pump Status */       
   D_SPT_HYD_PMP_HRS      ,  /* Main Hydraulic Pump Running Hours */
   D_SPT_HYD_CHL_PMP      ,  /* Hydrostatic Chiller Feed Pump Status */       
   D_SPT_HYD_CHL_PMP_HRS  ,  /* Hydrostatic Chiller Feed Pump Running Hours */
   D_SPT_HYD_CHL          ,  /* Hydrostatic Chiller Status */       
   D_SPT_HYD_CHL_HRS      ,  /* Hydrostatic Chiller Running Hours */
   D_SPT_HYD_PSCV         ,  /* Pre-Scavenge Pump Status */       
   D_SPT_HYD_PSCV_HRS     ,  /* Pre-Scavenge Pump Running Hours */
   D_SPT_HYD_SCV_PMP      ,  /* Scavenge Pump Status */       
   D_SPT_HYD_SCV_PMP_HRS  ,  /* Scavenge Pump Running Hours */

   D_SPT_HYD_AZM_GB       ,  /* Azimuth Guide Bearing Solenoid Status */
   D_SPT_HYD_AZM_GB_OPS_A ,  /* Azimuth Guide Bearing Solenoid Ops A */
   D_SPT_HYD_AZM_GB_OPS_B ,  /* Azimuth Guide Bearing Solenoid Ops B */
   D_SPT_HYD_AZM_LB       ,  /* Azimuth Load Bearing Solenoid Status */
   D_SPT_HYD_AZM_LB_OPS_A ,  /* Azimuth Load Bearing Solenoid Ops A */ 
   D_SPT_HYD_AZM_LB_OPS_B ,  /* Azimuth Load Bearing Solenoid Ops B */ 
   D_SPT_HYD_ALT_GB       ,  /* Altitude Guide Bearing Solenoid Status */
   D_SPT_HYD_ALT_GB_OPS_A ,  /* Altitude Guide Bearing Solenoid Ops A */ 
   D_SPT_HYD_ALT_GB_OPS_B ,  /* Altitude Guide Bearing Solenoid Ops B */ 
   D_SPT_HYD_ALT_LB       ,  /* Altitude Load Bearing Solenoid Status */ 
   D_SPT_HYD_ALT_LB_OPS_A ,  /* Altitude Load Bearing Solenoid Ops A */  
   D_SPT_HYD_ALT_LB_OPS_B ,  /* Altitude Load Bearing Solenoid Ops B */  
   D_SPT_MCO              ,  /* Mirror Cover Status */
   D_SPT_MCO_OPS_A        ,  /* Mirror Cover Ops A */
   D_SPT_MCO_OPS_B        ,  /* Mirror Cover Ops B */

   D_SPT_SPLC_STATUS      ,  /* SPLC Status */

   D_SPT_SPLC_VER_STATUS  ,  /* SPLC Version Status */
   D_SPT_SPLC_VER_NUMBER  ,  /* SPLC Version Number */

   D_SPT_DNET_MASTER_ERROR,  /* DeviceNet Master Error */
   D_SPT_DNET_NODE_2_ERR  ,  /* DeviceNet Node 2 Error */
   D_SPT_DNET_NODE_3_ERR  ,  /* DeviceNet Node 3 Error */
   D_SPT_DNET_NODE_5_ERR  ,  /* DeviceNet Node 5 Error */
   D_SPT_DNET_NODE_7_ERR  ,  /* DeviceNet Node 7 Error */
   D_SPT_DNET_NODE_10_ERR ,  /* DeviceNet Node 10 Error */
   D_SPT_DNET_NODE_11_ERR ,  /* DeviceNet Node 11 Error */
   D_SPT_DNET_NODE_16_ERR ,  /* DeviceNet Node 16 Error */
   D_SPT_DNET_NODE_21_ERR ,  /* DeviceNet Node 21 Error */
   D_SPT_DNET_NODE_24_ERR ,  /* DeviceNet Node 24 Error */
   D_SPT_DNET_NODE_28_ERR ,  /* DeviceNet Node 28 Error */
   D_SPT_DNET_NODE_35_ERR ,  /* DeviceNet Node 35 Error */
   D_SPT_DNET_NODE_36_ERR ,  /* DeviceNet Node 36 Error */
   D_SPT_DNET_NODE_37_ERR ,  /* DeviceNet Node 37 Error */
   D_SPT_DNET_NODE_X1_ERR ,  /* DeviceNet unused Node 1 Error */
   D_SPT_DNET_NODE_X2_ERR ,  /* DeviceNet unused Node 2 Error */
   D_SPT_DNET_NODE_X3_ERR ,  /* DeviceNet unused Node 3 Error */
   D_SPT_DNET_NODE_X4_ERR ,  /* DeviceNet unused Node 4 Error */
   D_SPT_DNET_NODE_X5_ERR ,  /* DeviceNet unused Node 5 Error */
   D_SPT_DNET_NODE_X6_ERR ,  /* DeviceNet unused Node 6 Error */
   D_SPT_AZM_OIL_STATUS   ,  /* Overall Azimuth Oil Status */
   D_SPT_ALT_OIL_STATUS   ,  /* Overall Altitude Oil Status */
   D_SPT_L2_LIMIT         ,  /* Overall L2 Limit Status */
   D_SPT_L3_LIMIT         ,  /* Overall L3 Limit Status */
   D_SPT_AZM_PRESS_OOR    ,  /* Azimuth Oil Pressure Out of Range */
   D_SPT_ALT_PRESS_OOR    ,  /* Altitude Oil Pressure Out of Range */
   D_SPT_AZM_TRAP_KEY_CA  ,  /* Azimuth Trapped Key Ca */
   D_SPT_AZM_TRAP_KEY_FA  ,  /* Azimuth Trapped Key Fa */
   D_SPT_ALT_TRAP_KEY_DA  ,  /* Altitude Trapped Key Da */
   D_SPT_ALT_TRAP_KEY_GA  ,  /* Altitude Trapped Key Ga */
   D_SPT_CAS_TRAP_KEY_EA  ,  /* Cassegrain Trapped Key Ea */
   D_SPT_CAS_TRAP_KEY_HA  ,  /* Cassegrain Trapped Key Ha */
   D_SPT_TRAP_KEY_ERROR   ,  /* Multiple Trapped Key Error */
   D_SPT_AZM_FLOW_OOR     ,  /* Azimuth Oil Flow Out of Range */
   D_SPT_ALT_FLOW_OOR     ,  /* Altitude Oil Flow Out of Range */
   D_SPT_TH_Z1_CUR_LOW    ,  /* Trace Heating Zone 1 Current Low */
   D_SPT_TH_Z2_CUR_LOW    ,  /* Trace Heating Zone 2 Current Low */
   D_SPT_TH_Z3_CUR_LOW    ,  /* Trace Heating Zone 3 Current Low */

   D_SPT_HTG_Z1_SPLY      ,  /* Trace Heating Zone 1 Supply */
   D_SPT_HTG_Z2_SPLY      ,  /* Trace Heating Zone 2 Supply */
   D_SPT_HTG_Z3_SPLY      ,  /* Trace Heating Zone 3 Supply */
   D_SPT_PWR_P48_AMP      ,  /* Amplifier Panel Supply */
   D_SPT_PWR_P02_SCAV     ,  /* Scavenge Pumpset Supply */
   D_SPT_MV_3_PH_L1_SPLY  ,  /* Incoming MV 3 Phase 1 Supply */
   D_SPT_MV_3_PH_L2_SPLY  ,  /* Incoming MV 3 Phase 2 Supply */
   D_SPT_MV_3_PH_L3_SPLY  ,  /* Incoming MV 3 Phase 3 Supply */
   D_SPT_MV_3_PH_SPLY     ,  /* Incoming MV 3 Phase Supply */
   D_SPT_LGT_MCB_PUMP_ROOM,  /* Pump Room Lights MCB */
   D_SPT_LGT_MCB_ENCLOSURE,  /* Enclosure Lights MCB */
   D_SPT_LGT_MCB_WELL_AREA,  /* Well Area Lights MCB */
   D_SPT_LGT_MCB_IT_ROOM  ,  /* IT Room Lights MCB */  
   D_SPT_FREQ_CURRENT_CONV,  /* Frequency to Current Converter */
   D_SPT_TRUSS_TEMP_MCB   ,  /* Truss Temperature Monitoring Panel */

   D_SPT_AZM_AMP1_TEMP    ,  /* Azimuth Amplifier 1 Temperature Status */   
   D_SPT_AZM_AMP1_TEMP_VAL,  /* Azimuth Amplifier 1 Temperature Value */    
   D_SPT_AZM_AMP2_TEMP    ,  /* Azimuth Amplifier otor 2 Temperature Status */   
   D_SPT_AZM_AMP2_TEMP_VAL,  /* Azimuth Amplifier 2 Temperature Value */    
   D_SPT_ALT_AMP1_TEMP    ,  /* Altitude Amplifier 1 Temperature Status */  
   D_SPT_ALT_AMP1_TEMP_VAL,  /* Altitude Amplifier 1 Temperature Value */   
   D_SPT_ALT_AMP2_TEMP    ,  /* Altitude Amplifier 2 Temperature Status */  
   D_SPT_ALT_AMP2_TEMP_VAL,  /* Altitude Amplifier 2 Temperature Value */   
   D_SPT_CAS_AMP1_TEMP    ,  /* Cassegrain Amplifier 1 Temperature Status */
   D_SPT_CAS_AMP1_TEMP_VAL,  /* Cassegrain Amplifier 1 Temperature Value */ 
   D_SPT_CAS_AMP2_TEMP    ,  /* Cassegrain Amplifier 2 Temperature Status */
   D_SPT_CAS_AMP2_TEMP_VAL,  /* Cassegrain Amplifier 2 Temperature Value */ 
   D_SPT_HTG_Z1_TEMP      ,  /* Trace Heating 1 Temperature Status */
   D_SPT_HTG_Z1_TEMP_VAL  ,  /* Trace Heating 1 Temperature Value */
   D_SPT_HTG_Z2_TEMP      ,  /* Trace Heating 2 Temperature Status */
   D_SPT_HTG_Z2_TEMP_VAL  ,  /* Trace Heating 2 Temperature Value */
   D_SPT_HTG_Z3_TEMP      ,  /* Trace Heating 3 Temperature Status */
   D_SPT_HTG_Z3_TEMP_VAL  ,  /* Trace Heating 3 Temperature Value */
   D_SPT_ENC_SEAL_TEMP    ,  /* Enclosure Seals Trace Heating Temperature Status */
   D_SPT_ENC_SEAL_TEMP_VAL,  /* Enclosure Seals Trace Heating Temperature Value */

   D_SPT_AZM_BRK1         ,  /* Azimuth Brake 1 Status */
   D_SPT_AZM_BRK1_OPS_A   ,  /* Azimuth Brake 1 Ops A */
   D_SPT_AZM_BRK1_OPS_B   ,  /* Azimuth Brake 1 Ops B */
   D_SPT_AZM_BRK2         ,  /* Azimuth Brake 2 Status */
   D_SPT_AZM_BRK2_OPS_A   ,  /* Azimuth Brake 2 Ops A */ 
   D_SPT_AZM_BRK2_OPS_B   ,  /* Azimuth Brake 2 Ops B */ 
   D_SPT_ALT_BRK1         ,  /* Altitude Brake 1 Status */
   D_SPT_ALT_BRK1_OPS_A   ,  /* Altitude Brake 1 Ops A */ 
   D_SPT_ALT_BRK1_OPS_B   ,  /* Altitude Brake 1 Ops B */ 
   D_SPT_ALT_BRK2         ,  /* Altitude Brake 2 Status */
   D_SPT_ALT_BRK2_OPS_A   ,  /* Altitude Brake 2 Ops A */ 
   D_SPT_ALT_BRK2_OPS_B   ,  /* Altitude Brake 2 Ops B */ 
   D_SPT_CAS_BRK1         ,  /* Cassegrain Brake 1 Status */
   D_SPT_CAS_BRK1_OPS_A   ,  /* Cassegrain Brake 1 Ops A */ 
   D_SPT_CAS_BRK1_OPS_B   ,  /* Cassegrain Brake 1 Ops B */ 
   D_SPT_CAS_BRK2         ,  /* Cassegrain Brake 2 Status */
   D_SPT_CAS_BRK2_OPS_A   ,  /* Cassegrain Brake 2 Ops A */ 
   D_SPT_CAS_BRK2_OPS_B   ,  /* Cassegrain Brake 2 Ops B */ 
   D_SPT_AZM_BRK          ,  /* Azimuth Brakes Status */
   D_SPT_AZM_BRK_OPS_A    ,  /* Azimuth Brakes Ops A */ 
   D_SPT_AZM_BRK_OPS_B    ,  /* Azimuth Brakes Ops B */ 
   D_SPT_ALT_BRK          ,  /* Altitude Brakes Status */
   D_SPT_ALT_BRK_OPS_A    ,  /* Altitude Brakes Ops A */ 
   D_SPT_ALT_BRK_OPS_B    ,  /* Altitude Brakes Ops B */ 
   D_SPT_CAS_BRK          ,  /* Cassegrain Brakes Status */
   D_SPT_CAS_BRK_OPS_A    ,  /* Cassegrain Brakes Ops A */ 
   D_SPT_CAS_BRK_OPS_B    ,  /* Cassegrain Brakes Ops B */ 
   D_SPT_AXIS_RELAY       ,  /* Axis Relay Status */
   D_SPT_AXIS_RELAY_OPS_A ,  /* Axis Relay Ops A */ 
   D_SPT_AXIS_RELAY_OPS_B ,  /* Axis Relay Ops B */ 
   D_SPT_ESTOP_SYS        ,  /* Emergency Stop System Status */
   D_SPT_ESTOP_SYS_OPS_A  ,  /* Emergency Stop System Ops A */ 
   D_SPT_ESTOP_SYS_OPS_B  ,  /* Emergency Stop System Ops B */ 

   D_SPT_AZM_AXIS_VEL     ,  /* Azimuth Axis Velocity */
   D_SPT_AZM_AXIS_ACCEL   ,  /* Azimuth Axis Acceleration */
   D_SPT_ALT_AXIS_VEL     ,  /* Altitude Axis Velocity */    
   D_SPT_ALT_AXIS_ACCEL   ,  /* Altitude Axis Acceleration */
   D_SPT_CAS_AXIS_VEL     ,  /* Cassegrain Axis Velocity */    
   D_SPT_CAS_AXIS_ACCEL   ,  /* Cassegrain Axis Acceleration */
   D_SPT_PUMP_RM_AIR_COND ,  /* Pump Room Air Conditioning */
   D_SPT_EPLC_WDOG_TRIP   ,  /* Enclosure PLC Watchdog */

   D_SPT_HMI_VER_STATUS   ,  /* HMI Version Status */
   D_SPT_HMI_VER_NUMBER   ,  /* HMI Version Number */

   D_SPT_SPLC_WDOG_TRIP   ,  /* Services PLC Hardwired Watchdog */
   D_SPT_SPLC_HLTHY_TALLY ,  /* Total cumulative SPLC Healthy going true events */
   D_SPT_SPLC_UHLTHY_FINE ,  /* SPLC Healthy false duration in 0.01s units */
   D_SPT_SPLC_UHLTHY_1SEC ,  /* SPLC Healthy false duration in 1s units */

   D_SPT_PWR_CHILLER      ,  /* Chiller Power */
   D_SPT_PWR_BB_PWR_SWITCH,  /* Basebox Power Switch Power */
   D_SPT_ALARM_WORD_00    ,  /* Alarm table - word 00          */
   D_SPT_ALARM_WORD_01    ,  /* Alarm table - word 01          */
   D_SPT_ALARM_WORD_02    ,  /* Alarm table - word 02          */
   D_SPT_ALARM_WORD_03    ,  /* Alarm table - word 03          */
   D_SPT_ALARM_WORD_04    ,  /* Alarm table - word 04          */
   D_SPT_ALARM_WORD_05    ,  /* Alarm table - word 05          */
   D_SPT_ALARM_WORD_06    ,  /* Alarm table - word 06          */
   D_SPT_ALARM_WORD_07    ,  /* Alarm table - word 07          */
   D_SPT_ALARM_WORD_08    ,  /* Alarm table - word 08          */
   D_SPT_ALARM_WORD_09    ,  /* Alarm table - word 09          */
   D_SPT_ALARM_WORD_10    ,  /* Alarm table - word 10          */
   D_SPT_ALARM_WORD_11    ,  /* Alarm table - word 11          */
   D_SPT_ALARM_WORD_12    ,  /* Alarm table - word 12          */
   D_SPT_ALARM_WORD_13    ,  /* Alarm table - word 13          */
   D_SPT_ALARM_WORD_14    ,  /* Alarm table - word 14          */
   D_SPT_ALARM_WORD_15    ,  /* Alarm table - word 15          */
   D_SPT_ALARM_WORD_16    ,  /* Alarm table - word 16          */
   D_SPT_ALARM_WORD_17    ,  /* Alarm table - word 17          */
   D_SPT_ALARM_WORD_18    ,  /* Alarm table - word 18          */
   D_SPT_ALARM_WORD_19    ,  /* Alarm table - word 19          */
   D_SPT_ALARM_WORD_20    ,  /* Alarm table - word 20          */
   D_SPT_ALARM_WORD_21    ,  /* Alarm table - word 21          */
   D_SPT_ALARM_WORD_22    ,  /* Alarm table - word 22          */
   D_SPT_ALARM_WORD_23    ,  /* Alarm table - word 23          */
   D_SPT_ALARM_WORD_24    ,  /* Alarm table - word 24          */
   D_SPT_ALARM_WORD_25    ,  /* Alarm table - word 25          */
   D_SPT_ALARM_WORD_26    ,  /* Alarm table - word 26          */
   D_SPT_ALARM_WORD_27    ,  /* Alarm table - word 27          */
   D_SPT_ALARM_WORD_28    ,  /* Alarm table - word 28          */
   D_SPT_ALARM_WORD_29    ,  /* Alarm table - word 29          */
   D_SPT_ALARM_WORD_30    ,  /* Alarm table - word 30          */
   D_SPT_MCO_TL_CLOSED    ,  /* Mirror cover top-left closed       */
   D_SPT_MCO_TL_CLD_OPS_A ,  /* Mirror cover top-left closed A     */
   D_SPT_MCO_TL_CLD_OPS_B ,  /* Mirror cover top-left closed B     */
   D_SPT_MCO_TL_OPENED    ,  /* Mirror cover top-left opened       */
   D_SPT_MCO_TL_OPN_OPS_A ,  /* Mirror cover top-left opened A     */
   D_SPT_MCO_TL_OPN_OPS_B ,  /* Mirror cover top-left opened B     */
   D_SPT_MCO_BL_OPENED    ,  /* Mirror cover bottom-left opened    */
   D_SPT_MCO_BL_OPN_OPS_A ,  /* Mirror cover bottom-left opened A  */
   D_SPT_MCO_BL_OPN_OPS_B ,  /* Mirror cover bottom-left opened B  */
   D_SPT_MCO_BL_CLOSED    ,  /* Mirror cover bottom-left closed    */
   D_SPT_MCO_BL_CLD_OPS_A ,  /* Mirror cover bottom-left closed A  */
   D_SPT_MCO_BL_CLD_OPS_B ,  /* Mirror cover bottom-left closed B  */
   D_SPT_MCO_TR_CLOSED    ,  /* Mirror cover top-right closed      */
   D_SPT_MCO_TR_CLD_OPS_A ,  /* Mirror cover top-right closed A    */
   D_SPT_MCO_TR_CLD_OPS_B ,  /* Mirror cover top-right closed B    */
   D_SPT_MCO_TR_OPENED    ,  /* Mirror cover top-right opened      */
   D_SPT_MCO_TR_OPN_OPS_A ,  /* Mirror cover top-right opened A    */
   D_SPT_MCO_TR_OPN_OPS_B ,  /* Mirror cover top-right opened B    */
   D_SPT_MCO_BR_OPENED    ,  /* Mirror cover bottom-right opened   */
   D_SPT_MCO_BR_OPN_OPS_A ,  /* Mirror cover bottom-right opened A */
   D_SPT_MCO_BR_OPN_OPS_B ,  /* Mirror cover bottom-right opened B */
   D_SPT_MCO_BR_CLOSED    ,  /* Mirror cover bottom-right closed   */
   D_SPT_MCO_BR_CLD_OPS_A ,  /* Mirror cover bottom-right closed A */
   D_SPT_MCO_BR_CLD_OPS_B ,  /* Mirror cover bottom-right closed B */
   D_SPT_SPARE_62         ,  /* Spare word 62 */
   D_SPT_SPARE_63         ,  /* Spare word 63 */
   D_SPT_SPARE_64         ,  /* Spare word 64 */
   D_SPT_SPARE_65         ,  /* Spare word 65 */
   D_SPT_SPARE_66         ,  /* Spare word 66 */
   D_SPT_SPARE_67         ,  /* Spare word 67 */
   D_SPT_SPARE_68         ,  /* Spare word 68 */
   D_SPT_SPARE_69         ,  /* Spare word 69 */
   D_SPT_SPARE_70         ,  /* Spare word 70 */
   D_SPT_SPARE_71         ,  /* Spare word 71 */
   D_SPT_SPARE_72         ,  /* Spare word 72 */
   D_SPT_SPARE_73         ,  /* Spare word 73 */
   D_SPT_SPARE_74         ,  /* Spare word 74 */
   D_SPT_SPARE_75         ,  /* Spare word 75 */
   D_SPT_SPARE_76         ,  /* Spare word 76 */
   D_SPT_SPARE_77         ,  /* Spare word 77 */
   D_SPT_SPARE_78         ,  /* Spare word 78 */
   D_SPT_SPARE_79         ,  /* Spare word 79 */
   D_SPT_SPARE_80         ,  /* Spare word 80 */
   D_SPT_SPARE_81         ,  /* Spare word 81 */
   D_SPT_SPARE_82         ,  /* Spare word 82 */
   D_SPT_SPARE_83         ,  /* Spare word 83 */
   D_SPT_SPARE_84         ,  /* Spare word 84 */
   D_SPT_SPARE_85         ,  /* Spare word 85 */
   D_SPT_SPARE_86         ,  /* Spare word 86 */
   D_SPT_SPARE_87         ,  /* Spare word 87 */
   D_SPT_SPARE_88         ,  /* Spare word 88 */
   D_SPT_SPARE_89         ,  /* Spare word 89 */
   D_SPT_SPARE_90         ,  /* Spare word 90 */
   D_SPT_SPARE_91         ,  /* Spare word 91 */
   D_SPT_SPARE_92         ,  /* Spare word 92 */

   D_SPT_PLC_SYSTEM_STATUS,  /* SPLC System Status */

   D_SPT_STATUS_DUMMY     ,  /* Dummy end of status list */

   D_SPT_PLC_ALARM_LEVEL  ,  /* Overall PLC alarm level */

   D_SPT_HPLC_MODE          , /* HPLC Control Mode */
   D_SPT_HPLC_STATE         , /* HPLC State */
   D_SPT_SPLC_ESTOP_CTRS    , /* SPLC Emergency Stop Contactors */
   D_SPT_HPLC_ESTOP_CTRS    , /* HPLC Emergency Stop Contactors */
   D_SPT_MAIN_ISOL          , /* Services Panel Main Isolator Status */
   D_SPT_PWR_HPLC_MAIN      , /* OSS Panel Main Power Status */
   D_SPT_HPLC_UPS           , /* HPLC UPS Status */
   D_SPT_PWR_P37_T1         , /* Axis Drives Power Supply 1 Status */
   D_SPT_PWR_P37_T2         , /* Axis Drives Power Supply 2 Status */
   D_SPT_SPLC_WD            , /* SPLC Watchdog Status */
   D_SPT_HPLC_WD            , /* HPLC Watchdog Status */
   D_SPT_EPLC_WD            , /* EPLC Watchdog Status */
   D_SPT_HPLC_WD_OVR        , /* HPLC Watchdog Key Bypass Status */
   D_SPT_HPLC_COMMS         , /* HPLC Comms Status */
   D_SPT_NASP_WD            , /* Nasmyth Plus Watchdog */
   D_SPT_NASM_WD            , /* Nasmyth Minus Watchdog */
   D_SPT_CAS_SEL            , /* Cassegrain Rotator Selected Status */
   D_SPT_NASP_SEL           , /* Nasmyth Plus Rotator Selected Status */
   D_SPT_NASM_SEL           , /* Nasmyth Minus Rotator Selected Status */
   D_SPT_NASP_AXIS_VEL      , /* Nasmyth Plus Axis Velocity */
   D_SPT_NASP_AXIS_ACCEL    , /* Nasmyth Plus Axis Acceleration */
   D_SPT_NASM_AXIS_VEL      , /* Nasmyth Minus Axis Velocity */    
   D_SPT_NASM_AXIS_ACCEL    , /* Nasmyth Minus Axis Acceleration */
   D_SPT_MREST_AIR          , /* Mirror Restraint Air Supply Status */
   D_SPT_L2_LIMIT_OVR       , /* L2 Limit Override Status */
   D_SPT_HPLC_FULL_RUN      , /* HPLC Full Hydrostatic System Running */
   D_SPT_AZM_ACW_REC_REQ    , /* Azimuth ACW Limit Recovery Required */
   D_SPT_AZM_CW_REC_REQ     , /* Azimuth CW Limit Recovery Required */
   D_SPT_ALT_ACW_REC_REQ    , /* Altitude ACW Limit Recovery Required */
   D_SPT_ALT_CW_REC_REQ     , /* Altitude CW Limit Recovery Required */
   D_SPT_CAS_ACW_REC_REQ    , /* Cassegrain ACW Limit Recovery Required */
   D_SPT_CAS_CW_REC_REQ     , /* Cassegrain CW Limit Recovery Required */
   D_SPT_NASP_ACW_REC_REQ   , /* Nasmyth Plus ACW Limit Recovery Required */
   D_SPT_NASP_CW_REC_REQ    , /* Nasmyth Plus CW Limit Recovery Required */
   D_SPT_NASM_ACW_REC_REQ   , /* Nasmyth Minus ACW Limit Recovery Required */
   D_SPT_NASM_CW_REC_REQ    , /* Nasmyth Minus CW Limit Recovery Required */
   D_SPT_AZM_NO_DIR         , /* Azimuth No Direction Detected */
   D_SPT_ALT_NO_DIR         , /* Altitude No Direction Detected */
   D_SPT_CAS_NO_DIR         , /* Cassegrain No Direction Detected */
   D_SPT_NASP_NO_DIR        , /* Nasmyth Plus No Direction Detected */
   D_SPT_NASM_NO_DIR        , /* Nasmyth Minus No Direction Detected */
   D_SPT_AZM_VEL_ENC_FLT    , /* Azimuth Velocity Encoder Fault */
   D_SPT_ALT_VEL_ENC_FLT    , /* Altitude Velocity Encoder Fault */
   D_SPT_CAS_VEL_ENC_FLT    , /* Cassegrain Velocity Encoder Fault */
   D_SPT_NASP_VEL_ENC_FLT   , /* Nasmyth Plus Velocity Encoder Fault */
   D_SPT_NASM_VEL_ENC_FLT   , /* Nasmyth Minus Velocity Encoder Fault */
   D_SPT_AZM_L2_LIM_FLT     , /* Azimuth L2 Limit Fault */
   D_SPT_ALT_HOR_L2_LIM_FLT , /* Altitude Horizon L2 Limit Fault */
   D_SPT_ALT_ZEN_L2_LIM_FLT , /* Altitude Zenith L2 Limit Fault */
   D_SPT_CAS_ACW_L2_LIM_FLT , /* Cassegrain ACW L2 Limit Fault */
   D_SPT_CAS_CW_L2_LIM_FLT  , /* Cassegrain CW L2 Limit Fault */
   D_SPT_NASP_ACW_L2_LIM_FLT, /* Nasmyth Plus ACW L2 Limit Fault */
   D_SPT_NASP_CW_L2_LIM_FLT , /* Nasmyth Plus CW L2 Limit Fault */ 
   D_SPT_NASM_ACW_L2_LIM_FLT, /* Nasmyth Minus ACW L2 Limit Fault */
   D_SPT_NASM_CW_L2_LIM_FLT , /* Nasmyth Minus CW L2 Limit Fault */ 
   D_SPT_ALT_L3_K8_FLT      , /* Altitude L3 Limit Relay K8 Fault */
   D_SPT_ALT_L3_K7_FLT      , /* Altitude L3 Limit Relay K7 Fault */
   D_SPT_CAS_L3_K6_FLT      , /* Cassegrain L3 Limit Relay K6 Fault */
   D_SPT_CAS_L3_K5_FLT      , /* Cassegrain L3 Limit Relay K5 Fault */
   D_SPT_NASP_L3_K2_FLT     , /* Nasmyth Plus L3 Limit Relay K2 Fault */
   D_SPT_NASP_L3_K1_FLT     , /* Nasmyth Plus L3 Limit Relay K1 Fault */
   D_SPT_NASM_L3_K4_FLT     , /* Nasmyth Minus L3 Limit Relay K4 Fault */
   D_SPT_NASM_L3_K3_FLT     , /* Nasmyth Minus L3 Limit Relay K3 Fault */
   D_SPT_AZM_AMP1_DIS_FLT   , /* Azimuth Amplifier 1 Disabled Status */
   D_SPT_AZM_AMP2_DIS_FLT   , /* Azimuth Amplifier 2 Disabled Status */
   D_SPT_ALT_AMP1_DIS_FLT   , /* Altitude Amplifier 1 Disabled Status */
   D_SPT_ALT_AMP2_DIS_FLT   , /* Altitude Amplifier 2 Disabled Status */
   D_SPT_ROT_AMP1_DIS_FLT   , /* Rotator Amplifier 1 Disabled Status */
   D_SPT_ROT_AMP2_DIS_FLT   , /* Rotator Amplifier 2 Disabled Status */
   D_SPT_HPLC_PT01_SENS_STAT, /* OSS Pressure Sensor PT01 Status */
   D_SPT_HPLC_PT02_SENS_STAT, /* OSS Pressure Sensor PT02 Status */
   D_SPT_HPLC_PT03_SENS_STAT, /* OSS Pressure Sensor PT03 Status */
   D_SPT_HPLC_PT04_SENS_STAT, /* OSS Pressure Sensor PT04 Status */
   D_SPT_HPLC_PT05_SENS_STAT, /* OSS Pressure Sensor PT05 Status */
   D_SPT_HPLC_PT06_SENS_STAT, /* OSS Pressure Sensor PT06 Status */
   D_SPT_HPLC_PT07_SENS_STAT, /* OSS Pressure Sensor PT07 Status */
   D_SPT_HPLC_PT08_SENS_STAT, /* OSS Pressure Sensor PT08 Status */
   D_SPT_HPLC_PT09_SENS_STAT, /* OSS Pressure Sensor PT09 Status */
   D_SPT_HPLC_PT10_SENS_STAT, /* OSS Pressure Sensor PT10 Status */
   D_SPT_HPLC_PT11_SENS_STAT, /* OSS Pressure Sensor PT11 Status */
   D_SPT_HPLC_PT12_SENS_STAT, /* OSS Pressure Sensor PT12 Status */
   D_SPT_HPLC_PT13_SENS_STAT, /* OSS Pressure Sensor PT13 Status */
   D_SPT_HPLC_PT14_SENS_STAT, /* OSS Pressure Sensor PT14 Status */
   D_SPT_HPLC_TT01_SENS_STAT, /* OSS Temperature Sensor TT01 Status */
   D_SPT_HPLC_TT02_SENS_STAT, /* OSS Temperature Sensor TT02 Status */
   D_SPT_HPLC_TT03_SENS_STAT, /* OSS Temperature Sensor TT03 Status */
   D_SPT_HPLC_TT04_SENS_STAT, /* OSS Temperature Sensor TT04 Status */
   D_SPT_HPLC_LT01_SENS_STAT, /* OSS Level Sensor LT01 Status */
   D_SPT_HPLC_FT01_SENS_STAT, /* OSS Flow Sensor FT01 Status */
                              
   D_SPT_PWR_AZBRK          , /* Azimuth Brake Panel Power */
   D_SPT_PWR_MRES           , /* Mirror Restraint Panel Power */
   D_SPT_PWR_BBOX_FAN       , /* Base Box Fans Power */
   D_SPT_TRAP_KEY_AA        , /* Trapped Key Aa Status */
   D_SPT_TRAP_KEY_BB        , /* Trapped Key Bb Status */
   D_SPT_TRAP_KEY_BC        , /* Trapped Key Bc Status */
   D_SPT_TRAP_KEY_BD        , /* Trapped Key Bd Status */
   D_SPT_MRES_SYS           , /* Overall Mirror Restraint System Status */
   D_SPT_MRES_REDYEL        , /* Mirror Restraint Red-Yellow Sector Status */
   D_SPT_MRES_YELBLU        , /* Mirror Restraint Yellow-Blue Sector Status */
   D_SPT_MRES_BLURED        , /* Mirror Restraint Blue-Red-Sector Status */
   D_SPT_HPLC_MCB_F1_STAT   , /* OSS MCB F1 Main Power Detector Status */
   D_SPT_HPLC_MCB_F2_STAT   , /* OSS MCB F2 Main Power UPS Status */
   D_SPT_HPLC_MCB_F6_STAT   , /* OSS MCB F6 240V Control Voltage Status */
   D_SPT_HPLC_MCB_F3_STAT   , /* OSS MCB F3 Control Panel Heaters Status */
   D_SPT_HPLC_MCB_F4_STAT   , /* OSS MCB F4 Control Panel Fan Status */
   D_SPT_HPLC_MCB_F21_STAT  , /* OSS MCB F21 Power Supply Unit 1 Status */
   D_SPT_HPLC_MCB_F22_STAT  , /* OSS MCB F22 Power Supply Unit 2 Status */
   D_SPT_HPLC_PSU_T1_STAT   , /* OSS Power Supply Unit 1 Status */
   D_SPT_HPLC_PSU_T2_STAT   , /* OSS Power Supply Unit 2 Status */
   D_SPT_HPLC_MCB_F100_STAT , /* OSS MCB F100 24V Control Voltage Status */
   D_SPT_HPLC_MCB_F101_STAT , /* OSS MCB F101 Signal Oil Unit Status */
   D_SPT_HPLC_MCB_F102_STAT , /* OSS MCB F102 PLC Outputs via E-Stop Status */
   D_SPT_HPLC_MCB_F103_STAT , /* OSS MCB F103 PLC Outputs Status */
   D_SPT_HPLC_VAC_SW_STAT   , /* OSS Vacuum Switch Status */
   D_SPT_HPLC_AFREE_SW_STAT , /* OSS Anti Freeze Switch Status */
   D_SPT_HPLC_FQC1_STAT     , /* OSS Frequency Current Converter Status */
   D_SPT_HPLC_GF1_SW_STAT   , /* OSS Compressor Flow Switch GF1 Status */
   D_SPT_HPLC_LP_SW_STAT    , /* OSS Compressor High Low Pressure Switch Status */
   D_SPT_HPLC_FILT75_STAT   , /* OSS Filter 75 percent Status */
   D_SPT_HPLC_FILT100_STAT  , /* OSS Filter 100 percent Status */
   D_SPT_HPLC_ILOCK_STAT    , /* OSS External Interlock Status */
                              
   D_SPT_NASP_LIMITS        , /* Nasmyth Plus Limits */
   D_SPT_NASM_LIMITS        , /* Nasmyth Minus Limits */
                              
   D_SPT_NASP_M1_TEMP       , /* Nasmyth Plus Motor 1 Temperature Status */
   D_SPT_NASP_M1_TEMP_VAL   , /* Nasmyth Plus Motor 1 Temperature Value */ 
   D_SPT_NASP_M2_TEMP       , /* Nasmyth Plus Motor 2 Temperature Status */
   D_SPT_NASP_M2_TEMP_VAL   , /* Nasmyth Plus Motor 2 Temperature Value */ 
   D_SPT_NASM_M1_TEMP       , /* Nasmyth Minus Motor 1 Temperature Status */
   D_SPT_NASM_M1_TEMP_VAL   , /* Nasmyth Minus Motor 1 Temperature Value */ 
   D_SPT_NASM_M2_TEMP       , /* Nasmyth Minus Motor 2 Temperature Status */
   D_SPT_NASM_M2_TEMP_VAL   , /* Nasmyth Minus Motor 2 Temperature Value */ 
   D_SPT_SPLC_TEMP          , /* SPLC Electrical Cabinet Temperature Status */
   D_SPT_SPLC_TEMP_VAL      , /* SPLC Electrical Cabinet Temperature Value */ 
   D_SPT_HPLC_TEMP          , /* OSS Electrical Cabinet Temperature Status */
   D_SPT_HPLC_TEMP_VAL      , /* OSS Electrical Cabinet Temperature Value */ 
   D_SPT_HPLC_TEMP_TT1      , /* HPLC Reservoir Oil Temperature TT1 Status */
   D_SPT_HPLC_TEMP_TT1_VAL  , /* HPLC Reservoir Oil Temperature TT1 Value */ 
   D_SPT_HPLC_TEMP_TT2      , /* HPLC Pressure Line Oil Temperature TT2 Status */
   D_SPT_HPLC_TEMP_TT2_VAL  , /* HPLC Pressure Line Oil Temperature TT2 Value */ 
   D_SPT_HPLC_TEMP_TT3      , /* HPLC Return Line Oil Temperature TT3 Status */
   D_SPT_HPLC_TEMP_TT3_VAL  , /* HPLC Return Line Oil Temperature TT3 Value */ 
   D_SPT_HPLC_TEMP_TT4      , /* HPLC Ambient Temperature Status */
   D_SPT_HPLC_TEMP_TT4_VAL  , /* HPLC Ambient Temperature Value */ 
   D_SPT_HPLC_TT3_TT4       , /* OSS Oil Temperature TT3 to TT4 Difference Status */
   D_SPT_HPLC_TT3_TT4_VAL   , /* OSS Oil Temperature TT3 to TT4 Difference Value */ 
   D_SPT_HPLC_FQC1_SETPT    , /* HPLC Frequency Current Converter - Set Point Status */
   D_SPT_HPLC_FQC1_SETPT_VAL, /* HPLC Frequency Current Converter - Set Point Value */ 
   D_SPT_HPLC_FQC1_CONT     , /* HPLC Frequency Current Converter - Cont Value Status */
   D_SPT_HPLC_FQC1_CONT_VAL , /* HPLC Frequency Current Converter - Cont Value Value */ 
   D_SPT_HPLC_FQC1_VALUE    , /* HPLC Frequency Current Converter - Actual Value Status */
   D_SPT_HPLC_FQC1_VALUE_VAL, /* HPLC Frequency Current Converter - Actual Value Value */ 
   D_SPT_HYD_OIL_FLOW       , /* Oil Flow Transmitter FT1 Status */
   D_SPT_HYD_OIL_FLOW_VAL   , /* Oil Flow Transmitter FT1 Value */ 
   D_SPT_HYD_OIL_LEVEL      , /* Main Hydraulic Tank Oil Level LT1 Status */
   D_SPT_HYD_OIL_LEVEL_VAL  , /* Main Hydraulic Tank Oil Level LT1 Value */ 
   D_SPT_HYD_PT01_PRES      , /* Oil Pressure Transmitter PT01 Status - Azimuth Load */
   D_SPT_HYD_PT01_PRES_VAL  , /* Oil Pressure Transmitter PT01 Value - Azimuth Load */
   D_SPT_HYD_PT02_PRES      , /* Oil Pressure Transmitter PT02 Status - Azimuth Load */
   D_SPT_HYD_PT02_PRES_VAL  , /* Oil Pressure Transmitter PT02 Value - Azimuth Load */
   D_SPT_HYD_PT03_PRES      , /* Oil Pressure Transmitter PT03 Status - Azimuth Load */
   D_SPT_HYD_PT03_PRES_VAL  , /* Oil Pressure Transmitter PT03 Value - Azimuth Load */
   D_SPT_HYD_PT04_PRES      , /* Oil Pressure Transmitter PT04 Status - Azimuth Guide */
   D_SPT_HYD_PT04_PRES_VAL  , /* Oil Pressure Transmitter PT04 Value - Azimuth Guide */
   D_SPT_HYD_PT05_PRES      , /* Oil Pressure Transmitter PT05 Status - Azimuth Guide */
   D_SPT_HYD_PT05_PRES_VAL  , /* Oil Pressure Transmitter PT05 Value - Azimuth Guide */
   D_SPT_HYD_PT06_PRES      , /* Oil Pressure Transmitter PT06 Status - Azimuth Guide */
   D_SPT_HYD_PT06_PRES_VAL  , /* Oil Pressure Transmitter PT06 Value - Azimuth Guide */
   D_SPT_HYD_PT07_PRES      , /* Oil Pressure Transmitter PT07 Status - Altitude Guide */
   D_SPT_HYD_PT07_PRES_VAL  , /* Oil Pressure Transmitter PT07 Value - Altitude Guide */
   D_SPT_HYD_PT08_PRES      , /* Oil Pressure Transmitter PT08 Status - Altitude Load */
   D_SPT_HYD_PT08_PRES_VAL  , /* Oil Pressure Transmitter PT08 Value - Altitude Load */
   D_SPT_HYD_PT09_PRES      , /* Oil Pressure Transmitter PT09 Status - Altitude Guide */
   D_SPT_HYD_PT09_PRES_VAL  , /* Oil Pressure Transmitter PT09 Value - Altitude Guide */
   D_SPT_HYD_PT10_PRES      , /* Oil Pressure Transmitter PT10 Status - Altitude Load */
   D_SPT_HYD_PT10_PRES_VAL  , /* Oil Pressure Transmitter PT10 Value - Altitude Load */
   D_SPT_HYD_PT11_PRES      , /* Oil Pressure Transmitter PT11 Status - Altitude Load */
   D_SPT_HYD_PT11_PRES_VAL  , /* Oil Pressure Transmitter PT11 Value - Altitude Load */
   D_SPT_HYD_PT12_PRES      , /* Oil Pressure Transmitter PT12 Status - Altitude Load */
   D_SPT_HYD_PT12_PRES_VAL  , /* Oil Pressure Transmitter PT12 Value - Altitude Load */
   D_SPT_HYD_PT13_PRES      , /* Oil Pressure Transmitter PT13 Status - Pressure Line */
   D_SPT_HYD_PT13_PRES_VAL  , /* Oil Pressure Transmitter PT13 Value - Pressure Line */
   D_SPT_HYD_PT14_PRES      , /* Oil Pressure Transmitter PT14 Status - Pre-charge Line */
   D_SPT_HYD_PT14_PRES_VAL  , /* Oil Pressure Transmitter PT14 Value - Pre-charge Line */
                              
   D_SPT_NASP_M1            , /* Nasmyth Plus Motor 1 Status */
   D_SPT_NASP_M2            , /* Nasmyth Plus Motor 2 Status */
   D_SPT_NASM_M1            , /* Nasmyth Minus Motor 1 Status */
   D_SPT_NASM_M2            , /* Nasmyth Minus Motor 2 Status */
   D_SPT_HPLC_M1_STAT       , /* HPLC Motor M1 Status - Hydraulic Pump Status */
   D_SPT_HPLC_M2_STAT       , /* HPLC Motor M2 Status - Oil Circulating Motor Status */
   D_SPT_HPLC_P1_STAT       , /* HPLC Pump P1 Status - Circulation Pump Status */
   D_SPT_HPLC_M3_STAT       , /* HPLC Motor M3 Status - Compressor Status */
   D_SPT_HPLC_P2_STAT       , /* HPLC Pump P2 Status - Circulation Pump Status */
   D_SPT_HPLC_M4_STAT       , /* HPLC Motor M4 Status - Compressor Fan Status */
   D_SPT_HPLC_M5_STAT       , /* HPLC Motor M5 Status - Compressor Fan Status */
                              
   D_SPT_NASP_BRK           , /* Nasmyth Plus Brakes Status */
   D_SPT_NASM_BRK           , /* Nasmyth Minus Brakes Status */
   D_SPT_HPLC_SV1           , /* HPLC Solenoid SV1 Status */
   D_SPT_HPLC_SV2           , /* HPLC Solenoid SV2 Status */
   D_SPT_NASP_BRK1          , /* Nasmyth Plus Brake 1 Status */
   D_SPT_NASP_BRK2          , /* Nasmyth Plus Brake 1 Status */
   D_SPT_NASM_BRK1          , /* Nasmyth Minus Brake 1 Status */
   D_SPT_NASM_BRK2          , /* Nasmyth Minus Brake 2 Status */
   D_SPT_SPLC_ESTOP         , /* SPLC Emergency Stop System */
   D_SPT_HPLC_ESTOP         , /* HPLC Emergency Stop System */
                              
   D_SPT_HPLC_STATUS        , /* HPLC Status */
                              
   D_SPT_HPLC_VER_STATUS    , /* HPLC Version Status */
   D_SPT_HPLC_VER_NUMBER    , /* HPLC Version Number */
   D_SPT_HHMI_VER_STATUS    , /* HPLC HMI Version Status */
   D_SPT_HHMI_VER_NUMBER    , /* HPLC HMI Version Number */
                              
   D_SPT_HPLC_SYSTEM_STATUS , /* HPLC System Status */

   D_SPT_P55_MCB_STAT       , /* Rotator Select Panel Power Status */
   D_SPT_P55_TEMP           , /* Rotator Select Panel Temperature Status */
   D_SPT_P55_TEMP_VAL       , /* Rotator Select Panel Temperature Value */

   D_SPT_KEY_BC_AZM_ACW     , /* Trapped Key Bc Azimuth L3 Anti-Clockwise */
   D_SPT_KEY_BC_AZM_CW      , /* Trapped Key Bc Azimuth L3 Clockwise */
   D_SPT_KEY_BC_ALT_ACW     , /* Trapped Key Bc Altitude L3 Horizon */
   D_SPT_KEY_BC_ALT_CW      , /* Trapped Key Bc Altitude L3 Zenith */
   D_SPT_KEY_BC_CAS_ACW     , /* Trapped Key Bc Cassegrain L3 Anti-Clockwise */
   D_SPT_KEY_BC_CAS_CW      , /* Trapped Key Bc Cassegrain L3 Clockwise */
   D_SPT_KEY_BC_NASP_ACW    , /* Trapped Key Bc Nasmyth+ L3 Anti-Clockwise */
   D_SPT_KEY_BC_NASP_CW     , /* Trapped Key Bc Nasmyth+ L3 Clockwise */
   D_SPT_KEY_BC_NASM_ACW    , /* Trapped Key Bc Nasmyth- L3 Anti-Clockwise */
   D_SPT_KEY_BC_NASM_CW     , /* Trapped Key Bc Nasmyth- L3 Clockwise */

   D_SPT_STATUS_EOL         ,  /* End of status list */

   D_SPT_DATAID_EOL            /* End of list marker */

} eSptDataId_t;


/* Commands from T2-010 interface re-used for the T2-408 interface. */

typedef enum eSpiDataId_e
{
   D_SPT_DMD_SEL_CAS    = D_SPT_DMD_SW_THTG,    /* Select Cassegrain rotator */
   D_SPT_DMD_SEL_NASP   = D_SPT_DMD_SW_SCV_PWR, /* Select Nasmyth Plus rotator */
   D_SPT_DMD_SEL_NASM   = D_SPT_DMD_SW_RIO_PWR, /* Select Nasmyth Minus rotator */
   D_SPT_DMD_SW_AZB_PWR = D_SPT_DMD_SW_CR1_PWR, /* Switch Azimuth Brake Power On/Off */
   D_SPT_DMD_SW_PMR_PWR = D_SPT_DMD_SW_CR2_PWR, /* Switch Mirror Restraint Power On/Off */
   D_SPT_DMD_HPLC_MUTE  = D_SPT_DMD_SW_BBP_PWR  /* HPLC Mute Alarm */
} eSpiDataId_t;


#endif

/*******************************************************************************
** End of File Name: Spt.h
*******************************************************************************/
