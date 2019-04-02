/*
** BciMain.c
**
** Main module of the Boltwood Cloud Interface.
**
*/


/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <syslog.h>


/*
** Local include files
*/

#define M_BCI_MAIN_C                /* ensure data area declared in one place */
#include "BciPrivate.h"


/*
** main
**
** Top level function.
**
*/

int main( int argc, char *argv[] )
{
   char     OutFilepath[ FILENAME_MAX ]; /* name of output path to create */
   char     OutFilestem[ FILENAME_MAX ]; /* stem of output filename */
   char     RawFilename[ FILENAME_MAX ]; /* name of raw output file to create */
   char     PortName[ FILENAME_MAX ];  /* name of serial port to use */
   int      PortId;                    /* file descriptor for serial port */
   int      NoCommsCount;              /* count of instances of no comms */
   struct timespec SleepTime;          /* to implement delays */
   struct timespec RemainingTime;      /* to implement delays */

   /* initialise all internal data and parse the command-line */
   iBciSetup( argc, argv, PortName, OutFilepath, OutFilestem, RawFilename );

   /* if a port argument hasn't been supplied, exit */
   if ( strlen( PortName ) == 0 )
   {
      /* exit */
      exit( EXIT_SUCCESS );
   }

   /* open and configure the serial port */
   PortId = iBciInitPort( PortName );

#if 0
   /* if a reset has been requested */
   if ( strcmp( OutputFilename, I_BCI_FILENAME_RESET ) == 0 )
   {
      /* issue reset command to the device */
      if ( iBciIssueReset( PortId ) == TRUE )
      {
         syslog( LOG_NOTICE, "Reset issued successfully to device\n" );

         /* exit, success */
         exit( EXIT_SUCCESS );
      }
      else
      {
         syslog( LOG_ERR, "Failed to reset the device\n" );

         /* exit, failure */
         exit( EXIT_FAILURE );
      }
   }
#endif

   /* zero the tally of no communications to the device */
   NoCommsCount = 0;

   /* send an initial request byte to re-start comms */
   iBciSendRequest( PortId );

   /* loop continually retrieving status data */
   while( 1 )
   {
      /* attempt to retreive status data */
      if ( iBciGetStatusData( PortId, RawFilename ) == TRUE )
      {
         syslog( LOG_INFO, "Got some data" );

         /* if status retrieved OK, write output file */
         iBciWriteOutputFile( OutFilepath, OutFilestem );

         /* zero the count of no communications */
         NoCommsCount = 0;

         /* wait until next time */
         SleepTime.tv_sec  = I_BCI_PERIOD_SECS;
         SleepTime.tv_nsec = I_BCI_PERIOD_NSECS;
         nanosleep( &SleepTime, &RemainingTime );
      }
      /* otherwise try and re-establish comms */
      else
      {
         /* increment the count of no comms */
         NoCommsCount++;

         syslog( LOG_INFO, "NoCommsCount %d", NoCommsCount );

         /* if we've not communicated for a while */
         if ( NoCommsCount >= I_BCI_NO_COMMS_LIMIT )
         {
            /* send a request byte to try and re-start comms */
            iBciSendRequest( PortId );

            /* zero the count of no communications */
            NoCommsCount = 0;
         }

         /* short wait */
         SleepTime.tv_sec  = I_BCI_RETRY_SECS;
         SleepTime.tv_nsec = I_BCI_RETRY_NSECS;
         nanosleep( &SleepTime, &RemainingTime );
      }
   }

   return EXIT_SUCCESS;

}


/*
** iBciSetup
**
** Performs initial setup of internal data.
**
*/

void iBciSetup( int argc, char *argv[], char * PortName, 
                char * OutFilepath, char * OutFilestem, char * RawFilename )
{
   int      ArgIndex;
   char   * PortNamePtr;

   /* write in the default port name */
   strcpy( PortName, I_BCI_DFLT_PORT_NAME );

   /* clear down the measurement data store */
   memset( &iBciData, sizeof( iBciData ), 0 );

   /* write in the default output file path */
   strcpy( OutFilepath, I_BCI_DFLT_OUTPATH );

   /* clear down the filenames initially */
   memset( OutFilestem, sizeof( OutFilestem ), 0 );
   memset( RawFilename, sizeof( RawFilename ), 0 );

   /* loop around command-line arguments */
   for ( ArgIndex = 0; ArgIndex < argc; ArgIndex++ )
   {
      /* process relevant argument */
      switch ( ArgIndex )
      {
         /* if argument is expected to be the port namne */
         case I_BCI_ARG_PORTNAME : strcpy( PortName, argv[ ArgIndex ] );
                                   break;
                                   
         /* if argument is expected to be the output filepath */
         case I_BCI_ARG_FILEPATH : strcpy( OutFilepath, argv[ ArgIndex ] );
                                   break;

         /* if argument is expected to be the raw output file */
         case I_BCI_ARG_RAWFILE :  strcpy( RawFilename, argv[ ArgIndex ] );
                                   break;

         /* no other arguments relevant */                           
         default :                 break;
      }
   }

   /* construct stem for output files from the port name */
   PortNamePtr = strrchr( PortName, I_BCI_DIR_SEPARATOR );
   
   /* we only want the bit after the last '/' */
   if ( PortNamePtr != NULL )
   {
      strcpy( OutFilestem, PortNamePtr + 1 );
   }
   /* else just take the entire port name */
   else
   {
      strcpy( OutFilestem, PortName );
   }

   /* if a port has been supplied */
   if ( strlen( PortName ) > 0 )
   {
      /* open the syslog */
      setlogmask ( LOG_UPTO( LOG_NOTICE ) );
      openlog ( I_BCI_PROGRAM_NAME, LOG_CONS | LOG_NDELAY, LOG_SYSLOG );

      /* output some sign-on details */
      syslog( LOG_NOTICE, "%s v%d.%02d, %s\n", 
              I_BCI_PROGRAM_NAME, I_BCI_MAJOR_VERSION, 
              I_BCI_MINOR_VERSION, I_BCI_RELEASE_DATE );
      syslog( LOG_NOTICE, "%s\n", 
              I_BCI_PROGRAM_ABOUT );
      syslog( LOG_NOTICE, "Las Cumbres Observatory %s (http://www.lcogt.net)\n",
              I_BCI_YEAR );
      syslog( LOG_NOTICE, "Using device '%s'\n", PortName );
      syslog( LOG_NOTICE, "Creating output files in '%s' using stem '%s'\n", 
              OutFilepath, OutFilestem );
      if ( strlen( RawFilename ) > 0 )
      {
         syslog( LOG_NOTICE, "Creating raw file '%s'\n", RawFilename );
      }
   }
   /* othwerwise produce the usage information */
   else
   {
      printf( "%s v%d.%02d, %s\n", 
              I_BCI_PROGRAM_NAME, I_BCI_MAJOR_VERSION, 
              I_BCI_MINOR_VERSION, I_BCI_RELEASE_DATE );
      printf( "%s\n", 
              I_BCI_PROGRAM_ABOUT );
      printf( "Las Cumbres Observatory %s (http://www.lcogt.net)\n",
              I_BCI_YEAR );
      printf( "Usage: Bci <device file> {<output path> | reset} {raw file}\n" );
      printf( "   <device file>              Name of device file for port\n" );
      printf( "   <output path>              Path for output files\n" );
      printf( "   reset                      Issue reset to device then terminate\n" );
      printf( "   <raw file>                 Path/name of raw data output file\n" );
   }

}


/*
** EOF
*/

