/*
** Module Name:
**    testcount.c
**
** Purpose:
**    Test program for the counting commands in the SDB ICD.
**
** Description:
**    This program tests the COUNTSOURCES, COUNTDATA and COUNTMSRMENTS
**    commands for the SDB.
**
**    This program uses the package ID "STC" - SDB Test Count.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testcount.c,v 0.4 2004/01/19 11:06:06 sxf Exp $
**
** History:
**    $Log: testcount.c,v $
**    Revision 0.4  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.3  2000/09/08 14:30:35  djm
**    Implemented new global data enumeration naming scheme.
**
**    Revision 0.2  2000/06/23 10:35:16  djm
**    Added multiple-request testing function.
**
**    Revision 0.1  2000/06/22 07:58:41  djm
**    Initial creation.
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
#define M_STC_CIL_MAPNAME "/opt/ttl/etc/Cil.map"
#else
#define M_STC_CIL_MAPNAME "/ttl/sw/etc/Cil.map"
#endif

#define M_STC_DATASIZE     16384      /* Max size of accepted CIL messages */
#define M_STC_TIMEOUT      300        /* CIL Rx timeout (in milliseconds) */
/* NB: the timeout needs to be faster than the CHB heartbeat period */
#define M_STC_CIL_ID       E_CIL_TU0  /* Default CIL ID */
#define M_STC_SDB_CIL_ID   E_CIL_SDB  /* ID of database server */
#define M_STC_MCB_CIL_ID   E_CIL_SDB  /* ID target to send messages to */
#define M_STC_NAMELEN      8          /* Max length for a CIL name */

#if 0
#define M_STC_MAXSRC       256        /* Max no. of IDs to deal with */
#define M_STC_MAXDAT       4096       /* Max no. of IDs to deal with */
#define M_STC_TIMESTRLEN   24         /* Max lenght for timestamp strings */
#define M_STC_ESC_CHAR     27         /* Escape char for ANSI esc.sequences */
#endif

/* Global data */

typedef struct
{
   Int32_t CilId;            /* CIL address ID */
} mStcCmdLineArgs_t;

mStcCmdLineArgs_t mStcCmdLineArgs
   = {
      M_STC_CIL_ID,          /* Default CIL ID */
   };

/* Other global variables */
Uint32_t mStcSeqNum = 0;     /* CIL message sequence number */
char mStcCilName[E_CIL_EOL][M_STC_NAMELEN];   /* Storage for CIL names */




/* Function prototypes */

Status_t mStcParseArgs(int argc, char *argv[]);
void mStcUsage(char *ExecNamePtr, char *MessagePtr);
Status_t mStcCountData(Int32_t SourceId);
Status_t mStcCountMsrments(Int32_t SourceId, Int32_t DatumId);






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
**    Top level function of the "testcount" program.
**
** Description:
**    Top level function of the "SDB counter". It is called on
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
**    21-Jun-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;        /* Function return status variable */
   Int32_t SourceId;
   Int32_t DatumId;

   /* Print startup diagnostic */
   printf("SDB TESTCOUNT PROGRAM\n");

   /* Parse command line arguments (CLAs) */
   Status = mStcParseArgs(argc, argv);
   if(Status != SYS_NOMINAL)
   {
      printf("Failure parsing command line arguments\n");
      return EXIT_FAILURE;
   }



   /* Do the CIL setup */
   Status = eCilSetup(M_STC_CIL_MAPNAME, mStcCmdLineArgs.CilId);
   if(Status != SYS_NOMINAL)
   {
      printf
      (
         "Error: Failed to allocate CIL address (%d = 0x%x)\n",
         mStcCmdLineArgs.CilId, mStcCmdLineArgs.CilId
      );
      return EXIT_FAILURE;
   }


   /* Fill in array of CIL names for rapid access */
   strcpy(mStcCilName[0], "???");
   for(SourceId = E_CIL_BOL+1; SourceId < E_CIL_EOL; SourceId++)
   {
      Status = eCilName(M_STC_CIL_MAPNAME, SourceId, 
                        M_STC_NAMELEN, mStcCilName[SourceId]);

      if(Status != SYS_NOMINAL)
      {
         printf("Error getting name for SourceId=0x%x\n", SourceId);
         exit(EXIT_FAILURE);
      }
   }


   /* Call the test functions */

   Status = mStcCountSources();
   if(Status != SYS_NOMINAL)
   {
      printf("Error <0x%x>: Failed to count sources in SDB\n", Status);
   }

   for(SourceId = E_CIL_BOL+1; SourceId < E_CIL_EOL; SourceId++)
   {
      Status = mStcCountData(SourceId);
      if(Status != SYS_NOMINAL)
      {
         printf(
            "Error <0x%x>: Failed to count data for source %d in SDB\n",
            Status, SourceId
         );
      }
   }

   SourceId = E_CIL_SDB;
   DatumId = D_SDB_PROC_STATE + 2;
   Status = mStcCountMsrments(SourceId, DatumId);
   if(Status != SYS_NOMINAL)
   {
      printf(
         "Error <0x%x>: Failed to count measurements in SDB\n",
         Status
      );
   }

   /* Terminate program */
   printf("Complete\n");
   return EXIT_SUCCESS;

}  /* End of main() */



