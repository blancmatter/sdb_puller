/*
** Module Name:
**    SdbStore.c
**
** Purpose:
**    A module contains functions for storing SDB data to file.
**
** Description:
**    This module contains code for writing Status Database (SDB) data
**    to disk.
**
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbStore.c,v 0.26 2007/07/17 10:20:02 mjf Exp $
**
** History:
**    $Log: SdbStore.c,v $
**    Revision 0.26  2007/07/17 10:20:02  mjf
**    Added functionality to pass raw UDP packets from Sdb
**    to a MySql database.
**
**    Revision 0.25  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.24  2005/06/20 13:35:31  mjf
**    Minor changes to aid porting to Linux - no functional changes.
**
**    Revision 0.23  2003/06/05 12:48:46  dws
**    Removed Cmt debug. Added comment to explain how to fix problem if
**    coding fails consistently.
**
**    Revision 0.22  2003/06/04 14:19:26  dws
**    Added debug to help investiage the missing data problem.
**
**    Revision 0.21  2002/08/08 19:57:58  mjf
**    Correction of parameter to debug message.
**
**    Revision 0.20  2002/07/27 11:02:55  mjf
**    Addition of two command-line parameters, to specify the path of the
**    datafile storage, and to specify the number of days after which to
**    clean up SDB files.
**
**    Revision 0.19  2002/01/10 17:43:47  mjf
**    Upon closing an SDB file, ensure all submissions have been archived to
**    disk, if the value is the same as the value already archived.
**
**    Revision 0.18  2001/09/14 11:08:01  djm
**    Improved diagnostics.
**
**    Revision 0.17  2001/04/24 11:20:59  mjf
**    For a created file set 'user', 'group' and 'other' attributes to be
**    read and write.
**
**    Revision 0.16  2001/03/27 18:53:04  djm
**    Removed fflush() function calls in order to increase the performance
**    of the SDB.
**
**    Revision 0.15  2000/11/21 16:43:37  djm
**    Added iSdbStorePrevData() function.
**
**    Revision 0.14  2000/10/24 13:43:43  djm
**    Added additional diagnostic, so the source/datum that causes a
**    new file to be open is displayed. This allows easier tracing of
**    data that has incorrect timestamps.
**
**    Revision 0.13  2000/09/28 13:42:58  djm
**    Fixed bug where zero-time-stamped data would cause an incorrect
**    time comparison when trying to determine if a file was already
**    open. This would trick the SDB into thinking the zero time file
**    was already open when it actually wasn't. This has been fixed by
**    adding additional check based on the file pointer.
**
**    Revision 0.12  2000/09/27 11:53:36  djm
**    Changed function name to make storage of data distinct
**    from the storage of units.
**
**    Revision 0.11  2000/09/01 09:34:54  djm
**    Fixed bug in file close/opening (incorrect file being closed).
**
**    Revision 0.10  2000/08/31 15:37:09  djm
**    Remove extraneous carriage-returns in file reporting.
**
**    Revision 0.9  2000/08/29 14:35:51  djm
**    Increased debug message detail.
**
**    Revision 0.8  2000/08/29 10:24:40  djm
**    Fixed bug in detecting whether to write data to a new file.
**
**    Revision 0.7  2000/08/24 11:51:47  djm
**    Converted to make use of the globalised functions in SdbCode.c.
**
**    Revision 0.6  2000/08/22 09:18:06  djm
**    Fix to cope with changes to global types.
**
**    Revision 0.5  2000/08/18 14:41:15  djm
**    Changed symbolic constant for storage file path.
**
**    Revision 0.4  2000/07/21 14:41:48  djm
**    Fixed problem with new files not being written correctly.
**
**    Revision 0.3  2000/07/14 16:33:01  djm
**    Removed storage file ID code generation to a separate function.
**    Fixed problem with storage files not being closed after use.
**
**    Revision 0.2  2000/07/11 17:37:18  djm
**    Basic storage files are now being written.
**
**    Revision 0.1  2000/07/07 18:02:32  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>     /* TCP UDP headers */
#include <netinet/in.h>     /* TCP UDP headers */
#include <arpa/inet.h>      /* TCP UDP headers */
#include <termios.h>
#include <netdb.h>

#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Log.h"
#include "Cil.h"
#include "Tim.h"
#include "Sdb.h"
#include "SdbPrivate.h"

