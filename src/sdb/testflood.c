/*
** Module Name:
**    testflood.c
**
** Purpose:
**    A program for injecting large amounts of test data into the SDB.
**
** Description:
**    This module contains code for a SDB testing program. It sends
**    a data submission to the SDB based on user input. It is designed
**    to put a large block of data into the system in order to test the
**    Data Analysis Tool (DAT).
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testflood.c,v 0.4 2004/01/19 11:06:06 sxf Exp $
**
** History:
**    $Log: testflood.c,v $
**    Revision 0.4  2004/01/19 11:06:06  sxf
**    Change hardcoded paths for QNX6.
**
**    Revision 0.3  2004/01/16 08:55:04  sxf
**    Minor changes to port to QNX6.
**
**    Revision 0.2  2000/10/04 11:17:29  djm
**    Minor changes for debugging reasons.
**
**    Revision 0.1  2000/09/20 14:21:13  djm
**    Initial creation.
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
#include <math.h>


#include "TtlSystem.h"
#include "Wfl.h"
#include "Cil.h"
#include "Tim.h"
#include "Sdb.h"

/* Symbolic constants */

#define OPERATION   1           /* Task to test */
                                /* 0 = heartbeat */
                                /* 1 = submit */
                                /* 2 = retrieve */
                                /* 3 = get list of sources */
                                /* 4 = get list of data from source */
#define CLIENT_NUM  E_CIL_TU0	/* Default client (MUST BE TU0) DEBUG */
#define ROUTER_NUM  E_CIL_MCB   /* Choice router */
#define SERVER_NUM  E_CIL_SDB   /* Choice server to send to */
#define REPORT_FREQ 1		    /* How often to print to screen */
#define NAME_LEN 32	           /* Length of CIL name string */

#define DATA_MAX  100          /* Maximum data value */
#define DATA_MIN -100

/* Global variables */

