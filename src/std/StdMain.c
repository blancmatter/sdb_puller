/*****************************************************************************
** Module Name:
**     StdMain.c
**
** Purpose:
**     Program to dump the contents of an Sdb file to stdout or a text file.
**
** Description:
**    This test program will list the contents of any System Database
**    (SDB) storage file (*.sdb), printing the information to standard
**    output or a user define file.
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
**      $Id: StdMain.c,v 0.15 2006/07/12 10:57:34 mjf Exp $
**
** History:
**      $Log: StdMain.c,v $
**      Revision 0.15  2006/07/12 10:57:34  mjf
**      Ported to Cygwin.
**
**      Revision 0.14  2006/06/26 11:23:45  mjf
**      Flush data retrieved so far to an output file in the event of the
**      retrieval encountering an error.
**
**      Revision 0.13  2005/06/21 13:50:39  mjf
**      Addition of a command-line switch to specify a 'stride' in seconds to
**      ensure extracted measurements are a specified distance apart in time.
**
**      Revision 0.12  2005/06/20 13:38:44  mjf
**      Tidied up to aid porting to Linux - no functional changes.
**
**      Revision 0.11  2005/06/17 10:08:50  mjf
**      STD_1_04 - incorporate functional changes made in STD_0_13 -> STD_0_15.
**
**      Revision 0.10  2004/10/15 13:48:24  mkb
**        Fix problems that make STD enter infinte loops during some
**         searches.
**         Put search results into linked list so they can be sorted
**         chronologically because the SBD does not guarantee the
**         order of records in *.sdb files.
**
**      Revision 0.9  2004/04/21 09:18:11  sxf
**      Initialise time difference variable to prevent incorrect tight loop.
**
**      Revision 0.8  2004/04/02 15:25:21  mjf
**      No functional changes, ported to QNX v6 by SXF.
**
**      Revision 0.7  2003/09/15 15:42:33  man
**      Included a summary of the number of lines of data
**      retreived.
**
**      Revision 0.6  2003/09/15 14:35:18  man
**      Included a percentage complete indicator and lines
**      to filter data before and after start and finish
**      time respectively.
**
**      Revision 0.5  2003/08/15 13:23:55  man
**      Display time range for data extraction.
**
**      Revision 0.4  2003/01/15 16:08:34  man
**      Modified Std code to use Std library functions.
**
**      Revision 0.3  2002/10/03 13:37:20  man
**      Updated the code to print datum with same timestamp
**      on the same line in the output file.
**
**      Revision 0.2  2002/08/14 11:56:27  man
**      General tidy up for new baseline and read
**      Sdb data in chunks to make data retrieval faster.
**
**      Revision 0.1  2002/07/08 10:13:15  man
**      Initial revision.
**
**
**
*****************************************************************************/

#define E_STD_MAIN_C

#include "StdPrivate.h"

