/*****************************************************************************
** Module Name:
**     StdInit.c
**
** Purpose:
**     Contains functions used in to initialise the program.
**
** Description:     
**     Containts functions to initiliase the CLU, parse 
**     the command line and read in a configuration file.
**  
** Authors:
**    Derek J. McKay (djm)
**    Martin Norbury (man)
**
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
**
** Version:
**      $Id: StdInit.c,v 0.13 2005/06/27 15:47:14 mjf Exp $    
**
** History:
**      $Log: StdInit.c,v $
**      Revision 0.13  2005/06/27 15:47:14  mjf
**      Set environment variable TZ to zero to ensure calls to time
**      manipulation functions are not affected by daylight saving.
**
**      Revision 0.12  2005/06/21 13:50:39  mjf
**      Addition of a command-line switch to specify a 'stride' in seconds to
**      ensure extracted measurements are a specified distance apart in time.
**
**      Revision 0.11  2005/06/20 13:38:44  mjf
**      Tidied up to aid porting to Linux - no functional changes.
**
**      Revision 0.10  2005/06/17 10:08:50  mjf
**      STD_1_04 - incorporate functional changes made in STD_0_13 -> STD_0_15.
**
**      Revision 0.9  2004/10/15 13:48:24  mkb
**        Fix problems that make STD enter infinte loops during some
**         searches.
**         Put search results into linked list so they can be sorted
**         chronologically because the SBD does not guarantee the
**         order of records in *.sdb files.
**
**      Revision 0.8  2004/04/02 15:25:21  mjf
**      No functional changes, ported to QNX v6 by SXF.
**
**      Revision 0.7  2003/09/15 14:35:18  man
**      Included a command line switch in which an alternate
**      path for the sdb data can be specified.
**
**      Revision 0.6  2003/08/15 13:25:46  man
**      If unable to resolve name of datum, continue and display datum number
**      in header. This can be used if the datum is too recent to have made it
**      into the Hti lookup files.
**
**      Revision 0.5  2003/01/31 08:07:49  man
**      Removed IdTable path from command line.
**
**      Revision 0.4  2003/01/15 16:17:14  man
**      Updated to use Std library calls.
**
**      Revision 0.3  2002/10/03 13:36:46  man
**      Allow hex numbers to be input in the configuration
**      file.
**
**      Revision 0.2  2002/08/14 11:57:39  man
**      General tidy up. Included commonly used routines
**      in functions.
**
**      Revision 0.1  2002/07/08 10:13:49  man
**      Initial revision.
**
**
**                   
*****************************************************************************/


/* Local include files */
#include "StdPrivate.h"


/*****************************************************************************
** Function Name:
**    StdCluInit
**
** Type:
**    Status_t
**
** Purpose:
**    Initialised the CLU and parses the command line.
**
** Description:
**    Function called to initalise the CLU and parse the command
**    line for custom arguments. Also sets the configuration
**    items to default values.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
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
**    djm: Derek J. McKay (TTL)
**    man: Martin Norbury
**
*****************************************************************************/