/*
#define MYSQL_HOST     "mizar"
#define MYSQL_PORT     "13320"
*/
#define MYSQL_MYPORT   "13025"
#define GROUP_SIZE     1


/* Function prototypes */

int mSdbRawSend( void *DataPtr,   size_t DataLen);
Status_t mSdbReadyDbFile(eTtlTime_t *FileStartTimePtr, Int32_t *DbFileIndexPtr,
                         iSdbDefn_t *DefnPtr);
Status_t mSdbGetHour(eTtlTime_t *TimePtr, eTtlTime_t *HourPtr);
Status_t mSdbDurToUsec(eTtlTime_t *DurationPtr, Uint32_t *UsecPtr);
Status_t mSdbWriteHeader(int Index);


/* Functions */

Status_t iSdbStorePrevData(
   iSdbDefn_t *DefnPtr
)
{
/*
** Function Name:
**    iSdbStorePrevData
**
** Type:
**    Status_t
**
** Purpose:
**    Saves to disk the last datum recorded.
**
** Description:
**    This function will check if the pervious-to-last received
**    datum needs to be recorded to file. This is to ensure that
**    data is preserved for an event driven system and in order
**    to keep a neat display on data analysis (e.g. for the DAT
**    application).
**
**    The second-last "event" will be written to disk if:
**    a) the last event is different to it in value, and
**    b) the second-last event has not already bee written
**
**    NB: In the comments through this function, the term "event"
**        unless otherwise specified, will refer to the "latest"
**        event pointed to by the data definition (DefnPtr). I.e.:
**        DefnPtr->LastSubPtr
**
** Arguments:
**    iSdbDefn_t *DefnPtr    (in/out)
**       A pointer to an SDB structure definition.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    24-Aug-2000 djm Globalised the code generation functions.
**    06-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   eTtlTime_t FileStartTime; /* Timestamp for the hour for the file */
   eTtlTime_t TimeDiff;      /* Diff' between FileStartTime and Datum time */
   Int32_t Index;            /* Index into the iSdbDbFileList array */
   eSdbRawFmt_t FileData;    /* Buffer for data to be written to file */
   size_t NumRecords;        /* Number of records (written to file) */
   eSdbSngReq_t Req;         /* Datum specification (for code generation) */
   eSdbDatum_t MySqlDatum;   /* Datum to send to MySql host */


   /* Only do this if we _have_ previous data... otherwise bail out now */
   if(DefnPtr->LastSubPtr->PrevPtr == NULL)
   {
      return SYS_NOMINAL;
   }

   /* If there is no change in value, then don't bother */
   if(DefnPtr->LastSubPtr->PrevPtr->Value == DefnPtr->LastSubPtr->Value)
   {
      return SYS_NOMINAL;
   }

   /*
   ** If the previous value has already been written,
   ** then there is no need to do it again either
   */
   if(DefnPtr->ValueRecorded == TRUE)
   {
      return SYS_NOMINAL;
   }


   /* Determine the timestamp of the file to be written to */
   Status = mSdbGetHour(
               &(DefnPtr->LastSubPtr->PrevPtr->TimeStamp), &FileStartTime);
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }


   /* Now determine the offset between the Datum to write and the file start */
   Status = eTimDifference(
      &FileStartTime, &(DefnPtr->LastSubPtr->PrevPtr->TimeStamp), &TimeDiff
   );
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }


