/*
** Module Name:
**    testprog.c
**
** Purpose:
**    A program to test all the functions in the TIM package.
**
** Description:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: testprog.c,v 0.13 2004/01/15 14:36:49 sxf Exp $
**
** History:
**    $Log: testprog.c,v $
**    Revision 0.13  2004/01/15 14:36:49  sxf
**    Updated for Neutrino.
**
**    Revision 0.12  2001/11/20 11:31:21  mjf
**    Change of name of added functions.
**
**    Revision 0.11  2001/11/20 09:31:55  mjf
**    Implementation of functionality to cache the time to prevent the need
**    for time-consuming re-reads of the system clock.
**
**    Revision 0.10  2001/01/18 16:55:32  djm
**    Added test of the new eTimToDouble() function.
**
**    Revision 0.9  2000/11/16 09:54:37  djm
**    Minor change to fix problem with printing negative times.
**
**    Revision 0.8  2000/11/01 15:34:57  asp
**    Port to Linux.
**
**    Revision 0.7  2000/07/07 10:21:35  djm
**    Added test to show "zero time".
**
**    Revision 0.6  2000/06/16 15:21:37  rdl
**    Added time structure copying functions: 'eTimNearCopy' and 'eTimExtCopy'.
**
**    Revision 0.5  2000/06/15 14:25:26  rdl
**    Added test cases for:
**    1. New copying functions (eTimCopy and eTimFarCopy).
**    2. Additional tests for invalid arguments.
**
**    Revision 0.4  2000/06/02 13:11:21  djm
**    Added tests for the eTimDifference() and eTimSum() functions.
**
**    Revision 0.3  2000/05/31 08:43:27  djm
**    Added tests for eTimCompare().
**
**    Revision 0.2  2000/05/30 12:41:53  djm
**    Fixed syntax error in an error reporting line.
**
**    Revision 0.1  2000/05/26 08:11:54  djm
**    Initial creation.
**
**
*/

/* Include files */

#include <stdio.h>
#include <stdlib.h>

#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Tim.h"


