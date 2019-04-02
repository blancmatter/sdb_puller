//===========================================================================
//
// NAME:    powerdaq.h
//
// DESCRIPTION:
//
//          PowerDAQ QNX driver definitions needed to include into the library
//          and applications
//
// AUTHOR:  Alex Ivchenko
//
// DATE:    12-APR-2000
//
// REV:     
//
// R DATE:  3.1
//
// HISTORY:
//
//      Rev 0.8,     12-MAR-2000,     Initial version.
//      Rev 3.1,     23-NOV-2001,     FV, Converted driver to resource manager
//      Rec 3.2,     01_APR-2004,     ASP - Modified for QNX6
//
//
//---------------------------------------------------------------------------
//      Copyright (C) 2002 United Electronic Industries, Inc.
//      All rights reserved.
//---------------------------------------------------------------------------
//
#ifndef __POWER_DAQ_H__
#define __POWER_DAQ_H__


#include "pdfw_def.h"
#include "win2qnx.h"

// AIn modes definition
#define AIN_SINGLE_ENDED   0
#define AIN_DIFFERENTIAL   AIB_INPMODE

#define AIN_UNIPOLAR       0
#define AIN_BIPOLAR        AIB_INPTYPE

#define AIN_RANGE_5V       0
#define AIN_RANGE_10V      AIB_INPRANGE

#define AIN_CV_CLOCK_SW          0
#define AIN_CV_CLOCK_INTERNAL    AIB_CVSTART0
#define AIN_CV_CLOCK_EXTERNAL    AIB_CVSTART1
#define AIN_CV_CLOCK_CONTINUOUS  (AIB_CVSTART0 | AIB_CVSTART1)

#define AIN_CV_EXT_CLOCK_RISE    0
#define AIN_CV_EXT_CLOCK_FALL    AIB_EXTCVS

#define AIN_CL_CLOCK_SW          0
#define AIN_CL_CLOCK_INTERNAL    AIB_CLSTART0
#define AIN_CL_CLOCK_EXTERNAL    AIB_CLSTART1
#define AIN_CL_CLOCK_CONTINUOUS  (AIB_CLSTART0 | AIB_CLSTART1)

#define AIN_CL_EXT_CLOCK_RISE    0
#define AIN_CL_EXT_CLOCK_FALL    AIB_EXTCLS

#define AIN_CV_INT_CLOCK_BASE_11MHZ  0
#define AIN_CV_INT_CLOCK_BASE_33MHZ  AIB_INTCVSBASE

#define AIN_CL_INT_CLOCK_BASE_11MHZ  0
#define AIN_CL_INT_CLOCK_BASE_33MHZ  AIB_INTCLSBASE

#define AIN_START_TRIGGER_NONE       0
#define AIN_START_TRIGGER_RISE       AIB_STARTTRIG0
#define AIN_START_TRIGGER_FALL       (AIB_STARTTRIG0 | AIB_STARTTRIG1)

#define AIN_STOP_TRIGGER_NONE        0
#define AIN_STOP_TRIGGER_RISE        AIB_STOPTRIG1
#define AIN_STOP_TRIGGER_FALL        (AIB_STOPTRIG0 | AIB_STOPTRIG1)

#define AIN_SCANRETMODE_MMAP      0
#define AIN_SCANRETMODE_RAW       1
#define AIN_SCANRETMODE_VOLTS     2

#define SCANRETMODE_MMAP      0
#define SCANRETMODE_RAW       1
#define SCANRETMODE_VOLTS     2

#define AIN_SHM_PREFIX "AinXfer"
#define AOUT_SHM_PREFIX "AoutXfer"
#define REG_BUF_SHM_PREFIX "PdRegBuf"

//---------------------------------------------------------------------------
//
// Macros for constructing Channel List entries.
//
#define CHAN(c)     ((c) & 0x3f)
#define GAIN(g)     (((g) & 0x3) << 6)
#define SLOW        (1<<8)
#define CHLIST_ENT(c,g,s)   (CHAN(c) | GAIN(g) | ((s) ? SLOW : 0))

//---------------------------------------------------------------------------
//
// Macro to convert A/D samples from 2's complement to straight binary.
//
//Alex::No longer needed. Use pd_hcaps.h for conversion
// ...
//#define AIN_BINARY(a)   (((a) & ANDWORD) ^ XORWORD)


//-----------------------------------------------------------------------
// Definitions for mapdsp.sys Device Driver.
//-----------------------------------------------------------------------
#ifndef TRUE
#define TRUE        1
#endif
#ifndef FALSE
#define FALSE       0
#endif

