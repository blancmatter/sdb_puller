/*
** File name : Hhe.h
**
** Purpose : Heidenhain (IK220) Driver
**           package definitions file.
**
** Description :
** Contains definitions, macros and types that are available to users
** of the Heidenhain Encoder QNX Driver software.
**
** Authors:
**    DEH:     D. E. Hughes  (TTL).
**    RDL:     R. D. Lett    (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**    $Id: Hhe.h,v 0.18 2007/07/18 12:41:37 mjf Exp $
**
** History:
**    $Log: Hhe.h,v $
**    Revision 0.18  2007/07/18 12:41:37  mjf
**    Change of enumeration constructions for automatic lookup table generation.
**    No functional changes.
**
**    Revision 0.17  2006/10/17 16:34:38  mjf
**    Removal of redundant item.
**
**    Revision 0.16  2005/06/17 11:45:25  mjf
**    Tidied up in line with HHE_0_22. Additional (unproven) functions added
**    to read reference pulse edges.
**
**    Revision 0.15  2005/05/13 13:16:43  mjf
**    Consistent with TQB_0_44.
**
**    Revision 0.14  2004/10/27 13:46:19  mkb
**    Add port definitions for secondary mirror linear encoders.
**
**    Revision 0.13  2004/06/21 11:09:00  asp
**    Updated driver version number to 1.04
**
**    Revision 0.12  2004/06/08 09:18:35  asp
**    Added context structure for parallel reads.
**
**    Revision 0.11  2004/04/20 15:09:24  asp
**    EnDat encoder support added.
**
**    Revision 0.10  2004/02/17 10:03:03  sxf
**    Port to Q6 using Wfl library.
**
**    Revision 0.9  2002/03/22 12:08:32  mjf
**    Code added to implement functions IK220Status, IK220GetCorrA,
**    IK220GetCorrB and IK220LoadCorrA (currently untested).
**
**    Revision 0.8  2002/03/05 17:21:10  djm
**    Added more enumerations to the HHE signal state list.
**
**    Revision 0.7  2002/01/23 17:51:24  djm
**    Updated the version number.
**
**    Revision 0.6  2002/01/18 17:30:31  djm
**    Added a new signal amplitude state (E_HHE_AMPLITUDE_NORMAL)
**    and put in comments for the others that already existed.
**
**    Revision 0.5  2002/01/11 11:13:44  rdl
**    Spelling correction in comment.
**
**    Revision 0.4  2001/11/05 15:39:50  rdl
**    Added standard information to comments at top of file to comform to
**    TTL coding standard.
**    (Copyright Notice, Author, RCS Version and History tags).
**
**
*/

#ifndef _HHE_HEADER_INCLUDED
#define _HHE_HEADER_INCLUDED

#include "TtlSystem.h"

#ifdef _HHE_MAIN

    #define HHE_EXTERN
    #define HHE_INIT(Initialisation) = Initialisation
#else
    
    #define HHE_EXTERN extern
    #define HHE_INIT(Initialisation)

#endif 

/*
**  Generic Type Definitions
**  ========================
*/

typedef unsigned        eHheLatch_t;            /* Latch Number */
typedef Int32_t         eHheMilliArcSecs_t;     /* Position in milli arc seconds */
typedef double          eHhe48BitValue_t;       /* Support for 48 Bit Numbers */


/*
**  Status Value Definitions
**  ========================
*/