/* DEBUG - THIS MAY BE A PROBLEM!!! */

   /* Recover a file pointer, based on the timestamp */
   Status = mSdbReadyDbFile(&FileStartTime, &Index, DefnPtr);
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }

   /*
   ** DEBUG - This next block may need to be revisited. At the moment,
   **         there is no use for any Defn.code item. It is done on the
   **         fly for the data storage structure.
   */

   /* Format the data for writing to file */
   Req.SourceId = DefnPtr->SourceId;
   Req.DatumId = DefnPtr->DatumId;
   Status = eSdbStoreIdEncode(&Req, &FileData.Code);
   if(Status != SYS_NOMINAL)
   {
      /* If this occurs regularly please se bug #49 in TDT */
      eLogErr(Status, "Error Encoding Datum for  %s ",
                                                eCilNameString(Req.SourceId) );
   }
   DefnPtr->SourceId = Req.SourceId;
   DefnPtr->DatumId = Req.DatumId;



   Status = mSdbDurToUsec(&TimeDiff, &FileData.TimeOffset);
   FileData.Value = DefnPtr->LastSubPtr->PrevPtr->Value;
   if(iSdbDbFileList[Index].FilePtr == NULL)
   {
      eLogWarning(Status, "NULL file pointer (file index = %d)", Index);
   }
   
 


   /* Actually write the data to the file */
   NumRecords = fwrite(
                   &FileData, sizeof(FileData), 1,
                   iSdbDbFileList[Index].FilePtr
                );
   if(NumRecords != 1)
   {
      Status = E_SDB_FWRITE_FAIL;
      eLogErr(Status, "Error writing data to file");
      return Status;
   }

   /* DEBUG - FORCE A FILE FLUSH (inefficient) FOR IMMEDIATE CHECKING */
   /*         REMOVED THIS FOR PRODUCTION CODE */

   /*
   fflush(iSdbDbFileList[Index].FilePtr);
   */

   /* Update the last-accessed time for that file */
   Status = eTimGetTime(&(iSdbDbFileList[Index].LastAccessed));
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }

   /*
   ** Make a note within the defn of the file to which we have written.
   ** This will be used on the next pass through this function to see if
   ** we need to write the datum value again.
   */
   DefnPtr->FileIndex = Index;

   if ( iSdbSendToSql == TRUE )
   {
      /* Send the value to the MySQL host as a raw udp packet */
      MySqlDatum.SourceId = DefnPtr->SourceId;
      MySqlDatum.DatumId  = DefnPtr->DatumId;
      MySqlDatum.Units    = DefnPtr->Units;
      MySqlDatum.Msrment.TimeStamp  = DefnPtr->LastSubPtr->PrevPtr->TimeStamp;
      MySqlDatum.Msrment.Value      = FileData.Value;
#if 1
      mSdbRawSend( (void *)&MySqlDatum,
                   sizeof( eSdbDatum_t )
                 );

#endif       
   }

   /* Terminate the function, indicating success */
   Status = SYS_NOMINAL;
   return Status;

}  /* End of iSdbStorePrevData() */




