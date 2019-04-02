/********************************************************************************
** Module Name:
**    Agc.h
**
** Purpose:
**    Public header file for the Autoguider Camera Library
**
** Description:
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    Autoguider Control System Detailed Design Document v0.01.
**
** Author(s):
**    Stephen Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**                                                                           
** Version:
**    $Id: Agc.h,v 0.15 2006/12/07 11:50:05 sxf Exp $
**
** History:
**    $Log: Agc.h,v $
**    Revision 0.15  2006/12/07 11:50:05  sxf
**    Changed prototype of msiBeginTimedReadout.
**
**    Revision 0.14  2006/11/28 11:02:57  sxf
**    Added prototype for checksum function.
**
**    Revision 0.13  2006/06/26 13:23:25  sxf
**    Added flag to check up on health of Agc via iostat() function.
**
**    Revision 0.12  2004/04/15 16:27:26  mjf
**    Further tidying as part of QNX v6 port.
**
**    Revision 0.11  2004/04/15 15:57:32  mjf
**    Check-in of MAN work porting to QNX v6 prior to departure.
**
**    Revision 0.10  2003/09/09 09:04:28  man
**    Modified routine used to acquire current power setting. Routine
**    now returns error state if it fails to send or retrieve the power
**    setting.
**
**    Revision 0.9  2003/01/23 12:20:33  man
**    Included AgcPtc.c - a utility to produce
**    a photon transfer curve for testing and
**    calibration.
**
**    Revision 0.8  2002/10/15 09:13:27  man
**    Correction to FITS header prototype.
**
**    Revision 0.7  2002/10/15 09:09:28  man
**    Included function prototypes for FITS header
**    functions.
**
**    Revision 0.6  2002/10/10 10:00:59  man
**    Included macros to easily select required ISA buss
**    speed.
**
**    Revision 0.5  2002/07/08 15:20:57  man
**    Absorbed msiapi.h and msierr.h into Agc.h.
**
**    Revision 0.4  2002/06/21 07:33:06  sxf
**    Added msierrs.h
**
**    Revision 0.3  2002/06/10 10:04:39  man
**    Included some system error states.
**
**    Revision 0.2  2002/05/03 13:29:55  man
**    Included definitions for the PCI bios.
**
**    Revision 0.1  2002/05/01 10:14:08  man
**    *** empty log message ***
**
**
*******************************************************************************/


#ifndef AGC_H_DEFINED
#define AGC_H_DEFINED


/*
** System include files
*/
#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Cil.h"
#include "Clu.h"
#include "Log.h"
#include "Sdb.h"
#include "Agc.h"
#include "Mcp.h"
#include "Wfl.h"

typedef enum eAgcStatus_e
{
   E_AGC_GEN_ERR = STATUS_START(AGC),/* Miscellaneous error. */
   E_AGC_PCIFIND_ERR,           /* Error in finding PCI card */
   E_AGC_PCIREAD_ERR,           /* Error reading PCI card */
   E_AGC_PCIWRITE_ERR,          /* Error writing to PCI card */
   E_AGC_QNXCOM_ERR,            /* QNX command error */
   E_AGC_IRQ_ERR,               /* Error setting irq */
   E_AGC_PROXY_ERR,             /* Error setting proxy */
   E_AGC_COMMS_ERR,             /* Communication error */
   E_AGC_DISP_ERR,              /* Error with the dispatch handler */
   E_AGC_DEVATTACH_ERR,         /* Error attaching device name */
   E_AGC_PULSEATTACH_ERR,       /* Error attaching pulse */
   E_AGC_MSGCONNECT_ERR,        /* Error connecting message */
   E_AGC_HW_ERR,                /* Hardware error. */
   E_AGC_STREAM_ERR,            /* Error with stream buffer */
   E_AGC_SEND_COM_TIMEOUT,      /* SendCommand timedout */
   E_AGC_STATUS_EOL             /* End of status list marker (do not use) */
} eAgcStatus_t;

/* Constants */