Status_t mStcParseArgs(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    mStcParseArgs
**
** Type:
**    Status_t
**
** Purpose:
**    Command line argument processing function for the STC program.
**
** Description:
**    Takes the command line arguments (as specified as arguments
**    to the  main() function) and processes them to fill in the
**    global structure.
**
**    NOTE: If the command line argument handling is change, then
**    one must remember to update the mStcUsage() function as well.
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
         mStcUsage(argv[0], NULL);
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
            mStcUsage(argv[0], "No CIL name provided");
            return E_SDB_CLA_UNKNOWN;
         }

         /* Then try to look it up */
         Status = eCilLookup(M_STC_CIL_MAPNAME, argv[ArgNum], &CilUnit);
         if(Status != SYS_NOMINAL) {
            printf("CIL name \"%s\" not recognised\n", argv[ArgNum]);
            mStcUsage(argv[0], "Unknown CIL name");
            return E_SDB_CLA_UNKNOWN;
         }

         /* If we get this far, alls well. We'll try to use this CIL ID */
         mStcCmdLineArgs.CilId = CilUnit;

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
            mStcCmdLineArgs.CilId = E_CIL_TU0 + CilTestUnitOffset;
         }
         else
         {
            printf("Test unit \"%s\" not recognised\n", argv[ArgNum]);
            mStcUsage(argv[0], "Unknown CIL test unit");
            return E_SDB_CLA_UNKNOWN;
         }

         /* Now check that the test unit ID is valid */
         if
         (
            (mStcCmdLineArgs.CilId < E_CIL_TU0) ||
            (mStcCmdLineArgs.CilId > E_CIL_TU9)
         )
         {
            printf("Test unit number out of range \"%s\"\n", argv[ArgNum]);
            mStcUsage(argv[0], "Incorrect CIL test unit");
            return E_SDB_CLA_UNKNOWN;
         }
      }
      else
      {
         printf("Argument \"%s\" not recognised\n", argv[ArgNum]);
         mStcUsage(argv[0], "Argument not recognised");
         return E_SDB_CLA_UNKNOWN;
      }


   }  /* End of for loop */


   /* Terminate the function and return success */
   return SYS_NOMINAL;

}  /* End of mStcParseArgs() */




void mStcUsage(
   char *ExecNamePtr,
   char *MessagePtr
)
{
/*
** Function Name:
**    mStcUsage
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


} /* End of mStcUsage() */



Status_t mStcCountSources()
{
/*
** Function Name:
**    mStcCountSources
**
** Type:
**    Status_t
**
** Purpose:
**    Test function for counting the sources in the database.
**
** Description:
**    ...
**
** Arguments:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    21-Jun-2000 djm Initial creation.
**
*/

   /* No local variables */
   Status_t Status;          /* Function return status */
   eCilMsg_t Msg;            /* CIL message */
   char Data[M_STC_DATASIZE];   /* Data buffer */
   Int32_t DelivererId;      /* CIL ID of task that sent packet to us */
   Uint32_t NumSources;      /* Number of sources held in the database */


   /* Formulate message */
   Msg.SourceId = mStcCmdLineArgs.CilId;
   Msg.DestId = M_STC_SDB_CIL_ID;
   Msg.Service = E_SDB_COUNTSOURCES;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.SeqNum = ++mStcSeqNum;
   Msg.TimeStamp.t_sec = 0;
   Msg.TimeStamp.t_nsec = 0;
   Msg.DataPtr = &Data[0];
   Msg.DataLen = 0;


   /* Send CIL message */
   Status = eCilSend(M_STC_MCB_CIL_ID, &Msg);
   if(Status != SYS_NOMINAL)
   {
      printf("Unable to send source list request (0x%x)\n", Status);
      return Status;
   }

   /* Block, waiting for an inbound message */
   while(
      ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStcSeqNum)) &&
      (Status != E_CIL_TIMEOUT)
   )
   {
      Msg.DataLen = M_STC_DATASIZE;
      Status = eCilReceive(M_STC_TIMEOUT, &DelivererId, &Msg);
   }

