/*****************************************************************************
** Header File Name:
**     StdPrivate.h
**
** Purpose:
**     Sdb Testdump Device (STD). A utility used to search and extract data
**     points from SDB files within a given time range.     
**
** Description:
**      
**    This module contains definitions and function headers local to 
**    the Std process.
**
** Authors:
**    Martin Norbury (man).
**
** Copyright (c) Telescope Technologies Limited (TTL), 2001
**
**
** Version:
**    $Id: StdPrivate.h,v 0.27 2006/10/03 12:39:04 sxf Exp $
**    
** History:   
**    $Log: StdPrivate.h,v $
**    Revision 0.27  2006/10/03 12:39:04  sxf
**    Build against static libraries.
**
**    Revision 0.26  2006/09/28 12:23:04  sxf
**    Built on QNX6.3.
**
**    Revision 0.25  2006/07/12 10:57:34  mjf
**    STD_1_11.
**
**    Revision 0.24  2006/06/26 11:23:45  mjf
**    STD_1_10.
**
**    Revision 0.23  2005/08/31 14:18:08  sxf
**    Updated for baseline STD_1_09.
**
**    Revision 0.22  2005/08/01 14:48:47  mjf
**    STD_1_08.
**
**    Revision 0.21  2005/06/27 15:47:49  mjf
**    STD_1_07.
**
**    Revision 0.20  2005/06/21 13:50:39  mjf
**    Addition of a command-line switch to specify a 'stride' in seconds to
**    ensure extracted measurements are a specified distance apart in time.
**
**    Revision 0.19  2005/06/20 13:38:44  mjf
**    Tidied up to aid porting to Linux - no functional changes.
**
**    Revision 0.18  2005/06/17 10:08:50  mjf
**    STD_1_04 - incorporate functional changes made in STD_0_13 -> STD_0_15.
**
**    Revision 0.17  2004/10/18 15:39:28  mkb
**    Reset FirstTime flag within public library when end of
**    search has been reached to allow clients to start a new
**    search.
**
**    Revision 0.16  2004/10/15 13:48:24  mkb
**      Fix problems that make STD enter infinte loops during some
**       searches.
**       Put search results into linked list so they can be sorted
**       chronologically because the SBD does not guarantee the
**       order of records in *.sdb files.
**
**    Revision 0.15  2004/04/21 09:21:53  sxf
**    Updated for new baseline.
**
**    Revision 0.14  2004/04/16 13:21:59  sxf
**    Updated for new baseline.
**
**    Revision 0.13  2004/04/02 15:25:21  mjf
**    No functional changes, ported to QNX v6 by SXF.
**
**    Revision 0.12  2004/02/24 08:41:28  mjf
**    Don't attempt to create a log file by default.
**
**    Revision 0.11  2003/09/15 15:42:33  man
**    Corrected error in default sdb directory macro.
**
**    Revision 0.10  2003/09/15 14:35:18  man
**    Included a command line switch in which an alternate
**    path for the sdb data can be specified.
**
**    Revision 0.9  2003/09/05 11:29:30  sxf
**    Use correct path for Std.h
**
**    Revision 0.8  2003/08/15 13:37:10  man
**    Updated for baseline STD_0_06.
**
**    Revision 0.7  2003/01/31 08:07:49  man
**    Removed IdTable path from command line.
**
**    Revision 0.6  2003/01/15 16:20:52  man
**    Updated Std to use Std library functions.
**
**    Revision 0.5  2002/10/03 14:41:09  man
**    Changed default syslogging to FALSE.
**
**    Revision 0.4  2002/10/03 13:36:20  man
**    Removed some command line flags and updated
**    version number.
**
**    Revision 0.3  2002/08/14 12:02:57  man
**    Removed unwanted include file.
**
**    Revision 0.2  2002/08/14 12:00:07  man
**    Included error messages from Std.h in this header
**    file. Since this is a utility, only use private
**    header files.
**
**    Revision 0.1  2002/07/08 10:18:16  man
**    Initial revision.
**
**
**
**
*****************************************************************************/

#ifndef STD_PRIVATE_H_DEFINED
#define STD_PRIVATE_H_DEFINED
 
#include "Std.h"
#include "Wfl.h"

/* Application details - normally found in StdPrivate.h */

#define I_STD_PROGRAM_NAME   "Std"
#define I_STD_PROGRAM_ABOUT  "Sdb Test Dump utility"
#define I_STD_RELEASE_DATE   "3 October 2006"
#define I_STD_YEAR           "2003-06"
#define I_STD_MAJOR_VERSION  1
#define I_STD_MINOR_VERSION  13