#define MSI_IDENT_LENGTH    50
#define SENSOR_FRAME        1
#define SENSOR_FULL         2
#define SENSOR_INTERLINE    4
#define CAMERA_HIGH_SPEED   1
#define CAMERA_HIGH_RES     2
#define CAMERA_LOW_NOISE    4
#define CAMERA_LOW_GAIN     1
#define CAMERA_HIGH_GAIN    2
#define MSI_FILE_ROWSAREY   1
#define MSI_FILE_FLIPDATA   2
#define MSI_FILE_MIRRORDATA 4
#define MSI_INTERP_NONE     0
#define MSI_INTERP_BILINEAR 1
#define MSI_FILL_COPY       1
#define MSI_FILL_BLANK      2
#define MSI_FILL_NONE       2
#define MSI_KERNEL_LEFT     1
#define MSI_KERNEL_RIGHT    2
#define MSI_KERNEL_UP       3
#define MSI_KERNEL_DOWN     4
#define MSI_BAUD_1200       1
#define MSI_BAUD_4800       2
#define MSI_BAUD_9600       3
#define MSI_BAUD_14400      4
#define MSI_BAUD_19200      5
#define MSI_BAUD_38400      6
#define MSI_SERIAL_ECHO_ON  0
#define MSI_SERIAL_ECHO_OFF 1
#define MSI_SERIAL_FLOW_HWARE  2
#define MSI_SERIAL_FLOW_NONE   0
#define MSI_SERIAL_DATA_8BIT   0
#define MSI_SERIAL_DATA_7BIT   4
#define MSI_SERIAL_DATA_6BIT   8
#define MSI_SERIAL_STOP_1BIT   0
#define MSI_SERIAL_STOP_2BIT   16
#define MSI_SERIAL_PARITY_NONE 0
#define MSI_SERIAL_PARITY_ODD  32
#define MSI_SERIAL_PARITY_EVEN 64
#define MSI_CHUNK_SIZE         2048
#define MSI_TIMED_READ_CONTINUE  0
#define MSI_TIMED_READ_STOP_AND_WAIT  1
#define MSI_TIMED_READ_STOP_IMMEDIATE 2
#define MSI_MAX_WINDOWS  5

/* MSI camera states */
#define CAMOK                      -1
#define MSI_CANT_OPEN_DEVICE       -2
#define MSI_CAMERA_NOT_RESPONDING  -3
#define MSI_CAMERA_DATA_CORRUPT    -4
#define MSI_CAMERA_TIMED_OUT       -5
#define MSI_NOT_INITIALISED        -6
#define MSI_CANT_DESTROY           -7
#define MSI_BAD_PARAMETER          -8
#define MSI_CANT_HANDLE            -9
#define MSI_OUT_OF_MEMORY          -10
#define MSI_FILE_OPEN_FAILED       -11
#define MSI_BAD_FILE               -12
#define MSI_FILE_WRITE_FAILED      -13
#define MSI_FILE_READ_FAILED       -14
#define MSI_USER_CANCEL            -15
#define MSI_SERIAL_TIMED_OUT       -16
#define MSI_SERIAL_BUFFER_EMPTY    -17
#define	MSI_COMMAND_NOT_COMPLETE  -18
#define MSI_LUT_NOT_MONOTONIC      -19
#define MSI_UNKNOWN_HOST           -50
#define MSI_CANT_CONNECT           -51
#define MSI_NETCAM_BUSY            -52
#define E_AGC_DEVICENAME           "/dev/camera"

/* Converions between camera pixels and unsigned pixels */

#if 1
/* 16 bit conversion */
#define CAM_TO_UNSIGNED(x) ( 0x7fff-(int)(x) )
#define UNSIGNED_TO_CAM(x) ( (MSI_IMAGE_ELEMENT)(0x7fff-(x)) )
#else
/* 8 bit conversion */
#define CAM_TO_UNSIGNED(x) ( 0x7f-(int)(x) )
#define UNSIGNED_TO_CAM(x) ( (MSI_IMAGE_ELEMENT)(0x7f-(x)) )
#endif

/*
** Macro definitions
*/

#define E_AGC_MAX_FILENAME_LEN  256
#define E_AGC_BUFLEN            256

/*
** PCI definitions
** Register addresses obtained from Geode CS5530A
*/

