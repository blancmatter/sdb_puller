
#include <stdio.h>

#include "Wfl.h"

int main()
{

   eWflTimedPulse_t T;
   eWflTimedPulse_t S;

   Status_t         Status;
   char             Key;
   eWflPulseId_t    PulseId;


   T.ValSecs = 0;
   T.ValNSecs = 100000000;
   T.IntSecs = 0;
   T.IntNSecs = 100000000;


   S.ValSecs = 0;
   S.ValNSecs = 200000000;
#if 1
   S.IntSecs = 0;
   S.IntNSecs = 200000000;
#else
   S.IntSecs = 0;
   S.IntNSecs = 0;
#endif

   Status = eWflCreateTimedPulse( &T );

   if ( Status != SYS_NOMINAL )
   { 
      printf( "Unable to create timed pulse 1, error = 0x%x\n", Status );
   }


   Status = eWflCreateTimedPulse( &S );

   if ( Status != SYS_NOMINAL )
   { 
      printf( "Unable to create timed pulse 2, error = 0x%x\n", Status );
   }


   for ( ;; )
   {

      Status = eWflReceivePulse( &PulseId );

      if ( Status != SYS_NOMINAL )
      { 
         printf( "Error receiving pulse, Status = 0x%x\n", Status );
      }

      if ( PulseId == T.PulseId )
      {
         printf( "Got pulse 1 (timer = %f \n" , (double)(T.ValNSecs)/1E9 );

      }

      if ( PulseId == S.PulseId )
      {
         printf( "Got pulse 2 (timer = %f \n" , (double)(S.ValNSecs)/1E9 );

      }

      if ( eWflKeyboardHit() )
      {
         Key = eWflGetKeyPress();
         if ( (Key == 'u') || (Key == 'U') )
         {
            T.ValNSecs *= 2;
            T.IntNSecs *= 2;
            Status = eWflUpdateTimer( &T );
         }
         else if (( Key =='d' ) || ( Key == 'D' ) )
         {
            T.ValNSecs /= 2;
            T.IntNSecs /= 2;
            Status = eWflUpdateTimer( &T );

         }
         else if (( Key =='x' ) || ( Key == 'X' ) )
         {
            return 1;
         }
      }         
   }
 
}