int main
(
   int argc,
   char *argv[]
)
/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    Top level function of TIM test program.
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
**    30-may-2000 djm Added extra tests.
**    25-May-2000 mkb Initial creation.
**
*/
{
   Status_t Status;          /* Return value from functions */
   eTtlTime_t *NullTimePtr;  /* Structure for holding a time */
   eTtlTime_t Time;          /* Structure for holding a time */
   eTtlTime_t TimeDiff;      /* Structure for holding a time duration */
   eTtlTime_t TimeA;         /* Structure for holding a time */
   eTtlTime_t TimeB;         /* Structure for holding a time */
   eTtlTime_t SrcTime;       /* Structure for holding a time */
   eTtlTime_t DestTime;      /* Structure for holding a time */
   eTtlTime_t *DestTime2;    /* Structure for holding a time */
/* QNX4 specific code so exclude Neutrino */
#ifndef __QNXNTO__
#ifdef __QNX__
   eTtlTime_t __far DestTimeFar;        /* Structure for holding a time */
   eTtlTime_t __far DestTimeFar2;       /* Structure for holding a time */
   eTtlTime_t __far *DestTimeFarPtr;    /* Structure for holding a time */
   eTtlTime_t __far *DestTimeFarPtr2;   /* Structure for holding a time */
#endif /* __QNX__ */
#endif
   int Duration;             /* A delta-time specified in seconds */
   int Increment;            /* A delta-time specified in seconds */
   char String[80];          /* A string buffer for storing ASCII time */
   int Diff;                 /* Return result from eTimCompare() */
	char  *NullStrPtr ;       /* a pointer to a null string */ 
   double Value;             /* Floating point storage variable */

   /* Print starting message */
   printf("TIM - TTL SYSTEM TIME LIBRARY TEST PROGRAM\n\n");


   /* TEST 1 - Sleep for a specified number of seconds */
   printf("\n***** TEST 1 - eTimSleep() *****\n");
   Duration = 1;
   printf("Sleeping for %d seconds... ", Duration);
   fflush(stdout);
   Status = eTimSleep(Duration);
   if(Status != SYS_NOMINAL)
   {
      printf("Error (0x%x)\n", Status);
   }
   else
   {
      printf("DONE!\n");
   }
   fflush(stdout);


   /* Second test basic time recovery */
   printf("\n***** TEST 2 - eTimGetTime() *****\n");
   Status = eTimGetTime(&Time);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
   else
   {
      printf("eTimGetTime() function completed. Results are:\n");
      printf("Seconds     = %d\n",    Time.t_sec);
      printf("Nanoseconds = %9.9d\n", Time.t_nsec);
   }

   /* Second test basic time recovery */
   printf("\n***** TEST 2A - eTimGetTime() - Invalid Arguments *****\n");
	NullTimePtr = NULL ;
   Status = eTimGetTime(NullTimePtr);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
   else
   {
      printf("eTimGetTime() function completed. Results are:\n");
      printf("Seconds     = %d\n",    NullTimePtr->t_sec);
      printf("Nanoseconds = %9.9d\n", NullTimePtr->t_nsec);
   }

   /* TEST 3 - Print out a time in "ASCII" format */
   printf("\n***** TEST 3 - eTimToString() *****\n");
   Status = eTimToString(&Time, 80, String);
   if(Status != SYS_NOMINAL)
   {
      printf("A problem occurred during the string conversion\n");
   }
   else
   {
      printf("The string output (of Test 3) is \"%s\"\n", String);
   }

   /* TEST 3A - Print out a time in "ASCII" format */
   printf("\n***** TEST 3A - eTimToString()  - Invalid Arguments *****\n");
   Status = eTimToString(NullTimePtr, 80, String);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimToString() function failed with error code 0x%x\n", Status);
   }
   else
   {
      printf("The string output (of Test 3) is \"%s\"\n", String);
   }

   /* TEST 3B - Print out a time in "ASCII" format */
   printf("\n***** TEST 3B - eTimToString()  - Invalid Arguments *****\n");
   Status = eTimToString(&Time, 21, String);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimToString() function failed with error code 0x%x\n", Status);
   }
   else
   {
      printf("The string output (of Test 3) is \"%s\"\n", String);
   }

   /* TEST 3C - Print out a time in "ASCII" format */
   printf("\n***** TEST 3C - eTimToString()  - Invalid Arguments *****\n");
   NullStrPtr = NULL ;
   Status = eTimToString(&Time, 80, NullStrPtr);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimToString() function failed with error code 0x%x\n", Status);
   }
   else
   {
      printf("The string output (of Test 3) is \"%s\"\n", String);
   }

   /* TEST 4 - Check addition of times for timeout generation */
   printf("\n***** TEST 4 - eTimGetTimeOut() *****\n");
   Increment = 5;
   printf("Adding %d seconds to current time\n", Increment);
   Status = eTimGetTimeOut(Increment, &Time);
   if(Status != SYS_NOMINAL)
   {
      printf("A problem occurred during the eTimGetTimeOut function\n");
   }
   else
   {
      Status = eTimToString(&Time, 80, String);
      printf("The string output is \"%s\"\n", String);
   }


   /* TEST 5 - Difference between two times */
   printf("\n***** TEST 5 - eTimDifference() *****\n");
   Duration = 1;
   printf("Pausing for about %d seconds\n", Duration);
   fflush(stdout);
   Status = eTimGetTime(&TimeA);
   Status = eTimSleep(Duration);
   Status = eTimGetTime(&TimeB);
   Status = eTimDifference(&TimeA, &TimeB, &TimeDiff);
   if(Status != SYS_NOMINAL)
   {
      printf("A problem occurred during difference function\n");
   }
   else
   {
      printf("T2 - T1 = %.9f seconds\n",
         TimeDiff.t_sec + TimeDiff.t_nsec/ E_TTL_NANOSECS_PER_SEC);
   }
   Status = eTimDifference(&TimeB, &TimeA, &TimeDiff);
   if(Status != SYS_NOMINAL)
   {
      printf("A problem occurred during difference function\n");
   }
   else
   {
      printf("T1 - T2 = %.9f seconds\n",
         TimeDiff.t_sec + TimeDiff.t_nsec/ E_TTL_NANOSECS_PER_SEC);
   }


   /* TEST 6 - Test time comparison */
   printf("\n***** TEST 6 - eTimCompare() *****\n");
   printf("---------------------\n");
   TimeA = TimeB = Time;
   Status = eTimToString(&TimeA, 80, String);
   printf("TimeA = %s (%d.%9.9d)\n", String, TimeA.t_sec, TimeA.t_nsec);
   Status = eTimToString(&TimeB, 80, String);
   printf("TimeB = %s (%d.%9.9d)\n", String, TimeB.t_sec, TimeB.t_nsec);
   Diff = eTimCompare(&TimeA, &TimeB);
   printf("Result of eTimCompare(A,B) = %d\n", Diff);
   printf("---------------------\n");
   TimeA.t_nsec++;
   Status = eTimToString(&TimeA, 80, String);
   printf("TimeA = %s (%d.%9.9d)\n", String, TimeA.t_sec, TimeA.t_nsec);
   Status = eTimToString(&TimeB, 80, String);
   printf("TimeB = %s (%d.%9.9d)\n", String, TimeB.t_sec, TimeB.t_nsec);
   Diff = eTimCompare(&TimeA, &TimeB);
   printf("Result of eTimCompare(A,B) = %d\n", Diff);
   printf("---------------------\n");
   TimeB.t_nsec += 2;
   Status = eTimToString(&TimeA, 80, String);
   printf("TimeA = %s (%d.%9.9d)\n", String, TimeA.t_sec, TimeA.t_nsec);
   Status = eTimToString(&TimeB, 80, String);
   printf("TimeB = %s (%d.%9.9d)\n", String, TimeB.t_sec, TimeB.t_nsec);
   Diff = eTimCompare(&TimeA, &TimeB);
   printf("Result of eTimCompare(A,B) = %d\n", Diff);
   printf("---------------------\n");
   TimeA = TimeB = Time;
   TimeA.t_sec++;
   Status = eTimToString(&TimeA, 80, String);
   printf("TimeA = %s (%d.%9.9d)\n", String, TimeA.t_sec, TimeA.t_nsec);
   Status = eTimToString(&TimeB, 80, String);
   printf("TimeB = %s (%d.%9.9d)\n", String, TimeB.t_sec, TimeB.t_nsec);
   Diff = eTimCompare(&TimeA, &TimeB);
   printf("Result of eTimCompare(A,B) = %d\n", Diff);
   printf("---------------------\n");
   TimeB.t_sec += 2;
   Status = eTimToString(&TimeA, 80, String);
   printf("TimeA = %s (%d.%9.9d)\n", String, TimeA.t_sec, TimeA.t_nsec);
   Status = eTimToString(&TimeB, 80, String);
   printf("TimeB = %s (%d.%9.9d)\n", String, TimeB.t_sec, TimeB.t_nsec);
   Diff = eTimCompare(&TimeA, &TimeB);
   printf("Result of eTimCompare(A,B) = %d\n", Diff);
   printf("---------------------\n");




   /* TEST 7 - Adding to a time */
   printf("\n***** TEST 7 - eTimSum() *****\n");
   Duration = 1;
   printf("Adding %d.%9.9d seconds to TimeA to get TimeB.\n",
          TimeDiff.t_sec, TimeDiff.t_nsec);
   fflush(stdout);
   Status = eTimGetTime(&TimeA);
   Status = eTimSum(&TimeA, &TimeDiff, &TimeB);
   if(Status != SYS_NOMINAL)
   {
      printf("A problem occurred during sum function\n");
   }
   else
   {
      Status = eTimToString(&TimeA, 80, String);
      printf("TimeA = %s (%d.%9.9d)\n", String, TimeA.t_sec, TimeA.t_nsec);
      Status = eTimToString(&TimeB, 80, String);
      printf("TimeB = %s (%d.%9.9d)\n", String, TimeB.t_sec, TimeB.t_nsec);
   }


   /* TEST 8 - Performing time copying */
   printf("\n***** TEST 8 - eTimCopy() *****\n");
   printf("Copying Source Time to Destination Time \n");
   Status = eTimGetTime(&SrcTime);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	
	Status = eTimCopy (SrcTime, &DestTime) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&SrcTime, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, SrcTime.t_sec, SrcTime.t_nsec);
      Status = eTimToString(&DestTime, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTime.t_sec, DestTime.t_nsec);
   }
	
   printf("\n***** TEST 8A - eTimCopy() *****\n");
   printf("Copying Source Time to Destination Time - Invalid Arguments \n");
   Status = eTimGetTime(&SrcTime);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	DestTime2 = NULL ;
	Status = eTimCopy (SrcTime, DestTime2) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&SrcTime, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, SrcTime.t_sec, SrcTime.t_nsec);
      Status = eTimToString(&DestTime, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTime.t_sec, DestTime.t_nsec);
   }
	