int main ( int argc, char *argv[])
{

   printf("You have recompiled and rebuild. Well done!!! This is Main function in StdMain.c\n");
   /* Local variables */
   Status_t       Status;                          /* Function return status */
   FILE          *OutFilePtr;                      /* File pointer for output file */
   eTtlTime_t     StartTime;                       /* Start search time */
   eTtlTime_t     StopTime;                        /* Stop search time */
   eTtlTime_t     TimeStamp;                       /* Timestamp of current Sdb datum */
   eSdbRawFmt_t  *SdbDataPtr;                      /* Pointer to chunk of Sdb data */
   eSdbRawFmt_t   SdbLine;                         /* Current line of Sdb */
   Bool_t         Finished;                        /* Flag to indicate search has finished */
   Int32_t        Value;                           /* Value of datum retrieved from Sdb */
   Bool_t         GotMatch;                        /* Flag indicating datum has been found */
   Int32_t        i;                               /* Counter */
   Int32_t        j;                               /* Counter stepping through source/datum pairs */
   Int32_t        CilId;                           /* Current Cil Id to search Sdb for */
   Int32_t        DatId;                           /* Current Datum Id to search Sdb for */
   Int32_t        CurrentLine = 0;                 /* Current line of Sdb chunk */
   iStdData_t    *StdDataPtr;                      /* Pointer to requested data id's */
   size_t         NumRecords = 0;                  /* Number of records retrieved. */
   Bool_t         DataWritePending = FALSE;        /* Matched data not written yet. */
   char           TimeStr[E_STD_MAX_STRING_LEN];   /* String to contain timestamp */

   /* Initialise the CLU and parse the command line */
   Status = iStdCluInit ( argc, argv );
   if( SYS_NOMINAL != Status )
   {
      printf("Error in StdCluInit\n");
      exit (EXIT_FAILURE);
   }

   /* Read the configuration file */
   Status = iStdReadConfig ( eCluCommon.ConfigFile );
   if( SYS_NOMINAL != Status )
   {
      eLogInfo("Can't read configuration file %s",eCluCommon.ConfigFile);
   }

   /* Work out Datum/Source name from Datum/Source Id */
   Status = iStdSrcDtmName ( );
   if( SYS_NOMINAL != Status )
   {
      eLogErr(Status, "Error getting source/datum names/id's");
      exit( EXIT_FAILURE );
   }

   /* Open the output file for writing tab formatted data to */
   OutFilePtr = fopen(iStdGlobVar.OutFile,"w");
   if( OutFilePtr == NULL)
   {
      eLogErr(E_STD_FILE_WRITE_ERR,"Error: unable to open output file");
      exit( EXIT_FAILURE );
   }

   Status = iStdWriteHeader( OutFilePtr );
   if( SYS_NOMINAL != Status )
   {
      eLogErr(Status, "Error writing to output file");
   }

   /* Initialise the pointer to the data array */
   StdDataPtr = iStdGlobVar.StdData;

   /*
   ** Work out the start and stop times i.e. convert the
   ** start and stop times read from the configuration file
   ** as a string into Ttl time format.
   */
   Status = iStdStartStopTime( &StartTime, &StopTime );

   /* Set the initial 'next stride' time to be the start time */
   for(i=0; i<E_STD_MAX_SEARCH_DATA; i++)
   {
      memcpy( &( (StdDataPtr+i)->NextStrideTime ), &StartTime,
              sizeof( (StdDataPtr+i)->NextStrideTime ) );
   }

   do
   {
      /* Reset the line index */
      CurrentLine = 0;

      /* Retrieve a chunk of Sdb data determined by E_STD_SDB_CHUNK_SIZE */
      Status = eStdRetrieveData( StartTime, StopTime,iStdGlobVar.DatPath,
                                &SdbDataPtr, &Finished, &NumRecords );
      if( SYS_NOMINAL != Status )
      {
         eLogErr(Status,"Error retrieving Sdb data");
         exit(EXIT_FAILURE);
      }
      if (Finished)
      {
        eLogDebug("Retrieved data. Finshed flag is TRUE");
      }
      else
      {
        eLogDebug("Retrieved data. Finshed flag is FALSE");
      }

      /* Loop through each line of the returned Sdb data */
      while( CurrentLine < (int) NumRecords )
      {
         SdbLine = SdbDataPtr[CurrentLine];

         /*
         ** Check see if this line matches any of the source
         ** datum pairs specified in the configuration file
         */
         for(j=0;j<iStdGlobVar.NumDataSearch;j++)
         {
            CilId = (StdDataPtr+j)->SourceId;
            DatId = (StdDataPtr+j)->DatumId;

            /* Check the current line of Sdb data to see it's a CilId and DatumId value*/
            Status = eStdSearchData(CilId,DatId, SdbLine,&TimeStamp,&Value,&GotMatch );
            if( SYS_NOMINAL != Status )
            {
               eLogErr(Status,"Error searching data");
               exit(EXIT_FAILURE);
            }

            /* Check to see if data is within time range we want */
            if(TimeStamp.t_sec<StartTime.t_sec)
               GotMatch = FALSE;
            if(TimeStamp.t_sec>StopTime.t_sec)
               GotMatch = FALSE;

            /*
            ** If we've got a match we save it to a linked list to allow
            ** us to print all source/datum pairs with the same timestamp at
            ** a later date
            */
            if( GotMatch )
            {

               /* Get the time of datum */
               Status = eTimToString( &TimeStamp, E_STD_MAX_STRING_LEN, TimeStr);
               if( Status != SYS_NOMINAL)
               {
                  eLogErr(Status,"Error converting system time to string");
                  exit(EXIT_FAILURE);
               }

               /*
               ** Only store this datum if it's the first following the start of
               ** a new 'stride' for a given datum.
               */
               if ( TimeStamp.t_sec >= (StdDataPtr+j)->NextStrideTime.t_sec )
               {
                  /* Increment the next-stride-time by the stride, could be 0 */
                  (StdDataPtr+j)->NextStrideTime.t_sec += iStdGlobVar.Stride;

                  eLogInfo("Got data match. Writing data %s %d",TimeStr,Value);
                  DataWritePending = TRUE;

                  /* Find place in linked list for this time-stamp. */
                  Status = iStdStoreData( TimeStamp, j, Value );
                  if( Status != SYS_NOMINAL)
                  {
                     eLogErr(Status,"Error storing matching data.");
                     exit(EXIT_FAILURE);
                  }
                  else
                  {
                     iStdLinesOfData++;
                     (StdDataPtr+j)->NumOfPoints++;
                  }
               }
            }

         }/* End of j for loop */

         CurrentLine++;

      }/* End of CurrentLine while loop */

   }while( !Finished );

   /*
   ** Print the data if we have any and if we're about
   ** to go to a new timestamp or if we are about to finish and
   ** data is waiting to be written.
   */
   if( DataWritePending )
   {

      Status = iStdWriteToScreen ( );
      if( SYS_NOMINAL != Status )
      {
         eLogErr(Status, "Error writing to screen.");
      }

      /* Output the data to disk */
      Status = iStdWriteToFile ( OutFilePtr );
      if( SYS_NOMINAL != Status )
      {
         eLogErr(Status, "Error writing data to file.");
      }

   }

   /* Close the output file */
   fclose( OutFilePtr );

   /* Output summary of data retrieved */
   for ( j=0; j < iStdGlobVar.NumDataSearch; j++ )
   {
      eLogNotice( 0, "Retrieved %6d entries for %s, %s",
                  (StdDataPtr+j)->NumOfPoints,
                  (StdDataPtr+j)->SourceName,
                  (StdDataPtr+j)->DatumName );
   }
   eLogNotice( 0, "Retrieved %6d data entries in total", iStdLinesOfData );

   /* Close the file and terminate the program */
   exit( EXIT_SUCCESS );

}