#define I_AGC_VENDOR_ID    0x1078
#define I_AGC_DEVICE_ID    0x100
#define I_AGC_CLK_DIVIDER  0x50
#define I_AGC_CLK_DIV_MASK 0xF8   
#define I_AGC_CLK_DIV_8    0x7   
#define I_AGC_CLK_DIV_4    0x3 

/* Resource manager definitions */
#define I_AGC_RESMGR_MAXNPARTS  1
#define I_AGC_RESMGR_MAXMSGSIZE 2048
#define I_AGC_RESMGR_DEVNAME    "/dev/camera"
#define I_AGC_BYTESPERCOMMAND   4

/* 
** Stucture definitions
*/

/* The type of pixels from the camera */
typedef short MSI_IMAGE_ELEMENT;

/* Information about the sensor fitted to this camera */
typedef struct tagSENSORINFO {
   int siRowSize;
   int siColSize;
   int siBinFactor;
   int siStoreSize;
   int siActiveRowStart;
   int siActiveRowEnd;
   int siActiveColStart;
   int siActiveColEnd;
   int siSensorType;
} SENSORINFO;

/* Data returned from the camera */
typedef struct tagMSI_IMAGE {
   int imRowStart;
   int imColStart;
   int imRowEnd;
   int imColEnd;
   int imRowBins;
   int imColBins;
   int imImageRow;
   int imImageCol;
   MSI_IMAGE_ELEMENT *imData;
} MSI_IMAGE;

/* Window type for multiple readout areas */
typedef struct tagMSI_WINDOW {
   int wRowStart;
   int wColStart;
   int wRowEnd;
   int wColEnd;
} MSI_WINDOW;

/* Structure to hold an image histogram */
typedef struct tagMSI_HISTOGRAM {
   int hgNumBars;
   int hgMinValue; /* Min/Max grey level */
   int hgMaxValue;
   double hgBinSize; /* (hgMaxValue-hgMinValue)/hgNumBars */
   unsigned int hgMaxCount;  /* Counts in highest bin */
   unsigned int hgTotalCounts; /* Total pixels in HG */
   unsigned int *hgBins;
} MSI_HISTOGRAM;

/* Structure to hold an intensity profile */
typedef struct tagMSI_PROFILE {
   int pfNumPoints;
   double pfMinValue; /* Min/Max grey level */
   double pfMaxValue;
   double pfXStart;   /* Start and end points of the profile */
   double pfYStart;
   double pfXEnd;
   double pfYEnd;
   double pfXDelta; /* step size in pixels */
   double pfYDelta;
   double *pfPoints;
} MSI_PROFILE;

/* Structure to hold an overlay */
typedef struct tagMSI_OVERLAY {
   int olWidth;
   int olHeight;
   unsigned int olPixelsInside;
   unsigned char *olData;
} MSI_OVERLAY;

/* Structure to hold a convolution kernel */
typedef struct tagMSI_KERNEL {
   int kXSize;
   int kYSize;
   double *kMask;
} MSI_KERNEL;

/* A single FITS card image */
typedef struct tagMSI_CARD_IMAGE {
   char ciImage[80];
} MSI_CARD_IMAGE;

typedef struct tagMSI_LUT {
   int lutNumPoints;
   MSI_IMAGE_ELEMENT *lutInPts;
   MSI_IMAGE_ELEMENT *lutOutPts;
} MSI_LUT;


/* Callback Function for data reading */
typedef void (*MSI_READ_FRAME_CALLBACK)(MSI_IMAGE *,int,int,void*);
/* Callback for image processing functions */
typedef int (*MSI_PROCESS_CALLBACK)(MSI_IMAGE *,int,int,void *);
/* Callback for fits frames */
typedef int (*MSI_FITS_FRAME_CALLBACK)(MSI_IMAGE *,int,int,int,int,void*);
/* Callback for Timed Readout - called for each read frame */
typedef int (*MSI_TIMED_READ_CALLBACK)(const MSI_IMAGE *,int,int,float,float,int);


/*
** Function prototypes
*/

#define MsiExport

int eAgcDumpImage(     MSI_IMAGE *, char * );
int eAgcWriteFITSFile( MSI_IMAGE *,
                       char      *,
                       int        ,
                       int        ,
                       int        ,
                       int        ,
                       char      *,
                       double     , 
                       double     ,
                       int        ,
                       double     ,
                       int        , 
                       int        ,
                       double      );

