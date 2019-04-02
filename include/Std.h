/*****************************************************************************
** Header File Name:
**     Std.h
**
** Purpose:
**     Sdb Testdump Device (STD). A utility used to search and extract data
**     points from SDB files within a given time range.     
**
** Description:
**      
**    Public header file for the STD.
**
** Authors:
**    Martin Norbury (man).
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
**
** Version:
**    $Id: Std.h,v 0.8 2004/10/15 13:48:24 mkb Exp $
**    
** History:   
**    $Log: Std.h,v $
**    Revision 0.8  2004/10/15 13:48:24  mkb
**      Fix problems that make STD enter infinte loops during some
**       searches.
**       Put search results into linked list so they can be sorted
**       chronologically because the SBD does not guarantee the
**       order of records in *.sdb files.
**
**    Revision 0.7  2004/04/16 13:13:13  sxf
**    Correct default path for QNX6.
**
**    Revision 0.6  2003/09/15 14:35:18  man
**    Included a command line switch in which an alternate
**    path for the sdb data can be specified.
**
**    Revision 0.5  2003/08/27 14:34:19  sxf
**    Converted to QNX format.
**
**    Revision 0.4  2003/08/15 13:37:30  man
**    Increased maximum number of data items that can be retrieved to 100.
**
**    Revision 0.3  2003/01/15 16:21:42  man
**    *** empty log message ***
**
**    Revision 0.2  2003/01/15 16:20:52  man
**    Updated Std to use Std library functions.
**
**    Revision 0.1  2002/07/10 10:34:18  man
**    Initial revision.
**
**
**
*****************************************************************************/

#ifndef STD_H_DEFINED
#define STD_H_DEFINED

/* Compiler Include Files */

/* System Include Files */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include "TtlSystem.h"
#include "TtlConstants.h"
#include "Wfl.h"
#include "Sdb.h"
#include "Cil.h"
#include "Tim.h"
#include "Clu.h"
#include "Log.h"
#include "Hti.h"
#include "Cfu.h"

/* Definitions */
#define E_STD_FILE_HDR_SIZE     4    /* Expected size of a storage file header */
#define E_STD_MAX_SEARCH_DATA   100  /* Maximum number of datum id's to search for */
#define E_STD_SDB_CHUNK_SIZE    400  /* Size of the array to store sdb data in*/
#define E_STD_SECONDS_PER_HOUR  3600 /* Seconds per hour */
#define E_STD_MAX_STRING_LEN    128  /* Maximum string length for TLA's and UNITS */
#define E_STD_BUFSIZE           16   /* Size of temporary storage buffer */
#define E_STD_STRING_LEN        128  /* Maximum size of a string */
#ifdef E_WFL_OS_QNX4
#define E_STD_DFLT_SDB_PATH     "/opt/ttl/data/" /* Default path for sdb files*/
#else
#define E_STD_DFLT_SDB_PATH     "/ttl/sw/data/"  /* Default path for sdb files*/
#endif



/* Enumerated list of package error codes */
enum eStdStatus_e
{
   E_STD_GEN_ERROR = STATUS_START(STD),
   E_STD_NULL_FILE_POINTER, /* File pointer is NULL */
   E_STD_MEM_ALLOC_ERR,     /* Memory allocation error */
   E_STD_FILE_WRITE_ERR,    /* Error writing to output file */
   E_STD_READ_HEAD_ERR,     /* Error reading Sdb header info */ 
   E_STD_READ_TIME_ERR,     /* Error reading Sdb timestamp */
   E_STD_FILE_OPEN_ERR,     /* Error opening file */
   E_STD_TIME_OUT_RANGE,    /* Attempting to open sdb file in the future */
   E_STD_SRCID_ERR,         /* Error setting the source ID */
   E_STD_DATID_ERR,         /* Error setting the datum ID */
   E_STD_ARRAY_SIZE,        /* Error with array size */
   E_STD_STATUS_EOL,
   E_STD_STATUS_MAX, 
   E_STD_MATCHFOUND,        /* Timestamp matches an existing data item. */
   E_STD_NOMATCH,           /* Data item with new timestamp. */
   E_STD_DUPLICATE_DATA,    /* Data item and time already found in sdb files. */
   E_STD_EOF                /* End of file. */
}; 

typedef struct eStdTime_s
{
   Uint32_t    Year;
   Uint32_t    Month;
   Uint32_t    Date;
   Uint32_t    Hour;
   Uint32_t    Minute;
   Uint32_t    Second;
   Uint32_t    MilliSecond;
} eStdTime_t;

/* Function prototypes */
Status_t eStdRetrieveData( eTtlTime_t StartTime, 
                           eTtlTime_t StopTime,
                           char *PathPtr, 
                           eSdbRawFmt_t **SdbDataPtr, 
                           Bool_t *Finished,
                           size_t *NumRecordsPtr );
Status_t eStdSearchData( Int32_t CilId, 
                         Int32_t DatId, 
                         eSdbRawFmt_t SdbLine,                         
                         eTtlTime_t *TimeStampPtr,
                         Int32_t *Value,
                         Bool_t *GotData );

#endif
