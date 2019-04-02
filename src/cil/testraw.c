/*
** Module Name:
**    testraw.c
**
** Purpose:
**    Test program for raw UDP comms via CIL.
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testraw.c,v 0.1 2000/06/02 15:45:06 djm Exp $
**
** History:
**    $Log: testraw.c,v $
**    Revision 0.1  2000/06/02 15:45:06  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <stdlib.h>

#include "TtlSystem.h"
#include "Cil.h"

/* Symbolic constants */

#define MY_MAP   "/opt/ttl/src/cil/Cil.map"
#define MY_ID    E_CIL_TU4
#define DEST_ID  E_CIL_LOG


main()
{
   /* Local variables */
   Status_t Status;          /* Function return values */
   char Data[] = "SYS: <0> Test message";   


   /* Intro title */
   printf("TESTRAW - tests raw packet transmission via the SDB\n");

   /* Setup the CIL port */
   Status = eCilSetup(MY_MAP, MY_ID);
   if(Status != SYS_NOMINAL)
   {
      printf("Failed to setup CIL\n");
      exit( EXIT_FAILURE );
   }

   /* Actually send the data */
   Status = eCilRawSend(DEST_ID, sizeof(Data), Data);
   if(Status != SYS_NOMINAL)
   {
      printf("Error (0x%x) from eCilRawSend()\n", Status);
      exit( EXIT_FAILURE );
   }

   printf("Program complete\n");
   exit( EXIT_SUCCESS );


}  /* End of main() */