#ifdef E_WFL_OS_QNX4
char MyMap[] = "/opt/ttl/etc/Cil.map";	/* Where to find the CIL map */
#else
char MyMap[] = "/ttl/sw/etc/Cil.map";	/* Where to find the CIL map */
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
**    19-Sep-2000 djm Initial creation.
**
*/

   /* Local variables */

   Status_t Status;          /* Function return status */
   Int32_t RxId;             /* CIL ID of sender of rx'd messages */
   char TxData[4096];        /* Buffer for data to send */
   char RxData[4096];        /* Data for received data */
   int Timeout = 300;        /* How long to wait before timing out on rx */
   eCilMsg_t TxMsg;          /* CIL message structure to send */
   eCilMsg_t RxMsg;          /* CIL message structure to receive */
   int NumValues;            /* Number of values read (for sscanf()) */
   Int32_t CilTestUnitOffset;/* The increment from E_CIL_TU0 (0-9) */
   Int32_t CilTestUnit;      /* CIL test unit ID */
   char CilName[NAME_LEN];   /* CIL name holding variable */
   char Another[NAME_LEN];   /* User request for more data */
   Uint32_t NumElts;         /* Number of data elements to submit/retrieve */
   eSdbDatum_t Datum;        /* Structure for Tx/Rx-ing data */
   int n;
   eTtlTime_t TimeDiff;      /* Difference in time */
   eTtlTime_t TimeNow;       /* Current system time */

   Int32_t ReqSrc;           /* SourceID, as requested by the user */
   Int32_t ReqDat;           /* DatumID, as requested by the user */
   Int32_t ReqVal;           /* Datum value, as requested by the user */
 /*  Int32_t ReqUts;  */       /* Datum units, as requested by the user */


   /* Print a start-up message */
   printf ("testflood - SDB data injector\n\n");


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
   Status = eCilName (MyMap, CilTestUnit, NAME_LEN, CilName);
   if (Status != SYS_NOMINAL)
   {
      printf ("Error determining who we are (%d)\n", CilTestUnit);
      exit (EXIT_FAILURE);
   }


   /* Setup CIL and state what address we will use */
   Status = eCilSetup (MyMap, CilTestUnit);
   if (Status != SYS_NOMINAL)
   {
      printf ("CIL setup failure - <%x>\n", Status);
      return (1);
   }

   do
   {

      printf("           Enter the source ID (decimal): ");
      scanf("%d", &ReqSrc);
      printf("            Enter the datum ID (decimal): ");
      scanf("%d", &ReqDat);
      printf("  Elements per datum to submit (decimal): ");
      scanf("%d", &NumElts);

/*
      scanf("%d", &ReqVal);
      scanf("%d", &ReqUts);
*/

      ReqVal = -1;

      Status = eTimGetTime(&TimeNow);


      /* Setup the common CIL message structure */
      TxMsg.SourceId = ReqSrc;
      TxMsg.DestId = SERVER_NUM;
      TxMsg.Class = E_CIL_CMD_CLASS;
      TxMsg.SeqNum = 0;
      TxMsg.TimeStamp.t_sec = 0;
      TxMsg.TimeStamp.t_nsec = 0;

      TxMsg.Service = E_SDB_SUBMIT_1;
      TxMsg.DataLen = sizeof(NumElts) + sizeof(eSdbDatum_t) * NumElts;
      NumElts = htonl(NumElts);
      memcpy(TxData, &NumElts, sizeof(NumElts));
      NumElts = ntohl(NumElts);

      for(n = 0; n < NumElts; n++)
      {
         Datum.SourceId = htonl(ReqSrc);
         Datum.DatumId = htonl(ReqDat);
         Datum.Units = htonl(E_SDB_NO_UNITS);

         TimeDiff.t_sec = n - NumElts;
         TimeDiff.t_nsec = 0;
         eTimSum(&TimeNow, &TimeDiff, &(Datum.Msrment.TimeStamp));
         Datum.Msrment.TimeStamp.t_sec  = htonl(Datum.Msrment.TimeStamp.t_sec);
         Datum.Msrment.TimeStamp.t_nsec = htonl(Datum.Msrment.TimeStamp.t_nsec);


         /* Simple random number */
         ReqVal = ( rand() % (DATA_MAX - DATA_MIN) ) + DATA_MIN;

         /* Some sort of wierd sinusoidal with random offset */
         ReqVal = (int)
                  (

                     sin((Datum.Msrment.TimeStamp.t_sec % 6000) / 10.0)
                     * 1000.0
#if 0
//                     + sin((Datum.Msrment.TimeStamp.t_sec % 6000) / 1000.0)
//                     * 1000.0
                     + (( rand() % (DATA_MAX - DATA_MIN) ) + DATA_MIN)
#endif                  
                  );


         Datum.Msrment.Value = htonl(ReqVal);
         memcpy(
            TxData+sizeof(NumElts)+sizeof(eSdbDatum_t)*n,
            &Datum,  sizeof(Datum)
         );
      }

      printf("\nMessage packet content:\n");
      for(n = 0; n < TxMsg.DataLen; n++) {
         if(((n-4)%sizeof(eSdbDatum_t)) == 0) printf("\n");
         printf("%2.2x ", TxData[n]);
      }
      printf("\n\n");
      TxMsg.DataPtr = TxData;
  

      /* Actually send the message */
      Status = eCilSend (ROUTER_NUM, &TxMsg);
      if (Status != SYS_NOMINAL)
      {
         printf ("Send failure - %d\n", Status);
         exit(EXIT_FAILURE);
      }

      /* Prepare to receive some data */

      /* Actually receive the data */
      while(Status != E_CIL_TIMEOUT) {
         RxMsg.DataPtr = RxData;
         RxMsg.DataLen = sizeof (RxData);
         Status = eCilReceive (Timeout, &RxId, &RxMsg);
         printf("Receiving data... (%x)\n", Status);
      }


      printf("Enter another datum (y/n)?: ");
      scanf("%s", &Another);

   }
   while( (Another[0] == 'y') || (Another[0] == 'Y') );


   printf("Complete\n");
   return EXIT_SUCCESS;

}  /* End of main() */


/* EOF */