Status_t iStdCluInit ( int argc, char *argv[] )
{
   Status_t Status = SYS_NOMINAL;      /* Return status of function calls */
   char     Switch[ E_CLU_SWITCH_LEN ];/* to report error parsing a switch */
   /*Int32_t  Id;*/
   Int32_t  NumDataSearch;
   /*unsigned Datum;*/
   iStdData_t *StdDataPtr;
   eTtlTime_t Time;
   eStdTime_t *StartTimePtr;
   eStdTime_t *StopTimePtr;
   eStdTime_t TimeNow;
   char       TimeStr[E_STD_MAX_STRING_LEN];
   char      *ParamPtr = NULL;
   int        i;

   /* data for command-line utilities */
   eCluProgNamePtr              = I_STD_PROGRAM_NAME;
   eCluProgAboutPtr             = I_STD_PROGRAM_ABOUT;
   eCluReleaseDatePtr           = I_STD_RELEASE_DATE;
   eCluYearPtr                  = I_STD_YEAR;
   eCluMajorVer                 = I_STD_MAJOR_VERSION;
   eCluMinorVer                 = I_STD_MINOR_VERSION;
  
   /* set default parameters before parsing command-line */
   eCluCommon.Quiet             = E_STD_DFLT_QUIET;
   eCluCommon.Verbose           = E_STD_DFLT_VERBOSE;
   eCluCommon.Syslog            = E_STD_DFLT_SYSLOG;
   eCluCommon.DebugLevel        = E_STD_DFLT_DEBUG;
   eCluCommon.Priority          = E_STD_DFLT_PRIORITY;
   eCluCommon.Help              = E_STD_DFLT_HELP;
   strcpy( eCluCommon.ConfigFile, E_STD_DFLT_CONFIG  );
   strcpy( eCluCommon.LogFile,    E_STD_DFLT_LOG     );
   strcpy( eCluCommon.CilName,    E_STD_DFLT_CIL     );
   strcpy( eCluCommon.CilMap,     E_STD_DFLT_CIL_MAP );

   /* Initialise the file and data search parameters */
   iStdGlobVar.LoadFile = FALSE;
   iStdGlobVar.NumDataSearch = 0;
   StdDataPtr = iStdGlobVar.StdData;
   StartTimePtr = &iStdGlobVar.StartTime;
   StopTimePtr = &iStdGlobVar.StopTime;
   NumDataSearch = iStdGlobVar.NumDataSearch;
   strncpy ( iStdGlobVar.OutFile, E_STD_DFLT_OUTFILE, E_STD_MAX_STRING_LEN);

   
   /*
   ** Initialise the first data structure to all source
   ** and all data id's.
   */
   for(i=0; i<E_STD_MAX_SEARCH_DATA; i++)
   {
      (StdDataPtr+i)->DataFlag = FALSE;
      (StdDataPtr+i)->SourceId = E_CIL_BOL;
      (StdDataPtr+i)->DatumId  = 0;      
      strncpy ( (StdDataPtr+i)->SourceName, "???", E_STD_MAX_STRING_LEN);
      strncpy ( (StdDataPtr+i)->DatumName , "???", E_STD_MAX_STRING_LEN);
      strncpy ( (StdDataPtr+i)->DatumUnits, "???", E_STD_MAX_STRING_LEN);      
      (StdDataPtr+i)->NumOfPoints = 0;
      memset( &( (StdDataPtr+i)->NextStrideTime ), 0, 
              sizeof( (StdDataPtr+i)->NextStrideTime ) );
   }


   /* register with the command-line utilities (CLU), ignore all except help */
   Status = eCluSetup( argc, argv, 
                          E_CLU_IGN_QUIET | E_CLU_IGN_VERBOSE 
                           | E_CLU_IGN_CIL_NAME );

   if( SYS_NOMINAL != Status )
   {
      printf("Error registering command-line utilities\n");
      return Status;
   }

   /* 
   ** Parse the command line 
   */
   Status = eCluParseArgs ( Switch );

   if( SYS_NOMINAL != Status )
   { 
      eCluShowUsage( NULL, NULL );
      printf( "Error parsing the command-line switch '%s'\n", Switch );
      /* exit the program */
      exit( EXIT_SUCCESS );
   }

   /* Check to see if help was requested */
   if( eCluCommon.Help == TRUE )
   {
      exit ( EXIT_SUCCESS );
   }

   /* Sign the program on */
   Status = eCluSignOn (NULL, NULL);
   if( SYS_NOMINAL != Status)
   {
      printf("Error signing on (%d = %x)\n", Status, Status);
      exit (EXIT_FAILURE);
   }
 
   /* Check for custom command line parameters */
   strcpy ( iStdGlobVar.DatPath, I_STD_DFLT_PATH ); /* Default */

   if ( eCluCustomArgExists( I_STD_ARG_PATH ) == E_CLU_ARG_SUPPLIED )
   {
      /* Get pointer to parameter */
      ParamPtr = eCluGetCustomParam( I_STD_ARG_PATH );
      /* If parameter supplied */
      if ( ParamPtr != NULL )
      {
         /* Copy from string into data area */
         strncpy( iStdGlobVar.DatPath, ParamPtr, I_STD_MAX_PATH_LEN );
         
         /* Check to see if last character is a slash*/
         if(ParamPtr[strlen(ParamPtr)-1] != '/')
         {
            eLogNotice(0,"Path should end with /");
            exit (EXIT_FAILURE);
         }
      }
   }

   eLogNotice(0,"Data directory = \"%s\"",iStdGlobVar.DatPath);

   iStdGlobVar.Stride = I_STD_DFLT_STRIDE; /* Default */

   if ( eCluCustomArgExists( I_STD_ARG_STRIDE ) == E_CLU_ARG_SUPPLIED )
   {
      /* Get pointer to parameter */
      ParamPtr = eCluGetCustomParam( I_STD_ARG_STRIDE );
      /* If parameter supplied */
      if ( ParamPtr != NULL )
      {
         /* Copy from string into data area */
         iStdGlobVar.Stride = (Int32_t) strtol( ParamPtr, NULL, 0 );
      }
   }

   eLogNotice(0,"Measurement stride = %d seconds",iStdGlobVar.Stride);

   iStdGlobVar.WriteMatlab = I_STD_DFLT_MLB; /* Default */

   if ( eCluCustomArgExists( I_STD_ARG_MLB ) == E_CLU_ARG_SUPPLIED )
   {
      /* Write blank data as "Nan" for import to octave/matlab. */
      iStdGlobVar.WriteMatlab = TRUE;
      iStdGlobVar.WriteGnuplot = FALSE;
   }

   iStdGlobVar.WriteGnuplot = I_STD_DFLT_GPT; /* Default */

   if ( eCluCustomArgExists( I_STD_ARG_GPT ) == E_CLU_ARG_SUPPLIED )
   {
      /* Write blank data as "Nan" for import to octave/matlab. */
      iStdGlobVar.WriteMatlab = FALSE;
      iStdGlobVar.WriteGnuplot = TRUE;
   }
   /* 
   ** Set the default start and stop times to now 
   ** just in case a time is not specified in the
   ** configuration file.
   */
   Status = eTimGetTime ( &Time );
   if( Status != SYS_NOMINAL)
   {
      eLogErr(Status,"Error getting current system time");
      return Status;
   }
   Status = eTimToString( &Time, E_STD_MAX_STRING_LEN, TimeStr);
   if( Status != SYS_NOMINAL)
   {
      eLogErr(Status,"Error converting system time to string");
      return Status;
   }
   eLogDebug("Time now is = %s", TimeStr);

   /* 
   ** Extract the time and date from time string to allow
   ** us to format it the way we want.
   */
   sscanf( TimeStr,"%d/%d/%d %d:%d:%d.%d", &TimeNow.Date, &TimeNow.Month,
          &TimeNow.Year, &TimeNow.Hour, &TimeNow.Minute, &TimeNow.Second, &TimeNow.MilliSecond);

   StartTimePtr->Year = StopTimePtr->Year = TimeNow.Year;
   StartTimePtr->Month = StopTimePtr->Month = TimeNow.Month;
   StartTimePtr->Date = StopTimePtr->Date = TimeNow.Date;
   StartTimePtr->Hour = StopTimePtr->Hour = TimeNow.Hour;
   StartTimePtr->Minute = StopTimePtr->Minute = TimeNow.Minute;
   StartTimePtr->Second = StopTimePtr->Second = TimeNow.Second;
   StartTimePtr->MilliSecond = StopTimePtr->MilliSecond = TimeNow.MilliSecond;

   
   return (Status);
}

