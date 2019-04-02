/*
** Module Name:
**    HtiSqlList.c
**
** Purpose:
**
** Description:
**
** Authors:
**    sxf: Steve Foale
**
** Version:
**    $Id: HtiSqlList.c,v 0.1 2007/03/12 15:17:07 sxf Exp $
**
** History:
**    $Log: HtiSqlList.c,v $
**    Revision 0.1  2007/03/12 15:17:07  sxf
**    Inital version.
**
**
*/


/* Include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "/ttl/sw/include/Cil.h"

int main(int argc, char* argv[])
{
/*
** Function Name:
**    main
**
*/

#define MAX_LINE 255
#define TELNAME   "-telescope"
#define FAULKES1  "FTN"
#define FAULKES2  "FTS"
#define CIL_MAP   "/ttl/sw/etc/Cil.map"
#define IDTABLE   "/ttl/sw/etc/IdTable.idx"
#define UNITSTABLE "/ttl/sw/etc/UnitsTable.idx"

#define CREATE_DB_STRING "CREATE DATABASE IF NOT EXISTS %sTelemetry;\n"    
#define USE_DB_STRING    "USE %sTelemetry;\n"    
#define DROP_TABLE_STRING "DROP TABLE IF EXISTS `Data`;"

#define CREATE_DATATABLE_STRING "CREATE TABLE IF NOT EXISTS `Data` ( " \
                                " `SourceId` int(11) default NULL, " \
                                " `DatumId` int(11) default NULL, " \
                                " `Value` varbinary(64) default NULL, " \
                                " `TimeSec` int(11) default NULL, " \
                                " `TimeNsec` int(11) default NULL " \
                                " ) ENGINE=MyISAM DEFAULT CHARSET=latin1; "

#define CREATE_LIVE_DATATABLE_STRING "CREATE TABLE IF NOT EXISTS `LiveData` ( " \
                                " `SourceId` int(11) default NULL, " \
                                " `DatumId` int(11) default NULL, " \
                                " `Value` varbinary(64) default NULL, " \
                                " `TimeSec` int(11) default NULL, " \
                                " `TimeNsec` int(11) default NULL " \
                                " ) ENGINE=MyISAM DEFAULT CHARSET=latin1; "

#define CREATE_DESCRIPTION_STRING "CREATE TABLE IF NOT EXISTS `DataDescription` ("\
                            " `SourceId` int(11) default NULL, " \
                            " `SourceName` varchar(3) default NULL, " \
                            " `DatumId` int(11) default NULL, " \
                            " `DatumName` varchar(64) default NULL, " \
                            " `DatumDescription` varchar(64) default NULL, " \
                            " `Units` varchar(32) default NULL, " \
                            " `Type` varchar(32) default NULL, " \
                            " `Archive` boolean default TRUE " \
                            " ) ENGINE=InnoDB DEFAULT CHARSET=latin1; "

#define CREATE_UNITS_STRING "CREATE TABLE IF NOT EXISTS `SdbUnits` ("\
                            " `Id` int(11) default NULL, " \
                            " `Name` varchar(32) default NULL, " \
                            " `Description` varchar(128) default NULL " \
                            " ) ENGINE=InnoDB DEFAULT CHARSET=latin1; "