typedef enum eHheStatus_e
{
    E_HHE_SHM_OPEN_FAILURE = STATUS_START(HHE),
    E_HHE_QNX_OSINFO_ERROR,
    E_HHE_NO_PCI_BIOS,
    E_HHE_CARD_NOT_PRESENT,
    E_HHE_MMAP_FAILURE,
    E_HHE_MUNMAP_FAILURE,
    E_HHE_PCI_NOT_INITIALISED,
    E_HHE_PCI_READ_CONFIG_BYTE_FAILURE,
    E_HHE_PCI_READ_CONFIG_DWORD_FAILURE,
    E_HHE_INVALID_CARD_ID,
    E_HHE_INVALID_PORT_ID,
    E_HHE_INVALID_ENCODER_ID,
    E_HHE_LATCH_NUMBER_TOO_HIGH,
    E_HHE_VALUE_NOT_LATCHED,
    E_HHE_INVALID_REGISTER_ADDRESS,
    E_HHE_INVALID_BUFFER_ADDRESS,
    E_HHE_COMMAND_TIMEOUT,
    E_HHE_PIPE_TIMEOUT,
    E_HHE_WRONG_ACKNOWLEDGE,
    E_HHE_IK220_WRITE_PARAMETER_ERROR,
    E_HHE_IK220_READ_PARAMETER_ERROR,
    E_HHE_WRONG_ENCODER_STATE,
    
    #ifdef _TESTBED_SUPPORT
        E_HHE_INVALID_TESTBED_COMMAND,
        E_HHE_INVALID_TESTBED_PARAMETER,
    #endif

    E_HHE_SOFTWARE_ERROR = STATUS_START(HHE) + 0xFFFF

} eHheStatus_t;

/*
** Encoder Identities for the Axis Nodes
** Some of the enumeration values are now duplicated to allow for an Hhe
** abs. encoder and Naysmyth but to remain backwardly compatible with 
** existing test software.
*/

typedef enum eHheEncoderID_e
{
    E_HHE_MOTOR_1 = 0,
    E_HHE_AUX_ENCODER_1 = E_HHE_MOTOR_1,    /* Length Encoder - development   */
    E_HHE_MOTOR_2,
    E_HHE_AUX_ENCODER_2 = E_HHE_MOTOR_2,    /* Length Encoder - development   */
    E_HHE_AXIS_ENCODER_1,
    E_HHE_AUX_ENCODER_3 = E_HHE_AXIS_ENCODER_1, /* Length Encoder - development   */
    E_HHE_AXIS_ENCODER_2,
    E_HHE_AUX_ENCODER_4 = E_HHE_AXIS_ENCODER_2, /* Length Encoder - development   */
    E_HHE_AXIS_ENCODER_3,                       /* Azimuth Only */
    E_HHE_LIN_ENC_1   = E_HHE_AXIS_ENCODER_3,   /* Linear encoder for secondary tilt. */
    E_HHE_AXIS_ENCODER_4,                       /* Azimuth Only */
    E_HHE_LIN_ENC_2   = E_HHE_AXIS_ENCODER_4,   /* Linear encoder for secondary tilt. */
    E_HHE_ABS_ENCODER,                          /* Azm abs. encoder - if no Naysmyth  */
    E_HHE_LIN_ENC_3   = E_HHE_ABS_ENCODER,      /* Linear encoder for secondary tilt. */
    E_HHE_NAY_MOTOR_1 = E_HHE_ABS_ENCODER,      /* Naysmth 1 */
    E_HHE_NAY_MOTOR_2,
    E_HHE_NAY_AXIS_ENCODER_1,
    E_HHE_NAY_AXIS_ENCODER_2,
    E_HHE_ABS_ENCODER_IF_NAYSMYTH,          /* Azm abs. encoder - if Naysmyth */
    
    E_HHE_MAX_NUMBER_OF_ENCODERS = 16       /* Used for range checking and determining */
                                            /* maximum number of cards */
} eHheEncoderID_t;