/*****************************************************************************
** Function Name:
**    StdReadConfig
**
** Type:
**    Status_t
**
** Purpose:
**    Read in the supplied configuration file.
**
** Description:
**    Function called to read in a configuration file
**    specified on the command line.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success;
**
** Argu11nts:
**    char *ConfigFilePtr[]             (in)
**       Pointer to the configuration file to be searched.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    man: Martin Norbury
**
*****************************************************************************/

Status_t iStdReadConfig ( char *ConfigFilePtr )
{
   Status_t    Status = SYS_NOMINAL;       /* Current status */
   char        Line[ E_CFU_STRING_LEN ];   /* To store line of config file */
   char        KeyWord[ E_CFU_STRING_LEN ];/* Current config file keyword */
   char        Value1[ E_CFU_STRING_LEN ]; /* Configuration value */
   char        Value2[ E_CFU_STRING_LEN ]; /* Configuration value */
   Int32_t     ReadSourceInt, ReadDatumInt;/* Source and Datum to search for*/
   Int32_t     NumDataSearch; /* Number of datum values to SDB for */
   Int32_t     ItemFound; /* Flag to indicate if a config item is recognised*/
   Uint32_t    Year,Month,Date;    /* Time read in from config file */ 
   Uint32_t    Hour,Minute,Second; /* Time read in from config file */ 
   iStdData_t  *StdDataPtr; /* Pointer to list of datum to search for */
   eStdTime_t  *StartTimePtr; /* Time to begin search */
   eStdTime_t  *StopTimePtr;  /* Time to finish search */
  
   /*
   ** Update values from the global structure and
   ** initialise pointers.
   */
   NumDataSearch = iStdGlobVar.NumDataSearch;
   StdDataPtr    = iStdGlobVar.StdData;
   StartTimePtr  = &iStdGlobVar.StartTime; 
   StopTimePtr   = &iStdGlobVar.StopTime;
   iStdLastMatchedPtr  = NULL;
   iStdLinesOfData     = 0;
   
   /* Open the configuration file */
   Status = eCfuSetup ( ConfigFilePtr );
   if( SYS_NOMINAL != Status)
   {
      eLogErr(Status,"Error opening configuration file %s using command line options",ConfigFilePtr);
      /* 
      Set the numver of datum to search for to 1 so we
      can use the command line options.
      */
      iStdGlobVar.NumDataSearch = 1;      
      return Status;
   }
   else
   {
      eLogInfo("Using configuration file %s",ConfigFilePtr);
      /* Set the flag to indicate we now have a configuration file */
      iStdGlobVar.GotConfig = TRUE;
   }

   /* 
   ** Begin searching through the configuration file for keywords
   ** and associated values.
   */

   while ( eCfuGetLine( Line ) == SYS_NOMINAL )
   {
      if ( ( eCfuGetParam( KeyWord ) == SYS_NOMINAL ) )
      {
      
         ItemFound = 0;

         /* Check for the output file keyword*/
         if( (strncmp (KeyWord, "OUTPUT", strlen(KeyWord)) == 0))
         {
            ItemFound = 1;
            if( eCfuGetParam( Value1 ) == SYS_NOMINAL)
            {
               strncpy( iStdGlobVar.OutFile, Value1, E_STD_MAX_STRING_LEN);
               eLogDebug("Output file = %s", iStdGlobVar.OutFile);
            }
            else
            {
               eLogErr(E_STD_GEN_ERROR,"Error reading output file name from configuration file");
            }
         }

         /* Check for the start time keyword*/
         if( (strncmp (KeyWord, "START TIME", strlen(KeyWord)) == 0))
         {
            ItemFound = 1;
            if( eCfuGetParam( Value1 ) == SYS_NOMINAL)
            {
               if(sscanf( Value1,"%d/%d/%d %d:%d:%d",&Year,&Month,&Date,&Hour,&Minute,&Second) ==6)
               {     
                  StartTimePtr->Year        = Year;
                  StartTimePtr->Month       = Month;
                  StartTimePtr->Date        =  Date;
                  StartTimePtr->Hour        =  Hour;
                  StartTimePtr->Minute      = Minute;
                  StartTimePtr->Second      = Second;                   
                  StartTimePtr->MilliSecond = 0;
                  eLogDebug("Start time = %.2d/%.2d/%.2d %.2d:%.2d:%.2d",Year,Month,Date,Hour,Minute,Second);
               }
               else
               {
                  eLogErr(E_STD_GEN_ERROR,"Failed to read in start time");
               }
            }
            else
            {
               eLogErr(E_STD_GEN_ERROR,"Error reading config time value");
            }
         }

         /* Check for stop time keyword */
         if( (strncmp (KeyWord, "STOP TIME", strlen(KeyWord)) == 0))
         {
            ItemFound = 1;
            if( eCfuGetParam( Value1 ) == SYS_NOMINAL)
            {
               if(sscanf( Value1,"%d/%d/%d %d:%d:%d",&Year,&Month,&Date,&Hour,&Minute,&Second) ==6)
               {                                    
                  StopTimePtr->Year        = Year;
                  StopTimePtr->Month       = Month;
                  StopTimePtr->Date        =  Date;
                  StopTimePtr->Hour        =  Hour;
                  StopTimePtr->Minute      = Minute;
                  StopTimePtr->Second      = Second;
                  StopTimePtr->MilliSecond = 0;
                  eLogDebug("Stop time  = %.2d/%.2d/%.2d %.2d:%.2d:%.2d",Year,Month,Date,Hour,Minute,Second);
               }
               else
               {
                  eLogErr(E_STD_GEN_ERROR,"Failed to read in stop time");
               }
            }
            else
            {
               eLogErr(E_STD_GEN_ERROR,"Error reading config time value");
            }
         }

         /* Check for duration keyword */
         if( (strncmp (KeyWord, "DURATION", strlen(KeyWord)) == 0))
         {
            ItemFound = 1;
         }

         /* 
         ** Check for soure and datum pair to search SDB for.
         ** Both source and datum Id's can be either integers
         ** or a string e.g. it will accept either 20 or AGS.       
         */
         if( (strncmp (KeyWord, "DATA", strlen(KeyWord)) == 0) &&
             NumDataSearch < E_STD_MAX_SEARCH_DATA )
         {
            ItemFound = 1;
            eLogDebug("Found a config item");
            if ( eCfuGetParam( Value1 ) == SYS_NOMINAL &&
                 eCfuGetParam( Value2 ) == SYS_NOMINAL   )
            {

               /* 
               ** Found a data item to search for so increment the
               ** counter. 
               */
               NumDataSearch++;
               eLogDebug("Value1 = %s Value2 = %s",Value1, Value2);

               /* 
               ** Extract the source id if it is an integer, else
               ** we assume it is a source/datum name instead.
               */              
               if( sscanf ( Value1, "0x%x", &ReadSourceInt ) == 1 )
               {
                  eLogDebug("Source Id requested is 0x%x",ReadSourceInt);
                  (StdDataPtr+NumDataSearch-1)->SourceId = ReadSourceInt;
               }
               else if( sscanf ( Value1, "%d", &ReadSourceInt ) == 1 )
               {
                  eLogDebug("Source Id requested is %d",ReadSourceInt);
                  (StdDataPtr+NumDataSearch-1)->SourceId = ReadSourceInt;
               }
               else
               {
                  strncpy ( (StdDataPtr+NumDataSearch-1)->SourceName, Value1, E_STD_MAX_STRING_LEN);                  
                  eLogDebug("Source name requested is %s", Value1);
               }

               if( sscanf ( Value2, "0x%x", &ReadDatumInt ) == 1 )
               {
                  eLogDebug("Datum Id requested is 0x%x",ReadDatumInt);
                  (StdDataPtr+NumDataSearch-1)->DatumId = ReadDatumInt;
               }
               else if( sscanf ( Value2, "%d", &ReadDatumInt ) == 1 )
               {
                  eLogDebug("Datum Id requested is %d",ReadDatumInt);
                  (StdDataPtr+NumDataSearch-1)->DatumId = ReadDatumInt;
               }
               else
               {
                  strncpy ( (StdDataPtr+NumDataSearch-1)->DatumName, Value2, E_STD_MAX_STRING_LEN);                               
                  eLogDebug("Source name requested is %s", Value2);
               }
              
            }
            else
            {
               eLogErr(E_STD_GEN_ERROR,"Error reading config values");
            }
     
         }

       /*  Status = sscanf (Value, "%d,%d", &ReadSource, &ReadDatum);
         eLogNotice (NULL, "sscanf returns %d (%d %d)", Status, ReadSource, ReadDatum);*/
         if( ItemFound == 0)
         {
            eLogErr(E_STD_GEN_ERROR, "Error with configuration item %s", KeyWord);
         }

      }
      else
      {
         eLogInfo("Ignoring line \"%s\"",Line);
      }
   }

   /*
   ** Managed to read the configuration file 
   ** and found NumDataSearch items to look 
   ** for
   */
   iStdGlobVar.NumDataSearch=NumDataSearch;

   /* Every thing has worked if we make it here */
   return SYS_NOMINAL;
}

