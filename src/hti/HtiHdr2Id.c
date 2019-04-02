/*
** Module Name:
**    HtiHdr2Id.c
**
** Purpose:
**    Read header files and extract the Sdb data definitions and descriptions
**    and generate a C programme to print them out.
**
** Description:
**    ...
**
** Authors:
**    asp: Andrzej S. Piascik
**
** Version:
**    $Id: HtiHdr2Id.c,v 0.15 2007/07/13 10:57:17 mjf Exp $
**
** History:
**    $Log: HtiHdr2Id.c,v $
**    Revision 0.15  2007/07/13 10:57:17  mjf
**    Addition of '-verbatim' switch to inhibit any changes made to label
**    names based on inherent rules.
**
**    Revision 0.14  2007/03/20 15:46:49  mjf
**    Fix to prevent spurious semi-colons when not in MySQL mode.
**
**    Revision 0.13  2007/03/12 15:22:59  sxf
**    Minor modification to build under linux.
**
**    Revision 0.12  2005/06/20 13:29:34  mjf
**    Tidied to aid porting to Linux - no functional changes.
**
**    Revision 0.11  2002/10/15 13:52:38  dws
**    Added commandline swtich  -package if this is specificied then the
**    package is converted to its numerical value and not just added as a
**    string
**
**    Revision 0.10  2001/10/02 15:30:39  mjf
**    Pre-prend the TLA onto the start of identifiers 'STOP_DEMAND', 'STOPPED'
**    and 'HOME'.
**
**    Revision 0.9  2001/09/19 12:47:57  mjf
**    Create source file including 'Htg.h' rather than 'HtiAllPkg.h'.
**
**    Revision 0.8  2001/08/21 09:18:34  mjf
**    Also pre-pend package ID onto 'HALT_DEMAND'.
**
**    Revision 0.7  2001/08/14 13:14:37  mjf
**    If identifier starts with 'STATE_', pre-pend the package name.
**
**    Revision 0.6  2001/06/19 10:23:14  mjf
**    Add the TLA onto the start of the label if the label is STATE,
**    AUTH_STATE, PROC_STATE, ENABLE, DEMAND or ACTUAL, or if the label
**    contains the text VERSION or SHUTDOWN.
**
**    Revision 0.5  2001/02/06 14:25:25  asp
**    Single level of #ifdef allowed and can be selected
**
**    Revision 0.4  2001/01/15 15:33:27  mjf
**    Removal of CR/LF and replacing with LF only.
**
**    Revision 0.3  2001/01/14 14:16:43  mjf
**    Return 0 rather than 1 - caused failure when called from Makefile.
**
**    Revision 0.2  2001/01/02 16:38:14  mjf
**    Use header file "HtiAllPkg.h" to include all package headers.
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
**    Read header files and extract the Sdb data definitions and descriptions
**    and generate a C programme to print them out.
**
** Description:
**    The programme generates code to print the package, SDB ID.
**    short name and description for each datum.
**    The arguments should change in the following order
**    
**    start
**      The programme will output the starting code
**
**    head file location
**    package name, 
**    enumeration typdef to search for
**      These would normally be done for each package
**      the programme generates code ( printf's ) 
**   
**    end
**       The programme generates the programme end
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
**    asp: Andrzej S. Piascik
**
** History:
**    04-Sep-2000 asp Initial creation.
**
*/

