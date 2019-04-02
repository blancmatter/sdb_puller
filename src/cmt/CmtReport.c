/*******************************************************************************
** Module Name:
**    CmtReport.c
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
**    CMT design and implementation document v0.01 1-Nov-2000.
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**    Andrzej S. Piascik (asp)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2000
**
** Version:
**    $Id: CmtReport.c,v 1.4 2002/08/01 10:50:33 sxf Exp $
**
** History:
**    $Log: CmtReport.c,v $
**    Revision 1.4  2002/08/01 10:50:33  sxf
**    Modified so that only changed elements are reported.
**
**    Revision 1.3  2001/03/16 21:31:35  asp
**    Hardware moniroting added.
**
**    Revision 1.2  2001/01/04 11:21:46  asp
**    Port to Linux and VMS added
**
**    Revision 1.1  2000/11/14 17:30:55  asp
**    Initial revision
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


/*
** Local include files
*/

#include "Cmt.h"
#include "CmtPrivate.h"

#include "Tim.h"


/*******************************************************************************
** Function Name:
**    iCmtSubmitStatus
**
** Purpose:
**    Function to supervise submission of data into the Status Database.
**
** Description:
**    ...
**
** Return Type:
**    Status_t    
**       Returns the completion status of CMT initialisation.
**
** Arguments:
**    none
**
** Author(s):
**    Martyn J. Ford (mjf)
**    Derek J. McKay (djm)
**
*******************************************************************************/

Status_t iCmtSubmitStatus ( void )
{
   Status_t      Status;        /* Completion status of this function */
   iCmtSdbData_t SdbCopy;       /* Copy of data for SDB submission */
   int           Oid;           /* Index to Oid table. */
   int           Sdb;           /* Index to Sdb submission. */
   eTtlTime_t    CurrentTime;   /* Time at which status was gained */

   /* Initialise status and count of values to submit to SDB. */
   Status = SYS_NOMINAL;
   Sdb    = 0;

   /* 
   ** Get the time and set the timestamp for each datum
   */
   Status = eTimGetTime( &CurrentTime ); 
 
   /*
   ** Return with error if unable to determine time.
   */
   if (Status != SYS_NOMINAL )
   {
      return Status; 
   }

   /*
   ** Loop through data and submit only those that have changed.
   */
   for ( Oid = I_CMT_FIRST_DATUMID; Oid <= I_CMT_FINAL_DATUMID; Oid++ )
   {
      /*
      ** If the value in the OID table has changed then add to
      ** the list of data to be submitted to the Sdb.
      */
      if ( iCmtData.SdbDatumChanged[ Oid ] == TRUE )
      {
         /* Save new value and add to the list to be submitted to the Sdb. */
         SdbCopy.Datum[ Sdb ].SourceId          = iCmtData.CilId;
         SdbCopy.Datum[ Sdb ].DatumId           = Oid;
         SdbCopy.Datum[ Sdb ].Msrment.TimeStamp = CurrentTime;
         SdbCopy.Datum[ Sdb ].Msrment.Value     = 
                 iCmtData.SdbParams.Datum[ Oid ].Msrment.Value;
         SdbCopy.Datum[ Sdb ].Units             = 
                 iCmtData.SdbParams.Datum[ Oid ].Units;

         /* Clear the `Changed` flag now that changes have been recorded. */
         iCmtData.SdbDatumChanged[ Oid ] = FALSE;
    
         Sdb++;
      }
   }

   /*
   ** If any values have changed then submit to SDB.
   */
   if (Sdb > 0 )
   {

      SdbCopy.NumElts = Sdb;
      iCmtData.SdbSubmissionMsg.DataLen = sizeof( Int32_t) +
                                          sizeof( eSdbDatum_t ) * Sdb;

      /* Increment the sequence number for SDB submission message */
      iCmtData.SdbSubmissionMsg.SeqNum++;

      /* Point to the constructed SDB message to transmit */
      iCmtData.SdbSubmissionMsg.DataPtr = &SdbCopy;

      /* Send message - message body is byte-swapped within this function */
      Status = iCmtSendCilMsg( iCmtData.SdbSubmissionMsg.DestId,
                              &iCmtData.SdbSubmissionMsg );

   }
  
   return Status;

}  /* End of iCmtSubmitStatus() */


/*******************************************************************************
** End of File Name: CmtReport.c
*******************************************************************************/
