/*
** Module Name:
**    testfilereq.c
**
** Purpose:
**    Test program for the file-request commands in the SDB ICD.
**
** Description:
**    This program tests the RETRIEVE_F command for the SDB.
**
**    This program uses the package ID "STM" - SDB Test Misc.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testfilereq.c,v 0.11 2004/01/19 11:06:06 sxf Exp $
**
** History:
**    $Log: testfilereq.c,v $
**    Revision 0.11  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.10  2004/01/16 08:55:04  sxf
**    Minor changes to port to QNX6.
**
**    Revision 0.9  2000/10/04 11:11:38  djm
**    Improved diagnostic output.
**
**    Revision 0.8  2000/09/25 15:38:10  asp
**    I'm not talking to Derek.
**
**    Revision 0.7  2000/09/25 14:13:08  djm
**    Minor changes.
**
**    Revision 0.6  2000/09/08 14:29:49  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.5  2000/08/29 14:37:21  djm
**    Modified code to accomodate recent changes to the SDB.
**    Also made slight improvement to the receiving of CIL messages.
**
**    Revision 0.4  2000/08/24 11:50:55  djm
**    Changed the CIL map reference to the new standard location.
**
**    Revision 0.3  2000/08/22 17:55:07  djm
**    Improved the file request times for testing the SFR.
**
**    Revision 0.2  2000/08/22 09:17:13  djm
**    Reduced the number of measurements requested. Also added timestamping.
**
**    Revision 0.1  2000/08/07 15:35:46  djm
**    Initial creation.
**
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include "TtlSystem.h"
#include "Wfl.h"
#include "Sdb.h"
#include "SdbPrivate.h"
#include "Cil.h"
#include "Log.h"
#include "Tim.h"


/* Definitions */

#ifdef E_WFL_OS_QNX4
#define M_STM_CIL_MAPNAME "/opt/ttl/etc/Cil.map"
#else
#define M_STM_CIL_MAPNAME "/ttl/sw/etc/Cil.map"
#endif

#define M_STM_DATASIZE     16384      /* Max size of accepted CIL messages */
#define M_STM_TIMEOUT      300        /* CIL Rx timeout (in milliseconds) */
/* NB: the timeout needs to be faster than the CHB heartbeat period */
#define M_STM_SOURCE_ID    E_CIL_SDB  /* Source of datum to fetch */
#define M_STM_DATUM_ID     0x04       /* Datum ID of datum to fetch */
#define M_STM_CIL_ID       E_CIL_TU0  /* Default CIL ID */
#define M_STM_SDB_CIL_ID   E_CIL_SDB  /* ID of database server */
#define M_STM_MCB_CIL_ID   E_CIL_MCB  /* ID target to send messages to */
#define M_STM_NAMELEN      8          /* Max length for a CIL name */
#define M_STM_MAXMSRMENTS  32       /* Maximum number of msrments to receive */
#define M_STM_NUMTIMEOUTS  10   /* Maximum number of timeouts to tolerate */


/* Global data */

typedef struct
{
   Int32_t CilId;            /* CIL address ID */
} mStmCmdLineArgs_t;

mStmCmdLineArgs_t mStmCmdLineArgs
   = {
      M_STM_CIL_ID,          /* Default CIL ID */
   };

/* Other global variables */
Uint32_t mStmSeqNum = 0;     /* CIL message sequence number */
char mStmCilName[E_CIL_EOL][M_STM_NAMELEN];   /* Storage for CIL names */




/* Function prototypes */

Status_t mStmParseArgs(int argc, char *argv[]);
void mStmUsage(char *ExecNamePtr, char *MessagePtr);
Status_t mStmCountData(Int32_t SourceId);
Status_t mStmCountMsrments(Int32_t SourceId, Int32_t DatumId);






