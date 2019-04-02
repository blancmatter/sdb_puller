//===========================================================================
//
// NAME:    powerdaq-internal.h
//
// DESCRIPTION:
//
//          PowerDAQ QNX driver definitions needed for kernel driver only
//
// AUTHOR:  Alex Ivchenko
//
// DATE:    12-APR-2000
//
// REV:     0.8
//
// R DATE:  
//
// HISTORY:
//
//      Rev 0.8,     12-MAR-2000,     Initial version.
//
//
//---------------------------------------------------------------------------
//      Copyright (C) 2000 United Electronic Industries, Inc.
//      All rights reserved.
//---------------------------------------------------------------------------
//
#ifndef __POWER_DAQ_INTERNAL_H__
#define __POWER_DAQ_INTERNAL_H__

#include "powerdaq.h"
#include "pdfw_def.h"
#include "win2qnx.h"

// The max number of boards the driver can deal with.
#define PD_MAX_BOARDS 8

// Some ID strings for the driver.
#define PD_VERSION "PowerDAQ QNX driver, release 0.1 (beta 0)\n"

#define PD_ID  "powerdaq: "
#define KERN_ERR
#define _NO_USERSPACE

// General debug messages
//#define PD_DEBUG

// Subsystem States (State):
typedef enum {
    ssConfig            = 0x0,      // configuration state (default)
    ssStandby           = 0x1,      // on standby ready to start
    ssRunning           = 0x2,      // running
    ssPaused            = 0x4,      // paused
    ssStopped           = 0x8,      // stopped
    ssDone              = 0x10,     // operation done, stopped
    ssError             = 0x20,     // error condition, stopped
    ssActive            = 0x3       // subsystem either running or standby
} PDSubsystemState;


// DaqBuf Get DaqBuf Status Info Struct
typedef struct
{
    u32   SubsysState;      // current subsystem state
    u32   BufSizeInBytes;
    u32   ScanValues;       // maximum number of scans in buffer
    u32   MaxValues;        // maximum number of samples in buffer
    u32   FrameValues;      // maximum number of frames in buffer
    u32   SampleSize;       // size of the sample
    u32   DataWidth;        // size of single value in bytes
    u32   ScanIndex;        // buffer index of first scan
    u32   NumValidValues;   // number of valid values available
    u32   NumValidScans;    // number of valid scans available
    u32   NumValidFrames;   // number of valid frames available
    u32   bFixedDMA;        // Fixed DMA mode
    u32   bDWValues;        // DWORD values shall be used instead of WORD
    u32   WrapCount;        // total num times buffer wrapped
    u32   ScanSize;         // scan size in samples (2 bytes each)
    u32   FrameSize;        // frame size in scans
    u32   NumFrames;        // number of scans
    u16*  databuf;          // pointer to the buffer contains samples
    u16*  userdatabuf;      // pointer to the user buffer contains samples
    int     databuf_shm_fd;     // file descriptor of the user data buffer shared  memory object
    int     user_shm_fd;       // store the user shared memory fd so that we can send it back
                                        // when buffer is unregistered
    u32   Head;             // head of the buffer
    u32   Tail;             // tail of the buffer
    u32   Count;            // current position in the buffer
    u32   ValueCount;       // number of samples
    u32   ValueIndex;       // sample index
    u32   BufMode;          // mode of buffer usage
    u32   bWrap;            // buffer is in the "WRAPPED" mode
    u32   bRecycle;         // buffer is in the "RECYCLED" mode
    u32   FirstTimestamp;   // first sample timestamp
    u32   LastTimestamp;    // last sample timestamp
} TBuf_Info, * PTBuf_Info;

// set up run parameters (set parameter to -1 to keep default):
typedef struct _XFERPARAM 
{
    int XFerMode;       // transfer mode
    int InXFerCyc;      // number of input blocks to xfer (in 512 samples chunks)
    int OutXFerCyc;     // number of output block to xfer (in 1024 samples chunks)
    int DoGetSamples;   // Do GetSamples after XFer when flush FIFO
    int BmFHFXFers;     // number of BM transfers from FIFO (in 512 samples chunks)
    int BmPageXFers;    // number of BM transfers to the page (in 512 samples chunks)
} XFERPARAM, *pXFERPARAM;

// common data structure used to synchronize access
// to each sub-system
typedef struct _synchSS
{
#ifdef __QNXNTO__
   pthread_cond_t   event;
   pthread_mutex_t  event_lock;
#endif
} TSynchSS, *PTSynchSS;

typedef struct _AllocContigMem
{
   u32 idx;
   u32 size;		// # of 4K pages (to be used later, currently defaults to 4)
   u32 linearAddr;
   u32 physAddr;
   u32 status;
} AllocContigMem;


