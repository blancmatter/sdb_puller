/*
** Module Name:
**    HtiFixRecLen.c
**
** Purpose:
**    Ensure all the records in a text file are the same length, trailing
**    spaces are appended.
**
** Description:
**    ...
**
** Authors:
**    asp: Andrzej S. Piascik
**
** Version:
**    $Id: HtiFixRecLen.c,v 0.3 2005/06/20 13:29:34 mjf Exp $
**
** History:
**    $Log: HtiFixRecLen.c,v $
**    Revision 0.3  2005/06/20 13:29:34  mjf
**    Tidied to aid porting to Linux - no functional changes.
**
**    Revision 0.2  2001/01/15 15:33:27  mjf
**    Removal of CR/LF and replacing with LF only.
**
**    Revision 0.1  2001/01/02 14:30:08  mjf
**    Initial revision.
**
**
*/


/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[])
{
/*
** Function Name:
**    main
**
** Type:
**    int
**
** Purpose:
**    Ensure all the records in a text file are the same length, trailing
**    spaces are appended.
**
** Description:
**    Each file record is read to determine the longest record length.
**    The file is re-read and each record is written out with trailing
**    spaces appended
**    
** Arguments:
**    int argc                 (in)
**       Number of arguments on the command line (including the
**       executable name).
**
**    char *argv[]             (in)
**       Array of null-terminated character strings containing
**       the command line arguments.
**
** Authors:
**    asp: Andrzej S. Piascik
**
** History:
**    07-Sep-2000 asp Initial creation.
**
*/

#define MAX_LINE 255
    
  char  File[ FILENAME_MAX + 1 ]; /* The file name                    */
  char  Line[ MAX_LINE     + 1 ]; /* Current text line read from file */

  int   MaxLen; /* Length of longest record */
  int   Len;    /* Length of current record */

  FILE *Fp;     /* Pointer to open file     */

  if ( argc <= 1 ) /* Check if sufficient arguments supplied */
  {
    fprintf( stderr, "Space-pad all records to be the same length\n" );
    fprintf( stderr, "Usage:-\n" );
    fprintf( stderr, "  %s  input-file\n", argv[ 0 ] );
    exit( EXIT_FAILURE );
  }

  /* Copy the input args into something more comfortable */
  strcpy( File, argv[ 1 ] );

  /* Attempt to open the source file */
  if ( !(Fp = fopen( File, "r" ) ) )
  {
    fprintf( stderr, "Failed to open input file %s\n", File );
    exit( EXIT_FAILURE ); 
  }

  /* Read records and determine the maximum record length */ 
  MaxLen = 0;
  while ( fgets( Line, MAX_LINE, Fp ) )
  {
    Len = strlen( Line );
    if ( Len > MaxLen )
      MaxLen = Len;
  }

  /* Set file pointer to start and write out space-padded records */
  rewind( Fp );
  while( fgets( Line, MAX_LINE, Fp ) )
  {
    /* Remove the newline char */
    Line[ strlen( Line ) - 1 ] = '\0';

    /* Space pad to the required length */    
    printf( "%-*s\n", MaxLen, Line );
  }

  fclose( Fp );

  return EXIT_SUCCESS;
}