/* Common arguments defaults */

#define E_STD_DFLT_QUIET     FALSE
#define E_STD_DFLT_VERBOSE   TRUE
#define E_STD_DFLT_SYSLOG    FALSE
#define E_STD_DFLT_DEBUG     E_LOG_NOTICE
#define E_STD_DFLT_PRIORITY  9
#define E_STD_DFLT_HELP      FALSE
#ifdef E_WFL_OS_QNX4
#define E_STD_DFLT_CONFIG    "/opt/ttl/etc/Std.cfg"
#define E_STD_DFLT_CIL_MAP   "/opt/ttl/etc/Cil.map"
#define E_STD_DFLT_ID_PATH   "/opt/ttl/etc" /* Default path for ID tables */
#define E_STD_DFLT_OUTFILE   "/opt/ttl/data/Std.dat"
#define I_STD_DFLT_PATH      "/opt/ttl/data/"
#else
#define E_STD_DFLT_CONFIG    "/ttl/sw/etc/Std.cfg"
#define E_STD_DFLT_CIL_MAP   "/ttl/sw/etc/Cil.map"
#define E_STD_DFLT_ID_PATH   "/ttl/sw/etc" /* Default path for ID tables */
#define E_STD_DFLT_OUTFILE   "/ttl/sw/data/Std.dat"
#define I_STD_DFLT_PATH      "/ttl/sw/data/"
#endif
#define E_STD_DFLT_LOG       ""
#define E_STD_DFLT_CIL       "TU0"

/* custom arguments - note that these must match the order in eCluCustomArg[] */

#define E_STD_CUSTOM_IDPATH  0

#define I_STD_CUSTOM_ARGS    4

/* Definitions */
#define E_STD_IDTABLE_STR    "%s %s \"%[^\"]\"" /* Format of ID table entry */
#define E_STD_IDTABLE_PARAMS 3        /* Number of params in ID table entry */
#define E_STD_ID_FILE        "IdTable.idx"    /* ID table file */
#define E_STD_UNIT_FILE      "UnitsTable.idx" /* Units table file */
#define E_STD_HOURS_IN_DAY   23
#define E_STD_DAYS_IN_MONTH  31
#define E_STD_MONTHS_IN_YEAR 12
#define I_STD_EXT_SDB        "sdb"
#define I_STD_EXT_GZIP       ".gz"

#define I_STD_SWITCH_PATH    "path <path>"
#define I_STD_SWITCH_STRIDE  "stride [secs]"
#define I_STD_SWITCH_MLB     "matlab"
#define I_STD_SWITCH_GPT     "gnuplot"

#define I_STD_EXPL_PATH      "Data directory"
#define I_STD_EXPL_STRIDE    "Gap between extracted measurements"
#define I_STD_EXPL_MLB       "Write file suitable for matlab"
#define I_STD_EXPL_GPT       "Write file suitable for gnuplot"
#define I_STD_MAX_PATH_LEN   100
#define I_STD_DFLT_STRIDE    0
#define I_STD_DFLT_MLB       FALSE
#define I_STD_DFLT_GPT       FALSE

enum iStdCustomArg_e
{
   I_STD_ARG_PATH,
   I_STD_ARG_STRIDE,
   I_STD_ARG_MLB,
   I_STD_ARG_GPT
};

/* Structude definition */
typedef struct iStdData_s
{
   Bool_t       DataFlag;
   Uint32_t     SourceId;
   char         SourceName[E_STD_MAX_STRING_LEN];
   Uint32_t     DatumId;
   char         DatumName [E_STD_MAX_STRING_LEN];
   char         DatumUnits[E_STD_MAX_STRING_LEN];
   Uint32_t     NumOfPoints;
   Int32_t      Value;
   eTtlTime_t   NextStrideTime;
} iStdData_t;

typedef struct iStdGlobVar_s
{
   Int32_t  NumDataSearch;    /* Number of datum id's to search for */
   char IdPath[ E_STD_STRING_LEN ]; /* Path for ID lookup tables */
   char InDir  [ E_STD_MAX_STRING_LEN ];
   char OutFile[ E_STD_MAX_STRING_LEN ];
   char         TimeStr[E_TIM_BUFFER_LENGTH];
   char         LastTimeStr[E_TIM_BUFFER_LENGTH];
   iStdData_t  StdData[E_STD_MAX_SEARCH_DATA];
   Bool_t GotConfig;
   Bool_t DisplayNames; /* Use look-up table to display identifier names */
   Bool_t LoadFile;     /* Flag to show if a file has been supplied */
   eStdTime_t StartTime;
   eStdTime_t StopTime;
   char DatPath[ I_STD_MAX_PATH_LEN ];
   Int32_t    Stride;
   Bool_t WriteMatlab;
   Bool_t WriteGnuplot;
} iStdGlobVar_t;