// this structure holds information about AIn subsystem
typedef struct
{
    u32   SubsysState;            // current subsystem state
    u32   SubsysConfig;           // keeps latest AIn configuration
    u32   bAsyncMode;
    u32   dwAInCfg;               // AIn configuration word
    u32   dwAInPreTrigCount;      // pre-trigger scan count
    u32   dwAInPostTrigCount;     // post-trigger scan count
    u32   dwCvRate;               // conversion start clock divider
    u32   dwClRate;               // channel list start clock divider
    u32   dwEventsNotify;         // subsystem user events notification
    u32   dwEventsStatus;         // subsystem user events status
    u32   dwEventsNew;            // new events
    u32   dwChListChan;           // number of channels in list
    u32   ChList[PD_MAX_CL_SIZE]; // channel list data buffer
    TBuf_Info BufInfo;            // buffer information
    TScan_Info ScanInfo;          // scan information
    u32   FifoValues;             // ???
    u32   bInUse;                 // TRUE -> SS is in use
    int   iPID;                   // PID of the process owned subsystem
    u32   dwWakeupEvents;         // events to wake up blocked request
    u32   dwNotifyEvents;         // events to notify on wakeup
    u32   timeout;
    u32   bCheckHalfDone;     // check FHF/HalfDone event
    u32   bCheckFifoError;    // check Overrun/Underrun Error event
    u32   bImmUpdate;         // Immediate update enable
    
    u32   XferBufValues;          // number of values in Xfer buffer
    int   XferBuf_fd;             // shared memory file descriptor
    u32*  pXferBuf;               // DMA ready driver buffer
    u32   XferBufValueCount;      // number of samples written to Xfer buffer
    u32   BlkXferValues;          // number of values to transfer using DSP DMA
    u32   FifoXFerCycles;         // blocks of 512 samples
    u32   DoGetSamples;           // perform get samples after XFer

    u32   BmFHFXFers;             // size of 1/2 FIFO in 512 samples chunks
    u32   BmPageXFers;            // size of cont physical page in 512 samples chunks
    
    struct _synchSS *synch;
} TAinSS, *PTAinSS;

// this structure holds information about AOut subsystem
typedef struct
{
    u32   SubsysState;
    u32   SubsysConfig;           // keeps latest AOut configuration
    u32   dwAOutCfg;              // AOut configuration word
    u32   dwMode;
    u32   dwAOutPreTrigCount;      // pre-trigger scan count
    u32   dwAOutPostTrigCount;     // post-trigger scan count
    u32   dwCvRate;               // conversion start clock divider
    u32   dwClRate;               // channel list start clock divider
    u32   dwAoutValue;
    u32   dwEventsNotify;
    u32   dwEventsStatus;
    u32   dwEventsNew;
    u32   dwChListChan;           // number of channels in list
    u32   ChList[PD_MAX_CL_SIZE]; // channel list data buffer
    TBuf_Info BufInfo;
    TScan_Info ScanInfo;
    u32   FifoValues;             // ???
    u32   bInUse;                 // TRUE -> SS is in use
    int   iPID;                   // PID of the process owned subsystem
    u32   bAsyncMode;
    u32   bCheckHalfDone;     // check FHF/HalfDone event
    u32   bCheckFifoError;    // check Overrun/Underrun Error event
    u32   bImmUpdate;         // Immediate update enable
    u32   dwWakeupEvents;         // events to wake up blocked request
    u32   dwNotifyEvents;         // events to notify on wakeup
    u32   timeout;
    
    u32   XferBufValues;          // number of values in Xfer buffer
    int   XferBuf_fd;             // shared memory file descriptor
    u32*  pXferBuf;               // DMA ready driver buffer
    u32   XferBufValueCount;      // number of samples written to Xfer buffer
    u32   BlkXferValues;          // number of values to transfer using DSP DMA
    u32   FifoXFerCycles;         // blocks of 512 samples
    u32   TranSize;

    u32   BmFHFXFers;             // size of 1/2 FIFO in 512 samples chunks
    u32   BmPageXFers;            // size of cont physical page in 512 samples chunks

    u32   bRev3Mode;              // Select between old way/new way of doing data transfer
    
    struct _synchSS *synch;
} TAoutSS, *PTAoutSS;

// this structure holds information about UCT subsystem
typedef struct
{
    u32   SubsysState;
    u32   bAsyncMode;
    u32   dwMode;
    u32   dwCvRate;
    u32   dwEventsNotify;
    u32   dwEventsStatus;
    u32   dwEventsNew;
    u32   bInUse;                 // TRUE -> SS is in use
    int   iPID;                   // PID of the process owned subsystem
    u32   dwWakeupEvents;         // events to wake up blocked request
    u32   dwNotifyEvents;         // events to notify on wakeup
    u32   timeout;
    struct _synchSS *synch;
} TUctSS, * PTUctSS;