/*****************************************************************************
** Function Name:
**    iStdStoreData
**
** Type:
**    Status_t
**
** Purpose:
**    Store a data value that matches the search criterior.
**
** Description:
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**
**
** Arguments:
**    eTtlTime TimeStamp        (in)
**       Time stamp for data that matches search.
**    Int32_t  DataItem         (in)
**       Index into array of requested data items.
**    Int32_t  Value            (in)
**       Data value.
**
** Authors:
**    Mark Bowman (mkb)
**
*****************************************************************************/
Status_t iStdStoreData(eTtlTime_t TimeStamp, Int32_t DataItem, Int32_t Value)
{

   Status_t Status;
   iStdMatchedData_t *DataPtr;

   /* Check to see if this is the first item found. */
   if (iStdLastMatchedPtr == NULL)
   {
      Status = iStdAddNew( TimeStamp, DataItem, Value, NULL);
   }
   else
   {
     /* Sort through matched data to find an entry with the same time stamp */
     /* or the place to make new entry.                                     */

     Status = iStdFindTimeMatch( TimeStamp, &DataPtr);

     /* Add value to existing time entry. */
     if (Status == E_STD_MATCHFOUND)
     {
       Status = iStdAddMatch( TimeStamp, DataItem, Value, DataPtr);
     }
     /* Add new entry. */
     else if (Status == E_STD_NOMATCH)
     {
       Status = iStdAddNew( TimeStamp, DataItem, Value, DataPtr);
     }
     /* Log error. */
     else
     {
       eLogErr(Status,"Failed to add new item to list of matched data.");
     }
   }

   return (Status);
}

/*****************************************************************************
** Function Name:
**    iStdAddMatch
**
** Type:
**    Status_t
**
** Purpose:
**    Store a data value in a linked list.
**
** Description:
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**
**
** Arguments:
**    eTtlTime TimeStamp        (in)
**       Time stamp for data that matches search.
**    Int32_t  DataItem         (in)
**       Index into array of requested data items.
**    Int32-t  Value            (in)
**       Data value.
**    iStdMatchedData_t *PreviousDataPtr (in)
**       Pointer to previsou item in matched data list.
**
** Authors:
**    Mark Bowman (mkb)
**
*****************************************************************************/
Status_t iStdAddMatch(eTtlTime_t TimeStamp, Int32_t DataItem, Int32_t Value,
                      iStdMatchedData_t *DataPtr)
{
   char     TimeStr[ E_STD_MAX_STRING_LEN ];

  if (!(DataPtr->SetFlag[DataItem]))
  {
     DataPtr->Value[DataItem]   = Value;
     DataPtr->SetFlag[DataItem] = TRUE;
  }
  else
  {
     eTimToString( &TimeStamp, E_STD_MAX_STRING_LEN, TimeStr );
     eLogInfo("Duplicate data/timestamp pair (%s)", TimeStr );
  }

  return SYS_NOMINAL;
}

