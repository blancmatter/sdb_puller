/*
** TESTCLIENT
**
** This program attempts to send messages to a "server" program
**
*/

/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "TtlSystem.h"
#include "TtlConstants.h"   /* Include the TTL constant definitions. */
#include "Wfl.h"
#include "Cil.h"
#include "Clu.h"
#include "Log.h"
#include "Tim.h"


/* Turn on timing statistics. */
/*#define I_XXX_TIMING_TESTS */

/* Application details - normally found in XxxPrivate.h */

#define I_XXX_PROGRAM_NAME   "testclient"
#define I_XXX_PROGRAM_ABOUT  "Test CIL message client"
#define I_XXX_RELEASE_DATE   "17 May 2004"
#define I_XXX_YEAR           "2000-04"
#define I_XXX_MAJOR_VERSION  1
#define I_XXX_MINOR_VERSION  0

/* Common arguments defaults */

#define M_XXX_DFLT_QUIET     FALSE
#define M_XXX_DFLT_VERBOSE   TRUE
#define M_XXX_DFLT_SYSLOG    TRUE
#define M_XXX_DFLT_DEBUG     E_LOG_NOTICE
#define M_XXX_DFLT_PRIORITY  10
#define M_XXX_DFLT_HELP      FALSE
#define M_XXX_DFLT_LOG       "testclient.txt"
#define M_XXX_DFLT_CIL       "TU0"

#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#define M_XXX_DFLT_CONFIG    "/opt/ttl/etc/testclient.cfg"
#else
#define M_XXX_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#define M_XXX_DFLT_CONFIG    "/ttl/sw/etc/testclient.cfg"
#endif

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_NORX    0
#define M_XXX_CUSTOM_REFRESH 1
#define M_XXX_CUSTOM_TIMEOUT 2
#define M_XXX_CUSTOM_DEST    3
#define M_XXX_CUSTOM_BROKER  4

#define M_XXX_CUSTOM_ARGS    5

/* Symbolic constants */

#define DFLT_REFRESH_RATE    1
#define DFLT_TIMEOUT         1000


/* for parsing custom command-line arguments */

