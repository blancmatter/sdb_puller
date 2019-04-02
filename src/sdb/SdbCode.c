/*
** Module Name:
**    SdbCode.c
**
** Purpose:
**    A module contains functions for identifying stored data.
**
** Description:
**    The Status Database (SDB) receives data from sources around the 
**    TTL computing system. These data come with a 64-bit 
**    identification code: a 32-bit source ID (usually corresponding to
**    the CIL address of the sender) and a 32-bit Datum ID, a code that
**    starts at zero and counts up for the number of items from that 
**    source.
**
**    This 64-bit ID that uniquely identifies the data is very resource
**    intensive for the anticipated number of data from the final 
**    system. As a result, we need to reduce the 64-bit code into 
**    something smaller.
**
**    The functions within this module convert from the provided source
**    and datum IDs into a "code" of reduced size. The functions come
**    as a matched pair, to "en-code" and "de-code" the source/datum
**    pair. Note that the algorithms are completely enclosed within
**    these functions, and that the internals of the coding are not
**    known by the rest of the SDB (or any other program that calls
**    these functions. However, the exported code type eSdbCode_t may
**    be used elsewhere, and its size is defined as part of that type
**    in the SdbPrivate.h private header file.
**
**    The contents of this file have been made public for use by external
**    programs (in particular the SFR) package.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** Version:
**    $Id: SdbCode.c,v 0.6 2006/07/10 15:21:48 mjf Exp $
**
** History:
**    $Log: SdbCode.c,v $
**    Revision 0.6  2006/07/10 15:21:48  mjf
**    Code portability improved.
**
**    Revision 0.5  2000/10/04 11:05:08  djm
**    Changed from a 2-byte to 4-byte storage code.
**
**    Revision 0.4  2000/08/24 11:48:45  djm
**    Made the functions in this file global (eSdb...). They are now to be
**    made part of the Sdb.lib library for use by other packages (notably
**    the SFR package). The API of these functions has also changed to make
**    use of publically defined data structures.
**
**    Revision 0.3  2000/08/22 09:14:58  djm
**    Corrected a type error.
**
**    Revision 0.2  2000/07/21 14:17:47  djm
**    Further work to fully implement the generation of storage codes
**    and the ability to decompose them into the original source and
**    datum ID pairs.
**
**    Revision 0.1  2000/07/13 11:45:35  djm
**    Initial creation.
**
**
*/


/* Include files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TtlSystem.h"
#include "Log.h"
#include "Sdb.h"
#include "SdbPrivate.h"


/* Function prototypes */


/* Functions */