#define MAX_LINE 255
#define TRUE  1
#define FALSE 0
#define USEPACKAGE "-package"
#define SQLPACKAGE "-sql"
#define USEVERBATIM "-verbatim"

  /* Local variables */
  char *StartCode[] = {
  "/*",
  "** Module Name:",
  "**    unknown",
  "**",
  "** Purpose:",
  "**    Print Sdb data definitions and descriptions",
  "**",
  "** Description:",
  "**    ...",
  "**",
  "** Authors:",
  "**    anon: Automatically Generated",
  "**",
  "** Version:",
  "**    v0.0 anon",
  "**",
  "** History:",
  "**    Revision 0.0 anon",
  "**    Automatically generates",
  "**",
  "**",
  "*/",
  "",
  "/* Include files */",
  "#include <stdio.h>",
  "#include <limits.h>",
  "#include \"Htg.h\"",
  "#include \"TtlSystem.h\"",
  "",
  "int main( int argc, char* argv[] )",        
  "{",
  "/*",
  "** Function Name:",
  "**    main",
  "**",
  "** Type:",
  "**    int",
  "**",
  "** Purpose:",
  "**    Print Sdb data definitions and descriptions",
  "**",
  "** Description:",
  "**    A large number of printfs generated from data",
  "**    extracted from package headers",
  "**",
  "** Arguments:",
  "**    int argc                 (in)",
  "**       Number of arguments on the command line (including the",
  "**       executable name).",
  "**    char *argv[]             (in)",
  "**       Array of null-terminated character strings containing",
  "**       the command line arguments.",
  "**",
  "** Authors:",
  "**    anon: automatically generated",
  "**",
  "** History:",
  "**    unknown",
  "**",
  "*/",
  "",
  "/* Code starts here */",
  "",
  NULL }; 

  char *EndCode[] = {
  "  return 0;",
  "}",
  "/* End of file */",
  NULL };
    
  char  File[ FILENAME_MAX + 1 ]; /* The file name */
  char  Enum1[ FILENAME_MAX + 1 ]; /* The enumeration typdef to search for         */
  char  Enum2[ FILENAME_MAX + 1 ]; /* The enumeration typdef to search for         */

  char  Pkg [ MAX_LINE + 1 ];     /* The supplied package name                    */
  char  Line[ MAX_LINE + 1 ];     /* Current text line from the package .h file   */

  char  s1[ MAX_LINE + 1 ];       /* String segments 1-3 read from header file    */ 
  char  s2[ MAX_LINE + 1 ]; 
  char  s3[ MAX_LINE + 1 ]; 

  char *ChrPtr;                   /* General purpose pointer to char              */
  char *NamePtr;                  /* Pointer to short name of Sdb datum           */ 
  char *DescHedPtr;               /* Pointer to start oflong description of datum */
  char *DescEndPtr;               /* Pointer to end of long description           */

  char *NoSkipPtr = "";           /* Pointer to #ifdef string not to be skipped  */

  int   i;                        /* Loop counter                                 */
  int   Count;                    /* Number of string segments                    */
  int   Busy  = TRUE;             /* Not at end of file yet                       */
  int   Found = FALSE;            /* Enumeration typedef string found             */
  int   Skip  = FALSE;            /* Skip ifdef'ed lines                          */
  int   PackageID = FALSE;        /* Determines if the program is running in cil mode or package mode */
  int   SqlID = FALSE;            /* Determines if the program is running in sql dump mode */
  int   Verbatim = FALSE;         /* Determines if datum names should not be manipulated */

  int   NameLen;                  /* Length of short name string                  */
  int   DescLen;                  /* Length of long description string            */
  int   FirstTime = TRUE;         /* Only true first time through loop.           */
  FILE *Fp;                       /* Pointer to open file                         */

/* Select the appropriate ifdefs to include */
#ifdef E_TTL_LIVERPOOL
   NoSkipPtr = "E_TTL_LIVERPOOL"      
#endif
#ifdef E_TTL_IUCAA
   NoSkipPtr = "E_TTL_IUCAA"
#endif
#ifdef E_TTL_FAULKES 
   NoSkipPtr = "E_TTL_FAULKES"
