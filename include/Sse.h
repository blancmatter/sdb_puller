/*******************************************************************************
** Module Name:
**    Sse.h
**
** Purpose:
**    Public header file for SSE package.
**
** Description:
**    Used to declare exported functions and definitions for the
**    SSE package.
**
**    This code has been written to conform to "TTL Coding Standard" v1.0.
**
** Traceability to Software Design:
**    ...
**
** Author(s):
**    Derek J. McKay (djm)
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
** Version:
**    $Id: Sse.h,v 0.2 2001/11/16 14:36:07 djm Exp $
**
** History:
**    $Log: Sse.h,v $
**    Revision 0.2  2001/11/16 14:36:07  djm
**    Added new error code (for NULL-pointer detection).
**
**    Revision 0.1  2001/11/06 17:04:12  djm
**    Initial creation.
**
**
*******************************************************************************/


#ifndef _SSE_H_DEFINED
#define _SSE_H_DEFINED


/*
** Required include files
*/

#include "TtlSystem.h"
#include <limits.h>


/*
** Define error stati and return values
*/

enum eSseStatus_e
{
   E_SSE_GEN_ERROR = STATUS_START(SSE),     /* General error/start-of-list */
   E_SSE_INSUFFICIENT_SPACE,     /* Insufficient buffer space to write string */
   E_SSE_STRING_OVERFLOW,        /* String has overflowed the buffer space */
   E_SSE_NULL_POINTER,           /* A function argument (pointer) is NULL */

   E_SSE_STATUS_EOL,             /* End-of-List (do not use for indexing)  */
   E_SSE_STATUS_MAX = INT_MAX    /* Force the enum to be 4 bytes */
};



/*
** Exported definitions.
*/

#define E_SSE_MIN_STR_LEN  9      /* Recommended string length for decoding */



/*
** Exported function prototypes.
*/

Status_t eSseEncode
(
   char    *InputStrPtr,
   Int32_t *OutputDatumPtr
);

Status_t eSseDecode
(
   Int32_t InputDatum,
   size_t  OutputStrLen,
   char    *OutputStrPtr
);


#endif


/*******************************************************************************
** End of File Name: Sse.h
*******************************************************************************/
