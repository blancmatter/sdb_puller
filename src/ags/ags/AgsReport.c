/*******************************************************************************
** Module Name:
**    AgsReport.c
**
** Purpose:
**    Functions to report status data.
**
** Description:
**    This source file contains functions to report the status data obtained by
**    this unit. This is reported both to the SDB and sent via a CIL message in
**    response to a request for the data.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    AGS design and implementation document v0.01.
**
** Author(s):
**    Stephen Foale (sxf)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: AgsReport.c,v 0.6 2002/02/05 08:23:54 sxf Exp $
**
** History:
**    $Log: AgsReport.c,v $
**    Revision 0.6  2002/02/05 08:23:54  sxf
**    Submit final datum id.
**
**    Revision 0.5  2002/01/23 14:11:19  sxf
**    Initialise Status for iAgsSubmitStatus function.
**
**    Revision 0.4  2002/01/21 09:19:49  sxf
**    Do not report processes own process state to the SDB.
**
**    Revision 0.3  2001/11/09 10:51:29  sxf
**    Update reporting to SDB with correct units.
**
**    Revision 0.2  2001/09/25 14:45:31  sxf
**    Initial version with basic functionality.
**
**    Revision 0.1  2001/09/18 14:13:25  sxf
**    Initial version.
**
**
*******************************************************************************/


/*
** Compiler include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
** System include files
*/
#include "Tim.h"
#include "Log.h"


/*
** Local include files
*/
#include "AgsPrivate.h"


/*******************************************************************************
** Function Name:
**    iAgsSubmitStatus
**
** Purpose:
**    Function to submit data into the Status Database.
**
** Description:
**    This function scans through the data table iAgsDataTable to establish
**    a list the Oid states and analogue values.
**    The Oid table iAgsOidTable is then scanned and any changes are reported
**    to the Sdb.   
**
** Return Type:
**    Status_t    
**       Returns the completion status of AGS initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Stephen Foale (sxf)
**    Lorna J. Slater (ljs)
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
*******************************************************************************/

Status_t iAgsSubmitStatus ( void )
{
   Status_t      Status = SYS_NOMINAL;
                            /* Completion status of this function */
   iAgsSdbData_t SdbCopy;   /* Copy of data for SDB submission */
   int           Oid;       /* Index to Oid table */
   int           Sdb;       /* Index to Sdb submission */

   /* Initialise count of the values to be submitted to SDB. */
   Sdb = 0;

   /* 
   ** For every Oid Check the stored value against the latest value and
   ** submit to the Sdb if it has changed.
   */

   for ( Oid = I_AGS_FIRST_DATUMID; Oid <= I_AGS_FINAL_DATUMID; Oid++ )
   {
      /*
      ** If the value in the OID table has changed then add to
      ** the list of data to be submitted to the Sdb.
      */
      if ( iAgsOidTable[ Oid ].Changed == TRUE )
      {
         /* Save new value and add to the list to be submitted to the Sdb. */
         SdbCopy.Datum[ Sdb ].SourceId          = iAgsData.CilId;
         SdbCopy.Datum[ Sdb ].DatumId           = iAgsOidTable[ Oid ].Oid;
         SdbCopy.Datum[ Sdb ].Msrment.TimeStamp = iAgsOidTable[ Oid ].TimeStamp;
         SdbCopy.Datum[ Sdb ].Msrment.Value     = iAgsOidTable[ Oid ].Value;
         SdbCopy.Datum[ Sdb ].Units             = iAgsOidTable[ Oid ].Units;

         /* Clear the `Changed` flag now that changes have been recorded. */
         iAgsOidTable[ Oid ] .Changed = FALSE;
    
         Sdb++;
      }
   }

   /* If any values have changed then submit to Sdb. */
   if ( Sdb > 0 )
   {
      SdbCopy.NumElts = Sdb;
      iAgsData.SdbSubmissionMsg.DataLen = sizeof( Int32_t) +
                                          sizeof( eSdbDatum_t ) * Sdb;

      /* Increment the sequence number for SDB submission message */
      iAgsData.SdbSubmissionMsg.SeqNum++;

      /* Point to the constructed SDB message to transmit */
      iAgsData.SdbSubmissionMsg.DataPtr = &SdbCopy;

      /* Send message - message body is byte-swapped within this function */
      Status = iAgsSendCilMsg( iAgsData.SdbSubmissionMsg.DestId,
                              &iAgsData.SdbSubmissionMsg );
   }

   return Status;

}  /* End of iAgsSubmitStatus() */

/*******************************************************************************
** End of File Name: AgsReport.c
*******************************************************************************/
