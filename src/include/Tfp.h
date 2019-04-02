/*
**
** Module Name:
**       Tfp.h
**
** Purpose:
**
** Description:
**      This file provides the public prototypes and definitions
**      for use with the time and frequency processor package.
**
** Authors:
**    JDM:     J. D. Mills (TTL).
**
** Copyright (C) Telescope Technologies Limited (TTL).
**
** Version:
**    $Id: Tfp.h,v 0.24 2006/11/08 07:06:59 mjf Exp $
**
** History:
**    $Log: Tfp.h,v $
**    Revision 0.24  2006/11/08 07:06:59  mjf
**    Addition of command-line switch '-lockafter' to specify the number of
**    seconds allowed for lock before going Failed.
**
**    Revision 0.23  2004/06/07 15:06:48  mkb
**    Add error code for duplicate TFP pulse.
**
**    Revision 0.22  2004/05/19 09:11:31  mkb
**    Set QNX6 process priority.
**
**    Revision 0.21  2004/04/27 09:39:33  mkb
**    Removed explicit path to executable.
**
**    Revision 0.20  2004/04/01 10:35:43  sxf
**    Corrected pulse code modulation definition.
**
**    Revision 0.19  2004/02/17 10:33:40  sxf
**    Port to Q6 using Wfl library..
**
**    Revision 0.18  2003/10/14 08:54:02  sxf
**    Made number of attempts to lock when using -lockonly flag an Oid
**    ( D_TFP_MAX_FLYWHEELCOUNT ).
**
**    Revision 0.17  2001/11/29 13:07:18  djm
**    Added time status bit for "initialising".
**
**    Revision 0.16  2001/11/27 14:13:54  mjf
**    Report 'simulate' flag into the SDB.
**
**    Revision 0.15  2001/11/26 15:50:56  mjf
**    Submit a tally of the number of registered proxies of each type i
**    the SDB.
**
**    Revision 0.14  2001/11/26 12:14:13  mjf
**    Additional service to allow clients to register for a periodic proxy
**    which contains the system time in the message. Fix to the operation
**    of the 'simulate' mode. Tick frequency can now be specified on the
**    command-line, and is reported into the SDB.
**
**    Revision 0.13  2001/10/31 11:40:38  mjf
**    Go to 'warn' process state when flywheeling.
**
**    Revision 0.12  2001/09/25 10:20:43  mjf
**    Combining of the TFP library into the TFP package. Required as the TFP
**    requires access to an interrupt status register from the library's
**    private header file.
**
**    Revision 0.11  2001/04/02 17:20:31  jdm
**    SDB_SUBMIT_FAILURE error code added.
**
**    Revision 0.10  2001/02/20 14:12:55  jdm
**    SIMULATION_ACTIVE OID removed.
**
**    Revision 0.9  2001/01/31 10:01:42  rdl
**    Addition of #define for Server Priority.
**
**    Revision 0.8  2000/12/21 08:12:24  jdm
**    Version number OID added.
**
**    Revision 0.7  2000/11/23 10:34:33  jdm
**    Simulate mode added.
**
**    Revision 0.6  2000/10/18 07:20:18  jdm
**    Name server registration name changed to be non node specific.
**
**    Revision 0.5  2000/10/11 14:51:58  jdm
**    Updated for use with new server type.
**
**
*/

#ifndef _TFP_HEADER_INCLUDED
#define _TFP_HEADER_INCLUDED

/*
** =============================================================================
** Start of include files:
** =============================================================================
*/