int msiGetCamlibVersion(int *,int *, int *);
int msiGetSensorInfo(SENSORINFO *);
int msiInitialiseCamera(int);
int msiDestroyCameraDevice();
int msiGetIdent(char *);
int msiFan(int);
int msiShutter(int);
int msiSetShutterBytes(unsigned char,unsigned char);
int msiDisableShutterBytes();
int msiWriteAuxilliaryOutput(unsigned char);
int msiReadAuxilliaryInput(unsigned char *);
int msiSetSpeed(int, int);
int msiChargeClear(int);
int msiSetIntegrateTime(int);
int msiIntegrate(int,int);
int msiIntegrateWhileTriggerHigh();
int msiWaitForTriggerHigh();
int msiHasCommandCompleted();
int msiCoolerPID(int,int,int,int);
int msiCoolerPIDReset();
int msiSetCoolerTemperature(float);
int msiReadChipTemperature(float *);
int msiReadCaseTemperature(float *);
int msiSetCoolerPower(int);
int msiChargeToStore(); 
int msiSetReadout(int,int,int,int,int,int);
int msiReadStore(MSI_IMAGE *);
int msiReadRow(MSI_IMAGE_ELEMENT *);
int msiReadFrame(MSI_IMAGE *, MSI_READ_FRAME_CALLBACK, void *, int); 
int msiImageMemory();
int msiImageSize(int *, int *); 
int msiMoveCharge(int);
int msiReadCoolerPower(int *);
int msiFlashCalibrationLight(int);
int msiCopyImagePortion(MSI_IMAGE *,int,int,int,int,MSI_IMAGE*,int,int);
int msiFillImagePortion(MSI_IMAGE *,int,int,int,int,MSI_IMAGE_ELEMENT);
int msiComputeDifferenceUnsigned(MSI_IMAGE *,int,int,int,int,MSI_IMAGE*,int,int);
int msiComputeDifferenceSigned(MSI_IMAGE *,int,int,int,int,MSI_IMAGE*,int,int);
int msiInvert(MSI_IMAGE *,int,int,int,int);
int msiComputeDistribution(MSI_IMAGE *,int,int,int,int,double *,double *);
int msiComputeMean(MSI_IMAGE *,int,int,int,int,double *);
int msiCalibrate(double *,double *,double *,double *,int,int,int,int,double *,double *, FILE *);
int msiComputeMaxMin(MSI_IMAGE *,int,int,int,int,MSI_IMAGE_ELEMENT *,MSI_IMAGE_ELEMENT *);
int msiComputeHistogram(MSI_IMAGE *,int,int,int,int,MSI_HISTOGRAM *);
int msiComputeThreshold(MSI_IMAGE *,int,int,int,int,MSI_IMAGE_ELEMENT, MSI_IMAGE_ELEMENT,MSI_OVERLAY *,unsigned char,unsigned char);
int msiComputeProfile(MSI_IMAGE *,MSI_PROFILE *,int);
double msiInterpolatePixel(MSI_IMAGE *,double,double,int);
int msiFileWriteFrames(char *,MSI_IMAGE **,int,int,time_t,unsigned short,int);
int msiFileFrameCount(char *,int *);
int msiFileFrameInfo(char *,int,int *,int *,int *,time_t *,unsigned short *);
int msiFileReadFrames(char *,MSI_IMAGE **,int,int);
int msiCancelIntegration();
int msiKernelRectangle(MSI_KERNEL *,int,int);
int msiKernelCircle(MSI_KERNEL *,double);
int msiKernelGaussian(MSI_KERNEL *,double,double,int,int);
int msiKernelDerivativeGaussian(MSI_KERNEL *,double,double,int,int,int);
int msiKernelSobel(MSI_KERNEL *,int);
int msiKernelPrewitt(MSI_KERNEL *,int);
int msiKernelLaplacian3(MSI_KERNEL *);
int msiComputeKernelScale(MSI_KERNEL *,double *);
int msiScaleKernel(MSI_KERNEL *,double);
int msiDeleteKernelMemory(MSI_KERNEL *);
int msiKernelConvolve(MSI_IMAGE *,int,int,int,int,MSI_IMAGE*,int,int,MSI_KERNEL *,int,MSI_IMAGE_ELEMENT, MSI_PROCESS_CALLBACK, void *);
int msiFitsBuildLogicalCard(MSI_CARD_IMAGE *,char *,int);
int msiFitsBuildIntegerCard(MSI_CARD_IMAGE *,char *,int);
int msiFitsBuildFloatCard(MSI_CARD_IMAGE *,char *,double);
int msiFitsStringToCard(MSI_CARD_IMAGE *,char *);
int msiExportFits(char *,MSI_FITS_FRAME_CALLBACK,void *,int,int,int,int,MSI_CARD_IMAGE *,int,double,double);
int msiSerialSetup(int,int);
int msiSerialIsPortReady(int *);
int msiSerialGetQueueLength(int *);
int msiSerialSendString(char *,int,int);
int msiSerialGetString(char *,int,int *);
int msiRotateImagePortion(MSI_IMAGE *,MSI_IMAGE *,int,int,int,int,double,double,double,int,MSI_IMAGE_ELEMENT);
int msiHistogramEqualise(MSI_IMAGE *,int,int,int,int);
int msiInitialiseLUT(MSI_LUT *);
int msiAddPointToLUT(MSI_LUT *,MSI_IMAGE_ELEMENT, MSI_IMAGE_ELEMENT);
int msiDeletePointFromLUT(MSI_LUT *,MSI_IMAGE_ELEMENT);
int msiFreeLUTMemory(MSI_LUT *);
int msiTranslateThroughLUT(MSI_IMAGE *,MSI_LUT *,int,int,int,int);
int msiGetLUTEntry(MSI_LUT *,double,double *);
int msiSetGammaLUT(MSI_LUT *,double,int);
int msiSetLogarithmicLUT(MSI_LUT *,int);
int msiSetLinearLUT(MSI_LUT *,int,int,int);
int msiSetHistEqualiseLUT(MSI_LUT *,MSI_IMAGE *,int,int,int,int,int);
int msiScaleLUT(MSI_LUT *,double,double);
int msiRelaxationLabel(MSI_IMAGE*,MSI_IMAGE *,int,int,int,int,int);
int msiResetFIFO();
int msiBeginTimedReadout(MSI_IMAGE *, MSI_TIMED_READ_CALLBACK, int);
int msiEndTimedReadout();
int msiHasTimedReadoutCompleted();
int msiInitialiseMultipleReadout();
int msiDestroyMultipleReadout();
int msiAddWindowToMultipleReadout(MSI_WINDOW *);
int msiMultipleWindowMemory();
int msiReadMultipleWindowsRaw(MSI_IMAGE *);
int msiRegisterSimulatedImageStore(char *, int, int, int);
int msiCancelSimulatedImages();
int msiTestFan(int *);
int msiTurnLEDOn( void );
int msiTurnLEDOff( void );

unsigned int eAgcCrc32( unsigned int , const unsigned char *, unsigned int );
int eAgcWriteCard(FILE *, MSI_CARD_IMAGE *);
int eAgcWriteCards(FILE *, MSI_CARD_IMAGE *, int );
int eAgcBlanckCard(MSI_CARD_IMAGE *);
int eAgcPadWithBlanks(FILE *, int );
int eAgcCopyKeyword(MSI_CARD_IMAGE *, char * );
int eAgcFitsBuildLogicalCard( MSI_CARD_IMAGE *, char *, int);
int eAgcFitsBuildIntegerCard( MSI_CARD_IMAGE *, char *, int);
int eAgcFitsBuildFloatCard( MSI_CARD_IMAGE *, char *, double);
int eAgcFitsStringToCard( MSI_CARD_IMAGE *, char * );
int eAgcWriteSimpleStringCard( FILE *, char *);
int eAgcWriteLogicalCard( FILE *, char *, int );
int eAgcWriteIntegerCard( FILE *, char *, int );
int eAgcWriteFloatCard( FILE *, char *, double );

#endif

/*******************************************************************************
** End of File Name: Agc.h
*******************************************************************************/