typedef enum eHheParameterID_e
{                               /* Heidenhain Card Parameter IDs */
    E_HHE_ENCODER_INPUT_SELECT   = 1,       /* Default = E_HHE_INCREMENTAL_ENCODER   */
    E_HHE_INCREMENTAL_INTERFACE_SELECT,     /* Default = E_HHE_1_VOLT_SS_INTERFACE   */
    E_HHE_LINEAR_ANGULAR_SELECT,            /* Default = E_HHE_LINEAR_ENCODER        */
    E_HHE_COUNTING_DIRECTION,               /* Default = E_HHE_POSITIVE_DIRECTION    */
    E_HHE_SIG_PERIOD_PER_ROTATION,          /* Default = 0 (Range = 0 - 0xFFFFFFFF)  */
    E_HHE_REFERENCE_MARKER_SELECT,          /* Default = E_HHE_SINGLE_REF_MARK       */
    E_HHE_NO_OF_INTERPOLATION_BITS,         /* Default = 12 (Range = 0 - 12)         */
    E_HHE_POSITION_CORRECTION_ENABLE,       /* Default = E_HHE_DISABLE               */
    E_HHE_CORRECTION_DETERMINATION_ENABLE,  /* Default = E_HHE_DISABLE               */
    E_HHE_SIGNAL_POINTS_PER_OKTAND,         /* Default = 8 (Range = 1 - 8)           */
    E_HHE_TIMER_PERIOD_SELECT,              /* Default = E_HHE_1000_MICROSECS        */
    E_HHE_TIMER_PRESCALER,                  /* Default = 1 (Range = 1 - 0xFFFF)      */
    E_HHE_NO_OF_VALUES_PER_CYCLE_IN_INT_RAM,/* Default = 8191 (Range = 1 - 8191)     */
    E_HHE_LATCH_SELECT,                     /* Default = E_HHE_NO_LATCH              */
    E_HHE_SSI_CODE_LENGTH,                  /* Default = 19 (Range = 1 - 48)         */
    E_HHE_SSI_PARITY_SELECT,                /* Default = E_HHE_SSI_EVEN_PARITY       */
    E_HHE_SSI_GRAY_BINARY_CONVERT_ENABLE,   /* Default = E_HHE_ENABLE                */
    E_HHE_SSI_NO_OF_LEADING_ZEROES,         /* Default = 0 (Range = 0 - 20)          */
    E_HHE_SSI_NO_OF_TRAILING_ZEROES         /* Default = 0 (Range = 0 - 20)          */
} eHheParameterID_t;

/*
**  Max and Min Range Values
*/

#define E_HHE_MIN_SIG_PERIOD_PER_ROTATION       0
#define E_HHE_MAX_SIG_PERIOD_PER_ROTATION       0xFFFFFFFF

#define E_HHE_MIN_NO_OF_INTERPOLATION_BITS      0
#define E_HHE_MAX_NO_OF_INTERPOLATION_BITS      12

#define E_HHE_MIN_SIGNAL_POINTS_PER_OKTAND      1
#define E_HHE_MAX_SIGNAL_POINTS_PER_OKTAND      8

#define E_HHE_MIN_TIMER_PRESCALER               1
#define E_HHE_MAX_TIMER_PRESCALER               0xFFFF

#define E_HHE_MIN_NO_OF_VALUES_PER_CYCLE        1
#define E_HHE_MAX_NO_OF_VALUES_PER_CYCLE        8191

#define E_HHE_MIN_SSI_CODE_LENGTH               1
#define E_HHE_MAX_SSI_CODE_LENGTH               48

#define E_HHE_MIN_SSI_NO_OF_LEADING_ZEROES      0
#define E_HHE_MAX_SSI_NO_OF_LEADING_ZEROES      20

#define E_HHE_MIN_SSI_NO_OF_TRAILING_ZEROES     0
#define E_HHE_MAX_SSI_NO_OF_TRAILING_ZEROES     20

#define E_HHE_ABS_ENCODER_UNUSED_BITS           0


/* 
** Parameter Values for E_HHE_ENCODER_INPUT_SELECT
*/

typedef enum eHheEncoderInputSelect_e 
{
    E_HHE_INCREMENTAL_ENCODER = 0,
    E_HHE_EN_DAT_ENCODER,
    E_HHE_SSI_ENCODER
} eHheEncoderInputSelect_t;

/*
** Parameter Values for E_HHE_INCREMENTAL_INTERFACE_SELECT
** (Note: only used if E_HHE_ENCODER_INPUT_SELECT = E_HHE_INCREMENTAL_ENCODER)
*/

typedef enum eHheIncrementalInterfaceSelect_e 
{
    E_HHE_11_MICRO_AMP_SS_INTERFACE = 0,
    E_HHE_1_VOLT_SS_INTERFACE
} eHheIncrementalInterfaceSelect_t;

/* 
** Parameter Values for E_HHE_LINEAR_ANGULAR_SELECT
*/

