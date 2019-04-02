/*****************************************************************************
** Module Name:
**    StdLib.c
**
** Purpose:
**    Function calls for extracting data from the Sdb. These functions
**    access the disk directly, which is quicker than retrieving data
**    using Cil messages.
** 
** Description:
**
** Authors:
**    Martin Norbury (man)
**
** Version:
**    $Id: StdLib.c,v 0.14 2006/07/12 10:57:34 mjf Exp $
**
** History:
**    $Log: StdLib.c,v $
**    Revision 0.14  2006/07/12 10:57:34  mjf
**    Ported to Cygwin.
**
**    Revision 0.13  2005/08/31 14:32:22  mjf
**    Tidied up log messages.
**
**    Revision 0.12  2005/08/31 14:18:08  sxf
**    Allow reading of gzipped sdb files.
**
**    Revision 0.11  2005/08/01 14:48:47  mjf
**    Start-of-hour calculation simplied, required for portability onto
**    Red Hat Linux.
**
**    Revision 0.10  2005/06/20 13:38:44  mjf
**    Tidied up to aid porting to Linux - no functional changes.
**
**    Revision 0.9  2005/06/17 10:23:59  mjf
**    Correction of line endings.
**
**    Revision 0.8  2005/06/17 10:08:50  mjf
**    STD_1_04 - incorporate functional changes made in STD_0_13 -> STD_0_15.
**
**    Revision 0.7  2004/10/18 15:39:28  mkb
**    Reset FirstTime flag within public library when end of
**    search has been reached to allow clients to start a new
**    search.
**
**    Revision 0.6  2004/10/15 13:48:24  mkb
**      Fix problems that make STD enter infinte loops during some
**       searches.
**       Put search results into linked list so they can be sorted
**       chronologically because the SBD does not guarantee the
**       order of records in *.sdb files.
**
**    Revision 0.5  2003/09/16 15:32:20  man
**    Update for new baseline.
**
**    Revision 0.4  2003/09/15 15:42:33  man
**    Commented out a piece of code to check if we're
**    trying to open sdb files in the future. If this is
**    true it will be captured elswhere.
**
**    Revision 0.3  2003/09/15 14:35:18  man
**    Included a command line switch in which an alternate
**    path for the sdb data can be specified.
**
**    Revision 0.2  2003/01/15 16:21:42  man
**    *** empty log message ***
**
**    Revision 0.1  2003/01/15 16:20:29  man
**    Initial revision.
**
**
**
**
*****************************************************************************/

#include <math.h>
#include <zlib.h>

/* Include files */
#include "StdPrivate.h"

/* Local function prototypes */
Status_t mStdOpenSdbFile      ( eStdTime_t Time, FILE **InFilePtr, gzFile **GzInFilePtr, char *PathPtr );
Status_t mStdConvertTime      ( eTtlTime_t InTime, eStdTime_t *OutTimePtr );
Status_t mStdReadSdbHeader    ( FILE *InFilePtr, gzFile *GzInFilePtr, size_t *NumBytes );
Status_t mStdReadSdbTimeStamp ( FILE *InFilePtr, gzFile *GzInFilePtr, eTtlTime_t *TimeHour);
Status_t mStdReadSdbChunk     ( FILE *, gzFile *, eSdbRawFmt_t *, size_t, size_t *);
Status_t mStdSetHour          ( eTtlTime_t, eTtlTime_t *);

Bool_t   mStdGzippedFile;

/* Module scope variables */
static eTtlTime_t TimeHour;