int main(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    Top level function of the "testfilereq" program.
**
** Description:
**    Top level function of the "SDB filereq tester". It is called on
**    startup of the executable.
**
** Arguments:
**    int argc                 (in)
**       Number of arguments on the command line (including the
**       executable name).
**    char *argv[]             (in)
**       Array of null-terminated character strings containing
**       the command line arguments.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    23-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Function return status variable */
   Int32_t SourceId;         /* CIL ID of a source */
   char Data[M_STM_DATASIZE];/* Buffer to hold data */
   char *DataPtr;            /* Pointer to Data buffer */
   eCilMsg_t Msg;            /* CIL message for transmition/receiption */
   eSdbMulReq_t MulReq;      /* Multiple-request structure */
   Int32_t DelivererId;      /* CIL ID of process who delivered the msg to us */
   eTtlTime_t DeltaTime;     /* Time difference (for generating ranges) */
   eTtlTime_t TempTime;      /* Temporary holder for system time */
   time_t Time;              /* Dummy variable for the time() function call */
   int NumTimeouts = 0;      /* Number of permitted timeouts */



   /* Print startup diagnostic */
   printf("SDB TESTFILEREQ PROGRAM\n");

   /* Randomise the random numbers */
   srand((unsigned int) time(&Time));

   /* Parse command line arguments (CLAs) */
   Status = mStmParseArgs(argc, argv);
   if(Status != SYS_NOMINAL)
   {
      printf("Failure parsing command line arguments\n");
      return EXIT_FAILURE;
   }



   /* Do the CIL setup */
   Status = eCilSetup(M_STM_CIL_MAPNAME, mStmCmdLineArgs.CilId);
   if(Status != SYS_NOMINAL)
   {
      printf
      (
         "Error: Failed to allocate CIL address (%d = 0x%x)\n",
         mStmCmdLineArgs.CilId, mStmCmdLineArgs.CilId
      );
      return EXIT_FAILURE;
   }


   /* Fill in array of CIL names for rapid access */
   strcpy(mStmCilName[0], "???");
   for(SourceId = E_CIL_BOL+1; SourceId < E_CIL_EOL; SourceId++)
   {
      Status = eCilName(M_STM_CIL_MAPNAME, SourceId, 
                        M_STM_NAMELEN, mStmCilName[SourceId]);

      if(Status != SYS_NOMINAL)
      {
         printf(
            "Error (0x%x) getting name for SourceId=0x%x\n",
            Status, SourceId
         );
         exit(EXIT_FAILURE);
      }
   }


   /*
   ** Okay... test time
   */

   /* Send a request to the SDB */
   Msg.SourceId = mStmCmdLineArgs.CilId;
   Msg.DestId = M_STM_SDB_CIL_ID;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.Service = E_SDB_RETRIEVE_F;
/*
   Msg.SeqNum = ++mStmSeqNum;
*/
   mStmSeqNum = rand();
   Msg.SeqNum = mStmSeqNum;
   Status = eTimGetTime(&Msg.TimeStamp);
   Msg.DataPtr = DataPtr = &Data[0];

   /* Apply first request  (all times) */
   MulReq.SourceId = M_STM_SOURCE_ID;
   MulReq.DatumId = M_STM_DATUM_ID;
   MulReq.NumMsrments = M_STM_MAXMSRMENTS;

   Status = eTimGetTime(&TempTime);
   DeltaTime.t_sec = 0 * -3600;        /* ARBITRARY DELTA TIME */
   DeltaTime.t_nsec = 0;
   Status = eTimSum(&TempTime, &DeltaTime, &MulReq.NewestTime);   



   Status = eTimGetTime(&TempTime);
   DeltaTime.t_sec = 1 * -3600;        /* ARBITRARY DELTA TIME */
   DeltaTime.t_nsec = 0;
   Status = eTimSum(&TempTime, &DeltaTime, &MulReq.OldestTime);   

   memcpy(DataPtr, &MulReq, sizeof(MulReq));
   Msg.DataLen = sizeof(MulReq);

   /* Print the "send" message */
   {
      int n;

      printf("-----------------------------------\n");
      printf("Contents of the transmitted message:\n");
      printf("DataLen = %d\n", Msg.DataLen);
      printf("Source = 0x%x Dest = 0x%x\n", Msg.SourceId, Msg.DestId);
      printf("Service = %d\n",  Msg.Service);
      printf("Message contents:");
      for(n = 0; n < Msg.DataLen; n++)
      {
         if((n%4) == 0) printf("\n ");
         printf("%2.2x", ((char *)Msg.DataPtr)[n]);
      }
      printf("\n");
      printf("-----------------------------------\n");
   }


   /* Convert the byte order */
   eCilConvert32bitArray(Msg.DataLen, Msg.DataPtr);


   /* Send the message */
   Status = eCilSend(M_STM_MCB_CIL_ID, &Msg);
   if(Status != SYS_NOMINAL)
   {
      printf("Unable to send source list request (0x%x)\n", Status);
      return Status;
   }

   /* Block, waiting for an inbound message */
   while(
      ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStmSeqNum)) &&
      (NumTimeouts < M_STM_NUMTIMEOUTS)
   )
   {
      Msg.DataLen = M_STM_DATASIZE;
      Status = eCilReceive(M_STM_TIMEOUT, &DelivererId, &Msg);
      if(Status != E_CIL_TIMEOUT)
      {
         printf(
            "Msg rx'd (Src=0x%x Service = 0x%x, Class = %d, "
            "SeqNum = %d, Lngth = %d)\n",
            Msg.SourceId,
            Msg.Service, Msg.Class, Msg.SeqNum, Msg.DataLen
         );

         if ( Msg.Service == 0xd000b )
         {
           printf( "Count=%i\n", (int *)&Data[ 3 * 4 ] );
              
         }
      } 
      else
      {
         printf("Timeout\n");
         NumTimeouts++;
      }
   }


   /* Check the CIL receive for a timeout result */
   if(Status == E_CIL_TIMEOUT)
   {
      printf("Timeout waiting for list of source IDs\n");
      return Status;
   }

   /* Check successful CIL reply for other errors */
   if(Status != SYS_NOMINAL)
   {
      printf("CIL receive error for source ID list (0x%x)\n");
      return Status;
   }


   /* Convert back the byte order */
   eCilConvert32bitArray(Msg.DataLen, Msg.DataPtr);


   /* Print the output */
   {
      int n;
      printf("-----------------------------------\n");
      printf("Contents of the received message:\n");
      printf("DataLen = %d\n", Msg.DataLen);
      printf("Message contents:\n");
      for(n = 0; n < Msg.DataLen; n++)
      {
         if((n%12) == 4) printf("\n");
         if((n%4) == 0) printf(" ");

         printf("%2.2x", ((char *)Msg.DataPtr)[n]);
      }
      printf("\n");
      printf("-----------------------------------\n");
   }


   /* Terminate program */
   printf("Complete\n");
   return EXIT_SUCCESS;

}  /* End of main() */



