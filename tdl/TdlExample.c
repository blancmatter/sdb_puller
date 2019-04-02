/*
** TdlExampleTx.c
**
** This program transmits or receives test messages using CIL addresses TU1/TU2.
**
** To compile this test program as a transmitter 'Tx' :-
**  cc TdlExample.c -DTX -o Tx Tdl.lib
** To compile this test program as a received 'Rx' :-
**  cc TdlExample.c -DRX -o Rx Tdl.lib
**
** The two executables 'Tx' and 'Rx' are then ready for use.
*/

/*
** Include Files
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>                    /* for 'sleep' */

#include "Tdl.h"


/*
** Definitions
*/

/* Location of CIL map file */

#define M_TDL_CIL_MAP       "Cil.map"

/* CIL ID for program to use */

#define M_TDL_SOURCE_ID      E_CIL_TU1
#define M_TDL_DESTN_ID       E_CIL_TU2

/* Size of test message packet (32-bit words) */

#define M_TDL_PACKET_SIZE    64

/* Test parameters for test packet */

#define M_TDL_TEST_CLASS     E_CIL_CMD_CLASS
#define M_TDL_TEST_SERVICE   1

/* Length of delay beween transmitting packets (seconds) */

#define M_TDL_TX_DELAY_SEC   1

/* Length of time to poll for incoming CIL message (milli-seconds) */

#define M_TDL_RX_DELAY_MSEC  10


/*
** Main Function
*/

int main( int argc, char *argv[] )
{
   int      WordCount;                 /* word index into a packet */
   long     ReferencePacket            /* reference packet for verification */
               [ M_TDL_PACKET_SIZE ];  
   long     TestPacket                 /* test packet to transmit or receive */
               [ M_TDL_PACKET_SIZE ];
   char     TimeBuffer                 /* text buffer for displaying the time */
               [ E_TIM_BUFFER_LENGTH ];
   Int32_t  CilId;                     /* CIL ID occupied by this process */
   Int32_t  DelivererId;               /* deliverer of incoming packet */
   Status_t Status;                    /* status of functions called */
   eCilMsg_t CilMsg;                   /* CIL message structure  */

   /* Fill test packet with an incrementing count */
   for ( WordCount = 0; WordCount < M_TDL_PACKET_SIZE; WordCount++ )
   {
      /* Assign contents of the reference packet */
      ReferencePacket[ WordCount ] = WordCount + 1;
   }

   /* Determine the CIL ID to use */
#ifdef TX
   CilId = M_TDL_SOURCE_ID;
#endif

#ifdef RX
   CilId = M_TDL_DESTN_ID;
#endif

   /* Attempt the CIL setup */
   Status = eCilSetup( M_TDL_CIL_MAP, CilId );

   /* Check for and report any errors */
   if ( Status != SYS_NOMINAL )
   {
      printf( "CIL setup failed, error code %x\n", Status );
   }
   /* else CIL setup OK */
   else
   {
      printf( "CIL setup OK, using %s\n\n", eCilNameString( CilId ) );

#ifdef TX
      /* Setup the static CIL message parameters for transmission */
      CilMsg.SourceId = M_TDL_SOURCE_ID;
      CilMsg.DestId   = M_TDL_DESTN_ID;
      CilMsg.Class    = M_TDL_TEST_CLASS;
      CilMsg.Service  = M_TDL_TEST_SERVICE;
      CilMsg.SeqNum   = 0;
#endif

      /* Start forever loop */
      for ( ;; )
      {
#ifdef TX
         /* Increment the sequence number of the packet to transmit */
         CilMsg.SeqNum++;

         /* Take copy of the packet to tranmit */
         memcpy( &TestPacket, &ReferencePacket, sizeof( TestPacket ) );

         /* Assign pointer to, and size of, the test packet */
         CilMsg.DataPtr  = &TestPacket;
         CilMsg.DataLen  = sizeof( TestPacket );

         /* Write the time-stamp into the outgoing packet */
         Status = eTimGetTime( &CilMsg.TimeStamp );

         /* Convert the packet from host to network byte-order */
         Status = eCilConvert32bitArray( CilMsg.DataLen, CilMsg.DataPtr );

         /* Transmit a test packet */
         Status = eCilSend( M_TDL_DESTN_ID, &CilMsg );

         /* Check status of transmission */
         switch ( Status )
         {
            /* Successful transmission */
            case SYS_NOMINAL :
               
               /* Output details of successful transmission */
               Status = eTimToString( &CilMsg.TimeStamp, 
                                      sizeof( TimeBuffer ), 
                                      TimeBuffer );
               printf( "Packet %d of size %d transmitted to %s at %s\n", 
                       CilMsg.SeqNum, CilMsg.DataLen, 
                       eCilNameString( CilMsg.DestId ), TimeBuffer );
               break;

            /* Error generated */
            default :

               /* Output details of the error */
               printf( "CIL TX failed, error code %x\n", Status );
               break;
         }

         /* Sleep before sending the next packet */
         sleep( M_TDL_TX_DELAY_SEC );
#endif

#ifdef RX
         /* Setup the CIL message parameters for reception */
         CilMsg.DataPtr  = &TestPacket;
         CilMsg.DataLen  = sizeof( TestPacket );

         /* Attempt to receive an incoming packet */
         Status = eCilReceive( M_TDL_RX_DELAY_MSEC, &DelivererId, &CilMsg );

         /* Check status of attempted packet receive */
         switch ( Status )
         {
            /* Packet successfully received */
            case SYS_NOMINAL :

               /* Convert the packet from netwotk to host byte-order */
               Status = eCilConvert32bitArray( CilMsg.DataLen, CilMsg.DataPtr );

               /* Output details of the receive */
               Status = eTimToString( &CilMsg.TimeStamp, 
                                      sizeof( TimeBuffer ), 
                                      TimeBuffer );
               printf( "Packet %d of size %d received from %s at %s, ", 
                       CilMsg.SeqNum, CilMsg.DataLen, 
                       eCilNameString( DelivererId ), TimeBuffer );

               /* Verify the packet contents against the reference packet */
               if ( memcmp( CilMsg.DataPtr, ReferencePacket, 
                            CilMsg.DataLen ) == 0 )
               {
                  printf( "packet OK\n" );
               }
               /* else packet contents differ to reference packet */
               else
               {
                  printf( "packet error\n" );
               }
               break;

            /* Receive timed-out, no packet received in time allowed */
            case E_CIL_TIMEOUT :

               /* Nothing to do here */
               break;

            /* All other errors */
            default :

               printf( "CIL receive failed, error code %x\n", Status );
               break;
         }
#endif
      }
   }
   
   return 0;

}


/*
** EOF
*/

