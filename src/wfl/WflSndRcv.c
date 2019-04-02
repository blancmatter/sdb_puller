
#include <errno.h>
#include <stdio.h>
#include "Wfl.h"

#define ATTACH_POINT "myname"

#define M_WFL_NO_BLOCK 1

Status_t eWflMsgReceivePid( void             *MsgPtr,
                            int               Size,
                            eWflPulseId_t    *RcvIdPtr,
                            int              *Pid );

/* We specify the header as being at least a pulse */

/* Our real data comes after the header */

/*** Server Side of the code ***/
int server() {
    eWflNameAttach_t Attach;
#if 1
    eWflTimedPulse_t TimedPulse;
    eWflTimedPulse_t TimedPulse2;

#endif
    eWflMsg_t Msg;
    int      *DataPtr;
    eWflPulseId_t RecvId;
    eWflPulseId_t ReplId;
    Status_t Status;

    DataPtr = (int *) &( Msg.Msg );

    /* Create a local name (/dev/name/local/...) */
    printf( "About to name attach\n" );

    Status = eWflNameAttach( ATTACH_POINT, &Attach );
    if ( Status != SYS_NOMINAL )
    {
       printf( "Unable to attach, Status = 0x%x\n", Status );
       exit ( EXIT_FAILURE );
    }

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
    printf( "Name attached, channel id = %d\n", Attach.chid );
#endif

#if 1
    TimedPulse.ValSecs = 1;
    TimedPulse.ValNSecs = 0;
    TimedPulse.IntSecs = 0;
    TimedPulse.IntNSecs = 500000000;
    Status = eWflCreateTimedPulse( &TimedPulse );
    if ( Status == SYS_NOMINAL )
    {
      printf("Timed pulse started, PulseId = %d\n", TimedPulse.PulseId );
    }
    else
    {
       exit( EXIT_FAILURE );
    }

    TimedPulse2.ValSecs = 1;
    TimedPulse2.ValNSecs = 0;
    TimedPulse2.IntSecs = 0;
    TimedPulse2.IntNSecs = 250000000;
    Status = eWflCreateTimedPulse( &TimedPulse2 );
    if ( Status == SYS_NOMINAL )
    {
      printf("Timed pulse started, PulseId = %d\n", TimedPulse2.PulseId );
    }
    else
    {
       exit( EXIT_FAILURE );
    }

#endif
    /* Do your MsgReceive's here now with the chid */
    while (1)
    {
        printf( "." );
        fflush( stdout );
        memset( &Msg, 0, sizeof(Msg) );
#if 1
        Status = eWflMsgReceive( &Msg, sizeof(Msg), &RecvId, &ReplId );
#endif

/*
#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
        printf( "Receive from %d, PulseId = %d, HdrValue = %d, Code = %d\n",
               RecvId, Msg.PulseId, Msg.Hdr.value.sival_int, Msg.Hdr.code );
        printf("Server receive %d \n", *DataPtr );
#else
        printf( "Receive from %d, PulseId = %d, Code = %d\n",
               RecvId, Msg.PulseId, Msg.Hdr.subtype );

        printf("Server receive %d \n", *DataPtr );
#endif
*/
        if ( Status != SYS_NOMINAL )
        {
           printf( "Status = 0x%x\n", Status );
           break;
        }

        /* A message (presumable ours) received, handle */
        if ( ( RecvId != TimedPulse.PulseId ) && 
             ( RecvId != TimedPulse2.PulseId ) )
        {
#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX
           printf( "Receive from %d, PulseId = %d, HdrValue = %d, Code = %d\n",
                  RecvId, Msg.PulseId, Msg.Hdr.value.sival_int, Msg.Hdr.subtype );
           printf("Server receive %d \n", *DataPtr );
#else
           printf( "Receive from %d, PulseId = %d, Code = %d\n",
                  RecvId, Msg.PulseId, Msg.Hdr.subtype );

           printf("Server receive %d \n", *DataPtr );
#endif
        }

    }

    /* Remove the name from the space */

    printf( "About to name detach\n");

#if 1
    eWflNameDetach( &Attach );
#endif

    printf( "Name detach ok\n" );
    return EXIT_SUCCESS;
}