typedef enum eHheLinearAngularSelect_e 
{ 
    E_HHE_LINEAR_ENCODER = 0,
    E_HHE_ANGULAR_ENCODER
} eHheLinearAngularSelect_t;

/* 
** Parameter Values for E_HHE_COUNTING_DIRECTION
*/

typedef enum eHheCountingDirection_e 
{
    E_HHE_POSITIVE_DIRECTION = 0,
    E_HHE_NEGATIVE_DIRECTION
} eHheCountingDirection_t;

/* 
** Parameter Values for E_HHE_REFERENCE_MARKER_SELECT
*/

typedef enum eHheReferenceMarkerSelect_e 
{
    E_HHE_SINGLE_REF_MARK               = 0,
    E_HHE_DIST_CODED_REF_MARK_500TP     = 500,
    E_HHE_DIST_CODED_REF_MARK_1000TP    = 1000,
    E_HHE_DIST_CODED_REF_MARK_2000TP    = 2000,
    E_HHE_DIST_CODED_REF_MARK_5000TP    = 5000
} eHheReferenceMarkerSelect_t;

/*
** Parameter Values for E_HHE_POSITION_CORRECTION_ENABLE, 
** E_HHE_CORRECTION_DETERMINATION_ENABLE and E_HHE_SSI_GRAY_BINARY_CONVERT_ENABLE
*/

typedef enum eHheEnableDisable_e 
{
    E_HHE_DISABLE = 0,
    E_HHE_ENABLE
} eHheEnableDisable_t;

/* 
** Parameter Values for E_HHE_TIMER_PERIOD_SELECT
*/

typedef enum eHheTimerPeriodSelect_e 
{
    E_HHE_50_MICROSECS = 16,
    E_HHE_100_MICROSECS,
    E_HHE_150_MICROSECS,
    E_HHE_200_MICROSECS,
    E_HHE_250_MICROSECS,
    E_HHE_300_MICROSECS,
    E_HHE_350_MICROSECS,
    E_HHE_400_MICROSECS,
    E_HHE_450_MICROSECS,
    E_HHE_500_MICROSECS,
    E_HHE_550_MICROSECS,
    E_HHE_600_MICROSECS,
    E_HHE_650_MICROSECS,
    E_HHE_700_MICROSECS,
    E_HHE_750_MICROSECS,
    E_HHE_800_MICROSECS,
    E_HHE_900_MICROSECS,
    E_HHE_1000_MICROSECS,
    E_HHE_1100_MICROSECS,
    E_HHE_1200_MICROSECS,
    E_HHE_1300_MICROSECS,
    E_HHE_1400_MICROSECS,
    E_HHE_1500_MICROSECS,
    E_HHE_1600_MICROSECS,
    E_HHE_1800_MICROSECS,
    E_HHE_2000_MICROSECS,
    E_HHE_2200_MICROSECS,
    E_HHE_2400_MICROSECS,
    E_HHE_2600_MICROSECS,
    E_HHE_2800_MICROSECS,
    E_HHE_3000_MICROSECS,
    E_HHE_3200_MICROSECS
} eHheTimerPeriodSelect_t;


/* 
** Parameter Bit Settings for E_HHE_LATCH_SELECT
*/

typedef unsigned char eHheLatchSelect_t;

#define E_HHE_NO_LATCH                      0
#define E_HHE_EXTERNAL_LATCH_L0             1
#define E_HHE_INTERNAL_TIMER_LATCH_L0       2
#define E_HHE_EXTERNAL_LATCH_L1             4

/* 
** Parameter Values for E_HHE_SSI_PARITY_SELECT
*/

typedef enum eHheSsiParitySelect_e 
{
    E_HHE_SSI_NO_PARITY = 0,
    E_HHE_SSI_EVEN_PARITY,
    E_HHE_SSI_EVEN_PARITY_LEADING_0
} eHheSsiParitySelect_t;

/*
** Definitions for scalar parameter values
*/