#include "TtlSystem.h"      /* Include the package definitions. */
#include "Wfl.h"
#include "Acn.h"            /* Include the ACN definitions. */

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
    E_TFP_CARD_NOT_INITIALISED = STATUS_START(TFP),
    E_TFP_MESSAGE_NOT_RECOGNISED,
    E_TFP_FAILED_TO_CONFIGURE_TIMER,
    E_TFP_OID_TABLE_INIT_FAILURE,
    E_TFP_UNABLE_TO_INITIALISE_TIMER_CARD,
    E_TFP_FAILED_TO_CONFIGURE_ISR,
    E_TFP_FAILED_TO_CONFIGURE_INTERRUPT_TIMEOUT,
    E_TFP_FAILED_TO_EXTEND_INTERRUPT_TIMEOUT,
    E_TFP_FAILED_TO_GET_CURRENT_TIME,
    E_TFP_FAILED_TO_CONFIGURE_SIM_TIMER,
    E_TFP_SDB_SUBMIT_FAILURE,

    /*
    ** Start of errors previously in library public header.
    */

    E_TFP_SHM_OPEN_FAILURE,
    E_TFP_QNX_OSINFO_ERROR,
    E_TFP_NO_PCI_BIOS,
    E_TFP_CARD_NOT_PRESENT,
    E_TFP_MMAP_FAILURE,
    E_TFP_MUNMAP_FAILURE,
    E_TFP_PCI_READ_CONFIG_BYTE_FAILURE,
    E_TFP_PCI_READ_CONFIG_DWORD_FAILURE,
    E_TFP_PCI_NOT_INITIALISED,
    E_TFP_INVALID_BUFFER_ADDRESS,
    E_TFP_COMMAND_OVERRUN,
    E_TFP_NO_RESPONSE,
    E_TFP_INVALID_DPRAM_ADDRESS,
    E_TFP_WRONG_STATE,
    E_TFP_QNX_HINT_ATTACH_FAILED,
    
    #ifdef _TESTBED_SUPPORT
        E_TFP_INVALID_TESTBED_COMMAND,
    #endif

    E_TFP_LIB_SOFTWARE_ERROR,

    /*
    ** End of errors previously in library public header.
    */

    E_TFP_TRIGGER_PROXY_ERROR,
    E_TFP_TIMER_PROXY_ERROR,
    E_TFP_DUPLICATE_TRIGGER,
 
    E_TFP_LOCK_FAILURE,
    E_TFP_ZERO_FREQUENCY,
    
    E_TFP_SOFTWARE_ERROR = STATUS_START(TFP) + 0xFFFF
}eTfpClientStatus_t;

/* Enumerate the OID's owned by this package. */
typedef enum eTfpDataId_e{
    D_TFP_DATAID_BOL = OID_START(TFP),
    D_TFP_PROC_STATE,                  /* The tfp process state. */
    D_TFP_APP_VERSION,                 /* TFP package revision. */
    D_TFP_SERVER_SHUTDOWN,             /* Server shutdown command flag. */

    D_TFP_CURRENT_TIME,                /* The current time on the IRIG-B card */
    D_TFP_CURRENT_TIME_DUMMY,          /* Dummy for current-time upper word */
    D_TFP_TIME_STATUS,                 /* IRIG-B status word */
    D_TFP_TRIGGER_ATTACH,              /* To attach proxies to interrupt */
    D_TFP_TRIGGER_DETACH,              /* To detach proxies from interrupt */
    D_TFP_SYSTEM_TIME,                 /* System time from real-time clock */
    D_TFP_SYSTEM_TIME_DUMMY,           /* Dummy for system-time upper word */
    D_TFP_TIMER_PROXY_ATTACH,          /* To attach timer proxy to interrupt */
    D_TFP_TIMER_PROXY_DETACH,          /* To detach time proxy from interrupt */
    D_TFP_PERIODIC_FREQUENCY,          /* Frequency of the periodic output */
    D_TFP_SIMULATE_FLAG,               /* TRUE if running in simulate mode */
    D_TFP_TALLY_TRIGGER,               /* Number of clients for trigger proxy */
    D_TFP_TALLY_TIMER,                 /* Number of clients for timer proxy */
    D_TFP_MAX_FLYWHEELCOUNT,           /* Max no of attempts to lock */

    D_TFP_DATAID_EOL                   
}eTfpDataId_t;

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
#define E_TFP_REG_SERVER_NAME   "E_TTL_TFP"
#define E_TFP_SERVER_PROC       "Tfp"
#define E_TFP_SERVER_PATH       ""