/*****************************************************************************
** Function Name:
**    eStdRetrieveData
**
** Type:
**    Status_t
**
** Purpose:
**    Retrieves a chunk of Sdb data of size E_STD_SDB_CHUNK_SIZE.
** 
** Description:
**    This function retrieves chunks of data from the Sdb. Given
**    a time range to search between, the function must generate
**    the Sdb filename to search.
**    Each time the function is called a chunk of size 
**    E_STD_SDB_CHUNK_SIZE is recovered. A static time counter
**    is used to allow future calls to this function to continue
**    from the end of the previous Sdb chunk read. This function 
**    can be called repeatadly until the Finished flag is true,
**    at which point the data with in the time range has been 
**    retrieved.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**
** Arguments:
**    eTtlTime_t    StartTime          (in)
**       The start time of the Sdb search.
**    eTtlTime_t    StopTime           (in)
**       The stop time of the sdb search.
**    char         *PathPtr            (in)
**       Pointer to alternate path for data
**    eSdbRawFmt_t *SdbDataPtr         (out)
**       Pointer to the Sdb data chunk. 
**    Bool_t       *Finished           (out)
**       Indicates if the Sdb search finished.
**
** Authors:
**    Martin Norbury (man)
**
*****************************************************************************/
Status_t eStdRetrieveData( eTtlTime_t StartTime, 
                           eTtlTime_t StopTime,       
                           char *     PathPtr,
                           eSdbRawFmt_t **SdbDataPtr, 
                           Bool_t    *FinishedPtr,
                           size_t    *NumRecordsPtr)
{
   Status_t          Status;   
   size_t            NumBytes;
   static Bool_t     NewSdbFile = TRUE;   /* Flag indicating a new file should be loaded */
   static Bool_t     FirstTime  = TRUE;   /* Flag indicating this is first call*/
   static FILE      *InFile = NULL;       /* File pointer to current Sdb file */
   static gzFile    *GzInFile = NULL;     /* File pointer to current Sdb file */
   static eTtlTime_t Time;                /* Current hour being searched */
   eStdTime_t        StdTime;
   static eSdbRawFmt_t *DataPtr;
 
   /* Set the finished flag to FALSE */
   *FinishedPtr = FALSE;

   /* Set the current time */
   if( FirstTime )
   {
      /* Allocate some memory for Sdb data */
      DataPtr = (eSdbRawFmt_t *) TTL_MALLOC( sizeof(eSdbRawFmt_t) * E_STD_SDB_CHUNK_SIZE);
      if( DataPtr == NULL )
      {
         return E_STD_MEM_ALLOC_ERR;
      }

      /* If this is the first call, set time to start time */   
      mStdSetHour(StartTime, &Time);

      /* Ensure file is opened afresh. */
      *FinishedPtr    = FALSE;
      NewSdbFile      = TRUE;
      InFile          = NULL;
      GzInFile        = NULL;
      
      /* Initialise the TimeHour variable */
      TimeHour.t_sec  = StartTime.t_sec;
      TimeHour.t_nsec = StartTime.t_nsec;

      FirstTime       = FALSE;
   }
   
   /* Initialise the data array */
   if( DataPtr != NULL )
   {
      *SdbDataPtr = DataPtr;
   }
   else
   {
      FirstTime     = TRUE;
      return E_STD_MEM_ALLOC_ERR;
   }

   /* Check to see if we've passed the stop time */
   if( Time.t_sec > StopTime.t_sec )
   {
      TTL_FREE( *SdbDataPtr );
      *FinishedPtr  = TRUE;
      FirstTime     = TRUE;
      /* Ensure file is closed properly. */
      if ( mStdGzippedFile == FALSE )
      { 
         if (InFile != NULL)
         {
           fclose(InFile);
         }
      }
      else
      {
         if (GzInFile != NULL)
         {
#if 0
           gzclose(GzInFile);
#endif
         }
      }

      return SYS_NOMINAL;
   }
   
   /* Check to see if we need to open new file */
   if( NewSdbFile )
   {
      NewSdbFile = FALSE;     

      /* Try and open a file until we find one */
      do
      {

         /* 
         ** Check to see if we're trying to open a file
         ** which is past the stop time
         */
         if( Time.t_sec > StopTime.t_sec )
         {
            /* Not an error just return finished */
            TTL_FREE( *SdbDataPtr );

            /* Ensure file is closed properly. */
            NewSdbFile  = TRUE;
            if ( mStdGzippedFile == FALSE )
            {
               if (InFile != NULL)
               {
                 fclose(InFile);
               }
            }
            else
            {
               if (GzInFile != NULL)
               {
                 gzclose(GzInFile);
               }
            }

            FirstTime     = TRUE;
            *FinishedPtr  = TRUE;
            eLogDebug("Passed stop time.");
            return SYS_NOMINAL;
         }

         /* Convert from Ttl to Std time format */
         Status = mStdConvertTime( Time, &StdTime );
         if( SYS_NOMINAL != Status )
         {
            TTL_FREE( *SdbDataPtr );

            /* Ensure file is closed properly. */
            NewSdbFile    = TRUE;
            if ( mStdGzippedFile == FALSE )
            {
               if (InFile != NULL)
               {
                 fclose(InFile);
               }
            }
            else
            {
               if (GzInFile != NULL)
               {
                 gzclose(GzInFile);
               }
            }

            FirstTime     = TRUE;
            return Status;
         }

         eLogDebug("Opening new file");
         Status = mStdOpenSdbFile( StdTime, &InFile, &GzInFile, PathPtr );

         if( ( InFile == NULL ) && ( GzInFile == NULL ) )
         {
            /* Advance the time by one hour */
            Time.t_sec += E_STD_SECONDS_PER_HOUR;          
         }

      }while( (InFile == NULL) && (GzInFile == NULL) );

      /* Read the Sdb header (not needed) */
      Status = mStdReadSdbHeader ( InFile, GzInFile, &NumBytes );
      if ( SYS_NOMINAL != Status )
      {
        TTL_FREE( *SdbDataPtr );

        /* Ensure file is closed properly. */
        NewSdbFile    = TRUE;
        if ( mStdGzippedFile == FALSE )
        {
           if (InFile != NULL)
           {
              fclose(InFile);
           }
        }
        else
        {
           if (GzInFile != NULL)
           {
              gzclose(GzInFile);
           }
        }

        FirstTime     = TRUE;
        return Status;
      }

      /* Read the Sdb hour */
      Status = mStdReadSdbTimeStamp ( InFile, GzInFile, &TimeHour );
      if ( SYS_NOMINAL != Status )
      {
        TTL_FREE( *SdbDataPtr );

        /* Ensure file is closed properly. */
        NewSdbFile  = TRUE;

        if ( mStdGzippedFile == FALSE )
        {
           if (InFile != NULL)
           {
             fclose(InFile);
           } 
        }
        else
        {
           if (GzInFile != NULL)
           {
              gzclose(GzInFile);
           }
        }

        FirstTime     = TRUE;
        return Status;
      }
   }

   /* Retrieve chunk of Sdb data */
   Status = mStdReadSdbChunk ( InFile, GzInFile, *SdbDataPtr, E_STD_SDB_CHUNK_SIZE, NumRecordsPtr);
   if( (SYS_NOMINAL != Status) && (Status != E_STD_EOF) )
   {
      TTL_FREE( *SdbDataPtr );

      /* Ensure file is closed properly. */
      NewSdbFile    = TRUE;
      if ( mStdGzippedFile == FALSE )
      {
         if (InFile != NULL)
         {
           fclose(InFile);
         }
      }
      else
      {
         if (GzInFile != NULL)
         {
           gzclose(GzInFile);
         }
      }

      FirstTime     = TRUE;
      return Status;
   }

   /* Check to see if we are at the end of sdb file */
   if( Status == E_STD_EOF )
   {
      /* Next time we need to load new Sdb file */
      eLogDebug("End of file. Closing file.");
      if ( mStdGzippedFile == FALSE )
      {
         fclose( InFile) ;
      }
      else
      {
         gzclose( GzInFile) ;
      }

      /* Advance the time by one hour */
      Time.t_sec += E_STD_SECONDS_PER_HOUR;
      InFile     = NULL; 
      NewSdbFile = TRUE;
   }

   return SYS_NOMINAL;
}