/* DEBUG - TEST DIAGNOSTIC CODE */
/*
{
int n;
printf("--- DataLen = %d\n", Msg.DataLen);
printf("Rx (0x%2.2x, %d): ", Msg.SourceId, Msg.Service);
for(n = 0; n < Msg.DataLen; n++)
  printf("%2.2x ", ((char *)Msg.DataPtr)[n]);
printf("\n");
}
*/

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


   /* Check the returned message length */
   if(Msg.DataLen < sizeof(NumSources))
   {
      printf(
         "Message truncated (%d of %d expected)\n",
         Msg.DataLen, sizeof(NumSources)
      );
      return SYS_ERROR_NON_FATAL;
   }

   /* Extract the result */
   memcpy(&NumSources, Data, sizeof(NumSources));
   NumSources = ntohl(NumSources);

   /* Print the results */
   printf("Number of sources = %d\n", NumSources);

   /* Terminate function */
   return SYS_NOMINAL;

}  /* End of mStcCountSources */



Status_t mStcCountData
(
   Int32_t SourceId
)
{
/*
** Function Name:
**    mStcCountData
**
** Type:
**    Status_t
**
** Purpose:
**    Test function for counting the sources in the database.
**
** Description:
**    ...
**
** Arguments:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    21-Jun-2000 djm Initial creation.
**
*/

   /* No local variables */
   Status_t Status;          /* Function return status */
   eCilMsg_t Msg;            /* CIL message */
   char Data[M_STC_DATASIZE];   /* Data buffer */
   Int32_t DelivererId;      /* CIL ID of task that sent packet to us */
   Uint32_t NumData;         /* Number of data definitons for a given source */


   /* Formulate message */
   Msg.SourceId = mStcCmdLineArgs.CilId;
   Msg.DestId = M_STC_SDB_CIL_ID;
   Msg.Service = E_SDB_COUNTDATA;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.SeqNum = ++mStcSeqNum;
   Msg.TimeStamp.t_sec = 0;
   Msg.TimeStamp.t_nsec = 0;
   Msg.DataPtr = &Data[0];
   Msg.DataLen = 0;


   /* Fill in data block */
   SourceId = htonl(SourceId);
   Msg.DataLen = sizeof(SourceId);
   memcpy(&Data[0], &SourceId, Msg.DataLen);


   /* Send CIL message */
   Status = eCilSend(M_STC_MCB_CIL_ID, &Msg);
   if(Status != SYS_NOMINAL)
   {
      printf("Unable to send source list request (0x%x)\n", Status);
      return Status;
   }

   /* Block, waiting for an inbound message */
   while(
      ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStcSeqNum)) &&
      (Status != E_CIL_TIMEOUT)
   )
   {
      Msg.DataLen = M_STC_DATASIZE;
      Status = eCilReceive(M_STC_TIMEOUT, &DelivererId, &Msg);
   }

/* DEBUG - TEST DIAGNOSTIC CODE */
/*
{
int n;
printf("--- DataLen = %d\n", Msg.DataLen);
printf("Rx (0x%2.2x, %d): ", Msg.SourceId, Msg.Service);
for(n = 0; n < Msg.DataLen; n++)
  printf("%2.2x ", ((char *)Msg.DataPtr)[n]);
printf("\n");
}
*/

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


   /* Check the returned message length */
   if(Msg.DataLen < sizeof(NumData))
   {
      printf(
         "Message truncated (%d of %d expected)\n",
         Msg.DataLen, sizeof(NumData)
      );
      return SYS_ERROR_NON_FATAL;
   }

   /* Extract the result */
   memcpy(&NumData, Data, sizeof(NumData));
   NumData = ntohl(NumData);

   /* Print the results */
   SourceId = ntohl(SourceId);
   printf(
      "Number of data for source %s (0x%2.2x) = ",
      mStcCilName[SourceId], SourceId
   );
   if(NumData > 0)
   {
      printf("%d\n", NumData);
   }
   else
   {
      printf("-\n");
   }

   /* Terminate function */
   return SYS_NOMINAL;

}  /* End of mStcCountData */