#ifdef  E_WFL_OS_QNX6
#define E_TFP_SERVER_PRI                 60
#endif
#ifdef  E_WFL_OS_QNX4
#define E_TFP_SERVER_PRI                 30
#endif

/*******************************************************************************
**
** Start of contents of previous public header file for library.
**
*******************************************************************************/

typedef eTtlTime_t eTfpPosixTime_t;

#ifdef _TFP_MAIN

    #define TFP_EXTERN
    #define TFP_INIT(Initialisation) = Initialisation
#else
    
    #define TFP_EXTERN extern
    #define TFP_INIT(Initialisation)

#endif 

/*
** Driver States
** =============
*/

typedef enum{
    E_TFP_CLOSED  = 0,
    E_TFP_OPEN,
    E_TFP_CONNECTED,
} eTfpState_t;


/*
**  Configuration Definitions
**  =========================
*/

/* Timing Mode Settings (Default = E_TFP_TIME_CODE_MODE) */

typedef enum{
    E_TFP_TIME_CODE_MODE    = 0x00,
    E_TFP_FREE_RUNNING_MODE = 0x01,
    E_TFP_1PPS_MODE         = 0x02,
    E_TFP_RTC_MODE          = 0x03,
    E_TFP_GPS_MODE          = 0x06
} eTfpTimingMode_t;

/* Input Time Code Format (Default = E_TFP_IRIG_B) */

typedef enum{
    E_TFP_IRIG_A            = 0x41,
    E_TFP_IRIG_B            = 0x42,
    E_TFP_IEEE_1344         = 0x49
} eTfpTimeCodeFormat_t;

/* Input Time Code Modulation Type */

typedef enum{
    E_TFP_AMPLITUDE_MODULATED_SINE_WAVE = 0x4D,
    E_TFP_PULSE_CODE_MODULATION_DCLS    = 0x44
} eTfpTimeCodeModulation_t;

/*
**  Control Regigister Definitions
**  ==============================
*/


#define E_TFP_CONTROL_LOCKEN_MASK       0x01
#define E_TFP_CONTROL_EVSOURCE_MASK     0x02
#define E_TFP_CONTROL_EVSENSE_MASK      0x04
#define E_TFP_CONTROL_EVENTEN_MASK      0x08
#define E_TFP_CONTROL_STREN_MASK        0x10
#define E_TFP_CONTROL_STRMODE_MASK      0x20
#define E_TFP_CONTROL_FREQSEL_MASK      0xC0

#define E_TFP_CONTROL_ALL_BITS_MASK     0xFF

/*
** Set Values
*/

#define E_TFP_ENABLE_LOCKOUT            E_TFP_CONTROL_LOCKEN_MASK
#define E_TFP_PERIODIC_CAPTURE          E_TFP_CONTROL_EVSOURCE_MASK
#define E_TFP_RISING_EDGE_ACTIVE        E_TFP_CONTROL_EVSENSE_MASK
#define E_TFP_ENABLE_EVENT_CAPTURE      E_TFP_CONTROL_EVENTEN_MASK
#define E_TFP_ENABLE_STROBE_OUTPUT      E_TFP_CONTROL_STREN_MASK
#define E_TFP_MINOR_TIME_ONLY_STROBE    E_TFP_CONTROL_STRMODE_MASK

/*
** Clear Values (for convenience)
*/

#define E_TFP_DISABLE_LOCKOUT           0
#define E_TFP_EVENT_INPUT_CAPTURE       0
#define E_TFP_FALLING_EDGE_ACTIVE       0
#define E_TFP_DISABLE_EVENT_CAPTURE     0
#define E_TFP_DISABLE_STROBE_OUTPUT     0
#define E_TFP_MAJOR_AND_MINOR_STROBE    0

/*
**  Output Frequency Select Values
*/

