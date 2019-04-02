/*
** Module Name:
**    SdbByteOrder.c
**
** Purpose:
**    Functions for converting between network and hardware byte orders.
**
** Description:
**    This module contains code for converting between network and
**    hardware byte orders for large messages.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbByteOrder.c,v 0.2 2000/06/23 10:31:30 djm Exp $
**
** History:
**    $Log: SdbByteOrder.c,v $
**    Revision 0.2  2000/06/23 10:31:30  djm
**    THIS MODULE IS NOW OBSOLETE.
**
**    Revision 0.1  2000/06/22 13:57:48  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "TtlSystem.h"
#include "Sdb.h"
#include "SdbPrivate.h"



/* Functions */


void iSdbConvert32bitArray(
   size_t NumBytes,
   void *Array
)
{
/*
** Function Name:
**    iSdbConvert32bitArray
**
** Type:
**    void
**
** Purpose:
**    Converts from network to hardware byte order an array of 32bit integers.
**
** Description:
**    This function converts an array of 32-bit integers from network
**    to hardware byte order, or vice-versa. Note that it just uses one
**    ntohl() call to do this, as the htonl() function is symetircal to it.
**    It doesn't make any checks on the length of the message and assumes 
**    that the array is valid.
**
** Arguments:
**    size_t NumBytes        (in)
**       The number of 8-bit bytes in the array to be converted. Note that
**       this value should always be a multiple of sizeof(Uint32_t) 
**       (= 32-bits).
**    void *Array            (in/out)
**       A pointer to the array containing the 32-bit words.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    22-Jun-2000 djm Initial creation.
**
*/

   fprintf(stderr, "Function iSdbConvert32bitArray() obsolete.\n");
   fprintf(stderr, "Use the equivalent function provided by CIL instead.\n");
   exit( EXIT_FAILURE );

   /* Local variables */
   int Byte;                 /* Loop counter for words in the array */

   /* Loop through the array, word at a time */
   for(Byte = 0; Byte < NumBytes; Byte += sizeof(Uint32_t))
   {
      /* Convert the byte order of the next 32-bits */
      *((Uint32_t *)Array) = ntohl(*((Uint32_t *)Array));

      /* Increment the array to point at the next word */
      ((Uint32_t *)Array)++;
   }

}  /* End of iSdbConvert32bitArray() */


/* EOF */