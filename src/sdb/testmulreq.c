/*
** Module Name:
**    testmulreq.c
**
** Purpose:
**    Test program for the multi-request commands in the SDB ICD.
**
** Description:
**    This program tests the RETRIEVE_N command for the SDB.
**
**    This program uses the package ID "STM" - SDB Test Mulreq.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testmulreq.c,v 0.3 2004/01/19 11:06:06 sxf Exp $
**
** History:
**    $Log: testmulreq.c,v $
**    Revision 0.3  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.2  2000/09/08 14:30:43  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.1  2000/06/27 08:44:46  djm
**    *** empty log message ***
**
**
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "TtlSystem.h"
#include "Wfl.h"
#include "Sdb.h"
#include "SdbPrivate.h"
#include "Cil.h"
#include "Log.h"
#include "Tim.h"


/* Definitions */

#ifdef E_WFL_OS_QNX4
#define M_STM_CIL_MAPNAME "//1/opt/ttl/src/cil/Cil.map"
#else
#define M_STM_CIL_MAPNAME "/ttl/sw/src/cil/Cil.map"
#endif

#define M_STM_DATASIZE     16384      /* Max size of accepted CIL messages */
#define M_STM_TIMEOUT      300        /* CIL Rx timeout (in milliseconds) */
/* NB: the timeout needs to be faster than the CHB heartbeat period */
#define M_STM_CIL_ID       E_CIL_TU0  /* Default CIL ID */
#define M_STM_SDB_CIL_ID   E_CIL_SDB  /* ID of database server */
#define M_STM_MCB_CIL_ID   E_CIL_SDB  /* ID target to send messages to */
#define M_STM_NAMELEN      8          /* Max length for a CIL name */
#define M_STM_MAXMSRMENTS  65535      /* Maximum number of msrments to receive */


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
**    Top level function of the "testmulreq" program.
**
** Description:
**    Top level function of the "SDB mulreq tester". It is called on
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
   Uint32_t NumReqs;         /* Number of requests being made */



   /* Print startup diagnostic */
   printf("SDB TESTMULREQ PROGRAM\n");

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
         printf("Error getting name for SourceId=0x%x\n", SourceId);
         exit(EXIT_FAILURE);
      }
   }


   /*
   ** Okay... test time
   */

   /* Send a request to the SDB */
   Msg.SourceId = M_STM_CIL_ID;
   Msg.DestId = M_STM_SDB_CIL_ID;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.Service = E_SDB_RETRIEVE_N;
   Msg.SeqNum = ++mStmSeqNum;
   Status = eTimGetTime(&Msg.TimeStamp);

   /* Put in the number of requests */
   NumReqs = 3;
   Msg.DataPtr = DataPtr = &Data[0];
   memcpy(DataPtr, &NumReqs, sizeof(NumReqs));
   DataPtr += sizeof(NumReqs);

   /* Apply first request  (all times) */
   MulReq.SourceId = E_CIL_SDB;
   MulReq.DatumId = D_SDB_PROC_STATE;
   MulReq.NumMsrments = M_STM_MAXMSRMENTS;
   MulReq.NewestTime.t_sec = 0;
   MulReq.NewestTime.t_nsec = 0;
   MulReq.OldestTime.t_sec = 0;
   MulReq.OldestTime.t_nsec = 0;
   memcpy(DataPtr, &MulReq, sizeof(MulReq));
   DataPtr += sizeof(MulReq);


   /* Apply second request (time selective) */
   MulReq.SourceId = E_CIL_SDB;
   MulReq.DatumId = D_SDB_QTY_SUBMITTED;
   MulReq.NumMsrments = M_STM_MAXMSRMENTS;
   Status = eTimGetTime(&MulReq.OldestTime);
   Status = eTimSleep(2);   
   Status = eTimGetTime(&MulReq.NewestTime);
   memcpy(DataPtr, &MulReq, sizeof(MulReq));
   DataPtr += sizeof(MulReq);

   /* Apply third request (numerically selective) */
   MulReq.SourceId = E_CIL_SDB;
   MulReq.DatumId = D_SDB_QTY_SUBMITTED;
   MulReq.NumMsrments = 4;
   MulReq.NewestTime.t_sec = 0;
   MulReq.NewestTime.t_nsec = 0;
   MulReq.OldestTime.t_sec = 0;
   MulReq.OldestTime.t_nsec = 0;
   memcpy(DataPtr, &MulReq, sizeof(MulReq));
   DataPtr += sizeof(MulReq);



   Msg.DataPtr = &Data[0];
   Msg.DataLen = sizeof(NumReqs) + (NumReqs * sizeof(MulReq));

   /* Print the "send" message */
   {
      int n;

      printf("-----------------------------------\n");
      printf("Contents of the transmitted message:\n");
      printf("DataLen = %d\n", Msg.DataLen);
      printf("Source = 0x%2.2x\nService = %d\n", Msg.SourceId, Msg.Service);
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
      (Status != E_CIL_TIMEOUT)
   )
   {
      Msg.DataLen = M_STM_DATASIZE;
      Status = eCilReceive(M_STM_TIMEOUT, &DelivererId, &Msg);
      if(Status == E_CIL_TIMEOUT)
      {
         printf(
            "Msg rx'd (Service = %d, Class = %d)\n",
            Msg.Service, Msg.Class
         );
      }
      else
      {
         printf("Timeout\n");
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
      printf("Contents of the receiveded message:\n");
      printf("DataLen = %d\n", Msg.DataLen);
      printf("Message contents:\n");
      for(n = 0; n < Msg.DataLen; n++)
      {
         if((n%12) == 8) printf("\n");
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