#define STATUS_SUCCESS      1
#define STATUS_UNSUCCESSFUL 0

#ifndef PCI_TYPE0_ADDRESSES
   #define PCI_TYPE0_ADDRESSES     6
#endif

#ifndef __QNXNTO__
   #pragma pack(1);
#endif

//---------------------------------------------------------------------------
//
// PCI Configuration Space:
//
typedef struct _PD_PCI_CONFIG
{
    unsigned short      VendorID;
    unsigned short      DeviceID;
    unsigned short      Command;
    unsigned short      Status;
    char                RevisionID;
	char				Class_Code[3];             
    char                CacheLineSize;
    char                LatencyTimer;
	char				Header_Type;               
	char				BIST;                      
    unsigned long       BaseAddress[6];
    unsigned long       CardBusCISPtr;
    unsigned short      SubsystemVendorID;
    unsigned short      SubsystemID;
	unsigned long		ROM_Base_Address;          
	unsigned long		Reserved2[2];              
	char				InterruptLine;            
	char				InterruptPin;             
	char				Min_Gnt;                   
	char				Max_Lat;                   
	char                Reserved3[16];
} PD_PCI_CONFIG, * PPD_PCI_CONFIG;


#define PD_MAX_BOARDS 8         // The max number of boards the driver can deal with.
#define PD_MAX_CL_SIZE 64       // Longest channel list
#define AINBUFSIZE 1024         // Buffer size
#define PD_MAX_SUBSYSTEMS 7 // max number of subsystems in PowerDAQ PCI cards
#define MAX_PCI_SPACES      1   // number of PCI Type 0 base address registers

//---------------------------------------------------------------------------
//
// Interface Data Structures:
//

#define PD_EEPROM_SIZE          256    // EEPROM size in 16-bit words
#define PD_SERIALNUMBER_SIZE    10      // serial number length in bytes
#define PD_DATE_SIZE            12      // date string length in bytes
#define PD_CAL_AREA_SIZE        32      // EEPROM calibration area size in 16-bit words
#define PD_SST_AREA_SIZE        32      // EEPROM start-up state area size in 16-bit words

typedef struct _PD_EEPROM
{
   union
   {
      struct _Header
      {
         UCHAR   ADCFifoSize;
         UCHAR   CLFifoSize;
         UCHAR   SerialNumber[PD_SERIALNUMBER_SIZE];
         UCHAR   ManufactureDate[PD_DATE_SIZE];
         UCHAR   CalibrationDate[PD_DATE_SIZE];
         ULONG   Revision;
         USHORT  FirstUseDate;
         USHORT  CalibrArea[PD_CAL_AREA_SIZE];
         USHORT  FWModeSelect;
         USHORT  StartupArea[PD_SST_AREA_SIZE];
         USHORT  PXI_Config[5];                  // added by S.S.
         UCHAR   DACFifoSize;
      } Header;

      USHORT WordValues[PD_EEPROM_SIZE];
   } u;
} PD_EEPROM, *PPD_EEPROM;

typedef enum
{
    BoardLevel  = 0,
    AnalogIn    = 1,
    AnalogOut,
    DigitalIn,
    DigitalOut,
    CounterTimer,
    CalDiag
} PD_SUBSYSTEM, * PPD_SUBSYSTEM;

#define EdgeDetect 0x8000

// Subsystem Events (Events):
//
typedef enum {
    eStartTrig          = (1L<<0),   // start trigger / operation started
    eStopTrig           = (1L<<1),   // stop trigger / operation stopped
    eInputTrig          = (1L<<2),   // subsystem specific input trigger
    eDataAvailable      = (1L<<3),   // new data / points available

    eScanDone           = (1L<<4),   // scan done
    eFrameDone          = (1L<<5),   // logical frame done
    eFrameRecycled      = (1L<<6),   // cyclic buffer frame recycled
    eBlockDone          = (1L<<7),   // logical block done (FUTURE)

    eBufferDone         = (1L<<8),   // buffer done
    eBufListDone        = (1L<<9),   // buffer list done (FUTURE)
    eBufferWrapped      = (1L<<10),  // cyclic buffer / list wrapped
    eConvError          = (1L<<11),  // conversion clock error

    eScanError          = (1L<<12),  // scan clock error
    eDataError          = (1L<<13),  // data error (out-of-range)
    eBufferError        = (1L<<14),  // buffer over/under run error
    eTrigError          = (1L<<15),  // trigger error

    eStopped            = (1L<<16),  // operation stopped
    eTimeout            = (1L<<17),  // operation timed-out
    eAllEvents          = (0xFFFFF)  // set/clear all events
} PDEvent;