Status_t mStcCountMsrments
(
   Int32_t SourceId,
   Int32_t DatumId
)
{
/*
** Function Name:
**    mStcCountMsrments
**
** Type:
**    Status_t
**
** Purpose:
**    Test function for counting the sources in the database.
**
** Description:
**    ...
**
** Arguments:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    21-Jun-2000 djm Initial creation.
**
*/


   /* No local variables */
   Status_t Status;          /* Function return status */
   eCilMsg_t Msg;            /* CIL message */
   char Data[M_STC_DATASIZE];   /* Data buffer */
   char *DataPtr;            /* Pointer to Data */
   Int32_t DelivererId;      /* CIL ID of task that sent packet to us */
   Uint32_t NumReqs;         /* Number of requests */
   eSdbMulReq_t MulReq;      /* Request structure */

   /* Formulate message */
   Msg.SourceId = mStcCmdLineArgs.CilId;
   Msg.DestId = M_STC_SDB_CIL_ID;
   Msg.Service = E_SDB_COUNTMSRMENTS;
   Msg.Class = E_CIL_CMD_CLASS;
   Msg.SeqNum = ++mStcSeqNum;
   Msg.TimeStamp.t_sec = 0;
   Msg.TimeStamp.t_nsec = 0;
   Msg.DataPtr = DataPtr = &Data[0];
   Msg.DataLen = 0;

   NumReqs = 1;

   /* Fill in data block */
   Msg.DataLen = sizeof(NumReqs);
   NumReqs = NumReqs;
   memcpy(DataPtr, &NumReqs, Msg.DataLen);
   DataPtr += sizeof(NumReqs);

   Msg.DataLen += sizeof(MulReq);
   MulReq.SourceId = SourceId;
   MulReq.DatumId = DatumId;
   MulReq.NumMsrments = 0;
   MulReq.OldestTime.t_sec = 0;
   MulReq.OldestTime.t_nsec = 0;
   MulReq.NewestTime.t_sec = 0;
   MulReq.NewestTime.t_nsec = 0;
   eTimGetTime(&MulReq.OldestTime);
   eTimSleep(5);
   eTimGetTime(&MulReq.NewestTime);
   memcpy(DataPtr, &MulReq, sizeof(MulReq));
   DataPtr += sizeof(MulReq);
   
{
int n;
printf("--- DataLen = %d\n", Msg.DataLen);
printf("Tx (0x%2.2x): ", Msg.SourceId);
for(n = 0; n < Msg.DataLen; n++)
  printf("%2.2x", ((char *)Msg.DataPtr)[n]);
printf("\n");
}


   /* Convert the byte-order of the entire message to network byte order */
   eCilConvert32bitArray(Msg.DataLen, Msg.DataPtr);

   /* Send CIL message */
   Status = eCilSend(M_STC_MCB_CIL_ID, &Msg);
   if(Status != SYS_NOMINAL)
   {
      printf("Unable to send measurement count request (0x%x)\n", Status);
      return Status;
   }

   /* Block, waiting for an inbound message */
   while(
      ((Msg.Class != E_CIL_RSP_CLASS) || (Msg.SeqNum != mStcSeqNum)) &&
      (Status != E_CIL_TIMEOUT)
   )
   {
      Msg.DataLen = M_STC_DATASIZE;
      Status = eCilReceive(M_STC_TIMEOUT, &DelivererId, &Msg);
   }

   /* Convert the byte-order of the entire message back again */
   eCilConvert32bitArray(Msg.DataLen, Msg.DataPtr);



/* DEBUG - TEST DIAGNOSTIC CODE */

{
int n;
printf("--- DataLen = %d\n", Msg.DataLen);
printf("Rx (0x%2.2x): ", Msg.SourceId);
for(n = 0; n < Msg.DataLen; n++)
  printf("%2.2x", ((char *)Msg.DataPtr)[n]);
printf("\n");
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


#if 0

   /* Check the returned message length */
   if(Msg.DataLen < sizeof(NumData))
   {
      printf(
         "Message truncated (%d of %d expected)\n",
         Msg.DataLen, sizeof(NumData)
      );
      return SYS_ERROR_NON_FATAL;
   }

   /* Extract the result */
   memcpy(&NumData, Data, sizeof(NumData));
   NumData = ntohl(NumData);

   /* Print the results */
   SourceId = ntohl(SourceId);
   printf(
      "Number of data for source %s (0x%2.2x) = ",
      mStcCilName[SourceId], SourceId
   );
   if(NumData > 0)
   {
      printf("%d\n", NumData);
   }
   else
   {
      printf("-\n");
   }

#endif


   /* Terminate function */
   return SYS_NOMINAL;


}  /* End of mStcCountMsrments */


/* EOF */