eCluArgSpec_t eCluCustomArg[ M_XXX_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                       FALSE, NULL */ 
{ { "norx",            3, "Don't expect reply from server",   FALSE, NULL },
  { "refresh [rate]",  3, "Display refresh rate (packets)",   FALSE, NULL },
  { "timeout [msec]",  3, "Message receive timeout (mSec)",   FALSE, NULL },
  { "dest <name>",     3, "CIL name of destination process",  FALSE, NULL },
  { "broker <name>",   3, "CIL name of broker process",       FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                          FALSE, NULL }
};


/* Global variables */

Int32_t mCilId = E_CIL_TU0;
Int32_t mDestId = E_CIL_TU1;
Int32_t mBrokerId = E_CIL_TU1;
Int32_t mRefreshRate = DFLT_REFRESH_RATE;
Int32_t mTimeout = DFLT_TIMEOUT;
Bool_t  mExpectReply = TRUE;

/* Function prototypes */

Status_t mParseArgs(int argc, char *argv[]);


/*
** MAIN FUNCTION
*/

int main(int argc, char *argv[])
{
    Status_t MyStatus;      /* Function return status */
    Int32_t RxId;           /* CIL ID of sender of rx'd messages */
    char TxData[32];        /* Buffer for data to send */
    char RxData[32];        /* Data for received data */
    int Count = 0;          /* Counter, for stats reporting */
    int NumTimeouts = 0;    /* Number of timeouts on message waits */
    int NumTxFail = 0;      /* Number of transmission failures */
    int NumSuccess = 0;     /* Number of successful transmissions */
    int NumErrors = 0;      /* Number of packet contents errors */
    eCilMsg_t TxMsg;        /* CIL message structure to send */
    eCilMsg_t RxMsg;        /* CIL message structure to receive */
    Status_t Status;        /* Function return status */
    eTtlTime_t CurrentTime; 
    char TimeString[ E_TIM_BUFFER_LENGTH ];
#ifdef I_XXX_TIMING_TESTS
uint64_t  send_start_time, send_end_time;
uint64_t  rec_start_time, rec_end_time;
#endif

    /* Parse the command line arguments */
    MyStatus = mParseArgs(argc, argv);
    if(MyStatus != SYS_NOMINAL)
    {
       exit(EXIT_FAILURE);
    }

    Status = eCluSignOn( NULL, NULL );
    if(Status != SYS_NOMINAL)
    {
       printf
       (
          "Error: Failed to sign-on (%d = 0x%x)\n", 
          Status, Status
       );
       exit(EXIT_FAILURE);
    }

    /* attempt to determine the CIL ID */
    Status = eCilLookup( eCluCommon.CilMap, eCluCommon.CilName, &mCilId );

    /* attempt to perform CIL setup now CIL ID has been determined */
    Status = eCilSetup( eCluCommon.CilMap, mCilId );

    /* check status returned - report any errors */
    switch ( Status )
    {
       case SYS_NOMINAL :
          /* not an error - successful completion */
          break;

       default :
          printf( "Unable to perform CIL setup : error %X\n", Status );
          exit(EXIT_FAILURE);
          break;
    }

    /* Print who we are */
    printf("  --  Name=%s, ID=%d\n", eCluCommon.CilName, mCilId);

    /* Fill the data buffer with some characters */
    memset(TxData,0xaa, sizeof(TxData));


    /* Start the main loop, repeatedly transmitting the messages */
    for(;;)
    {

        if((Count % mRefreshRate) == 0)
        {
            eTimGetTime( &CurrentTime );
            eTimToString( &CurrentTime, E_TIM_BUFFER_LENGTH, TimeString );
#ifndef I_XXX_TIMING_TESTS
            printf("\r%s OK: total=%d  ", TimeString, NumSuccess);
            printf("ERR: send=%d timeouts=%d contents=%d",
                NumTxFail, NumTimeouts, NumErrors);
            fflush(stdout);
#endif
            Count = 0;
        }
        Count++;


        /* Setup CIL message structure */
        TxMsg.SourceId = mCilId;
        TxMsg.DestId = mDestId;
        TxMsg.Class = E_CIL_CMD_CLASS;
        TxMsg.Service = 0;
        TxMsg.SeqNum++;
        TxMsg.DataPtr = TxData;
        TxMsg.DataLen = sizeof(TxData);

        /* Actually send the message */
#ifdef I_XXX_TIMING_TESTS
        send_start_time = ClockCycles(); 
#endif
        MyStatus = eCilSend(mBrokerId, &TxMsg);
#ifdef I_XXX_TIMING_TESTS
        send_end_time  = ClockCycles();
#endif
        if( MyStatus != SYS_NOMINAL)
        {
            eTimGetTime( &CurrentTime );
            eTimToString( &CurrentTime, E_TIM_BUFFER_LENGTH, TimeString );
            printf("Send failure - %X (%s)\n", MyStatus, TimeString);
            NumTxFail++;
            continue;
        }


        /* If a reply is exptected from the server */
        if ( mExpectReply == TRUE )
        {
            /* Prepare to receive some data */
            RxMsg.DataPtr = RxData;
            RxMsg.DataLen = sizeof(RxData);

            /* Actually receive the data */
#ifdef I_XXX_TIMING_TESTS
            rec_start_time = ClockCycles(); 
#endif
            MyStatus = eCilReceive(mTimeout, &RxId, &RxMsg);
#ifdef I_XXX_TIMING_TESTS
            rec_end_time  = ClockCycles();
            printf( "Send %9d clock-cycles. Receive %9d clock-cycles.\n",
              (int) (send_end_time - send_start_time), 
              (int) (rec_end_time - rec_start_time)  );
            fflush(stdout);
#endif
            if( MyStatus != SYS_NOMINAL)
            {
                eTimGetTime( &CurrentTime );
                eTimToString( &CurrentTime, E_TIM_BUFFER_LENGTH, TimeString );
                printf("Receive failure - %X (%s)\n", MyStatus, TimeString);
                NumTimeouts++;
                continue;
            }

            /* Verify data integrity */
            if(memcmp(TxData, RxData, sizeof(TxData)) != 0)
            {
                eTimGetTime( &CurrentTime );
                eTimToString( &CurrentTime, E_TIM_BUFFER_LENGTH, TimeString );
                printf("Rx Data <> Tx Data (%s)\n", TimeString );
                NumErrors++;
                continue;
            }
        }

        NumSuccess++;

    }  /* End of for(;;) */

}  /* End of main() */



Status_t mParseArgs(
   int argc,
   char *argv[]
)
{

   char     Switch[ E_CLU_SWITCH_LEN ];/* to report error parsing a switch */
   Int32_t  CilUnit;         /* Some other specified CIL unit */
   Status_t Status;          /* Return variable for function calls */
   Status_t CluStatus;       /* Return variable for CLU function calls */



   /* data for command-line utilities */
   eCluProgNamePtr              = I_XXX_PROGRAM_NAME;
   eCluProgAboutPtr             = I_XXX_PROGRAM_ABOUT;
   eCluReleaseDatePtr           = I_XXX_RELEASE_DATE;
   eCluYearPtr                  = I_XXX_YEAR;
   eCluMajorVer                 = I_XXX_MAJOR_VERSION;
   eCluMinorVer                 = I_XXX_MINOR_VERSION;
  
   /* set default parameters before parsing command-line */
   eCluCommon.Quiet             = M_XXX_DFLT_QUIET;
   eCluCommon.Verbose           = M_XXX_DFLT_VERBOSE;
   eCluCommon.Syslog            = M_XXX_DFLT_SYSLOG;
   eCluCommon.DebugLevel        = M_XXX_DFLT_DEBUG;
   eCluCommon.Priority          = M_XXX_DFLT_PRIORITY;
   eCluCommon.Help              = M_XXX_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, M_XXX_DFLT_CONFIG  );
   strcpy( eCluCommon.LogFile,    M_XXX_DFLT_LOG     );
   strcpy( eCluCommon.CilName,    M_XXX_DFLT_CIL     );
   strcpy( eCluCommon.CilMap,     M_XXX_DFLT_CIL_MAP );

   /* register with the command-line utilities (CLU), ignore CIL map */
   CluStatus = eCluSetup( argc, argv, 
                          E_CLU_IGN_QUIET | E_CLU_IGN_VERBOSE 
                           | E_CLU_IGN_SYSLOG | E_CLU_IGN_NOSYSLOG
                           | E_CLU_IGN_DEBUG | E_CLU_IGN_PRIORITY
                           | E_CLU_IGN_CONFIGFILE | E_CLU_IGN_LOGFILE );
   
   /* if setup was successful */
   if ( CluStatus == SYS_NOMINAL )
   {
      /* parse the incoming command-line */
      CluStatus = eCluParseArgs( Switch );

      /* if command-line parsed successfully */
      if ( CluStatus == SYS_NOMINAL )
      {  
         /* check for help requested */
         if ( eCluCommon.Help == TRUE )
         {
            /* exit the program */
            exit( EXIT_SUCCESS );
         }

         /* check for 'norx' */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_NORX ) == E_CLU_ARG_SUPPLIED )
         {
            /* This option doesn't expect a reply from the server */
            mExpectReply = FALSE;

            printf("Not expecting replies from the server\n" );
         }

         /* check for 'refresh [rate]' */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_REFRESH ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then convert and store refresh rate */
            mRefreshRate = (Int32_t) strtol( 
                                     eCluGetCustomParam( M_XXX_CUSTOM_REFRESH ),
                                     NULL, 0 );
            /* Check for positive non-zero refresh rate */
            if ( mRefreshRate <= 0 )
            {
               mRefreshRate = DFLT_REFRESH_RATE;
            }
            printf( "Refresh rate every %d packets\n", mRefreshRate );
         }

         /* check for 'timeout [msec]' */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_TIMEOUT ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then convert and store refresh rate */
            mTimeout = (Int32_t) strtol( 
                                     eCluGetCustomParam( M_XXX_CUSTOM_TIMEOUT ),
                                     NULL, 0 );
            /* Check for positive non-zero refresh rate */
            if ( mTimeout < 0 )
            {
               mTimeout = DFLT_TIMEOUT;
            }
            printf( "Timeout for message receive %d mSec\n", mTimeout );
         }

         /* check for 'dest <name>' */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_DEST ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to look it up */
            Status = eCilLookup( eCluCommon.CilMap, 
                                 eCluGetCustomParam( M_XXX_CUSTOM_DEST ), 
                                 &CilUnit);
            if(Status != SYS_NOMINAL) {
               printf("CIL name \"%s\" not recognised\n", 
                      eCluGetCustomParam( M_XXX_CUSTOM_DEST ));
               eCluShowUsage( NULL, "Unknown CIL name" );
               return SYS_ERROR_FATAL;
            }
            /* If we get this far, alls well. We'll try to use this CIL ID */
            mDestId = CilUnit;
            printf("Using destination CIL ID %d (= 0x%2.2X, %s)\n",
                   CilUnit, CilUnit, eCluGetCustomParam( M_XXX_CUSTOM_DEST ));
         }

         /* check for 'broker <name>' */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_BROKER ) == E_CLU_ARG_SUPPLIED )
         {
            /* Then try to look it up */
            Status = eCilLookup( eCluCommon.CilMap, 
                                 eCluGetCustomParam( M_XXX_CUSTOM_BROKER ), 
                                 &CilUnit);
            if(Status != SYS_NOMINAL) {
               printf("CIL name \"%s\" not recognised\n", 
               eCluGetCustomParam( M_XXX_CUSTOM_BROKER ));
               eCluShowUsage( NULL, "Unknown CIL name" );
               return SYS_ERROR_FATAL;
            }
            /* If we get this far, alls well. We'll try to use this CIL ID */
            mBrokerId = CilUnit;
            printf("Using broker CIL ID %d (= 0x%2.2X, %s)\n",
                   CilUnit, CilUnit, eCluGetCustomParam( M_XXX_CUSTOM_BROKER ));
         }
      }
      /* else command-line not parsed successfully */
      else
      {
         eCluShowUsage( NULL, NULL );
         printf( "Error parsing the command-line switch '%s'\n", Switch );
         return CluStatus;
      }
   }
   /* else command-line not parsed successfully */
   else
   {
      printf( "Error parsing the command-line\n" );
      return CluStatus;
   }

   /* Terminate the function and return success */
   return SYS_NOMINAL;

}  /* End of mTesParseArgs() */


/* EOF */