typedef enum {
    eDInEvent           = (1L<<0),   // Digital Input event
    eUct0Event          = (1L<<1),   // Uct0 countdown event
    eUct1Event          = (1L<<2),   // Uct1 countdown event
    eUct2Event          = (1L<<3),   // Uct2 countdown event
} PDDigEvent;

#define     AIB_BUFFERWRAPPED   0x1
#define     AIB_BUFFERRECYCLED  0x2
#define     BUF_BUFFERWRAPPED   0x1  // now applied to AIn/AOut/DIn/DOut/CT buffers
#define     BUF_BUFFERRECYCLED  0x2
#define     BUF_DWORDVALUES     0x10
#define     BUF_FIXEDDMA        0x20


//---------------------------------------------------------------------------
//
// PowerDAQ IOCTL:
//

#define PD_IOCTL_BASE 0x100
//#define __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +request, method) request+0x100
//__DIOTF(_DCMD_MISC, MY_CMD_CODE + 2, union _my_devctl_msg)


//---------------------------------------------------------------------------

//#define IOCTL_PWRDAQ_PRIVATE_MAP_DEVICE   __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +0, METHOD_BUFFERED)
//#define IOCTL_PWRDAQ_PRIVATE_UNMAP_DEVICE __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +1, METHOD_BUFFERED)
#define IOCTL_PWRDAQ_PRIVATE_GETCFG       __DIOF(_DCMD_MISC, PD_IOCTL_BASE +2, PD_PCI_CONFIG)
#define IOCTL_PWRDAQ_PRIVATE_SETCFG       __DIOT(_DCMD_MISC, PD_IOCTL_BASE +3, PD_PCI_CONFIG)
#define IOCTL_PWRDAQ_PRIVATE_SET_EVENT    __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +4, TCmd)
#define IOCTL_PWRDAQ_PRIVATE_CLR_EVENT    __DION(_DCMD_MISC, PD_IOCTL_BASE +5)

#define IOCTL_PWRDAQ_GET_NUMBER_ADAPTER __DIOF(_DCMD_MISC, PD_IOCTL_BASE +9, TCmd)
//#define IOCTL_PWRDAQ_ACQUIRESUBSYSTEM   __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +10, TCmd)
#define IOCTL_PWRDAQ_REGISTER_BUFFER    __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +11, TCmd)
#define IOCTL_PWRDAQ_UNREGISTER_BUFFER  __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +12, TCmd)
//#define IOCTL_PWRDAQ_REGISTER_EVENTS    __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +13, METHOD_BUFFERED)
//#define IOCTL_PWRDAQ_UNREGISTER_EVENTS  __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +14, METHOD_BUFFERED)
//#define IOCTL_PWRDAQ_GET_EVENTS         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +15, METHOD_BUFFERED)

#define IOCTL_PWRDAQ_GETADCFIFOSIZE     __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +16, u32)
#define IOCTL_PWRDAQ_GETSERIALNUMBER    __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +17, TCmd)
#define IOCTL_PWRDAQ_GETMANFCTRDATE     __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +18, TCmd)
#define IOCTL_PWRDAQ_GETCALIBRATIONDATE __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +19, TCmd)


#define IOCTL_PWRDAQ_SET_USER_EVENTS    __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +20, TCmd)
#define IOCTL_PWRDAQ_CLEAR_USER_EVENTS  __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +21, TCmd)
#define IOCTL_PWRDAQ_GET_USER_EVENTS    __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +22, TCmd)
#define IOCTL_PWRDAQ_IMMEDIATE_UPDATE   __DION(_DCMD_MISC, PD_IOCTL_BASE +23)
#define IOCTL_PWRDAQ_SET_TIMED_UPDATE   __DION(_DCMD_MISC, PD_IOCTL_BASE +24)

// PowerDAQ Asynchronous Buffered AIn/AOut Operations.
#define IOCTL_PWRDAQ_AIN_ASYNC_INIT     __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +30, TCmd)
#define IOCTL_PWRDAQ_AIN_ASYNC_TERM     __DION(_DCMD_MISC, PD_IOCTL_BASE +31)
#define IOCTL_PWRDAQ_AIN_ASYNC_START    __DION(_DCMD_MISC, PD_IOCTL_BASE +32)
#define IOCTL_PWRDAQ_AIN_ASYNC_STOP     __DION(_DCMD_MISC, PD_IOCTL_BASE +33)