/*****************************************************************************
** Function Name:
**    iStdSrcDtmName
**
** Type:
**    Status_t
**
** Purpose:
**    Gets the source or datum names for a pair of source and datum id's
**    or vice versa.
**
** Description:
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success;
**
** Arguments;
**    void
**
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/

Status_t iStdSrcDtmName ( void ) 
{
   Status_t Status;
   int i;
   Int32_t     ReadSourceInt, ReadDatumInt;/* Source and Datum to search for*/
   iStdData_t *StdDataPtr; /* Pointer to requested data id's */

   /* Initialise the pointer to the data */
   StdDataPtr = iStdGlobVar.StdData;

   /* 
   ** Get the CIL name, datum name and units
   ** for the data we are searching for
   */

   for( i=0 ; i<iStdGlobVar.NumDataSearch; i++)
   {

      /* 
      ** If we have a source ID, work out 
      ** the CIL name. If we have a CIL name, work
      ** out the source ID. If we don't have either
      ** then there is a problem.
      */
      if( (StdDataPtr+i)->SourceId == 0 )
      {                                                                                
         eLogDebug("Getting source ID from CIL name %s",(StdDataPtr+i)->SourceName);     
         Status = eCilLookup( eCluCommon.CilMap, (StdDataPtr+i)->SourceName, &ReadSourceInt);
         (StdDataPtr+i)->SourceId = (Uint32_t) ReadSourceInt;
         if( Status != SYS_NOMINAL )
         {
            eLogErr(Status, "Error getting CIL Id for %s", (StdDataPtr+i)->SourceName );
            return Status;
         }
         eLogDebug("Source name %s is %d", (StdDataPtr+i)->SourceName, (StdDataPtr+i)->SourceId);
      }                  
      else if ( strcmp( (StdDataPtr+i)->SourceName, "???") == 0)
      {
         eLogDebug("Getting CIL name for source ID %d", (StdDataPtr+i)->SourceId);      
         Status = eCilName( eCluCommon.CilMap, (StdDataPtr+i)->SourceId, 
                            E_CIL_IDLEN, (StdDataPtr+i)->SourceName);
         if( Status != SYS_NOMINAL )
         {
            eLogErr(Status, "Error getting CIL name for Id=0x%x", (StdDataPtr+i)->SourceId );
            return Status;
         }
         eLogDebug("Source Id %d is %s", (StdDataPtr+i)->SourceId, (StdDataPtr+i)->SourceName);
      }
      else
      {
         eLogErr(E_STD_SRCID_ERR, "Error setting source ID");
         return E_STD_SRCID_ERR;
      }
      
      /* 
      ** If we have a datum ID, work out the datum name.
      ** If we have a datum name, work out the datum ID, 
      ** If we don't have either then there is a problem
      */

      if( (StdDataPtr+i)->DatumId == 0 )
      {         
        eLogDebug("Getting datum ID from datum name %s",(StdDataPtr+i)->DatumName);    
        Status = eHtiGetGeneralId( (StdDataPtr+i)->SourceId, (StdDataPtr+i)->DatumName, &ReadDatumInt);
        (StdDataPtr+i)->DatumId = (Uint32_t) ReadDatumInt;
        if( Status != SYS_NOMINAL )
        {
           eLogErr(Status, "Error getting datum Id for %s", (StdDataPtr+i)->DatumName );
           return Status;
        }
        
        eLogDebug("Source name %s is %d", (StdDataPtr+i)->DatumName, (StdDataPtr+i)->DatumId);
      }
      else if ( strcmp( (StdDataPtr+i)->DatumName, "???") == 0)
      {
         eLogDebug("Getting datum name from datum ID %d", (StdDataPtr+i)->DatumId);
         Status = eHtiGetDataLabel( (StdDataPtr+i)->SourceId, (StdDataPtr+i)->DatumId, (StdDataPtr+i)->DatumName );
         if( Status != SYS_NOMINAL )
         {
            eLogErr(Status, "Error getting datum name for Id=0x%x", (StdDataPtr+i)->SourceId );
            sprintf( (StdDataPtr+i)->DatumName, "0x%x", (StdDataPtr+i)->DatumId );
            Status = SYS_NOMINAL;
         }
         eLogDebug("Datum Id %d is %s", (StdDataPtr+i)->DatumId, (StdDataPtr+i)->DatumName); 
      }
      else
      {
        eLogErr(E_STD_DATID_ERR, "Error setting datum ID");
        return E_STD_DATID_ERR;
      }
 
   }

   return SYS_NOMINAL;

}
/*****************************************************************************
** Function Name:
**    iStdStartStopTime
**
** Type:
**    Status_t
**
** Purpose:
**    Converts a time from string format (from configuration file) to 
**    Ttl format i.e. seconds and nanoseconds since 1970.
**
** Description:
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success;
**
** Arguments;
**    eTtlTime_t *StartTime   (out)
**       Pointer to the Ttl format start time
**    eTtlTime_t *StopTime    (out)
**       Pointer to the Ttl format stop time
**
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t iStdStartStopTime( eTtlTime_t *TtlStartPtr, eTtlTime_t *TtlStopPtr )
{
   struct tm starttime;
   struct tm stoptime;
   eStdTime_t  *StdStartPtr; /* Time to begin search */
   eStdTime_t  *StdStopPtr;  /* Time to finish search */
   char       TimeStr[E_STD_MAX_STRING_LEN]; /* String to contain timestamp */
   Status_t   Status;

   /*
   ** Clear the (internal) copy of the TZ environment variable, to prevent 
   ** calls to time manipulation functions being affected by any daylight
   ** saving issues local to the machine.
   */
   if ( getenv( "TZ" ) == NULL )
   {
      eLogInfo( "Initially TZ not set" );
   }
   else
   {
      eLogInfo( "Initially TZ=%s", getenv( "TZ" ) );
   }
   /* Set TZ to be zero */
   setenv( "TZ", "0", 1 );
   eLogInfo( "Now TZ=%s", getenv( "TZ" ) );

   StdStartPtr = &iStdGlobVar.StartTime; 
   StdStopPtr  = &iStdGlobVar.StopTime;

   starttime.tm_year  = StdStartPtr->Year - 1900;
   starttime.tm_mon   = StdStartPtr->Month -1;
   starttime.tm_mday  = StdStartPtr->Date;
   starttime.tm_hour  = StdStartPtr->Hour;
   starttime.tm_min   = StdStartPtr->Minute;
   starttime.tm_sec   = StdStartPtr->Second;
   starttime.tm_wday  = 0;
   starttime.tm_yday  = 0;
   starttime.tm_isdst = 0;

   stoptime.tm_year  = StdStopPtr->Year - 1900;
   stoptime.tm_mon   = StdStopPtr->Month -1;
   stoptime.tm_mday  = StdStopPtr->Date;
   stoptime.tm_hour  = StdStopPtr->Hour;
   stoptime.tm_min   = StdStopPtr->Minute;
   stoptime.tm_sec   = StdStopPtr->Second;
   stoptime.tm_wday  = 0;
   stoptime.tm_yday  = 0;
   stoptime.tm_isdst = 0;

   TtlStartPtr->t_sec  = mktime( &starttime );
   TtlStartPtr->t_nsec = 0;
   TtlStopPtr->t_sec   = mktime( &stoptime );  
   TtlStopPtr->t_nsec  = 0;

   /* Print the start and stop time to screen */
   Status = eTimToString( TtlStartPtr, E_STD_MAX_STRING_LEN, TimeStr);
   if( Status != SYS_NOMINAL)
   {
      eLogErr(Status,"Error converting system time to string");
      exit(EXIT_FAILURE);
   }
   eLogNotice(0,"Start time %s",TimeStr);

   Status = eTimToString( TtlStopPtr, E_STD_MAX_STRING_LEN, TimeStr);
   if( Status != SYS_NOMINAL)
   {
      eLogErr(Status,"Error converting system time to string");
      exit(EXIT_FAILURE);
   }
   eLogNotice(0,"Stop time  %s",TimeStr);
   return SYS_NOMINAL;
}
