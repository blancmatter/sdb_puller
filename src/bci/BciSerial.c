/*
** BciSerial.c
**
** Contains functions for accessing the input serial port.
**
*/


/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <syslog.h>


/*
** Local include files
*/

#include "BciPrivate.h"


/*
** Module-level definitions
*/

/* defaults for serial port configuration */

#define M_BCI_PORT_DATABITS  0         /* CS5, CS6, CS7, CS8 */
#define M_BCI_PORT_STOPBITS  1         /* 1, 2 */


/*
** iBciInitPort
** 
** Opens and configures the specified port.
**
** Returns the handler to the opened port, -1 on error.
**
*/

int iBciInitPort( char * PortName )
{
   int      PortId;                    /* returned handler to port */
#if 0
   struct termios TermParams;          /* structure of device parameters */
#endif

   /* attempt to open the port */
   PortId = open( PortName, O_RDWR | O_NOCTTY | O_NDELAY);

   /* if port opened successfully */
   if ( PortId != -1 )
   {
      syslog( LOG_NOTICE, "Device file '%s' opened successfully\n", PortName );

#if 0
      /* configure the serial port, start by retrieving existing parameters */
      tcgetattr( PortID, &TermParams );
        
      /* reset the number of data bits in the TermParams flag */
      TermParams.c_cflag &= ~( CS5 | CS6 | CS7| CS8 );

      /* set the number of data bits in the TermParams flag */
      TermParams.c_cflag |= M_BCI_PORT_DATABITS;
      
      /* set the stop bits to be either 1 or 2 */
      switch( M_BCI_PORT_STOPBITS )
      { 
         case 1 :  TermParams.c_cflag &= ~CSTOPB;
                   break;
         case 2 :  TermParams.c_cflag |= CSTOPB;
                   break;
         default : break;
      }

      /* reset the parity checking bits */
      TermParams.c_cflag &= ~( PARENB | PARODD );

      /* now set the parity */
      switch( Parity )
      {
         /* No parity */
         case 
            break;
         /* Odd parity */
         case 
            TermParams.c_cflag |= PARENB;
            TermParams.c_cflag |= PARODD;
            break;
         /* Even parity */
         case 
            TermParams.c_cflag |= PARENB;
            break;
         /* Invalid parity */
         default:
            break;
      }

      syslog( LOG_NOTICE, "Device file '%s' configured successfully\n", 
              PortName );
#endif

      /* attempt to flush input buffer on serial device */
      if( tcflush( PortId, TCIOFLUSH ) != -1 )
      {
         syslog( LOG_INFO, "Device file '%s' flushed successfully\n", 
                 PortName );
      }
      else
      {
         syslog( LOG_ERR, "Unable to flush device file '%s'\n", PortName );
      }
   }
   else
   {
      /* unable to open serial port */
      syslog( LOG_ERR, "Unable to open device file '%s'\n", PortName );
   }

   return PortId;

}


/*
** iBciPortSend
** 
** Sends the specified characters down the serial port. A timeout is provided
** (nanoseconds) in case the other end isn't ready to receive.
**
** Returns the number of bytes transmitted.
**
*/

int iBciPortSend( int PortId, char * DataPtr, int DataCount, int Timeout )
{
   int      i;
   int      CharsSent;
  
   /*
   ** Timeout not currently implemented - to implement if required.
   */

   syslog( LOG_DEBUG, "Sending %d chars\n", DataCount );

   /* write the characters down the port */
   CharsSent = write( PortId, DataPtr, DataCount );

   syslog( LOG_DEBUG, "%d chars sent\n", CharsSent );

   /* check that the transmission actually worked */
   if( CharsSent == -1 )
   {
      /* transmission failed - report it */
      syslog( LOG_ERR, "Unable to transmit to device\n" );
   }

   /* Check that we have transmitted what we expected */
   if( CharsSent != DataCount )
   {
      /* transmission was incomplete - report it */
      syslog( LOG_ERR, "Attempted to transmit %d characters, %d sent\n",
              DataCount, CharsSent );
   }

   return CharsSent;

}


/*
** iBciPortReceive
** 
** Attempts to receive characters from the serial port. The maximum number of
** characters is provided to prevent buffer overflow, together with a timeout 
** to wait for the data to be fully received.
**
** Returns the number of characters received.
**
*/

int iBciPortReceive( int PortId, char * DataPtr, int MaxData, int Timeout )
{
   int      CharsReceived;

   /*
   ** Timeout not currently implemented - to implement if required.
   */

   syslog( LOG_DEBUG, "Waiting to receive up to %d chars\n", MaxData );

   /* read the characters from the port */
   CharsReceived = read( PortId, DataPtr, MaxData );

   syslog( LOG_DEBUG, "Received %d chars\n", CharsReceived );
 
   /* if read was unsuccessful, rather than zero characters received */
   if ( CharsReceived == -1 )
   {
      /* check 'errno' */
      switch ( errno )
      {
         /* zero data is not an error */
         case EAGAIN : CharsReceived = 0;
                       break;

         default     : syslog( LOG_ERR, 
                               "Error receiving from device, errno %d\n", 
                               errno );
         break;
      }
   }

   return CharsReceived;

}


/*
** EOF
*/