#define E_TFP_FREQ_10_MHZ               0x00
#define E_TFP_FREQ_5_MHZ                0x40
#define E_TFP_FREQ_1_MHZ                0x80        /* Note: 0xC0 is also valid! */
#define E_TFP_FREQ_1_MHZ_ALTERNATIVE    0xC0        /* Note: 0x80 is also valid! */

/*
**  Interrupt Register Definitions  
**  ==============================
*/

typedef enum{
    E_TFP_EVENT_INPUT_INTERRUPT = 0,
    E_TFP_PERIODIC_OUTPUT_INTERRUPT,
    E_TFP_TIME_COINCIDENCE_STROBE_INTERRUPT,
    E_TFP_ONE_SECOND_EPOCH_INTERRUPT,
    E_TFP_GPS_DATA_PACKET_INTERRUPT
} eTfpInterruptNumber_t;

#define E_TFP_INTERRUPT_ALL_BITS_MASK   0x1F

#define E_TFP_INTERRUPT_MASK(InterruptNum)  (1 << (InterruptNum))

/*
**  Periodic Output Definitions
**  ===========================
*/

/*
** The following two definitions are replaced by ones below, but are retained
** as they are required by hardware test tools.
*/

#define E_TFP_400_HZ_PERIOD             250             

#define E_TFP_250_MICROSEC_PROCESSING   10

/*
** The following two definitions define the Programmable Periodic Output. Note
** the following :-
**    n1         = 1,000,000 / (Frequency * n2)
**    Frequency  = 1,000,000 / (n1 * n2) Hz
**    Duty Cycle = (1 - (1 / n2)) * 100 %
*/

#define E_TFP_DFLT_DIVIDER_N2           10 

#define E_TFP_DFLT_FREQUENCY            E_ACN_TICKS_PER_SECOND

typedef enum{
    E_TFP_DO_NOT_SYNC_TO_1PPS_EPOCH = 0,
    E_TFP_SYNC_TO_1PPS_EPOCH = 1
} eTfpPeriodicSync_t;

/*
**  Time Status Mask Values
**  =======================
*/

#define E_TFP_LOCKED                    0x00
#define E_TFP_FLYWHEELING               0x01
#define E_TFP_TIME_OFFSET               0x02
#define E_TFP_FREQ_OFFSET               0x04
#define E_TFP_INITIALISING              0x08


typedef enum{
   E_TFP_BOOT_STATUS_UNKNOWN,
   E_TFP_BOOT_STATUS_SPECIAL,
   E_TFP_BOOT_STATUS_NORMAL,
} eTfpBootStatus_t;


/*
**  Testbed Support Definitons
**  ==========================
*/


#ifdef _TESTBED_SUPPORT

    #define E_TFP_MAX_DPRAM_RESPONSE    12       /* Maximum number of DPRAM Response Bytes */
    
    /*
    ** Sub-Status Flag Values
    */
    
    #define E_TFP_NO_STATUS_AVAILABLE           0x00
    #define E_TFP_DPRAM_RESPONSE_AVAILABLE      0x01
    #define E_TFP_GENERAL_STATUS_AVAILABLE      0x02

    typedef enum{
        E_TFP_PCI_OPEN,
        E_TFP_PCI_CLOSE,
        E_TFP_SET_CONTROL_REGISTER,
        E_TFP_SET_MASK_REGISTER,
        E_TFP_SET_BOOT_MODE,
        E_TFP_GET_BOOT_MODE,
        E_TFP_RESET_CARD,
        E_TFP_GET_TFP_STATUS,
        E_TFP_SET_PERIODIC_OUTPUT,
        E_TFP_GET_PERIODIC_SETTING
    }eTfpTestbedTFPCommandType_t;  
    
    typedef char eTfpTestbedResponseBytes_t[E_TFP_MAX_DPRAM_RESPONSE];

    typedef struct{

        Uint32_t        ControlRegister;
        Uint32_t        MaskRegister;
        Uint32_t        IntStatusRegister;
        eTtlTime_t      TimeRegister;
        eTtlTime_t      EventRegister;
        Uint8_t         TimeStatus;
        Uint8_t         EventStatus;

    } eTfpTestbedGeneralStatus_t;
    
    typedef struct{
        Bool_t          SyncTo1ppsEpoch;
        Uint16_t        N1Value;
        Uint16_t        N2Value;
    
    } eTfpTestbedPeriodicParams_t; 
   
    typedef struct{

        eTfpTestbedTFPCommandType_t     CommandType;
        union {
            Uint16_t                    BootConfig;
            Uint32_t                    ControlRegister;
            eTfpTestbedPeriodicParams_t PeriodicParameters;
        } Parameters;

    } eTfpTestbedTFPCommand_t; 
    
    typedef struct{

        eTfpTestbedTFPCommandType_t     CommandType;
        char                            SubStatus;
        struct {
            eTfpTestbedResponseBytes_t  DPRamResponse;
            eTfpTestbedGeneralStatus_t  GenStatus;
            eTfpState_t                 State;
        } ResponseValues;

    } eTfpTestbedTFPResponse_t; 