Status_t mStmParseArgs(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    mStmParseArgs
**
** Type:
**    Status_t
**
** Purpose:
**    Command line argument processing function for the STM program.
**
** Description:
**    Takes the command line arguments (as specified as arguments
**    to the  main() function) and processes them to fill in the
**    global structure.
**
**    NOTE: If the command line argument handling is change, then
**    one must remember to update the mStmUsage() function as well.
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
**    21-Jun-2000 djm Initial creation (adapted from mSdbParseArgs()).
**
*/

   /* No local variables */
   int ArgNum;               /* Loop counter for going through arguments */
   int NumValues;            /* Number of values read from sscanf() */
   int CilTestUnitOffset;    /*  */
   Int32_t CilUnit;          /* Some other specified CIL unit */
   Status_t Status;          /* Return variable for function calls */

   /* Check arguments */
   for(ArgNum = 1; ArgNum < argc; ArgNum++)
   {

      if(
         strcmp(argv[ArgNum], "-help") == 0 ||
         strcmp(argv[ArgNum], "-h") == 0 ||
         strcmp(argv[ArgNum], "-?") == 0
      )
      {
         /* Just print usage and exit */
         mStmUsage(argv[0], NULL);
         exit( EXIT_SUCCESS );
      }
      else if(strncmp(argv[ArgNum], "-cil", 4) == 0)
      {
         /*
         ** This option, allows the use of other CIL IDs instead
         ** of the default SDB ID (defined in SdbPrivate.h)
         */

         /* First check to make sure a CIL name was provided */
         if((++ArgNum) >= argc)
         {
            mStmUsage(argv[0], "No CIL name provided");
            return E_SDB_CLA_UNKNOWN;
         }

         /* Then try to look it up */
         Status = eCilLookup(M_STM_CIL_MAPNAME, argv[ArgNum], &CilUnit);
         if(Status != SYS_NOMINAL) {
            printf("CIL name \"%s\" not recognised\n", argv[ArgNum]);
            mStmUsage(argv[0], "Unknown CIL name");
            return E_SDB_CLA_UNKNOWN;
         }

         /* If we get this far, alls well. We'll try to use this CIL ID */
         mStmCmdLineArgs.CilId = CilUnit;

         printf("Using CIL ID %d (= 0x%2.2X, %s)\n",
                CilUnit, CilUnit, argv[ArgNum]);

      }
      else if(strncmp(argv[ArgNum], "-tu", 3) == 0)
      {
         /*
         ** This option, allows the use of CIL Test Unit IDs instead
         ** of the default SDB ID (defined in SdbPrivate.h
         */

         NumValues = sscanf(argv[ArgNum], "-tu%d", &CilTestUnitOffset);
         if(NumValues == 1)
         {
            mStmCmdLineArgs.CilId = E_CIL_TU0 + CilTestUnitOffset;
         }
         else
         {
            printf("Test unit \"%s\" not recognised\n", argv[ArgNum]);
            mStmUsage(argv[0], "Unknown CIL test unit");
            return E_SDB_CLA_UNKNOWN;
         }

         /* Now check that the test unit ID is valid */
         if
         (
            (mStmCmdLineArgs.CilId < E_CIL_TU0) ||
            (mStmCmdLineArgs.CilId > E_CIL_TU9)
         )
         {
            printf("Test unit number out of range \"%s\"\n", argv[ArgNum]);
            mStmUsage(argv[0], "Incorrect CIL test unit");
            return E_SDB_CLA_UNKNOWN;
         }
      }
      else
      {
         printf("Argument \"%s\" not recognised\n", argv[ArgNum]);
         mStmUsage(argv[0], "Argument not recognised");
         return E_SDB_CLA_UNKNOWN;
      }


   }  /* End of for loop */


   /* Terminate the function and return success */
   return SYS_NOMINAL;

}  /* End of mStmParseArgs() */