Status_t iSdbStoreData(
   Bool_t Force,
   iSdbDefn_t *DefnPtr
)
{
/*
** Function Name:
**    iSdbStoreData
**
** Type:
**    Status_t
**
** Purpose:
**    Organises the file-storage of provided SDB data.
**
** Description:
**    ...
**
**    NB: In the comments through this function, the term "event"
**        unless otherwise specified, will refer to the "latest"
**        event pointed to by the data definition (DefnPtr). I.e.:
**        DefnPtr->LastSubPtr
**
** Arguments:
**    Bool_t Force           (in)
**       A boolean indicating whether we want to force a write
**       to disk. If this is TRUE, then this fuction will attempt
**       to write the data to disk, regardless of the other
**       conditions. If it is FALSE, then this function will
**       assess whether there is a need to write the data to disk.
**       In general, this will be set to FALSE, and as a result, the
**       decision algorithm for whether or not to write the data 
**       will be contained within this function.
**    iSdbDefn_t *DefnPtr    (in/out)
**       A pointer to an SDB structure definition.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    24-Aug-2000 djm Globalised the code generation functions.
**    06-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   eTtlTime_t FileStartTime; /* Timestamp for the hour for the file */
   eTtlTime_t TimeDiff;      /* Diff' between FileStartTime and Datum time */
   Int32_t Index;            /* Index into the iSdbDbFileList array */
   eSdbRawFmt_t FileData;    /* Buffer for data to be written to file */
   size_t NumRecords;        /* Number of records (written to file) */
   eSdbSngReq_t Req;         /* Datum specification (for code generation) */
   eSdbDatum_t MySqlDatum;   /* Datum to send to MySql host */


   /* Reset the "have-been-written" flag */
   DefnPtr->ValueRecorded = FALSE;

   /* Determine the timestamp of the file to be written to */
   Status = mSdbGetHour(&(DefnPtr->LastSubPtr->TimeStamp), &FileStartTime);
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }


   /* Now determine the offset between the Datum to write and the file start */
   Status = eTimDifference(
      &FileStartTime, &(DefnPtr->LastSubPtr->TimeStamp), &TimeDiff
   );
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }


   /* Recover a file pointer, based on the timestamp */
   Status = mSdbReadyDbFile(&FileStartTime, &Index, DefnPtr);
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }
/*
Status = eTimToString(&(DefnPtr->LastSubPtr->TimeStamp), E_TIM_BUFFER_LENGTH, Str);
printf("DataTime: %s,  ", Str);
Status = eTimToString(&FileStartTime, E_TIM_BUFFER_LENGTH, Str);
printf("StartTime: %s\n", Str);
*/


   /*
   ** Decide whether the data needs to be written (if not, then bail-out)
   ** Note that this is AFTER file creation. This should be okay, as we
   ** want to write the data to file if we start a new file.
   */

   if(Force == FALSE)
   {
      if(DefnPtr->FileIndex == Index)
      {
         if(DefnPtr->LastSubPtr->PrevPtr != NULL)
         {
            if(DefnPtr->LastSubPtr->Value == DefnPtr->LastSubPtr->PrevPtr->Value)
            {
               return SYS_NOMINAL;
            }
         }
      }
   }
   else
   {
      /* Output a log message for a forced write */
      eLogInfo( "Forced archive source/datum pair (0x%x '%s', 0x%x) %8.8x = %d",
                DefnPtr->SourceId, eCilNameString( DefnPtr->SourceId ),
                DefnPtr->DatumId, 
                DefnPtr->LastSubPtr->Value, DefnPtr->LastSubPtr->Value );
   }


   /*
   ** DEBUG - This next block may need to be revisited. At the moment,
   **         there is no use for any Defn.code item. It is done on the
   **         fly for the data storage structure.
   */

   /* Format the data for writing to file */
   Req.SourceId = DefnPtr->SourceId;
   Req.DatumId = DefnPtr->DatumId;
   Status = eSdbStoreIdEncode(&Req, &FileData.Code);
   if(Status != SYS_NOMINAL)
   {
      /* If the 'coding' fails regulary please see bug #49 in TDT. */
      eLogErr(Status, "Error Encoding Datum for  %s ",
                                                eCilNameString(Req.SourceId) );
      return Status; 
   }
   DefnPtr->SourceId = Req.SourceId;
   DefnPtr->DatumId = Req.DatumId;

   Status = mSdbDurToUsec(&TimeDiff, &FileData.TimeOffset);
   FileData.Value = DefnPtr->LastSubPtr->Value;
   if(iSdbDbFileList[Index].FilePtr == NULL)
   {
      eLogWarning(Status, "NULL file pointer (file index = %d)", Index);
   }

/*
printf("(Returned) FilePtr = %ld\n", (long)iSdbDbFileList[Index].FilePtr);
printf("Code = 0x%4.4hx,  Time = 0x%x = %d,  Value = 0x%x = %d\n",
   FileData.Code, FileData.TimeOffset, FileData.TimeOffset,
   FileData.Value, FileData.Value
);
*/

   /* Actually write the data to the file */
   NumRecords = fwrite(
                   &FileData, sizeof(FileData), 1,
                   iSdbDbFileList[Index].FilePtr
                );
   if(NumRecords != 1)
   {
      Status = E_SDB_FWRITE_FAIL;
      eLogErr(Status, "Error writing data to file");
      return Status;
   }

   /* DEBUG - FORCE A FILE FLUSH (inefficient) FOR IMMEDIATE CHECKING */
   /*         REMOVED THIS FOR PRODUCTION CODE */

   /*
   fflush(iSdbDbFileList[Index].FilePtr);
   */

   /* Update the last-accessed time for that file */
   Status = eTimGetTime(&(iSdbDbFileList[Index].LastAccessed));
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }

   /*
   ** Make a note within the defn of the file to which we have written.
   ** This will be used on the next pass through this function to see if
   ** we need to write the datum value again.
   */
   DefnPtr->FileIndex = Index;

   /* Set the written-flag, then terminate the function, indicating success */
   DefnPtr->ValueRecorded = TRUE;
   Status = SYS_NOMINAL;

   if ( iSdbSendToSql == TRUE )
   {
      /* Send the value to the MySQL host as a raw udp packet */
      MySqlDatum.SourceId = DefnPtr->SourceId;
      MySqlDatum.DatumId  = DefnPtr->DatumId;
      MySqlDatum.Units    = DefnPtr->Units;
      MySqlDatum.Msrment.TimeStamp  = DefnPtr->LastSubPtr->TimeStamp;
      MySqlDatum.Msrment.Value      = FileData.Value;

      mSdbRawSend( (void *)&MySqlDatum,
                   sizeof( eSdbDatum_t )
                 );
   }

   return Status;

}  /* End of iSdbStoreData() */




