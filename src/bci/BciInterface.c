/*
** BciInterrface.c
**
** Contains functions to implement the interface to the Boltwood Cloud Sensor.
**
*/


/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <syslog.h>


/*
** Local include files
*/

#include "BciPrivate.h"


/*
** Module-level definitions
*/

/* message definitions */

#define M_BCI_REQUEST_STRING "\001"
#define M_BCI_COMMAND_STRING "\002%c\n"
#define M_BCI_MESSAGE_STRING "\002%c%s\n"

#define M_BCI_HEADER_CHAR    '\002'
#define M_BCI_FOOTER_CHAR    '\n'

#define M_BCI_FRAME_COMMAND  'C'
#define M_BCI_FRAME_POLL     'P'
#define M_BCI_FRAME_MESSAGE  'M'

#define M_BCI_MSG_STATUS     'D'
#define M_BCI_MSG_ID_CALIB   'T'
#define M_BCI_MSG_WET_CALIB  'K'
#define M_BCI_MSG_TERM_CALIB 'C'

#define M_BCI_CHAR_WET       'W'
#define M_BCI_CHAR_PREV_WET  'w'

#define M_BCI_OFFSET_FRAME   0
#define M_BCI_OFFSET_TYPE    1

/* timeouts in nanoseconds for transmitting/receiving messages */

#define M_BCI_TX_TIMEOUT     1000000000
#define M_BCI_RX_TIMEOUT     1000000000
#define M_BCI_NO_TIMEOUT     0

/* size of incoming serial port buffer */

#define M_BCI_BUFFER_SIZE    256

/* expected length of incoming status string */

#define M_BCI_STATUS_STR_LEN 160

/* number of arguments to parse from string */

#define M_BCI_PARSE_COUNT    6

/* various indicators for specific conditions */

#define M_BCI_SKYAMB_LIMIT   -998.0
#define M_BCI_INV_SKYAMB     999.0
#define M_BCI_INV_WIND_SPEED -999.0
#define M_BCI_WETNESS_WET    1
#define M_BCI_WETNESS_DRY    0

/* unit conversion factors */

#define M_BCI_KMH_TO_MS      ( 1000.0 / ( 60.0 * 60.0 ) )


/*
** iBciSendRequest
** 
** Sends a request character. This is the only communication instigated by the 
** host.
**
** Returns TRUE if successful.
** 
*/

int iBciSendRequest( int PortId )
{
   int      CharsSent;
   int      SuccessFlag;

   /* log a message */
   syslog( LOG_INFO, "Sending request string to device\n" );

   /* send the request string to the device */
   CharsSent = iBciPortSend( PortId, M_BCI_REQUEST_STRING, 
                             strlen( M_BCI_REQUEST_STRING ), 
                             M_BCI_TX_TIMEOUT  );

   /* if all characters weren't sent */
   if ( CharsSent != strlen( M_BCI_REQUEST_STRING ) )
   {
      syslog( LOG_ERR, "Unable to send request string\n" );
      SuccessFlag = TRUE;
   }
   else
   {
      SuccessFlag = FALSE;
   }

   return SuccessFlag;

}


/*
** iBciGetStatusData
**
** Retrieves the status measurements from the Cloud Sensor. This involves a 
** check for the incoming message type - it might not necessarily be status 
** data. The raw data from the sensor is logged to a file.
**
** Returns TRUE if status data successfully received.
** 
*/