#endif

  /* Print the start code block if requested*/
  if ( argc == 2 && !strcmp( argv[ 1 ], "start") )
  {
    i = 0;
    while ( StartCode[ i ] )
      printf( "%s\n", StartCode[ i++ ] );
    exit( EXIT_SUCCESS );
  }

  /* Print the trailing code block if requested */
  if ( argc == 2 && !strcmp( argv[ 1 ], "end" ) )
  {
    i = 0;
    while ( EndCode[ i ] )
      printf( "%s\n", EndCode[ i++ ] );
    exit( EXIT_SUCCESS );
  }

  if ( argc <= 3 )         /* Check if sufficient arguments supplied           */
  {
    fprintf( stderr, "Usage:-\n" );
    fprintf( stderr, "  %s  header-file package-ID, enum-ID [-package] [-sql] [-verbatim] [#ifdef-to-include]\n", argv[0] );
    fprintf( stderr, "  %s  start\n", argv[0] );
    fprintf( stderr, "  %s  end\n", argv[0] );
    exit( EXIT_FAILURE );
  }
  if (argc > 3)
  {
     /* Check if the user has specifed to use package id to do numerical conversion */
     for(i=0;i<argc;i++)
     {
        if(strcmp(argv[i],USEPACKAGE) == 0)
        {
            PackageID = TRUE; 
        }
        if (strcmp(argv[i],SQLPACKAGE) == 0)
        {
            SqlID = TRUE;
        }
        if(strcmp(argv[i],USEVERBATIM) == 0)
        {
            Verbatim = TRUE; 
        }
     }
  }

  /* Copy the input args into something more comfortable */
  strcpy( File, argv[ 1 ] );
  strcpy( Pkg,  argv[ 2 ] );
  strcpy( Enum1, argv[ 3 ] );
  sprintf( Enum2, "%s{", Enum1 ); /* For those who use cuddled braces */ 

  /* If a 4th argument then that is the #ifdef not to be skipped */
  if ( argc == 5 )
    NoSkipPtr = argv[ 4 ];

  /* Attempt to open the source file */
  if ( !(Fp = fopen( File, "r" ) ) )
  {
    fprintf( stderr, "Failed to open input file %s\n", File );
    exit( EXIT_FAILURE ); 
  }

  /* While there are lines to be read and not finished reading definitions */ 
  Busy  = TRUE;
  while ( fgets( Line, MAX_LINE, Fp ) && Busy )
  {
    /* Read the definition string */
    Count = sscanf( Line, "%s%s%s ", s1, s2, s3 );
    NamePtr = s1;

    /* Check if skipping lines because of ifdef */
    if ( Skip && ( Count >= 1 ) ) 
    {
      /* Skipping ifdef'ed lines - Check for endif */ 
      if ( !strcmp( s1, "#endif" ) )
      {
        /* Found endif so stop skipping lines */
        Skip = FALSE;         
      }

      /* Always skip the current line */
      continue;
    }
    else if ( !Skip && (Count >= 2 ) )
    {
      /* Normal operation - check for #ifdef present */ 
      if ( ( !strcmp( s1, "#ifdef" ) )&&    /* Found ifdef string     */
           (  strcmp( s2, NoSkipPtr) )  )   /* that should be skipped */
      {
        /* Found ifdef, skip further processing until endif found */ 
        Skip = TRUE;
        continue;      
      }    
    }
       
    /* Line processing after enumeration definition block has been found    */
    if ((Found                                 )&&   /* Defn. block found   */
        (Count      >= 1                       )&&   /* Not a blank line    */
        (*++NamePtr == '_'                     )&&   /* Start of defn. word */
        (NamePtr    =  strchr( ++NamePtr, '_' ))  )  /* End of defn. prefix */  
    {
      /* Remove any un-pleasant trailing character */
      /* Only alpha-numeric and underscore allowed */
      for ( ChrPtr = ++NamePtr; 
           *ChrPtr && (isalnum( *ChrPtr ) || (*ChrPtr == '_') );
            ChrPtr++ )
        ;

      *ChrPtr = '\0';

      /* Special fix for the prefix to be displayed if some strings contained */
      if ( ( Verbatim == FALSE ) && 
            ( ( strcmp( NamePtr, "STATE" ) == 0 )
            || ( strcmp( NamePtr, "AUTH_STATE" ) == 0 )
            || ( strcmp( NamePtr, "PROC_STATE" ) == 0 )
            || ( strstr( NamePtr, "VERSION" ) != NULL )
            || ( strstr( NamePtr, "SHUTDOWN" ) != NULL )
            || ( strcmp( NamePtr, "ENABLE" ) == 0 )
            || ( strcmp( NamePtr, "DEMAND" ) == 0 )
            || ( strcmp( NamePtr, "ACTUAL" ) == 0 )
            || ( strcmp( NamePtr, "HALT_DEMAND" ) == 0 )
            || ( strcmp( NamePtr, "STOP_DEMAND" ) == 0 )
            || ( strcmp( NamePtr, "STOPPED" ) == 0 )
            || ( strcmp( NamePtr, "HOME" ) == 0 )
            || ( strncmp( NamePtr, "STATE_", strlen( "STATE_" ) ) == 0 ) ) )
      {
        /* The Prefix occurs after the D_ so skip 2 characters */ 
        NamePtr = s1 + 2; 
      }

      /* Get the length of the name  */ 
      NameLen = strlen( NamePtr );

      DescLen    = 0;
      DescHedPtr = "";
      if ((DescHedPtr = strstr(Line,           "/*"))&&  /* Is a comment */
          (DescEndPtr = strstr(DescHedPtr + 2, "*/"))  ) /* present?     */
      {   
        /* Skip leading spaces in comment line */
        for( DescHedPtr += 2; 
            *DescHedPtr && isspace( *DescHedPtr );
             DescHedPtr++ )
          ;

        /* Skip trailing space in the comment line */
        for( DescEndPtr--;
            (DescEndPtr > DescHedPtr) && isspace( *DescEndPtr );
             DescEndPtr-- );
          ;

        DescLen = DescEndPtr - DescHedPtr;
        if (DescLen < 0)
            DescLen = 0;
        else
            DescLen++;
      }
      if ( SqlID == TRUE)
      { 
          if ( FirstTime == TRUE )
          {
             FirstTime = FALSE; 
             printf( " printf( \"CREATE DATABASE IF NOT EXISTS sdb;\\n\" );\n");
             printf( " printf( \"USE sdb; \\n\" );\n");
#if 0
             printf( " printf( \"CREATE TABLE IF NOT EXISTS `DataIdTable` ( \\n\" );\n");
             printf( " printf( \"  `PkgId` INTEGER default NULL, \\n\" );\n");
             printf( " printf( \"  `DatumId` INTEGER default NULL, \\n\" );\n");
             printf( " printf( \"  `DatumName` varchar(64) default NULL, \\n\" );\n");
             printf( " printf( \"  `DatumDesc` varchar(64) default NULL \\n\" );\n");
             printf( " printf( \") ENGINE=InnoDB DEFAULT CHARSET=latin1; \\n\" );\n");
#endif
             printf( " printf( \"INSERT INTO `DataIdTable` (`PkgId`,`DatumId`,`DatumName`,`DatumDesc`) VALUES \\n\" );\n");

          }
          printf("  printf( \"(%%d, %%d, \\\'%*.*s\\\', \\\'%*.*s\\\'),\\n\", E_CIL_%s, %s) ;\n",
                    NameLen, NameLen, NamePtr,
                    DescLen, DescLen, DescHedPtr, Pkg,
                    s1 ); 

      }
      else if( PackageID  == TRUE)                
      {


          /* Generate a code line of the form:-
          **    
          **   printf( "PKG %i \"SHORT_DESC\" \"Long description\"\n", 
          **           E_PKG_SHORT_DESC );
          **
          ** when the above code line is run it will output a line of the form:-
          **
          **  PKG 12 "SHORT_DESC" "Long Description"
          */
          printf("  printf( \"0x%%x 0x%%06x \\\"%*.*s\\\" \\\"%*.*s\\\"\\n\", %s, %s );\n",
                    NameLen, NameLen, NamePtr,
                    DescLen, DescLen, DescHedPtr, Pkg,
                    s1 ); 
      }
      else if( PackageID == FALSE)
      {
          /* Generate a code line of the form:-
          **    
          **   printf( "PKG %i \"SHORT_DESC\" \"Long description\"\n", 
          **           E_PKG_SHORT_DESC );
          **
          ** when the above code line is run it will output a line of the form:-
          **
          **  PKG 12 "SHORT_DESC" "Long Description"
          */
          printf("  printf( \"%s 0x%%06x \\\"%*.*s\\\" \\\"%*.*s\\\"\\n\", %s );\n",
                    Pkg,
                    NameLen, NameLen, NamePtr,
                    DescLen, DescLen, DescHedPtr,
                    s1 ); 

      }
    }
           
    /* Line processing before enumeration block has been found */    
    if (!Found                  &&  /* Looking for an enum definition   */
         Count >= 3             &&  /* At least 3 strings to look at    */
        !strcmp( s1, "typedef" )&&  /* Check if we have found the       */
        !strcmp( s2, "enum"    )&&  /* enumeration definition that we   */
       (!strcmp( s3,  Enum1) || !strcmp( s3, Enum2 )))  /* are seeking for  */                          
    {
      Found = TRUE;
    }
       
    /*  Check if end of enumeration block found */
    if ( Found && strchr( Line, '}' ) )
      Busy = FALSE;
  }

  if ( SqlID == TRUE)
  { 
     if ( Busy == FALSE )
     {
        printf( "printf(\";\\n\");\n" );
     }
  }

  fclose( Fp );

  return EXIT_SUCCESS;
}
