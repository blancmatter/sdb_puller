/*
** TESTSERV
**
** A simple program to test Cil functions. It establishes a number of
** associations with CIL map addresses, then receives messages. Any
** received message is returned to its sender. It keeps count of where
** messages come from.
**
** Authors:
**    plw: Paul L. Wright (TTL)
**    djm: Derek J. McKay (TTL)
**
** $Id: testserver.c,v 1.19 2006/07/10 15:13:51 mjf Exp $
**
** $Log: testserver.c,v $
** Revision 1.19  2006/07/10 15:13:51  mjf
** Code portability enhanced.
**
** Revision 1.18  2005/06/20 13:25:56  mjf
** Tidied up to aid porting to Linux - no functional changes.
**
** Revision 1.17  2004/06/03 12:28:02  mjf
** Version 1.00 - ported to QNX v6.
**
** Revision 1.16  2001/01/23 15:07:54  mjf
** Use CLU for command-line processing.
**
** Revision 1.15  2000/12/04 11:50:59  mjf
** TABs removed from the source code.
**
** Revision 1.14  2000/11/29 09:20:50  mjf
** Addition of command-line switch to specify timeout for receive messages.
**
** Revision 1.13  2000/11/29 08:47:21  mjf
** Overhaul including enhancing of command-line options.
**
** Revision 1.12  2000/10/06 15:54:45  djm
** Added -noecho command line switch to prevent received messages from
** being automatically returned to the sending client.
**
** Revision 1.11  2000/10/05 16:29:52  djm
** Changed location for the path to the CIL map.
**
** Revision 1.10  2000/07/14 16:35:45  djm
** Fixed a problem with pointers to static string constants being
** indexed and changed. THis was fine under QNX but caused a problem
** when ported to Linux/GNU-C.
**
** Revision 1.9  2000/07/13 16:34:29  djm
** Fixed error messages.
**
** Revision 1.8  2000/06/02 15:49:04  djm
** Added ability to control CIL ID etc. from the command line.
**
**
** Original: 23-Mar-2000
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "TtlSystem.h"
#include "Wfl.h"
#include "Cil.h"
#include "Clu.h"
#include "Log.h"


/* Application details - normally found in XxxPrivate.h */

#define I_XXX_PROGRAM_NAME   "testserver"
#define I_XXX_PROGRAM_ABOUT  "Test CIL message server"
#define I_XXX_RELEASE_DATE   "3 June 2004"
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
#define M_XXX_DFLT_LOG       "testserver.txt"
#define M_XXX_DFLT_CIL       "TES"

#ifdef E_WFL_OS_QNX4
#define M_XXX_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#define M_XXX_DFLT_CONFIG    "/opt/ttl/etc/testserver.cfg"
#else
#define M_XXX_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#define M_XXX_DFLT_CONFIG    "/ttl/sw/etc/testserver.cfg"
#endif

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define M_XXX_CUSTOM_NOECHO  0
#define M_XXX_CUSTOM_REFRESH 1
#define M_XXX_CUSTOM_TIMEOUT 2

#define M_XXX_CUSTOM_ARGS    3


#define MAXSZ 5
#define COL 13
#define DFLT_REFRESH_RATE 1
#define DFLT_TIMEOUT         1000


/* for parsing custom command-line arguments */