/*****************************************************************************
** Function Name:
**    eStdSearchData
**
** Type:
**    Status_t
**
** Purpose:
**    Searches a line of raw sdb data for requested source/datum pairs.
** 
** Description:
**    This function accepts a single line of raw Sdb data and checks to
**    see if it matches requested source/datum pairs. If a match is found
**    the function returns the timestamp and value of the source/datum
**    pair.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**
** Arguments:
**    Int32_t       CilId          (in)
**       Cil Id to search for.
**    Int32_t       DatId          (in)
**       Data Id to search for.
**    eSdbRawFmt_t  SdbLine        (in)
**       A line from the Sdb chunk.
**    eTtlTime_t   *TimeStamp      (out)
**       Time stamp when datum was submitted.
**    Int32_t      *Value          (out)
**       Value of datum.  
**    Bool_t       *GotData        (out)
**       Flag indicating if we've found data.
** Authors:
**    Martin Norbury (man)
**
*****************************************************************************/
Status_t eStdSearchData( Int32_t CilId, 
                         Int32_t DatId, 
                         eSdbRawFmt_t SdbLine,
                         eTtlTime_t *TimeStampPtr,
                         Int32_t *Value,
                         Bool_t *GotMatch )
{             
   Status_t      Status;
   eTtlTime_t    TimeOffset;
   eSdbSngReq_t  SrcDat;
   char       TimeStr[E_STD_MAX_STRING_LEN];
       
   /* Decode the line of Sdb data */
   Status = eSdbStoreIdDecode(&(SdbLine.Code), &SrcDat);    
   if ( SYS_NOMINAL != Status )
   {
      return Status;
   }   

   /* Check to see if source/datum pairs match */
   if( (CilId != SrcDat.SourceId) || (DatId != SrcDat.DatumId) )
   {  
      /* Not what we're looking for so return */
      *GotMatch = FALSE;
      return SYS_NOMINAL;
   }

   /* Calculate the time stamp */
   TimeOffset.t_sec  = SdbLine.TimeOffset / E_TTL_MICROSECS_PER_SEC;
   TimeOffset.t_nsec = (SdbLine.TimeOffset % (long)E_TTL_MICROSECS_PER_SEC) *
                       ((long)E_TTL_NANOSECS_PER_SEC / (long)E_TTL_MICROSECS_PER_SEC);
   
   Status = eTimSum(&TimeHour, &TimeOffset, TimeStampPtr);
   if ( Status != SYS_NOMINAL)
   {
      return Status;
   }

   /* Get the time of datum */
   Status = eTimToString( TimeStampPtr, E_STD_MAX_STRING_LEN, TimeStr);
   if( Status != SYS_NOMINAL)
   {
      eLogErr(Status,"Error converting system time to string");           
      exit( EXIT_FAILURE );
   }

   *GotMatch = TRUE;

   *Value = SdbLine.Value;

   return SYS_NOMINAL;
}
/*****************************************************************************
** Function Name:
**    mStdConvertTime
**
** Type:
**    Status_t
**
** Purpose:
**    Convert time from eTtlTime_t format to eStdTime_t format.
**
** Description:
**    eTtlTime is elapsed time in seconds and nanoseconds relative
**    to some arbitrary point in the past. eSdtTime is expressed in
**    year, month, day, hour, minute, second and milliseconds.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success. 
**
** Arguments:
**    iTtlTime_t    InTime      (in)
**       Input time format.
**    eStdTime_t   *OutTimePtr  (out)
**       Output time format.
**    
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t mStdConvertTime( eTtlTime_t InTime, eStdTime_t *OutTimePtr )
{
   Status_t   Status;       /* Status of function calls */
   char       TimeStr[E_STD_MAX_STRING_LEN]; /* String to store time */

   /* Convert the Ttl format time to a string */
   Status = eTimToString( &InTime, E_STD_MAX_STRING_LEN, TimeStr);
   if( Status != SYS_NOMINAL)
   {
      eLogErr(Status,"Error converting system time to string");
      return Status;
   }
   sscanf( TimeStr,"%d/%d/%d %d:%d:%d.%d", &(OutTimePtr->Date), &(OutTimePtr->Month),
                                           &(OutTimePtr->Year), &(OutTimePtr->Hour), 
                                           &(OutTimePtr->Minute), &(OutTimePtr->Second), 
                                           &(OutTimePtr->MilliSecond));

   return SYS_NOMINAL;
}
/*****************************************************************************
** Function Name:
**    mStdReadSdbHeader
**
** Type:
**    Status_t
**
** Purpose:
**    Read the header portion of the Sdb file.
**
** Description:
**    Reads the header portion of the Sdb file which is discarded as it is
**    not required.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success or E_STD_READ_HEAD_ERR if there was
**       a problem reading the header data i.e. the number of bytes read by
**       fread is not equal to the expected header size.
**
** Arguments:
**    FILE *InFilePtr             (in)
**       Pointer to the current output file.
**    size_t *NumBytes            (out)
**       Pointer to the number of bytes read by fread.
**
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t mStdReadSdbHeader ( FILE *InFilePtr, gzFile *GzInFilePtr, size_t *NumBytes)
{
   char Buf[E_STD_BUFSIZE]; /* Dummy buffer */

   if( (mStdGzippedFile == FALSE) &&
       (InFilePtr == NULL ) &&
       (GzInFilePtr == NULL ) )
   {
      return E_STD_NULL_FILE_POINTER;
   }

   /* Read the header */
   if ( mStdGzippedFile == FALSE )
   {
      *NumBytes = fread(Buf, sizeof(char), E_STD_FILE_HDR_SIZE, InFilePtr);
   }
   else
   {
      *NumBytes = gzread( GzInFilePtr, Buf, sizeof(char) * E_STD_FILE_HDR_SIZE );
   }

   if(*NumBytes != E_STD_FILE_HDR_SIZE)
   {
      return E_STD_READ_HEAD_ERR; 
   }   

   return SYS_NOMINAL;
}
/*****************************************************************************
** Function Name:
**    mStdReadSdbTimeStamp
**
** Type:
**    Status_t
**
** Purpose:
**    Read the timestamp for the current Sdb file.
**
** Description:
**    Read the timestamp for the current Sdb file. All Sdb time measurements
**    are read as an offset relative to this timestamp.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success or E_STD_READ_TIME_ERR if there was
**       a problem reading the timestamp.
**
** Arguments:
**    File        *InFilePtr (in)
**       Pointer to current Sdb file.
**    eTtlTime_t  *TimeHour  (out)
**       Timestamp of the current Sdb file.
**
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t mStdReadSdbTimeStamp ( FILE *InFilePtr, gzFile *GzInFilePtr, eTtlTime_t *TimeHour )
{
   int NumRecords;    /* Number of records read in by fread */

   if( (mStdGzippedFile == FALSE) &&
       (InFilePtr == NULL ) &&
       (GzInFilePtr == NULL ) )
   {
      return E_STD_NULL_FILE_POINTER;
   }

   /* Get the file "time-stamp", from which all other times are offset. */
   if ( mStdGzippedFile == FALSE )
   {
      NumRecords = fread(&(TimeHour->t_sec), sizeof(TimeHour->t_sec), 1, InFilePtr);
   }
   else
   {
      NumRecords = gzread( GzInFilePtr, &(TimeHour->t_sec), sizeof(TimeHour->t_sec) * 1) / sizeof(TimeHour->t_sec);
   }

   if(NumRecords != 1)
   {
      return E_STD_READ_TIME_ERR;
   }
   
   TimeHour->t_nsec = 0;

   return SYS_NOMINAL;
}
/*****************************************************************************
** Function Name:
**    mStdReadSdbChunk
**
** Type:
**    Status_t
**
** Purpose:
**    Read in a chunk of the SDB file.
**
** Description:
**    Reads in an SDB file a chunk at a time. Reading in a large section of 
**    the SDB file is faster than multiple calls to the fread statement.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success. Currently this function cannot fail.
**
** Arguments:
**    FILE *InFilePtr             (in)
**       Pointer to the current loaded SDB file.
**    eSdbRawFmt_t *SdbChunkPtr   (in)
**       Pointer to structure to store the SDB date read in.
**    size_t  SdbChunkSize        (in)
**       Number of eSdbRawFmt elements to be read in.
**    size_t *NumRecords          (out)
**       Number of eSdbRawFmt elements actually read in.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**    man: Martin Norbury
**
*****************************************************************************/
Status_t mStdReadSdbChunk ( FILE         *InFilePtr,
                            gzFile       *GzInFilePtr,
                            eSdbRawFmt_t *SdbChunkPtr,
                            size_t        SdbChunkSize,
                            size_t       *NumRecords)
{

   if( (mStdGzippedFile == FALSE) &&
       (InFilePtr == NULL ) &&
       (GzInFilePtr == NULL ) )
   {
      return E_STD_NULL_FILE_POINTER;
   }

   if ( mStdGzippedFile == FALSE )
   {
      *NumRecords = fread (SdbChunkPtr, sizeof (eSdbRawFmt_t), SdbChunkSize, InFilePtr);
   }
   else
   {
      *NumRecords = gzread ( GzInFilePtr, SdbChunkPtr, sizeof (eSdbRawFmt_t) * SdbChunkSize ) / sizeof (eSdbRawFmt_t) ;
   }

   eLogDebug("Read %d records from time point %x.", NumRecords, SdbChunkPtr->TimeOffset);

   /* Check for end of file. */
   if ( mStdGzippedFile == FALSE )
   {
      if (feof(InFilePtr))
      {
        return E_STD_EOF;
      }
      else
      {
        return SYS_NOMINAL;
      }
   }
   else
   {
      if (gzeof(GzInFilePtr))
      {
        return E_STD_EOF;
      }
      else
      {
        return SYS_NOMINAL;
      }
   }
}
/*****************************************************************************
** Function Name:
**    mStdOpenSdbFile
**
** Type:
**    Status_t
**
** Purpose:
**    Open an Sdb file from disk.
**
** Description:
**    This function generates the name of the Sdb file, based on the
**    year, month, day and hour. The function then returns the file
**    pointer.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**       Returns E_STD_FILE_OPEN_ERR if failed to open Sdb file 
**
** Arguments:
**    eStdTime_t    Time        (in)
**       Time used to generate filename.
**    FILE         **InFilePtr  (out)
**       File pointer to sdb file.
**    
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t mStdOpenSdbFile( eStdTime_t   Time, 
                          FILE       **InFilePtr,
                          gzFile     **GzInFilePtr,
                          char        *PathPtr )
{
   char SdbFile[ E_STD_STRING_LEN ]; /* String to hold current sdb filename*/
   char SdbFilePath[ E_STD_STRING_LEN ];

   strncpy( SdbFilePath, PathPtr, E_STD_MAX_STRING_LEN);
   sprintf(SdbFile,"%.2d%.2d%.2d%.2d.sdb",
                    Time.Year, Time.Month,
                    Time.Date, Time.Hour);
   eLogNotice(0,"Starting to process time index %.8s", SdbFile);
   strcat (SdbFilePath, SdbFile);

   if( (*InFilePtr = fopen(SdbFilePath, "rb")) == NULL)
   {
      /* If unable to open file, try gzipped version */
      strcat( SdbFilePath, I_STD_EXT_GZIP  );
      if( (*GzInFilePtr = gzopen(SdbFilePath, "rb")) == NULL) 
      {
         eLogNotice(E_STD_FILE_OPEN_ERR,"Unable to open file %s[%s]", SdbFile, I_STD_EXT_GZIP );
         return E_STD_FILE_OPEN_ERR;
      }
      else
      {
         mStdGzippedFile = TRUE;
      }
   }
   else
   {
      mStdGzippedFile = FALSE;
   }
   
   return SYS_NOMINAL;
} 

/*****************************************************************************
** Function Name:
**    mStdSetHour
**
** Type:
**    Status_t
**
** Purpose:
**    Find the time in seconds truncated to the start of the hour.
**
** Description:
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**
** Arguments:
**    eTtlTime_t    Time        (in)
**       Time in seconds.
**    eTtlTime_t   *TimeHourPtr (out)
**       Time truncated to start of hour
**    
** Authors:
**    Mark Bowman (mkb)
**
*****************************************************************************/
Status_t mStdSetHour( eTtlTime_t Time, eTtlTime_t *TimeHourPtr)
{

   TimeHourPtr -> t_nsec = 0; 
#if 0
   TimeHourPtr -> t_sec = ((Int32_t) floor((double)Time.t_sec / (double)E_TTL_SECS_PER_HOUR)) 
                          * E_TTL_SECS_PER_HOUR;
#endif

   TimeHourPtr -> t_sec = ( Time.t_sec / (Int32_t) E_TTL_SECS_PER_HOUR ) * E_TTL_SECS_PER_HOUR;

   return SYS_NOMINAL;
}