Status_t mSdbReadyDbFile(
   eTtlTime_t *FileStartTimePtr,
   Int32_t *DbFileIndexPtr,
   iSdbDefn_t *DefnPtr
)
{
/*
** Function Name:
**    mSdbReadyDbFile
**
** Type:
**    Status_t
**
** Purpose:
**    Ready a file for writing data.
**
** Description:
**    This function makes use of the global array, iSdbDbFileList[].
**    The function will put the index of the readied file in the
**    memory pointed to by DbFileIndexPtr.
**
** Arguments:
**    ...
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    28-Sep-2000 djm Fixed bug with zero-time stamped data
**    01-Sep-2000 djm Try to fix file close/open bug.
**    11-Jul-2000 djm Added call for header block writing.
**    06-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   Int32_t Index;            /* Local DbFile index */
   Int32_t OldestIndex;      /* Index of oldest open file in list */
   eTtlTime_t OldestAccessTime;        /* Oldest file in the set */
   int Comp;                 /* Comparison return between two times */
   char TimStr[E_TIM_BUFFER_LENGTH];   /* Buffer for holding ascii time */
   char FileName[I_SDB_MAX_FILENAME];  /* Character array with filename */
   long int FilePos;         /* Position of the file pointer within the file */
   int HashIndex;            /* Index into the hash-table array */
   iSdbDefn_t *HashDefnPtr;  /* Data element definiton from hash-table */
                             /* Start time of file relevant to measurement */
   eTtlTime_t RelevantFileStartTime;


   /* Check to see if the appropriate file is already open */
   for(Index = 0; Index < I_SDB_MAX_DB_FILES; Index++)
   {
      Comp = eTimCompare(&(iSdbDbFileList[Index].StartTime), FileStartTimePtr);
      if((Comp == 0) && (iSdbDbFileList[Index].FilePtr != NULL))
      {
         *DbFileIndexPtr = Index;
         return SYS_NOMINAL;
      }
   }


   /* If the relevant file is not open (true, if we get this far) */

   /* Make a note that File-0 is the one with the oldest access time */
   /* We'll correct this in the following loop, should that not be the case */
   OldestIndex = 0;
   OldestAccessTime = iSdbDbFileList[OldestIndex].LastAccessed;


   /* Loop through our list of files and find a NULL pointer */
   for(Index = 0; Index < I_SDB_MAX_DB_FILES; Index++)
   {
      /* If we find a NULL entry, then use it */
      if(iSdbDbFileList[Index].FilePtr == NULL)
      {
         *DbFileIndexPtr = Index;
         break;
      }

      /* While we are doing this, keep a note of the oldest file access */
      Comp = eTimCompare(
                &OldestAccessTime, &(iSdbDbFileList[Index].LastAccessed));
      if(Comp > 0)
      {
         OldestIndex = Index;
         OldestAccessTime = iSdbDbFileList[OldestIndex].LastAccessed;
      }
      eLogDebug(
         "File pointer %d = 0x%x, Comp = %d Accessed = %d",
         Index, (long)iSdbDbFileList[Index].FilePtr, Comp,
         iSdbDbFileList[Index].LastAccessed
      );
   }

   /* If no "NULL pointers" were found in the file */
   if(Index >= I_SDB_MAX_DB_FILES)
   {
      /* Note the file that is being closed */
      eLogNotice(0, "Closing file with index = %d", OldestIndex);

      /* Need to ensure all measurements from this period have been archived */
      for( HashIndex = 0; HashIndex < I_SDB_HASHSIZE; HashIndex++ )
      {
         /* Loop over the hash-entries for the current hash-table index */
         for( HashDefnPtr = iSdbHashTable[ HashIndex ];
              HashDefnPtr != NULL;
              HashDefnPtr = HashDefnPtr->NextHashPtr )
         {
            /* Ignore dummy entries */
            if( HashDefnPtr->NewestPtr == NULL )
               continue;

            /* If the last measurement has not been written to disk */
            if ( HashDefnPtr->ValueRecorded == FALSE )
            {
               /* Determine timestamp of the relevant file for measurement */
               Status = mSdbGetHour( &( HashDefnPtr->LastSubPtr->TimeStamp ),
                                     &RelevantFileStartTime );
               if( Status != SYS_NOMINAL )
               {
                  return Status;
               }

               /* If last measurement was in the period for file being closed */
               if ( eTimCompare
                       ( &( iSdbDbFileList[ OldestIndex ].StartTime ), 
                         &RelevantFileStartTime ) == E_TIM_TIMEA_EQ_TIMEB )
               {
                  /* Store the measurement, forcing a write to disk */
                  iSdbStoreData( TRUE, HashDefnPtr );
               }
            }
         }
      }

      /* Close the file with the oldest access time */
      fclose(iSdbDbFileList[OldestIndex].FilePtr);
      iSdbDbFileList[OldestIndex].FilePtr = NULL;

      /* Note the index number of the file that was closed */
      Index = OldestIndex;
      *DbFileIndexPtr = Index;
   }

   /* Determine the filename */
   Status = eTimToString(FileStartTimePtr, E_TIM_BUFFER_LENGTH, TimStr);
   sprintf(FileName,
      "%s%.2s%.2s%.2s%.2s.sdb",
      iSdbDatafilePath,
      &TimStr[6],
      &TimStr[3],
      &TimStr[0],
      &TimStr[9]
   );

   eLogNotice(
      0, "Opening file \"%s\" (index = %d, for %s,0x%x)",
      FileName, Index, eCilNameString(DefnPtr->SourceId), DefnPtr->DatumId
   );

   /* Open the file (note that we are appending to it) */
   iSdbDbFileList[Index].FilePtr = fopen(FileName, "ab");
   if(iSdbDbFileList[Index].FilePtr == NULL)
   {
      eLogErr(E_SDB_FOPEN_FAIL, "Unable to open file \"%s\"", FileName);
      return E_SDB_FOPEN_FAIL;
   }

   /* Ensure that the file has 'user', 'group' and 'world' write access */
   if ( chmod( FileName, I_SDB_FILE_MODE ) != 0 )
   {
      eLogErr( E_SDB_FOPEN_FAIL, "Unable to set file mode for \"%s\", errno %d", 
               FileName, errno );
   }

   /* Put the file "start-time" into the DbFileList array */
   iSdbDbFileList[Index].StartTime.t_sec = FileStartTimePtr->t_sec;
   iSdbDbFileList[Index].StartTime.t_nsec = FileStartTimePtr->t_nsec;


   /* Check the file position. If we are at 0 (new file) then write header */
   FilePos = ftell(iSdbDbFileList[Index].FilePtr);
   if(FilePos == 0)
   {
      Status = mSdbWriteHeader(Index);
      if(Status != SYS_NOMINAL)
      {
         return Status;
      }
   }
      
   /* Update the time that the file was last accessed */
   Status = eTimGetTime(&(iSdbDbFileList[Index].LastAccessed));
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }


   /* Terminate the function, returning success */
   Status = SYS_NOMINAL;
   return Status;

}  /* End of mSdbReadyDbFile() */