#ifndef __QNXNTO__
#ifdef __QNX__	
   /* TEST 9 - Performing time far copying */
   Duration = 3;
   Status = eTimSleep(Duration);
	
   printf("\n***** TEST 9 - eTimFarCopy() *****\n");
   printf("Copying Source Time to Destination Time \n");
   Status = eTimGetTime(&SrcTime);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	
	Status = eTimFarCopy (SrcTime, &DestTimeFar) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimFarCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&SrcTime, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, SrcTime.t_sec, SrcTime.t_nsec);
      Status = eTimToString(&DestTimeFar, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTimeFar.t_sec, DestTimeFar.t_nsec);
   }

	
   /* TEST 9 - Performing time far copying */
   Duration = 3;
   Status = eTimSleep(Duration);
	
   printf("\n***** TEST 9A - eTimFarCopy() *****\n");
   printf("Copying Source Time to Destination Time - Invalid Arguments \n");
   Status = eTimGetTime(&SrcTime);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	DestTimeFarPtr = NULL ;
	Status = eTimFarCopy (SrcTime, DestTimeFarPtr) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimFarCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&SrcTime, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, SrcTime.t_sec, SrcTime.t_nsec);
      Status = eTimToString(&DestTimeFar, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTimeFar.t_sec, DestTimeFar.t_nsec);
   }

	
   /* TEST 10 - Performing near time copying */
   printf("\n***** TEST 10 - eTimNearCopy() *****\n");
   printf("Copying Source Time to Destination Time \n");
   Status = eTimGetTime(&DestTimeFar);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	
	Status = eTimNearCopy (DestTimeFar, &DestTime) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimNearCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&DestTimeFar, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, DestTimeFar.t_sec, DestTimeFar.t_nsec);
      Status = eTimToString(&DestTime, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTime.t_sec, DestTime.t_nsec);
   }
	
   printf("\n***** TEST 10A - eTimCopy() *****\n");
   printf("Copying Source Time to Destination Time - Invalid Arguments \n");
   Status = eTimGetTime(&DestTimeFar);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	DestTime2 = NULL ;
	Status = eTimNearCopy (DestTimeFar, DestTime2) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimNearCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&DestTimeFar, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, DestTimeFar.t_sec, DestTimeFar.t_nsec);
      Status = eTimToString(&DestTime, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTime.t_sec, DestTime.t_nsec);
   }


   /* TEST 11 - Performing time extended copying */
   Duration = 2;
   Status = eTimSleep(Duration);
	
   printf("\n***** TEST 11 - eTimExtCopy() *****\n");
   printf("Copying Source Time to Destination Time \n");
   Status = eTimGetTime (&DestTimeFar);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	
	Status = eTimExtCopy (DestTimeFar,
	                      &DestTimeFar2) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimExtCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&DestTimeFar, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, DestTimeFar.t_sec, DestTimeFar.t_nsec);
      Status = eTimToString(&DestTimeFar2, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTimeFar2.t_sec, DestTimeFar2.t_nsec);
   }


   /* TEST 11 - Performing time extended copying */
   Duration = 3;
   Status = eTimSleep(Duration);
	
   printf("\n***** TEST 11A - eTimExtCopy() *****\n");
   printf("Copying Source Time to Destination Time - Invalid Arguments \n");
   Status = eTimGetTime(&DestTimeFar);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
	DestTimeFarPtr2 = NULL ;
	Status = eTimExtCopy (DestTimeFar, DestTimeFarPtr2) ;
   if(Status != SYS_NOMINAL)
   {
      printf("eTimExtCopy() function failed with error code 0x%x\n", Status);
   }
   else
   {
      Status = eTimToString(&DestTimeFar, 80, String);
      printf("Source Time      = %s (%d.%9.9d)\n", 
	          String, DestTimeFar.t_sec, DestTimeFar.t_nsec);
      Status = eTimToString(&DestTimeFar2, 80, String);
      printf("Destination Time = %s (%d.%9.9d)\n", 
	   String, DestTimeFar2.t_sec, DestTimeFar2.t_nsec);
   }