void mStmUsage(
   char *ExecNamePtr,
   char *MessagePtr
)
{
/*
** Function Name:
**    mStmUsage
**
** Type:
**    void
**
** Purpose:
**    Print an error message regarding the correct usage of this
**    program.
**
** Description:
**    ...
**
** Arguments:
**    char *ExecNamePtr        (in)
**       Character string containg the name of the executable.
**    char *MessagePtr         (in)
**       A null-terminated character string containg a
**       diagnostic message to print. This character string is
**       prefixed by "ERROR: " and is written to stderr. It
**       should not contain an newline ("\n") character at the
**       end of the string. If this variable MessagePtr is set
**       to NULL, then no error message will be printed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    21-Jun-2000 djm Initial version.
**
*/

   /* No local variables */

   /* If we have an associated error message to print, then do so. */
   if(MessagePtr != NULL)
   {
      fprintf(stderr, "ERROR: %s\n", MessagePtr);
   }

   /* Print information on how to use the application */
   fprintf(stderr, "\nUsage: %s [options]\n\n", ExecNamePtr);
   fprintf(stderr,
      "Options:\n"
      "               (no option) Normal program operation\n"
      " -help         Print this text and exit\n"
      " -cil NAME     Use CIL ID (specified by case-sensitive NAME)\n"
      " -tu#          Use CIL test unit ID TU# (where #=0-9) instead of TU0\n"
   );

   /* There is no return value */


} /* End of mStmUsage() */


/* EOF */