Status_t mSdbGetHour(
   eTtlTime_t *TimePtr,
   eTtlTime_t *HourPtr
)
{
/*
** Function Name:
**    mSdbGetHour
**
** Type:
**    Status_t
**
** Purpose:
**    Gets the timestamp of the start of the hour of a specified
**    timestamp
**
** Description:
**    This function takes a specified time stamp and sets the minutes,
**    seconds and fractions of a second (nanoseconds) to zero. This
**    results in an output timestamp for the start of the hour at which
**    the specified time stamp would start.
**
** Arguments:
**    eTtlTime_t *TimePtr    (in)
**       Pointer to a time stamp containing the time from which the
**       hour will be determined.
**    eTtlTime_t *HourPtr    (out)
**       A pointer to a time stamp in which the determined time stamp
**       of the hour shall be put.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    06-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   char TimStr[E_TIM_BUFFER_LENGTH];   /* Buffer for holding ascii time */

   /* Put in place a string that contains the actual time */
   Status = eTimToString(TimePtr, E_TIM_BUFFER_LENGTH, TimStr);
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }

   /* Calculate the "hour" */
   HourPtr->t_sec = TimePtr->t_sec;
   HourPtr->t_sec -= atoi(&TimStr[12]) * 60;
   HourPtr->t_sec -= atoi(&TimStr[15]);
   HourPtr->t_nsec = 0;

   /* Terminate the function, returning success */
   Status = SYS_NOMINAL;
   return Status;

}  /* End of mSdbGetHour() */



