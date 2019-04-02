/*
** Module Name:
**    testclient.c
**
** Purpose:
**    A program for testing the SDB.
**
** Description:
**    This module contains code for a SDB testing program. It sends
**    test messages to the status database (SDB).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    plw: Paul L. Wright (TTL)
**
** Version:
**    $Id: testclient.c,v 0.9 2004/01/19 11:06:06 sxf Exp $
**
** History:
**    $Log: testclient.c,v $
**    Revision 0.9  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.8  2001/09/14 11:08:01  djm
**    Trivial changes to assist with testing the new CLEAR_S service.
**
**    Revision 0.7  2000/09/05 14:21:32  djm
**    Added selection menu. Implemented SAFESTATE/ACTIVATE/SHUTDOWN
**    test commands.
**
**    Revision 0.6  2000/05/31 08:50:02  djm
**    ÿ¡Minor changes to comments and transmitted values.
**
**    Revision 0.5  2000/05/21 15:26:48  djm
**    Added code to make use of the SDB's "LIST" services.
**
**    Revision 0.4  2000/05/17 15:11:58  djm
**    Major changes to allow both submission and retrieve messages
**    to be sent.
**
**    Revision 0.3  2000/05/12 15:23:51  djm
**    Added ability to submit messages of random data elements to SDB.
**
**    Revision 0.2  2000/05/12 08:14:35  djm
**    Minor changes to facilitate testing the data submission
**    to the SDB.
**
**    Revision 0.1  2000/05/09 16:49:00  djm
**    Initial creation. Includes basic heartbeat transmission.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>


#include "TtlSystem.h"
#include "Wfl.h"
#include "Cil.h"
#include "Sdb.h"

/* Symbolic constants */

#define OPERATION   8           /* Task to test */
                                /* 0 = heartbeat */
                                /* 1 = submit */
                                /* 2 = retrieve */
                                /* 3 = get list of sources */
                                /* 4 = get list of data from source */
                                /* 5 = safe-state command */
                                /* 6 = activate command */
                                /* 7 = shutdown command */
                                /* 8 = clear source command */
#define CLIENT_NUM  E_CIL_TU0	/* Default client (MUST BE TU0) DEBUG */
#define ROUTER_NUM  E_CIL_SDB   /* Choice router */
#define SERVER_NUM  E_CIL_SDB   /* Choice server to send to */
#define REPORT_FREQ 1		/* How often to print to screen */
#define NAME_LEN 32		/* Length of CIL name string */

/* Global variables */

#ifdef E_WFL_OS_QNX4
char MyMap[] = "/opt/ttl/etc/Cil.map";  /* Where to find the CIL map */
#else
char MyMap[] = "/ttl/sw/etc/Cil.map";  /* Where to find the CIL map */
#endif



/*
** MAIN FUNCTION
*/

