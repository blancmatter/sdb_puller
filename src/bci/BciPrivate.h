/*
** BciPrivate.h
**
** Private header file for Boltwood Cloud Sensor Interface.
**
*/

#ifndef BCI_PRIVATE_H_DEFINED
#define BCI_PRIVATE_H_DEFINED


/*
** Macro definitions
*/

/* about the BCI */

#define I_BCI_PROGRAM_NAME   "BCI"
#define I_BCI_PROGRAM_ABOUT  "Boltwood Cloud Sensor Interface"
#define I_BCI_YEAR           "2008"
#define I_BCI_RELEASE_DATE   "29 February 2008"
#define I_BCI_MAJOR_VERSION  1
#define I_BCI_MINOR_VERSION  4

/* define command-line arguments */

#define I_BCI_ARG_PORTNAME   1
#define I_BCI_ARG_FILEPATH   2
#define I_BCI_ARG_RAWFILE    3

/* option to issue reset to device then terminate */

#define I_BCI_FILENAME_RESET "reset"

/* definition of defaults for port and output filename */

#define I_BCI_DFLT_PORT_NAME ""
#define I_BCI_DFLT_OUTPATH   "./"

/* definitions for output filenames */

#define I_BCI_WMS_FILEPREFIX "Wms"
#define I_BCI_RCS_FILEPREFIX "Rcs"
#define I_BCI_OUT_FILEEXT   ".txt"
#define I_BCI_TMP_FILEEXT   ".tmp"
#define I_BCI_DIR_SEPARATOR '/'

/* period between attempting to establish comms to the device */

#define I_BCI_RETRY_SECS     1
#define I_BCI_RETRY_NSECS    0

/* period between querying the device */

#define I_BCI_PERIOD_SECS    0
#define I_BCI_PERIOD_NSECS   1000000

/* permitted instances of no communications */

#define I_BCI_NO_COMMS_LIMIT 5 
                               
/* length for declaring buffers */

#define I_BCI_BUFFER_LEN     256

/* for declaring, defining and initialising global variables */

#ifdef M_BCI_MAIN_C
#define I_BCI_EXTERN
#define I_BCI_INIT( Initialisation ) = Initialisation
#else
#define I_BCI_EXTERN         extern
#define I_BCI_INIT( Initialisation )
#endif

/* TRUE and FALSE */

#undef FALSE
#undef TRUE

#define FALSE                0
#define TRUE                 1


/*
** Type definitions
*/

/* structure for BCI data store */

typedef struct
{
   double   Temperature;
   double   Humidity;
   double   DewPoint;
   double   WindSpeed;
   int      Wetness;
   double   SkyAmbient;
} iBciData_t;


/*
** Variables - external to all except BciMain
*/ 

/* BCI data area */

I_BCI_EXTERN iBciData_t iBciData;


/*
** Function prototypes
*/

/* BciMain.c */

void     iBciSetup           ( int argc, char *argv[], char * PortName, 
                               char * OutFilepath, char * OutFilestem, 
                               char * RawFilename );

/* BciInterface.c */

int      iBciSendRequest     ( int PortId );
int      iBciGetStatusData   ( int PortId, char * RawFilename );
int      iBciIssueReset      ( int PortId );

/* BciSerial.c */

int      iBciInitPort        ( char * PortName );
int      iBciPortSend        ( int PortId, char * DataPtr, int DataCount, 
                               int Timeout );
int      iBciPortReceive     ( int PortId, char * DataPtr, int MaxData,   
                               int Timeout );

/* BciOutput.c */

void     iBciWriteOutputFile ( char * OutFilepath, char * OutFilestem );


#endif

/*
** EOF
*/