#define IOCTL_PWRDAQ_AO_ASYNC_INIT     __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +34, TCmd)
#define IOCTL_PWRDAQ_AO_ASYNC_TERM     __DION(_DCMD_MISC, PD_IOCTL_BASE +35)
#define IOCTL_PWRDAQ_AO_ASYNC_START    __DION(_DCMD_MISC, PD_IOCTL_BASE +36)
#define IOCTL_PWRDAQ_AO_ASYNC_STOP     __DION(_DCMD_MISC, PD_IOCTL_BASE +37)

#define IOCTL_PWRDAQ_GET_DAQBUF_STATUS  __DION(_DCMD_MISC, PD_IOCTL_BASE +40)
#define IOCTL_PWRDAQ_GET_DAQBUF_SCANS   __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +41, TScan_Info)
#define IOCTL_PWRDAQ_CLEAR_DAQBUF       __DION(_DCMD_MISC, PD_IOCTL_BASE +42)

// Low Level PowerDAQ Board Level Commands.
#define IOCTL_PWRDAQ_BRDRESET           __DION(_DCMD_MISC, PD_IOCTL_BASE +100)
#define IOCTL_PWRDAQ_BRDEEPROMREAD      __DIOF(_DCMD_MISC, PD_IOCTL_BASE +101, TCmd)
#define IOCTL_PWRDAQ_BRDEEPROMWRITEDATE __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +102, TCmd)
#define IOCTL_PWRDAQ_BRDENABLEINTERRUPT __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +103, TCmd)
#define IOCTL_PWRDAQ_BRDTESTINTERRUPT   __DION(_DCMD_MISC, PD_IOCTL_BASE +104 )
#define IOCTL_PWRDAQ_BRDSTATUS          __DIOF(_DCMD_MISC, PD_IOCTL_BASE +105, TCmd)
#define IOCTL_PWRDAQ_BRDSETEVNTS1       __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +106, TCmd)
#define IOCTL_PWRDAQ_BRDSETEVNTS2       __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +107, TCmd)
#define IOCTL_PWRDAQ_BRDFWLOAD          __DION(_DCMD_MISC, PD_IOCTL_BASE +108)
#define IOCTL_PWRDAQ_BRDFWEXEC          __DION(_DCMD_MISC, PD_IOCTL_BASE +109)
#define IOCTL_PWRDAQ_BRDREGWR           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +110, TCmd)
#define IOCTL_PWRDAQ_BRDREGRD           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +111, TCmd)

// Low Level PowerDAQ AIn Subsystem Commands.
#define IOCTL_PWRDAQ_AISETCFG           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +200, TCmd)
#define IOCTL_PWRDAQ_AISETCVCLK         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +201, TCmd)
#define IOCTL_PWRDAQ_AISETCLCLK         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +202, TCmd)
#define IOCTL_PWRDAQ_AISETCHLIST        __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +203, TCmd)
#define IOCTL_PWRDAQ_AISETEVNT          __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +204, TCmd)
#define IOCTL_PWRDAQ_AISTATUS           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +205, TCmd)
#define IOCTL_PWRDAQ_AICVEN             __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +206, TCmd)
#define IOCTL_PWRDAQ_AISTARTTRIG        __DION(_DCMD_MISC, PD_IOCTL_BASE +207)
#define IOCTL_PWRDAQ_AISTOPTRIG         __DION(_DCMD_MISC, PD_IOCTL_BASE +208)
#define IOCTL_PWRDAQ_AISWCVSTART        __DION(_DCMD_MISC, PD_IOCTL_BASE +209)
#define IOCTL_PWRDAQ_AISWCLSTART        __DION(_DCMD_MISC, PD_IOCTL_BASE +210)
#define IOCTL_PWRDAQ_AICLRESET          __DION(_DCMD_MISC, PD_IOCTL_BASE +211)
#define IOCTL_PWRDAQ_AICLRDATA          __DION(_DCMD_MISC, PD_IOCTL_BASE +212)
#define IOCTL_PWRDAQ_AIRESET            __DION(_DCMD_MISC, PD_IOCTL_BASE +213)
#define IOCTL_PWRDAQ_AIGETVALUE         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +214, TCmd)
#define IOCTL_PWRDAQ_AIGETSAMPLES       __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +215, TCmd)
#define IOCTL_PWRDAQ_AISETSSHGAIN       __DION(_DCMD_MISC, PD_IOCTL_BASE +216)
#define IOCTL_PWRDAQ_AIGETXFERSAMPLES   __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +217, TCmd)
#define IOCTL_PWRDAQ_AISETXFERSIZE      __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +218, TCmd)
#define IOCTL_PWRDAQ_AIXFERSIZE         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +219, TCmd) 
#define IOCTL_PWRDAQ_AIRETRIEVE         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +220, TCmd) 