Status_t eTfpProcessTestbedCommand (eTfpTestbedTFPCommand_t TestCommand,
                                    eTfpTestbedTFPResponse_t *TestResponse);
#endif


/*
**  Fixed Parameter Definitions
*/

#define I_TFP_SPECIAL_BOOT_MODE     0x0100
#define I_TFP_NORMAL_BOOT_MODE      0x0000


TFP_EXTERN const char eTfpDriverIdentification[] TFP_INIT({"TTL TFP QNX Driver Version 0.3"});

#ifdef _TFP_DEBUG_ENABLED

    #define E_TFP_DEBUG_BUFFER_SIZE          128
    #define E_TFP_DEBUG_MAX_CHARS            E_TFP_DEBUG_BUFFER_SIZE - 1
    
    typedef char eTfpDebugBuffer_t[E_TFP_DEBUG_BUFFER_SIZE];
    
    #ifdef _PHOTON_DEBUG_ENABLED

        TFP_EXTERN pid_t eTfpDebugServerPid TFP_INIT(0); /* Debug Server Process ID */

    #endif

    TFP_EXTERN Bool_t eTfpDebugLoggingEnabled TFP_INIT(TRUE);

#endif

Status_t    eTfpPCIClose( void );
Status_t    eTfpPCIInit( void );

void        eTfpGetTFPStatus (eTfpState_t *TfpState, Uint32_t *ControlRegister, Uint32_t *MaskRegister, Uint32_t *IntStatusRegister);
Status_t    eTfpSetPeriodicOutput (eTfpPeriodicSync_t SyncFlag, Uint16_t DividerN1, Uint16_t DividerN2);
Status_t    eTfpConnectInterruptHandler(eWflProxPulsId_t (*Handler)(void) );
Status_t    eTfpDisconnectInterruptHandler(void);
Status_t    eTfpSetControlRegister(Uint32_t ControlRegister);
Status_t    eTfpSetMaskRegister(Uint32_t MaskRegister);
Status_t    eTfpSetIntStatusRegister(Uint32_t IntStatusRegister);
Status_t    eTfpGetIntStatusRegister(Uint32_t * IntStatusRegister);
Status_t    eTfpGetEventTime (eTtlTime_t *EventTime, Uint8_t *Status);
Status_t    eTfpGetCurrentTime (eTtlTime_t *CurrentTime, Uint8_t *Status);
Status_t    eTfpResetCard (eTfpTimingMode_t TimingMode, eTfpTimeCodeFormat_t TimeCodeFormat, eTfpTimeCodeModulation_t TimeCodeModulation);
Status_t    eTfpGetBootStatus (eTfpBootStatus_t *BootStatus);
Status_t    eTfpSetSpecialBootStatus (void);


/*******************************************************************************
**
** End of contents of previous public header file for library.
**
*******************************************************************************/


#endif  /* _TFP_HEADER_INCLUDED */                                                    

/*
** =============================================================================
** End of header:   Tfp.h
** =============================================================================
*/
