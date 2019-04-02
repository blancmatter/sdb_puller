/*
** Module Name:
**    SdbUnits.c
**
** Purpose:
**    A module contains functions for storing/retrieving SDB units.
**
** Description:
**    This module contains code for writing Status Database (SDB)
**    units to disk and reading them back again.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbUnits.c,v 0.8 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbUnits.c,v $
**    Revision 0.8  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.7  2006/06/27 16:35:27  mjf
**    Tidy up of logging - no functional changes.
**
**    Revision 0.6  2005/06/20 13:35:31  mjf
**    Minor changes to aid porting to Linux - no functional changes.
**
**    Revision 0.5  2004/04/06 13:57:18  sxf
**    Additional call to fseek() for QNX6 to cope with different ftell()
**    implementation between QNX4 and QNX6.
**
**    Revision 0.4  2002/07/27 11:02:55  mjf
**    Addition of two command-line parameters, to specify the path of the
**    datafile storage, and to specify the number of days after which to
**    clean up SDB files.
**
**    Revision 0.3  2001/04/24 11:20:59  mjf
**    For a created file set 'user', 'group' and 'other' attributes to be
**    read and write.
**
**    Revision 0.2  2000/09/27 11:52:54  djm
**    Removed obsolete diagnostic.
**
**    Revision 0.1  2000/09/21 10:54:03  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Log.h"
#include "Cil.h"
#include "Tim.h"
#include "Sdb.h"
#include "SdbPrivate.h"



/* Functions */


Status_t iSdbStoreUnits(
   iSdbDefn_t *DefnPtr   
)
{
/*
** Function Name:
**    iSdbStoreUnits
**
** Type:
**    Status_t
**
** Purpose:
**    Records the units to a file.
**
** Description:
**    This function will take the SourceId, DatumId and Units from
**    the supplied Definition (*DefnPtr), and append a line with this
**    information to the end of the SDB units file (starting a new one
**    if required). If there are any problems in doing this, the function
**    will return an error. If the function is successful, then the
**    "UnitsRecorded" field of the definition structure will be set to
**    true. If the "UnitsRecorded" field was already set to TRUE, then
**    the function shall return immediately (with Status = SYS_NOMINAL).
**    It will also return SYS_NOMINAL if the units are successfully
**    written to the file.
**
** Arguments:
**    iSdbDefn_t *DefnPtr    (in/out)
**       A pointer to an SDB structure definition.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    20-Sep-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   FILE *FilePtr;            /* Pointer to the file to be used */
                             /* Name of file where unit listings are stored */
   char FileName[ I_SDB_MAX_FILENAME ];
   long int FilePos;         /* The position within the file */


   /* Construct the units file name */
   strcpy( FileName, iSdbDatafilePath );
   strcat( FileName, E_SDB_UNITS_FILENAME );

   /* If we've already recorded the units, just return success immediately */
   if(DefnPtr->UnitsRecorded == TRUE)
   {
      return SYS_NOMINAL;
   }

   /* Open the file (note that we are appending to it) */
   FilePtr = fopen(FileName, "ab");
   if(FilePtr == NULL)
   {
      Status = E_SDB_FOPEN_FAIL;
      eLogErr(
         Status, "Unable to open file \"%s\" for recording units", FileName
      );
      return Status;
   }

   /* Ensure that the file has 'user', 'group' and 'world' write access */
   if ( chmod( FileName, I_SDB_FILE_MODE ) != 0 )
   {
      eLogErr( E_SDB_FOPEN_FAIL, "Unable to set file mode for \"%s\", errno %d", 
               FileName, errno );
   }

   /* Check the file position. If we are at 0 (new file) then write header */

#ifndef E_WFL_OS_QNX4
   /*
   ** Move to end of file
   */
   fseek( FilePtr, 0, SEEK_END );
#endif

   FilePos = ftell(FilePtr);
   if(FilePos == 0)
   {
      eLogNotice(
         E_SDB_NO_UNITS_FILE,
         "Units file empty or non-existent. New file \"%s\" created",
         FileName
      );
      fprintf(FilePtr, "# File: SdbUnits.dat (contains unit listings)\n");
      fprintf(FilePtr, "# Format: SourceId DatumId Units\n");
   }


   /* Append the line containing the source ID, datum ID and units */
   fprintf(
      FilePtr,
      "%4.4x %4.4x %4.4x\n",
      DefnPtr->SourceId,
      DefnPtr->DatumId,
      DefnPtr->Units
   );


   /* Close the file */
   fclose(FilePtr);


   /* Note that the units are now recorded, and return success */
   DefnPtr->UnitsRecorded = TRUE;
   return SYS_NOMINAL;

}  /* End of iSdbStoreUnits() */



/* EOF */