eCluArgSpec_t eCluCustomArg[ M_XXX_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                       FALSE, NULL */ 
{ { "noecho",          3, "Don't echo packets back to sender",FALSE, NULL },
  { "refresh [rate]",  3, "Display refresh rate (packets)",   FALSE, NULL },
  { "timeout [msec]",  3, "Message receive timeout (mSec)",   FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                          FALSE, NULL }
};


Bool_t mTesEcho = TRUE;
Int32_t mRefreshRate = DFLT_REFRESH_RATE;
Int32_t mTimeout = DFLT_TIMEOUT;


/* Function prototypes */

void CrsHome(void);
void CrsGoto(int x, int y);
void CrsClrToEol(void);
void CrsClrScreen(void);
Status_t mTesParseArgs(int argc, char *argv[]);
void mTesUsage(char *ExecNamePtr, char *MessagePtr);

int main(int argc, char *argv[])
{
    Status_t MyStatus;
    Status_t Status;
    Int32_t Id;
    Int32_t RxId;
    eCilMsg_t Msg;
    char Data[1024];
    int Count1 = 0;
    int Count2 = 0;
    int CountTcs = 0;
    int Count3 = 0;
    Int32_t TotalCount = 0;
    Int32_t SecondTally = 0;
    Int32_t LastSecondTally = 0;
    Int32_t SwapVar;
    int n;
    int NumTimeouts = 0;
    int Count[E_CIL_EOL];
    time_t CurrentTime = (time_t) 0;
    time_t PrevTime = (time_t) 0;


    /* Parse the command line arguments */
    Status = mTesParseArgs(argc, argv);
    if(Status != SYS_NOMINAL)
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
    Status = eCilLookup( eCluCommon.CilMap, eCluCommon.CilName, &Id );

    /* attempt to perform CIL setup now CIL ID has been determined */
    Status = eCilSetup( eCluCommon.CilMap, Id );

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

    /* Reset counts */
    for(Id = E_CIL_BOL + 1; Id < E_CIL_EOL; Id++)
    {
       Count[Id] = 0;
    }


    /*
    ** SET-UP SCREEN LAYOUT
    */

    CrsClrScreen();
    CrsGoto(1,1);
    printf("____CIL_TEST_SERVER (%s)____\n", eCluCommon.CilName);
    printf(" Timeouts:\n");
    printf(" Last sec:\n");
    printf("Rx'd from:\n");
    printf("       ID:\n");
    printf(" Num Rx'd:\n");
    printf(" SourceId:\n");
    printf("   DestId:\n");
    printf("    Class:\n");
    printf("  Service:\n");
    printf("   SeqNum:\n");
    printf("   TS sec:\n");
    printf("  TS nsec:\n");
    printf("  DataLen:\n");
    printf("     Data:\n");
/*
    for(Id = E_CIL_BOL+1; Id < E_CIL_EOL; Id++)
    {
       CrsGoto(1, Id);
       printf("%2d %s", Id, Name[Id]);
       CrsGoto(8, Id);
       printf("%6d", Count[Id]);
    }
*/

    /*
    ** GO INTO THE MAIN LOOP
    */

    for(;;)
    {

#ifdef DEBUG
#define uout(x)  printf(#x " = %u\n", x);
#define xout(x)  printf(#x " = 0x%X\n", x);
#else
#define uout(x)  
#define xout(x)
#endif



        Msg.DataPtr = Data;
        Msg.DataLen = sizeof(Data);

uout(&Data[0]);
uout((unsigned long)(Msg.DataPtr));
uout(&(Msg));
uout(&(Msg.SourceId));
uout(&(Msg.DestId));
uout(&(Msg.DataPtr));
uout(&(Msg.DataLen));

        MyStatus = eCilReceive(mTimeout, &RxId, &Msg);


        if( MyStatus == SYS_NOMINAL)
        {
            /* Increment some counters based on where the message source */
            switch(Msg.SourceId)
            {
                case E_CIL_TU1 :   Count1++;   break;
                case E_CIL_TU2 :   Count2++;   break;
                case E_CIL_TU3 :   CountTcs++; break;
                default        :   Count3++;   break;
            }

            Count[RxId]++;
            TotalCount++;
            SecondTally++;

            /* Determine whether to update number of packets in last second */
            CurrentTime = time( NULL );
            if ( PrevTime != CurrentTime )
            {
               PrevTime = CurrentTime;
               LastSecondTally = SecondTally;
               SecondTally = 0;
            }

            /* If whole number of 'refresh rate' received, output to  to screen */
            if ( ( TotalCount % mRefreshRate ) == 0 )
            {
               /* Print data to screen */
               CrsGoto(COL,  2);  printf("(%d)", NumTimeouts);
               CrsGoto(COL,  3);  printf("%5d", LastSecondTally);
               CrsGoto(COL,  4);  printf("%s", eCilNameString(RxId));
               CrsGoto(COL,  5);  printf("%d ", RxId);
               CrsGoto(COL,  6);  printf("%d", Count[RxId]);
               CrsGoto(COL,  7);  printf("0x%8.8x = %s", Msg.SourceId, 
                                                  eCilNameString(Msg.SourceId));
               CrsGoto(COL,  8);  printf("0x%8.8x = %s", Msg.DestId, 
                                                  eCilNameString(Msg.DestId));
               CrsGoto(COL,  9);  printf("0x%8.8x", Msg.Class);
               CrsGoto(COL, 10);  printf("0x%8.8x", Msg.Service);
               CrsGoto(COL, 11);  printf("0x%8.8x", Msg.SeqNum);
               CrsGoto(COL, 12);  printf("0x%8.8x", Msg.TimeStamp.t_sec);
               CrsGoto(COL, 13);  printf("0x%8.8x", Msg.TimeStamp.t_nsec);
               CrsGoto(COL, 14);  printf("0x%8.8x = %d", Msg.DataLen, Msg.DataLen);
               CrsClrToEol();
               CrsGoto(COL, 15);
               memcpy(Data, Msg.DataPtr, Msg.DataLen);
               for(n = 0; n < Msg.DataLen; n++)
               {
                  if(((n % 16) == 0) && (n > 0))
                  {
                     printf("\n            ");
                  }
                  printf("%2.2x ", Data[n]);
               }
               printf("End");
               CrsClrToEol();
            }
/*
            printf("\n            --- End of data ---");
            CrsClrToEol();
            printf("\n");
            CrsClrToEol();
*/
/*

            printf("%6d ", Count[RxId]);
            printf("%8.8x %8.8x %8.8x %8.8x ",
               Msg.SourceId, Msg.DestId, Msg.Class, Msg.Service);
            printf("%8.8x %8.8x %8.8x", Msg.SeqNum, Msg.TimeStamp.t_sec,
                                                    Msg.TimeStamp.t_nsec);
            printf("%c[K", 27);
            CrsGoto(8, E_CIL_EOL);
*/

            /* Print these to screen, to see what is going on */
/*
			printf("\r %d : %d : %d : %d", Count1, Count2, CountTcs, Count3);
*/
            /* Swap source and destination addresses */
            SwapVar = Msg.SourceId;
            Msg.SourceId = Msg.DestId;
            Msg.DestId = SwapVar;

            /* Send message back again if we are echoing */
            if( mTesEcho == TRUE )
            {
               MyStatus = eCilSend(RxId, &Msg);
               if( MyStatus != SYS_NOMINAL)
               {
                  printf("Send failure - 0x%d\n", MyStatus);
                  return(3);
               }
            }
        }
        else if( MyStatus == E_CIL_TIMEOUT)
        {
            NumTimeouts++;
            CrsGoto(COL,  2);
            printf("%d", NumTimeouts);
            CrsClrToEol();
            CrsGoto(COL, 15);
        }
        else
        {
            printf("Receive failure - 0x%x\n", MyStatus);
            return(4);
        }
    }

/*
	return(0);
*/
}






void CrsHome(void)
{
   static char EscSequence[] = " [?6h";

   EscSequence[0] = 27;
   puts(EscSequence);
}

void CrsGoto(int x, int y)
{
   printf("%c[%d;%df", 27, y, x);
}

void CrsClrToEol(void)
{
   static char EscSequence[] = " [K";

   EscSequence[0] = 27;
   puts(EscSequence);
}

void CrsClrScreen(void)
{
   static char EscSequence[] = " [2J";

   EscSequence[0] = 27;
   puts(EscSequence);
}



Status_t mTesParseArgs(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    mTesParseArgs
**
** Type:
**    Status_t
**
** Purpose:
**    Command line argument processing function for the TES.
**
** Description:
**    Takes the command line arguments (as specified as arguments
**    to the  main() function) and processes them to fill in the
**    global structure.
**
**    NOTE: If the command line argument handling is change, then
**    one must remember to update the mTesUsage() function as well.
**
**    Also note that non-TTL coding standard conventions were used
**    for the arguments argc & argv, as they are "standard" ISO/C
**    usage in this form.
**
** Arguments:
**    int argc                 (in)
**       Number of arguments on the command line (including the
**       executable name). As in main().
**    char *argv[]             (in)
**       Array of null-terminated character strings containing
**       the command line arguments. As in main().
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    31-May-2000 djm Initial creation (adapted from mSdbParseArgs()).
**
*/

   char     Switch[ E_CLU_SWITCH_LEN ];/* to report error parsing a switch */
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

         /* check for 'noecho' */
         if ( eCluCustomArgExists( M_XXX_CUSTOM_NOECHO ) == E_CLU_ARG_SUPPLIED )
         {
            /* This option disables packet echoing */
            mTesEcho = FALSE;
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