#endif /* __QNX__ */ 
#endif /* __QNXNTO__ */


   printf("\n***** TEST 12 Zero time *****\n");
   Time.t_sec = 0;
   Time.t_nsec = 0;
   Status = eTimToString(&Time, 80, String);
   printf("Time {0,0} = %s\n", String); 


   printf("\n***** TEST 13 conversion to double *****\n");
   Status = eTimGetTime(&Time);
   if(Status != SYS_NOMINAL)
   {
      printf("eTimGetTime() function failed with error code 0x%x\n", Status);
   }
   else
   {
      printf("Seconds        = %d\n",    Time.t_sec);
      printf("Nanoseconds    = %9.9d\n", Time.t_nsec);

      Status = eTimToDouble(&Time, &Value);
      if(Status != SYS_NOMINAL)
      {
         printf("eTimToDouble() function failed with error code 0x%x\n", Status);
      }
      else
      {
         printf("Storage size   = %d\n", sizeof(Value));
         printf("Floating Point = %.12lf\n", Value);
      }
   }

   /* TEST 14 - Test time caching */
   printf("\n***** TEST 14 time caching *****\n");

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimCacheTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimCacheTime   = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimCacheTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimCacheTime   = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimUncacheTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimUncacheTime = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );

   Status = eTimGetTime( &Time );
   Status = eTimToString( &Time, 80, String );
   printf( "Time from eTimGetTime     = %s\n", String ); 
   Status = eTimSleep( 1 );


   printf("\nTests complete.\n");
   return EXIT_SUCCESS;

}  /* End of main() */


/* EOF */