//?ALEX:
#define IOCTL_PWRDAQ_AIENABLECLCOUNT    __DION(_DCMD_MISC, PD_IOCTL_BASE +120)
#define IOCTL_PWRDAQ_AIENABLETIMER      __DION(_DCMD_MISC, PD_IOCTL_BASE +121)
#define IOCTL_PWRDAQ_AIFLUSHFIFO        __DION(_DCMD_MISC, PD_IOCTL_BASE +122)
#define IOCTL_PWRDAQ_AIGETSAMPLECOUNT   __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +123, TCmd)

// Low Level PowerDAQ AOut Subsystem Commands.
#define IOCTL_PWRDAQ_AOSETCFG           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +300, TCmd)
#define IOCTL_PWRDAQ_AOSETCVCLK         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +301, TCmd)
#define IOCTL_PWRDAQ_AOSETEVNT          __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +302, TCmd)
#define IOCTL_PWRDAQ_AOSTATUS           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +303, TCmd)
#define IOCTL_PWRDAQ_AOCVEN             __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +304, TCmd)
#define IOCTL_PWRDAQ_AOSTARTTRIG        __DION(_DCMD_MISC, PD_IOCTL_BASE +305)
#define IOCTL_PWRDAQ_AOSTOPTRIG         __DION(_DCMD_MISC, PD_IOCTL_BASE +306)
#define IOCTL_PWRDAQ_AOSWCVSTART        __DION(_DCMD_MISC, PD_IOCTL_BASE +307)
#define IOCTL_PWRDAQ_AOCLRDATA          __DION(_DCMD_MISC, PD_IOCTL_BASE +308)
#define IOCTL_PWRDAQ_AORESET            __DION(_DCMD_MISC, PD_IOCTL_BASE +309)
#define IOCTL_PWRDAQ_AOPUTVALUE         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +310, TCmd)
#define IOCTL_PWRDAQ_AOPUTBLOCK         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +311, TCmd)
#define IOCTL_PWRDAQ_AODMASET            __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +312, TCmd)
#define IOCTL_PWRDAQ_AOSETWAVE           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +313, TCmd)


// Low Level PowerDAQ DIn Subsystem Commands.
#define IOCTL_PWRDAQ_DISETCFG           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +400, TCmd)
#define IOCTL_PWRDAQ_DISTATUS           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +401, TCmd)
#define IOCTL_PWRDAQ_DIREAD             __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +402, TCmd)
#define IOCTL_PWRDAQ_DICLRDATA          __DION(_DCMD_MISC, PD_IOCTL_BASE +403)
#define IOCTL_PWRDAQ_DIRESET            __DION(_DCMD_MISC, PD_IOCTL_BASE +404)

// Low Level PowerDAQ DOut Subsystem Commands.
#define IOCTL_PWRDAQ_DOWRITE            __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +500, TCmd)
#define IOCTL_PWRDAQ_DORESET            __DION(_DCMD_MISC, PD_IOCTL_BASE +501)

// Low Level PowerDAQ DIO-256 Subsystem Commands.
#define IOCTL_PWRDAQ_DIO256CMDWR        __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +502, TCmd)
#define IOCTL_PWRDAQ_DIO256CMDRD        __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +503, TCmd)
#define IOCTL_PWRDAQ_DIO256SETINTRMASK   __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +504, TCmd)
#define IOCTL_PWRDAQ_DIO256GETINTRDATA   __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +505, TCmd)
#define IOCTL_PWRDAQ_DIO256INTRREENABLE  __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +506, TCmd)
#define IOCTL_PWRDAQ_DIODMASET           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +507, TCmd)