#ifdef E_STD_MAIN_C
#define E_STD_EXTERN
#else
#define E_STD_EXTERN extern
#endif

/* Structure to store a time point with matching data. */
struct iStdMatchedData_s
{
  eTtlTime_t                 TimeStamp;
  Int32_t                    Value[E_STD_MAX_SEARCH_DATA];
  Bool_t                     SetFlag[E_STD_MAX_SEARCH_DATA];
  struct iStdMatchedData_s  *PreviousPtr;
  struct iStdMatchedData_s  *NextPtr;
}; 
typedef struct iStdMatchedData_s   iStdMatchedData_t;
E_STD_EXTERN   iStdMatchedData_t  *iStdLastMatchedPtr;
E_STD_EXTERN   Int32_t             iStdLinesOfData;   /* Number of lines of data */



/* for parsing custom command-line arguments */

#ifdef E_STD_MAIN_C
eCluArgSpec_t eCluCustomArg[ I_STD_CUSTOM_ARGS + 1 ] = 
/*  arg [param],    match, description,                       FALSE, NULL */ 
{ 
  { I_STD_SWITCH_PATH, 1, I_STD_EXPL_PATH,                    FALSE, NULL },
  { I_STD_SWITCH_STRIDE,1, I_STD_EXPL_STRIDE,                 FALSE, NULL },
  { I_STD_SWITCH_MLB,  1, I_STD_EXPL_MLB,                     FALSE, NULL },
  { I_STD_SWITCH_GPT,  1, I_STD_EXPL_GPT,                     FALSE, NULL },
  { E_CLU_EOL,         0, E_CLU_EOL,                          FALSE, NULL }
};
#else
extern eCluArgSpec_t eCluCustomArg[ I_STD_CUSTOM_ARGS + 1];
#endif

/* Declare the global variables */
E_STD_EXTERN iStdGlobVar_t iStdGlobVar;

/* Local function prototypes */
Status_t iStdCluInit ( int argc, char *argv[]);
Status_t iStdReadConfig ( char *ConfigFilePtr );
Status_t iStdReadSdbChunk ( FILE *, eSdbRawFmt_t *, size_t, size_t *);
Status_t iStdSearchChunk ( eSdbRawFmt_t, eSdbSngReq_t, eTtlTime_t, Bool_t *, Bool_t * );
Status_t iStdWriteHeader ( FILE *OutFilePtr );
Status_t iStdReadSdbHeader ( FILE *InFilePtr, size_t *NumBytes );
Status_t iStdReadSdbTimeStamp ( FILE *InFilePtr, eTtlTime_t *TimeHour);
Status_t iStdWriteToScreen ( void );
Status_t iStdWriteToFile ( FILE *OutFilePtr );
Status_t iStdSrcDtmName ( void );
Status_t iStdAdvanceTime ( eStdTime_t *, Bool_t * );
Status_t iStdStartStopTime( eTtlTime_t *TtlStartPtr, eTtlTime_t *TtlStopPtr );
Status_t iStdCompareTime ( eTtlTime_t LastTime, eTtlTime_t ThisTime, Bool_t *NewTime);
Status_t iStdStoreData(eTtlTime_t TimeStamp, Int32_t DataItem, Int32_t Value);
Status_t iStdAddMatch(eTtlTime_t TimeStamp, Int32_t DataItem, Int32_t Value, iStdMatchedData_t *PreviousDataPtr);
Status_t iStdAddNew  (eTtlTime_t TimeStamp, Int32_t DataItem, Int32_t Value, iStdMatchedData_t *PreviousDataPtr);
Status_t iStdFindTimeMatch( eTtlTime_t TimeStamp, iStdMatchedData_t **DataPreviousPtr); 
Status_t iStdCompareTime  ( eTtlTime_t LastTime, eTtlTime_t ThisTime, Bool_t *NewTime);
Status_t iStdCompareTimeString ( char *pLstTimeStr,char *pTimeStr, Bool_t *pNewTime, Bool_t GotData );
Status_t iStdGetTimeStamp ( eSdbRawFmt_t , eTtlTime_t , Bool_t *, Bool_t);


#endif
