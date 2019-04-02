/*
** Module Name:
**    teststore.c
**
** Purpose:
**    Test program for disk storage for the SDB.
**
** Description:
**    This program is used to test the storage format and storage
**    data rates for disk-based sotrage from the SDB.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: teststore.c,v 0.2 2004/01/16 08:55:04 sxf Exp $
**
** History:
**    $Log: teststore.c,v $
**    Revision 0.2  2004/01/16 08:55:04  sxf
**    Minor changes to port to QNX6.
**
**    Revision 0.1  2000/10/09 10:42:35  djm
**    Initial creation.
**
**
**
**
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TtlSystem.h"
#include "Sdb.h"
#include "Tim.h"


/* Definitions */

#define M_SDB_MAX_SOURCES 16  /* The maximum number of "test" sources */
#define M_SDB_MAX_DATA    16  /* The maximum number of "test" data per source */
#define M_SDB_DATA_PATH "./data/"      /* Path to the data files */


/* Structure definitions */
struct mSdbStore_s {
   /*
   ** Datum provided information
   */
   Int32_t     SourceId;     /* ID of source of datum */
   Int32_t     DatumId;      /* ID number of the datum itself */
   Int32_t     Units;        /* Data Units */
   eTtlTime_t  TimeStamp;    /* Time associated with the value */
   Int32_t     Value;        /* */

   /*
   ** Derived data, actually written to file
   */
   Uint16_t    Code;         /* ID code for storing data */
   Uint32_t    TimeOffset;   /* Time since start of hour in 10ns quanta */


   /*
   ** Other miscellaneous stuff
   */
   Bool_t      Static;       /* TRUE if Value doesn't change */
};

typedef struct mSdbStore_s mSdbStore_t; 


/* Function prototypes */

Status_t mSdbCreateDatum(mSdbStore_t *StorePtr);
Status_t mSdbStoreDatum(mSdbStore_t *StorePtr);


int main(
   int argc,
   char *argv[]
)
{
/*
** Function Name:
**    main
**
** Type:
**    void
**
** Purpose:
**    Top level function of the "teststore" program.
**
** Description:
**    Top level function of the "SDB store tester". It is called on
**    startup of the executable.
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
**
** History:
**    05-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Function return status variable */
   mSdbStore_t Store;        /* Structure holding datum information */
   int i;

   /* Main loop */
   for(i = 0; i < 10; i++)
   {
      /* Create a "test" datum */
      Status = mSdbCreateDatum(&Store);

      /* Store it to disk */
      Status = mSdbStoreDatum(&Store);

   }  /* End of main loop */

}  /* End of main() */






Status_t mSdbCreateDatum
(
   mSdbStore_t *StorePtr
)
{
/*
** Function Name:
**    mSdbCreateDatum
**
** Type:
**    int
**
** Purpose:
**    Fill in a "store" with randomly generated data.
**
** Description:
**    ...
**
** Arguments:
**    mSdbStore_t *StorePtr (in)
**       Structure into which the data is put. Not all fields 
**       will necessarily be changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Jul-2000 djm Initial creation.
**
*/

   /* No local variables */
   Status_t Status;          /* Function return value */
   char TimStr[E_TIM_BUFFER_LENGTH];   /* Buffer for holding ascii time */
   char HourStr[E_TIM_BUFFER_LENGTH];  /* Buffer for holding ascii time */
   eTtlTime_t Hour, Diff;

   StorePtr->SourceId = rand() % M_SDB_MAX_SOURCES;
   StorePtr->DatumId = rand() % M_SDB_MAX_DATA;
   StorePtr->Units = E_SDB_NO_UNITS;
   Status = eTimGetTime(&(StorePtr->TimeStamp));

   if(StorePtr->SourceId > (M_SDB_MAX_SOURCES/2))
   {
      StorePtr->Static = FALSE;
   }
   else
   {
      StorePtr->Static = TRUE;
   }


   if(StorePtr->Static == FALSE)
   {
      StorePtr->Value = rand();
   }
   else
   {
      StorePtr->Value = StorePtr->SourceId;
   }

   /* Put in place a string that contains the actual time */
   Status = eTimToString(&(StorePtr->TimeStamp), E_TIM_BUFFER_LENGTH, TimStr);

   /* Calculate the "hour" for the filename, etc. */
   Hour.t_sec = StorePtr->TimeStamp.t_sec;
   Hour.t_nsec = StorePtr->TimeStamp.t_nsec;
   Hour.t_sec -= atoi(&TimStr[12]) * 60;
   Hour.t_sec -= atoi(&TimStr[15]);
   Hour.t_nsec = 0;
   Status = eTimToString(&(Hour), E_TIM_BUFFER_LENGTH, HourStr);

   /* Calculate the "delta-time" to be written into the file */
   eTimDifference(&(Hour), &(StorePtr->TimeStamp), &Diff);
   printf("delta = %10d  ", (Diff.t_sec * 1000000) + (Diff.t_nsec / 1000));

   printf(
      "(0x%x,0x%x) = 0x%4.4x @ %s %s\n",
      StorePtr->SourceId, StorePtr->DatumId, StorePtr->Value, HourStr, TimStr
   );

   return SYS_NOMINAL;
}  /* End of mSdbCreateDatum() */





Status_t mSdbStoreDatum
(
   mSdbStore_t *StorePtr
)
{
/*
** Function Name:
**    mSdbStoreDatum
**
** Type:
**    Status_t
**
** Purpose:
**    Write a "store" to disk.
**
** Description:
**    ...
**
** Arguments:
**    mSdbStore_t *StorePtr (in)
**       Structure containing the data to write to disk. Not all
**       it is written and the format is changed.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    05-Jul-2000 djm Initial creation.
**
*/

   /* No local variables */

   /* If "code" is M_SDB_NO_CODE (i.e. zero) then determine the datum code */

   /* Determine filename to write to (and thus the hour) */

   /* Determine the modified time (10ns since hour) */

   /* Write to file code + mod.time + value */

   /* Terminate the function */
   return SYS_NOMINAL;

}  /* End of mSdbStoreDatum() */



/* EOF */