Status_t eSdbStoreIdEncode(
   eSdbSngReq_t *ReqPtr,
   eSdbCode_t *CodePtr
)
{
/*
** Function Name:
**    eSdbStoreIdEncode
**
** Type:
**    Status_t
**
** Purpose:
**    Determine the "abbreviated" code for storing data.
**
** Description:
**    This function derives a code with which to identify the data.
**    Normally, the data may be uniquely identified using the Source
**    ID and Datum ID. However, these combined take 8 bytes, and
**    thus a lot of space for recording on every occurance of the data.
**    A reduced-space code (only 4 bytes) is thus calculated in order
**    to get a 2-to-1 compression of this "look up address". The "code"
**    is only used for short-term storage, and it is thus irrelevent
**    how the code is determined, as long as it provides a unique
**    index number.
**
**    The present algorithm is merely a tighter packing of the existing
**    fields.
**
**
**       |<------ M bits ----->|
**       +-------+-------------+
**       |  M-N  |      N      |
**       +-------+-------------+
**
**    The SourceId is packed into (M-N) bits (at the most significant
**    bit end of the word. The DatumId is packed into the (N) least
**    significant bits. The number of bits, N, is defined in SdbPrivate.h
**    as the symbolic constant, I_SDB_CODE_MASKSIZE. The size of M is
**    given by the size of the word which is used for the result.
**    At the time of writing, this was set to 32 (from Int32_t), but
**    the code should be able to cope with a changed type. Note also
**    that this resultant field is presently signed, allowing for an
**    escape sequence (-1) to be easily tested, again independant of
**    the actual word size.
**
**    See also the module comment header block.
**
** Arguments:
**    eSdbSngReq_t *ReqPtr   (in)
**       Pointer to a data specification. The details within it, will be 
**       used to calculate the "reduced space" storage code, allowing
**       the identification of data.
**    eSdbCode_t *CodePtr    (out)
**       This is the variable where the reduced code is put.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    03-Oct-2000 djm Allowed for larger code size.
**    24-Aug-2000 djm Made a public function.
**    21-Aug-2000 djm Modified for globalised file storage definitions.
**    13-Jul-2000 djm Moved into the SdbCode module and renamed.
**    10-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   eSdbCode_t UpperMask;     /* Bit-mask for most-significant part of code */
   eSdbCode_t LowerMask;     /* Bit-mask for least-significant part of code */
   eSdbSngReq_t CheckReq;    /* Data specification for integrity checking */


   /* Determine the new code based on parts of the SourceID and DatumID */

   /* Create a mask for the two components */
   LowerMask = (1 << E_SDB_CODE_MASKSIZE) - 1;
   UpperMask = ~(LowerMask);

   /* Reset the code */
   *CodePtr = 0;

   /* Add in the two components in the appropriate areas of the short word */
   *CodePtr += ReqPtr->SourceId << E_SDB_CODE_MASKSIZE;
   *CodePtr += LowerMask & (ReqPtr->DatumId);

   /* Check the code generation integrity by back-tracking and comparing */
   Status = eSdbStoreIdDecode(CodePtr, &CheckReq);
   if(Status != SYS_NOMINAL)
   {
      return Status;
   }

   /* Check the decoded version against what we supplied */
   if(
      (CheckReq.SourceId != ReqPtr->SourceId) ||
      (CheckReq.DatumId != ReqPtr->DatumId)
   )
   {
      return E_SDB_ENCODE_FAILURE;
   }

   /* Return success */
   Status = SYS_NOMINAL;
   return Status;

}  /* End of eSdbStoreIdEncode() */





Status_t eSdbStoreIdDecode(
   eSdbCode_t *CodePtr,
   eSdbSngReq_t *ReqPtr
)
{
/*
** Function Name:
**    eSdbStoreIdDecode
**
** Type:
**    Status_t
**
** Purpose:
**    Determine the "abbreviated" code for storing data.
**
** Description:
**    This is the reciprocal function to eSdbStoreIdEncode. It takes
**    a specified Code (*CodePtr) and uses it to fill in the SourceId
**    and DatumId fields of a provided data specification (*ReqPtr).
**
**    See also the module comment header block.
**
** Arguments:
**    eSdbCode_t *CodePtr    (in)
**       This is a pointer to the storage "code".
**    eSdbReq_t *ReqPtr    (out)
**       Pointer to a data definition. The SourceId and DatumId fields
**       will be updated with the new code information. If the code
**       cannot be decoded into valid values, the the definition fields
**       shall remain unchanged.
**
** Authors:
**    djm: Derek J. McKay (TTL)
**
** History:
**    03-Oct-2000 djm Allowed for larger code size.
**    24-Aug-2000 djm Made a public function.
**    21-Aug-2000 djm Modified for globalised file storage definitions
**    17-Jul-2000 djm Implemented the algorithms and got it working.
**    13-Jul-2000 djm Initial creation.
**
*/

   /* Local variables */
   Status_t Status;          /* Return value from called functions */
   eSdbCode_t UpperMask;
   eSdbCode_t LowerMask;


   /* Create a mask for the two components */
   LowerMask = (1 << E_SDB_CODE_MASKSIZE) - 1;
   UpperMask = ~(LowerMask);


   /* Extract the source and datum IDs from the code */   
   ReqPtr->SourceId = (*CodePtr & UpperMask) >> E_SDB_CODE_MASKSIZE;
   ReqPtr->DatumId =  *CodePtr & LowerMask;


   /* Return success */
   Status = SYS_NOMINAL;
   return Status;



}  /* End of eSdbStoreIdDecode() */



/* EOF */