Status_t mSdbDurToUsec(
   eTtlTime_t *DurationPtr,
   Uint32_t *UsecPtr
)
{
/*
** Function Name:
**    mSdbDurToUsec
**
** Type:
**    Status_t
**
** Purpose:
**    Converts a duration from TTL timestamp format to microseconds.
**
** Description:
**    This function takes a specified duration, expressed using the 
**    eTtlTime_t type and converts it into an unsigned integer where
**    the precision is 1 microsecond. This is used to find, for example,
**    the number of microseconds since the start of the hour. In real
**    terms, the 32-bit unsigned integer format can cope with durations 
**    up to about 71 minutes.
**
** Arguments:
**    eTtlTime_t *DurationPtr          (in)
**       Pointer to a time stamp containing the duration which is to
**       be converted.
**    Uint32_t *UsecPtr                (out)
**       A pointer to a unsigned integer which contains the duration
**       expressed in microseconds.
time stamp in which the determined time stamp
**       of the hour shall be put.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    11-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */


   /* Do the conversion */
   *UsecPtr = DurationPtr->t_sec * E_TTL_MICROSECS_PER_SEC;
   *UsecPtr += DurationPtr->t_nsec / 
               (E_TTL_NANOSECS_PER_SEC / E_TTL_MICROSECS_PER_SEC);


   /* DEBUG - DO CHECKING HERE */


   /* Terminate the function, returning success */
   Status = SYS_NOMINAL;
   return Status;

}  /* End of mSdbDurToUsec() */



Status_t mSdbWriteHeader(
   int Index
)
{
/*
** Function Name:
**    mSdbWriteHeader
**
** Type:
**    Status_t
**
** Purpose:
**    Writes a header block at the start of an SDB storage file.
**
** Description:
**    This function takes a file pointer to an SDB storage file and
**    writes a header block. The header block is based on the start
**    time of the file data and some additional characters to allow
**    recognition of the file. The start time is provided as a TTL
**    time structure, but the 
**    This information is all gleaned from
**    the global array containing this file data, the index into
**    which is passed as an argument to this function.
**    The file header format is as follows:
**
**       |                               |
**       |<------- 8 bytes total ------->|
**       |                               |
**       +---+---+---+---+---+---+---+---+
**       | S | D | B | R |  Timestamp.s  |
**       +---+---+---+---+---+---+---+---+
**       |                               |
**
**
** Arguments:
**    int Index              (in)
**       Array index for the global array iSdbDbFileList.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    11-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Function return value */
   size_t NumRecords;        /* Number of records (written to file) */
   char *HeaderStrPtr =      /* ASCII string to put at start of file */
      E_SDB_HEADER_STRING;   /*    Initial value for HeaderStrPtr */



   /* Write a header string */
   NumRecords = fwrite(
      HeaderStrPtr, strlen(HeaderStrPtr), 1, iSdbDbFileList[Index].FilePtr
   );
   if(NumRecords != 1)
   {
      Status = E_SDB_HDR_MN_WRITE_ERR;
      eLogErr(Status, "Error writing header key to file");
      return Status;
   }

   /* Write the timestamp */
   NumRecords = fwrite(
      &iSdbDbFileList[Index].StartTime.t_sec, 
      sizeof(iSdbDbFileList[Index].StartTime.t_sec),
      1, iSdbDbFileList[Index].FilePtr
   );
   if(NumRecords != 1)
   {
      Status = E_SDB_HDR_TS_WRITE_ERR;
      eLogErr(Status, "Error writing header time to file");
      return Status;
   }

   /* If we get this far, then it all worked. Return success */
   return SYS_NOMINAL;

}  /* End of mSdbWriteHeader() */