typedef Uint32_t    eHheSigPeriodPerRotation_t;
typedef Uint16_t    eHheNoOfInterpolationBits_t;
typedef Uint16_t    eHheSignalPointsPerOktand_t;
typedef Uint16_t    eHheTimerPrescaler_t;
typedef Uint16_t    eHheNoOfValuesPerCycleInIntRam_t;
typedef Uint16_t    eHheSsiCodeLength_t;
typedef Uint16_t    eHheSsiNoOfLeadingZeroes_t;
typedef Uint16_t    eHheSsiNoOfTrailingZeroes_t;


/*
** Structure defining complete parameter set
*/

typedef struct eHheParameterSet_e
{ 
    eHheEncoderInputSelect_t            EncoderInputSelect;
    eHheIncrementalInterfaceSelect_t    IncrementalInterfaceSelect;
    eHheLinearAngularSelect_t           LinearAngularSelect;
    eHheCountingDirection_t             CountingDirection;
    eHheSigPeriodPerRotation_t          SigPeriodPerRotation;
    eHheReferenceMarkerSelect_t         ReferenceMarkerSelect;
    eHheNoOfInterpolationBits_t         NoOfInterpolationBits;
    eHheEnableDisable_t                 PositionCorrectionEnable;
    eHheEnableDisable_t                 CorrectionDeterminationEnable;
    eHheSignalPointsPerOktand_t         SignalPointsPerOktand;
    eHheTimerPeriodSelect_t             TimerPeriodSelect;
    eHheTimerPrescaler_t                TimerPrescaler;
    eHheNoOfValuesPerCycleInIntRam_t    NoOfValuesPerCycleInIntRam;
    eHheLatchSelect_t                   LatchSelect;
    eHheSsiCodeLength_t                 SsiCodeLength;
    eHheSsiParitySelect_t               SsiParitySelect;
    eHheEnableDisable_t                 SsiGrayBinaryConvertEnable;
    eHheSsiNoOfLeadingZeroes_t          SsiNoOfLeadingZeroes;
    eHheSsiNoOfTrailingZeroes_t         SsiNoOfTrailingZeroes;
} eHheParameterSet_t;

/*
** Overall Encoder Status Values
*/

typedef enum eHheEncoderState_e
{
    E_HHE_ENCODER_NOT_PRESENT,
    E_HHE_ENCODER_PRESENT,
    E_HHE_ENCODER_READY_UNHOMED,
    E_HHE_ENCODER_RUNNING_UNHOMED,
    E_HHE_ENCODER_READY_HOMING,
    E_HHE_ENCODER_RUNNING_HOMING,
    E_HHE_ENCODER_READY_HOMED,
    E_HHE_ENCODER_RUNNING_HOMED,
    E_HHE_ENCODER_ERROR
} eHheEncoderState_t;


/*
** Definitions specific to EnDat encoders
*/
typedef struct
{
   Uint16_t  Status;
   Uint16_t  Type;
   Uint32_t  Period;
   Uint32_t  Step;
   Uint16_t  Turns;
   Uint16_t  RefDist;
   Uint16_t  CntDir;
} eHheEnDatConfig_t;


/*
** Definitions for Signal Status Requests
*/

typedef enum eHheFrequencyStatus_e
{ 
    E_HHE_FREQUENCY_OK = 0,
    E_HHE_FREQUENCY_EXCEEDED
} eHheFrequencyStatus_t;

typedef enum eHheAmplitudeStatus_e
{
    E_HHE_AMPLITUDE_NONE      = -3,  /* This is a TTL definition (= no card) */
    E_HHE_AMPLITUDE_INIT      = -2,  /* This is a TTL definition (= init) */
    E_HHE_AMPLITUDE_UNKNOWN   = -1,  /* This is a TTL definition (= unknown) */

    E_HHE_AMPLITUDE_NORMAL    = 0,  /* This is a Heidenhain definition */
    E_HHE_AMPLITUDE_SMALL     = 1,  /*  - - ditto - - */
    E_HHE_AMPLITUDE_TOO_HIGH  = 2,  /*  - - ditto - - */
    E_HHE_AMPLITUDE_TOO_SMALL = 3   /*  - - ditto - - */
} eHheAmplitudeStatus_t;