/*****************************************************************************
** Function Name:
**    iStdAddNew
**
** Type:
**    Status_t
**
** Purpose:
**    Store a data value in a linked list.
**
** Description:
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**
**
** Arguments:
**    eTtlTime TimeStamp        (in)
**       Time stamp for data that matches search.
**    Int32_t  DataItem         (in)
**       Index into array of requested data items.
**    Int32-t  Value            (in)
**       Data value.
**    iStdMatchedData_t *PreviousDataPtr (in)
**       Pointer to previsou item in matched data list.
**
** Authors:
**    Mark Bowman (mkb)
**
*****************************************************************************/
Status_t iStdAddNew(eTtlTime_t TimeStamp, Int32_t DataItem, Int32_t Value,
                      iStdMatchedData_t *PreviousDataPtr)
{

  iStdMatchedData_t *DataPtr;

  /* Allocate some memory for Sdb data */
  DataPtr = (iStdMatchedData_t *) TTL_MALLOC( sizeof(iStdMatchedData_t) );
  if( DataPtr == NULL )
  {
    return E_STD_MEM_ALLOC_ERR;
  }

  /* Initialise memory. */
  memset(DataPtr, 0, sizeof(iStdMatchedData_t));

  DataPtr->TimeStamp         = TimeStamp;
  DataPtr->Value[DataItem]   = Value;
  DataPtr->SetFlag[DataItem] = TRUE;
  DataPtr->PreviousPtr       = PreviousDataPtr;

  if (PreviousDataPtr  != NULL)
  {
    eLogDebug("PreviousDataPtr != NULL");
    if (PreviousDataPtr->NextPtr != NULL)
    {
       eLogDebug("PreviousDataPtr->NextPtr != NULL");
       DataPtr->NextPtr         = PreviousDataPtr->NextPtr;
       /* Fix entries either side of new entry. */
       (PreviousDataPtr->NextPtr)->PreviousPtr = DataPtr;
    }
    else
    {
       DataPtr->NextPtr      = NULL;
       iStdLastMatchedPtr    = DataPtr;
    }

    PreviousDataPtr->NextPtr = DataPtr;

  }
  else
  {
    DataPtr->NextPtr        = NULL;
    iStdLastMatchedPtr      = DataPtr;
  }

  eLogDebug("Ptr = %d. Time = %d. Prev = %d. Next = %d. Last = %d",
             DataPtr, DataPtr->TimeStamp.t_sec, DataPtr->PreviousPtr,
             DataPtr->NextPtr,iStdLastMatchedPtr);

  return SYS_NOMINAL;
}

/*****************************************************************************
** Function Name:
**    iStdFindTimeMatch
**
** Type:
**    Status_t
**
** Purpose:
**    Find position of new data item in time ordered list.
**
** Description:
**    Start from the end of the linked list because data in Sdb will
**    generally be in chronological order.
**
** Return type:
**    Status_t
**       Returns SYS_NOMINAL on success.
**
**
** Arguments:
**    eTtlTime TimeStamp        (in)
**       Time stamp for data that matches search.
**    iStdMatchedData_t *PreviousDataPtr (out)
**       Pointer to previsou item in matched data list.
**
** Authors:
**    Mark Bowman (mkb)
**
*****************************************************************************/
Status_t iStdFindTimeMatch(eTtlTime_t TimeStamp, iStdMatchedData_t **PreviousMatch)
{
  Status_t Status;
  Bool_t Continue=TRUE;
  iStdMatchedData_t  *IndexPtr;

  /* Start from end. */
  IndexPtr = iStdLastMatchedPtr;

  while (Continue)
  {
    if (IndexPtr != NULL)
    {
      if ( TimeStamp.t_sec  < IndexPtr->TimeStamp.t_sec )
      {
        IndexPtr = IndexPtr->PreviousPtr;
      }
      else if ((TimeStamp.t_sec  == IndexPtr->TimeStamp.t_sec ) &&
               (TimeStamp.t_nsec < IndexPtr->TimeStamp.t_nsec) )
      {
        IndexPtr = IndexPtr->PreviousPtr;
      }
      else if ((TimeStamp.t_sec  == IndexPtr->TimeStamp.t_sec ) &&
               (TimeStamp.t_nsec == IndexPtr->TimeStamp.t_nsec) )
      {
        Status   = E_STD_MATCHFOUND;
        Continue = FALSE;
      }
      else
      {
        Status   = E_STD_NOMATCH;
        Continue = FALSE;
      }
    }
    else
    {
      Status   = E_STD_NOMATCH;
      Continue = FALSE;
    }
  }

  *PreviousMatch = IndexPtr;

  return Status;

}
