/*****************************************************************************
** Module Name:
**     StdOutput.c
**
** Purpose:
**     Contains functions to output data from Std to disk or screen.
**
** Description:     
**     Formerly StdSdbRead.c
**
** Authors:
**    Martin Norbury (man)
**
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
**
** Version:
**      $Id: StdOutput.c,v 0.5 2006/06/26 11:23:45 mjf Exp $    
**
** History:
**      $Log: StdOutput.c,v $
**      Revision 0.5  2006/06/26 11:23:45  mjf
**      Change order of source/datum name in output file header.
**
**      Revision 0.4  2005/06/28 14:01:34  mjf
**      Correction to title output for '-matlab' option.
**
**      Revision 0.3  2005/06/17 10:08:50  mjf
**      STD_1_04 - incorporate functional changes made in STD_0_13 -> STD_0_15.
**
**      Revision 0.2  2004/10/15 13:48:24  mkb
**        Fix problems that make STD enter infinte loops during some
**         searches.
**         Put search results into linked list so they can be sorted
**         chronologically because the SBD does not guarantee the
**         order of records in *.sdb files.
**
**      Revision 0.1  2003/01/15 16:15:04  man
**      Formerly StdSdbRead.c modified to use Std
**      library calls.
**
**
**                   
*****************************************************************************/

/* Local include files */
#include "StdPrivate.h"

/* Local function prototypes */

/*****************************************************************************
** Function Name:
**    iStdWriteHeader
**
** Type:
**    Status_t
**
** Purpose:
**    Write the top line of the output file. 
**
** Description:
**    Writes the first line of the output file, containing information on
**    the initial search parameters, the contents of each column, units etc.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success or E_STD_FILE_WRITE_ERR if there was
**       a problem writing to disk.
**
** Arguments:
**    FILE *InFilePtr             (in)
**       Pointer to the current output file.
**
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t iStdWriteHeader ( FILE *OutFilePtr )
{
   int         ret;        /* Return value of the fprint call */
   int         i;          /* counter to cycle through source datum pairs*/
   iStdData_t *StdDataPtr; /* Pointer to requested data id's */ 
   
   /* Initialise pointer to data */
   StdDataPtr = iStdGlobVar.StdData;
   
   /* Write the column headers */
   if ( iStdGlobVar.WriteMatlab == TRUE )
   {
      ret = fprintf(OutFilePtr,"%%Seconds\t");
   }
   else if ( iStdGlobVar.WriteGnuplot == TRUE )
   {
      ret = fprintf(OutFilePtr,"#Date\tTime\tSeconds\t");
   }
   else
   {
      ret = fprintf(OutFilePtr,"Date\tTime\tSeconds\t");
   }

   if( ret < 0 )
   {
      /* Error writing to file */
      return E_STD_FILE_WRITE_ERR;
   }

   /* 
   ** Write the source and datum id's along with the
   ** appropriate hexadecimal value.
   */
   for(i=0; i<iStdGlobVar.NumDataSearch; i++)
   {
      ret = fprintf(OutFilePtr,"%s(0x%x),%s(0x%x) \t",
                (StdDataPtr+i)->DatumName, (StdDataPtr+i)->DatumId,
                (StdDataPtr+i)->SourceName, (StdDataPtr+i)->SourceId);
      if( ret < 0 )
      {
         /* Error writing to file */
         return E_STD_FILE_WRITE_ERR;
      }
   }

   /* Insert a carriage return ready for the data*/
   ret = fprintf(OutFilePtr,"\n");
   if( ret < 0 )
   {
      /* Error writing to file */
      return E_STD_FILE_WRITE_ERR;
   }

   return SYS_NOMINAL;

}
/*****************************************************************************
** Function Name:
**    iStdWriteToScreen
**
** Type:
**    Status_t
**
** Purpose:
**    Write the details of any Sdb data satisfying the search criteria.
**
** Description:
**    Write Sdb data which satisfies the search criteria to screen. 
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
Status_t iStdWriteToScreen ( void )
{
   int                j;
   iStdMatchedData_t *IndexPtr;
   iStdData_t        *StdDataPtr; /* Pointer to requested data id's */
   Bool_t             Continue =TRUE;

   /* Initialise data pointer */
   StdDataPtr = iStdGlobVar.StdData;

   /*
   ** Wind back through linked list to find first entry. 
   */
   IndexPtr = iStdLastMatchedPtr;
   while (Continue)
   {
      if (IndexPtr->PreviousPtr != NULL)
      {
        IndexPtr = IndexPtr->PreviousPtr;
      }
      else
      {
        Continue = FALSE;
      }
   }

   /*
   ** Loop through the source datum pairs to see which 
   ** one contains the data.
   */

   Continue = TRUE;
   while (Continue)
   { 
      for(j=0;j<iStdGlobVar.NumDataSearch;j++)
      {
         eLogInfo(
                  "%s %s,%s (0x%2.2x) = %8.8x %d",
                  iStdGlobVar.LastTimeStr, 
                  (StdDataPtr+j)->SourceName, (StdDataPtr+j)->DatumName,
                  (StdDataPtr+j)->SourceId, (StdDataPtr+j)->DatumId,
                  IndexPtr->Value[j]
                 );
      }

      if (IndexPtr->NextPtr != NULL)
      {
        IndexPtr = IndexPtr->NextPtr;
      }
      else
      {
        Continue = FALSE;
      }

   }
 
   return SYS_NOMINAL;   

}
/*****************************************************************************
** Function Name:
**    iStdWriteToFile
**
** Type:
**    Status_t
**
** Purpose:
**    Write the details of any Sdb data satisfying the search criteria.
**
** Description:
**    Write Sdb data which satisfies the search criteria to file. Source
**    datum pairs have their own columns. If no data is present for a 
**    particular source datum pair, a tab is inserted to maintain the
**    tab delimeted format.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success. 
**
** Arguments:
**    FILE *OutFilePtr             (in)
**       Pointer to output file.
**    char *TimeStr                (in)
**       String containing the current timestamp
**
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t iStdWriteToFile ( FILE *OutFilePtr )
{
   int                j;            /* Counter to cycle through search datum id's*/
   eStdTime_t         StdTime;      /* The Lst time stamp */
   iStdData_t        *StdDataPtr;   /* Pointer to requested data id's */
   iStdMatchedData_t *IndexPtr;
   Bool_t             Continue =TRUE;
   char               TimeStr[E_STD_MAX_STRING_LEN]; /* String to contain timestamp */
   Status_t           Status;
   double             TimeSecs;

   /* Initialise the Sdb data pointer */
   StdDataPtr = iStdGlobVar.StdData;

   /*fprintf(OutFilePtr, "%s", iStdGlobVar.TimeStr);*/
   eLogDebug("Writing to file");

   /* Initialise data pointer */
   StdDataPtr = iStdGlobVar.StdData;

   /*
   ** Wind back through linked list to find first entry. 
   */
   IndexPtr = iStdLastMatchedPtr;
   while (Continue)
   {
      if (IndexPtr->PreviousPtr != NULL)
      {
        IndexPtr = IndexPtr->PreviousPtr;
      }
      else
      {
        Continue = FALSE;
      }
   }

   /*
   ** Loop through the source datum pairs to see which 
   ** one contains the data.
   */

   Continue = TRUE;
   while (Continue)
   {
      /* Print the start and stop time to screen */
      Status = eTimToString( &IndexPtr->TimeStamp, E_STD_MAX_STRING_LEN, TimeStr);
      if( Status != SYS_NOMINAL)
      {
        eLogErr(Status,"Error converting time to string");
        exit(EXIT_FAILURE);
      }

      sscanf(TimeStr,"%d/%d/%d %d:%d:%d.%d",&(StdTime.Year),&(StdTime.Month),&(StdTime.Date),
                                         &(StdTime.Hour),&(StdTime.Minute),
                                         &(StdTime.Second),&(StdTime.MilliSecond));

      /* Print the time stamp */
      TimeSecs = (double) IndexPtr->TimeStamp.t_sec + 
                           ( (double) IndexPtr->TimeStamp.t_nsec / E_TTL_NANO_PER_UNIT );

      if ( iStdGlobVar.WriteMatlab == TRUE )
      {
         fprintf(OutFilePtr, "%f\t", 
                 TimeSecs);
      }
      else
      {
         fprintf(OutFilePtr, "%.2d/%.2d/%.2d\t%.2d:%.2d:%.2d.%.3d\t%f\t", 
                 StdTime.Year, StdTime.Month, StdTime.Date, 
                 StdTime.Hour, StdTime.Minute, StdTime.Second, StdTime.MilliSecond,
                 TimeSecs);
      }

      for(j=0;j<iStdGlobVar.NumDataSearch;j++)
      {
         if( IndexPtr->SetFlag[j] )
         {
            /* We've got some data so print it to file */
            fprintf(OutFilePtr,"%d\t",IndexPtr->Value[j]);
         }
         else
         {
            /* No data for this datum id, so insert tab */
            if ( ( iStdGlobVar.WriteMatlab == TRUE ) ||
                 ( iStdGlobVar.WriteGnuplot == TRUE ) )
            {
               fprintf(OutFilePtr,"NaN\t");
            }
            else
            {
               fprintf(OutFilePtr,"\t");
            }
         }
      }

      if (IndexPtr->NextPtr != NULL)
      {
        IndexPtr = IndexPtr->NextPtr;
      }
      else
      {
        Continue = FALSE;
      }

      fprintf(OutFilePtr,"\n");

   }
 
   return SYS_NOMINAL;

}
    