// this structure holds information about DIO subsystem
typedef struct
{
    u32   SubsysState;
    u32   bAsyncMode;
    u32   dwMode;
    u32   dwCvRate;
    u32   dwEventsNotify;
    u32   dwEventsStatus;
    u32   dwEventsNew;
    u32   bInUse;                 // TRUE -> SS is in use
    int   iPID;                   // PID of the process owned subsystem
    u32   dwWakeupEvents;         // events to wake up blocked request
    u32   dwNotifyEvents;         // events to notify on wakeup
    u32   timeout;
    u32   intrData[16];
    u32   intrMask[8];
    struct _synchSS *synch;
} TDioSS, * PTDioSS;

// Prototype of a routine that a user can install to be notified
// when the board receives an interrupt.
// This is only used with RTLinux.
typedef void (* TUser_isr)(void *user_param);
//
// this structure holds everything the driver knows about a board
//
typedef struct
{
   struct pci_dev *dev;
   void *address;
   u32 size;

   int index;
   int irq;
   int open;
   int bTestInt;     // TRUE for interrupt test

   u32 HI32CtrlCfg;
   u16 caps_idx;    // board type index in pd_hcaps.h
   u32 dwXFerMode;

   iofunc_notify_t *nop;  // for asynchronous notification 

   // Subsystem-related variables
   PD_EEPROM Eeprom;
   PD_PCI_CONFIG PCI_Config;
   int bImmUpdate;
   int bUseHeavyIsr;
   int fd[PD_MAX_SUBSYSTEMS];

   // Event-related variables
   TEvents FwEventsConfig;   // current adapter's events config
   TEvents FwEventsNotify;   // firmware events to notify
   TEvents FwEventsStatus;   // firmware event status saved

   u32 intMutex;
   
#ifdef __QNXNTO__
    int	                isrid;
    struct sigevent      event;
    pthread_t     isrThread;
    void*		    pciDeviceHandle;
#endif

   TAinSS  AinSS;
   TAoutSS AoutSS;
   TUctSS  UctSS;
   TDioSS  DinSS;
   TDioSS  DoutSS;

   void*  pPhysBMB[4];         // physical addresses of BM buffer
   void*  pSysBMB[4];          // virtual addresses of BM buffer
   void*  pLinBMB[4];          // linear, i.e. user space address
   u32    SizeBMB[4];          // size of BM buffer
   u32    cp;                  // current AIn page
   void *extension;
} pd_board_t;

// This extension is specific to RTLinux
// it contains the user ISR data
typedef struct _board_extension
{
   TUser_isr user_isr;
   void *user_param;
#ifdef __QNXNTO__
   pthread_mutex_t user_isr_lock;
#endif
} pd_board_ext_t;


//  The max number of times to poll the board before we declare it dead.
#define MAX_PCI_BUSY_WAIT 0xFFFFFF

//  The max number of milliseconds to wait for the board to act before we
//  declare it dead.  This is not currently used.
#define MAX_PCI_WAIT 1000

// AIn/AOut temporary storage data transfer buffer definition.
//
// Size set to be sufficient to store AIn/AOut FIFO samples.
//      (AIn/AOut FIFO samples * 2 bytes/sample)
//
#define PD_AIN_MAX_FIFO_VALUES  0x2000   //AI00310: max xfer buffer size
#define PD_AIN_FIFO_VALUES      0x400    //AI00310: minimal safe transfer
#define PD_AIN_MAX_XFERSIZE     0x1000   //AI00310: maximum xfer size
#define PD_BM_PAGES             4        // number of contigous pages
#define PD_BM_SPP               0x400    // samples per page

#define ANALOG_XFERBUF_VALUES   0x10000
#define ANALOG_XFERBUF_SIZE     (ANALOG_XFERBUF_VALUES * sizeof(ULONG))
#define PD_AOUT_MAX_FIFO_VALUES 0x800   // Maximum number of samples in DAC FIFO

#define XFERMODE_NORMAL     0       // use read/write with status register check
#define XFERMODE_FAST       1       // use read/write without status register check
#define XFERMODE_BM         2       // use bus master for downstream transfers
#define XFERMODE_NOAIMEM    0xe     // error: no AI contigous memory
#define XFERMODE_NOAOMEM    0xf     // error: no AO contigous memory
#define XFERMODE_BRDDIS     0xff    // error: board disabled

// bus mastering pages
#define AI_PAGE0    0
#define AI_PAGE1    1
#define AO_PAGE0    2
#define AO_PAGE1    3

#endif