// Low Level PowerDAQ UCT Subsystem Commands.
#define IOCTL_PWRDAQ_UCTSETCFG          __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +600, TCmd)
#define IOCTL_PWRDAQ_UCTSTATUS          __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +601, TCmd)
#define IOCTL_PWRDAQ_UCTWRITE           __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +602, TCmd)
#define IOCTL_PWRDAQ_UCTREAD            __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +603, TCmd)
#define IOCTL_PWRDAQ_UCTSWGATE          __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +604, TCmd)
#define IOCTL_PWRDAQ_UCTSWCLK           __DION(_DCMD_MISC, PD_IOCTL_BASE +605)
#define IOCTL_PWRDAQ_UCTRESET           __DION(_DCMD_MISC, PD_IOCTL_BASE +606)

// Low Level PowerDAQ Cal Subsystem Commands.
#define IOCTL_PWRDAQ_CALSETCFG          __DION(_DCMD_MISC, PD_IOCTL_BASE +700)
#define IOCTL_PWRDAQ_CALDACWRITE        __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +701, TCmd)

// Low Level PowerDAQ Diag Subsystem Commands.
#define IOCTL_PWRDAQ_DIAGPCIECHO        __DION(_DCMD_MISC, PD_IOCTL_BASE +800)
#define IOCTL_PWRDAQ_DIAGPCIINT         __DION(_DCMD_MISC, PD_IOCTL_BASE +801)
#define IOCTL_PWRDAQ_BRDEEPROMWRITE     __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +802, TCmd)
#define IOCTL_PWRDAQ_DIAGSETPRM          __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +803, TCmd)

// QNX driver test only ioctl's
#define IOCTL_PWRDAQ_TESTEVENTS         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +901, TCmd)
#define IOCTL_PWRDAQ_TESTBUFFERING      __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +902, TCmd)
#define IOCTL_PWRDAQ_OPENSUBSYSTEM      __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +903, TCmd)
#define IOCTL_PWRDAQ_CLOSESUBSYSTEM     __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +904, TCmd)
#define IOCTL_PWRDAQ_RELEASEALL         __DIOTF(_DCMD_MISC, PD_IOCTL_BASE +905, TCmd)


//
// File contains structures to perform requests to the driver
// structures for PowerDAQ user-level library
//

// Command to access the board
// Make shure that additional "high-level" commands
// are not conflictiong with FW commands in pdfw_def.h
//
#define  AINSETSYNCCFG  0xF101
#define  AINREADSCAN    0xF102

// Analog Input synchronous operation structure
// return averaged channel values in dwChList
typedef struct
{
  uint32_t Subsystem;
  uint32_t dwMode;
  uint32_t dwChListSize;
  uint32_t dwChList[PD_MAX_CL_SIZE];

} TAinSyncCfg, *PTAinSyncCfg;

typedef struct
{
  uint32_t Subsystem;
  uint32_t dwAInCfg;
  uint32_t dwEventsNotify;
  uint32_t dwAInPreTrigCount;
  uint32_t dwAInPostTrigCount;
  uint32_t dwCvRate;
  uint32_t dwClRate;
  uint32_t dwChListSize;
  uint32_t dwChList[PD_MAX_CL_SIZE];

} TAinAsyncCfg, *PTAinAsyncCfg;

// Board hardware event words.
typedef struct
{
    uint32_t   Board;
    uint32_t   ADUIntr;
    uint32_t   AIOIntr;
    uint32_t   AInIntr;
    uint32_t   AOutIntr;

} TEvents, * PTEvents;

// DaqBuf Get Scan Info Struct
typedef struct
{
    uint32_t   Subsystem;
    uint32_t   NumScans;       // number of scans to get
    uint32_t   ScanIndex;      // buffer index of first scan
    uint32_t   NumValidScans;  // number of valid scans available
    uint32_t   ScanRetMode;
    
} TScan_Info, * PTScan_Info;


typedef struct
{
    unsigned long MaxSize;
    unsigned long WordsRead;
    uint16_t Buffer[PD_EEPROM_SIZE];

} TEepromAcc;

typedef struct
{
   int32_t   fileDescriptor;
   uint32_t   subSystem;
} TAcqSS;

typedef struct {
	PD_SUBSYSTEM sys;
    uint32_t *events;
} pd_get_user_events_t;


// Main command structure
// union contains ioctl-specific information needed to communicate
// with the driver
//
typedef union {
     TAinSyncCfg  AinSyncCfg;
     TEvents      Event;
     uint32_t     dwParam[16];
     TEepromAcc   EepromAcc;
     TAinAsyncCfg AinAsyncCfg;
     TAcqSS      AcqSS;
     char      stringParam[64];
} TCmd, *PTCmd;
#endif