/*****************************************************************************
** Function Name:
**    iStdCompareTime
**
** Type:
**    Status_t
**
** Purpose:
**    Compare two times to decide if the new line of sdb data should be
**    printed on a new line in the file.
**
** Description:
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success. 
**
** Arguments:
**   eTtlTime_t    TimeNow     (in)
**      The current time stamp
**   eTtlTime_t   *TimeOld     (in/out)
**      The old time stamp
**   Bool_t *NewTime 
**       Pointer to flag indicating if we've got a new time.
**
** Authors:
**    man: Martin Norbury
**
*****************************************************************************/
Status_t iStdCompareTime ( eTtlTime_t LastTime, eTtlTime_t ThisTime, Bool_t *NewTime)
{
   Status_t Status;
   static Bool_t FirstTime = TRUE;
   eTtlTime_t TimeDiff;

   if( FirstTime )
   {
       FirstTime = FALSE;
      *NewTime = FALSE;
      return SYS_NOMINAL;
   }

   Status = eTimDifference( &LastTime, &ThisTime, &TimeDiff);
   if( SYS_NOMINAL != Status)
   {
      return Status;
   }

   if( (TimeDiff.t_sec == 0) && (TimeDiff.t_nsec == 0))
   {
      *NewTime = FALSE;
   }
   else
   {
      *NewTime = TRUE;
   }

   return SYS_NOMINAL;
}