int iBciGetStatusData( int PortId, char * RawFilename )
{
   int      CharsReceived;
   int      BufferIndex;
   int      StatusReceived;
   char     HeaderChar;
   char     IncomingChar;
  
   char     Buffer[ M_BCI_BUFFER_SIZE ];
   char     TimeBuffer[ M_BCI_BUFFER_SIZE ];
   FILE *   RawFile;
   time_t   CurrentTime;

   /* attempt to receive the header character from the port */
   HeaderChar = 0;
   CharsReceived = iBciPortReceive( PortId, &HeaderChar, 
                                    1, M_BCI_RX_TIMEOUT );

   /* if nothing received, or it's not the header character */
   if ( ( CharsReceived <= 0 ) || ( HeaderChar != M_BCI_HEADER_CHAR ) )
   {
      /* return indicating we didn't receive valid status */
      return FALSE;
   }

   /* we've got the header, prepare to receive the remainder of the message */
   BufferIndex = 0;

   /* we've got the header, read characters until the end of message */
   do
   {
      /* attempt to receive the next character from the port */
      CharsReceived = iBciPortReceive( PortId, &IncomingChar, 
                                       1, M_BCI_RX_TIMEOUT );

      /* if something arrived, copy into the buffer and increment the counter */
      if ( CharsReceived >= 0 )
      {
         Buffer[ BufferIndex ] = IncomingChar;
         BufferIndex++;
      }
   } 
   /* keep going until the buffer is full or we get the end of message */
   while ( ( BufferIndex < M_BCI_BUFFER_SIZE ) 
            && ( IncomingChar != M_BCI_FOOTER_CHAR ) );
                            
   /* if end of buffer reached, bail out ready for another go next time */
   if ( BufferIndex == M_BCI_BUFFER_SIZE ) 
   {
      /* return indicating we didn't receive valid status */
      return FALSE;
   }

   /* ensure buffer is NULL terminated */
   Buffer[ BufferIndex ] = '\0';

   /* initially assume we'not received a valid status message */
   StatusReceived = FALSE;

   /* now check the type of incoming frame */
   switch ( Buffer[ M_BCI_OFFSET_FRAME ] )
   {
      /* command frame */
      case M_BCI_FRAME_COMMAND :
         /* nothing to do here */
         break;
         
      /* poll frame */
      case M_BCI_FRAME_POLL :
         /* nothing to do here */
         break;
         
      case M_BCI_FRAME_MESSAGE :
         /* we've got a valid message, output it to the raw file if enabled */
         if ( strlen( RawFilename ) > 0 )
         {
            RawFile = fopen( RawFilename, "a" );
            if ( RawFile != NULL )
            {
               /* include time-stamp in message */
               if ( time( &CurrentTime ) != -1 )
               {
                  strftime( TimeBuffer, I_BCI_BUFFER_LEN, "%Y%m%d %H:%M:%S",  
                            gmtime( (time_t *) &( CurrentTime ) ) );
                  fprintf( RawFile, "%s : %s", TimeBuffer, Buffer + 1 );
                  fclose( RawFile );
               }
            }
         }
        
         /* process depending on type of message */
         switch ( Buffer[ M_BCI_OFFSET_TYPE ] )
         {
            /* status data */
            case M_BCI_MSG_STATUS :
               /* parse the data stream, pulling out the data we want */
               StatusReceived = mBciParseDataStream( Buffer );
               if ( StatusReceived == TRUE )
               {
                  syslog( LOG_INFO, "Status received '%s'", Buffer );
               }
               break;
            /* serial number, firmware, revision, calibration data */
            case M_BCI_MSG_ID_CALIB :
               /* log the serial number, firmware revision, etc. */
               syslog( LOG_NOTICE, Buffer + 1 );
               break;
      
            /* wetness oscillator calibration data */
            case M_BCI_MSG_WET_CALIB :
               /* log this for completeness */
               syslog( LOG_NOTICE, Buffer + 1 );
               break;
      
            /* thermopile calibration data */
            case M_BCI_MSG_TERM_CALIB :
               /* log this for completeness */
               syslog( LOG_NOTICE, Buffer + 1 );
               break;
      
            /* unrecognised message */
            default :
               /* log the error */
               syslog( LOG_ERR, "Unrecognised incoming message '%s'", Buffer );
               break;
         }
         break;

      default :
         break;
   }

   return StatusReceived;

}


/*
** iBciIssueReset
**
** Issues a reset to the Cloud Sensor.
**
** Returns TRUE if successful.
** 
*/

int iBciIssueReset( int PortId )
{


   return TRUE;

}


/*
** mBciParseDataStream  
** 
** Parse a stream of status from the sensor, pulling out only the relevant data.
**
** Return TRUE if the stream successfully parsed.
**
*/

int mBciParseDataStream( char * Buffer )
{
   int      StatusParsed;
   int      Items;
   double   SkyAmbient;
   double   WindSpeed;
   char     Wetness;

   /* initially assume status is not parsed, no items parsed */
   StatusParsed = FALSE;
   Items = 0;

   /* string is expected to be of a fixed length */
   if ( strlen( Buffer ) != M_BCI_STATUS_STR_LEN )
   {
      /* report the incorrectly-formed string */
      syslog( LOG_ERR, "Incorrect length of status string '%s'\n", Buffer );
   }
   /* otherwise attempt to retrieve the relevant parameters */
   else
   {
      Items = sscanf( Buffer, 
                  "M%*c %*c %*c %*c %*c %*c %*c %lf %lf %lf %c %*c %lf %lf %*s",
                      &SkyAmbient, 
                      &iBciData.Temperature,
                      &WindSpeed, 
                      &Wetness, 
                      &iBciData.Humidity,
                      &iBciData.DewPoint );
   }

   /* check we've parsed the required number of arguments */
   if ( Items == M_BCI_PARSE_COUNT )
   {
      /*
      ** Sky ambient may also be :-
      **  999.9 - saturated  hot
      ** -999.9 - saturated  cold
      ** -998.9 - wet
      */
      if ( SkyAmbient <= M_BCI_SKYAMB_LIMIT )
      {
         iBciData.SkyAmbient = M_BCI_INV_SKYAMB;
      }
      else
      {
         iBciData.SkyAmbient = SkyAmbient;
      }

      /* 
      ** Wind speed not reported correctly when wet :-
      ** -1 - if still heating up,
      ** -2 - if wet
      ** -3 - if the A/D from the wind probe is bad 
      ** -4 - if the probe is not heating
      ** We want to set an 'invalid' number for all these cases.
      */
      if ( WindSpeed < 0.0 )
      {
         iBciData.WindSpeed = M_BCI_INV_WIND_SPEED;
      }
      else
      {
         /* convert wind speed from km/hr to m/s */
         iBciData.WindSpeed = WindSpeed * M_BCI_KMH_TO_MS;
      }

      /* 
      ** Wetness character reported as :-
      ** 'N' - when dry
      ** 'W' - when wet now
      ** 'w' - when wet in last minute
      ** We want to assign a number accordingly.
      */
      if ( ( Wetness == M_BCI_CHAR_WET ) || ( Wetness == M_BCI_CHAR_PREV_WET ) )
      {
         iBciData.Wetness = M_BCI_WETNESS_WET;
      }
      else
      {
         iBciData.Wetness = M_BCI_WETNESS_DRY;
      }

      /* status parsed successfully */
      StatusParsed = TRUE;
   }

   return StatusParsed;

}


/*
** EOF
*/

