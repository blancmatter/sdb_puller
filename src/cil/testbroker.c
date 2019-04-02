/*
** TESTBROKER
**
** A simple program to test Cil functions. If receives messages from any
** source, and retransmits it.
**
**
** PLW & DJM (TTL)
**
** 27-Mar-2000
*/


#include <TtlSystem.h>
#include <Cil.h>

#include <stdio.h>
#include <string.h>


char MyMap[] = "./Cil.map";

int main (int argc, char *argv[])
{
   Status_t MyStatus;
   Int32_t RxId;
   eCilMsg_t Msg;
   char Data[1024];
   int Timeout = 5000;
   int Count1 = 0;
   int Count2 = 0;
   int Count3 = 0;
   int Count4 = 0;


   printf ("CIL test message broker\n");

   /*
   ** ASSOCIATE OUT "CODES" WITH CIL-MAP ENTRIES
   */


   MyStatus = eCilSetup (MyMap, E_CIL_TMB);
   if (MyStatus != SYS_NOMINAL)
   {
      printf ("Setup failure - %d\n", (MyStatus && 0xffff));
      return (1);
   }


   /*
   ** GO INTO THE MAIN LOOP
   */

   for (;;)
   {

      /* Allocate some space for the received message */
      Msg.DataPtr = Data;
      Msg.DataLen = sizeof (Data);

      /* Wait for a message */
      MyStatus = eCilReceive (Timeout, &RxId, &Msg);

      if (MyStatus == SYS_NOMINAL)
      {
         /* Increment some counters based on where the message source */
         switch (Msg.SourceId)
         {
            case E_CIL_TU1:
               Count1++;
               break;
            case E_CIL_TU2:
               Count2++;
               break;
            case E_CIL_TU3:
               Count3++;
               break;
            default:
               Count4++;
               break;
         }

         /* Print these to screen, to see what is going on */
         printf ("\r %d : %d : %d : %d", Count1, Count2, Count3, Count4);

         /* Send message back again! */
         MyStatus = eCilSend (Msg.DestId, &Msg);
         if (MyStatus != SYS_NOMINAL)
         {
            printf ("\nSend failure - %x\n", MyStatus);
            return (3);
         }
      }
      else if (MyStatus == E_CIL_TIMEOUT)
      {
         printf ("\nReceive timeout");
      }
      else
      {
         printf ("Receive failure - %x\n", MyStatus);
         return (4);
      }
   }
}  /* End of main() */


/* EOF */