int main (int argc, char *argv[])
{
/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    ...
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
**    09-May-2000 djm Initial creation.
**
*/

   /* Local variables */

   Status_t MyStatus;		/* Function return status */
   Int32_t RxId;		/* CIL ID of sender of rx'd messages */
   char TxData[512];		/* Buffer for data to send */
   char RxData[512];		/* Data for received data */
   int Timeout = 500;		/* How long to wait before timing out on rx */
   int Count = 0;		/* Counter, for stats reporting */
   int NumTimeouts = 0;		/* Number of timeouts on message waits */
   int NumTxFail = 0;		/* Number of transmission failures */
   int NumSuccess = 0;		/* Number of successful transmissions */
   int NumErrors = 0;		/* Number of packet contents errors */
   eCilMsg_t TxMsg;		/* CIL message structure to send */
   eCilMsg_t RxMsg;		/* CIL message structure to receive */
   int NumValues;		/* Number of values read (for sscanf()) */
   Int32_t CilTestUnitOffset;	/* The increment from E_CIL_TU0 (0-9) */
   Int32_t CilTestUnit;    /* CIL test unit ID */
   char CilName[NAME_LEN]; /* CIL name holding variable */
   Uint32_t NumElts;       /* Number of data elements to submit/retrieve */
   eSdbDatum_t Datum;      /* Structure for Tx/Rx-ing data */
   eSdbSngReq_t SngReq;    /* Request for a single data element */
   Int32_t SourceId;       /* ID for listing data elements for that source */
   Int32_t DatumId;        /* ID for listing data elements for that source */
   int n;
   int op;                 /* Operation to perform */


   /* Print a start-up message */
   printf ("SDB test client\n");

   printf("\n");
   printf("_#___OPTIONS______________________\n");
   printf(" 0 = heartbeat\n");
   printf(" 1 = submit\n");
   printf(" 2 = retrieve\n");
   printf(" 3 = get list of sources\n");
   printf(" 4 = get list of data from source\n");
   printf(" 5 = safe-state command\n");
   printf(" 6 = activate command\n");
   printf(" 7 = shutdown command\n");
   printf(" 8 = clear source\n");
   printf("\n");
   printf("Enter operation: ");
   scanf("%d", &op);


   /* Parse for optional command line arguments */

   if (argc == 2)
   {
      NumValues = sscanf (argv[1], "-tu%d", &CilTestUnitOffset);
      if (NumValues == 1)
      {
         CilTestUnit = CLIENT_NUM + CilTestUnitOffset;
      }
      else
      {
         CilTestUnit = CLIENT_NUM;
      }
   }
   else if (argc == 1)
   {
      CilTestUnit = CLIENT_NUM;
   }
   else
   {
      printf ("Usage: %s [-tu#]\n\nOptions:\n", argv[0]);
      printf ("   -tu#    CIL test unit number (where #=0-9)\n");
      exit (EXIT_FAILURE);
   }


   /* Print who we are */
   MyStatus = eCilName (MyMap, CilTestUnit, NAME_LEN, CilName);
   if (MyStatus != SYS_NOMINAL)
   {
      printf ("Error determining who we are (%d)\n", CilTestUnit);
      exit (EXIT_FAILURE);
   }
   printf ("CIL Name=%s, ID=%d\n", CilName, CilTestUnit);


   /* Setup CIL and state what address we will use */
   MyStatus = eCilSetup (MyMap, CilTestUnit);
   if (MyStatus != SYS_NOMINAL)
   {
      printf ("Setup failure - %d\n", MyStatus);
      return (1);
   }



   /* Fill the data buffer with some characters */
   memset (TxData, 0xaa, sizeof (TxData));

   /* Setup the common CIL message structure */
   TxMsg.SourceId = CilTestUnit;
   TxMsg.DestId = SERVER_NUM;
   TxMsg.Class = E_CIL_CMD_CLASS;
   TxMsg.SeqNum = 0;
   TxMsg.TimeStamp.t_sec = 0;
   TxMsg.TimeStamp.t_nsec = 0;

   /* Select the custom CIL message information */
   switch(op)
   {
      case 0: /* Heartbeat */
         TxMsg.Service = E_SDB_HEARTBEAT;
         TxMsg.DataPtr = TxData;
         TxMsg.DataLen = 0;
         break;
      case 1:  /* Submit data */
         TxMsg.Service = E_SDB_SUBMIT_1;
         NumElts = 1;
         TxMsg.DataLen = sizeof(NumElts) + sizeof(eSdbDatum_t) * NumElts;
         NumElts = htonl(NumElts);
         memcpy(TxData, &NumElts, sizeof(NumElts));

         for(n = 0; n < ntohl(NumElts); n++)
         {
            Datum.SourceId = htonl(CilTestUnit);
            Datum.DatumId = htonl(rand() % 10);
            printf("--- %d\n", ntohl(Datum.DatumId));
            Datum.Units = htonl(E_SDB_NO_UNITS);
            Datum.Msrment.TimeStamp.t_sec = time(NULL);
            Datum.Msrment.TimeStamp.t_nsec = 0;
            Datum.Msrment.Value = htonl(13);
            memcpy(
               TxData+sizeof(NumElts)+sizeof(eSdbDatum_t)*n,
               &Datum,  sizeof(Datum)
            );
         }
         for(n = 0; n < TxMsg.DataLen; n++) {
            if(((n-4)%sizeof(eSdbDatum_t)) == 0) printf("\n");
            printf("%2.2x ", TxData[n]);
         }
         printf("\n");
         TxMsg.DataPtr = TxData;
         break;
      case 2:  /* Retrieve data */
         TxMsg.Service = E_SDB_RETRIEVE_1;
         NumElts = 2;
         TxMsg.DataLen = sizeof(NumElts) + sizeof(eSdbSngReq_t) * NumElts;
         NumElts = htonl(NumElts);
         memcpy(TxData, &NumElts, sizeof(NumElts));
         for(n = 0; n < ntohl(NumElts); n++)
         {
            SngReq.SourceId = htonl(E_CIL_TU3);
            SngReq.DatumId = htonl(rand() % 10);
            memcpy(
               TxData+sizeof(NumElts)+sizeof(eSdbSngReq_t)*n,
               &SngReq,  sizeof(SngReq)
            );
         }
         TxMsg.DataPtr = TxData;
         break;
      case 3:  /* Retrieve list of sources */
         TxMsg.Service = E_SDB_LISTSOURCES;
         TxMsg.DataPtr = TxData;
         TxMsg.DataLen = 0;
         break;
      case 4:  /* Retrieve list of data element IDs for a given source */
         TxMsg.Service = E_SDB_LISTDATA;
         SourceId = 0x19;
         TxMsg.DataLen = sizeof(SourceId);
         SourceId = htonl(SourceId);
         memcpy(TxData, &SourceId, sizeof(SourceId));
         TxMsg.DataPtr = TxData;
         break;
      case 5:  /* Command remote process to go to safe state */
         TxMsg.Service = E_SDB_SAFESTATE;
         TxMsg.DataPtr = TxData;
         TxMsg.DataLen = 0;
         break;
      case 6:  /* Activate remote process */
         TxMsg.Service = E_SDB_ACTIVATE;
         TxMsg.DataPtr = TxData;
         TxMsg.DataLen = 0;
         break;
      case 7:  /* Terminate remote application */
         TxMsg.Service = E_SDB_SHUTDOWN;
         TxMsg.DataPtr = TxData;
         TxMsg.DataLen = 0;
         break;
      case 8:  /* Clear source */
         TxMsg.Service = E_SDB_CLEAR_S;
         NumElts = 1;
         TxMsg.DataLen = sizeof(SourceId) + 
                         sizeof(NumElts) + NumElts * sizeof(DatumId);

         SourceId = E_CIL_TU0;
         SourceId = htonl(SourceId);
         memcpy(TxData, &SourceId, sizeof(SourceId));

         NumElts = htonl(NumElts);
         memcpy(TxData+sizeof(SourceId), &NumElts, sizeof(NumElts));

         DatumId = 4;
         DatumId = htonl(DatumId);
         memcpy(TxData+sizeof(SourceId)+sizeof(NumElts),
                &DatumId, sizeof(DatumId));

         TxMsg.DataPtr = TxData;
         break;
      default:
         printf("Invalid test service\n");
         exit(EXIT_FAILURE);
   }


   /* Start the main loop, repeatedly transmitting the messages */
   for (;;)
   {

      /* Get a <cr> to continue */
      printf("\nPress any key to continue\n");
      fflush(stdout);
      getchar();

      if ((Count % REPORT_FREQ) == 0)
      {
         printf ("\rSUCCESSES: total=%d  ", NumSuccess);
         printf ("ERRORS: send=%d timeouts=%d contents=%d",
                 NumTxFail, NumTimeouts, NumErrors);
         fflush (stdout);
         Count = 0;
      }
      Count++;




      /* Fill in any time dependant data into the CIL message */
      switch(op)
      {
         case 0: /* Heartbeat */
            break;
         case 1:  /* Submit data */
            for(n = 0; n < ntohl(NumElts); n++)
            {
               Datum.DatumId = htonl(rand() % 10);
               Datum.SourceId = htonl(CilTestUnit);
               printf(" %d", ntohl(Datum.DatumId));
               Datum.Units = htonl(E_SDB_NO_UNITS);
               Datum.Msrment.Value = htonl((rand() % 20));
               Datum.Msrment.TimeStamp.t_sec = htonl(time(NULL)+rand());
               Datum.Msrment.TimeStamp.t_nsec = 0;
               memcpy(
                  TxData+sizeof(NumElts)+sizeof(eSdbDatum_t)*n,
                  &Datum,  sizeof(Datum)
               );
            }
/*
            for(n = 0; n < TxMsg.DataLen; n++) {
               if(((n-4)%sizeof(eSdbDatum_t)) == 0) printf("\n");
               printf("%2.2x ", TxData[n]);
            }
*/
            printf("\n");
            TxMsg.DataPtr = TxData;
            break;
         case 2:  /* Retrieve data */
            for(n = 0; n < ntohl(NumElts); n++)
            {
               SngReq.DatumId = htonl(rand() % 10);
               SngReq.SourceId = htonl(E_CIL_TU3);
               memcpy(
                  TxData+sizeof(NumElts)+sizeof(eSdbSngReq_t)*n,
                  &SngReq,  sizeof(SngReq)
               );
            }
            TxMsg.DataPtr = TxData;

         printf("\nTx: ");
         for(n = 0; n < TxMsg.DataLen; n++) {
/*
            if(((n-4)%sizeof(eSdbDatum_t)) == 0) printf("\n");
*/
            printf("%2.2x ", TxData[n]);
         }
         printf("\n");


            break;
         case 3:
         case 4:
         case 5:
         case 6:
         case 7:
         case 8:

            printf("\nTx: ");
            for(n = 0; n < TxMsg.DataLen; n++) {
               printf("%2.2x ", TxData[n]);
            }
            break;
         default:
            printf("Invalid test service\n");
            exit(EXIT_FAILURE);
      }




      /* Actually send the message */
      MyStatus = eCilSend (ROUTER_NUM, &TxMsg);
      if (MyStatus != SYS_NOMINAL)
      {
         printf ("Send failure - %d\n", MyStatus);
         NumTxFail++;
         continue;
      }

      /* Prepare to receive some data */
      RxMsg.DataPtr = RxData;
      RxMsg.DataLen = sizeof (RxData);

      /* Actually receive the data */
      MyStatus = eCilReceive (Timeout, &RxId, &RxMsg);
      if (MyStatus != SYS_NOMINAL)
      {
         NumTimeouts++;
         continue;
      }


{
int z;
printf("\nMessage received... ");
for(z = 0; z < RxMsg.DataLen; z++) {
  if((z%24) ==4) printf("\n");
  printf("%2.2x ", RxData[z]);
}
printf("\n");
}



      /* Verify data integrity */
      if (memcmp (TxData, RxData, sizeof (TxData)) != 0)
      {
         printf (" Rx Data != Tx Data");
         NumErrors++;
         continue;
      }

      NumSuccess++;

   }  /* End of for(;;) */


}  /* End of main() */


/* EOF */
