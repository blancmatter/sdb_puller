
#include <errno.h>
#include <stdio.h>
#include "Wfl.h"

#define M_WFL_INTSIG SIGRTMIN

#define ATTACH_POINT "myname"

void handler( int signo )
{
   return;
}


/* We specify the header as being at least a pulse */

/* Our real data comes after the header */

/*** Server Side of the code ***/
int server() {

    eWflPulseId_t Pulse;
    eWflPulseId_t PulseRcv;
    struct sigevent Event;
    sigset_t        Set, OldSet;
    struct sigaction act;


    eWflMsg_t Msg;
    int      *DataPtr;
    int rcvid;
    Status_t Status;

    DataPtr = (int *) &( Msg.Msg );

    /*
    sigfillset( &Set );
    sigblock( Set );
    */
    sigemptyset( &Set );
    sigprocmask( SIG_BLOCK, &Set, &OldSet );
    sigunblock( Set );

    Event.sigev_signo = M_WFL_INTSIG;
    act.sa_flags = 0;
    act.sa_mask =  Set;
    act.sa_handler = &handler;

    sigaction( M_WFL_INTSIG, &act, NULL );


    Status = eWflCreatePulse( &Event, &Pulse );

    if ( Status == SYS_NOMINAL )
    {
      printf("Pulse created, PulseId = %d\n", Pulse );
    }
    else
    {
       exit( EXIT_FAILURE );
    }
    
    /* Do your MsgReceive's here now with the chid */
    while (1)
    {
        printf( "About to receive\n" );
        Status = eWflReceivePulse( &PulseRcv );
        printf( "Pulse id = %d\n", PulseRcv );

        printf( "Message received, Status = 0x%x\n", Status);

        if ( errno == EINTR )
        {
           printf( "Message was a signal\n" );

           Status = SYS_NOMINAL;
        }

        if ( Status != SYS_NOMINAL )
        {
           printf( "Status = 0x%x\n", Status );
           break;
        }
    }

    /* Remove the name from the space */

    return EXIT_SUCCESS;
}

/*** Client Side of the code ***/
int client( int pid ) {

    Status_t      Status;
    int           sig;
    int           Coid;
    struct _server_info Info;

    Coid = 0;

    Coid = ConnectServerInfo( pid, Coid, &Info );

    printf( "Coid = %d\n", Coid );

    printf( " nd = %d\npid = %d\n, chid = %d\n, scoid = %d\n",
              Info.nd,
              Info.pid,
              Info.chid,
              Info.scoid );

    while ( 1 )
    {
       delay(100);

       if (eWflKeyboardHit() )
       {
          if ( eWflGetKeyPress() == 'k' )
          {
             sig = SIGKILL;
          }
          else
          {
             sig = M_WFL_INTSIG;
             /* sig = SIGUSR1; */
          }

          printf( "Sending signal %d to process %d...\n", sig, pid );
       
          kill( pid, sig );
       }
    };

    if ( Status != SYS_NOMINAL )
    {
       printf("Error closing connection, Status = 0x%x, errno = %d\n",
              Status, errno );
    
       return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}

int main(int argc, char **argv)
{
    int ret;
    int pid;

    if (argc < 2)
    {
        printf("Usage %s -s | -c pid \n", argv[0]);
        ret = EXIT_FAILURE;
    }
    else if (strcmp(argv[1], "-c") == 0)
    {

        if ( sscanf( argv[2], "%d", &pid ) == 1 )
        {
           printf("Running Client with Server pid %d ... \n", pid );
           ret = client( pid );   /* see name_open() for this code */
        }
    }
    else if (strcmp(argv[1], "-s") == 0) {
        printf("Running Server ... \n");
        ret = server();   /* see name_attach() for this code */
    }
    else {
        printf("Usage %s -s | -c pid \n", argv[0]);
        ret = EXIT_FAILURE;
    }
    return ret;
}
