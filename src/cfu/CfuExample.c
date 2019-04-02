/*
** Module Name:
**    CluExample.c
**
** Purpose:
**    Example of use of Configuration File Utilities (Cfu).
**
** Description:
**    ...
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** Version:
**    $Id: CfuExample.c,v 0.4 2000/11/07 12:22:03 mjf Exp $
**
** History:
**    $Log: CfuExample.c,v $
**    Revision 0.4  2000/11/07 12:22:03  mjf
**    Checking in of changes done by DJM.
**
**    Revision 0.3  2000/08/17 16:37:47  mjf
**    Use Status_t and SYS_NOMINAL to return status.
**
**    Revision 0.2  2000/08/09 14:14:33  mjf
**    Addition of functions to test exsistence of a parameter, and at a position,
**    and retrieve the number of parameters on a line.
**
**    Revision 0.1  2000/08/09 09:08:45  mjf
**    Initial revision.
**
**
*/


/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
** System include files
*/

#include "TtlSystem.h"
#include "Cfu.h"


/*
** Macro definitions
*/



/*
** Global variables
*/



/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    Main and only function for CFU example code.
**
** Description:
**    Sets up the CFU, then retrieves lines from the configuration file until 
**    EOF. Completes this usage and sets up again to use the same file. Then 
**    retrieves parameters from each line in the file until EOF. Finally 
**    completes usage of the file.
**
** Arguments:
**    int argc                         (in)
**       Number of arguments on the command-line including executable name.
**    char *argv[]                     (in)
**       Array of null-terminated character strings containing arguments.
**
** Authors:
**    mjf : Martyn J. Ford (TTL)
**
** History:
**    08-Aug-2000 mjf Initial creation.
**
*/

int main ( int argc, char *argv[] )
{
   /* Local variables */
   Status_t CfuStatus;                 /* status returned of CFU functions */
   Status_t ParamStatus;               /* status returned of CFU get param */
   int      Position;                  /* position of parameter within line */
   int      ParamNumber;               /* number of parameter to retrieve */
   char   * FilenamePtr;               /* pointer to configuration file name */
   char     Buffer[ E_CFU_STRING_LEN ];/* buffer used to retrieve text into */


   /* check number of arguments supplied */
   if ( argc == 1 )
   {
      printf( "Usage :-\nCfuExample <config file>\n\n" );
      return 0;
   }

   /* point to name of configuration file */
   FilenamePtr = argv[ 1 ];

   /* register with the configuration file utilities (CFU) */
   CfuStatus = eCfuSetup( FilenamePtr );
   
   /* if setup was successful, read lines from file */
   if ( CfuStatus == SYS_NOMINAL )
   {
      printf( "\nLines from configuration file '%s' :-\n", FilenamePtr );

      /* loop until no lines left to read */
      do
      {
         /* retrieve a line from the configuration file */
         CfuStatus = eCfuGetLine( Buffer );

         /* if line retrieved OK, then print it */
         if ( CfuStatus == SYS_NOMINAL )
         {
            printf( " line     : '%s'\n", Buffer );
         }
      }
      while ( CfuStatus == SYS_NOMINAL );
   }
   /* else possible problem opening file */
   else
   {
      printf( "Error : Problem opening configuration file '%s'\n", 
              FilenamePtr );
   }

   /* complete use of configuration file */
   CfuStatus = eCfuComplete();

   /* register with the configuration file utilities (CFU) */
   CfuStatus = eCfuSetup( FilenamePtr );
   
   /* if setup was successful, read parameters from file */
   if ( CfuStatus == SYS_NOMINAL )
   {
      printf( "\nParameters with totals from configuration file '%s' :-\n", 
              FilenamePtr );

      /* loop until no lines left to read */
      do
      {
         /* retrieve a line from the configuration file */
         CfuStatus = eCfuGetLine( NULL );

         if ( CfuStatus == SYS_NOMINAL )
         {
            printf( " param %2d :", eCfuParamCount() );
         }

         /* loop until no parameters left on line */
         do
         {
            /* retrieve a parameter from the configuration file */
            ParamStatus = eCfuGetParam( Buffer );

            /* if param retrieved OK, then print it */
            if ( ParamStatus == SYS_NOMINAL )
            {
               printf( " '%s'", Buffer );
            }
         }
         while( ParamStatus == SYS_NOMINAL );

         /* output blank line */
         printf( "\n\n" );
      }
      while ( CfuStatus == SYS_NOMINAL );
   }
   /* else possible problem opening file */
   else
   {
      printf( "Error : Problem opening configuration file '%s'\n", 
               FilenamePtr );
   }

   /* complete use of configuration file */
   CfuStatus = eCfuComplete();

   /* register with the configuration file utilities (CFU) */
   CfuStatus = eCfuSetup( FilenamePtr );
   
   /* if setup was successful, read param 1 from line 1, 2 from 2 etc */
   if ( CfuStatus == SYS_NOMINAL )
   {
      printf( "\nParameters with index from configuration file '%s' :-\n", 
              FilenamePtr );

      /* reset parameter number */
      ParamNumber = 1;

      /* loop until no lines left to read, or numbered param doesn't exist */
      do
      {
         /* retrieve a line from the configuration file */
         CfuStatus = eCfuGetLine( NULL );

         /* retrieve parameter by number from the configuration file */
         ParamStatus = eCfuGetNumParam( ParamNumber, Buffer );

         /* if param retrieved OK, then print it */
         if ( ParamStatus == SYS_NOMINAL )
         {
            printf( " param %2d : '%s', ", ParamNumber, Buffer );
            /* if odd number, check param's existence */
            if ( ParamNumber % 2 )
            {
               /* alter parameter and check for existence */
               ( Buffer[ 0 ] )++;
               printf( "test '%s' - ", Buffer );
               if ( eCfuTestParam( Buffer, &Position ) == E_CFU_PARAM_MATCHED )
                  printf( "found at %d, ", Position );
               else
                  printf( "absent, " );
               /* restore parameter and check for existence */
               ( Buffer[ 0 ] )--;
               printf( "test '%s' - ", Buffer );
               if ( eCfuTestParam( Buffer, &Position ) == E_CFU_PARAM_MATCHED )
                  printf( "found at %d, ", Position );
               else
                  printf( "absent, " );
               printf( "\n" );
            }
            /* if even number, check at specific position */
            else
            {
               /* check for existence at previous position */
               Position = ParamNumber - 1;
               printf( "test '%s' at %d - ", Buffer, Position );
               if ( eCfuTestNumParam( Buffer, Position ) == E_CFU_PARAM_MATCHED )
                  printf( "found, " );
               else
                  printf( "absent, " );
               /* restore position to check at */
               Position = ParamNumber;
               printf( "test '%s' at %d - ", Buffer, Position );
               if ( eCfuTestNumParam( Buffer, Position ) == E_CFU_PARAM_MATCHED)
                  printf( "found, " );
               else
                  printf( "absent, " );
               printf( "\n" );


            }
         }

         /* increment the param number */
         ParamNumber++;
      }
      while ( ( CfuStatus == SYS_NOMINAL ) && ( ParamStatus == SYS_NOMINAL ) );
   }
   /* else possible problem opening file */
   else
   {
      printf( "Error : Problem opening configuration file '%s'\n", 
              FilenamePtr );
   }

   /* complete use of configuration file */
   CfuStatus = eCfuComplete();

   return 0;

}


/*
** EOF
*/