typedef struct
{
    eHheFrequencyStatus_t   Frequency;
    eHheAmplitudeStatus_t   MinAmplitude;
    eHheAmplitudeStatus_t   ActualAmplitude;
    eHheAmplitudeStatus_t   MaxAmplitude;
} eHheSignalStatus_t;    

/*
** Definitions for Reference Status Requests
*/

typedef enum eHheEncoderHomeState_e
{
    E_HHE_NO_REF_TRAVERSE = 0,
    E_HHE_WAITING_FOR_FIRST_REFERENCE_MARK,
    E_HHE_WAITING_FOR_SECOND_REFERENCE_MARK,
    E_HHE_REF_TRAVERSE_COMPLETE
} eHheEncoderHomeState_t;


typedef struct
{
    Uint32_t                FirstRefMark;
    Uint32_t                SecondRefMark;
    Uint32_t                CalculatedDifference;
    Uint32_t                CalculatedOffset;
    eHheEncoderHomeState_t  EncoderHomeState;
} eHheReferenceStatus_t;

/*
** Definition for Compensation Values
*/

typedef struct
{
   Uint16_t Offset0;
   Uint16_t Offset90;
   Uint16_t Phase0;
   Uint16_t Phase90;
   Uint16_t Symmetry0;
   Uint16_t Symmetry90;
   Uint16_t Flag1;
   Uint16_t Flag2;
} eHheCompensationValues_t;

/*
** Parallel read support
*/
typedef struct
{
   Bool_t             *ConfiguredPtr;
   Bool_t              Active;
   Uint16_t            LatchMask;
   Uint16_t            Latch;      
   Uint16_t            CardID;
   Uint16_t            PortID;
   eHheEncoderID_t     EncoderID;
   eHhe48BitValue_t   *CountPtr;
   eHheEncoderState_t *StatePtr;
} eHheParallelReadReq_t;

/*
** Definition of reference pulse edge data.
*/

typedef struct
{
   Uint16_t    Active;
   Uint16_t    Interpolation;
   Uint32_t    PeriodCount;
   Uint16_t    EdgeCorrected0;
   Uint16_t    EdgeCorrected90;
   Uint16_t    EdgeUncorrected0;
   Uint16_t    EdgeUncorrected90;
} eHheRefPulseEdge_t;


#ifdef _TESTBED_SUPPORT
    typedef enum{
        E_HHE_PCI_OPEN,
        E_HHE_PCI_CLOSE,
        E_HHE_INITIALISE,
        E_HHE_GET_PARAMETERS,
        E_HHE_SET_PARAMETERS,
        E_HHE_GET_ENCODER_STATUS,
        E_HHE_GET_ENCODER_STATES,
        E_HHE_START_STOP_COUNTER,
        E_HHE_START_STOP_HOME,
        E_HHE_GET_CALIBRATE_STATUS,
        E_HHE_HOME_ALL_ENCODERS
    }eHheTestbedHHECommandType_t;

    typedef struct{
        eHheEncoderState_t      State;
        eHheReferenceStatus_t   RefStatus;
        eHheSignalStatus_t      SigStatus;
        eHhe48BitValue_t        LatchRegisterValues[2];
    } eHheTestbedStatusResponse_t;
    

    typedef struct{

        eHheTestbedHHECommandType_t     CommandType;
        eHheEncoderID_t                 EncoderID;
        union {
            eHheParameterSet_t      ParameterSet;
        } Parameters;

    } eHheTestbedHHECommand_t; 
    
    typedef struct{

        eHheTestbedHHECommandType_t     CommandType;
        union {
            eHheParameterSet_t          ParameterSet;
            eHheTestbedStatusResponse_t Status;
            eHheEncoderState_t          State [E_HHE_MAX_NUMBER_OF_ENCODERS];
        } ResponseValues;

    } eHheTestbedHHEResponse_t; 

Status_t eHheProcessTestbedCommand (eHheTestbedHHECommand_t TestCommand,
                                    eHheTestbedHHEResponse_t *TestResponse);
#endif