int mSdbRawSend
(
   void  *DataPtr,
   size_t DataLen
)
{
/*
** Function Name:
**    mSdbRawSend
**
** Type:
**    Status_t
**
** Purpose:
**    Sends a raw (pointer to void array) to an address specified by a
**    Host string and port.
**
**
*/

   /* Local variables */
   struct sockaddr_in To;     /* Socket structure to send to */
   size_t          ToLen = sizeof(To);
   int             TxCount;   /* Count of the number of bytes actually sent */

   struct hostent *HostPtr = NULL; /* Host entry - address details */
   static unsigned int IpAddress = 0; /* IP address */
   static int      Socket;    /* Socket */
   static int      GroupCount = 0;

   

   typedef struct GroupOfReadings_s
   {
       int Number;
       eSdbDatum_t Datum[ GROUP_SIZE ];
   } GroupOfReadings_t;
   

   GroupOfReadings_t Readings;
   /*
   ** Sanity check the input parameters.
   */
   if(DataPtr == NULL)
   {
      iSdbSqlLoggingOK = FALSE;
      return -1;
   }
  
   memcpy( (void *)&(Readings.Datum[ GroupCount ]), DataPtr, sizeof( eSdbDatum_t ) );

   GroupCount ++;

   Readings.Number = GroupCount;

   if ( GroupCount < GROUP_SIZE )
   {
       return 0;
   }

   GroupCount = 0;

   /* Formulate the socket address */
   To.sin_family = AF_INET;
   To.sin_port =   htons(atoi(iSdbMySqlPort));
   To.sin_addr.s_addr = htonl(INADDR_ANY);

   if ( Socket == (int)NULL )
   {
      /* Create an unbound socket */

      if ((Socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
         eLogErr( E_SDB_GEN_ERR,  "Unable to create socket, errno = %d\n", errno );
         iSdbSqlLoggingOK = FALSE;
         return -1;
      }

      if (bind(Socket, (void *) &To, sizeof(To)) < 0)
      {
         /* We have a problem, so close the socket... */
         close(Socket);
         /* ... and report the appropriate error */
         switch(errno)
         {
            case EADDRINUSE :
               eLogErr( E_SDB_GEN_ERR,  "The IP address is already in use\n" );
               iSdbSqlLoggingOK = FALSE;
               return -1;
               break;
            case EADDRNOTAVAIL :
               eLogErr( E_SDB_GEN_ERR, "The IP address is not available to the current host\n" );
               iSdbSqlLoggingOK = FALSE;
               return -1;
               break;
            default:
               eLogErr( E_SDB_GEN_ERR, "Failed to bind address\n");
               iSdbSqlLoggingOK = FALSE;
               return -1;
               break;
         }
      }
   }

   if ( IpAddress == 0 )
   {

      HostPtr = gethostbyname( iSdbMySqlHost );

      /* If the host name matched in the host table */
      if ( HostPtr != NULL )
      {
         /* IpAddress = ( ( unsigned int *) HostPtr->h_addr_list[0] ); */
         memcpy( &IpAddress, HostPtr->h_addr_list[0], 4 );
      }
      /* Else assume entry is an IP address */
      else
      {
         IpAddress = inet_addr(iSdbMySqlHost);
      }

      if (IpAddress == INADDR_NONE)
      {
         eLogErr( E_SDB_GEN_ERR, "Unable to resolve hostname \"%s\"\n", iSdbMySqlHost );
         iSdbSqlLoggingOK = FALSE;
         return -1;
      }
   }

   /* Formulate the socket address */
   To.sin_family = AF_INET;
   To.sin_port =   htons(atoi(iSdbMySqlPort));
   To.sin_addr.s_addr = IpAddress;

   /* Only attempt to lof SQL data if we have a valid IP address */
   if ( IpAddress != INADDR_NONE )
   {
      /* Finally we actually send the buffer of data */
      TxCount = sendto(Socket, &Readings, sizeof( int ) + Readings.Number * sizeof( eSdbDatum_t ), 0,
                       (void *) &To, ToLen);

      if (TxCount < 0)
      {
         eLogNotice( E_SDB_GEN_ERR,  "UDP send to SQL database failed, errno = %d\n", errno );
         iSdbSqlLoggingOK = FALSE;
         return -1;
      }
      else
      {
         iSdbSqlLoggingOK = TRUE;
      }
   }
   
   /* Terminate the function by returning the status of the "send" */
   return 0;

}  /* End of mSdbRawSend() */

/* EOF */