/*** Client Side of the code ***/
int client( pid_t server_pid  ) {

    eWflMsg_t     Msg;
    int          *DataPtr;
    int           i;
    Status_t      Status;

    DataPtr = (int *)(&(Msg.Msg[0]));
/*

#if 1
    printf( "About to open name \"%s\" \n", ATTACH_POINT );

    Status = eWflNameOpen( ATTACH_POINT, &Channel );

    printf( "Channel id = %d\n", Channel );


#else
    Channel = ChannelCreate( 0 );
    
    printf( "Channel created = %d\n", Channel );

    Coid = ConnectAttach( 0, server_pid, Channel, _NTO_SIDE_CHANNEL, 0 );

    printf( "Connection id = %d\n", Coid );

    Status = SYS_NOMINAL;

#endif

    if (Status != SYS_NOMINAL)
    {
        printf( "Unable to eWflNameOpen, Status = 0x%x\n", Status );
        return EXIT_FAILURE;
    }

    printf( "Name opened\n" );
#if 0
    {
       eWflPulseId_t ChannelTmp;

       Status_t      StatusTmp;

       StatusTmp = eWflNameOpen( ATTACH_POINT, &ChannelTmp );

       if (StatusTmp != SYS_NOMINAL)
       {
          printf( "Unable to confirm eWflNameOpen, StatusTmp = 0x%x\n", StatusTmp );
       }
       else
       {
          printf( "Confirmed that named process %s exists\n", ATTACH_POINT );
       }
       eWflNameClose( ChannelTmp );

    }
#else
*/
    {
       int i;    

       printf("Waiting for 10 seconds...\n");
       for (i=0; i<100; i++)
       {
          printf(".");
          fflush( stdout );
          delay(100);
       }
       printf("Restarting...\n");
       delay(1000);

/*
       Status = eWflCheckPulse( Channel );

       if ( Status == SYS_NOMINAL )
       {
          printf( "Channel ok for writing to\n");
       }
       else
       {
          printf( "Channel not ok for writing to, errno = %d (%s)\n",
                   errno, strerror( errno ) );
       }
*/
    }
/*
#endif
*/
    /* Do whatever work you wanted with server connection */
    for (i = 0; i < 5; i++ )
    {
        memset( &Msg, 0, sizeof(Msg) );

        *DataPtr = i;
        printf("Client sending %d \n", *DataPtr );

/*
#if defined M_WFL_NO_BLOCK

        Status = eWflMsgSendNoBlock(Channel, &Msg, sizeof(Msg), NULL, 0);
   
#else
        Status = eWflMsgSend(server_pid, &Msg, sizeof(Msg), NULL, 0);
#endif
*/
        Status = eWflSndMsgToPid( server_pid, &Msg, sizeof(Msg) );

        printf( "Send message with coid %d to pid %d\n", Msg.PulseId, server_pid );
        if ( Status != SYS_NOMINAL )
        {
           printf( "Return from eWflMsgSend was 0x%x, errno %d (%s)\n",
                    Status, errno, strerror( errno ));
        }

        delay(250);

    }

    printf( "Closing connection\n" );

    /* Close the connection */
/*
    Status = eWflNameClose( Channel );

    if ( Status != SYS_NOMINAL )
    {
       printf("Error closing connection, Status = 0x%x, errno = %d\n",
              Status, errno );
    
       return EXIT_FAILURE;
    }
 */
    return EXIT_SUCCESS;

}

int main(int argc, char **argv) {
    int ret;
    pid_t server_pid;

    if (argc < 2) {
        printf("Usage %s -s | -c server_pid\n", argv[0]);
        ret = EXIT_FAILURE;
    }
    else if (strcmp(argv[1], "-c") == 0) {
        printf("Running Client ... \n");

        if ( argc > 2 )
        {
           sscanf( argv[2], "%d", &server_pid );
        }
        else
        {
           printf("Usage %s -c server_pid\n", argv[0]);
           ret = EXIT_FAILURE;
        }

        ret = client( server_pid );   /* see name_open() for this code */
    }
    else if (strcmp(argv[1], "-s") == 0) {
        printf("Running Server ... \n");

        ret = server( );   /* see name_attach() for this code */
    }
    else {
        printf("Usage %s -s | -c server_pid\n", argv[0]);
        ret = EXIT_FAILURE;
    }
    return ret;
}