#ifdef _HHE_DEBUG_ENABLED

    #define E_HHE_DEBUG_BUFFER_SIZE          128
    #define E_HHE_DEBUG_MAX_CHARS            E_HHE_DEBUG_BUFFER_SIZE - 1
    
    typedef char eHheDebugBuffer_t[E_HHE_DEBUG_BUFFER_SIZE];
    
    #ifdef _PHOTON_DEBUG_ENABLED

        HHE_EXTERN pid_t eHheDebugServerPid HHE_INIT(0); /* Debug Server Process ID */

    #endif

    HHE_EXTERN Bool_t eHheDebugLoggingEnabled HHE_INIT(TRUE);

#endif

/*
** Function Prototypes
*/


Status_t eHhePCIInit( void );
Status_t eHhePCIClose( void );

Status_t eHheInitialise (eHheEncoderID_t EncoderID);

Status_t eHheSetParameter (eHheEncoderID_t EncoderID, eHheParameterID_t ParameterID, Uint32_t ParameterValue);
Status_t eHheSetAllParameters (eHheEncoderID_t EncoderID, eHheParameterSet_t Parameters);

Status_t eHheReadParameter (eHheEncoderID_t EncoderID, eHheParameterID_t ParameterID, Uint32_t *ParameterValue);
Status_t eHheReadAllParameters (eHheEncoderID_t EncoderID, eHheParameterSet_t *Parameters);

eHheEncoderState_t eHheEncoderState (eHheEncoderID_t EncoderID);
Status_t eHheGetSignalStatus (eHheEncoderID_t EncoderID, eHheSignalStatus_t *SignalStatus);
Status_t eHheGetReferenceStatus (eHheEncoderID_t EncoderID, eHheReferenceStatus_t *ReferenceStatus);
Status_t eHheGetEncoderStatus( eHheEncoderID_t EncoderID, Uint32_t * pEncoderStatus );

Status_t eHheStartCounter (eHheEncoderID_t EncoderID);
Status_t eHheStopCounter (eHheEncoderID_t EncoderID);

Status_t eHheStartHome (eHheEncoderID_t EncoderID);
Status_t eHheStopHome (eHheEncoderID_t EncoderID);

Status_t eHheGetCounterValue (eHheEncoderID_t EncoderID, eHheLatch_t LatchID, Bool_t CheckLatched, 
                              eHhe48BitValue_t *CounterValue, eHheEncoderState_t *State);

Status_t eHheGetEnDatCounterValue (eHheEncoderID_t EncoderID, 
                                   eHhe48BitValue_t *CounterValue,
                                   eHheEncoderState_t *State,
                                   Uint32_t *StatusBits);
                                   
Status_t eHheGetEnDatConfig( eHheEncoderID_t EncoderID, eHheEnDatConfig_t *EnDatConfig );                                    

eHheMilliArcSecs_t  eHheGetNormalisedPosition (eHhe48BitValue_t EncoderCountValue, eHhe48BitValue_t EncoderZeroOffset,
                                               eHhe48BitValue_t EncoderCountsPerTurn, int WrapValue);

Status_t eHheReadCorrA( eHheEncoderID_t EncoderID, eHheCompensationValues_t * pCompVals );
Status_t eHheReadCorrB( eHheEncoderID_t EncoderID, eHheCompensationValues_t * pCompVals );
Status_t eHheLoadCorrA( eHheEncoderID_t EncoderID, eHheCompensationValues_t * pCompVals );

Status_t eHheReadRefPulseEdge( eHheEncoderID_t EncoderID, int Timeout,
                               eHheRefPulseEdge_t * pRefPulseEdge ); 

Status_t eHheReadRefPulseEdges( eHheEncoderID_t EncoderID, int Timeout,
                                eHheRefPulseEdge_t * pRefPulseEdge1, 
                                eHheRefPulseEdge_t * pRefPulseEdge2 );

Status_t eHheGetLatchedCounterValues   
   (
      eHheParallelReadReq_t *ParallelPtr, 
      Uint32_t              *SuccessMask
   );

                                   
Status_t eHheFlushDataPort( eHheEncoderID_t EncoderID );                                    

#endif