#define INSERT_STRING  "INSERT INTO `DataDescription` (`SourceId`,`SourceName`,`DatumId`, `DatumName`,`DatumDescription`) VALUES "
#define INSERT_UNITS   "INSERT INTO `SdbUnits` (`Id`, `Name`, `Description` ) VALUES "

  char  File[ FILENAME_MAX + 1 ]; /* The file name */
  char  TelName[ FILENAME_MAX + 1 ]; /* The enumeration typdef to search for         */
  char  Line[256]; /* A line from the file         */
  char *TheRest;
  char CilName[128];
  char s2[128];
  char DatumName[128];
  char Description[128];
  char *AposPtr;
  char *ChrPtr;                   /* General purpose pointer to char              */
  char *NamePtr;                  /* Pointer to short name of Sdb datum           */ 
  char *DescHedPtr;               /* Pointer to start oflong description of datum */
  char *DescEndPtr;               /* Pointer to end of long description           */

  int   i;                        /* Loop counter                                 */
  int   Count;                    /* Number of string segments                    */
  int   Busy  = TRUE;             /* Not at end of file yet                       */
  int   Found = FALSE;            /* Enumeration typedef string found             */
  int   Skip  = FALSE;            /* Skip ifdef'ed lines                          */
  int   PackageID = FALSE;        /* Determines if the program is running in cil mode or package mode */
  int   SqlID = FALSE;            /* Determines if the program is running in sql dump mode */
  int   CilId;
  int   DataId;
  int   Status;
  int   NameLen;                  /* Length of short name string                  */
  int   DescLen;                  /* Length of long description string            */
  int   FirstTime = TRUE;         /* Only true first time through loop.           */
  FILE *Fp;                       /* Pointer to open file                         */
  FILE *Up;                       /* Pointer to open file                         */


  /* Print the start code block if requested*/
  Found = FALSE;
  if ( argc == 3  )
  {
     if ( strcmp( argv[ 1 ],  TELNAME ) == 0 )
     {
        if ( strcmp( argv[ 2 ],  FAULKES1 ) == 0 )
        {
           strcpy( TelName, FAULKES1 );   
           Found = TRUE;
        }
        else if ( strcmp( argv[ 2 ],  FAULKES2 ) == 0 )
        {
           strcpy( TelName, FAULKES2 );
           Found = TRUE;
        }

     }
  }

  if ( Found != TRUE )
  {
     printf( "No -telescope argument provided\n" );
     printf( "Choose on of: \n\t%s\n\t%s\n", FAULKES1, FAULKES2 );
     exit (EXIT_FAILURE);
  }


  /* Attempt to open the source file */
  if ( !(Fp = fopen( IDTABLE, "r" ) ) )
  {
    fprintf( stderr, "Failed to open input file %s\n", IDTABLE );
    exit( EXIT_FAILURE ); 
  }

  if ( !(Up = fopen( UNITSTABLE, "r" ) ) )
  {
    fprintf( stderr, "Failed to open units file %s\n", IDTABLE );
    exit( EXIT_FAILURE ); 
  }

  printf( CREATE_DB_STRING, TelName );
  printf( USE_DB_STRING, TelName );
  printf( "%s\n", DROP_TABLE_STRING );
  printf( "%s\n", CREATE_DATATABLE_STRING );
  printf( "%s\n", CREATE_LIVE_DATATABLE_STRING );
  printf( "%s\n", CREATE_DESCRIPTION_STRING );
  printf( "%s\n", CREATE_UNITS_STRING );
  printf( "%s\n", INSERT_STRING );

  /* While there are lines to be read and not finished reading definitions */ 
  Busy  = TRUE;
  FirstTime = TRUE;
  while ( fgets( Line, MAX_LINE, Fp ) && Busy )
  {

    if (  !FirstTime )
    {
       printf( "\t,\n" );
    }
    else
    { 
       FirstTime = FALSE;
    }

    /* Read the definition string */
    TheRest = strtok( Line, (void *)(" ") );
    strcpy( CilName, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( s2, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( DatumName, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( Description, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( Description, TheRest );


    AposPtr = strstr( Description, "'" );

    while ( AposPtr != NULL )
    {
       *AposPtr = ' ';
       AposPtr = strstr( Description, "'" );
    }

    sscanf( s2, "0x%x", &DataId );

    Status = eCilLookup( CIL_MAP, CilName, &CilId );
    if ( Status == SYS_NOMINAL )
    {
       printf( "(%d, '%s', %d, '%s', '%s')\n", CilId, CilName, DataId, DatumName, Description );
    }
    else 
    {
       printf( "Error looking up %s is 0x%x\n", CilName, Status );
    }
  }

  printf( "\t;\n" );

  fclose( Fp );

  printf( "%s\n", INSERT_UNITS );

  Busy  = TRUE;
  FirstTime = TRUE;
  while ( fgets( Line, MAX_LINE, Up ) && Busy )
  {

    if (  !FirstTime )
    {
       printf( "\t,\n" );
    }
    else
    { 
       FirstTime = FALSE;
    }

    /* Read the definition string */
    TheRest = strtok( Line, (void *)(" ") );
    strcpy( CilName, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( s2, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( DatumName, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( Description, TheRest );
    TheRest = strtok( NULL, (void *)("\"") );
    strcpy( Description, TheRest );


    AposPtr = strstr( Description, "'" );

    while ( AposPtr != NULL )
    {
       *AposPtr = ' ';
       AposPtr = strstr( Description, "'" );
    }

    sscanf( s2, "0x%x", &DataId );

    printf( "(%d, '%s', '%s')\n", DataId, DatumName, Description );
  }

  printf( "\t;\n" );

  fclose( Up );


  return EXIT_SUCCESS;
}
