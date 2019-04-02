/*
** BciOutput.c
**
** Contains functions to write the output file of measurements.
**
*/


/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <syslog.h>


/*
** Local include files
*/

#include "BciPrivate.h"


/* 
** Module-level definitions
*/

/*
** output string for WMS file (comma-delimited) :-
** - year (YYYY)
** - day (0-365)
** - time (HHMMSS)
** - temperature (°C)
** - humidity (%)
** - dew point (°C)
** - wind speed (m/s)
** - wetness (boolean)
** - sky-ambient temperature (°C)
*/

#define M_BCI_STR_WMS_YEAR   "%Y"
#define M_BCI_STR_WMS_DAY    "%j"
#define M_BCI_STR_WMS_TIME   "%H%M%S"

#define M_BCI_STR_WMS_OUTPUT "%s,%s,%s,%.1lf,%.1lf,%.1lf,%.1lf,%d,%.1lf\n"

/*
** output string for RCS file (space-delimited) :-
** - timestamp (YYYY-DDD'T'HHMMSS)
** - temperature (°C)
** - humidity (%)
** - dew point (°C)
** - wind speed (m/s)
** - wetness (boolean)
** - sky-ambient temperature (°C)
*/

#define M_BCI_STR_RCS_TIME   "%Y-%jT%H%M%S"

#define M_BCI_STR_RCS_OUTPUT "%s %.1lf %.1lf %.1lf %.1lf %d %.1lf\n"


/*
** iBciWriteOutputFile
** 
** Write the output file, opening it each time for overwriting.
**
*/

void iBciWriteOutputFile( char * OutFilepath, char * OutFilestem )
{
   FILE *   FileId;
   time_t   CurrentTime;
   long     Day;
   char *   FilenamePtr;
   char     WmsFilename[ FILENAME_MAX ];
   char     RcsFilename[ FILENAME_MAX ];
   char     TmpFilename[ FILENAME_MAX ];
   char     YearStr[ I_BCI_BUFFER_LEN ];
   char     DayStr[ I_BCI_BUFFER_LEN ];
   char     TimeStr[ I_BCI_BUFFER_LEN ];
   char     OutputStr[ I_BCI_BUFFER_LEN ];

   /* construct the filenames for output */
   sprintf( WmsFilename, "%s%c%s%s%s", 
            OutFilepath, I_BCI_DIR_SEPARATOR, I_BCI_WMS_FILEPREFIX, 
            OutFilestem, I_BCI_OUT_FILEEXT );
   sprintf( RcsFilename, "%s%c%s%s%s", 
            OutFilepath, I_BCI_DIR_SEPARATOR, I_BCI_RCS_FILEPREFIX, 
            OutFilestem, I_BCI_OUT_FILEEXT );

   /* get the current time to create the WMS file */
   if ( time( &CurrentTime ) != -1 )
   {
      /* point to the WMS filename */
      FilenamePtr = WmsFilename;

      /* write the year into a string */
      strftime( YearStr, I_BCI_BUFFER_LEN, M_BCI_STR_WMS_YEAR, 
                gmtime( (time_t *) &( CurrentTime ) ) );

      /* write the day into a string */
      strftime( DayStr, I_BCI_BUFFER_LEN, M_BCI_STR_WMS_DAY, 
                gmtime( (time_t *) &( CurrentTime ) ) );

#if 0
      /* convert day from range 1-366 to 0-365 */
      Day = strtol( DayStr, NULL, 10 );
      Day--;
      sprintf( DayStr, "%ld", Day );
#endif

      /* write the time into a string */
      strftime( TimeStr, I_BCI_BUFFER_LEN, M_BCI_STR_WMS_TIME, 
                gmtime( (time_t *) &( CurrentTime ) ) );

      /* construct the output string */
      sprintf( OutputStr, M_BCI_STR_WMS_OUTPUT,
               YearStr, 
               DayStr, 
               TimeStr,
               iBciData.Temperature,
               iBciData.Humidity,
               iBciData.DewPoint,
               iBciData.WindSpeed,
               iBciData.Wetness,
               iBciData.SkyAmbient );

      /* open a temporary output file for over-writing */
      sprintf( TmpFilename, "%s%s",FilenamePtr, I_BCI_TMP_FILEEXT );
      FileId = fopen( TmpFilename, "w" );

      /* if it's opened OK */
      if ( FileId != NULL )
      {
         fprintf( FileId, OutputStr );

         /* close the file */
         fclose( FileId );
      }

      /* rename the temporary file to the output file */
      rename( TmpFilename, FilenamePtr );

      syslog( LOG_INFO, "WMS : %s\n", OutputStr );
   }

   /* get the current time to create the RCS file */
   if ( time( &CurrentTime ) != -1 )
   {
      /* point to the RCS filename */
      FilenamePtr = RcsFilename;

      /* write the time into a string */
      strftime( TimeStr, I_BCI_BUFFER_LEN, M_BCI_STR_RCS_TIME, 
                gmtime( (time_t *) &( CurrentTime ) ) );

      /* construct the output string */
      sprintf( OutputStr, M_BCI_STR_RCS_OUTPUT,
               TimeStr,
               iBciData.Temperature,
               iBciData.Humidity,
               iBciData.DewPoint,
               iBciData.WindSpeed,
               iBciData.Wetness,
               iBciData.SkyAmbient );

      /* open a temporary output file for over-writing */
      sprintf( TmpFilename, "%s%s", FilenamePtr, I_BCI_TMP_FILEEXT );
      FileId = fopen( TmpFilename, "w" );

      /* if it's opened OK */
      if ( FileId != NULL )
      {
         fprintf( FileId, OutputStr );
 
         /* close the file */
         fclose( FileId );
      }

      /* rename the temporary file to the output file */
      rename( TmpFilename, FilenamePtr );

      syslog( LOG_INFO, "RCS : %s\n", OutputStr );
   }



}

/*
** EOF
*/

